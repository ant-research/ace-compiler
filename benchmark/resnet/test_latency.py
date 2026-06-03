# benchmark/resnet/test_latency.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# ResNet FHE latency regression benchmarks.
#
# Records inference timing from DatasetResult.timing and stores as YAML baselines.
# Latency values vary across runs; use --force-regen to update baselines after
# compiler/runtime changes. Rounding to 1 decimal reduces noise.
#
# Usage:
#   pytest benchmark/resnet/test_latency.py -v
#   pytest benchmark/resnet/test_latency.py -k resnet20 -v
#   pytest benchmark/resnet/test_latency.py --force-regen   # update baselines
#   pytest benchmark/resnet/test_latency.py -m "not slow"   # skip 1000-img tests

import pytest

from conftest import requires_torch_fx, requires_gpu


# ============================================================================
# Helper
# ============================================================================

def _compute_latency_data(program, images, labels, num_images):
    """Run FHE inference and extract timing data.

    Returns a dict suitable for data_regression.check().
    """
    n = min(num_images, len(labels))
    result = program.run_dataset(images[:n], labels[:n], top_k=1, verbose=False)
    t = result.timing
    return {
        "num_images": t.num_images,
        "total_ms": round(t.total_ms, 1),
        "avg_per_image_ms": round(t.avg_per_image_ms, 1),
        "min_image_ms": round(t.min_image_ms, 1),
        "max_image_ms": round(t.max_image_ms, 1),
    }


# ============================================================================
# Phantom (CUDA) latency tests
# ============================================================================

@requires_torch_fx
@requires_gpu
@pytest.mark.slow
class TestResNet20LatencyPhantom:
    """ResNet-20 CIFAR-10 latency on phantom/cuda."""

    @pytest.mark.parametrize("num_images", [1, 10, 100])
    def test_latency(self, num_images, compiled_resnet_20_phantom,
                     cifar10_images, data_regression):
        images, labels, _ = cifar10_images
        data = _compute_latency_data(
            compiled_resnet_20_phantom, images, labels, num_images,
        )
        data_regression.check(data, f"resnet20_cifar10_phantom_cuda_{num_images}img_latency")

    @pytest.mark.parametrize("num_images", [1000])
    @pytest.mark.slow
    def test_latency_slow(self, num_images, compiled_resnet_20_phantom,
                          cifar10_images, data_regression):
        images, labels, _ = cifar10_images
        data = _compute_latency_data(
            compiled_resnet_20_phantom, images, labels, num_images,
        )
        data_regression.check(data, f"resnet20_cifar10_phantom_cuda_{num_images}img_latency")


@requires_torch_fx
@requires_gpu
@pytest.mark.slow
class TestResNet32LatencyPhantom:
    """ResNet-32 CIFAR-10 latency on phantom/cuda."""

    @pytest.mark.parametrize("num_images", [1, 10, 100])
    def test_latency(self, num_images, compiled_resnet_32_phantom,
                     cifar10_images, data_regression):
        images, labels, _ = cifar10_images
        data = _compute_latency_data(
            compiled_resnet_32_phantom, images, labels, num_images,
        )
        data_regression.check(data, f"resnet32_cifar10_phantom_cuda_{num_images}img_latency")

    @pytest.mark.parametrize("num_images", [1000])
    @pytest.mark.slow
    def test_latency_slow(self, num_images, compiled_resnet_32_phantom,
                          cifar10_images, data_regression):
        images, labels, _ = cifar10_images
        data = _compute_latency_data(
            compiled_resnet_32_phantom, images, labels, num_images,
        )
        data_regression.check(data, f"resnet32_cifar10_phantom_cuda_{num_images}img_latency")


@requires_torch_fx
@requires_gpu
@pytest.mark.slow
class TestResNet44LatencyPhantom:
    """ResNet-44 CIFAR-10 latency on phantom/cuda."""

    @pytest.mark.parametrize("num_images", [1, 10, 100])
    def test_latency(self, num_images, compiled_resnet_44_phantom,
                     cifar10_images, data_regression):
        images, labels, _ = cifar10_images
        data = _compute_latency_data(
            compiled_resnet_44_phantom, images, labels, num_images,
        )
        data_regression.check(data, f"resnet44_cifar10_phantom_cuda_{num_images}img_latency")

    @pytest.mark.parametrize("num_images", [1000])
    @pytest.mark.slow
    def test_latency_slow(self, num_images, compiled_resnet_44_phantom,
                          cifar10_images, data_regression):
        images, labels, _ = cifar10_images
        data = _compute_latency_data(
            compiled_resnet_44_phantom, images, labels, num_images,
        )
        data_regression.check(data, f"resnet44_cifar10_phantom_cuda_{num_images}img_latency")


