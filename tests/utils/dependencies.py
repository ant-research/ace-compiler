# tests/utils/dependencies.py
"""
Centralized dependency checks for ANT-ACE tests.

This module re-exports dependency flags from the ace package and provides
additional test-specific utilities. For production use, import directly from ace:

    from ace import TORCH_AVAILABLE, HAS_RUNTIME, HAS_FRONTEND

For tests:
    from utils import TORCH_AVAILABLE, HAS_FRONTEND, HAS_RUNTIME, skip_if_no_torch
"""

import pytest

# ============================================================================
# Import from ace package (single source of truth)
# ============================================================================

from ace import (
    # C++ extensions
    runtime,
    HAS_RUNTIME,
    HAS_FRONTEND,
    # Optional dependencies
    TORCH_AVAILABLE,
    ONNX_AVAILABLE,
    # Backward compatibility
    torch,
    nn,
)

# Torch.fx availability (internal check for test skip markers)
try:
    import torch.fx as fx
    TORCH_FX_AVAILABLE = True
except ImportError:
    TORCH_FX_AVAILABLE = False
    fx = None

# IRBuilder for reference (internal use)
try:
    from ace.fhe.ir import IRBuilder
except ImportError:
    IRBuilder = None

# Re-export IRBuilder for tests that need it (TensorRegistry removed)

# Aliases for backward compatibility
HAS_TORCH_FX = TORCH_FX_AVAILABLE

# ============================================================================
# Test-specific availability checks
# ============================================================================

# FHE compiler availability
try:
    from ace.fhe import Driver, FHERuntime
    FHE_AVAILABLE = True
except ImportError:
    FHE_AVAILABLE = False
    Driver = None
    FHERuntime = None

# IR pipeline availability
try:
    from ace.fhe.ir import ASTToIRConverter, IRSerializer
except ImportError:
    ASTToIRConverter = None
    IRSerializer = None

try:
    from ace.fhe.ir import TorchExportToIRPipeline
    TORCH_IR_AVAILABLE = True
except ImportError:
    TORCH_IR_AVAILABLE = False
    TorchExportToIRPipeline = None

# Test cases availability
try:
    from ace.sample.ops.specs import ALL_OPS_SPECS
    from ace.sample.funcs.specs import ALL_FUNCS_SPECS
    MODEL_TEST_CASES = ALL_OPS_SPECS
    FUNCTION_TEST_CASES = ALL_FUNCS_SPECS
    TEST_CASES_AVAILABLE = True
except ImportError:
    TEST_CASES_AVAILABLE = False
    MODEL_TEST_CASES = []
    FUNCTION_TEST_CASES = []

# ============================================================================
# Skip Markers (for use with pytest.mark.skipif)
# ============================================================================

skip_if_no_torch = pytest.mark.skipif(
    not TORCH_AVAILABLE, reason="torch not available"
)

skip_if_no_torch_fx = pytest.mark.skipif(
    not TORCH_FX_AVAILABLE, reason="torch.fx not available"
)

skip_if_no_frontend = pytest.mark.skipif(
    not HAS_FRONTEND, reason="C++ frontend extension not available"
)

skip_if_no_fhe = pytest.mark.skipif(
    not FHE_AVAILABLE, reason="ace.fhe not available"
)

skip_if_no_onnx = pytest.mark.skipif(
    not ONNX_AVAILABLE, reason="onnx not available"
)

# Alias for common use case (decorator style)
requires_torch = skip_if_no_torch
requires_torch_fx = skip_if_no_torch_fx

# ============================================================================
# GPU Availability
# ============================================================================

def gpu_available() -> bool:
    """Check if GPU (CUDA) is available for testing.

    Uses torch.cuda.is_available() which checks if PyTorch can use CUDA.
    """
    if not TORCH_AVAILABLE:
        return False
    try:
        import torch
        return torch.cuda.is_available()
    except Exception:
        return False


skip_if_no_gpu = pytest.mark.skipif(
    not gpu_available(),
    reason="GPU not available"
)
requires_gpu = skip_if_no_gpu


# ============================================================================
# Library/Provider Specifications (derived from ace.fhe.backend)
# ============================================================================

# Import provider specs from the ace package (single source of truth)
try:
    from ace.fhe.backend import get_provider_specs
    PROVIDER_SPECS = get_provider_specs()
except ImportError:
    PROVIDER_SPECS = {
        "antlib": {"device": "cpu", "implemented": True},
    }


# ============================================================================
# Provider Parameters for Parametrized Tests
# ============================================================================

# All providers (name, device) — CPU always, GPU auto-skip
CPU_PROVIDER = [
    pytest.param(name, spec["device"], id=f"{name}-cpu")
    for name, spec in PROVIDER_SPECS.items()
    if spec["device"] == "cpu"
]

GPU_PROVIDER = [
    pytest.param(
        name, spec["device"],
        marks=pytest.mark.skipif(not gpu_available(), reason="GPU not available"),
        id=f"{name}-cuda"
    )
    for name, spec in PROVIDER_SPECS.items()
    if spec["device"] == "cuda"
]

ALL_PROVIDER = CPU_PROVIDER + GPU_PROVIDER

# Compilation targets: implemented providers only
# GPU entries auto-skip when no GPU available
TARGET_PARAMS = [
    pytest.param(name, spec["device"],
        marks=pytest.mark.skipif(
            spec["device"] == "cuda" and not gpu_available(),
            reason="GPU not available"
        ),
        id=f"{name}-{spec['device']}")
    for name, spec in PROVIDER_SPECS.items()
    if spec["implemented"]
]