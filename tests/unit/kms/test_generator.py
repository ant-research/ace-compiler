# tests/unit/kms/test_generator.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Unit tests for KMS generator module.

Tests generate_encryption_keys() and generate_evaluation_keys() API.
"""

import json
from pathlib import Path

import pytest

from ace.kms.generator import generate_encryption_keys, generate_evaluation_keys


# =============================================================================
# generate_encryption_keys
# =============================================================================


class TestGenerateEncryptionKeys:
    """Tests for generate_encryption_keys()."""

    def test_basic_generation(self, tmp_path):
        output = tmp_path / "enc-keys"
        bundle = generate_encryption_keys(
            ckks_params={"N": 65536, "q0": 60, "sf": 56},
            output_dir=output,
            key_id="test-enc",
        )
        assert bundle.key_id == "test-enc"
        assert bundle.key_type == "encryption"
        assert output.exists()
        assert (output / "metadata.json").exists()
        assert (output / "global").is_dir()

    def test_metadata_content(self, tmp_path):
        output = tmp_path / "enc-keys"
        generate_encryption_keys(
            ckks_params={"N": 65536, "q0": 60, "sf": 56},
            output_dir=output,
            key_id="test-enc",
            description="Test encryption keys",
        )
        with open(output / "metadata.json") as f:
            meta = json.load(f)
        assert meta["key_id"] == "test-enc"
        assert meta["key_type"] == "encryption"
        assert meta["ckks_params"]["N"] == 65536
        assert meta["ckks_params"]["q0"] == 60
        assert meta["ckks_params"]["sf"] == 56
        assert meta["description"] == "Test encryption keys"
        assert "secret_key" in meta["key_files"]
        assert "public_key" in meta["key_files"]

    def test_auto_key_id(self, tmp_path):
        output = tmp_path / "enc-keys"
        bundle = generate_encryption_keys(
            ckks_params={"N": 65536, "q0": 60, "sf": 56},
            output_dir=output,
        )
        assert bundle.key_id.startswith("enc-")

    def test_optional_params(self, tmp_path):
        output = tmp_path / "enc-keys"
        bundle = generate_encryption_keys(
            ckks_params={"N": 65536, "q0": 60, "sf": 56, "hamming_weight": 128, "sec_level": 1},
            output_dir=output,
            key_id="custom-params",
        )
        assert bundle.metadata.ckks_params["hamming_weight"] == 128
        assert bundle.metadata.ckks_params["sec_level"] == 1

    def test_missing_required_params(self, tmp_path):
        with pytest.raises(ValueError, match="Missing required CKKS parameters"):
            generate_encryption_keys(
                ckks_params={"N": 65536},  # missing q0, sf
                output_dir=tmp_path / "bad",
            )

    def test_placeholder_files_created(self, tmp_path):
        output = tmp_path / "enc-keys"
        bundle = generate_encryption_keys(
            ckks_params={"N": 65536, "q0": 60, "sf": 56},
            output_dir=output,
            key_id="placeholder-test",
        )
        # Placeholder files should exist
        assert bundle.secret_key_path.exists()
        assert bundle.public_key_path.exists()
        # They should contain placeholder text, not be empty
        sk_content = bundle.secret_key_path.read_text()
        assert "PLACEHOLDER" in sk_content

    def test_existing_dir_with_metadata(self, tmp_path):
        output = tmp_path / "existing"
        # Create first
        bundle1 = generate_encryption_keys(
            ckks_params={"N": 8192, "q0": 60, "sf": 56},
            output_dir=output,
            key_id="first",
        )
        # Overwrite with same dir
        bundle2 = generate_encryption_keys(
            ckks_params={"N": 65536, "q0": 60, "sf": 56},
            output_dir=output,
            key_id="second",
        )
        # Should use the existing key_id from metadata
        assert bundle2.key_id == "first"


# =============================================================================
# generate_evaluation_keys
# =============================================================================


class TestGenerateEvaluationKeys:
    """Tests for generate_evaluation_keys()."""

    def _make_conf(self, tmp_path, context_params=None):
        """Create a .conf file for testing."""
        if context_params is None:
            context_params = {
                "_poly_degree": 65536,
                "_first_mod_size": 60,
                "_scaling_mod_size": 56,
                "_mul_depth": 31,
                "_num_rot_idx": 3,
                "_rot_idxs": [1, 2, 3],
                "_hamming_weight": 192,
            }
        conf = tmp_path / "model.conf"
        tmp_path.mkdir(parents=True, exist_ok=True)
        conf.write_text(json.dumps({"context_params": context_params}))
        return tmp_path

    def test_basic_generation(self, tmp_path):
        compiled_dir = self._make_conf(tmp_path / "compiled")
        output = tmp_path / "eval-keys"
        bundle = generate_evaluation_keys(
            compiled_dir=compiled_dir,
            output_dir=output,
            key_id="test-eval",
        )
        assert bundle.key_id == "test-eval"
        assert bundle.key_type == "evaluation"
        assert (output / "metadata.json").exists()
        assert (output / "model").is_dir()

    def test_metadata_content(self, tmp_path):
        compiled_dir = self._make_conf(tmp_path / "compiled")
        output = tmp_path / "eval-keys"
        generate_evaluation_keys(
            compiled_dir=compiled_dir,
            output_dir=output,
            key_id="test-eval",
        )
        with open(output / "metadata.json") as f:
            meta = json.load(f)
        assert meta["key_type"] == "evaluation"
        assert meta["ckks_params"]["N"] == 65536
        assert meta["ckks_params"]["mul_depth"] == 31
        assert meta["ckks_params"]["rot_idxs"] == [1, 2, 3]
        assert "relin_key" in meta["key_files"]
        assert "galois_key" in meta["key_files"]
        assert "bootstrap_key" in meta["key_files"]  # mul_depth > 0

    def test_no_bootstrap_for_zero_depth(self, tmp_path):
        ctx = {
            "_poly_degree": 8,
            "_first_mod_size": 60,
            "_scaling_mod_size": 56,
            "_mul_depth": 0,
            "_rot_idxs": [],
        }
        compiled_dir = self._make_conf(tmp_path / "compiled", context_params=ctx)
        output = tmp_path / "eval-keys"
        generate_evaluation_keys(
            compiled_dir=compiled_dir,
            output_dir=output,
            key_id="no-bootstrap",
        )
        with open(output / "metadata.json") as f:
            meta = json.load(f)
        assert "bootstrap_key" not in meta["key_files"]

    def test_model_name_from_conf(self, tmp_path):
        compiled_dir = self._make_conf(tmp_path / "compiled")
        output = tmp_path / "eval-keys"
        bundle = generate_evaluation_keys(
            compiled_dir=compiled_dir,
            output_dir=output,
            key_id="test-model-name",
        )
        # Model name should be derived from the .conf file stem
        assert bundle.metadata.model_name == "model"

    def test_nonexistent_compiled_dir(self, tmp_path):
        with pytest.raises(FileNotFoundError, match="Compiled directory not found"):
            generate_evaluation_keys(
                compiled_dir=tmp_path / "nonexistent",
                output_dir=tmp_path / "output",
            )

    def test_no_conf_file(self, tmp_path):
        compiled_dir = tmp_path / "empty"
        compiled_dir.mkdir()
        with pytest.raises(FileNotFoundError, match=r"No \.conf file found"):
            generate_evaluation_keys(
                compiled_dir=compiled_dir,
                output_dir=tmp_path / "output",
            )


class TestGenerateEvaluationKeysEdgeCases:
    """Edge case tests for generate_evaluation_keys()."""

    def _make_conf(self, tmp_path, context_params=None):
        if context_params is None:
            context_params = {
                "_poly_degree": 65536,
                "_first_mod_size": 60,
                "_scaling_mod_size": 56,
                "_mul_depth": 31,
                "_num_rot_idx": 3,
                "_rot_idxs": [1, 2, 3],
                "_hamming_weight": 192,
            }
        conf = tmp_path / "model.conf"
        tmp_path.mkdir(parents=True, exist_ok=True)
        conf.write_text(json.dumps({"context_params": context_params}))
        return tmp_path

    def test_many_rotation_indices(self, tmp_path):
        """Generate with a large number of rotation indices."""
        ctx = {
            "_poly_degree": 65536,
            "_first_mod_size": 60,
            "_scaling_mod_size": 56,
            "_mul_depth": 5,
            "_num_rot_idx": 50,
            "_rot_idxs": list(range(50)),
            "_hamming_weight": 192,
        }
        compiled_dir = self._make_conf(tmp_path / "compiled", context_params=ctx)
        output = tmp_path / "eval-keys"
        bundle = generate_evaluation_keys(
            compiled_dir=compiled_dir,
            output_dir=output,
            key_id="many-rot",
        )
        with open(output / "metadata.json") as f:
            meta = json.load(f)
        assert len(meta["ckks_params"]["rot_idxs"]) == 50

    def test_empty_rotation_indices(self, tmp_path):
        """Generate with no rotation indices."""
        ctx = {
            "_poly_degree": 65536,
            "_first_mod_size": 60,
            "_scaling_mod_size": 56,
            "_mul_depth": 0,
            "_rot_idxs": [],
        }
        compiled_dir = self._make_conf(tmp_path / "compiled", context_params=ctx)
        output = tmp_path / "eval-keys"
        bundle = generate_evaluation_keys(
            compiled_dir=compiled_dir,
            output_dir=output,
            key_id="no-rot",
        )
        with open(output / "metadata.json") as f:
            meta = json.load(f)
        assert meta["ckks_params"]["rot_idxs"] == []

    def test_corrupt_conf_file(self, tmp_path):
        """Corrupt .conf file (invalid JSON) should raise JSONDecodeError."""
        compiled_dir = tmp_path / "compiled"
        compiled_dir.mkdir(parents=True)
        (compiled_dir / "model.conf").write_text("not valid json {{{")
        output = tmp_path / "eval-keys"
        with pytest.raises(json.JSONDecodeError):
            generate_evaluation_keys(
                compiled_dir=compiled_dir,
                output_dir=output,
            )

    def test_large_hamming_weight(self, tmp_path):
        """Unusual but valid hamming_weight should be preserved."""
        ctx = {
            "_poly_degree": 65536,
            "_first_mod_size": 60,
            "_scaling_mod_size": 56,
            "_mul_depth": 5,
            "_rot_idxs": [1],
            "_hamming_weight": 32768,
        }
        compiled_dir = self._make_conf(tmp_path / "compiled", context_params=ctx)
        output = tmp_path / "eval-keys"
        generate_evaluation_keys(
            compiled_dir=compiled_dir,
            output_dir=output,
            key_id="large-hw",
        )
        with open(output / "metadata.json") as f:
            meta = json.load(f)
        assert meta["ckks_params"]["hamming_weight"] == 32768