@requires_torch_fx
@requires_gpu
@pytest.mark.slow
class TestResNet56LatencyPhantom:
    """ResNet-56 CIFAR-10 latency on phantom/cuda."""

    @pytest.mark.parametrize("num_images", [1, 10, 100])
    def test_latency(self, num_images, compiled_resnet_56_phantom,
                     cifar10_images, data_regression):
        images, labels, _ = cifar10_images
        data = _compute_latency_data(
            compiled_resnet_56_phantom, images, labels, num_images,
        )
        data_regression.check(data, f"resnet56_cifar10_phantom_cuda_{num_images}img_latency")

    @pytest.mark.parametrize("num_images", [1000])
    @pytest.mark.slow
    def test_latency_slow(self, num_images, compiled_resnet_56_phantom,
                          cifar10_images, data_regression):
        images, labels, _ = cifar10_images
        data = _compute_latency_data(
            compiled_resnet_56_phantom, images, labels, num_images,
        )
        data_regression.check(data, f"resnet56_cifar10_phantom_cuda_{num_images}img_latency")


@requires_torch_fx
@requires_gpu
@pytest.mark.slow
class TestResNet110LatencyPhantom:
    """ResNet-110 CIFAR-10 latency on phantom/cuda."""

    @pytest.mark.parametrize("num_images", [1, 10, 100])
    def test_latency(self, num_images, compiled_resnet_110_phantom,
                     cifar10_images, data_regression):
        images, labels, _ = cifar10_images
        data = _compute_latency_data(
            compiled_resnet_110_phantom, images, labels, num_images,
        )
        data_regression.check(data, f"resnet110_cifar10_phantom_cuda_{num_images}img_latency")

    @pytest.mark.parametrize("num_images", [1000])
    @pytest.mark.slow
    def test_latency_slow(self, num_images, compiled_resnet_110_phantom,
                          cifar10_images, data_regression):
        images, labels, _ = cifar10_images
        data = _compute_latency_data(
            compiled_resnet_110_phantom, images, labels, num_images,
        )
        data_regression.check(data, f"resnet110_cifar10_phantom_cuda_{num_images}img_latency")


@requires_torch_fx
@requires_gpu
@pytest.mark.slow
class TestResNet32Cifar100LatencyPhantom:
    """ResNet-32 CIFAR-100 latency on phantom/cuda."""

    @pytest.mark.parametrize("num_images", [1, 10, 100])
    def test_latency(self, num_images, compiled_resnet_32_cifar100_phantom,
                     cifar100_images, data_regression):
        images, labels, _ = cifar100_images
        data = _compute_latency_data(
            compiled_resnet_32_cifar100_phantom, images, labels, num_images,
        )
        data_regression.check(data, f"resnet32_cifar100_phantom_cuda_{num_images}img_latency")

    @pytest.mark.parametrize("num_images", [1000])
    @pytest.mark.slow
    def test_latency_slow(self, num_images, compiled_resnet_32_cifar100_phantom,
                          cifar100_images, data_regression):
        images, labels, _ = cifar100_images
        data = _compute_latency_data(
            compiled_resnet_32_cifar100_phantom, images, labels, num_images,
        )
        data_regression.check(data, f"resnet32_cifar100_phantom_cuda_{num_images}img_latency")


# ============================================================================
# Antlib (CPU) latency tests
# ============================================================================

@requires_torch_fx
@pytest.mark.slow
class TestResNet20LatencyAntlib:
    """ResNet-20 CIFAR-10 latency on ant/cpu."""

    @pytest.mark.parametrize("num_images", [1, 10])
    def test_latency(self, num_images, compiled_resnet_20_ant,
                     cifar10_images, data_regression):
        images, labels, _ = cifar10_images
        data = _compute_latency_data(
            compiled_resnet_20_ant, images, labels, num_images,
        )
        data_regression.check(data, f"resnet20_cifar10_ant_cpu_{num_images}img_latency")