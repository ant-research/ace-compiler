"""
Type Inference Pass
===================

Implements type inference with constant propagation for PyACE.
Based on CuTile's type inference approach.
"""

from dataclasses import dataclass, field
from typing import Dict, Any, Optional, List, Tuple, Set
from enum import Enum

from base_dsl.python_ir import (
    Function, Block, Operation, Var,
    Load, Store, Const, BinOp, UnaryOp, Call, Return, ForLoop, If
)
from base_dsl.loc import Loc


class ConstantState(Enum):
    """State of a variable's constant-ness."""
    UNSET = 0
    MAY_BE_CONSTANT = 1
    NONCONSTANT = 2


@dataclass
class PhiState:
    """Tracks constant state across control flow branches."""
    constant_state: ConstantState = ConstantState.UNSET
    constant_value: Any = None

    def set_nonconstant(self):
        self.constant_state = ConstantState.NONCONSTANT

    def set_branch_constant(self, value: Any):
        if self.constant_state == ConstantState.UNSET:
            self.constant_state = ConstantState.MAY_BE_CONSTANT
            self.constant_value = value
        elif self.constant_state == ConstantState.MAY_BE_CONSTANT and value != self.constant_value:
            self.constant_state = ConstantState.NONCONSTANT


@dataclass
class TensorType:
    """Type representing a tensor with shape and dtype."""
    shape: Tuple[int, ...]
    dtype: str = "f32"
    
    def __str__(self):
        return f"Tensor[{', '.join(map(str, self.shape))}]<{self.dtype}>"


@dataclass
class ScalarType:
    """Type representing a scalar value."""
    dtype: str
    
    def __str__(self):
        return self.dtype


@dataclass 
class UnknownType:
    """Unknown type placeholder."""
    def __str__(self):
        return "unknown"


# Type alias
Type = TensorType | ScalarType | UnknownType


class TypingContext:
    """
    Context for type inference.
    
    Tracks types and constant values for all variables.
    """
    
    def __init__(self):
        self.typemap: Dict[str, Type] = {}
        self.constants: Dict[str, Any] = {}
        self.phis: Dict[str, PhiState] = {}
    
    def get_type(self, var: Var) -> Type:
        """Get the type of a variable."""
        if var.name in self.typemap:
            return self.typemap[var.name]
        return UnknownType()
    
    def set_type(self, var: Var, typ: Type) -> None:
        """Set the type of a variable."""
        self.typemap[var.name] = typ
    
    def get_constant(self, var: Var) -> Any:
        """Get the constant value of a variable."""
        if var.name in self.constants:
            return self.constants[var.name]
        raise KeyError(f"Variable {var.name} is not a constant")
    
    def try_get_constant(self, var: Var) -> Optional[Any]:
        """Try to get the constant value, return None if not constant."""
        return self.constants.get(var.name)
    
    def is_constant(self, var: Var) -> bool:
        """Check if a variable is a constant."""
        return var.name in self.constants
    
    def set_constant(self, var: Var, value: Any):
        """Set a variable as a constant with given value."""
        self.constants[var.name] = value


