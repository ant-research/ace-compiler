# tests/unit/kms/test_config_reader.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Unit tests for KMS config_reader module.

Tests .conf file parsing and CKKSParams extraction.
"""

import json
import tempfile
from pathlib import Path

import pytest

from ace.kms.config_reader import CKKSParams, find_conf_file, read_conf


# =============================================================================
# CKKSParams
# =============================================================================


class TestCKKSParams:
    """Tests for CKKSParams dataclass."""

    def test_required_fields(self):
        params = CKKSParams(N=65536, q0=60, sf=56)
        assert params.N == 65536
        assert params.q0 == 60
        assert params.sf == 56
        assert params.mul_depth == 0
        assert params.rot_idxs == []
        assert params.hamming_weight == 192

    def test_all_fields(self):
        params = CKKSParams(
            N=65536,
            q0=60,
            sf=56,
            mul_depth=31,
            rot_idxs=[1, 2, 3],
            hamming_weight=128,
            sec_level=1,
            num_q_parts=3,
            input_level=16,
            provider=3,
        )
        assert params.N == 65536
        assert params.mul_depth == 31
        assert params.rot_idxs == [1, 2, 3]
        assert params.hamming_weight == 128
        assert params.provider == 3

    def test_to_dict(self):
        params = CKKSParams(N=8192, q0=60, sf=56, mul_depth=2)
        d = params.to_dict()
        assert d["N"] == 8192
        assert d["q0"] == 60
        assert d["sf"] == 56
        assert d["mul_depth"] == 2
        assert d["rot_idxs"] == []
        assert "hamming_weight" in d

    def test_from_dict(self):
        d = {"N": 65536, "q0": 60, "sf": 56, "mul_depth": 31, "rot_idxs": [1, 2]}
        params = CKKSParams.from_dict(d)
        assert params.N == 65536
        assert params.mul_depth == 31
        assert params.rot_idxs == [1, 2]

    def test_from_dict_defaults(self):
        d = {"N": 65536, "q0": 60, "sf": 56}
        params = CKKSParams.from_dict(d)
        assert params.mul_depth == 0
        assert params.rot_idxs == []
        assert params.hamming_weight == 192

    def test_roundtrip(self):
        original = CKKSParams(N=65536, q0=60, sf=56, mul_depth=31, rot_idxs=[1, 2, 3])
        restored = CKKSParams.from_dict(original.to_dict())
        assert restored.N == original.N
        assert restored.q0 == original.q0
        assert restored.sf == original.sf
        assert restored.mul_depth == original.mul_depth
        assert restored.rot_idxs == original.rot_idxs

    def test_str(self):
        params = CKKSParams(N=65536, q0=60, sf=56, rot_idxs=[1, 2, 3])
        s = str(params)
        assert "N=65536" in s
        assert "q0=60" in s
        assert "3 items" in s


# =============================================================================
# find_conf_file
# =============================================================================


class TestFindConfFile:
    """Tests for find_conf_file()."""

    def test_finds_conf_file(self, tmp_path):
        conf = tmp_path / "model.conf"
        conf.write_text("{}")
        result = find_conf_file(tmp_path)
        assert result == conf

    def test_no_conf_file(self, tmp_path):
        with pytest.raises(FileNotFoundError, match=r"No \.conf file found"):
            find_conf_file(tmp_path)

    def test_nonexistent_dir(self):
        with pytest.raises(FileNotFoundError, match="Compiled directory not found"):
            find_conf_file("/nonexistent/path")

    def test_multiple_conf_files(self, tmp_path):
        (tmp_path / "a.conf").write_text("{}")
        (tmp_path / "b.conf").write_text("{}")
        result = find_conf_file(tmp_path)
        assert result.suffix == ".conf"


# =============================================================================
# read_conf
# =============================================================================


class TestReadConf:
    """Tests for read_conf()."""

    def _make_conf(self, tmp_path, context_params=None, extra=None):
        """Create a minimal .conf file for testing."""
        if context_params is None:
            context_params = {
                "_poly_degree": 8,
                "_first_mod_size": 60,
                "_scaling_mod_size": 56,
                "_mul_depth": 2,
                "_rot_idxs": [1, 2],
                "_hamming_weight": 192,
            }
        data = {"context_params": context_params}
        if extra:
            data.update(extra)
        conf = tmp_path / "test.conf"
        conf.write_text(json.dumps(data))
        return conf

    def test_basic_read(self, tmp_path):
        conf = self._make_conf(tmp_path)
        params, _raw = read_conf(conf)
        assert params.N == 8
        assert params.q0 == 60
        assert params.sf == 56
        assert params.mul_depth == 2
        assert params.rot_idxs == [1, 2]

    def test_large_model(self, tmp_path):
        ctx = {
            "_poly_degree": 65536,
            "_first_mod_size": 60,
            "_scaling_mod_size": 56,
            "_mul_depth": 31,
            "_num_q_parts": 3,
            "_num_rot_idx": 100,
            "_rot_idxs": list(range(100)),
            "_hamming_weight": 192,
            "_sec_level": 0,
            "_input_level": 16,
            "_provider": 3,
        }
        conf = self._make_conf(tmp_path, context_params=ctx)
        params, _raw = read_conf(conf)
        assert params.N == 65536
        assert params.mul_depth == 31
        assert len(params.rot_idxs) == 100
        assert params.num_q_parts == 3
        assert params.provider == 3

    def test_missing_required_field(self, tmp_path):
        ctx = {"_poly_degree": 8}  # missing _first_mod_size, _scaling_mod_size
        conf = self._make_conf(tmp_path, context_params=ctx)
        with pytest.raises(ValueError, match="Missing required fields"):
            read_conf(conf)

    def test_missing_context_params(self, tmp_path):
        conf = tmp_path / "bad.conf"
        conf.write_text(json.dumps({"input_count": 1}))
        with pytest.raises(ValueError, match="Missing 'context_params'"):
            read_conf(conf)

    def test_nonexistent_file(self):
        with pytest.raises(FileNotFoundError, match="Config file not found"):
            read_conf("/nonexistent/path/model.conf")

    def test_invalid_json(self, tmp_path):
        conf = tmp_path / "bad.conf"
        conf.write_text("not json")
        with pytest.raises(json.JSONDecodeError):
            read_conf(conf)

    def test_defaults_for_optional_fields(self, tmp_path):
        ctx = {
            "_poly_degree": 8,
            "_first_mod_size": 60,
            "_scaling_mod_size": 56,
        }
        conf = self._make_conf(tmp_path, context_params=ctx)
        params, _raw = read_conf(conf)
        assert params.mul_depth == 0
        assert params.rot_idxs == []
        assert params.hamming_weight == 192
        assert params.sec_level == 0
        assert params.num_q_parts == 1

    def test_raw_data_preserved(self, tmp_path):
        extra = {"input_count": 3, "output_count": 1}
        conf = self._make_conf(tmp_path, extra=extra)
        _params, raw = read_conf(conf)
        assert raw["input_count"] == 3
        assert raw["output_count"] == 1
        assert "context_params" in raw


class TestCKKSParamsEdgeCases:
    """Edge case tests for CKKSParams."""

    def test_extreme_small_N(self):
        """Very small N should still construct."""
        params = CKKSParams(N=2, q0=60, sf=56)
        assert params.N == 2

    def test_extreme_large_N(self):
        """Very large N should still construct."""
        params = CKKSParams(N=131072, q0=60, sf=56)
        assert params.N == 131072

    def test_extra_unexpected_fields_in_dict(self):
        """from_dict should handle extra unexpected fields gracefully."""
        d = {
            "N": 65536,
            "q0": 60,
            "sf": 56,
            "mul_depth": 5,
            "unknown_field": "should_be_ignored",
        }
        params = CKKSParams.from_dict(d)
        assert params.N == 65536

    def test_zero_mul_depth(self):
        """Explicit mul_depth=0."""
        params = CKKSParams(N=65536, q0=60, sf=56, mul_depth=0)
        assert params.mul_depth == 0

    def test_large_rotation_indices(self):
        """Many rotation indices."""
        rot_idxs = list(range(200))
        params = CKKSParams(N=65536, q0=60, sf=56, rot_idxs=rot_idxs)
        assert len(params.rot_idxs) == 200
        assert params.rot_idxs[0] == 0
        assert params.rot_idxs[-1] == 199

    def test_conf_with_rt_data_info(self, tmp_path):
        """Conf file with rt_data_info should parse CKKSParams correctly."""
        ctx = {
            "_poly_degree": 8,
            "_first_mod_size": 60,
            "_scaling_mod_size": 56,
            "_mul_depth": 2,
        }
        data = {
            "context_params": ctx,
            "rt_data_info": {
                "_entry_type": 1,
                "_file_name": "model.weight",
            },
        }
        conf = tmp_path / "test.conf"
        conf.write_text(json.dumps(data))
        params, raw = read_conf(conf)
        assert params.N == 8
        assert "rt_data_info" in raw
        assert raw["rt_data_info"]["_entry_type"] == 1
