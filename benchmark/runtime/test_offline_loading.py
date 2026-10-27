# benchmark/runtime/test_offline_loading.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# Runtime offline loading latency benchmark.
#
# Benchmarks offline key loading and offline weight encoding across backends
# (phantom, ace) and I/O transport modes (staged, compatible-gds).
# Also covers server_only mode and offline-encode x key-mode combinations.
#
# The model is configurable via ACE_BENCH_MODEL (default: resnet20).
#
# Usage:
#   pytest benchmark/runtime/test_offline_loading.py -v
#   pytest benchmark/runtime/test_offline_loading.py -k offline_keys -v
#   pytest benchmark/runtime/test_offline_loading.py --regen-all

import json
import os
import shutil
import tempfile

import pytest
from _fixtures import _compile_spec, gpu_tag
from conftest import (
    GPU_PARAMS,
    MODEL_SPEC,
)

from ace import fhe
from ace import runtime as _C
from ace.fhe.config import IoConfig, IoPolicySettings
from ace.fhe.runtime import DatasetEvaluator
from ace.kms import OfflinePrep as OfflineEncoder


# ============================================================================
# Constants
# ============================================================================

NUM_IMAGES = 3

# Tolerance: offline loading latency is sensitive to disk I/O
LOADING_TOLERANCE = dict(rtol=0.5, atol=5000)


# ============================================================================
# Helpers
# ============================================================================


def _apply_io_config(io_mode: str):
    """Apply I/O transport configuration."""
    policy_map = {
        "staged": "staged",
        "compatible_gds": "auto",
        "pure_gds": "gds",
    }
    IoPolicySettings(policy=policy_map[io_mode]).apply()


def _extract_profiler_times(profiler_result):
    """Extract fhe::init and fhe::run_batch_sequential times from profiler result."""
    init_ms = 0.0
    infer_ms = 0.0
    for event in profiler_result.fhe_events:
        if "fhe::init" in event.name and "fhe::mem" not in event.name:
            init_ms = max(init_ms, event.cpu_time_total_ms)
        elif "fhe::run_batch_sequential" in event.name:
            infer_ms = max(infer_ms, event.cpu_time_total_ms)
    return init_ms, infer_ms


# ============================================================================
# Fixtures
# ============================================================================


@pytest.fixture(scope="module")
def compiled_offline_model(request):
    """Compile the benchmark model for the given backend.

    Returns (program, library, device, package, kernel_path, config_path).
    """
    library, device = request.param
    vr_file = MODEL_SPEC.get_vr_profile()
    program = _compile_spec(MODEL_SPEC, library=library, device=device, relu_vr_file=vr_file)
    pkg = program.package
    return program, library, device, pkg, pkg["kernel"], pkg.get("config_path", "")


@pytest.fixture(scope="module")
def cifar10_data():
    """Load CIFAR-10 images for benchmarking."""
    from ace.model.dataset import load_cifar10_images

    images, labels = load_cifar10_images(NUM_IMAGES)
    return images, labels


@pytest.fixture(scope="module")
def key_dir():
    """Create a temporary directory for KMS keys."""
    d = tempfile.mkdtemp(prefix="bench_kms_")
    yield d
    shutil.rmtree(d, ignore_errors=True)


# ============================================================================
# Test classes
# ============================================================================

# DISABLED module-wide: every test in this module constructs OfflinePrep
# (now raises NotImplementedError — the encode-only FheContext path was removed
# and *_encode_weight_file C exports are uncompiled orphans), and the key-save
# tests additionally hang on PRELOAD/PRELOAD_FULL galois-key load via tmpfs key
# paths (gds_parallel_read batched read; IO-layer GDS-fallback gap). Re-enable
# after a standalone weight-encoding tool lands AND the GDS fallback is fixed,
# or after migrating these benchmarks onto runner.save_keys + a real-NVMe key
# path.
pytestmark = pytest.mark.skip(
    reason="OfflinePrep disabled + PRELOAD galois-key load hangs on tmpfs; pending standalone encoding tool and GDS fallback fix."
)


