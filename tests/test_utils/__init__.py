# tests/test_utils/__init__.py
"""
Test utilities for ANT-ACE tests.

This module provides centralized dependency checks and skip markers.
For production use, import directly from ace:

    from ace import TORCH_AVAILABLE, HAS_RUNTIME, HAS_FRONTEND

For tests:
    from test_utils import TORCH_AVAILABLE, HAS_FRONTEND, HAS_RUNTIME, skip_if_no_torch

Note: IRBuilder is available from test_utils for tests
that need direct IR manipulation, but they are not part of the public API.
"""

from .dependencies import (
    # From ace package (single source of truth)
    TORCH_AVAILABLE,
    TORCH_FX_AVAILABLE,
    HAS_TORCH_FX,
    ONNX_AVAILABLE,
    HAS_FRONTEND,
    HAS_RUNTIME,
    IRBuilder,
    torch,
    nn,
    runtime,
    # Test-specific
    FHE_AVAILABLE,
    TORCH_IR_AVAILABLE,
    TEST_CASES_AVAILABLE,
    ASTToIRConverter,
    IRSerializer,
    TorchExportToIRPipeline,
    MODEL_TEST_CASES,
    FUNCTION_TEST_CASES,
    # Skip markers
    skip_if_no_torch,
    skip_if_no_torch_fx,
    skip_if_no_frontend,
    skip_if_no_fhe,
    skip_if_no_onnx,
    # GPU and backend parameters
    gpu_available,
    CPU_BACKENDS,
    GPU_BACKENDS,
    ALL_BACKENDS,
)

__all__ = [
    # From ace package
    "TORCH_AVAILABLE",
    "TORCH_FX_AVAILABLE",
    "HAS_TORCH_FX",
    "ONNX_AVAILABLE",
    "HAS_FRONTEND",
    "HAS_RUNTIME",
    "IRBuilder",
    "torch",
    "nn",
    "runtime",
    # Test-specific
    "FHE_AVAILABLE",
    "TORCH_IR_AVAILABLE",
    "TEST_CASES_AVAILABLE",
    "ASTToIRConverter",
    "IRSerializer",
    "TorchExportToIRPipeline",
    "MODEL_TEST_CASES",
    "FUNCTION_TEST_CASES",
    # Skip markers
    "skip_if_no_torch",
    "skip_if_no_torch_fx",
    "skip_if_no_frontend",
    "skip_if_no_fhe",
    "skip_if_no_onnx",
    # GPU and backend parameters
    "gpu_available",
    "CPU_BACKENDS",
    "GPU_BACKENDS",
    "ALL_BACKENDS",
]