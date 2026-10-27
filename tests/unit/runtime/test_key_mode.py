# tests/unit/runtime/test_key_mode.py
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""Tests for the KeyMode Python API.

No GPU is required — these tests only touch global process state.
"""

import pytest

from ace import runtime as _C

# Also verify backward-compatible alias
from ace.fhe.config import InferenceMode, KeyMode


class TestKeyModeBasic:
    """Test basic set/get/validate operations for KeyMode."""

    def setup_method(self):
        """Reset key mode to ONLINE before each test."""
        _C.set_key_mode(0)  # ONLINE

    def teardown_method(self):
        """Reset key mode to ONLINE after each test."""
        _C.set_key_mode(0)

    # --- _validate tests ---

    def test_validate_online(self):
        assert KeyMode._validate("online") == "online"

    def test_validate_preload(self):
        assert KeyMode._validate("preload") == "preload"

    def test_validate_preload_full(self):
        assert KeyMode._validate("preload_full") == "preload_full"

    def test_validate_on_demand(self):
        assert KeyMode._validate("on_demand") == "on_demand"

    def test_validate_on_demand_full(self):
        assert KeyMode._validate("on_demand_full") == "on_demand_full"

    def test_validate_key_offline_compat(self):
        """key_offline is accepted as backward-compatible alias for preload_full."""
        assert KeyMode._validate("key_offline") == "key_offline"

    def test_validate_server_only_compat(self):
        """server_only is accepted as backward-compatible alias for preload."""
        assert KeyMode._validate("server_only") == "server_only"

    def test_validate_case_insensitive(self):
        """Mode strings are case-insensitive."""
        assert KeyMode._validate("ONLINE") == "online"
        assert KeyMode._validate("Preload") == "preload"
        assert KeyMode._validate("PRELOAD_FULL") == "preload_full"
        assert KeyMode._validate("KEY_OFFLINE") == "key_offline"

    def test_validate_invalid_raises(self):
        """Invalid mode string raises ValueError."""
        with pytest.raises(ValueError, match="Invalid key_mode"):
            KeyMode._validate("invalid_mode")

    def test_validate_empty_raises(self):
        with pytest.raises(ValueError, match="Invalid key_mode"):
            KeyMode._validate("")

    def test_validate_encode_only_removed(self):
        """encode_only is no longer a valid key mode."""
        with pytest.raises(ValueError, match="Invalid key_mode"):
            KeyMode._validate("encode_only")

    # --- _to_cpp tests ---

    def test_to_cpp_values(self):
        assert KeyMode._to_cpp("online") == 0
        assert KeyMode._to_cpp("preload") == 1
        assert KeyMode._to_cpp("on_demand") == 2
        assert KeyMode._to_cpp("preload_full") == 3
        assert KeyMode._to_cpp("on_demand_full") == 4
        # Backward compat aliases
        assert KeyMode._to_cpp("key_offline") == 3  # maps to PRELOAD_FULL
        assert KeyMode._to_cpp("server_only") == 1  # maps to PRELOAD

    # --- _needs_full_keys tests ---

    def test_needs_full_keys(self):
        assert KeyMode._needs_full_keys("key_offline") is True
        assert KeyMode._needs_full_keys("preload_full") is True
        assert KeyMode._needs_full_keys("on_demand_full") is True
        assert KeyMode._needs_full_keys("server_only") is False
        assert KeyMode._needs_full_keys("online") is False
        assert KeyMode._needs_full_keys("preload") is False
        assert KeyMode._needs_full_keys("on_demand") is False

    # --- _REQUIRES_KEY_PATH tests ---

    def test_requires_key_path(self):
        assert "online" not in KeyMode._REQUIRES_KEY_PATH
        assert "preload" in KeyMode._REQUIRES_KEY_PATH
        assert "preload_full" in KeyMode._REQUIRES_KEY_PATH
        assert "on_demand" in KeyMode._REQUIRES_KEY_PATH
        assert "on_demand_full" in KeyMode._REQUIRES_KEY_PATH
        assert "key_offline" in KeyMode._REQUIRES_KEY_PATH
        assert "server_only" in KeyMode._REQUIRES_KEY_PATH

    # --- apply + get roundtrip tests ---

    def test_apply_and_get_online(self):
        KeyMode.apply("online")
        assert KeyMode.get() == "online"
        assert _C.get_key_mode() == 0

    def test_apply_and_get_preload(self):
        KeyMode.apply("preload")
        assert KeyMode.get() == "preload"
        assert _C.get_key_mode() == 1

    def test_apply_and_get_preload_full(self):
        KeyMode.apply("preload_full")
        assert KeyMode.get() == "preload_full"
        assert _C.get_key_mode() == 3

    def test_apply_and_get_on_demand(self):
        KeyMode.apply("on_demand")
        assert KeyMode.get() == "on_demand"
        assert _C.get_key_mode() == 2

    def test_apply_and_get_on_demand_full(self):
        KeyMode.apply("on_demand_full")
        assert KeyMode.get() == "on_demand_full"
        assert _C.get_key_mode() == 4

    def test_apply_server_only_compat(self):
        """server_only is an alias for preload — get() returns canonical name."""
        KeyMode.apply("server_only")
        assert KeyMode.get() == "preload"  # canonical, not alias
        assert _C.get_key_mode() == 1  # maps to PRELOAD

    def test_apply_key_offline_compat(self):
        """key_offline is an alias for preload_full — get() returns canonical name."""
        KeyMode.apply("key_offline")
        assert KeyMode.get() == "preload_full"  # canonical, not alias
        assert _C.get_key_mode() == 3  # maps to PRELOAD_FULL

    # --- load_full_keys deprecated upgrade tests ---

    def test_apply_preload_with_load_full_keys_true(self):
        """load_full_keys=True with preload upgrades to preload_full."""
        KeyMode.apply("preload", load_full_keys=True)
        assert KeyMode.get() == "preload_full"  # upgraded from preload
        assert _C.get_key_mode() == 3

    def test_apply_on_demand_with_load_full_keys_true(self):
        """load_full_keys=True with on_demand upgrades to on_demand_full."""
        KeyMode.apply("on_demand", load_full_keys=True)
        assert KeyMode.get() == "on_demand_full"  # upgraded from on_demand
        assert _C.get_key_mode() == 4

    def test_apply_preload_with_load_full_keys_false(self):
        """load_full_keys=False with preload keeps preload."""
        KeyMode.apply("preload", load_full_keys=False)
        assert KeyMode.get() == "preload"
        assert _C.get_key_mode() == 1

    def test_apply_overwrites_previous(self):
        KeyMode.apply("server_only")
        assert KeyMode.get() == "preload"  # canonical name for server_only

        KeyMode.apply("online")
        assert KeyMode.get() == "online"

    # --- Direct _C bridge tests ---

    def test_c_set_and_get_key_mode(self):
        """Low-level _C.set_key_mode / _C.get_key_mode roundtrip."""
        for mode_val in [0, 1, 2, 3, 4]:
            _C.set_key_mode(mode_val)
            assert _C.get_key_mode() == mode_val

    def test_c_default_is_online(self):
        """Default key mode is 0 (ONLINE)."""
        _C.set_key_mode(0)
        assert _C.get_key_mode() == 0

    def test_c_key_mode_enum_values(self):
        """_C.KeyMode enum has the expected values."""
        assert int(_C.KeyMode.online) == 0
        assert int(_C.KeyMode.preload) == 1
        assert int(_C.KeyMode.on_demand) == 2
        assert int(_C.KeyMode.preload_full) == 3
        assert int(_C.KeyMode.on_demand_full) == 4

    # --- Constants tests ---

    def test_mode_constants(self):
        """KeyMode class has the expected string constants."""
        assert KeyMode.ONLINE == "online"
        assert KeyMode.PRELOAD == "preload"
        assert KeyMode.PRELOAD_FULL == "preload_full"
        assert KeyMode.ON_DEMAND == "on_demand"
        assert KeyMode.ON_DEMAND_FULL == "on_demand_full"
        assert KeyMode.KEY_OFFLINE == "key_offline"
        assert KeyMode.SERVER_ONLY == "server_only"

    # --- Multiple cycles ---

    def test_multiple_mode_cycles(self):
        """Switching modes multiple times works correctly."""
        # (input, expected canonical output from get())
        cases = [
            ("online", "online"),
            ("preload", "preload"),
            ("preload_full", "preload_full"),
            ("on_demand", "on_demand"),
            ("on_demand_full", "on_demand_full"),
            ("online", "online"),
            ("key_offline", "preload_full"),  # alias → canonical
            ("server_only", "preload"),  # alias → canonical
            ("online", "online"),
        ]
        for mode, expected in cases:
            KeyMode.apply(mode)
            assert KeyMode.get() == expected, f"apply({mode!r}) → get()={KeyMode.get()!r}, expected {expected!r}"

    # --- Apply with class constant ---

    def test_apply_via_class_constant(self):
        """Using KeyMode constants for apply()."""
        KeyMode.apply(KeyMode.PRELOAD)
        assert KeyMode.get() == "preload"

        KeyMode.apply(KeyMode.PRELOAD_FULL)
        assert KeyMode.get() == "preload_full"


class TestInferenceModeAlias:
    """Test backward-compatible InferenceMode alias."""

    def test_inference_mode_is_key_mode(self):
        """InferenceMode is an alias for KeyMode."""
        assert InferenceMode is KeyMode

    def test_inference_mode_constants(self):
        """InferenceMode has the same constants as KeyMode."""
        assert InferenceMode.ONLINE == KeyMode.ONLINE
        assert InferenceMode.PRELOAD == KeyMode.PRELOAD
        assert InferenceMode.PRELOAD_FULL == KeyMode.PRELOAD_FULL


class TestKeyModeOnDemand:
    """Test on_demand modes which are less commonly used."""

    def setup_method(self):
        _C.set_key_mode(0)

    def teardown_method(self):
        _C.set_key_mode(0)

    def test_on_demand_apply_roundtrip(self):
        """on_demand mode apply/get roundtrip."""
        KeyMode.apply("on_demand")
        assert KeyMode.get() == "on_demand"
        assert _C.get_key_mode() == 2

    def test_on_demand_full_apply_roundtrip(self):
        """on_demand_full mode apply/get roundtrip."""
        KeyMode.apply("on_demand_full")
        assert KeyMode.get() == "on_demand_full"
        assert _C.get_key_mode() == 4

    def test_all_five_modes_to_cpp(self):
        """Verify complete _to_cpp mapping for all 5 canonical modes."""
        expected = {
            "online": 0,
            "preload": 1,
            "on_demand": 2,
            "preload_full": 3,
            "on_demand_full": 4,
        }
        for mode_name, cpp_val in expected.items():
            assert KeyMode._to_cpp(mode_name) == cpp_val, (
                f"_to_cpp({mode_name!r}) = {KeyMode._to_cpp(mode_name)}, expected {cpp_val}"
            )

    def test_all_aliases_to_cpp(self):
        """Verify _to_cpp for backward-compatible aliases."""
        assert KeyMode._to_cpp("key_offline") == 3  # PRELOAD_FULL
        assert KeyMode._to_cpp("server_only") == 1  # PRELOAD

    def test_on_demand_requires_key_path(self):
        """on_demand mode requires key_path."""
        assert "on_demand" in KeyMode._REQUIRES_KEY_PATH
        assert "on_demand_full" in KeyMode._REQUIRES_KEY_PATH

    def test_on_demand_full_needs_full_keys(self):
        """on_demand_full needs full keys (SK+PK)."""
        assert KeyMode._needs_full_keys("on_demand_full") is True
        assert KeyMode._needs_full_keys("on_demand") is False
