#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# MobileNet FHE latency regression benchmark.
#
# Follows benchmark/squeezenet/test_latency.py (and the ResNet/LeNet latency
# benchmarks before it): records inference timing from DatasetResult.timing
# and stores numeric baselines via pytest-regressions num_regression with
# tolerance. Runs on real CIFAR-10 images; FHE latency is deterministic,
# 10 images suffice.
#
# MobileNet is ant-cpu only (AveragePool2d is unsupported on the
# phantom/ace GPU executors — see conftest.py), so the fixture is
# MOBILENET_TARGET_PARAMS with the GPU backends skipped at collection time.
#
# IMPORTANT: performance tests must be run in isolation (never parallel,
# never back-to-back with the accuracy benchmark in the same pytest process).
#
# Usage:
#   pytest benchmark/mobilenet/test_latency.py -v
#   pytest benchmark/mobilenet/test_latency.py -k ant -v
#   pytest benchmark/mobilenet/test_latency.py --force-regen  # update baselines
#
# CPU single-image mode (avoids averaging the OpenMP-parallel 10-image batch):
#   LATENCY_NUM=1 pytest benchmark/mobilenet/test_latency.py -k ant-cpu -v

import os

import pytest
from _fixtures import MOBILENET_TARGET_PARAMS, gpu_tag


# ============================================================================
# Constants
# ============================================================================

# Default number of images for latency measurement.  Can be overridden via the
# LATENCY_NUM environment variable so that slower CPU backends can measure a
# single image instead of averaging over the default 10-image batch.
NUM_IMAGES = int(os.environ.get("LATENCY_NUM", "10"))

# Tolerance design copied from the ResNet/LeNet/SqueezeNet latency benchmarks.
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
# MobileNet latency tests
# ============================================================================


@pytest.mark.slow
class TestMobileNetLatency:
    """MobileNet CIFAR-10 latency (ant-cpu; GPU targets skipped)."""

    @pytest.mark.parametrize("compiled_mobilenet", MOBILENET_TARGET_PARAMS, indirect=True)
    def test_latency(self, compiled_mobilenet, cifar10_images, num_regression):
        program, library, device = compiled_mobilenet
        images, labels, _ = cifar10_images
        data = _compute_latency_data(program, images, labels)
        num_regression.check(
            data,
            f"mobilenet_cifar10_{library}_{device}_{gpu_tag()}_latency",
            tolerances=EXTREME_TOLERANCES,
            default_tolerance=LATENCY_TOLERANCE,
        )
