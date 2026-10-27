# tests/unit/kms/test_storage.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Unit tests for KMS storage module.

Tests file-based key bundle storage: save, load, list, info.
"""

import json
from pathlib import Path

import pytest

from ace.kms.config_reader import CKKSParams
from ace.kms.key_bundle import KeyBundle, KeyBundleMetadata, make_encryption_metadata, make_evaluation_metadata
from ace.kms.storage import get_key_info, list_keys, load_key_bundle, resolve_output_dir, save_key_bundle


# =============================================================================
# save_key_bundle / load_key_bundle
# =============================================================================


class TestSaveLoadKeyBundle:
    """Tests for save_key_bundle() and load_key_bundle()."""

    def test_save_encryption_bundle(self, tmp_path):
        params = CKKSParams(N=65536, q0=60, sf=56)
        meta = make_encryption_metadata(params, key_id="test-enc")
        bundle = KeyBundle(path=tmp_path / "enc-bundle", metadata=meta)

        result = save_key_bundle(bundle)
        assert result == tmp_path / "enc-bundle"
        assert (tmp_path / "enc-bundle" / "metadata.json").exists()
        assert (tmp_path / "enc-bundle" / "global").is_dir()

    def test_save_evaluation_bundle(self, tmp_path):
        params = CKKSParams(N=65536, q0=60, sf=56, mul_depth=31)
        meta = make_evaluation_metadata(params, key_id="test-eval", model_name="resnet20")
        bundle = KeyBundle(path=tmp_path / "eval-bundle", metadata=meta)

        result = save_key_bundle(bundle)
        assert (tmp_path / "eval-bundle" / "metadata.json").exists()
        assert (tmp_path / "eval-bundle" / "model").is_dir()

    def test_load_bundle(self, tmp_path):
        params = CKKSParams(N=65536, q0=60, sf=56)
        meta = make_encryption_metadata(params, key_id="test-load")
        bundle = KeyBundle(path=tmp_path / "load-bundle", metadata=meta)
        save_key_bundle(bundle)

        loaded = load_key_bundle(tmp_path / "load-bundle")
        assert loaded.key_id == "test-load"
        assert loaded.key_type == "encryption"
        assert loaded.metadata.ckks_params["N"] == 65536

    def test_roundtrip_metadata(self, tmp_path):
        params = CKKSParams(N=8192, q0=60, sf=56, mul_depth=2, rot_idxs=[1, 2, 3])
        meta = make_encryption_metadata(params, key_id="roundtrip", description="test roundtrip")
        bundle = KeyBundle(path=tmp_path / "rt-bundle", metadata=meta)
        save_key_bundle(bundle)

        loaded = load_key_bundle(tmp_path / "rt-bundle")
        assert loaded.metadata.key_id == "roundtrip"
        assert loaded.metadata.description == "test roundtrip"
        assert loaded.metadata.ckks_params["rot_idxs"] == [1, 2, 3]

    def test_load_nonexistent_dir(self, tmp_path):
        with pytest.raises(FileNotFoundError, match="Key bundle directory not found"):
            load_key_bundle(tmp_path / "nonexistent")

    def test_load_missing_metadata(self, tmp_path):
        bundle_dir = tmp_path / "empty-bundle"
        bundle_dir.mkdir()
        with pytest.raises(FileNotFoundError, match=r"metadata\.json not found"):
            load_key_bundle(bundle_dir)

    def test_save_no_create_dirs(self, tmp_path):
        params = CKKSParams(N=65536, q0=60, sf=56)
        meta = make_encryption_metadata(params, key_id="no-dirs")
        bundle = KeyBundle(path=tmp_path / "no-dirs-bundle", metadata=meta)

        # Without create_dirs, the directory won't exist
        with pytest.raises(FileNotFoundError):
            save_key_bundle(bundle, create_dirs=False)


# =============================================================================
# list_keys
# =============================================================================


class TestListKeys:
    """Tests for list_keys()."""

    def test_list_empty_dir(self, tmp_path):
        result = list_keys(tmp_path)
        assert result == []

    def test_list_nonexistent_dir(self):
        result = list_keys("/nonexistent/path")
        assert result == []

    def test_list_multiple_bundles(self, tmp_path):
        # Create two bundles
        for key_id in ["enc-001", "enc-002"]:
            bundle_path = tmp_path / key_id
            params = CKKSParams(N=65536, q0=60, sf=56)
            meta = make_encryption_metadata(params, key_id=key_id)
            bundle = KeyBundle(path=bundle_path, metadata=meta)
            save_key_bundle(bundle)

        result = list_keys(tmp_path)
        assert len(result) == 2
        key_ids = {r["key_id"] for r in result}
        assert key_ids == {"enc-001", "enc-002"}

    def test_list_skips_non_bundle_dirs(self, tmp_path):
        # Create a bundle and a non-bundle directory
        bundle_path = tmp_path / "real-bundle"
        params = CKKSParams(N=65536, q0=60, sf=56)
        meta = make_encryption_metadata(params, key_id="real-bundle")
        bundle = KeyBundle(path=bundle_path, metadata=meta)
        save_key_bundle(bundle)

        (tmp_path / "not-a-bundle").mkdir()

        result = list_keys(tmp_path)
        assert len(result) == 1
        assert result[0]["key_id"] == "real-bundle"

    def test_list_skips_invalid_metadata(self, tmp_path):
        # Create a bundle with invalid JSON
        bad_dir = tmp_path / "bad-bundle"
        bad_dir.mkdir()
        (bad_dir / "metadata.json").write_text("not json")

        result = list_keys(tmp_path)
        assert result == []  # skipped gracefully


# =============================================================================
# get_key_info
# =============================================================================


class TestGetKeyInfo:
    """Tests for get_key_info()."""

    def test_get_info(self, tmp_path):
        params = CKKSParams(N=65536, q0=60, sf=56)
        meta = make_encryption_metadata(params, key_id="info-test")
        bundle = KeyBundle(path=tmp_path / "info-test", metadata=meta)
        save_key_bundle(bundle)

        info = get_key_info("info-test", key_dir=tmp_path)
        assert info["key_id"] == "info-test"
        assert info["key_type"] == "encryption"
        assert info["ckks_params"]["N"] == 65536

    def test_get_info_not_found(self, tmp_path):
        with pytest.raises(FileNotFoundError, match=r"Key bundle.*not found"):
            get_key_info("nonexistent", key_dir=tmp_path)


# =============================================================================
# resolve_output_dir
# =============================================================================


class TestResolveOutputDir:
    """Tests for resolve_output_dir()."""

    def test_new_dir_with_key_id(self, tmp_path):
        output = tmp_path / "new-bundle"
        path, key_id = resolve_output_dir(output, key_id="my-key")
        assert path == output.resolve()
        assert key_id == "my-key"

    def test_new_dir_without_key_id(self, tmp_path):
        output = tmp_path / "my-bundle"
        _path, key_id = resolve_output_dir(output)
        assert key_id == "my-bundle"  # derived from dir name

    def test_existing_bundle_reads_key_id(self, tmp_path):
        bundle_path = tmp_path / "existing-bundle"
        params = CKKSParams(N=65536, q0=60, sf=56)
        meta = make_encryption_metadata(params, key_id="original-id")
        bundle = KeyBundle(path=bundle_path, metadata=meta)
        save_key_bundle(bundle)

        _path, key_id = resolve_output_dir(bundle_path)
        assert key_id == "original-id"  # read from existing metadata


class TestStorageEdgeCases:
    """Edge case tests for storage operations."""

    def test_load_corrupted_metadata_json(self, tmp_path):
        """Loading a bundle with invalid metadata JSON should raise error."""
        bundle_dir = tmp_path / "corrupt-bundle"
        bundle_dir.mkdir()
        (bundle_dir / "metadata.json").write_text('{"key_id": "bad", "key_type": "encryption",')
        # Missing closing brace — invalid JSON
        with pytest.raises(json.JSONDecodeError):
            load_key_bundle(bundle_dir)

    def test_load_missing_key_type_in_metadata(self, tmp_path):
        """Metadata without key_type: from_dict requires all mandatory fields,
        so loading a bundle with missing key_type should raise KeyError."""
        bundle_dir = tmp_path / "no-type-bundle"
        bundle_dir.mkdir()
        (bundle_dir / "global").mkdir()
        # Include all required fields except key_type
        meta = {
            "key_id": "no-type",
            "created_at": "2026-01-01T00:00:00Z",
            "ckks_params": {"N": 65536, "q0": 60, "sf": 56},
            # key_type is missing
        }
        (bundle_dir / "metadata.json").write_text(json.dumps(meta))
        # from_dict requires key_type, so this should raise KeyError
        with pytest.raises(KeyError, match="key_type"):
            load_key_bundle(bundle_dir)

    def test_list_keys_sorted(self, tmp_path):
        """list_keys should return results (order may vary)."""
        for kid in ["zebra", "alpha", "middle"]:
            bp = tmp_path / kid
            params = CKKSParams(N=65536, q0=60, sf=56)
            meta = make_encryption_metadata(params, key_id=kid)
            bundle = KeyBundle(path=bp, metadata=meta)
            save_key_bundle(bundle)
        result = list_keys(tmp_path)
        key_ids = [r["key_id"] for r in result]
        assert len(key_ids) == 3
        assert set(key_ids) == {"zebra", "alpha", "middle"}

    def test_save_with_description_roundtrip(self, tmp_path):
        """Description should survive save/load roundtrip."""
        params = CKKSParams(N=65536, q0=60, sf=56, mul_depth=5)
        meta = make_evaluation_metadata(
            params,
            key_id="desc-test",
            model_name="resnet20",
            description="Test keys for CI",
        )
        bundle = KeyBundle(path=tmp_path / "desc-bundle", metadata=meta)
        save_key_bundle(bundle)

        loaded = load_key_bundle(tmp_path / "desc-bundle")
        assert loaded.metadata.description == "Test keys for CI"

    def test_save_evaluation_bundle_with_bootstrap(self, tmp_path):
        """Evaluation bundle with bootstrap key should have model dir."""
        params = CKKSParams(N=65536, q0=60, sf=56, mul_depth=31)
        meta = make_evaluation_metadata(params, key_id="eval-boot", model_name="model")
        bundle = KeyBundle(path=tmp_path / "eval-boot", metadata=meta)
        save_key_bundle(bundle)
        assert (tmp_path / "eval-boot" / "model").is_dir()
        assert "bootstrap_key" in meta.key_files
