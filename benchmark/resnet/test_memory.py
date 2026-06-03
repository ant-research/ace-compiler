# benchmark/resnet/test_memory.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# ResNet FHE memory regression benchmarks.
#
# Records GPU memory snapshots from CompiledProgram.profile() and stores
# peak memory as YAML baselines via pytest-regressions.
#
# Usage:
#   pytest benchmark/resnet/test_memory.py -v
#   pytest benchmark/resnet/test_memory.py -k resnet20 -v
#   pytest benchmark/resnet/test_memory.py --force-regen   # update baselines

import pytest

from conftest import requires_torch_fx, requires_gpu


# ============================================================================
# Helper
# ============================================================================

def _compute_memory_data(program, images, labels, device="cuda"):
    """Profile FHE inference and extract memory data.

    Returns a dict suitable for data_regression.check().
    """
    profile_result = program.profile(images[:1], labels[:1], device=device)

    snapshots = profile_result.memory_snapshots
    if not snapshots:
        return {
            "peak_gpu_used_mb": 0,
            "snapshots": [],
        }

    peak_gpu_used_mb = max(m.gpu_used_mb for m in snapshots)
    return {
        "peak_gpu_used_mb": peak_gpu_used_mb,
        "snapshots": [
            {"phase": m.phase, "gpu_used_mb": m.gpu_used_mb, "gpu_free_mb": m.gpu_free_mb}
            for m in snapshots
        ],
    }


# ============================================================================
# Phantom (CUDA) memory tests
# ============================================================================

@requires_torch_fx
@requires_gpu
@pytest.mark.slow
class TestResNet20MemoryPhantom:
    """ResNet-20 CIFAR-10 memory on phantom/cuda."""

    def test_memory(self, compiled_resnet_20_phantom, cifar10_images, data_regression):
        images, labels, _ = cifar10_images
        data = _compute_memory_data(compiled_resnet_20_phantom, images, labels, device="cuda")
        data_regression.check(data, "resnet20_cifar10_phantom_cuda_memory")


@requires_torch_fx
@requires_gpu
@pytest.mark.slow
class TestResNet32MemoryPhantom:
    """ResNet-32 CIFAR-10 memory on phantom/cuda."""

    def test_memory(self, compiled_resnet_32_phantom, cifar10_images, data_regression):
        images, labels, _ = cifar10_images
        data = _compute_memory_data(compiled_resnet_32_phantom, images, labels, device="cuda")
        data_regression.check(data, "resnet32_cifar10_phantom_cuda_memory")


@requires_torch_fx
@requires_gpu
@pytest.mark.slow
class TestResNet44MemoryPhantom:
    """ResNet-44 CIFAR-10 memory on phantom/cuda."""

    def test_memory(self, compiled_resnet_44_phantom, cifar10_images, data_regression):
        images, labels, _ = cifar10_images
        data = _compute_memory_data(compiled_resnet_44_phantom, images, labels, device="cuda")
        data_regression.check(data, "resnet44_cifar10_phantom_cuda_memory")


@requires_torch_fx
@requires_gpu
@pytest.mark.slow
class TestResNet56MemoryPhantom:
    """ResNet-56 CIFAR-10 memory on phantom/cuda."""

    def test_memory(self, compiled_resnet_56_phantom, cifar10_images, data_regression):
        images, labels, _ = cifar10_images
        data = _compute_memory_data(compiled_resnet_56_phantom, images, labels, device="cuda")
        data_regression.check(data, "resnet56_cifar10_phantom_cuda_memory")


@requires_torch_fx
@requires_gpu
@pytest.mark.slow
class TestResNet110MemoryPhantom:
    """ResNet-110 CIFAR-10 memory on phantom/cuda."""

    def test_memory(self, compiled_resnet_110_phantom, cifar10_images, data_regression):
        images, labels, _ = cifar10_images
        data = _compute_memory_data(compiled_resnet_110_phantom, images, labels, device="cuda")
        data_regression.check(data, "resnet110_cifar10_phantom_cuda_memory")


@requires_torch_fx
@requires_gpu
@pytest.mark.slow
class TestResNet32Cifar100MemoryPhantom:
    """ResNet-32 CIFAR-100 memory on phantom/cuda."""

    def test_memory(self, compiled_resnet_32_cifar100_phantom, cifar100_images, data_regression):
        images, labels, _ = cifar100_images
        data = _compute_memory_data(compiled_resnet_32_cifar100_phantom, images, labels, device="cuda")
        data_regression.check(data, "resnet32_cifar100_phantom_cuda_memory")