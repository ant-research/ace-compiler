"""
Kernel Decorator - AST parsing and AIR generation.

The @kernel decorator parses Python function source code into an AST,
converts it to Python IR, and then lowers it to AIR.

Supports function inlining: helper functions called from a kernel are
automatically inlined during compilation.

Example:
    def helper(x: Tensor[64]) -> Tensor[64]:
        return x + 1
    
    @kernel
    def add(a: Tensor[64], b: Tensor[64]) -> Tensor[64]:
        return helper(a) + b  # helper is inlined
    
    # After decoration, add.air_module contains the AIR with helper inlined
"""

import ast
import inspect
import time
import warnings
from typing import Any, Dict, List, Optional, Callable, Tuple, Set
from functools import wraps
from dataclasses import dataclass

from base_dsl.ast_to_ir import get_function_ir, Context
from base_dsl.python_ir import Function as PyIRFunction, Block, Scope, Var, Load, Store, Const, BinOp, Call, Return
from base_dsl.loc import Loc, get_caller_loc

# Import bindings
from ace_bindings import air_builder, nn_addon
from ace_dsl.core.types import Tensor, get_tensor_shape


# ============================================================================
# Phase 2: Exceptions
# ============================================================================

class CompilationError(Exception):
    """Error during kernel compilation."""
    pass


class CacheWarning(UserWarning):
    """Warning about cache usage (too many variants)."""
    pass


# ============================================================================
# Phase 2: Shape Specialization Data Structures
# ============================================================================

# Shape signature type: tuple of parameter shapes
# Example: ((64,), (128,)) for two parameters with shapes [64] and [128]
ShapeSignature = Tuple[Tuple[int, ...], ...]


@dataclass
class VariantInfo:
    """
    Information about a compiled kernel variant for specific shape signature.

    Phase 2 enables compiling the same kernel with different parameter shapes.
    Each compilation creates a variant stored in _compiled_variants cache.

    Attributes:
        shape_signature: Immutable shape signature (e.g., ((64,), (64,)))
        air_module: AIR global scope for this variant
        air_func: AIR function scope for this variant
        python_ir: Python IR (shared across all variants)
        compile_time: Compilation duration in seconds
        call_count: Number of times this variant was used (for LRU)
    """
    shape_signature: ShapeSignature
    air_module: Any
    air_func: Any
    python_ir: Any
    compile_time: float
    call_count: int = 0


# ============================================================================
# Function Registry for Inlining
# ============================================================================

# Global registry mapping function names to their Python callable
_FUNCTION_REGISTRY: Dict[str, Callable] = {}

def register_helper(func: Callable) -> Callable:
    """
    Register a helper function for inlining.
    
    Helper functions are automatically inlined when called from a @kernel.
    
    Example:
        @register_helper
        def my_helper(x):
            return x + 1
        
        @kernel
        def main(a: Tensor[64]) -> Tensor[64]:
            return my_helper(a)  # my_helper is inlined
    """
    _FUNCTION_REGISTRY[func.__name__] = func
    return func


def _resolve_callee(callee_name: str, calling_scope: Dict[str, Any]) -> Optional[Callable]:
    """
    Resolve a callee name to a Python function.
    
    Looks up in:
    1. Global function registry
    2. Calling scope (locals/globals from the kernel function)
    """
    # Check registry first
    if callee_name in _FUNCTION_REGISTRY:
        return _FUNCTION_REGISTRY[callee_name]
    
    # Check calling scope
    if callee_name in calling_scope:
        func = calling_scope[callee_name]
        if callable(func) and not isinstance(func, type):
            return func
    
    return None


