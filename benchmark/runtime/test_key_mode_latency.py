# benchmark/runtime/test_key_mode_latency.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# Runtime key-mode latency benchmark.
#
# Compares inference latency across key loading modes:
#   - online:      Generate keys at init (baseline)
#   - key_offline: Load all keys from disk (SK+PK+RLK+GK)
#   - server_only: Load eval keys only (RLK+GK, no SK/PK)
#
# Key metrics:
#   - avg_per_image_ms: Average per-image inference latency
#   - online_avg_per_image_ms: Online baseline for comparison
#
# The model is configurable via ACE_BENCH_MODEL (default: resnet20).
#
# Baseline files: test_key_mode_latency/{model}_{library}_{device}_{gpu}_{mode}_latency.csv
#
# Usage:
#   pytest benchmark/runtime/test_key_mode_latency.py -v
#   pytest benchmark/runtime/test_key_mode_latency.py -k key_offline -v
#   ACE_BENCH_MODEL=resnet56 pytest benchmark/runtime/test_key_mode_latency.py -v
#   pytest benchmark/runtime/test_key_mode_latency.py --regen-all

import os
import shutil

import pytest
from _fixtures import gpu_tag
from conftest import (
    GPU_PARAMS,
    KEY_MODE_PARAMS,
    MODEL_SPEC,
    setup_key_mode_package,
)

from ace import fhe
from ace import runtime as _C
from ace.fhe.config import IoPolicySettings
from ace.fhe.runtime import DatasetEvaluator


# ============================================================================
# Constants
# ============================================================================

NUM_IMAGES = 3

# Wider tolerance for offline modes — disk I/O introduces variance
KEY_MODE_TOLERANCE = dict(rtol=0.3, atol=100)


# ============================================================================
# Key-mode latency tests
# ============================================================================


@pytest.mark.slow
@pytest.mark.runtime
class TestKeyModeLatency:
    """Benchmark: inference latency across key loading modes."""

    @pytest.mark.parametrize("compiled_model", GPU_PARAMS, indirect=True)
    @pytest.mark.parametrize("key_mode", ["online", "key_offline"])
    def test_key_mode_latency(self, compiled_model, cifar10_images, key_mode, tmp_path_factory, num_regression):
        """Measure per-image latency for each key mode."""
        program, library, device = compiled_model
        images, labels = cifar10_images

        # Step 1: Run online to generate keys (needed for key save)
        IoPolicySettings(policy="auto").apply()
        _C.set_key_path("")
        runner_online = fhe.FHERuntime(program.package)
        result_online = DatasetEvaluator(runner_online).evaluate(
            images[:NUM_IMAGES], labels[:NUM_IMAGES], top_k=1, verbose=False
        )
        timing_online = result_online.timing

        # Step 2: Set up for the target key mode — save_keys via the live
        # runner (requires active FheContext, so BEFORE finalize). For online,
        # no save is needed; runner_online stays un-finalized until after setup.
        tmp_dir = str(tmp_path_factory.mktemp("key_mode_bench"))
        try:
            km_package, _key_dir = setup_key_mode_package(
                program, key_mode, tmp_dir, library=library, runner=runner_online
            )

            # Now that keys are saved (if any), the online runner can be released.
            runner_online.finalize()

            if key_mode == "online":
                _C.set_key_path("")
                runner = fhe.FHERuntime(km_package)
            else:
                runner = fhe.FHERuntime(km_package)
                runner.init()

            # Step 3: Run inference
            if key_mode != "server_only":
                result = DatasetEvaluator(runner).evaluate(
                    images[:NUM_IMAGES], labels[:NUM_IMAGES], top_k=1, verbose=False
                )
                timing = result.timing
                data = {
                    f"{key_mode}_avg_per_image_ms": timing.avg_per_image_ms,
                    f"{key_mode}_min_image_ms": timing.min_image_ms,
                    f"{key_mode}_max_image_ms": timing.max_image_ms,
                }
            else:
                # server_only can't encrypt — placeholder zeros
                data = {
                    f"{key_mode}_avg_per_image_ms": 0.0,
                    f"{key_mode}_min_image_ms": 0.0,
                    f"{key_mode}_max_image_ms": 0.0,
                }

            runner.finalize()

            # Include online baseline for comparison
            data["online_avg_per_image_ms"] = timing_online.avg_per_image_ms

            tag = gpu_tag()
            model_name = MODEL_SPEC.name
            num_regression.check(
                data,
                f"{model_name}_{library}_{device}_{tag}_{key_mode}_latency",
                default_tolerance=KEY_MODE_TOLERANCE,
            )

            print(
                f"\n  {model_name}/{library}/{device}/{key_mode}: "
                f"online_avg={timing_online.avg_per_image_ms:.1f}ms  "
                f"mode_avg={data.get(f'{key_mode}_avg_per_image_ms', 0):.1f}ms"
            )

        finally:
            IoPolicySettings(policy="auto").apply()
            _C.clear_key_path()
            if os.path.exists(tmp_dir):
                shutil.rmtree(tmp_dir, ignore_errors=True)
