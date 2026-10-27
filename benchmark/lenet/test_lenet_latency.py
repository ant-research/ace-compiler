#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# LeNet FHE latency regression benchmark.
#
# Follows benchmark/resnet/test_latency.py: records inference timing from
# DatasetResult.timing and stores numeric baselines via pytest-regressions
# num_regression with tolerance. Runs on real CIFAR-10 images; FHE latency
# is deterministic, 10 images suffice.
#
# IMPORTANT: performance tests must be run in isolation (never parallel,
# never back-to-back with the memory benchmark in the same pytest process).
#
# Note: accuracy vs CIFAR labels is intentionally NOT benchmarked — LeNet
# is trained on synthetic CIFAR-shaped data, so real-CIFAR accuracy is not
# meaningful. Only latency/memory are performance contracts.
#
# Usage:
#   pytest benchmark/lenet/test_lenet_latency.py -v
#   pytest benchmark/lenet/test_lenet_latency.py -k phantom -v
#   pytest benchmark/lenet/test_lenet_latency.py --force-regen  # update baselines

import pytest
from _fixtures import gpu_tag
from conftest import SMALL_TARGET_PARAMS


# ============================================================================
# Constants
# ============================================================================

NUM_IMAGES = 10

# Tolerance design copied from the ResNet latency benchmark.
LATENCY_TOLERANCE = dict(rtol=0.2, atol=30)
EXTREME_TOLERANCES = {
    "min_image_ms": dict(rtol=0.3, atol=30),
    "max_image_ms": dict(rtol=2.0, atol=30),
}


# ============================================================================
# Helper
# ============================================================================


def _compute_latency_data(program, images, labels):
    """Run FHE inference and extract timing data."""
    n = min(NUM_IMAGES, len(labels))
    result = program.run_dataset(images[:n], labels[:n], top_k=1, verbose=False)
    t = result.timing
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
# LeNet latency tests
# ============================================================================


@pytest.mark.slow
class TestLeNetLatency:
    """LeNet CNN latency across backends."""

    @pytest.mark.parametrize("compiled_lenet", SMALL_TARGET_PARAMS, indirect=True)
    def test_latency(self, compiled_lenet, cifar10_images, num_regression):
        program, library, device = compiled_lenet
        images, labels, _ = cifar10_images
        data = _compute_latency_data(program, images, labels)
        num_regression.check(
            data,
            f"lenet_cifar10_{library}_{device}_{gpu_tag()}_latency",
            tolerances=EXTREME_TOLERANCES,
            default_tolerance=LATENCY_TOLERANCE,
        )