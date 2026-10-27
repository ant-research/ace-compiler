# tests/unit/io/test_io_policy.py
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""Unit tests for IoPolicy and IoPolicySettings.

No GPU required — these tests only exercise Python validation logic
and the Python-C++ bridge for IO policy state.
"""

import warnings

import pytest

from ace import runtime as _C
from ace.fhe.config import IoConfig, IoPolicy, IoPolicySettings


class TestIoPolicyConstants:
    """Test IoPolicy string constants."""

    def test_auto(self):
        assert IoPolicy.AUTO == "auto"

    def test_gds(self):
        assert IoPolicy.GDS == "gds"

    def test_staged(self):
        assert IoPolicy.STAGED == "staged"

    def test_mapped(self):
        assert IoPolicy.MAPPED == "mapped"

    def test_all_policies_count(self):
        """There should be exactly 4 policies."""
        policies = {
            IoPolicy.AUTO,
            IoPolicy.GDS,
            IoPolicy.STAGED,
            IoPolicy.MAPPED,
        }
        assert len(policies) == 4


class TestIoPolicySettingsValidation:
    """Test IoPolicySettings.__post_init__ validation."""

    def test_default_policy(self):
        """Default policy is auto."""
        settings = IoPolicySettings()
        assert settings.policy == "auto"
        assert settings.disk_paths == []

    def test_valid_policies(self):
        """All valid policies should be accepted."""
        for policy in [IoPolicy.AUTO, IoPolicy.GDS, IoPolicy.STAGED, IoPolicy.MAPPED]:
            settings = IoPolicySettings(policy=policy)
            assert settings.policy == policy

    def test_invalid_policy_raises(self):
        """Invalid policy string should raise ValueError."""
        with pytest.raises(ValueError, match="Invalid policy"):
            IoPolicySettings(policy="invalid_policy")

    def test_empty_policy_raises(self):
        with pytest.raises(ValueError, match="Invalid policy"):
            IoPolicySettings(policy="")

    def test_disk_paths_default_empty(self):
        settings = IoPolicySettings()
        assert settings.disk_paths == []

    def test_disk_paths_custom(self):
        settings = IoPolicySettings(disk_paths=["/mnt/nvme0", "/mnt/nvme1"])
        assert settings.disk_paths == ["/mnt/nvme0", "/mnt/nvme1"]


class TestIoPolicySettingsBackwardCompat:
    """Test deprecated policy names are mapped correctly."""

    def test_auto_detect_maps_to_auto(self):
        with warnings.catch_warnings(record=True) as w:
            warnings.simplefilter("always")
            settings = IoPolicySettings(policy="auto_detect")
            assert settings.policy == "auto"
            assert len(w) == 1
            assert issubclass(w[0].category, DeprecationWarning)

    def test_galois_key_load_maps_to_auto(self):
        with warnings.catch_warnings(record=True) as w:
            warnings.simplefilter("always")
            settings = IoPolicySettings(policy="galois_key_load")
            assert settings.policy == "auto"
            assert len(w) == 1
            assert issubclass(w[0].category, DeprecationWarning)

    def test_key_persistence_maps_to_auto(self):
        with warnings.catch_warnings(record=True) as w:
            warnings.simplefilter("always")
            settings = IoPolicySettings(policy="key_persistence")
            assert settings.policy == "auto"
            assert len(w) == 1
            assert issubclass(w[0].category, DeprecationWarning)

    def test_checkpoint_maps_to_staged(self):
        with warnings.catch_warnings(record=True) as w:
            warnings.simplefilter("always")
            settings = IoPolicySettings(policy="checkpoint")
            assert settings.policy == "staged"
            assert len(w) == 1
            assert issubclass(w[0].category, DeprecationWarning)

    def test_high_throughput_maps_to_auto(self):
        with warnings.catch_warnings(record=True) as w:
            warnings.simplefilter("always")
            settings = IoPolicySettings(policy="high_throughput")
            assert settings.policy == "auto"
            assert len(w) == 1
            assert issubclass(w[0].category, DeprecationWarning)


class TestIoPolicySettingsApply:
    """Test IoPolicySettings.apply() -> C++ bridge roundtrip."""

    def setup_method(self):
        """Reset to default policy before each test."""
        _C.set_io_policy(_C.IoPolicy.auto)

    def teardown_method(self):
        """Reset to default policy after each test."""
        _C.set_io_policy(_C.IoPolicy.auto)

    def test_apply_auto(self):
        IoPolicySettings(policy="auto").apply()
        assert _C.get_io_policy() == _C.IoPolicy.auto

    def test_apply_gds(self):
        IoPolicySettings(policy="gds").apply()
        assert _C.get_io_policy() == _C.IoPolicy.gds

    def test_apply_staged(self):
        IoPolicySettings(policy="staged").apply()
        assert _C.get_io_policy() == _C.IoPolicy.staged

    def test_apply_mapped(self):
        IoPolicySettings(policy="mapped").apply()
        assert _C.get_io_policy() == _C.IoPolicy.mapped

    def test_apply_overwrites_previous(self):
        IoPolicySettings(policy="gds").apply()
        assert _C.get_io_policy() == _C.IoPolicy.gds

        IoPolicySettings(policy="staged").apply()
        assert _C.get_io_policy() == _C.IoPolicy.staged

    def test_apply_with_disk_paths(self):
        """apply() with disk_paths calls _C.set_io_policy_with_disk_paths."""
        IoPolicySettings(
            policy="auto",
            disk_paths=["/tmp/test_disk_0", "/tmp/test_disk_1"],
        ).apply()
        assert _C.get_io_policy() == _C.IoPolicy.auto

    def test_apply_without_disk_paths(self):
        """apply() without disk_paths calls _C.set_io_policy."""
        IoPolicySettings(policy="staged").apply()
        assert _C.get_io_policy() == _C.IoPolicy.staged

    def test_apply_deprecated_name(self):
        """Deprecated names should still work through apply()."""
        with warnings.catch_warnings():
            warnings.simplefilter("ignore", DeprecationWarning)
            IoPolicySettings(policy="checkpoint").apply()
        assert _C.get_io_policy() == _C.IoPolicy.staged


class TestIoProfile:
    """Test IoProfile business presets return correct (IoPolicySettings, IoConfig)."""

    def test_galois_key_load_defaults(self):
        """galois_key_load returns auto policy, 4 threads, merged format."""
        from ace.fhe.config import IoProfile

        settings, config = IoProfile.galois_key_load()
        assert settings.policy == IoPolicy.AUTO
        assert settings.disk_paths == []
        assert config.threads == 4
        assert config.galois_merged_format is True

    def test_galois_key_load_with_disk_paths(self):
        """galois_key_load propagates disk_paths to both settings and config."""
        from ace.fhe.config import IoProfile

        dp = ["/mnt/nvme0", "/mnt/nvme1"]
        settings, config = IoProfile.galois_key_load(disk_paths=dp)
        assert settings.disk_paths == dp
        assert config.disk_paths == dp

    def test_key_persistence_defaults(self):
        """key_persistence returns auto policy, 2 threads, per-key format."""
        from ace.fhe.config import IoProfile

        settings, config = IoProfile.key_persistence()
        assert settings.policy == IoPolicy.AUTO
        assert settings.disk_paths == []
        assert config.threads == 2
        assert config.galois_merged_format is False

    def test_checkpoint_defaults(self):
        """checkpoint returns staged policy, 1 thread, per-key format."""
        from ace.fhe.config import IoProfile

        settings, config = IoProfile.checkpoint()
        assert settings.policy == IoPolicy.STAGED
        assert config.threads == 1
        assert config.galois_merged_format is False

    def test_high_throughput_equals_galois_key_load(self):
        """high_throughput is an alias for galois_key_load."""
        from ace.fhe.config import IoProfile

        ht_settings, ht_config = IoProfile.high_throughput()
        gkl_settings, gkl_config = IoProfile.galois_key_load()
        assert ht_settings.policy == gkl_settings.policy
        assert ht_settings.disk_paths == gkl_settings.disk_paths
        assert ht_config.threads == gkl_config.threads
        assert ht_config.galois_merged_format == gkl_config.galois_merged_format

    def test_all_profiles_return_tuple_of_correct_types(self):
        """Each profile returns (IoPolicySettings, IoConfig)."""
        from ace.fhe.config import IoProfile

        for preset_fn in [
            IoProfile.galois_key_load,
            IoProfile.key_persistence,
            IoProfile.checkpoint,
            IoProfile.high_throughput,
        ]:
            result = preset_fn()
            assert isinstance(result, tuple) and len(result) == 2
            assert isinstance(result[0], IoPolicySettings)
            assert isinstance(result[1], IoConfig)

    def test_key_persistence_with_disk_paths(self):
        """key_persistence propagates disk_paths."""
        from ace.fhe.config import IoProfile

        dp = ["/mnt/nvme0"]
        settings, config = IoProfile.key_persistence(disk_paths=dp)
        assert settings.disk_paths == dp
        assert config.disk_paths == dp


class TestIoPolicySettingsDiskPaths:
    """Test IoPolicySettings disk_paths edge cases."""

    def test_disk_paths_default_empty(self):
        settings = IoPolicySettings()
        assert settings.disk_paths == []

    def test_disk_paths_single(self):
        settings = IoPolicySettings(disk_paths=["/mnt/nvme0"])
        assert settings.disk_paths == ["/mnt/nvme0"]

    def test_disk_paths_multiple(self):
        dp = [f"/mnt/nvme{i}" for i in range(4)]
        settings = IoPolicySettings(disk_paths=dp)
        assert settings.disk_paths == dp

    def test_apply_with_disk_paths_roundtrip(self):
        """apply() with disk_paths sets global config disk_paths."""
        IoPolicySettings(
            policy="auto",
            disk_paths=["/tmp/test_dp_0", "/tmp/test_dp_1"],
        ).apply()
        got = _C.get_io_config()
        assert got.disk_paths == ["/tmp/test_dp_0", "/tmp/test_dp_1"]
        # Reset
        IoPolicySettings(policy="auto").apply()


class TestIoPolicySettingsIoConfigInteraction:
    """Test interaction between IoPolicySettings and IoConfig when both applied."""

    def setup_method(self):
        _C.set_io_policy(_C.IoPolicy.auto)
        _C.set_io_config(IoConfig()._to_cpp())

    def teardown_method(self):
        _C.set_io_policy(_C.IoPolicy.auto)
        _C.set_io_config(IoConfig()._to_cpp())

    def test_policy_settings_then_config_apply(self):
        """IoPolicySettings applies policy, then IoConfig applies expert params.

        Both settings should coexist: policy from IoPolicySettings,
        threads/chunk_size from IoConfig.
        """
        IoPolicySettings(policy="staged").apply()
        assert _C.get_io_policy() == _C.IoPolicy.staged

        with warnings.catch_warnings():
            warnings.simplefilter("ignore", DeprecationWarning)
            IoConfig(threads=2, chunk_size=4 * 1024 * 1024).apply()

        # Policy from IoPolicySettings should still be staged
        assert _C.get_io_policy() == _C.IoPolicy.staged
        # IoConfig params should be set
        got = _C.get_io_config()
        assert got.num_io_threads == 2

    def test_config_then_policy_settings_apply(self):
        """IoPolicySettings applied last overwrites the policy portion."""
        with warnings.catch_warnings():
            warnings.simplefilter("ignore", DeprecationWarning)
            IoConfig(threads=8).apply()

        IoPolicySettings(policy="gds").apply()
        assert _C.get_io_policy() == _C.IoPolicy.gds


class TestCPythonBridge:
    """Test _C.IoPolicy enum and set/get roundtrip."""

    def setup_method(self):
        _C.set_io_policy(_C.IoPolicy.auto)

    def teardown_method(self):
        _C.set_io_policy(_C.IoPolicy.auto)

    def test_enum_values(self):
        """C++ IoPolicy enum has expected members."""
        assert hasattr(_C.IoPolicy, "auto")
        assert hasattr(_C.IoPolicy, "gds")
        assert hasattr(_C.IoPolicy, "staged")
        assert hasattr(_C.IoPolicy, "mapped")

    def test_set_get_roundtrip(self):
        """Direct _C.set_io_policy / _C.get_io_policy roundtrip."""
        for policy_val in [_C.IoPolicy.auto, _C.IoPolicy.gds, _C.IoPolicy.staged, _C.IoPolicy.mapped]:
            _C.set_io_policy(policy_val)
            assert _C.get_io_policy() == policy_val

    def test_default_is_auto(self):
        _C.set_io_policy(_C.IoPolicy.auto)
        assert _C.get_io_policy() == _C.IoPolicy.auto