@pytest.mark.slow
@pytest.mark.runtime
class TestOfflineKeysLoading:
    """Benchmark offline key loading across backends and I/O modes."""

    @pytest.mark.parametrize("compiled_offline_model", GPU_PARAMS, indirect=True)
    @pytest.mark.parametrize("io_mode", ["staged", "compatible_gds"])
    def test_offline_keys_init_latency(self, compiled_offline_model, cifar10_data, key_dir, io_mode):
        """Offline key loading should produce valid inference results."""
        _program, library, device, pkg, kernel_path, _config_path = compiled_offline_model
        images, labels = cifar10_data

        backend_key_dir = os.path.join(key_dir, f"{library}_{io_mode}")
        os.makedirs(backend_key_dir, exist_ok=True)

        # Step 1: Online baseline — generate keys
        IoPolicySettings(policy="auto").apply()
        _C.set_key_path("")

        runner_online = fhe.FHERuntime(pkg)
        with fhe.profiler(device=device) as prof:
            result_online = DatasetEvaluator(runner_online).evaluate(
                images[:NUM_IMAGES], labels[:NUM_IMAGES], top_k=1, verbose=False
            )
        runner_online.finalize()
        init_online_ms, _ = _extract_profiler_times(prof.result)

        # Step 2: Save keys
        prep = OfflineEncoder(kernel_path=kernel_path, library=library)
        prep.save_keys(backend_key_dir)

        # Step 3: Load keys and run
        _apply_io_config(io_mode)
        _C.set_key_path(backend_key_dir)

        offline_pkg = {**pkg, "key_path": backend_key_dir, "key_mode": "key_offline"}
        runner_offline = fhe.FHERuntime(offline_pkg)
        with fhe.profiler(device=device) as prof:
            result_offline = DatasetEvaluator(runner_offline).evaluate(
                images[:NUM_IMAGES], labels[:NUM_IMAGES], top_k=1, verbose=False
            )
        runner_offline.finalize()
        _C.clear_key_path()

        init_offline_ms, _ = _extract_profiler_times(prof.result)

        # Verify FHE predictions match
        assert result_offline.predictions == result_online.predictions, (
            "Predictions differ between online and offline-key inference"
        )

        print(
            f"\n  {library}/{io_mode}: "
            f"online_init={init_online_ms:.0f}ms  "
            f"offline_init={init_offline_ms:.0f}ms  "
            f"speedup={init_online_ms / max(init_offline_ms, 1):.1f}x"
        )

        # Reset I/O config
        IoPolicySettings(policy="auto").apply()


@pytest.mark.slow
@pytest.mark.runtime
class TestOfflineEncodeLoading:
    """Benchmark offline weight encoding for phantom backend."""

    @pytest.mark.parametrize(
        "compiled_offline_model",
        [GPU_PARAMS[0]],  # phantom only (ACE may not have encode_weight_file)
        indirect=True,
    )
    @pytest.mark.parametrize("io_mode", ["staged", "compatible_gds"])
    def test_offline_encode_init_latency(self, compiled_offline_model, cifar10_data, io_mode):
        """Offline encoded weights should produce matching inference results."""
        _program, library, device, pkg, kernel_path, config_path = compiled_offline_model
        images, labels = cifar10_data

        # Find weight file
        weight_file = None
        if config_path and os.path.exists(config_path):
            with open(config_path) as f:
                config = json.load(f)
            weight_file = config.get("rt_data_info", {}).get("_file_name", "")

        if not weight_file or not os.path.exists(weight_file):
            pytest.skip("No weight file found for offline encoding")

        # Step 1: Online baseline
        IoPolicySettings(policy="auto").apply()
        _C.set_key_path("")

        runner_online = fhe.FHERuntime(pkg)
        with fhe.profiler(device=device) as prof:
            result_online = DatasetEvaluator(runner_online).evaluate(
                images[:NUM_IMAGES], labels[:NUM_IMAGES], top_k=1, verbose=False
            )
        runner_online.finalize()
        init_online_ms, _ = _extract_profiler_times(prof.result)

        # Step 2: Encode weights offline
        encoder = OfflineEncoder(kernel_path=kernel_path, library=library)
        encoded_weight_file = encoder.encode(weight_file)
        encoded_config_path = OfflineEncoder.update_config(config_path, encoded_weight_file)

        # Step 3: Run with encoded weights
        encoded_pkg = dict(pkg)
        encoded_pkg["config_path"] = encoded_config_path

        _apply_io_config(io_mode)
        _C.set_key_path("")

        runner_encoded = fhe.FHERuntime(encoded_pkg)
        with fhe.profiler(device=device) as prof:
            result_encoded = DatasetEvaluator(runner_encoded).evaluate(
                images[:NUM_IMAGES], labels[:NUM_IMAGES], top_k=1, verbose=False
            )
        runner_encoded.finalize()

        init_encoded_ms, _ = _extract_profiler_times(prof.result)

        # Verify predictions match
        assert result_encoded.predictions == result_online.predictions, (
            "Predictions differ between online and offline-encode inference"
        )

        print(
            f"\n  {library}/{io_mode}: "
            f"online_init={init_online_ms:.0f}ms  "
            f"encode_init={init_encoded_ms:.0f}ms  "
            f"speedup={init_online_ms / max(init_encoded_ms, 1):.1f}x"
        )

        # Reset I/O config
        IoPolicySettings(policy="auto").apply()

        # Restore original config
        backup_path = config_path + ".bak"
        if os.path.exists(backup_path):
            shutil.copy2(backup_path, config_path)


