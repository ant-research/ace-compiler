# benchmark/resnet/test_latency.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# ResNet FHE latency regression benchmarks.
#
# Records inference timing from DatasetResult.timing and stores as numeric
# baselines via pytest-regressions num_regression with tolerance.
# FHE per-image latency is deterministic (pure cryptographic computation);
# 10 images suffice for stable avg/min/max. No need to test 100/1000.
#
# Baseline files are GPU-specific: resnet20_cifar10_ace_cuda_h20_latency.csv
# The GPU tag is auto-detected from torch.cuda.get_device_name().
#
# Usage:
#   pytest benchmark/resnet/test_latency.py -v
#   pytest benchmark/resnet/test_latency.py -k resnet20 -v
#   pytest benchmark/resnet/test_latency.py -k ace -v
#   pytest benchmark/resnet/test_latency.py --regen-all   # update baselines

import pytest
from _fixtures import TARGET_PARAMS, gpu_tag


# ============================================================================
# Constants
# ============================================================================

NUM_IMAGES = 10

# Tolerance design (per-metric).
#
# FHE per-image latency is deterministic crypto compute, but the *spread*
# across 10 frames differs by metric: shared 8-GPU CI hosts produce
# occasional single-frame spikes (power throttling / PCIe contention) that
# are always upward, never downward.
#   - avg / total: smoothed over 10 frames, spikes diluted. Primary
#     regression gate, kept tight (rtol=0.2).
#   - min: locks to baseline's best frame (idle-machine optimistic lower
#     bound). Lightly relaxed.
#   - max: captures exactly the one spiked frame; needs the most headroom
#     to absorb CI concurrency noise.
# Relaxing min/max does NOT weaken detection: a true regression slows every
# frame, so avg trips first under its tight tolerance. The extreme metrics
# only carry noise-absorption duty.
LATENCY_TOLERANCE = dict(rtol=0.2, atol=30)  # default for avg / total
EXTREME_TOLERANCES = {
    "min_image_ms": dict(rtol=0.3, atol=30),
    "max_image_ms": dict(rtol=2.0, atol=30),
}


# ============================================================================
# Helper
# ============================================================================


def _compute_latency_data(program, images, labels):
    """Run FHE inference and extract timing data.

    Returns a flat numeric dict suitable for num_regression.check().
    """
    n = min(NUM_IMAGES, len(labels))
    result = program.run_dataset(images[:n], labels[:n], top_k=1, verbose=False)
    t = result.timing
    # max_image_ms is included so it is recorded in the baseline CSV, but made
    # non-gating via rtol=2.0 (see EXTREME_TOLERANCES): on shared 8-GPU CI hosts
    # it captures a single power-throttle / PCIe-contention spike whose
    # magnitude is unbounded (observed up to ~2x baseline). avg/total (rtol=0.2)
    # remain the real regression gate; a true regression slows every frame and
    # trips avg first.
    data = {
        "total_ms": t.total_ms,
        "avg_per_image_ms": t.avg_per_image_ms,
        "min_image_ms": t.min_image_ms,
        "max_image_ms": t.max_image_ms,
    }
    print(
        f"\n  latency: total={t.total_ms:.1f}ms  "
        f"avg={t.avg_per_image_ms:.1f}ms  "
        f"min={t.min_image_ms:.1f}ms  "
        f"max={t.max_image_ms:.1f}ms"
    )
    return data


# ============================================================================
# ResNet-20 latency tests
# ============================================================================


@pytest.mark.slow
class TestResNet20Latency:
    """ResNet-20 CIFAR-10 latency across backends."""

    @pytest.mark.parametrize("compiled_resnet_20", TARGET_PARAMS, indirect=True)
    def test_latency(self, compiled_resnet_20, cifar10_images, num_regression):
        program, library, device = compiled_resnet_20
        images, labels, _ = cifar10_images
        data = _compute_latency_data(program, images, labels)
        num_regression.check(
            data,
            f"resnet20_cifar10_{library}_{device}_{gpu_tag()}_latency",
            tolerances=EXTREME_TOLERANCES,
            default_tolerance=LATENCY_TOLERANCE,
        )


