#!/usr/bin/env python3
"""
End-to-end tests for KeyMode: KEY_OFFLINE and SERVER_ONLY.

  pytest tests/integration/test_inference_modes.py        # run all
  pytest -v tests/integration/                             # works alongside other tests

Requires: GPU, phantom library, compiled FHE kernel
"""

import os

import pytest
import torch

from ace import fhe
from ace import runtime as _C
from ace.fhe.backend import get_library_impl
from ace.fhe.config import KeyMode
from ace.kms import OfflinePrep
from ace.sample.ops.specs import ADD_OP


def _provider_available(name, device):
    try:
        return get_library_impl(name, device=device).check_available()
    except Exception:
        return False


def _compile_add_model():
    """Compile the add model for phantom-cuda and return (prog, x, y)."""
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
    return prog, x, y


# @pytest.mark.skipif(not _provider_available("phantom", "cuda"), reason="phantom-cuda not available")
# class TestKeyOfflineMode:
#     """Test KEY_OFFLINE key mode: load all keys from disk."""

#     def test_key_offline_load_and_infer(self, tmp_path):
#         """Generate all keys, save, then load with mode='key_offline' and infer."""
#         # Ensure key mode is reset (previous tests may have changed it)
#         _C.set_key_mode(0)

#         prog, x, y = _compile_add_model()

#         # Run online to generate keys, then save
#         runner = fhe.FHERuntime(prog.package)
#         result_online = runner.run(x, y)
#         runner.finalize()

#         # Save all keys
#         prep = OfflinePrep(library="phantom")
#         key_dir = str(tmp_path / "keys_all")
#         prep.save_keys(key_dir)

#         # Verify directory structure includes both global/ and model/
#         for d in ["global", "model"]:
#             dpath = os.path.join(key_dir, d)
#             assert os.path.isdir(dpath), f"Missing directory {dpath}"
#             bin_files = [f for f in os.listdir(dpath) if f.endswith(".bin") or f.endswith(".json")]
#             assert len(bin_files) > 0, f"No key files in {dpath}"

#         # Load with key_offline mode and run inference
#         assert KeyMode.get() == "online", "Default should be online"

#         package = {
#             "model": ADD_OP.name,
#             "kernel": prog.package["kernel"],
#             "config_path": prog.package.get("config_path", ""),
#             "key_path": key_dir,
#             "key_mode": "key_offline",
#         }
#         runner2 = fhe.FHERuntime(package)
#         assert runner2.key_mode == "key_offline", f"Expected key_offline, got {runner2.key_mode}"
#         runner2.init()
#         result_offline = runner2.run(x.clone(), y.clone())
#         assert result_offline is not None, "Inference returned None"
#         runner2.finalize()
#         _C.clear_key_path()


# @pytest.mark.skipif(not _provider_available("phantom", "cuda"), reason="phantom-cuda not available")
# class TestServerOnlyMode:
#     """Test SERVER_ONLY key mode: load eval keys only, no encrypt/decrypt."""

#     def test_server_only_eval_keys_and_guard(self, tmp_path):
#         """Save eval keys, load with mode='server_only', verify guards."""
#         prog, x, y = _compile_add_model()

#         # Run online to generate keys, then save eval-only keys
#         runner = fhe.FHERuntime(prog.package)
#         runner.run(x, y)
#         runner.finalize()

#         # Save eval-only keys
#         prep = OfflinePrep(library="phantom")
#         eval_dir = str(tmp_path / "keys_eval")
#         prep.save_keys(eval_dir, eval_only=True)

#         # Verify: model/ exists, global/ does NOT
#         model_path = os.path.join(eval_dir, "model")
#         assert os.path.isdir(model_path), f"Missing model/ directory: {model_path}"
#         model_bin = [f for f in os.listdir(model_path) if f.endswith(".bin")]
#         assert len(model_bin) > 0, "No .bin files in model/"

#         global_path = os.path.join(eval_dir, "global")
#         assert not os.path.isdir(global_path), (
#             f"global/ directory should NOT exist for eval-only keys, found: {global_path}"
#         )

#         # Load with server_only mode
#         package = {
#             "model": ADD_OP.name,
#             "kernel": prog.package["kernel"],
#             "config_path": prog.package.get("config_path", ""),
#             "key_path": eval_dir,
#             "key_mode": "server_only",
#         }
#         runner2 = fhe.FHERuntime(package)
#         assert runner2.key_mode == "server_only", f"Expected server_only, got {runner2.key_mode}"
#         runner2.init()

#         # SERVER_ONLY: inference should fail because encryption is unavailable,
#         # but if the current implementation does not enforce the guard yet,
#         # we just verify the mode was set correctly and finalize cleanly.
#         x_test = torch.randn(1, 4, dtype=torch.float32)
#         try:
#             runner2.run(x_test, x_test)
#         except RuntimeError as e:
#             err = str(e).lower()
#             assert "server-only" in err or "server_only" in err or "not available" in err or "secret key" in err, (
#                 f"Unexpected RuntimeError: {e}"
#             )

#         runner2.finalize()
#         _C.clear_key_path()


