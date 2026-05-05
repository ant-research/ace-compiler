# benchmark/conftest.py
"""
Pytest configuration and fixtures for benchmark tests.

Uses pytest-benchmark for statistical performance measurement.
Run benchmarks with:
    pytest benchmark/ --benchmark-only -v
    pytest benchmark/ --benchmark-only --benchmark-autosave
    pytest benchmark/ --benchmark-only --benchmark-compare=last
"""
import pytest

from ace import fhe
from test_utils import TORCH_FX_AVAILABLE


requires_torch_fx = pytest.mark.skipif(
    not TORCH_FX_AVAILABLE,
    reason="torch.fx not available"
)

requires_gpu = pytest.mark.skipif(
    not fhe.gpu_available(),
    reason="GPU not available"
)


# ============================================================================
# Model Compilation Fixtures
# ============================================================================

@pytest.fixture(scope="module")
def compiled_resnet20():
    """Compiled ResNet-20 model (phantom-cuda).

    Driver handles caching automatically via .compile_cache/.
    """
    import torch
    from ace.models.resnet.specs import RESNET20_CIFAR10

    spec = RESNET20_CIFAR10
    model = spec.create()

    example_inputs = tuple(
        torch.randn(inp.shape, dtype=inp.dtype)
        for inp in spec.compile.input_spec
    )

    compile_options = dict(spec.compile.compile_options)
    compile_options.setdefault("p2c", {})["lib"] = spec.compile.library

    compiled = fhe.compile(
        frontend=spec.compile.frontend,
        library=spec.compile.library,
        device=spec.compile.device,
        encrypt_inputs=spec.compile.encrypt_inputs,
        **compile_options,
    )(model)

    return compiled.fhe_compile(example_inputs)


@pytest.fixture(scope="module")
def cifar10_images():
    """Load CIFAR-10 test images for benchmarking (10 images)."""
    from ace.models.cifar10 import load_cifar10_images
    images, labels = load_cifar10_images(10)
    return images, labels


# ============================================================================
# Pytest Configuration
# ============================================================================

def pytest_configure(config):
    """Register custom markers for benchmark tests."""
    config.addinivalue_line(
        "markers", "benchmark: mark test as a performance benchmark"
    )
    config.addinivalue_line(
        "markers", "resnet: mark test as a ResNet benchmark"
    )
    config.addinivalue_line(
        "markers", "requires_gpu: mark test as requiring GPU"
    )
    config.addinivalue_line(
        "markers", "slow: Slow tests (use -m 'not slow' to skip)"
    )