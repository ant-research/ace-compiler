# benchmark/resnet/test_memory.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# ResNet FHE memory regression benchmarks.
#
# Records GPU memory snapshots from CompiledProgram.profile() and stores
# peak memory as numeric baselines via pytest-regressions num_regression
# with tolerance. FHE memory is determined by model + encryption params,
# not by inference batch size; 1 image suffices.
#
# Usage:
#   pytest benchmark/resnet/test_memory.py -v
#   pytest benchmark/resnet/test_memory.py -k resnet20 -v
#   pytest benchmark/resnet/test_memory.py -k ace -v
#   pytest benchmark/resnet/test_memory.py --force-regen   # update baselines

import pytest
from _fixtures import TARGET_PARAMS


# ============================================================================
# Constants
# ============================================================================

# Tolerance: FHE memory is mostly deterministic (model + key size), but can
# vary due to GPU driver / allocator state (esp. CI cold start). rtol=0.15
# covers fluctuation; atol=200 provides a floor for small values and absorbs
# allocator fragmentation differences across runner environments.
# Passed per-column via tolerances= (not default_tolerance=) so the band is
# reliably applied on CI — matches how test_latency.py passes EXTREME_TOLERANCES.
MEMORY_TOLERANCE = dict(rtol=0.15, atol=200)
MEMORY_TOLERANCES = {"peak_gpu_used_mb": MEMORY_TOLERANCE}


# ============================================================================
# Helper
# ============================================================================


def _compute_memory_data(program, images, labels):
    """Profile FHE inference and extract memory data.

    Runs three profiling passes and takes the median peak GPU memory to
    reduce variance from GPU driver / allocator fluctuation (esp. CI cold
    start). Returns a flat numeric dict suitable for num_regression.check().
    """
    peaks = []
    for _ in range(3):
        profile_result = program.profile(images[:1], labels[:1])
        snapshots = profile_result.memory_snapshots
        if snapshots:
            peaks.append(max(m.gpu_used_mb for m in snapshots))

    if not peaks:
        data = {"peak_gpu_used_mb": 0.0}
    else:
        peaks.sort()
        peak_gpu_used_mb = peaks[len(peaks) // 2]  # median
        # Store as float so pytest-regressions' dataframe comparison takes the
        # np.isclose path (governed by tolerances=); integer columns fall into
        # the strict != branch (dataframe_regression.py:144) which ignores
        # tolerances entirely, so even a 10 MiB allocator/driver drift (well
        # within rtol=0.15 / atol=200) would fail CI. Float obviated type
        # mismatch via the number-vs-number relaxation in _check_data_types.
        data = {"peak_gpu_used_mb": float(peak_gpu_used_mb)}

    print(f"\n  memory: peak_gpu_used={data['peak_gpu_used_mb']:.0f}MB (median of {len(peaks)} runs: {peaks})")
    return data


# ============================================================================
# ResNet-20 memory tests
# ============================================================================


@pytest.mark.slow
class TestResNet20Memory:
    """ResNet-20 CIFAR-10 memory across backends."""

    @pytest.mark.parametrize("compiled_resnet_20", TARGET_PARAMS, indirect=True)
    def test_memory(self, compiled_resnet_20, cifar10_images, num_regression):
        program, library, device = compiled_resnet_20
        images, labels, _ = cifar10_images
        data = _compute_memory_data(program, images, labels)
        num_regression.check(
            data,
            f"resnet20_cifar10_{library}_{device}_memory",
            tolerances=MEMORY_TOLERANCES,
        )


# ============================================================================
# ResNet-32 memory tests
# ============================================================================


@pytest.mark.slow
class TestResNet32Memory:
    """ResNet-32 CIFAR-10 memory across backends."""

    @pytest.mark.parametrize("compiled_resnet_32", TARGET_PARAMS, indirect=True)
    def test_memory(self, compiled_resnet_32, cifar10_images, num_regression):
        program, library, device = compiled_resnet_32
        images, labels, _ = cifar10_images
        data = _compute_memory_data(program, images, labels)
        num_regression.check(
            data,
            f"resnet32_cifar10_{library}_{device}_memory",
            tolerances=MEMORY_TOLERANCES,
        )


# ============================================================================
# ResNet-44 memory tests
# ============================================================================


@pytest.mark.slow
class TestResNet44Memory:
    """ResNet-44 CIFAR-10 memory across backends."""

    @pytest.mark.parametrize("compiled_resnet_44", TARGET_PARAMS, indirect=True)
    def test_memory(self, compiled_resnet_44, cifar10_images, num_regression):
        program, library, device = compiled_resnet_44
        images, labels, _ = cifar10_images
        data = _compute_memory_data(program, images, labels)
        num_regression.check(
            data,
            f"resnet44_cifar10_{library}_{device}_memory",
            tolerances=MEMORY_TOLERANCES,
        )


# ============================================================================
# ResNet-56 memory tests
# ============================================================================


@pytest.mark.slow
class TestResNet56Memory:
    """ResNet-56 CIFAR-10 memory across backends."""

    @pytest.mark.parametrize("compiled_resnet_56", TARGET_PARAMS, indirect=True)
    def test_memory(self, compiled_resnet_56, cifar10_images, num_regression):
        program, library, device = compiled_resnet_56
        images, labels, _ = cifar10_images
        data = _compute_memory_data(program, images, labels)
        num_regression.check(
            data,
            f"resnet56_cifar10_{library}_{device}_memory",
            tolerances=MEMORY_TOLERANCES,
        )


# ============================================================================
# ResNet-110 memory tests
# ============================================================================


@pytest.mark.slow
class TestResNet110Memory:
    """ResNet-110 CIFAR-10 memory across backends."""

    @pytest.mark.parametrize("compiled_resnet_110", TARGET_PARAMS, indirect=True)
    def test_memory(self, compiled_resnet_110, cifar10_images, num_regression):
        program, library, device = compiled_resnet_110
        images, labels, _ = cifar10_images
        data = _compute_memory_data(program, images, labels)
        num_regression.check(
            data,
            f"resnet110_cifar10_{library}_{device}_memory",
            tolerances=MEMORY_TOLERANCES,
        )


# ============================================================================
# ResNet-32 CIFAR-100 memory tests
# ============================================================================


@pytest.mark.slow
class TestResNet32Cifar100Memory:
    """ResNet-32 CIFAR-100 memory across backends."""

    @pytest.mark.parametrize("compiled_resnet_32_cifar100", TARGET_PARAMS, indirect=True)
    def test_memory(self, compiled_resnet_32_cifar100, cifar100_images, num_regression):
        program, library, device = compiled_resnet_32_cifar100
        images, labels, _ = cifar100_images
        data = _compute_memory_data(program, images, labels)
        num_regression.check(
            data,
            f"resnet32_cifar100_{library}_{device}_memory",
            tolerances=MEMORY_TOLERANCES,
        )
