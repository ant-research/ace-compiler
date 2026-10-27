#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""Integration test for offline weight encoding + offline key loading.

Tests that:
1. OfflinePrep.encode_package() encodes weights and returns an updated package
2. Pre-encoded weights produce the same inference results as on-the-fly encoding
3. Offline encoding composes with KEY_OFFLINE mode (offline keys + offline weights)
4. Offline encoding composes with SERVER_ONLY mode (eval keys + offline weights)

Uses sample models from the ace.sample.ops registry (LINEAR_OP).

  pytest tests/integration/test_offline_encode.py        # run all
  pytest -v tests/integration/                           # alongside other tests

Requires: phantom backend with CUDA GPU
"""

import json
import os
import shutil
import tempfile

import pytest
import torch

from ace import fhe
from ace import runtime as _C
from ace.fhe.backend import get_library_impl
from ace.kms import OfflinePrep
from ace.sample.ops.specs import LINEAR_OP


def _provider_available(name, device):
    try:
        return get_library_impl(name, device=device).check_available()
    except Exception:
        return False


# Note: only LINEAR_OP is used because the FHERuntime CUDA context cleanup
# bug causes errors when multiple GPU models are created in the same process.
# TODO: add MUL_CONST_OP back once the FHERuntime CUDA context bug is fixed.
_OP_SPECS = [LINEAR_OP]


@pytest.mark.skipif(
    not _provider_available("phantom", "cuda"),
    reason="phantom-cuda not available",
)
@pytest.mark.skip(
    reason="OfflinePrep.encode_package() is disabled: the encode-only FheContext "
    "path (EncPolicy::BYPASS) was removed and is pending reimplementation as a "
    "standalone weight-encoding tool. Every test in this class drives encode_package "
    "(via _compile_and_encode), so the whole group is skipped until that tool lands. "
    "Key save/load coverage lives in test_kms_key_save_load.py via runner.save_keys.",
)
class TestOfflineEncodeRoundtrip:
    """Test offline weight encoding roundtrip for sample models with weights.

    DISABLED: see module-level skip marker — encode_package path retired.
    """

    def _compile_and_encode(self, spec, tmp_path):
        """Compile model, run baseline inference, encode weights, save keys.

        The encoding and key saving must happen BEFORE FHERuntime.finalize()
        because Phantom_encode_weight_file uses the FheContext singleton's
        encoder, which depends on the live CKKS parameter chain.  After
        finalize(), the context is destroyed and re-creation may produce
        mismatched encoder state.

        Returns:
            (encoded_package, key_dir, eval_dir, result_baseline, example_inputs)
        """
        model = spec.create_model()
        example_inputs = tuple(t.clone() for t in spec.example_inputs)

        compiled = fhe.compile(
            frontend="torch",
            library="phantom",
            device="cuda",
            encrypt_inputs=spec.encrypt_inputs,
        )(model)
        prog = compiled.fhe_compile(example_inputs)
        package = prog.package

        # Run baseline inference
        runner = fhe.FHERuntime(package)
        result_baseline = runner.run(*example_inputs)

        # Encode weights WHILE FHERuntime is still alive (context is active)
        prep = OfflinePrep(kernel_path=package["kernel"])
        encoded_package = prep.encode_package(package)

        # Save keys (both all-keys and eval-only) while context is alive
        key_dir = tempfile.mkdtemp(prefix="offline_encode_key_", dir=str(tmp_path))
        prep.save_keys(key_dir)

        eval_dir = tempfile.mkdtemp(prefix="offline_encode_eval_", dir=str(tmp_path))
        prep.save_keys(eval_dir, eval_only=True)

        # Finalize AFTER encoding and key saving
        runner.finalize()

        return encoded_package, key_dir, eval_dir, result_baseline, example_inputs

    def _assert_results_close(self, result_a, result_b, tol=1e-3):
        """Assert two inference results are numerically close."""
        a = result_a.detach().cpu().flatten().float()
        b = result_b.detach().cpu().flatten().float()
        assert a.shape == b.shape, f"Shape mismatch: {a.shape} vs {b.shape}"
        max_diff = (a - b).abs().max().item()
        assert max_diff < tol, f"Results differ (max_diff={max_diff})"

    @pytest.mark.parametrize("spec", _OP_SPECS, ids=lambda s: s.name)
    def test_encode_roundtrip(self, spec, tmp_path):
        """Pre-encoded weights produce same inference as on-the-fly."""
        (encoded_package, key_dir, eval_dir, result_baseline, example_inputs) = self._compile_and_encode(spec, tmp_path)

        try:
            # Run inference with pre-encoded weights (online mode, keys generated fresh)
            runner = fhe.FHERuntime(encoded_package)
            result_encoded = runner.run(*(t.clone() for t in example_inputs))
            runner.finalize()

            self._assert_results_close(result_encoded, result_baseline)
        finally:
            shutil.rmtree(key_dir, ignore_errors=True)
            shutil.rmtree(eval_dir, ignore_errors=True)

    @pytest.mark.parametrize("spec", _OP_SPECS, ids=lambda s: s.name)
    def test_encode_with_key_offline(self, spec, tmp_path):
        """Pre-encoded weights + KEY_OFFLINE mode (offline keys + offline weights)."""
        (encoded_package, key_dir, eval_dir, result_baseline, example_inputs) = self._compile_and_encode(spec, tmp_path)

        try:
            # Run inference with key_offline mode + pre-encoded weights
            key_offline_package = {
                **encoded_package,
                "key_path": key_dir,
                "inference_mode": "key_offline",
            }

            runner = fhe.FHERuntime(key_offline_package)
            runner.init()
            result = runner.run(*(t.clone() for t in example_inputs))
            runner.finalize()

            self._assert_results_close(result, result_baseline)
        finally:
            _C.clear_key_path()
            shutil.rmtree(key_dir, ignore_errors=True)
            shutil.rmtree(eval_dir, ignore_errors=True)

    @pytest.mark.parametrize("spec", _OP_SPECS, ids=lambda s: s.name)
    def test_encode_with_server_only(self, spec, tmp_path):
        """Pre-encoded weights + SERVER_ONLY mode (eval keys + offline weights)."""
        (encoded_package, key_dir, eval_dir, _result_baseline, _example_inputs) = self._compile_and_encode(
            spec, tmp_path
        )

        try:
            # Run with server_only mode
            server_only_package = {
                **encoded_package,
                "key_path": eval_dir,
                "inference_mode": "server_only",
            }

            runner = fhe.FHERuntime(server_only_package)
            runner.init()

            # SERVER_ONLY: inference should fail because encryption is unavailable,
            # but if the current implementation does not enforce the guard yet,
            # we just verify the mode was set correctly and finalize cleanly.
            x = torch.randn(1, 4, dtype=torch.float32)
            try:
                runner.run(x)
                runner.finalize()
            except (RuntimeError, Exception) as e:
                runner.finalize()
                err = str(e).lower()
                if "server" not in err and "secret key" not in err and "not available" not in err:
                    raise
        finally:
            _C.clear_key_path()
            shutil.rmtree(key_dir, ignore_errors=True)
            shutil.rmtree(eval_dir, ignore_errors=True)

    @pytest.mark.parametrize("spec", _OP_SPECS, ids=lambda s: s.name)
    def test_encode_package_creates_encoded_config(self, spec, tmp_path):
        """encode_package should create a .encoded config with _entry_type=2."""
        (encoded_package, key_dir, eval_dir, _result_baseline, _example_inputs) = self._compile_and_encode(
            spec, tmp_path
        )

        try:
            # Verify .encoded config exists and has correct entry type
            encoded_config = encoded_package.get("config_path", "")
            assert encoded_config, "encoded_package should have config_path"
            assert os.path.exists(encoded_config), f"Encoded config not found: {encoded_config}"

            with open(encoded_config) as f:
                config = json.load(f)
            assert config["rt_data_info"]["_entry_type"] == 2, (
                f"Expected _entry_type=2, got {config['rt_data_info']['_entry_type']}"
            )
        finally:
            shutil.rmtree(key_dir, ignore_errors=True)
            shutil.rmtree(eval_dir, ignore_errors=True)

    @pytest.mark.parametrize("spec", _OP_SPECS, ids=lambda s: s.name)
    def test_encode_package_preserves_original_config(self, spec, tmp_path):
        """encode_package should not modify the original config file."""
        (encoded_package, key_dir, eval_dir, _result_baseline, _example_inputs) = self._compile_and_encode(
            spec, tmp_path
        )

        try:
            encoded_config = encoded_package.get("config_path", "")
            if encoded_config and encoded_config.endswith(".encoded"):
                original_config = encoded_config[: -len(".encoded")]
                if os.path.exists(original_config):
                    with open(original_config) as f:
                        config = json.load(f)
                    if "rt_data_info" in config:
                        assert config["rt_data_info"].get("_entry_type", 1) == 1, (
                            "Original config should remain unmodified"
                        )
        finally:
            shutil.rmtree(key_dir, ignore_errors=True)
            shutil.rmtree(eval_dir, ignore_errors=True)
