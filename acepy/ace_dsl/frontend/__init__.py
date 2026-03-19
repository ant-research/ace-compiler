"""
ACE DSL Frontend
================

Decorators and compilation entry points.
"""

from .decorator import kernel
from .compile import compile_fhe, compile_to_ir, load_onnx
from .lowering_registry import (
    register_lowering,
    get_lowering,
    has_lowering,
    list_lowerings,
    clear_lowerings,
    get_ops_to_skip,
    sync_skip_ops_to_cpp,
    LoweringInfo,
)

__all__ = [
    "kernel", 
    "compile_fhe", 
    "compile_to_ir", 
    "load_onnx",
    # Lowering registry
    "register_lowering",
    "get_lowering",
    "has_lowering",
    "list_lowerings",
    "clear_lowerings",
    "get_ops_to_skip",
    "sync_skip_ops_to_cpp",
    "LoweringInfo",
]

