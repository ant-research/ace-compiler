"""
Lowering Registry for PyACE

This module provides a mechanism to register custom lowering functions
that expand high-level operations (like nn::core::conv) into sequences
of lower-level operations (like nn::vector ops).

Usage:
    @register_lowering("nn::core", "conv")
    @vector_kernel
    def lower_conv(input: VectorTensor, weight: VectorTensor) -> VectorTensor:
        # Vector-level implementation
        return input * weight + bias
"""

from typing import Callable, Dict, Any, List, Optional, Tuple
from dataclasses import dataclass

# ═══════════════════════════════════════════════════════════════════════════════
# Lowering Registry
# ═══════════════════════════════════════════════════════════════════════════════

@dataclass
class LoweringInfo:
    """Information about a registered lowering function."""
    source_domain: str      # e.g., "nn::core"
    op_name: str            # e.g., "conv"
    target_domain: str      # e.g., "nn::vector"
    lowering_func: Any      # The kernel function (DomainKernel)
    description: str = ""
    skip_cpp: bool = True   # If True, C++ passes skip this op (default behavior)

# Registry: maps (source_domain, op_name) -> LoweringInfo
_LOWERING_REGISTRY: Dict[Tuple[str, str], LoweringInfo] = {}

# Domain hierarchy for automatic target detection
DOMAIN_HIERARCHY = {
    "nn::core": "nn::vector",
    "nn::vector": "fhe::sihe", 
    "fhe::sihe": "fhe::ckks",
    "fhe::ckks": "fhe::poly",
    "fhe::poly": None  # Terminal domain
}


def register_lowering(source_domain: str, op_name: str, 
                      target_domain: Optional[str] = None,
                      description: str = "",
                      skip_cpp: bool = True):
    """
    Decorator to register a lowering function for an operation.
    
    Args:
        source_domain: The domain of the operation to lower (e.g., "nn::core")
        op_name: The name of the operation (e.g., "conv", "matmul")
        target_domain: The target domain (auto-detected if not specified)
        description: Optional description of the lowering
        skip_cpp: If True (default), C++ passes will skip lowering this op,
                  leaving it for the Python post-lowering pass to handle.
                  If False, both C++ and Python lowerings can be applied.
    
    Example:
        @register_lowering("nn::core", "conv", skip_cpp=True)
        @vector_kernel
        def lower_conv(input, weight, bias):
            return input * weight + bias
    """
    def decorator(func):
        # Auto-detect target domain from kernel type if not specified
        nonlocal target_domain
        if target_domain is None:
            if hasattr(func, 'DOMAIN'):
                target_domain = func.DOMAIN
            else:
                target_domain = DOMAIN_HIERARCHY.get(source_domain, source_domain)
        
        # Register the lowering
        key = (source_domain, op_name)
        _LOWERING_REGISTRY[key] = LoweringInfo(
            source_domain=source_domain,
            op_name=op_name,
            target_domain=target_domain,
            lowering_func=func,
            description=description or f"Lower {source_domain}::{op_name} to {target_domain}",
            skip_cpp=skip_cpp
        )
        
        # Mark the function as a lowering
        func._is_lowering = True
        func._lowering_key = key
        func._skip_cpp = skip_cpp
        
        # Notify C++ bridge if skip_cpp is True
        if skip_cpp:
            _notify_cpp_skip_op(source_domain, op_name)
        
        return func
    return decorator


def _notify_cpp_skip_op(domain: str, op_name: str):
    """Notify the C++ PythonLoweringBridge to skip this op."""
    try:
        from ace_bindings import passmanager
        if hasattr(passmanager, 'add_skip_op'):
            full_name = f"{domain}::{op_name}"
            passmanager.add_skip_op(full_name)
    except ImportError:
        pass  # C++ bindings not available


def get_ops_to_skip() -> List[str]:
    """Get list of ops that C++ should skip (ops with skip_cpp=True)."""
    return [
        f"{info.source_domain}::{info.op_name}"
        for info in _LOWERING_REGISTRY.values()
        if info.skip_cpp
    ]


def sync_skip_ops_to_cpp():
    """Sync all skip ops to the C++ PythonLoweringBridge."""
    try:
        from ace_bindings import passmanager
        if hasattr(passmanager, 'set_skip_ops'):
            ops = get_ops_to_skip()
            passmanager.set_skip_ops(ops)
    except ImportError:
        pass  # C++ bindings not available


def get_lowering(source_domain: str, op_name: str) -> Optional[LoweringInfo]:
    """Get the lowering info for an operation."""
    return _LOWERING_REGISTRY.get((source_domain, op_name))


def has_lowering(source_domain: str, op_name: str) -> bool:
    """Check if a lowering is registered for an operation."""
    return (source_domain, op_name) in _LOWERING_REGISTRY


def list_lowerings(source_domain: Optional[str] = None) -> List[LoweringInfo]:
    """List all registered lowerings, optionally filtered by source domain."""
    if source_domain is None:
        return list(_LOWERING_REGISTRY.values())
    return [info for info in _LOWERING_REGISTRY.values() 
            if info.source_domain == source_domain]


def clear_lowerings():
    """Clear all registered lowerings (useful for testing)."""
    _LOWERING_REGISTRY.clear()


# ═══════════════════════════════════════════════════════════════════════════════
# Inlining Mechanism
# ═══════════════════════════════════════════════════════════════════════════════

