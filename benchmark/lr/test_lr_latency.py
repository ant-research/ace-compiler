#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# LR FHE latency regression benchmark.
#
# Follows benchmark/resnet/test_latency.py: records inference timing from
# DatasetResult.timing and stores numeric baselines via pytest-regressions
# num_regression with tolerance. FHE per-image latency is deterministic;
# 10 synthetic samples suffice.
#
# IMPORTANT: performance tests must be run in isolation (never parallel,
# never back-to-back with the memory benchmark in the same pytest process).
#
# Usage:
#   pytest benchmark/lr/test_lr_latency.py -v
#   pytest benchmark/lr/test_lr_latency.py -k phantom -v
#   pytest benchmark/lr/test_lr_latency.py --force-regen    # update baselines

import pytest
from _fixtures import gpu_tag
from conftest import SMALL_TARGET_PARAMS


# ============================================================================
# Constants
# ============================================================================

NUM_IMAGES = 10

# Tolerance design copied from the ResNet latency benchmark (see
# benchmark/resnet/test_latency.py). avg/total are the primary gate; min/max
# absorb CI power-throttle / PCIe contention spikes.
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
# LR latency tests
# ============================================================================


@pytest.mark.slow
class TestLRLatency:
    """LR single-GEMM latency across backends."""

    @pytest.mark.parametrize("compiled_lr", SMALL_TARGET_PARAMS, indirect=True)
    def test_latency(self, compiled_lr, lr_sample_batch, num_regression):
        program, library, device = compiled_lr
        images, labels = lr_sample_batch
        data = _compute_latency_data(program, images, labels)
        num_regression.check(
            data,
            f"lr_1d_{library}_{device}_{gpu_tag()}_latency",
            tolerances=EXTREME_TOLERANCES,
            default_tolerance=LATENCY_TOLERANCE,
        )