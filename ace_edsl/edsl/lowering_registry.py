"""
Selective Lowering Registry for ACE EDSL

This module provides a mechanism to register custom Python lowering functions
that expand high-level operations (like nn::core::conv) into sequences
of lower-level operations (like nn::vector ops).

Key Advantage over acepy:
    ACE EDSL uses operator overloading, so inlining happens AUTOMATICALLY
    during tracing. When a lowering function is called inside a kernel,
    the operator overloading traces through the lowering body and emits
    all operations directly to the AIR. No separate inlining pass needed!

Usage:
    @register_lowering("nn::core", "conv")
    @vector_kernel
    def lower_conv(input, weight, bias):
        # Vector-level implementation - gets traced automatically
        result = bias
        for k in range(9):  # 3x3 kernel
            result = result + input * weight
        return result

    @nn_kernel
    def my_model(x, w, b):
        # Call the lowering directly - operator overloading traces it
        return lower_conv(x, w, b)

Flow:
    1. User defines lowering with @register_lowering + @vector_kernel
    2. User calls lowering function inside another kernel
    3. Operator overloading traces through lowering body automatically
    4. All vector ops are emitted to AIR (inlined automatically!)
    5. C++ passes can be told to skip certain ops via skip list
"""

from typing import Callable, Dict, Any, List, Optional, Tuple, Set
from dataclasses import dataclass, field


# ═══════════════════════════════════════════════════════════════════════════════
# Lowering Registry
# ═══════════════════════════════════════════════════════════════════════════════

@dataclass
class LoweringInfo:
    """Information about a registered lowering function."""
    source_domain: str      # e.g., "nn::core"
    op_name: str            # e.g., "conv"
    target_domain: str      # e.g., "nn::vector"
    lowering_func: Any      # The @vector_kernel function
    description: str = ""
    skip_cpp: bool = True   # If True, C++ passes should skip this op
    
    @property
    def full_op_name(self) -> str:
        """Full op name like 'nn::core::conv'."""
        return f"{self.source_domain}::{self.op_name}"


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


def register_lowering(
    source_domain: str, 
    op_name: str, 
    target_domain: Optional[str] = None,
    description: str = "",
    skip_cpp: bool = True
):
    """
    Decorator to register a lowering function for an operation.
    
    Args:
        source_domain: The domain of the operation to lower (e.g., "nn::core")
        op_name: The name of the operation (e.g., "conv", "matmul")
        target_domain: The target domain (auto-detected if not specified)
        description: Optional description of the lowering
        skip_cpp: If True (default), C++ passes will skip lowering this op
    
    Example:
        @register_lowering("nn::core", "conv", skip_cpp=True)
        @vector_kernel
        def lower_conv(input, weight, bias):
            # This body gets traced automatically via operator overloading
            result = bias
            for k in range(9):
                result = result + input * weight
            return result
    
    Note:
        Unlike acepy, ace_edsl does NOT need a separate inlining pass!
        The lowering body is traced automatically when you call the function.
    """
    def decorator(func):
        nonlocal target_domain
        
        # Auto-detect target domain from kernel type if not specified
        if target_domain is None:
            if hasattr(func, '_py_domain'):
                target_domain = func._py_domain
            elif hasattr(func, 'DOMAIN'):
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
        func._source_domain = source_domain
        func._op_name = op_name
        
        # Notify C++ to skip this op if skip_cpp is True
        if skip_cpp:
            _notify_cpp_skip_op(source_domain, op_name)
        
        return func
    return decorator


def _notify_cpp_skip_op(domain: str, op_name: str):
    """
    Notify C++ bindings to skip this op during lowering passes.
    
    This is called automatically when registering a lowering with skip_cpp=True.
    The C++ PythonLoweringBridge will check this list before lowering each op.
    """
    full_name = f"{domain}::{op_name}"
    
    # Try passmanager binding
    try:
        from ace_bindings import passmanager
        if hasattr(passmanager, 'add_skip_op'):
            passmanager.add_skip_op(full_name)
    except ImportError:
        pass  # C++ bindings not available
    
    # Try nn_addon binding
    try:
        from ace_bindings import nn_addon
        if hasattr(nn_addon, 'add_skip_op'):
            nn_addon.add_skip_op(full_name)
    except ImportError:
        pass


def sync_skip_ops_to_cpp():
    """
    Sync all registered skip ops to C++ bindings.
    
    Call this before running C++ passes to ensure all Python-registered
    lowerings are known to C++.
    
    Example:
        # Register lowerings
        @register_lowering("nn::core", "conv")
        @vector_kernel
        def conv_impl(...): ...
        
        # Sync to C++ before compilation
        sync_skip_ops_to_cpp()
        
        # Now run C++ passes - they will skip conv
    """
    ops = get_ops_to_skip()
    
    # Try passmanager
    try:
        from ace_bindings import passmanager
        if hasattr(passmanager, 'set_skip_ops'):
            passmanager.set_skip_ops(ops)
        elif hasattr(passmanager, 'add_skip_op'):
            for op in ops:
                passmanager.add_skip_op(op)
    except ImportError:
        pass
    
    # Try nn_addon
    try:
        from ace_bindings import nn_addon
        if hasattr(nn_addon, 'set_skip_ops'):
            nn_addon.set_skip_ops(ops)
    except ImportError:
        pass
    
    return ops


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


