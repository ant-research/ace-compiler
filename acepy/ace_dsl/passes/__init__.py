"""
PyACE Passes

This package contains Python passes that run on AIR.

The key pass is `python_lowering_pass` which:
1. Finds ops with Python-registered lowerings
2. Inlines the registered lowering body
3. Runs AFTER C++ passes (which skip these ops)

Usage:
    from ace_dsl.passes import register_lowering, run_python_lowering_pass
    
    # Register a lowering
    @register_lowering("nn::core", "conv")
    @vector_kernel
    def conv_impl(input, weight, bias):
        ...
    
    # Compile and run pass
    kernel.compile()
    run_python_lowering_pass(kernel.glob_scope)
"""

from .python_lowering_pass import (
    register_lowering,
    get_lowering,
    has_lowering,
    list_registered_lowerings,
    clear_lowerings,
    get_ops_to_skip,
    run_python_lowering_pass,
    compile_with_python_lowering,
    PythonLoweringPass,
    LoweringEntry,
)

__all__ = [
    'register_lowering',
    'get_lowering',
    'has_lowering',
    'list_registered_lowerings',
    'clear_lowerings',
    'get_ops_to_skip',
    'run_python_lowering_pass',
    'compile_with_python_lowering',
    'PythonLoweringPass',
    'LoweringEntry',
]