# ============================================================================
# ResNet-32 latency tests
# ============================================================================


@pytest.mark.slow
class TestResNet32Latency:
    """ResNet-32 CIFAR-10 latency across backends."""

    @pytest.mark.parametrize("compiled_resnet_32", TARGET_PARAMS, indirect=True)
    def test_latency(self, compiled_resnet_32, cifar10_images, num_regression):
        program, library, device = compiled_resnet_32
        images, labels, _ = cifar10_images
        data = _compute_latency_data(program, images, labels)
        num_regression.check(
            data,
            f"resnet32_cifar10_{library}_{device}_{gpu_tag()}_latency",
            tolerances=EXTREME_TOLERANCES,
            default_tolerance=LATENCY_TOLERANCE,
        )


# ============================================================================
# ResNet-32 CIFAR-100 latency tests
# ============================================================================


@pytest.mark.slow
class TestResNet32Cifar100Latency:
    """ResNet-32 CIFAR-100 latency across backends."""

    @pytest.mark.parametrize("compiled_resnet_32_cifar100", TARGET_PARAMS, indirect=True)
    def test_latency(self, compiled_resnet_32_cifar100, cifar100_images, num_regression):
        program, library, device = compiled_resnet_32_cifar100
        images, labels, _ = cifar100_images
        data = _compute_latency_data(program, images, labels)
        num_regression.check(
            data,
            f"resnet32_cifar100_{library}_{device}_{gpu_tag()}_latency",
            tolerances=EXTREME_TOLERANCES,
            default_tolerance=LATENCY_TOLERANCE,
        )


# ============================================================================
# ResNet-44 latency tests
# ============================================================================


@pytest.mark.slow
class TestResNet44Latency:
    """ResNet-44 CIFAR-10 latency across backends."""

    @pytest.mark.parametrize("compiled_resnet_44", TARGET_PARAMS, indirect=True)
    def test_latency(self, compiled_resnet_44, cifar10_images, num_regression):
        program, library, device = compiled_resnet_44
        images, labels, _ = cifar10_images
        data = _compute_latency_data(program, images, labels)
        num_regression.check(
            data,
            f"resnet44_cifar10_{library}_{device}_{gpu_tag()}_latency",
            tolerances=EXTREME_TOLERANCES,
            default_tolerance=LATENCY_TOLERANCE,
        )


# ============================================================================
# ResNet-56 latency tests
# ============================================================================


@pytest.mark.slow
class TestResNet56Latency:
    """ResNet-56 CIFAR-10 latency across backends."""

    @pytest.mark.parametrize("compiled_resnet_56", TARGET_PARAMS, indirect=True)
    def test_latency(self, compiled_resnet_56, cifar10_images, num_regression):
        program, library, device = compiled_resnet_56
        images, labels, _ = cifar10_images
        data = _compute_latency_data(program, images, labels)
        num_regression.check(
            data,
            f"resnet56_cifar10_{library}_{device}_{gpu_tag()}_latency",
            tolerances=EXTREME_TOLERANCES,
            default_tolerance=LATENCY_TOLERANCE,
        )


# ============================================================================
# ResNet-110 latency tests
# ============================================================================


@pytest.mark.slow
class TestResNet110Latency:
    """ResNet-110 CIFAR-10 latency across backends."""

    @pytest.mark.parametrize("compiled_resnet_110", TARGET_PARAMS, indirect=True)
    def test_latency(self, compiled_resnet_110, cifar10_images, num_regression):
        program, library, device = compiled_resnet_110
        images, labels, _ = cifar10_images
        data = _compute_latency_data(program, images, labels)
        num_regression.check(
            data,
            f"resnet110_cifar10_{library}_{device}_{gpu_tag()}_latency",
            tolerances=EXTREME_TOLERANCES,
            default_tolerance=LATENCY_TOLERANCE,
        )
