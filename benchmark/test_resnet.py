# benchmark/test_resnet.py
"""
ResNet FHE Performance Benchmarks.

Uses pytest-benchmark for statistical measurement of compilation
and inference latency/throughput.

Run benchmarks:
    pytest benchmark/ --benchmark-only -v
    pytest benchmark/ --benchmark-only --benchmark-autosave   # save results
    pytest benchmark/ --benchmark-only --benchmark-compare=last  # compare with baseline
    pytest benchmark/ -k resnet20 -v                          # specific benchmark

Accuracy tests are in tests/test_regression/test_resnet.py.
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
# Compilation Benchmarks
# ============================================================================

@requires_torch_fx
@requires_gpu
@pytest.mark.slow
def test_resnet20_compile_time(benchmark):
    """Benchmark ResNet-20 FHE compilation time."""
    import torch
    from ace.models.resnet.specs import RESNET20_CIFAR10

    spec = RESNET20_CIFAR10
    model = spec.create()

    # Build example_inputs from input_spec
    example_inputs = tuple(
        torch.randn(inp.shape, dtype=inp.dtype)
        for inp in spec.compile.input_spec
    )

    # Inject library-specific p2c config
    compile_options = dict(spec.compile.compile_options)
    compile_options.setdefault("p2c", {})["lib"] = "phantom"

    # Apply decorator (fast), then benchmark fhe_compile (slow)
    compiled = fhe.compile(
        frontend=spec.compile.frontend,
        library=spec.compile.library,
        device=spec.compile.device,
        encrypt_inputs=spec.compile.encrypt_inputs,
        **compile_options,
    )(model)

    # Compilation is slow, only run once
    result = benchmark.pedantic(
        compiled.fhe_compile,
        args=(example_inputs,),
        rounds=1,
        iterations=1,
    )
    assert result is not None


# ============================================================================
# Inference Latency Benchmarks
# ============================================================================

@requires_torch_fx
@requires_gpu
@pytest.mark.slow
def test_resnet20_inference_latency(benchmark, compiled_resnet20, cifar10_images):
    """Benchmark single ResNet-20 FHE inference latency."""
    images, _ = cifar10_images

    # Warmup (not measured)
    compiled_resnet20(images[0:1])

    # Benchmark
    result = benchmark(compiled_resnet20, images[0:1])
    assert result is not None


@requires_torch_fx
@requires_gpu
@pytest.mark.slow
def test_resnet20_inference_throughput(benchmark, compiled_resnet20, cifar10_images):
    """Benchmark ResNet-20 FHE inference throughput (inferences/sec)."""
    images, _ = cifar10_images

    # Warmup
    compiled_resnet20(images[0:1])

    # FHE inference is slow, use pedantic mode with few rounds
    result = benchmark.pedantic(
        lambda: compiled_resnet20(images[0:1]),
        rounds=3,
        iterations=1,
        warmup_rounds=1,
    )
    assert result is not None