class TypeInferencePass:
    """
    Type inference pass that propagates types through the IR.
    
    Features:
    - Infers tensor shapes from operations
    - Tracks constant values for constant propagation
    - Handles control flow (loops, if/else)
    """
    
    # Binary operation result type rules
    BINOP_RESULT_TYPES = {
        'add': lambda a, b: _broadcast_type(a, b),
        'sub': lambda a, b: _broadcast_type(a, b),
        'mul': lambda a, b: _broadcast_type(a, b),
        'truediv': lambda a, b: _broadcast_type(a, b),
        'floordiv': lambda a, b: _broadcast_type(a, b),
        'matmul': lambda a, b: _matmul_type(a, b),
        'eq': lambda a, b: _comparison_type(a, b),
        'ne': lambda a, b: _comparison_type(a, b),
        'lt': lambda a, b: _comparison_type(a, b),
        'le': lambda a, b: _comparison_type(a, b),
        'gt': lambda a, b: _comparison_type(a, b),
        'ge': lambda a, b: _comparison_type(a, b),
    }
    
    def __init__(self, func: Function, annotations: Dict[str, Any] = None):
        self.func = func
        self.annotations = annotations or {}
        self.ctx = TypingContext()
    
    def run(self) -> TypingContext:
        """Run type inference on the function."""
        # Initialize parameter types from annotations
        for param in self.func.parameters:
            if param.name in self.annotations:
                param_type = self._annotation_to_type(self.annotations[param.name])
                self.ctx.set_type(param, param_type)
            else:
                # Default to unknown
                self.ctx.set_type(param, UnknownType())
        
        # Infer types in the function body
        self._infer_block(self.func.root_block)
        
        return self.ctx
    
    def _infer_block(self, block: Block):
        """Infer types for all operations in a block."""
        for op in block.operations:
            self._infer_operation(op)
    
    def _infer_operation(self, op: Operation):
        """Infer types for a single operation."""
        op_type = type(op).__name__
        
        if op_type == 'Const':
            self._infer_const(op)
        elif op_type == 'Load':
            self._infer_load(op)
        elif op_type == 'Store':
            self._infer_store(op)
        elif op_type == 'BinOp':
            self._infer_binop(op)
        elif op_type == 'UnaryOp':
            self._infer_unaryop(op)
        elif op_type == 'Call':
            self._infer_call(op)
        elif op_type == 'Return':
            pass  # Return doesn't create new variables
        elif op_type == 'ForLoop':
            self._infer_forloop(op)
        elif op_type == 'If':
            self._infer_if(op)
    
    def _infer_const(self, op: Const):
        """Infer type for constant operation."""
        if op.result_vars:
            result = op.result_vars[0]
            value = op.value
            
            # Set constant value
            self.ctx.set_constant(result, value)
            
            # Infer type from value
            if isinstance(value, bool):
                self.ctx.set_type(result, ScalarType("bool"))
            elif isinstance(value, int):
                self.ctx.set_type(result, ScalarType("i64"))
            elif isinstance(value, float):
                self.ctx.set_type(result, ScalarType("f64"))
            else:
                self.ctx.set_type(result, UnknownType())
    
    def _infer_load(self, op: Load):
        """Infer type for load operation."""
        if op.result_vars:
            result = op.result_vars[0]
            var_name = op.var_name
            
            # Look up type of loaded variable
            # Check if it's in the context
            if var_name in self.ctx.typemap:
                self.ctx.set_type(result, self.ctx.typemap[var_name])
            elif var_name in self.annotations:
                self.ctx.set_type(result, self._annotation_to_type(self.annotations[var_name]))
            else:
                self.ctx.set_type(result, UnknownType())
            
            # Propagate constant
            if var_name in self.ctx.constants:
                self.ctx.set_constant(result, self.ctx.constants[var_name])
    
    def _infer_store(self, op: Store):
        """Infer type for store operation (updates variable type)."""
        var_name = op.var_name
        value_var = op.operands.get('value')
        
        if value_var and value_var.name in self.ctx.typemap:
            self.ctx.typemap[var_name] = self.ctx.typemap[value_var.name]
        
        # Propagate constant
        if value_var and value_var.name in self.ctx.constants:
            self.ctx.constants[var_name] = self.ctx.constants[value_var.name]
    
    def _infer_binop(self, op: BinOp):
        """Infer type for binary operation."""
        if not op.result_vars:
            return
        
        result = op.result_vars[0]
        lhs = op.operands.get('lhs')
        rhs = op.operands.get('rhs')
        
        lhs_type = self.ctx.get_type(lhs) if lhs else UnknownType()
        rhs_type = self.ctx.get_type(rhs) if rhs else UnknownType()
        
        # Get result type from rule
        type_rule = self.BINOP_RESULT_TYPES.get(op.op_name)
        if type_rule:
            result_type = type_rule(lhs_type, rhs_type)
        else:
            result_type = _broadcast_type(lhs_type, rhs_type)
        
        self.ctx.set_type(result, result_type)
        
        # Constant folding
        if lhs and rhs and self.ctx.is_constant(lhs) and self.ctx.is_constant(rhs):
            lhs_val = self.ctx.get_constant(lhs)
            rhs_val = self.ctx.get_constant(rhs)
            try:
                result_val = _fold_binop(op.op_name, lhs_val, rhs_val)
                if result_val is not None:
                    self.ctx.set_constant(result, result_val)
            except:
                pass
    
    def _infer_unaryop(self, op: UnaryOp):
        """Infer type for unary operation."""
        if not op.result_vars:
            return
        
        result = op.result_vars[0]
        operand = op.operands.get('operand')
        
        if operand:
            operand_type = self.ctx.get_type(operand)
            self.ctx.set_type(result, operand_type)
            
            # Constant folding
            if self.ctx.is_constant(operand):
                val = self.ctx.get_constant(operand)
                try:
                    result_val = _fold_unaryop(op.op_name, val)
                    if result_val is not None:
                        self.ctx.set_constant(result, result_val)
                except:
                    pass
    
    def _infer_call(self, op: Call):
        """Infer type for function call."""
        if not op.result_vars:
            return
        
        result = op.result_vars[0]
        callee = op.operands.get('callee')
        args = op.operands.get('args', ())
        
        # Get callee name
        callee_name = None
        if isinstance(callee, str):
            callee_name = callee
        elif hasattr(callee, 'name') and callee.name in self.ctx.typemap:
            # It might be a Load for function name
            pass
        
        # For now, propagate first arg type for most operations
        if args:
            first_arg_type = self.ctx.get_type(args[0])
            self.ctx.set_type(result, first_arg_type)
        else:
            self.ctx.set_type(result, UnknownType())
    
    def _infer_forloop(self, op: ForLoop):
        """Infer types for for loop."""
        # Set loop variable type (usually int)
        if op.loop_var:
            # Create a dummy Var for the loop variable
            loop_var = Var(op.loop_var, Loc.unknown())
            self.ctx.set_type(loop_var, ScalarType("i64"))
            self.ctx.typemap[op.loop_var] = ScalarType("i64")
        
        # Process body
        if op.nested_blocks:
            self._infer_block(op.nested_blocks[0])
    
    def _infer_if(self, op: If):
        """Infer types for if statement."""
        # Process then block
        if op.nested_blocks:
            self._infer_block(op.nested_blocks[0])
            
            # Process else block if present
            if len(op.nested_blocks) > 1:
                self._infer_block(op.nested_blocks[1])
    
    def _annotation_to_type(self, annotation: Any) -> Type:
        """Convert a type annotation to a Type object."""
        # Handle Tensor[shape] annotations
        if hasattr(annotation, 'shape'):
            return TensorType(annotation.shape, "f32")
        if hasattr(annotation, '__origin__'):
            # Handle typing generics
            return UnknownType()
        return UnknownType()