@pytest.mark.slow
@pytest.mark.runtime
class TestOfflineKeysLoadingServerOnly:
    """Benchmark offline key loading with server_only mode across backends."""

    @pytest.mark.parametrize("compiled_offline_model", GPU_PARAMS, indirect=True)
    @pytest.mark.parametrize("io_mode", ["staged"])
    def test_server_only_init_latency(self, compiled_offline_model, cifar10_data, key_dir, io_mode):
        """Server-only key loading should init successfully (no encryption)."""
        _program, library, _device, pkg, kernel_path, _config_path = compiled_offline_model
        images, labels = cifar10_data

        backend_key_dir = os.path.join(key_dir, f"{library}_{io_mode}_server_only")
        os.makedirs(backend_key_dir, exist_ok=True)

        # Step 1: Online — generate keys
        IoPolicySettings(policy="auto").apply()
        _C.set_key_path("")

        runner_online = fhe.FHERuntime(pkg)
        DatasetEvaluator(runner_online).evaluate(images[:NUM_IMAGES], labels[:NUM_IMAGES], top_k=1, verbose=False)

        # Step 2: Save eval-only keys
        prep = OfflineEncoder(kernel_path=kernel_path, library=library)
        prep.save_keys(backend_key_dir, eval_only=True)
        runner_online.finalize()

        # Step 3: Load with server_only
        _apply_io_config(io_mode)
        _C.set_key_path(backend_key_dir)

        server_pkg = {
            **pkg,
            "key_path": backend_key_dir,
            "key_mode": "server_only",
        }
        runner_server = fhe.FHERuntime(server_pkg)
        runner_server.init()
        # server_only can't encrypt — just verify init succeeded
        runner_server.finalize()
        _C.clear_key_path()

        # Verify eval keys directory exists, global does not
        assert os.path.isdir(os.path.join(backend_key_dir, "model"))
        assert not os.path.isdir(os.path.join(backend_key_dir, "global"))

        IoPolicySettings(policy="auto").apply()


@pytest.mark.slow
@pytest.mark.runtime
class TestOfflineEncodeKeyModeCombo:
    """Benchmark offline encoding combined with different key modes."""

    @pytest.mark.parametrize(
        "compiled_offline_model",
        [GPU_PARAMS[0]],  # phantom only
        indirect=True,
    )
    @pytest.mark.parametrize("key_mode", ["key_offline", "server_only"])
    def test_offline_encode_with_key_mode(self, compiled_offline_model, cifar10_data, key_dir, key_mode):
        """Offline encoded weights combined with key_offline/server_only."""
        _program, library, _device, pkg, kernel_path, config_path = compiled_offline_model
        images, labels = cifar10_data

        # Find weight file
        weight_file = None
        if config_path and os.path.exists(config_path):
            with open(config_path) as f:
                config = json.load(f)
            weight_file = config.get("rt_data_info", {}).get("_file_name", "")

        if not weight_file or not os.path.exists(weight_file):
            pytest.skip("No weight file found for offline encoding")

        # Step 1: Online baseline
        IoPolicySettings(policy="auto").apply()
        _C.set_key_path("")

        runner_online = fhe.FHERuntime(pkg)
        result_online = DatasetEvaluator(runner_online).evaluate(
            images[:NUM_IMAGES], labels[:NUM_IMAGES], top_k=1, verbose=False
        )

        # Step 2: Encode weights offline
        encoder = OfflineEncoder(kernel_path=kernel_path, library=library)
        encoded_weight_file = encoder.encode(weight_file)
        encoded_config_path = OfflineEncoder.update_config(config_path, encoded_weight_file)

        # Step 3: Save keys
        mode_key_dir = os.path.join(key_dir, f"{library}_encode_{key_mode}")
        os.makedirs(mode_key_dir, exist_ok=True)
        eval_only = key_mode == "server_only"
        prep = OfflineEncoder(kernel_path=kernel_path, library=library)
        prep.save_keys(mode_key_dir, eval_only=eval_only)

        runner_online.finalize()

        # Step 4: Run with encoded weights + key mode
        encoded_pkg = dict(pkg)
        encoded_pkg["config_path"] = encoded_config_path
        encoded_pkg["key_path"] = mode_key_dir
        encoded_pkg["key_mode"] = key_mode

        IoPolicySettings(policy="staged").apply()
        _C.set_key_path(mode_key_dir)

        runner = fhe.FHERuntime(encoded_pkg)
        runner.init()

        if key_mode == "key_offline":
            # Can verify inference results match
            result = DatasetEvaluator(runner).evaluate(images[:NUM_IMAGES], labels[:NUM_IMAGES], top_k=1, verbose=False)
            assert result.predictions == result_online.predictions, (
                f"Predictions differ for {key_mode} + offline encode"
            )

        runner.finalize()
        _C.clear_key_path()
        IoPolicySettings(policy="auto").apply()

        # Restore original config
        backup_path = config_path + ".bak"
        if os.path.exists(backup_path):
            shutil.copy2(backup_path, config_path)