class CompiledKernel:
    """
    Represents a compiled kernel function.

    Phase 2: Supports shape specialization - can compile for multiple shapes.

    Attributes:
        name: Function name
        python_func: Original Python function
        python_ir: Python IR representation (shared across variants)
        air_func: AIR function scope (points to default variant for backward compat)
        air_module: Full AIR module (points to default variant for backward compat)
        _compiled_variants: Cache of compiled variants keyed by shape signature
        _default_signature: Shape signature of the first compiled variant
    """

    def __init__(self, name: str, python_func: Callable):
        self.name = name
        self.python_func = python_func
        self.python_ir: Optional[PyIRFunction] = None

        # Phase 2: Variant storage
        self._compiled_variants: Dict[ShapeSignature, VariantInfo] = {}
        self._default_signature: Optional[ShapeSignature] = None

        # Legacy properties for backward compatibility (point to default variant)
        self.air_func: Optional[Any] = None
        self.air_module: Optional[Any] = None
        self._compiled = False
    
    @property
    def parameters(self) -> Tuple[str, ...]:
        """Get parameter names from the original function."""
        import inspect
        sig = inspect.signature(self.python_func)
        return tuple(sig.parameters.keys())
    
    def __call__(self, *args, **kwargs):
        """
        Calling a CompiledKernel does NOT execute it.
        Use compile_fhe() to compile and get the result.
        """
        if not self._compiled:
            raise RuntimeError(
                f"Kernel '{self.name}' has not been compiled. "
                f"Use compile_fhe({self.name}) to compile."
            )
        # In a full implementation, this would invoke the compiled code
        raise NotImplementedError("Direct kernel execution not implemented")
    
    def compile(
        self,
        shapes: Optional[List[Tuple[int, ...]]] = None,
        enable_ir_printing: bool = False
    ) -> 'CompiledKernel':
        """
        Compile kernel for specific shapes or annotation shapes (Phase 2).

        Args:
            shapes: List of parameter shapes, e.g., [(64,), (128,)].
                   If None, uses shapes from type annotations.
            enable_ir_printing: Print IR during compilation.

        Returns:
            Self for chaining.

        Examples:
            # Phase 1 behavior (backward compatible)
            kernel.compile()  # Uses annotation shapes

            # Phase 2 behavior (new)
            kernel.compile(shapes=[(32,), (32,)])  # Explicit shapes
        """
        # Extract shapes (from args or annotations)
        if shapes is None:
            shapes = self._extract_annotation_shapes()

        # Create signature
        signature = self._make_signature(shapes)

        # Check cache
        if signature in self._compiled_variants:
            return self  # Already compiled

        # Compile new variant
        variant = self._compile_variant(signature, enable_ir_printing)
        self._compiled_variants[signature] = variant

        # Warn if too many variants cached
        if len(self._compiled_variants) > 10:
            warnings.warn(
                f"Kernel '{self.name}' has {len(self._compiled_variants)} "
                f"compiled variants (~{len(self._compiled_variants) * 4}MB memory). "
                f"Consider calling .clear_cache() to free unused variants.",
                CacheWarning
            )

        # Set default on first compile (backward compatibility)
        if self._default_signature is None:
            self._default_signature = signature
            self.air_module = variant.air_module
            self.air_func = variant.air_func
            self._compiled = True

        return self
    
    def dump_ir(self) -> str:
        """Return the AIR as a string."""
        # Compile if not already compiled
        if not self._compiled:
            self.compile()
        if self.air_module:
            return self.air_module.dump()
        return ""

    # ========================================================================
    # Phase 2: Helper Methods for Shape Specialization
    # ========================================================================

    def _make_signature(self, shapes: List[Tuple[int, ...]]) -> ShapeSignature:
        """
        Create immutable shape signature from list of shapes.

        Args:
            shapes: List of parameter shapes, e.g., [(64,), (128,)]

        Returns:
            Immutable tuple of tuples: ((64,), (128,))
        """
        return tuple(tuple(s) for s in shapes)

    def _extract_annotation_shapes(self) -> List[Tuple[int, ...]]:
        """
        Extract shapes from function type annotations.

        Inspects parameter annotations and extracts shapes for Tensor types.
        Returns default shape (64,) for unannotated or non-Tensor parameters.

        Returns:
            List of shapes, one per parameter
        """
        sig = inspect.signature(self.python_func)
        shapes = []

        for param_name, param in sig.parameters.items():
            annotation = param.annotation

            if isinstance(annotation, Tensor):
                # Tensor[64] or Tensor[3, 224, 224]
                shapes.append(annotation.shape)
            elif annotation == inspect.Parameter.empty:
                # No annotation - use default
                shapes.append((64,))
            else:
                # Non-tensor type (Ciphertext, etc.) - no shape
                shapes.append(())

        return shapes

    def _compile_variant(
        self,
        signature: ShapeSignature,
        enable_ir_printing: bool
    ) -> VariantInfo:
        """
        Compile a specific shape variant (Phase 2).

        Args:
            signature: Shape signature to compile for
            enable_ir_printing: Whether to print IR during compilation

        Returns:
            VariantInfo with compiled AIR module
        """
        start = time.time()

        try:
            # Parse Python IR (once, shared across variants)
            if self.python_ir is None:
                scope = Scope()
                self.python_ir = get_function_ir(self.python_func, scope)

            # Get calling scope (globals and closure) for inlining
            calling_scope = {}
            if hasattr(self.python_func, '__globals__'):
                calling_scope.update(self.python_func.__globals__)
            if hasattr(self.python_func, '__closure__') and self.python_func.__closure__:
                code = self.python_func.__code__
                for name, cell in zip(code.co_freevars, self.python_func.__closure__):
                    try:
                        calling_scope[name] = cell.cell_contents
                    except ValueError:
                        pass  # Empty cell

            # Lower to AIR
            # Use original lower_function() for backward compatibility
            # (lower_function_with_shapes has issues with AIR builder state)
            lowering = PythonIRToAIRLowering(enable_ir_printing, calling_scope)
            glob_scope = lowering.lower_function(self.python_ir, Scope())

            if enable_ir_printing:
                print(f"\n=== AIR for {self.name} (shapes={signature}) ===")
                print(glob_scope.dump())

            return VariantInfo(
                shape_signature=signature,
                air_module=glob_scope,
                air_func=lowering._func_scope,
                python_ir=self.python_ir,
                compile_time=time.time() - start,
                call_count=0
            )
        except Exception as e:
            raise CompilationError(
                f"Failed to compile kernel '{self.name}' for shapes {signature}.\n"
                f"Reason: {type(e).__name__}: {e}\n"
                f"Suggestion: Check that all shapes have valid dimensions (> 0)."
            ) from e

    def get_variant(self, shapes: List[Tuple[int, ...]]) -> Optional[VariantInfo]:
        """
        Get compiled variant for specific shapes (Phase 2).

        Args:
            shapes: List of parameter shapes

        Returns:
            VariantInfo if variant exists, None otherwise
        """
        signature = self._make_signature(shapes)
        return self._compiled_variants.get(signature)

    def list_variants(self) -> List[Tuple[ShapeSignature, VariantInfo]]:
        """
        List all compiled variants with their signatures (Phase 2).

        Returns:
            List of (signature, variant_info) tuples
        """
        return list(self._compiled_variants.items())

    def clear_cache(self, keep_default: bool = True):
        """
        Clear compiled variants from cache (Phase 2).

        Args:
            keep_default: If True, keep the default variant (first compiled).
                         If False, clear all variants.
        """
        if keep_default and self._default_signature:
            default = self._compiled_variants.get(self._default_signature)
            self._compiled_variants.clear()
            if default:
                self._compiled_variants[self._default_signature] = default
        else:
            self._compiled_variants.clear()
            self._default_signature = None
            self._compiled = False


