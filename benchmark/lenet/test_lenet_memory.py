#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# LeNet FHE memory regression benchmark.
#
# Follows benchmark/resnet/test_memory.py: records peak GPU memory from
# CompiledProgram.profile() and stores numeric baselines via num_regression.
# FHE memory is determined by model + CKKS params; 1 image suffices.
#
# IMPORTANT: The phantom runtime is a process-wide singleton and the memory
# snapshot pair (init_before/init_after) is only taken when the runtime
# context is created. Performance tests therefore MUST NOT be run in
# parallel or back-to-back with latency in the same pytest process — run
# this file on its own:
#
#   pytest benchmark/lenet/test_memory.py -k phantom --force-regen
#
# Usage:
#   pytest benchmark/lenet/test_memory.py -v
#   pytest benchmark/lenet/test_memory.py --force-regen      # update baselines

import pytest
from _fixtures import TARGET_PARAMS
from conftest import SMALL_TARGET_PARAMS


# ============================================================================
# Constants
# ============================================================================

MEMORY_TOLERANCE = dict(rtol=0.15, atol=200)
MEMORY_TOLERANCES = {"peak_gpu_used_mb": MEMORY_TOLERANCE}


# ============================================================================
# Helper
# ============================================================================


def _compute_memory_data(program, images, labels):
    """Profile FHE inference, return median peak GPU memory (float).

    Creates a fresh CompiledProgram per pass so the phantom runtime context
    is initialized anew each time (the memory snapshot pair is only taken
    at runtime init). Reusing a session-scoped program after a prior
    run_dataset would return no snapshot.
    """
    from ace.fhe.runtime import CompiledProgram

    peaks = []
    for _ in range(3):
        fresh = CompiledProgram(program.package)
        profile_result = fresh.profile(images[:1], labels[:1])
        snapshots = profile_result.memory_snapshots
        if snapshots:
            peaks.append(max(m.gpu_used_mb for m in snapshots))

    if not peaks:
        data = {"peak_gpu_used_mb": 0.0}
    else:
        peaks.sort()
        data = {"peak_gpu_used_mb": float(peaks[len(peaks) // 2])}

    print(f"\n  memory: peak_gpu_used={data['peak_gpu_used_mb']:.0f}MB (median of {len(peaks)} runs)")
    return data


# ============================================================================
# LeNet memory tests
# ============================================================================


@pytest.mark.slow
class TestLeNetMemory:
    """LeNet CNN GPU memory across backends."""

    @pytest.mark.parametrize("compiled_lenet", SMALL_TARGET_PARAMS, indirect=True)
    def test_memory(self, compiled_lenet, cifar10_images, num_regression):
        program, library, device = compiled_lenet
        images, labels, _ = cifar10_images
        data = _compute_memory_data(program, images, labels)
        num_regression.check(
            data,
            f"lenet_cifar10_{library}_{device}_memory",
            tolerances=MEMORY_TOLERANCES,
        )