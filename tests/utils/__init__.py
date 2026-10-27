# tests/utils/__init__.py
"""
Test utilities for ANT-ACE tests.

This module provides centralized dependency checks and skip markers.
For production use, import directly from ace:

    from ace import TORCH_AVAILABLE, HAS_RUNTIME, HAS_FRONTEND

For tests:
    from utils import TORCH_AVAILABLE, HAS_FRONTEND, HAS_RUNTIME, skip_if_no_torch

Note: IRBuilder is available from utils for tests
that need direct IR manipulation, but they are not part of the public API.
"""

from .dependencies import (
    ALL_PROVIDER,
    CPU_PROVIDER,
    # Test-specific
    FHE_AVAILABLE,
    FUNCTION_TEST_CASES,
    GPU_PROVIDER,
    HAS_FRONTEND,
    HAS_RUNTIME,
    HAS_TORCH_FX,
    MODEL_TEST_CASES,
    ONNX_AVAILABLE,
    PROVIDER_SPECS,
    TARGET_PARAMS,
    TEST_CASES_AVAILABLE,
    # From ace package (single source of truth)
    TORCH_AVAILABLE,
    TORCH_FX_AVAILABLE,
    TORCH_IR_AVAILABLE,
    ASTToIRConverter,
    IRBuilder,
    IRSerializer,
    TorchExportToIRPipeline,
    # GPU and backend parameters
    gpu_available,
    nn,
    requires_gpu,
    requires_torch,
    requires_torch_fx,
    runtime,
    skip_if_no_fhe,
    skip_if_no_frontend,
    skip_if_no_gpu,
    skip_if_no_onnx,
    # Skip markers
    skip_if_no_torch,
    skip_if_no_torch_fx,
    torch,
)


__all__ = [
    "ALL_PROVIDER",
    "CPU_PROVIDER",
    # Test-specific
    "FHE_AVAILABLE",
    "FUNCTION_TEST_CASES",
    "GPU_PROVIDER",
    "HAS_FRONTEND",
    "HAS_RUNTIME",
    "HAS_TORCH_FX",
    "MODEL_TEST_CASES",
    "ONNX_AVAILABLE",
    "PROVIDER_SPECS",
    "TARGET_PARAMS",
    "TEST_CASES_AVAILABLE",
    # From ace package
    "TORCH_AVAILABLE",
    "TORCH_FX_AVAILABLE",
    "TORCH_IR_AVAILABLE",
    "ASTToIRConverter",
    "IRBuilder",
    "IRSerializer",
    "TorchExportToIRPipeline",
    # GPU and backend parameters
    "gpu_available",
    "nn",
    "requires_gpu",
    "requires_torch",
    "requires_torch_fx",
    "runtime",
    "skip_if_no_fhe",
    "skip_if_no_frontend",
    "skip_if_no_gpu",
    "skip_if_no_onnx",
    # Skip markers
    "skip_if_no_torch",
    "skip_if_no_torch_fx",
    "torch",
]