def get_ops_to_skip() -> List[str]:
    """
    Get list of ops that C++ should skip (ops with skip_cpp=True).
    
    Returns list of full op names like ["nn::core::conv", "nn::core::matmul"].
    """
    return [
        info.full_op_name
        for info in _LOWERING_REGISTRY.values()
        if info.skip_cpp
    ]


def get_ops_to_skip_for_pass(pass_name: str) -> List[str]:
    """
    Get ops to skip for a specific pass.
    
    Args:
        pass_name: Name of the C++ pass (e.g., "tensor2vector", "vector2sihe")
        
    Returns:
        List of op names to skip (just the op name, not full path)
    """
    # Map pass names to source domains
    pass_domain_map = {
        "tensor2vector": "nn::core",
        "vector2sihe": "nn::vector",
        "sihe2ckks": "fhe::sihe",
        "ckks2poly": "fhe::ckks",
    }
    
    target_domain = pass_domain_map.get(pass_name)
    if target_domain is None:
        return []
    
    return [
        info.op_name
        for info in _LOWERING_REGISTRY.values()
        if info.source_domain == target_domain and info.skip_cpp
    ]


# ═══════════════════════════════════════════════════════════════════════════════
# Automatic Inlining Support
# ═══════════════════════════════════════════════════════════════════════════════

def call_lowering(lowering_func: Callable, *args, **kwargs) -> Any:
    """
    Call a registered lowering function.
    
    This is a helper that ensures the lowering is traced properly.
    In ace_edsl, operator overloading handles this automatically,
    but this function can be used for explicit lowering calls.
    
    Usage:
        @nn_kernel
        def my_model(x, w, b):
            # Option 1: Direct call (operator overloading traces automatically)
            return lower_conv(x, w, b)
            
            # Option 2: Explicit call via helper
            return call_lowering(lower_conv, x, w, b)
    """
    # Simply call the function - operator overloading does the rest!
    return lowering_func(*args, **kwargs)


def get_lowering_for_op(op_name: str) -> Optional[Callable]:
    """
    Get the lowering function for an op name.
    
    Searches all domains for a matching lowering.
    """
    for (domain, name), info in _LOWERING_REGISTRY.items():
        if name == op_name:
            return info.lowering_func
    return None


# ═══════════════════════════════════════════════════════════════════════════════
# Pipeline Integration
# ═══════════════════════════════════════════════════════════════════════════════

def configure_pipeline_skip_ops(glob_scope: Any, verbose: bool = False) -> List[str]:
    """
    Configure C++ pipeline to skip ops with Python lowerings.
    
    This should be called before running C++ passes.
    
    Args:
        glob_scope: The AIR GlobScope
        verbose: Print debug info
        
    Returns:
        List of ops that will be skipped
    """
    ops_to_skip = get_ops_to_skip()
    
    if verbose and ops_to_skip:
        print(f"[SelectiveLowering] Configuring C++ to skip: {ops_to_skip}")
    
    # Notify C++ bindings if available
    try:
        from ace_bindings import passmanager
        if hasattr(passmanager, 'set_skip_ops'):
            passmanager.set_skip_ops(ops_to_skip)
        if hasattr(passmanager, 'add_skip_op'):
            for op in ops_to_skip:
                passmanager.add_skip_op(op)
    except ImportError:
        pass
    
    # Also try nn_addon
    try:
        from ace_bindings import nn_addon
        if hasattr(nn_addon, 'set_skip_ops'):
            nn_addon.set_skip_ops(ops_to_skip)
    except ImportError:
        pass
    
    return ops_to_skip


# ═══════════════════════════════════════════════════════════════════════════════
# Debugging
# ═══════════════════════════════════════════════════════════════════════════════

def print_registry_status():
    """Print the current state of the lowering registry."""
    print("=" * 60)
    print("Selective Lowering Registry Status")
    print("=" * 60)
    
    if not _LOWERING_REGISTRY:
        print("  (no lowerings registered)")
        return
    
    for key, info in _LOWERING_REGISTRY.items():
        skip_str = "SKIP C++" if info.skip_cpp else "allow C++"
        print(f"  {info.full_op_name} → {info.target_domain} [{skip_str}]")
        if info.description:
            print(f"    {info.description}")
    
    print()
    ops_to_skip = get_ops_to_skip()
    if ops_to_skip:
        print(f"Ops C++ will skip: {ops_to_skip}")
    print("=" * 60)

