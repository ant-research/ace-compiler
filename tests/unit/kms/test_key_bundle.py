# tests/unit/kms/test_key_bundle.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Unit tests for KMS key_bundle module.

Tests KeyBundle, KeyBundleMetadata, and metadata factory functions.
"""

import json
from pathlib import Path

import pytest

from ace.kms.config_reader import CKKSParams
from ace.kms.key_bundle import (
    KeyBundle,
    KeyBundleMetadata,
    make_encryption_metadata,
    make_evaluation_metadata,
)


# =============================================================================
# KeyBundleMetadata
# =============================================================================


class TestKeyBundleMetadata:
    """Tests for KeyBundleMetadata dataclass."""

    def test_encryption_metadata_to_dict(self):
        meta = KeyBundleMetadata(
            key_id="test-enc",
            created_at="2026-06-05T00:00:00Z",
            key_type="encryption",
            ckks_params={"N": 65536, "q0": 60, "sf": 56},
            key_files={"secret_key": "global/secret_key.bin", "public_key": "global/public_key.bin"},
        )
        d = meta.to_dict()
        assert d["key_id"] == "test-enc"
        assert d["key_type"] == "encryption"
        assert d["ckks_params"]["N"] == 65536
        assert "model_name" not in d  # None fields omitted

    def test_evaluation_metadata_to_dict(self):
        meta = KeyBundleMetadata(
            key_id="test-eval",
            created_at="2026-06-05T00:00:00Z",
            key_type="evaluation",
            ckks_params={"N": 65536, "q0": 60, "sf": 56},
            model_name="resnet20",
            conf_source="/path/to/model.conf",
            key_files={"relin_key": "model/relin_key.bin"},
        )
        d = meta.to_dict()
        assert d["model_name"] == "resnet20"
        assert d["conf_source"] == "/path/to/model.conf"

    def test_from_dict_roundtrip(self):
        original = KeyBundleMetadata(
            key_id="roundtrip",
            created_at="2026-06-05T00:00:00Z",
            key_type="encryption",
            ckks_params={"N": 8192, "q0": 60, "sf": 56},
            key_files={"secret_key": "global/secret_key.bin"},
            description="test",
        )
        restored = KeyBundleMetadata.from_dict(original.to_dict())
        assert restored.key_id == original.key_id
        assert restored.key_type == original.key_type
        assert restored.ckks_params == original.ckks_params
        assert restored.description == original.description

    def test_from_dict_minimal(self):
        d = {
            "key_id": "minimal",
            "created_at": "2026-06-05T00:00:00Z",
            "key_type": "encryption",
            "ckks_params": {"N": 8192, "q0": 60, "sf": 56},
        }
        meta = KeyBundleMetadata.from_dict(d)
        assert meta.model_name is None
        assert meta.conf_source is None
        assert meta.key_files == {}
        assert meta.description == ""


# =============================================================================
# make_encryption_metadata / make_evaluation_metadata
# =============================================================================


class TestMakeMetadata:
    """Tests for metadata factory functions."""

    def test_make_encryption_metadata(self):
        params = CKKSParams(N=65536, q0=60, sf=56)
        meta = make_encryption_metadata(params, key_id="enc-test")
        assert meta.key_id == "enc-test"
        assert meta.key_type == "encryption"
        assert meta.ckks_params["N"] == 65536
        assert "secret_key" in meta.key_files
        assert "public_key" in meta.key_files

    def test_make_encryption_metadata_auto_id(self):
        params = CKKSParams(N=65536, q0=60, sf=56)
        meta = make_encryption_metadata(params)
        assert meta.key_id.startswith("enc-")
        assert meta.created_at  # non-empty timestamp

    def test_make_evaluation_metadata(self):
        params = CKKSParams(N=65536, q0=60, sf=56, mul_depth=31)
        meta = make_evaluation_metadata(
            params,
            model_name="resnet20",
            conf_source="/path/to/model.conf",
            key_id="eval-test",
        )
        assert meta.key_id == "eval-test"
        assert meta.key_type == "evaluation"
        assert meta.model_name == "resnet20"
        assert meta.conf_source == "/path/to/model.conf"
        assert "relin_key" in meta.key_files
        assert "galois_key" in meta.key_files
        assert "bootstrap_key" in meta.key_files  # mul_depth > 0

    def test_make_evaluation_metadata_no_bootstrap(self):
        params = CKKSParams(N=8192, q0=60, sf=56, mul_depth=0)
        meta = make_evaluation_metadata(params, key_id="eval-no-bootstrap")
        assert "relin_key" in meta.key_files
        assert "galois_key" in meta.key_files
        assert "bootstrap_key" not in meta.key_files  # mul_depth == 0

    def test_make_evaluation_metadata_auto_id(self):
        params = CKKSParams(N=65536, q0=60, sf=56, mul_depth=31)
        meta = make_evaluation_metadata(params)
        assert meta.key_id.startswith("eval-")


# =============================================================================
# KeyBundle
# =============================================================================


class TestKeyBundle:
    """Tests for KeyBundle dataclass."""

    def _make_bundle(self, tmp_path, key_type="encryption"):
        bundle_path = tmp_path / "test-bundle"
        bundle_path.mkdir()
        if key_type == "encryption":
            (bundle_path / "global").mkdir()
        else:
            (bundle_path / "model").mkdir()

        params = CKKSParams(N=65536, q0=60, sf=56, mul_depth=31)
        if key_type == "encryption":
            meta = make_encryption_metadata(params, key_id="test")
        else:
            meta = make_evaluation_metadata(params, key_id="test", model_name="resnet20")

        return KeyBundle(path=bundle_path, metadata=meta)

    def test_encryption_bundle_paths(self, tmp_path):
        bundle = self._make_bundle(tmp_path, "encryption")
        assert bundle.secret_key_path == bundle.path / "global" / "secret_key.bin"
        assert bundle.public_key_path == bundle.path / "global" / "public_key.bin"

    def test_evaluation_bundle_paths(self, tmp_path):
        bundle = self._make_bundle(tmp_path, "evaluation")
        assert bundle.relin_key_path == bundle.path / "model" / "relin_key.bin"
        assert bundle.galois_key_path == bundle.path / "model" / "galois_key.bin"
        assert bundle.bootstrap_key_path == bundle.path / "model" / "bootstrap_key.bin"

    def test_key_file_path(self, tmp_path):
        bundle = self._make_bundle(tmp_path, "encryption")
        assert bundle.key_file_path("secret_key") == bundle.secret_key_path
        assert bundle.key_file_path("public_key") == bundle.public_key_path

    def test_key_file_path_invalid(self, tmp_path):
        bundle = self._make_bundle(tmp_path, "encryption")
        with pytest.raises(ValueError, match="Unknown key name"):
            bundle.key_file_path("invalid_key")

    def test_has_encryption_keys(self, tmp_path):
        bundle = self._make_bundle(tmp_path, "encryption")
        assert not bundle.has_encryption_keys()  # no files written yet
        # Write placeholder files
        bundle.secret_key_path.write_text("sk")
        bundle.public_key_path.write_text("pk")
        assert bundle.has_encryption_keys()

    def test_has_evaluation_keys(self, tmp_path):
        bundle = self._make_bundle(tmp_path, "evaluation")
        assert not bundle.has_evaluation_keys()
        bundle.relin_key_path.write_text("rk")
        bundle.galois_key_path.write_text("gk")
        assert bundle.has_evaluation_keys()

    def test_summary(self, tmp_path):
        bundle = self._make_bundle(tmp_path, "encryption")
        bundle.secret_key_path.parent.mkdir(parents=True, exist_ok=True)
        bundle.secret_key_path.write_text("secret-key-placeholder")
        bundle.public_key_path.write_text("public-key-placeholder")

        summary = bundle.summary()
        assert summary["key_id"] == "test"
        assert summary["key_type"] == "encryption"
        assert "key_files" in summary
        assert "secret_key" in summary["key_files"]
        assert summary["key_files"]["secret_key"]["exists"] is True
        assert summary["key_files"]["secret_key"]["size_bytes"] > 0


class TestKeyBundleEdgeCases:
    """Edge case tests for KeyBundle."""

    def test_bundle_with_nonexistent_path(self, tmp_path):
        """Bundle with a path that doesn't exist on disk should still construct."""
        bundle_path = tmp_path / "does-not-exist"
        params = CKKSParams(N=65536, q0=60, sf=56)
        meta = make_encryption_metadata(params, key_id="ghost")
        bundle = KeyBundle(path=bundle_path, metadata=meta)
        assert bundle.key_id == "ghost"
        assert not bundle.has_encryption_keys()  # path doesn't exist

    def test_key_file_path_all_names(self, tmp_path):
        """All five key names should resolve to valid paths."""
        bundle_path = tmp_path / "all-keys"
        bundle_path.mkdir()
        (bundle_path / "global").mkdir()
        (bundle_path / "model").mkdir()
        params = CKKSParams(N=65536, q0=60, sf=56, mul_depth=5)
        meta = make_evaluation_metadata(params, key_id="all", model_name="m")
        bundle = KeyBundle(path=bundle_path, metadata=meta)
        for name in ["secret_key", "public_key", "relin_key", "galois_key", "bootstrap_key"]:
            path = bundle.key_file_path(name)
            assert path.name.endswith(".bin")

    def test_has_evaluation_keys_with_bootstrap(self, tmp_path):
        """Evaluation bundle with bootstrap key file should return True."""
        bundle_path = tmp_path / "eval-boot"
        bundle_path.mkdir()
        (bundle_path / "model").mkdir()
        params = CKKSParams(N=65536, q0=60, sf=56, mul_depth=5)
        meta = make_evaluation_metadata(params, key_id="boot-eval", model_name="m")
        bundle = KeyBundle(path=bundle_path, metadata=meta)
        # Write placeholder files
        bundle.relin_key_path.write_text("rk")
        bundle.galois_key_path.write_text("gk")
        bundle.bootstrap_key_path.write_text("bk")
        assert bundle.has_evaluation_keys()

    def test_from_dict_with_extra_fields(self, tmp_path):
        """from_dict should ignore unknown fields gracefully."""
        d = {
            "key_id": "extra",
            "created_at": "2026-01-01T00:00:00Z",
            "key_type": "encryption",
            "ckks_params": {"N": 65536, "q0": 60, "sf": 56},
            "future_field": "some_value",
            "another_new_field": 42,
        }
        meta = KeyBundleMetadata.from_dict(d)
        assert meta.key_id == "extra"
        assert not hasattr(meta, "future_field") or meta.future_field is None

    def test_summary_no_key_files(self, tmp_path):
        """Summary with no key files on disk should show exists=False."""
        bundle_path = tmp_path / "empty-bundle"
        bundle_path.mkdir()
        (bundle_path / "global").mkdir()
        params = CKKSParams(N=65536, q0=60, sf=56)
        meta = make_encryption_metadata(params, key_id="empty")
        bundle = KeyBundle(path=bundle_path, metadata=meta)
        summary = bundle.summary()
        assert summary["key_id"] == "empty"
        # No .bin files exist → exists should be False
        for _name, info in summary["key_files"].items():
            assert info["exists"] is False