# ═══════════════════════════════════════════════════════════════════════════════
# Type computation helpers
# ═══════════════════════════════════════════════════════════════════════════════

def _broadcast_type(a: Type, b: Type) -> Type:
    """Compute broadcast result type."""
    if isinstance(a, TensorType) and isinstance(b, TensorType):
        # Broadcasting rules
        result_shape = _broadcast_shapes(a.shape, b.shape)
        result_dtype = _promote_dtype(a.dtype, b.dtype)
        return TensorType(result_shape, result_dtype)
    elif isinstance(a, TensorType):
        return a
    elif isinstance(b, TensorType):
        return b
    elif isinstance(a, ScalarType) and isinstance(b, ScalarType):
        return ScalarType(_promote_dtype(a.dtype, b.dtype))
    return UnknownType()


def _broadcast_shapes(a: Tuple[int, ...], b: Tuple[int, ...]) -> Tuple[int, ...]:
    """Compute broadcast shape."""
    result = []
    for dim_a, dim_b in zip(reversed(a), reversed(b)):
        if dim_a == 1:
            result.append(dim_b)
        elif dim_b == 1:
            result.append(dim_a)
        elif dim_a == dim_b:
            result.append(dim_a)
        else:
            raise ValueError(f"Cannot broadcast shapes {a} and {b}")
    
    # Handle different lengths
    if len(a) > len(b):
        result.extend(reversed(a[:-len(b)]))
    elif len(b) > len(a):
        result.extend(reversed(b[:-len(a)]))
    
    return tuple(reversed(result))