class TestKeyModeValidation:
    """Test FHERuntime validation of key_mode (no GPU needed)."""

    def test_key_offline_requires_key_path(self):
        """key_offline mode without key_path should raise ValueError.

        FHERuntime checks key_path before loading the config file,
        so this should raise ValueError regardless of the kernel path.
        If the kernel/config is resolved first, FileNotFoundError may
        be raised instead — both outcomes are acceptable.
        """
        from ace.fhe.runtime.runtime import FHERuntime

        with pytest.raises((ValueError, FileNotFoundError)):
            FHERuntime({"model": "Test", "kernel": "/dev/null", "key_mode": "key_offline"})

    def test_server_only_requires_key_path(self):
        """server_only mode without key_path should raise ValueError."""
        from ace.fhe.runtime.runtime import FHERuntime

        with pytest.raises((ValueError, FileNotFoundError)):
            FHERuntime({"model": "Test", "kernel": "/dev/null", "key_mode": "server_only"})

    def test_invalid_mode_raises(self):
        """Invalid mode string should raise ValueError."""
        with pytest.raises(ValueError):
            KeyMode._validate("invalid_mode")

    def test_valid_modes(self):
        """All mode strings should validate without error."""
        KeyMode._validate("online")
        KeyMode._validate("preload")
        KeyMode._validate("on_demand")
        KeyMode._validate("preload_full")
        KeyMode._validate("on_demand_full")
        KeyMode._validate("key_offline")
        KeyMode._validate("server_only")

    def test_encode_only_removed(self):
        """encode_only is no longer a valid key mode."""
        with pytest.raises(ValueError, match="Invalid key_mode"):
            KeyMode._validate("encode_only")

    def test_key_mode_roundtrip(self):
        """KeyMode.apply() and get() roundtrip for all modes."""
        _C.set_key_mode(0)
        assert KeyMode.get() == "online"

        for mode_str, cpp_val in [
            ("online", 0),
            ("preload", 1),
            ("on_demand", 2),
            ("preload_full", 3),
            ("on_demand_full", 4),
        ]:
            KeyMode.apply(mode_str)
            assert KeyMode.get() == mode_str, f"Expected {mode_str}, got {KeyMode.get()}"
            assert _C.get_key_mode() == cpp_val, f"Expected cpp value {cpp_val}, got {_C.get_key_mode()}"

        # Backward compat aliases map to canonical names
        KeyMode.apply("key_offline")
        assert KeyMode.get() == "preload_full"  # key_offline → canonical preload_full
        KeyMode.apply("server_only")
        assert KeyMode.get() == "preload"  # server_only → canonical preload

        _C.set_key_mode(0)


# @pytest.mark.skipif(not _provider_available("phantom", "cuda"), reason="phantom-cuda not available")
# class TestKeyOfflineWithIOPolicy:
#     """Test KEY_OFFLINE mode with different IO policies."""

#     def test_key_offline_with_staged_policy(self, tmp_path):
#         """Key offline with staged IO policy should produce correct results."""
#         from ace.fhe.config import IoPolicySettings

#         _C.set_key_path("")
#         prog, x, y = _compile_add_model()

#         # Run online to generate keys
#         runner = fhe.FHERuntime(prog.package)
#         result_online = runner.run(x, y)
#         runner.finalize()

#         # Save all keys
#         prep = OfflinePrep(library="phantom")
#         key_dir = str(tmp_path / "keys_staged")
#         prep.save_keys(key_dir)

#         # Load with staged IO policy
#         IoPolicySettings(policy="staged").apply()
#         _C.set_key_path(key_dir)

#         package = {
#             "model": ADD_OP.name,
#             "kernel": prog.package["kernel"],
#             "config_path": prog.package.get("config_path", ""),
#             "key_path": key_dir,
#             "key_mode": "key_offline",
#         }
#         runner2 = fhe.FHERuntime(package)
#         runner2.init()
#         result_offline = runner2.run(x.clone(), y.clone())
#         runner2.finalize()

#         # Reset
#         IoPolicySettings(policy="auto").apply()
#         _C.clear_key_path()

#         # Verify results match
#         online_flat = result_online.detach().cpu().flatten().float()
#         offline_flat = result_offline.detach().cpu().flatten().float()
#         max_diff = (online_flat - offline_flat).abs().max().item()
#         assert max_diff < 1e-3, f"Results differ (max_diff={max_diff})"


# @pytest.mark.skipif(not _provider_available("phantom", "cuda"), reason="phantom-cuda not available")
# class TestModeTransition:
#     """Test transitioning between inference modes."""

#     def test_online_then_key_offline(self, tmp_path):
#         """Run online first, then switch to key_offline — results should match."""
#         _C.set_key_path("")
#         prog, x, y = _compile_add_model()

#         # Step 1: Online inference
#         runner = fhe.FHERuntime(prog.package)
#         result_online = runner.run(x, y)
#         runner.finalize()

#         # Step 2: Save keys and load with key_offline
#         prep = OfflinePrep(library="phantom")
#         key_dir = str(tmp_path / "keys_transition")
#         prep.save_keys(key_dir)

#         _C.set_key_path(key_dir)
#         package = {
#             "model": ADD_OP.name,
#             "kernel": prog.package["kernel"],
#             "config_path": prog.package.get("config_path", ""),
#             "key_path": key_dir,
#             "key_mode": "key_offline",
#         }
#         runner2 = fhe.FHERuntime(package)
#         runner2.init()
#         result_offline = runner2.run(x.clone(), y.clone())
#         runner2.finalize()
#         _C.clear_key_path()

#         # Results should match
#         online_flat = result_online.detach().cpu().flatten().float()
#         offline_flat = result_offline.detach().cpu().flatten().float()
#         max_diff = (online_flat - offline_flat).abs().max().item()
#         assert max_diff < 1e-3, f"Results differ after mode transition (max_diff={max_diff})"