class LoweringContext:
    """Context for lowering operations with inlining support."""
    
    def __init__(self, container, var_map: Dict[str, Any]):
        self.container = container
        self.var_map = var_map
        self._inline_stack: List[Dict[str, Any]] = []
    
    def push_inline_scope(self):
        """Push a new scope for inlining."""
        self._inline_stack.append(self.var_map.copy())
    
    def pop_inline_scope(self):
        """Pop the inline scope and restore previous var_map."""
        if self._inline_stack:
            self.var_map = self._inline_stack.pop()
    
    def inline_lowering(self, lowering_info: LoweringInfo, 
                        operands: List[Any]) -> Any:
        """
        Inline a lowering function's operations into the current container.
        
        Args:
            lowering_info: The registered lowering information
            operands: The actual operand values to substitute
            
        Returns:
            The result value from the inlined operations
        """
        kernel = lowering_info.lowering_func
        
        # Ensure kernel is compiled
        if hasattr(kernel, 'compile') and not getattr(kernel, '_compiled', False):
            kernel.compile()
        
        # Get the kernel's Python IR
        python_ir = getattr(kernel, 'python_ir', None)
        if python_ir is None:
            raise ValueError(f"Lowering kernel {kernel} has no Python IR")
        
        # Save current scope
        self.push_inline_scope()
        
        try:
            # Map kernel parameters to actual operands
            for i, param in enumerate(python_ir.parameters):
                if i < len(operands):
                    self.var_map[param.name] = operands[i]
            
            # Process the kernel's operations
            result = self._inline_block(python_ir.root_block)
            
            return result
            
        finally:
            # Restore scope
            self.pop_inline_scope()
    
    def _inline_block(self, block) -> Any:
        """Inline all operations from a block."""
        result = None
        
        for op in block.operations:
            result = self._inline_operation(op)
        
        return result
    
    def _inline_operation(self, op) -> Any:
        """Inline a single operation, emitting to the container."""
        from base_dsl.python_ir import (
            Load, Store, BinaryOp, Return, Constant, Call
        )
        
        op_type = type(op).__name__
        
        if op_type == 'Load':
            # Look up variable in current scope
            return self.var_map.get(op.name)
        
        elif op_type == 'Store':
            # Evaluate value and store
            value = self._inline_operation(op.value)
            self.var_map[op.target] = value
            return value
        
        elif op_type == 'BinaryOp':
            left = self._inline_operation(op.left)
            right = self._inline_operation(op.right)
            return self._emit_binary_op(op.op, left, right)
        
        elif op_type == 'Return':
            if op.value:
                return self._inline_operation(op.value)
            return None
        
        elif op_type == 'Constant':
            return self._emit_constant(op.value)
        
        elif op_type == 'Call':
            # Check if this call has a registered lowering
            func_name = op.func if isinstance(op.func, str) else op.func.name
            
            # Evaluate arguments
            args = [self._inline_operation(arg) for arg in op.args]
            
            # Check for nested lowering
            for domain in ["nn::core", "nn::vector", "fhe::sihe", "fhe::ckks"]:
                if has_lowering(domain, func_name):
                    lowering = get_lowering(domain, func_name)
                    return self.inline_lowering(lowering, args)
            
            # Otherwise emit as a call
            return self._emit_call(func_name, args)
        
        else:
            raise NotImplementedError(f"Cannot inline operation type: {op_type}")
    
    def _emit_binary_op(self, op: str, left: Any, right: Any) -> Any:
        """Emit a binary operation to the container."""
        op_map = {
            'add': 'new_add',
            'sub': 'new_sub', 
            'mul': 'new_mul',
            'div': 'new_div',
            'matmul': 'new_matmul',
        }
        
        method_name = op_map.get(op)
        if method_name and hasattr(self.container, method_name):
            return getattr(self.container, method_name)(left, right)
        
        raise NotImplementedError(f"Binary op {op} not supported for inlining")
    
    def _emit_constant(self, value) -> Any:
        """Emit a constant to the container."""
        if hasattr(self.container, 'new_intconst'):
            if isinstance(value, int):
                return self.container.new_intconst(value)
            elif isinstance(value, float):
                return self.container.new_floatconst(value)
        return value
    
    def _emit_call(self, func_name: str, args: List[Any]) -> Any:
        """Emit a function call to the container."""
        # Map common function names to container methods
        call_map = {
            'relu': 'new_relu',
            'sigmoid': 'new_sigmoid',
            'tanh': 'new_tanh',
            'exp': 'new_exp',
            'log': 'new_log',
            'sqrt': 'new_sqrt',
        }
        
        method_name = call_map.get(func_name)
        if method_name and hasattr(self.container, method_name):
            return getattr(self.container, method_name)(*args)
        
        # Generic call
        if hasattr(self.container, 'new_call'):
            return self.container.new_call(func_name, args)
        
        raise NotImplementedError(f"Call to {func_name} not supported")


# ═══════════════════════════════════════════════════════════════════════════════
# Pass Integration
# ═══════════════════════════════════════════════════════════════════════════════

def apply_lowerings(source_domain: str, python_ir, container, var_map: Dict[str, Any]):
    """
    Apply all registered lowerings for a domain to a Python IR.
    
    This is called during pass execution to inline custom lowerings.
    """
    ctx = LoweringContext(container, var_map)
    
    def process_op(op):
        op_type = type(op).__name__
        
        if op_type == 'Call':
            func_name = op.func if isinstance(op.func, str) else getattr(op.func, 'name', str(op.func))
            
            # Check if there's a registered lowering
            if has_lowering(source_domain, func_name):
                lowering = get_lowering(source_domain, func_name)
                # Evaluate arguments
                args = [process_op(arg) for arg in op.args]
                # Inline the lowering
                return ctx.inline_lowering(lowering, args)
        
        # Default: return op unchanged (let normal lowering handle it)
        return None
    
    # Process all operations in the IR
    for op in python_ir.root_block.operations:
        result = process_op(op)
        if result is not None:
            # Store result if it's a store operation
            if hasattr(op, 'target'):
                var_map[op.target] = result