def _matmul_type(a: Type, b: Type) -> Type:
    """Compute matmul result type."""
    if isinstance(a, TensorType) and isinstance(b, TensorType):
        # Matrix multiplication: [..., m, k] @ [..., k, n] -> [..., m, n]
        if len(a.shape) >= 2 and len(b.shape) >= 2:
            result_shape = list(a.shape[:-2]) + [a.shape[-2], b.shape[-1]]
            return TensorType(tuple(result_shape), _promote_dtype(a.dtype, b.dtype))
    return UnknownType()


def _comparison_type(a: Type, b: Type) -> Type:
    """Compute comparison result type (bool tensor)."""
    if isinstance(a, TensorType) and isinstance(b, TensorType):
        result_shape = _broadcast_shapes(a.shape, b.shape)
        return TensorType(result_shape, "bool")
    elif isinstance(a, TensorType):
        return TensorType(a.shape, "bool")
    elif isinstance(b, TensorType):
        return TensorType(b.shape, "bool")
    return ScalarType("bool")


def _promote_dtype(a: str, b: str) -> str:
    """Promote two dtypes to a common type."""
    dtype_order = ["bool", "i8", "i16", "i32", "i64", "f16", "f32", "f64"]
    try:
        idx_a = dtype_order.index(a) if a in dtype_order else -1
        idx_b = dtype_order.index(b) if b in dtype_order else -1
        return dtype_order[max(idx_a, idx_b)] if max(idx_a, idx_b) >= 0 else "f32"
    except:
        return "f32"


# ═══════════════════════════════════════════════════════════════════════════════
# Constant folding helpers
# ═══════════════════════════════════════════════════════════════════════════════

def _fold_binop(op_name: str, a: Any, b: Any) -> Optional[Any]:
    """Fold a binary operation on constants."""
    ops = {
        'add': lambda x, y: x + y,
        'sub': lambda x, y: x - y,
        'mul': lambda x, y: x * y,
        'truediv': lambda x, y: x / y,
        'floordiv': lambda x, y: x // y,
        'mod': lambda x, y: x % y,
        'pow': lambda x, y: x ** y,
        'eq': lambda x, y: x == y,
        'ne': lambda x, y: x != y,
        'lt': lambda x, y: x < y,
        'le': lambda x, y: x <= y,
        'gt': lambda x, y: x > y,
        'ge': lambda x, y: x >= y,
    }
    if op_name in ops:
        return ops[op_name](a, b)
    return None


def _fold_unaryop(op_name: str, a: Any) -> Optional[Any]:
    """Fold a unary operation on a constant."""
    ops = {
        'neg': lambda x: -x,
        'pos': lambda x: +x,
        'not': lambda x: not x,
        'invert': lambda x: ~x,
    }
    if op_name in ops:
        return ops[op_name](a)
    return None


# ═══════════════════════════════════════════════════════════════════════════════
# Public API
# ═══════════════════════════════════════════════════════════════════════════════

def infer_types(func: Function, annotations: Dict[str, Any] = None) -> TypingContext:
    """
    Run type inference on a Python IR function.
    
    Args:
        func: The Python IR Function to analyze
        annotations: Optional type annotations for parameters
        
    Returns:
        TypingContext with inferred types and constants
    """
    pass_ = TypeInferencePass(func, annotations)
    return pass_.run()

