#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""Integration test: key_mode x io_policy matrix combinations.

Tests that various combinations of key loading modes and IO policies
produce correct inference results.

Uses small models (ADD_OP) to keep test time short.

  pytest tests/integration/test_mode_combinations.py -v

Requires: GPU, phantom backend
"""

import os
import shutil
import tempfile

import pytest
import torch

from ace import fhe
from ace import runtime as _C
from ace.fhe.backend import get_library_impl
from ace.fhe.config import IoConfig, IoPolicySettings
from ace.sample.ops.specs import ADD_OP


def _provider_available(name, device):
    try:
        return get_library_impl(name, device=device).check_available()
    except Exception:
        return False


def _compile_and_run_online():
    """Compile ADD_OP, run online inference, save keys. Return all artifacts."""
    model = ADD_OP.create_model()
    x, y = tuple(t.clone() for t in ADD_OP.example_inputs)

    _C.set_key_path("")
    compiled = fhe.compile(
        frontend="torch",
        library="phantom",
        device="cuda",
        encrypt_inputs=ADD_OP.encrypt_inputs,
    )(model)
    prog = compiled.fhe_compile((x, y))

    runner = fhe.FHERuntime(prog.package)
    result_online = runner.run(x, y)

    # Save keys via the live runner (requires active FheContext — before finalize).
    key_dir = tempfile.mkdtemp(prefix="mode_combo_key_")
    rc = runner.save_keys(key_dir)
    assert rc == 0, f"save_keys failed rc={rc}"

    eval_dir = tempfile.mkdtemp(prefix="mode_combo_eval_")
    rc = runner.save_keys(eval_dir, eval_only=True)
    assert rc == 0, f"save_keys(eval_only) failed rc={rc}"

    # Offline weight encoding (OfflinePrep.encode_package) was retired — pending
    # a standalone encoding tool. No encoded_package produced here; the two
    # test_offline_encode_* cases are skipped (see their bodies).
    encoded_package = None

    runner.finalize()
    _C.clear_key_path()

    return {
        "prog": prog,
        "x": x,
        "y": y,
        "result_online": result_online,
        "key_dir": key_dir,
        "eval_dir": eval_dir,
        "encoded_package": encoded_package,
    }


def _assert_results_close(result_a, result_b, tol=1e-3):
    """Assert two inference results are numerically close."""
    a = result_a.detach().cpu().flatten().float()
    b = result_b.detach().cpu().flatten().float()
    assert a.shape == b.shape, f"Shape mismatch: {a.shape} vs {b.shape}"
    max_diff = (a - b).abs().max().item()
    assert max_diff < tol, f"Results differ (max_diff={max_diff})"


def _cleanup(artifacts):
    """Clean up temporary directories."""
    for key in ["key_dir", "eval_dir"]:
        d = artifacts.get(key)
        if d and os.path.exists(d):
            shutil.rmtree(d, ignore_errors=True)
    _C.clear_key_path()
    IoPolicySettings(policy="auto").apply()


@pytest.mark.skipif(
    not _provider_available("phantom", "cuda"),
    reason="phantom-cuda not available",
)
class TestModeCombinations:
    """Test key_mode x io_policy combinations for inference correctness."""

    def test_online_default(self, tmp_path):
        """Online mode (default) — baseline inference."""
        model = ADD_OP.create_model()
        x, y = tuple(t.clone() for t in ADD_OP.example_inputs)

        _C.set_key_path("")
        compiled = fhe.compile(
            frontend="torch",
            library="phantom",
            device="cuda",
            encrypt_inputs=ADD_OP.encrypt_inputs,
        )(model)
        prog = compiled.fhe_compile((x, y))

        runner = fhe.FHERuntime(prog.package)
        result = runner.run(x, y)
        runner.finalize()

        assert result is not None
        assert not torch.isnan(result).any()

    @pytest.mark.skip(
        reason="Hangs: PRELOAD_FULL (key_offline) galois-key load on tmpfs key paths — "
        "gds_parallel_read batched read dead-waits when cuFile Bar1 mapping is "
        "unavailable on /tmp; per-file reads fall back but the galois merged batched "
        "read does not. IO-layer (fhe_lib/common/io) GDS-fallback gap, not an API issue. "
        "Re-enable after the fallback fix or by pointing key_dir at real NVMe."
    )
    def test_key_offline_staged(self, tmp_path):
        """key_offline + staged IO policy."""
        artifacts = _compile_and_run_online()
        try:
            IoPolicySettings(policy="staged").apply()
            _C.set_key_path(artifacts["key_dir"])

            package = {
                "model": ADD_OP.name,
                "kernel": artifacts["prog"].package["kernel"],
                "config_path": artifacts["prog"].package.get("config_path", ""),
                "key_path": artifacts["key_dir"],
                "key_mode": "key_offline",
            }
            runner = fhe.FHERuntime(package)
            runner.init()
            result = runner.run(artifacts["x"].clone(), artifacts["y"].clone())
            runner.finalize()

            _assert_results_close(result, artifacts["result_online"])
        finally:
            _cleanup(artifacts)

    @pytest.mark.skip(
        reason="Hangs: PRELOAD_FULL galois-key load on tmpfs (gds_parallel_read batched read; see test_key_offline_staged skip note). IO-layer GDS-fallback gap."
    )
    def test_key_offline_auto(self, tmp_path):
        """key_offline + auto IO policy (GDS if available)."""
        artifacts = _compile_and_run_online()
        try:
            IoPolicySettings(policy="auto").apply()
            _C.set_key_path(artifacts["key_dir"])

            package = {
                "model": ADD_OP.name,
                "kernel": artifacts["prog"].package["kernel"],
                "config_path": artifacts["prog"].package.get("config_path", ""),
                "key_path": artifacts["key_dir"],
                "key_mode": "key_offline",
            }
            runner = fhe.FHERuntime(package)
            runner.init()
            result = runner.run(artifacts["x"].clone(), artifacts["y"].clone())
            runner.finalize()

            _assert_results_close(result, artifacts["result_online"])
        finally:
            _cleanup(artifacts)

    @pytest.mark.skip(
        reason="Hangs: PRELOAD (server_only) galois-key load on tmpfs (gds_parallel_read batched read; see test_key_offline_staged skip note). IO-layer GDS-fallback gap."
    )
    def test_server_only_staged(self, tmp_path):
        """server_only + staged IO policy — verify init succeeds."""
        artifacts = _compile_and_run_online()
        try:
            IoPolicySettings(policy="staged").apply()
            _C.set_key_path(artifacts["eval_dir"])

            package = {
                "model": ADD_OP.name,
                "kernel": artifacts["prog"].package["kernel"],
                "config_path": artifacts["prog"].package.get("config_path", ""),
                "key_path": artifacts["eval_dir"],
                "key_mode": "server_only",
            }
            runner = fhe.FHERuntime(package)
            runner.init()
            # server_only cannot encrypt, so we just verify init succeeded
            runner.finalize()
        finally:
            _cleanup(artifacts)

    def test_offline_encode_key_offline(self, tmp_path):
        """Offline encoding + key_offline mode."""
        artifacts = _compile_and_run_online()
        try:
            if artifacts["encoded_package"] is None:
                pytest.skip(
                    "Offline weight encoding is disabled (OfflinePrep.encode_package retired, "
                    "pending standalone encoding tool)"
                )

            IoPolicySettings(policy="staged").apply()
            _C.set_key_path(artifacts["key_dir"])

            package = {
                **artifacts["encoded_package"],
                "key_path": artifacts["key_dir"],
                "key_mode": "key_offline",
            }
            runner = fhe.FHERuntime(package)
            runner.init()
            result = runner.run(artifacts["x"].clone(), artifacts["y"].clone())
            runner.finalize()

            _assert_results_close(result, artifacts["result_online"])
        finally:
            _cleanup(artifacts)

    def test_offline_encode_server_only(self, tmp_path):
        """Offline encoding + server_only mode — verify init succeeds."""
        artifacts = _compile_and_run_online()
        try:
            if artifacts["encoded_package"] is None:
                pytest.skip(
                    "Offline weight encoding is disabled (OfflinePrep.encode_package retired, "
                    "pending standalone encoding tool)"
                )

            IoPolicySettings(policy="staged").apply()
            _C.set_key_path(artifacts["eval_dir"])

            package = {
                **artifacts["encoded_package"],
                "key_path": artifacts["eval_dir"],
                "key_mode": "server_only",
            }
            runner = fhe.FHERuntime(package)
            runner.init()
            # server_only cannot encrypt
            runner.finalize()
        finally:
            _cleanup(artifacts)
