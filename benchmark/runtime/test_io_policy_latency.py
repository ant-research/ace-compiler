# benchmark/runtime/test_io_policy_latency.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# Runtime IO policy latency benchmark.
#
# Compares key_offline latency across IO policies:
#   - staged:   Host staged (fread + cudaMemcpy)
#   - mapped:   Host mapped (mmap + cudaMemcpy)
#   - auto:     Auto-detect (GDS if available, else staged)
#
# Key metric: avg_per_image_ms (key load + inference, most sensitive to IO policy)
#
# The model is configurable via ACE_BENCH_MODEL (default: resnet20).
#
# Baseline files: test_io_policy_latency/{model}_{library}_{device}_{gpu}_{io_policy}_latency.csv
#
# Usage:
#   pytest benchmark/runtime/test_io_policy_latency.py -v
#   pytest benchmark/runtime/test_io_policy_latency.py -k staged -v
#   ACE_BENCH_MODEL=resnet56 pytest benchmark/runtime/test_io_policy_latency.py -v
#   pytest benchmark/runtime/test_io_policy_latency.py --regen-all

import os
import shutil
import tempfile

import pytest
from _fixtures import _compile_spec, gpu_tag
from conftest import (
    GPU_PARAMS,
    IO_POLICY_PARAMS,
    MODEL_SPEC,
)

from ace import fhe
from ace import runtime as _C
from ace.fhe.config import IoPolicySettings
from ace.fhe.runtime import DatasetEvaluator


# ============================================================================
# Constants
# ============================================================================

NUM_IMAGES = 3

# Wide tolerance — disk I/O is inherently variable
IO_POLICY_TOLERANCE = dict(rtol=0.5, atol=5000)


# ============================================================================
# Helpers
# ============================================================================


def _save_keys(library: str, key_dir: str, runner=None):
    """Save FHE keys via the live runner.

    ``runner`` must be a FHERuntime with an active FheContext — call BEFORE
    ``runner.finalize()``. This replaces the retired ctypes HFert_/Phantom_/Ace_*
    path (which re-dlopen'd the backend after finalize).

    Args:
        library: Backend library name ("phantom" or "ace") — kept for API
            compatibility; no longer used.
        key_dir: Output directory for the key bundle.
        runner: A live FHERuntime whose context is active (REQUIRED).
    """
    if runner is None:
        raise ValueError("_save_keys: requires a live runner to save keys (call before finalize())")
    os.makedirs(key_dir, exist_ok=True)
    rc = runner.save_keys(key_dir)
    assert rc == 0, f"runner.save_keys failed rc={rc}"


# ============================================================================
# IO policy latency tests
# ============================================================================


@pytest.mark.slow
@pytest.mark.runtime
class TestIOPolicyLatency:
    """Benchmark: key_offline latency across IO policies."""

    @pytest.mark.parametrize("compiled_model", [GPU_PARAMS[0]], indirect=True)
    @pytest.mark.parametrize("io_policy", IO_POLICY_PARAMS)
    def test_key_offline_io_policy_latency(
        self, compiled_model, cifar10_images, tmp_path_factory, io_policy, num_regression
    ):
        """Measure key load + inference latency with different IO policies."""
        program, library, device = compiled_model
        images, labels = cifar10_images

        # Step 1: Run online to generate keys (context active).
        IoPolicySettings(policy="auto").apply()
        _C.set_key_path("")
        runner = fhe.FHERuntime(program.package)
        DatasetEvaluator(runner).evaluate(images[:1], labels[:1], top_k=1, verbose=False)

        # Step 2: Save keys via the live runner (BEFORE finalize — save_keys
        # needs an active FheContext). Replaces the retired ctypes path.
        key_dir = str(tmp_path_factory.mktemp("io_policy_keys"))
        _save_keys(library, key_dir, runner=runner)
        runner.finalize()

        # Step 3: Load keys with the target IO policy and run inference
        IoPolicySettings(policy=io_policy).apply()
        _C.set_key_path(key_dir)

        package = {
            **program.package,
            "key_path": key_dir,
            "key_mode": "key_offline",
        }

        runner2 = fhe.FHERuntime(package)
        runner2.init()
        result = DatasetEvaluator(runner2).evaluate(images[:NUM_IMAGES], labels[:NUM_IMAGES], top_k=1, verbose=False)
        timing = result.timing
        runner2.finalize()

        IoPolicySettings(policy="auto").apply()
        _C.clear_key_path()

        data = {
            "avg_per_image_ms": timing.avg_per_image_ms,
            "min_image_ms": timing.min_image_ms,
            "max_image_ms": timing.max_image_ms,
            "total_ms": timing.total_ms,
        }

        tag = gpu_tag()
        model_name = MODEL_SPEC.name
        num_regression.check(
            data,
            f"{model_name}_{library}_{device}_{tag}_{io_policy}_latency",
            default_tolerance=IO_POLICY_TOLERANCE,
        )

        print(
            f"\n  {model_name}/{library}/{device}/{io_policy}: "
            f"avg={timing.avg_per_image_ms:.1f}ms  "
            f"total={timing.total_ms:.1f}ms"
        )
