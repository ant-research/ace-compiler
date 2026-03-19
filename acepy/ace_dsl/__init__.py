"""
ACE DSL - Python DSL for ACE Compiler
======================================

A Python Domain-Specific Language for the ACE (ANT Compiler for Encryption) 
compiler framework. Enables writing neural network operations in Python that 
compile to FHE (Fully Homomorphic Encryption) operations.

Usage:
    from ace_dsl import kernel, compile_fhe, Tensor

    @kernel
    def my_model(x: Tensor[1, 3, 224, 224], w: Tensor[64, 3, 3, 3]):
        h = conv(x, w, kernel_size=(3, 3))
        return relu(h)

    c_code = compile_fhe(my_model)
"""

from .core.types import Tensor, Ciphertext, Polynomial
from .core.registry import (
    nn_to_vector, 
    vector_to_sihe, 
    sihe_to_ckks,
    ckks_to_poly,
)
from .core.air_value import AIRValue
from .frontend.decorator import kernel, register_helper
from .frontend.compile import compile_fhe, compile_to_ir, load_onnx

# High-level compiler API (no explicit pass specification needed)
from .compiler import (
    ace_compile,
    jit_compile,
    CompilerOptions,
    CompileResult,
    OptLevel,
    Target,
)

# Domain-specific decorators for different pipeline levels
from .frontend.domain_kernels import (
    vector_kernel,
    sihe_kernel,
    ckks_kernel,
    poly_kernel,
    # Domain-specific types
    VectorTensor,
    SiheCiphertext,
    CkksCiphertext,
)

# Import lowering functions to register them
from .lowering import nn_to_vector_ops
from .lowering import vector_to_sihe_ops

__all__ = [
    # Types
    "Tensor",
    "Ciphertext",
    "Polynomial",
    "VectorTensor",
    "SiheCiphertext",
    "CkksCiphertext",
    # Decorators - all pipeline levels
    "kernel",          # Level 1: Tensor (nn::core)
    "vector_kernel",   # Level 2: Vector (nn::vector)
    "sihe_kernel",     # Level 3: SIHE (fhe::sihe)
    "ckks_kernel",     # Level 4: CKKS (fhe::ckks)
    "poly_kernel",     # Level 5: Polynomial (fhe::poly)
    # Helper function inlining
    "register_helper", # Register functions for inlining
    # Compilation (low-level)
    "compile_fhe",
    "compile_to_ir",
    "load_onnx",
    # Compilation (high-level - no pass specification needed)
    "ace_compile",
    "jit_compile",
    "CompilerOptions",
    "CompileResult",
    "OptLevel",
    "Target",
    # Lowering registration
    "nn_to_vector",
    "vector_to_sihe",
    "sihe_to_ckks",
    "ckks_to_poly",
    # Values
    "AIRValue",
]

__version__ = "0.1.0"

