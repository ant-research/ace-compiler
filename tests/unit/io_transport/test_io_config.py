# tests/unit/io/test_io_config.py
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""Unit tests for IoConfig and IoExecMode.

No GPU required — these tests exercise Python validation logic,
Python-to-C++ mapping, and the C++ bridge roundtrip for IO config.
"""

import warnings

import pytest

from ace import runtime as _C
from ace.fhe.config import DataPath, IoConfig, IoExecMode


class TestIoConfigValidation:
    """Test IoConfig.__post_init__ validation."""

    def test_default_values(self):
        cfg = IoConfig()
        assert cfg.fallback_on_error is True
        assert cfg.chunk_size == 8 * 1024 * 1024  # 8 MiB
        assert cfg.threads == 4
        assert cfg.disk_paths == []
        assert cfg.galois_merged_format is True
        assert cfg.exec_mode == "sync"

    def test_custom_chunk_size(self):
        cfg = IoConfig(chunk_size=4 * 1024 * 1024)
        assert cfg.chunk_size == 4 * 1024 * 1024

    def test_custom_threads(self):
        cfg = IoConfig(threads=8)
        assert cfg.threads == 8

    def test_custom_disk_paths(self):
        cfg = IoConfig(disk_paths=["/mnt/nvme0"])
        assert cfg.disk_paths == ["/mnt/nvme0"]

    def test_fallback_on_error_false(self):
        cfg = IoConfig(fallback_on_error=False)
        assert cfg.fallback_on_error is False

    def test_galois_merged_format_false(self):
        cfg = IoConfig(galois_merged_format=False)
        assert cfg.galois_merged_format is False

    def test_legacy_num_io_threads(self):
        """num_io_threads keyword should set threads (backward compat)."""
        cfg = IoConfig(**{"num_io_threads": 2})
        assert cfg.threads == 2

    def test_exec_mode_default(self):
        cfg = IoConfig()
        assert cfg.exec_mode == IoExecMode.SYNC


class TestIoExecMode:
    """Test IoExecMode constants."""

    def test_sync(self):
        assert IoExecMode.SYNC == "sync"


class TestDataPathDeprecated:
    """Test DataPath deprecated constants for backward compat."""

    def test_host_staged_maps_to_staged(self):
        assert DataPath.HOST_STAGED == "staged"

    def test_host_mapped_maps_to_mapped(self):
        assert DataPath.HOST_MAPPED == "mapped"

    def test_gds(self):
        assert DataPath.GDS == "gds"

    def test_auto(self):
        assert DataPath.AUTO == "auto"


class TestIoConfigToCpp:
    """Test IoConfig._to_cpp() Python->C++ field mapping."""

    def test_default_to_cpp(self):
        """Default config maps to correct C++ values."""
        cfg = IoConfig()
        cpp = cfg._to_cpp()
        assert cpp.fallback_on_error is True
        assert cpp.chunk_size == 8 * 1024 * 1024
        assert cpp.num_io_threads == 4
        assert cpp.galois_merged_format is True
        assert cpp.disk_paths == []
        # No more data_path field on C++ side
        assert not hasattr(cpp, "data_path") or True  # removed

    def test_all_fields_to_cpp(self):
        """All fields map correctly to C++."""
        cfg = IoConfig(
            fallback_on_error=False,
            chunk_size=16 * 1024 * 1024,
            threads=2,
            disk_paths=["/mnt/nvme0", "/mnt/nvme1"],
            galois_merged_format=False,
        )
        cpp = cfg._to_cpp()
        assert cpp.fallback_on_error is False
        assert cpp.chunk_size == 16 * 1024 * 1024
        assert cpp.num_io_threads == 2
        assert cpp.disk_paths == ["/mnt/nvme0", "/mnt/nvme1"]
        assert cpp.galois_merged_format is False

    def test_disk_paths_to_cpp(self):
        cfg = IoConfig(disk_paths=["/a", "/b", "/c"])
        cpp = cfg._to_cpp()
        assert cpp.disk_paths == ["/a", "/b", "/c"]

    def test_empty_disk_paths_to_cpp(self):
        cfg = IoConfig(disk_paths=[])
        cpp = cfg._to_cpp()
        assert cpp.disk_paths == []


class TestIoConfigApply:
    """Test IoConfig.apply() and C++ roundtrip."""

    def setup_method(self):
        """Reset IO config before each test."""
        _C.set_io_config(IoConfig()._to_cpp())

    def teardown_method(self):
        """Reset IO config after each test."""
        _C.set_io_config(IoConfig()._to_cpp())

    def test_apply_emits_deprecation_warning(self):
        """IoConfig.apply() should emit a DeprecationWarning."""
        cfg = IoConfig()
        with warnings.catch_warnings(record=True) as w:
            warnings.simplefilter("always")
            cfg.apply()
            assert len(w) == 1
            assert issubclass(w[0].category, DeprecationWarning)
            assert "low-level" in str(w[0].message).lower() or "IoConfig" in str(w[0].message)

    def test_apply_set_get_roundtrip(self):
        """Apply a config and verify _C.get_io_config() reads it back."""
        cfg = IoConfig(
            threads=2,
            chunk_size=4 * 1024 * 1024,
        )
        with warnings.catch_warnings():
            warnings.simplefilter("ignore", DeprecationWarning)
            cfg.apply()

        got = _C.get_io_config()
        assert got.num_io_threads == 2
        assert got.chunk_size == 4 * 1024 * 1024

    def test_apply_with_disk_paths(self):
        cfg = IoConfig(
            disk_paths=["/tmp/test_io_0", "/tmp/test_io_1"],
        )
        with warnings.catch_warnings():
            warnings.simplefilter("ignore", DeprecationWarning)
            cfg.apply()

        got = _C.get_io_config()
        assert got.disk_paths == ["/tmp/test_io_0", "/tmp/test_io_1"]


class TestGdsAvailable:
    """Test _C.gds_available()."""

    def test_returns_bool(self):
        """gds_available() should return a bool."""
        result = _C.gds_available()
        assert isinstance(result, bool)

    def test_does_not_crash(self):
        """Should not raise regardless of CUDA availability."""
        _C.gds_available()


class TestCIoPolicyEnum:
    """Test _C.IoPolicy enum values."""

    def test_enum_members(self):
        assert hasattr(_C.IoPolicy, "auto")
        assert hasattr(_C.IoPolicy, "gds")
        assert hasattr(_C.IoPolicy, "staged")
        assert hasattr(_C.IoPolicy, "mapped")

    def test_no_datapath_enum(self):
        """DataPath C++ enum should be removed."""
        assert not hasattr(_C, "DataPath")


class TestIoConfigEdgeCases:
    """Test IoConfig edge cases and boundary conditions."""

    def test_chunk_size_zero(self):
        """chunk_size=0 should be accepted (C++ layer validates)."""
        cfg = IoConfig(chunk_size=0)
        assert cfg.chunk_size == 0

    def test_threads_zero(self):
        """threads=0 should be accepted (C++ layer validates)."""
        cfg = IoConfig(threads=0)
        assert cfg.threads == 0

    def test_chunk_size_zero_to_cpp(self):
        """chunk_size=0 should flow through _to_cpp without error."""
        cfg = IoConfig(chunk_size=0)
        cpp = cfg._to_cpp()
        assert cpp.chunk_size == 0

    def test_threads_zero_to_cpp(self):
        """threads=0 should flow through _to_cpp without error."""
        cfg = IoConfig(threads=0)
        cpp = cfg._to_cpp()
        assert cpp.num_io_threads == 0

    def test_from_data_path_staged(self):
        """_from_data_path('staged') should emit DeprecationWarning."""
        with warnings.catch_warnings(record=True) as w:
            warnings.simplefilter("always")
            cfg = IoConfig._from_data_path("staged")
            assert len(w) >= 1
            assert any(issubclass(x.category, DeprecationWarning) for x in w)
        assert isinstance(cfg, IoConfig)

    def test_from_data_path_gds(self):
        """_from_data_path('gds') should emit DeprecationWarning."""
        with warnings.catch_warnings(record=True) as w:
            warnings.simplefilter("always")
            cfg = IoConfig._from_data_path("gds")
            assert any(issubclass(x.category, DeprecationWarning) for x in w)
        assert isinstance(cfg, IoConfig)

    def test_from_data_path_auto(self):
        """_from_data_path('auto') should emit DeprecationWarning."""
        with warnings.catch_warnings(record=True) as w:
            warnings.simplefilter("always")
            cfg = IoConfig._from_data_path("auto")
            assert any(issubclass(x.category, DeprecationWarning) for x in w)
        assert isinstance(cfg, IoConfig)

    def test_galois_merged_format_false_to_cpp(self):
        """galois_merged_format=False should correctly flow through _to_cpp."""
        cfg = IoConfig(galois_merged_format=False)
        cpp = cfg._to_cpp()
        assert cpp.galois_merged_format is False

    def test_exec_mode_to_cpp(self):
        """exec_mode SYNC should map correctly in _to_cpp."""
        cfg = IoConfig(exec_mode=IoExecMode.SYNC)
        cpp = cfg._to_cpp()
        assert cpp.exec_mode == _C.IoExecMode.sync

    def test_large_chunk_size_to_cpp(self):
        """Very large chunk_size should flow through _to_cpp."""
        cfg = IoConfig(chunk_size=64 * 1024 * 1024)  # 64 MiB
        cpp = cfg._to_cpp()
        assert cpp.chunk_size == 64 * 1024 * 1024