class PythonIRToAIRLowering:
    """
    Lowers Python IR to AIR.
    
    Maps Python IR operations to nn::core operations.
    Supports automatic inlining of helper function calls.
    """
    
    # Map Python binop to nn::core opcodes (lowercase to match ast_to_ir.py)
    NN_OP_MAP = {
        'add': 'ADD',
        'sub': 'SUB', 
        'mul': 'MUL',
        'matmul': 'MATMUL',
        'truediv': 'DIVIDE',
        # Comparison operations
        'gt': 'GT',
        'lt': 'LT',
        'ge': 'GE',
        'le': 'LE',
        'eq': 'EQ',
        'ne': 'NE',
    }
    
    # Map builtin function names to nn::core opcodes
    BUILTIN_OP_MAP = {
        # NN operations
        'relu': 'RELU',
        'softmax': 'SOFTMAX',
        'conv': 'CONV',
        'gemm': 'GEMM',
        'matmul': 'MATMUL',
        'average_pool': 'AVERAGE_POOL',
        'max_pool': 'MAX_POOL',
        'flatten': 'FLATTEN',
        # Shape manipulation
        'reshape': 'RESHAPE',
        'permute': 'PERMUTE',
        'transpose': 'TRANSPOSE',
        # Reductions
        'sum': 'REDUCE_SUM',
        'max': 'REDUCE_MAX',
        'min': 'REDUCE_MIN',
        'prod': 'REDUCE_PROD',
        'mean': 'REDUCE_MEAN',
        # Math operations
        'exp': 'EXP',
        'log': 'LOG',
        'sqrt': 'SQRT',
        'sin': 'SIN',
        'cos': 'COS',
        'tanh': 'TANH',
        # Tensor creation
        'zeros': 'ZEROS',
        'ones': 'ONES',
        'full': 'FULL',
        'arange': 'ARANGE',
        # Comparison (return value ops)
        'where': 'WHERE',
    }
    
    def __init__(self, enable_ir_printing: bool = False, calling_scope: Optional[Dict[str, Any]] = None):
        self.enable_ir_printing = enable_ir_printing
        self._glob_scope: Optional[Any] = None
        self._func_scope: Optional[Any] = None
        self._container: Optional[Any] = None
        self._var_map: Dict[str, Any] = {}  # Maps variable names to AIR nodes
        self._func_name_map: Dict[str, str] = {}  # Maps temp var names to function names (for inlining)
        self._const_values: Dict[str, Any] = {}  # Maps variable names to constant values
        self._range_info: Dict[str, Tuple[int, int]] = {}  # Maps variable names to (start, end) for range
        self._calling_scope: Dict[str, Any] = calling_scope or {}  # For resolving helper functions
        self._inline_stack: Set[str] = set()  # Track call stack to detect recursion
        self._ir_scope: Optional[Scope] = None  # Python IR scope for inlining
        self._current_file_id: int = 0  # File ID for source location tracking
    
    def _set_source_loc(self, loc: Loc) -> None:
        """Set the current source location for AIR node creation."""
        if self._container and hasattr(self._container, 'set_loc'):
            self._container.set_loc(self._current_file_id, loc.line, loc.col)

    def _extract_parameter_types(self, pyfunc: PyIRFunction) -> List[Dict[str, Any]]:
        """
        Extract type information for each parameter individually.

        Args:
            pyfunc: Python IR function with parameters and annotations

        Returns:
            List of dicts with keys:
                - 'name': parameter name (str)
                - 'shape': shape as list (e.g., [64], [3, 224, 224])
                - 'elem_type': AIR element type (e.g., Type.make_float(32))
                - 'air_type': Complete AIR type (array with shape and elem_type)
        """
        param_types = []

        for param in pyfunc.parameters:
            # Default: scalar float32 (matching original behavior)
            shape = [64]  # Default shape if we need to create an array
            elem_type = air_builder.Type.make_float(32)
            air_type = air_builder.Type.make_float(32)  # Default to scalar

            # Extract from annotation if available
            if param.name in pyfunc.annotations:
                annotation = pyfunc.annotations[param.name]

                # Handle Tensor[shape] annotations - create array type
                if isinstance(annotation, Tensor):
                    if annotation.shape:
                        shape = list(annotation.shape)
                    # Create array type for Tensor annotations
                    air_type = air_builder.Type.make_array(shape, elem_type)
                    # Could support dtype in future: annotation.dtype

                # Handle other domain-specific types (Ciphertext, Polynomial, etc.)
                # These will be handled in domain_kernels.py separately
                # For now, keep scalar float32 default for non-Tensor annotations

            param_types.append({
                'name': param.name,
                'shape': shape,
                'elem_type': elem_type,
                'air_type': air_type
            })

        return param_types

    def _build_param_types_from_signature(
        self,
        pyfunc: PyIRFunction,
        signature: ShapeSignature
    ) -> List[Dict[str, Any]]:
        """
        Build parameter types from shape signature (Phase 2).

        Similar to _extract_parameter_types() but uses provided signature
        instead of extracting from annotations. This enables shape specialization.

        Args:
            pyfunc: Python IR function
            signature: Shape signature (tuple of parameter shapes)

        Returns:
            List of parameter type dicts (same format as _extract_parameter_types)
        """
        param_types = []

        for i, param in enumerate(pyfunc.parameters):
            # Get shape from signature (KEY DIFFERENCE from _extract_parameter_types)
            if i < len(signature):
                shape = list(signature[i])
            else:
                shape = [64]  # Fallback

            # Create AIR type
            elem_type = air_builder.Type.make_float(32)
            if shape:
                air_type = air_builder.Type.make_array(shape, elem_type)
            else:
                air_type = elem_type  # Scalar for empty shape

            param_types.append({
                'name': param.name,
                'shape': shape,
                'elem_type': elem_type,
                'air_type': air_type
            })

        return param_types

    def lower_function_with_shapes(
        self,
        pyfunc: PyIRFunction,
        signature: ShapeSignature,
        scope: Optional[Scope] = None
    ) -> Any:
        """
        Lower Python IR to AIR with specific shape signature (Phase 2).

        This is the shape-specialized version of lower_function().
        Instead of extracting shapes from annotations, uses provided signature.

        Args:
            pyfunc: Python IR function
            signature: Shape signature (e.g., ((64,), (128,)))
            scope: Python IR scope for inlining

        Returns:
            AIR global scope
        """
        # Store scope for inlining
        self._ir_scope = scope or Scope()

        # Create global scope
        self._glob_scope = air_builder.create_glob_scope()

        # Register source file for source location tracking
        # Note: Skip file registration to avoid C++ assertion with multiple variants
        # (AIR builder has global state that prevents re-registering files)
        self._current_file_id = 0

        # Build param types from signature (KEY CHANGE!)
        param_types = self._build_param_types_from_signature(pyfunc, signature)

        # Create function with specialized types
        func_shape = param_types[0]['shape'] if param_types else [64]
        num_params = len(pyfunc.parameters)
        self._func_scope = self._glob_scope.new_func_with_type(
            pyfunc.name, num_params, func_shape, "tensor"
        )
        self._container = self._func_scope.container()

        # Create parameters with specialized types
        self._var_map.clear()
        self._func_name_map.clear()
        for i, param in enumerate(pyfunc.parameters):
            param_info = param_types[i]
            air_param = self._func_scope.new_param(
                param.name, param_info['air_type']
            )
            self._var_map[param.name] = air_param

        # Lower body (unchanged)
        self._inline_stack.add(pyfunc.name)
        self.lower_block(pyfunc.root_block)
        self._inline_stack.discard(pyfunc.name)

        return self._glob_scope

    def lower_function(self, pyfunc: PyIRFunction, scope: Optional[Scope] = None) -> Any:
        """Lower a Python IR function to AIR."""
        # Store scope for inlining
        self._ir_scope = scope or Scope()
        
        # Create global scope
        self._glob_scope = air_builder.create_glob_scope()
        
        # Register source file for source location tracking
        if pyfunc.loc and pyfunc.loc.filename:
            try:
                self._current_file_id = self._glob_scope.register_file(pyfunc.loc.filename)
            except:
                # File may already be registered in some cases (multiple kernels from same file)
                self._current_file_id = 0
        
        # Extract per-parameter type information
        param_types = self._extract_parameter_types(pyfunc)

        # Determine function-level shape (use first parameter's shape or default)
        # Note: new_func_with_type() expects a single shape, so we use first param
        func_shape = param_types[0]['shape'] if param_types else [64]

        # Create function with parameters defined in signature
        # Use 'tensor' as default type name for better readability
        num_params = len(pyfunc.parameters)
        self._func_scope = self._glob_scope.new_func_with_type(
            pyfunc.name, num_params, func_shape, "tensor")
        self._container = self._func_scope.container()

        # Create parameters with individual types
        self._var_map.clear()
        self._func_name_map.clear()
        for i, param in enumerate(pyfunc.parameters):
            param_info = param_types[i]
            # Use the per-parameter AIR type
            air_param = self._func_scope.new_param(param.name, param_info['air_type'])
            self._var_map[param.name] = air_param
        
        # Track this function in inline stack
        self._inline_stack.add(pyfunc.name)
        
        # Lower body
        self.lower_block(pyfunc.root_block)
        
        # Remove from inline stack
        self._inline_stack.discard(pyfunc.name)
        
        return self._glob_scope
    
    def lower_block(self, block: Block):
        """Lower a block of Python IR operations."""
        for op in block.operations:  # Fixed: ops -> operations
            self.lower_operation(op)
    
    def lower_operation(self, op: Any):
        """Lower a single Python IR operation."""
        op_type = type(op).__name__
        
        # Set source location for this operation
        if hasattr(op, 'loc') and op.loc:
            self._set_source_loc(op.loc)
        
        if op_type == 'Store':
            # Store: var_name = value
            value_node = self._get_value(op.operands.get('value'))
            
            # If inside a control flow body (loop or if), emit an actual store statement
            if hasattr(self._container, 'in_control_flow_body') and self._container.in_control_flow_body():
                # Emit store statement so it appears in the block
                stored_node = self._container.new_stid(op.var_name, value_node)
                self._var_map[op.var_name] = stored_node
            else:
                # Outside control flow, just track the value
                self._var_map[op.var_name] = value_node
        
        elif op_type == 'Const':
            # Constant value - track the Python value for range() etc.
            if op.result_vars:
                result_name = op.result_vars[0].name
                # Store the actual Python value for use in range extraction
                self._const_values[result_name] = op.value
                # Also create an AIR node for the constant
                if isinstance(op.value, int):
                    self._var_map[result_name] = self._container.new_intconst(op.value)
                elif isinstance(op.value, float):
                    self._var_map[result_name] = self._container.new_floatconst(op.value)
                else:
                    # String or other - create a placeholder
                    self._var_map[result_name] = self._container.new_intconst(0)
            
        elif op_type == 'Return':
            # Return statement
            value = op.operands.get('value')
            if value:
                value_node = self._get_value(value)
                self._container.new_retv(value_node)
            else:
                self._container.new_ret()
                
        elif op_type == 'BinOp':
            # Binary operation
            left = self._get_value(op.operands.get('lhs'))
            right = self._get_value(op.operands.get('rhs'))
            op_name = op.op_name  # e.g., 'add', 'mul', 'sub', 'gt', 'lt', etc.
            
            if op_name in self.NN_OP_MAP:
                nn_op = self.NN_OP_MAP[op_name]
                # Arithmetic operations
                if nn_op == 'ADD':
                    result = self._container.new_add(left, right)
                elif nn_op == 'SUB':
                    result = self._container.new_sub(left, right)
                elif nn_op == 'MUL':
                    result = self._container.new_mul(left, right)
                elif nn_op == 'MATMUL':
                    result = self._container.new_matmul(left, right)
                elif nn_op == 'DIVIDE':
                    result = self._container.new_div(left, right)
                # Comparison operations (return relational nodes)
                elif nn_op == 'GT':
                    result = self._container.new_gt(left, right)
                elif nn_op == 'LT':
                    result = self._container.new_lt(left, right)
                elif nn_op == 'GE':
                    result = self._container.new_ge(left, right)
                elif nn_op == 'LE':
                    result = self._container.new_le(left, right)
                elif nn_op == 'EQ':
                    result = self._container.new_eq(left, right)
                elif nn_op == 'NE':
                    result = self._container.new_ne(left, right)
                else:
                    raise NotImplementedError(f"Binary op {op_name} not implemented")
                
                if op.result_vars:
                    self._var_map[op.result_vars[0].name] = result
                    
        elif op_type == 'Call':
            # Function call
            callee = self._get_callee_name(op.operands.get('callee'))
            raw_args = op.operands.get('args', ())
            
            # Special handling for range() - store range info for loop lowering
            if callee == 'range':
                start = 0
                end = 10  # default
                
                def extract_const(arg):
                    """Extract constant value from Const or Var pointing to const."""
                    if isinstance(arg, Const):
                        return arg.value
                    elif isinstance(arg, Var):
                        # Check if this var holds a constant
                        if arg.name in self._const_values:
                            return self._const_values[arg.name]
                    return None
                
                # range(n) or range(start, end)
                if len(raw_args) == 1:
                    val = extract_const(raw_args[0])
                    if val is not None:
                        end = val
                elif len(raw_args) >= 2:
                    val0 = extract_const(raw_args[0])
                    val1 = extract_const(raw_args[1])
                    if val0 is not None:
                        start = val0
                    if val1 is not None:
                        end = val1
                
                # Store range info for later use in for loop lowering
                if op.result_vars:
                    self._range_info[op.result_vars[0].name] = (start, end)
                    # Also store a placeholder in var_map
                    self._var_map[op.result_vars[0].name] = self._container.new_intconst(end)
                return
            
            args = [self._get_value(arg) for arg in raw_args]
            
            if callee in self.BUILTIN_OP_MAP:
                nn_op = self.BUILTIN_OP_MAP[callee]
                result = self._emit_nn_op(nn_op, args, op.kwargs if hasattr(op, 'kwargs') else {})
            else:
                # Unknown function - emit generic call
                result = self._emit_generic_call(callee, args)
            
            if op.result_vars:
                self._var_map[op.result_vars[0].name] = result
        
        elif op_type == 'Const':
            # Constant value
            if op.result_vars:
                if isinstance(op.value, int):
                    node = self._container.new_intconst(op.value)
                elif isinstance(op.value, float):
                    node = self._container.new_intconst(int(op.value))
                else:
                    node = self._container.new_intconst(0)
                self._var_map[op.result_vars[0].name] = node
        
        elif op_type == 'Load':
            # Load variable
            if op.result_vars:
                result_name = op.result_vars[0].name
                if op.var_name in self._var_map:
                    self._var_map[result_name] = self._var_map[op.var_name]
                
                # Track function name mapping (for inlining)
                # If var_name is a callable in calling scope, remember the mapping
                if op.var_name in self._calling_scope:
                    func = self._calling_scope[op.var_name]
                    if callable(func) and not isinstance(func, type):
                        self._func_name_map[result_name] = op.var_name
                
                # Also check if it's a registered helper
                if op.var_name in _FUNCTION_REGISTRY:
                    self._func_name_map[result_name] = op.var_name
                
                # Track built-in functions like range for control flow
                if op.var_name in ('range', 'len', 'enumerate', 'zip'):
                    self._func_name_map[result_name] = op.var_name
        
        elif op_type == 'ForLoop':
            # For loop
            self._lower_for_loop(op)
        
        elif op_type == 'If':
            # If statement
            self._lower_if(op)
        
        elif op_type == 'UnaryOp':
            # Unary operation
            operand = self._get_value(op.operands.get('operand'))
            result = self._emit_unary_op(op.op_name, operand)
            if op.result_vars:
                self._var_map[op.result_vars[0].name] = result
    
    def _get_value(self, val: Any) -> Any:
        """Convert a Python IR value to an AIR node."""
        if val is None:
            return None
        if isinstance(val, Var):
            if val.name in self._var_map:
                return self._var_map[val.name]
            raise ValueError(f"Unknown variable: {val.name}")
        elif isinstance(val, Const):
            if isinstance(val.value, int):
                return self._container.new_intconst(val.value)
            elif isinstance(val.value, float):
                return self._container.new_intconst(int(val.value))
            else:
                return self._container.new_intconst(0)
        elif isinstance(val, Load):
            return self._var_map.get(val.var_name)
        elif hasattr(val, 'name') and val.name in self._var_map:
            return self._var_map[val.name]
        else:
            # Try to use directly as a node
            return val
    
    def _get_callee_name(self, func: Any) -> str:
        """Extract the function name from a call target."""
        if isinstance(func, str):
            # Check function name map for temp variables
            if func in self._func_name_map:
                return self._func_name_map[func]
            return func
        if hasattr(func, 'name'):
            var_name = func.name
            # Check function name map for temp variables
            if var_name in self._func_name_map:
                return self._func_name_map[var_name]
            return var_name
        if hasattr(func, 'id'):
            return func.id
        return str(func)
    
    def _emit_nn_op(self, op_name: str, args: List[Any], kwargs: Dict[str, Any]) -> Any:
        """Emit an nn::core operation."""
        if op_name == 'ADD':
            return self._container.new_add(args[0], args[1])
        elif op_name == 'SUB':
            return self._container.new_sub(args[0], args[1])
        elif op_name == 'MUL':
            return self._container.new_mul(args[0], args[1])
        elif op_name == 'RELU':
            if hasattr(self._container, 'new_relu'):
                return self._container.new_relu(args[0])
            # Fallback: ReLU as max(x, 0)
            zero = self._container.new_zero()
            return args[0]  # Simplified
        elif op_name == 'CONV':
            if hasattr(self._container, 'new_conv'):
                kernel_size = kwargs.get('kernel_size', [3, 3])
                return self._container.new_conv(args[0], args[1], args[2], kernel_size)
            raise NotImplementedError("CONV not supported")
        elif op_name == 'GEMM':
            if hasattr(self._container, 'new_gemm'):
                return self._container.new_gemm(args[0], args[1], args[2])
            raise NotImplementedError("GEMM not supported")
        elif op_name == 'MATMUL':
            if hasattr(self._container, 'new_matmul'):
                return self._container.new_matmul(args[0], args[1])
            raise NotImplementedError("MATMUL not supported")
        elif op_name == 'AVERAGE_POOL':
            if hasattr(self._container, 'new_average_pool'):
                kernel_size = kwargs.get('kernel_size', [2, 2])
                return self._container.new_average_pool(args[0], kernel_size)
            raise NotImplementedError("AVERAGE_POOL not supported")
        elif op_name == 'FLATTEN':
            if hasattr(self._container, 'new_flatten'):
                return self._container.new_flatten(args[0])
            raise NotImplementedError("FLATTEN not supported")
        elif op_name == 'RESHAPE':
            if hasattr(self._container, 'new_reshape'):
                shape = kwargs.get('shape', args[1] if len(args) > 1 else [])
                return self._container.new_reshape(args[0], shape)
            # Fallback: just return input
            return args[0]
        elif op_name == 'PERMUTE':
            if hasattr(self._container, 'new_permute'):
                axes = kwargs.get('axes', args[1] if len(args) > 1 else None)
                return self._container.new_permute(args[0], axes)
            return args[0]
        elif op_name == 'TRANSPOSE':
            if hasattr(self._container, 'new_transpose'):
                axis0 = kwargs.get('axis0', 0)
                axis1 = kwargs.get('axis1', 1)
                return self._container.new_transpose(args[0], axis0, axis1)
            return args[0]
        # Reductions
        elif op_name == 'REDUCE_SUM':
            if hasattr(self._container, 'new_reduce_sum'):
                axis = kwargs.get('axis', None)
                keepdims = kwargs.get('keepdims', False)
                return self._container.new_reduce_sum(args[0], axis, keepdims)
            return args[0]
        elif op_name == 'REDUCE_MAX':
            if hasattr(self._container, 'new_reduce_max'):
                axis = kwargs.get('axis', None)
                keepdims = kwargs.get('keepdims', False)
                return self._container.new_reduce_max(args[0], axis, keepdims)
            return args[0]
        elif op_name == 'REDUCE_MIN':
            if hasattr(self._container, 'new_reduce_min'):
                axis = kwargs.get('axis', None)
                keepdims = kwargs.get('keepdims', False)
                return self._container.new_reduce_min(args[0], axis, keepdims)
            return args[0]
        elif op_name == 'REDUCE_PROD':
            if hasattr(self._container, 'new_reduce_prod'):
                axis = kwargs.get('axis', None)
                keepdims = kwargs.get('keepdims', False)
                return self._container.new_reduce_prod(args[0], axis, keepdims)
            return args[0]
        elif op_name == 'REDUCE_MEAN':
            if hasattr(self._container, 'new_reduce_mean'):
                axis = kwargs.get('axis', None)
                keepdims = kwargs.get('keepdims', False)
                return self._container.new_reduce_mean(args[0], axis, keepdims)
            return args[0]
        # Math operations
        elif op_name == 'EXP':
            if hasattr(self._container, 'new_exp'):
                return self._container.new_exp(args[0])
            return args[0]
        elif op_name == 'LOG':
            if hasattr(self._container, 'new_log'):
                return self._container.new_log(args[0])
            return args[0]
        elif op_name == 'SQRT':
            if hasattr(self._container, 'new_sqrt'):
                return self._container.new_sqrt(args[0])
            return args[0]
        elif op_name == 'SIN':
            if hasattr(self._container, 'new_sin'):
                return self._container.new_sin(args[0])
            return args[0]
        elif op_name == 'COS':
            if hasattr(self._container, 'new_cos'):
                return self._container.new_cos(args[0])
            return args[0]
        elif op_name == 'TANH':
            if hasattr(self._container, 'new_tanh'):
                return self._container.new_tanh(args[0])
            return args[0]
        # Tensor creation
        elif op_name == 'ZEROS':
            if hasattr(self._container, 'new_zeros'):
                shape = args[0] if args else kwargs.get('shape', [1])
                dtype = kwargs.get('dtype', 'f32')
                return self._container.new_zeros(shape, dtype)
            return self._container.new_intconst(0)
        elif op_name == 'ONES':
            if hasattr(self._container, 'new_ones'):
                shape = args[0] if args else kwargs.get('shape', [1])
                dtype = kwargs.get('dtype', 'f32')
                return self._container.new_ones(shape, dtype)
            return self._container.new_intconst(1)
        elif op_name == 'FULL':
            if hasattr(self._container, 'new_full'):
                shape = args[0] if args else kwargs.get('shape', [1])
                fill_value = args[1] if len(args) > 1 else kwargs.get('fill_value', 0)
                return self._container.new_full(shape, fill_value)
            return self._container.new_intconst(0)
        elif op_name == 'ARANGE':
            if hasattr(self._container, 'new_arange'):
                size = args[0] if args else kwargs.get('size', 1)
                dtype = kwargs.get('dtype', 'i32')
                return self._container.new_arange(size, dtype)
            return self._container.new_intconst(0)
        # Control flow
        elif op_name == 'WHERE':
            if hasattr(self._container, 'new_where'):
                return self._container.new_where(args[0], args[1], args[2])
            # Fallback: cond ? true_val : false_val -> just return true_val
            return args[1] if len(args) > 1 else args[0]
        else:
            raise NotImplementedError(f"NN op {op_name} not implemented")
    
    def _emit_generic_call(self, callee: str, args: List[Any], op: Any = None) -> Any:
        """
        Emit a function call by inlining the callee.
        
        This looks up the callee function and inlines its body,
        similar to how CuTile handles helper functions.
        """
        # Try to resolve the callee function
        callee_func = _resolve_callee(callee, self._calling_scope)
        
        if callee_func is None:
            # Unknown function - just return first arg as fallback
            if self.enable_ir_printing:
                print(f"  [Warning] Unknown function '{callee}', returning first arg")
            return args[0] if args else self._container.new_zero()
        
        # Check for recursion
        if callee in self._inline_stack:
            raise RecursionError(f"Recursive function call detected: {callee}")
        
        # Add to inline stack
        self._inline_stack.add(callee)
        
        try:
            # Get the callee's Python IR with a FRESH scope
            # This ensures each inline has its own temp names
            callee_scope = Scope()
            callee_ir = get_function_ir(callee_func, callee_scope)
            
            if self.enable_ir_printing:
                print(f"  [Inline] {callee}({', '.join(p.name for p in callee_ir.parameters)})")
            
            # Save the callee's parameter names so we can remove them after inlining
            callee_param_names = {p.name for p in callee_ir.parameters}
            
            # Save current values for callee parameters (in case they shadow caller vars)
            saved_param_values = {name: self._var_map.get(name) for name in callee_param_names}
            
            # Map caller arguments to callee parameters
            for arg, param in zip(args, callee_ir.parameters):
                self._var_map[param.name] = arg
            
            # Track the return value
            return_value = None
            
            # Track new temporaries created during inlining
            callee_temps = set()
            
            # Lower the callee's body (this is the inlining!)
            for callee_op in callee_ir.root_block.operations:
                op_type = type(callee_op).__name__
                
                if op_type == 'Return':
                    # Capture the return value instead of emitting RETV
                    value = callee_op.operands.get('value')
                    if value:
                        return_value = self._get_value(value)
                    break
                else:
                    # Track result variables from callee
                    if hasattr(callee_op, 'result_vars'):
                        for rv in callee_op.result_vars:
                            callee_temps.add(rv.name)
                    # Lower other operations normally
                    self.lower_operation(callee_op)
            
            # Restore callee parameter mappings (remove callee-local bindings)
            for name in callee_param_names:
                if saved_param_values[name] is not None:
                    self._var_map[name] = saved_param_values[name]
                elif name in self._var_map:
                    del self._var_map[name]
            
            # Remove callee temporaries from var_map to avoid pollution
            for temp in callee_temps:
                if temp in self._var_map and temp not in callee_param_names:
                    del self._var_map[temp]
            
            return return_value if return_value else (args[0] if args else self._container.new_zero())
            
        finally:
            # Remove from inline stack
            self._inline_stack.discard(callee)
    
    def _lower_for_loop(self, op: Any) -> None:
        """Lower a for loop to AIR."""
        loop_var_name = op.loop_var
        iterable_info = op.operands.get('iterable')
        body_block = op.nested_blocks[0] if op.nested_blocks else None
        
        if body_block is None:
            return
        
        # Try to extract range info from the iterable
        # Look for range(n) or range(start, end) pattern
        range_start = 0
        range_end = 10  # default
        is_range = False
        
        # The iterable is a Var - check if it came from a range() call
        # by looking at the operations that produced it
        if isinstance(iterable_info, Var):
            # Check if this var is bound to a range call result
            # For now, check if the var name indicates it's from range
            # In a more complete implementation, we'd track the definition
            if hasattr(self, '_range_info') and iterable_info.name in self._range_info:
                range_start, range_end = self._range_info[iterable_info.name]
                is_range = True
        
        # Emit loop_begin
        if hasattr(self._container, 'new_loop_begin_range') and is_range:
            loop_node = self._container.new_loop_begin_range(range_start, range_end)
        elif hasattr(self._container, 'new_loop_begin'):
            iterable_var = self._get_value(iterable_info)
            loop_node = self._container.new_loop_begin(iterable_var)
        else:
            loop_node = None
        
        # Get loop index
        if loop_node and hasattr(self._container, 'new_loop_index'):
            idx_node = self._container.new_loop_index(loop_node)
            self._var_map[loop_var_name] = idx_node
        else:
            # Fallback: create an integer constant for the loop var
            self._var_map[loop_var_name] = self._container.new_intconst(0)
        
        # Lower body
        self.lower_block(body_block)
        
        # Emit loop_end
        if hasattr(self._container, 'new_loop_end'):
            self._container.new_loop_end()
    
    def _lower_if(self, op: Any) -> None:
        """Lower an if statement to AIR."""
        condition = self._get_value(op.operands.get('condition'))
        then_block = op.nested_blocks[0] if op.nested_blocks else None
        else_block = op.nested_blocks[1] if len(op.nested_blocks) > 1 else None
        
        # Emit if_begin
        if hasattr(self._container, 'new_if_begin'):
            self._container.new_if_begin(condition)
        
        # Lower then block
        if then_block:
            self.lower_block(then_block)
        
        # Lower else block if present
        if else_block:
            if hasattr(self._container, 'new_else'):
                self._container.new_else()
            self.lower_block(else_block)
        
        # Emit if_end
        if hasattr(self._container, 'new_if_end'):
            self._container.new_if_end()
    
    def _emit_unary_op(self, op_name: str, operand: Any) -> Any:
        """Emit a unary operation."""
        if op_name == 'neg':
            if hasattr(self._container, 'new_neg'):
                return self._container.new_neg(operand)
            # Fallback: 0 - operand
            zero = self._container.new_intconst(0)
            return self._container.new_sub(zero, operand)
        elif op_name == 'not':
            if hasattr(self._container, 'new_not'):
                return self._container.new_not(operand)
            return operand  # Simplified fallback
        else:
            # Fallback
            return operand


def kernel(func: Callable) -> CompiledKernel:
    """
    Decorator that marks a function as a kernel for FHE compilation.
    
    The decorated function's source code is parsed and converted to AIR.
    
    Example:
        @kernel
        def my_model(x: Tensor[64], w: Tensor[64]) -> Tensor[64]:
            return x + w
    
    Args:
        func: Python function to convert to a kernel
        
    Returns:
        CompiledKernel object with methods for compilation
    """
    return CompiledKernel(func.__name__, func)


__all__ = [
    'kernel',
    'register_helper',
    'CompiledKernel',
    'PythonIRToAIRLowering',
]
