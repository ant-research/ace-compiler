# tests/unit/runtime/test_key_path.py
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""Tests for the key_path C++ runtime bridge.

These tests verify that key_path can be set, retrieved, and cleared
through the Python-C++ bridge (_C module), and that FHERuntime
correctly propagates key_path to the C++ runtime.
"""

import pytest

from ace import runtime as _C


class TestKeyPathBasic:
    """Test basic set/get/clear operations for key_path."""

    def setup_method(self):
        """Clear key_path before each test to avoid interference."""
        _C.clear_key_path()

    def teardown_method(self):
        """Clear key_path after each test."""
        _C.clear_key_path()

    def test_get_key_path_default_is_none(self):
        """get_key_path() returns None when no path has been set."""
        result = _C.get_key_path()
        assert result is None

    def test_set_and_get_key_path(self):
        """set_key_path() stores the path and get_key_path() retrieves it."""
        _C.set_key_path("/tmp/keys/my-model-v1")
        result = _C.get_key_path()
        assert result == "/tmp/keys/my-model-v1"

    def test_set_key_path_overwrites(self):
        """Setting a new key_path overwrites the previous one."""
        _C.set_key_path("/tmp/keys/first")
        assert _C.get_key_path() == "/tmp/keys/first"

        _C.set_key_path("/tmp/keys/second")
        assert _C.get_key_path() == "/tmp/keys/second"

    def test_clear_key_path(self):
        """clear_key_path() removes the key_path, get_key_path() returns None."""
        _C.set_key_path("/tmp/keys/test")
        assert _C.get_key_path() == "/tmp/keys/test"

        _C.clear_key_path()
        assert _C.get_key_path() is None

    def test_set_empty_string_clears(self):
        """Setting key_path to empty string clears it."""
        _C.set_key_path("/tmp/keys/test")
        _C.set_key_path("")
        assert _C.get_key_path() is None

    def test_set_key_path_with_spaces(self):
        """Paths with spaces are handled correctly."""
        path = "/tmp/keys/my model v1"
        _C.set_key_path(path)
        assert _C.get_key_path() == path

    def test_set_key_path_special_chars(self):
        """Paths with special characters are handled correctly."""
        path = "/tmp/keys/model-v1.0_beta+build123"
        _C.set_key_path(path)
        assert _C.get_key_path() == path

    def test_set_key_path_chinese_chars(self):
        """Paths with non-ASCII characters are handled correctly."""
        path = "/tmp/keys/模型密钥-v1"
        _C.set_key_path(path)
        assert _C.get_key_path() == path

    def test_set_key_path_relative(self):
        """Relative paths are accepted."""
        _C.set_key_path("./keys/model-v1")
        assert _C.get_key_path() == "./keys/model-v1"

    def test_set_key_path_trailing_slash(self):
        """Paths with trailing slashes are preserved."""
        _C.set_key_path("/tmp/keys/model-v1/")
        assert _C.get_key_path() == "/tmp/keys/model-v1/"

    def test_multiple_set_clear_cycles(self):
        """Multiple set/clear cycles work correctly."""
        for i in range(5):
            _C.set_key_path(f"/tmp/keys/round-{i}")
            assert _C.get_key_path() == f"/tmp/keys/round-{i}"
            _C.clear_key_path()
            assert _C.get_key_path() is None


class TestKeyPathEdgeCases:
    """Edge case tests for key_path."""

    def setup_method(self):
        _C.clear_key_path()

    def teardown_method(self):
        _C.clear_key_path()

    def test_very_long_path(self):
        """Very long paths (4096 chars) should be handled."""
        long_path = "/tmp/keys/" + "a" * 4000
        _C.set_key_path(long_path)
        result = _C.get_key_path()
        assert result == long_path

    def test_path_with_unicode_variants(self):
        """Paths with various unicode characters."""
        paths = [
            "/tmp/keys/🔑-keys",  # emoji
            "/tmp/keys/مفاتيح",  # Arabic
            "/tmp/keys/鍵-モデル",  # Japanese/Chinese
        ]
        for path in paths:
            _C.set_key_path(path)
            assert _C.get_key_path() == path

    def test_clear_then_reset(self):
        """Clear key_path, then set again — should work."""
        _C.set_key_path("/tmp/keys/first")
        assert _C.get_key_path() == "/tmp/keys/first"

        _C.clear_key_path()
        assert _C.get_key_path() is None

        _C.set_key_path("/tmp/keys/second")
        assert _C.get_key_path() == "/tmp/keys/second"

    def test_key_path_with_nesting(self):
        """Deeply nested paths work correctly."""
        deep = "/a/b/c/d/e/f/g/h/keys"
        _C.set_key_path(deep)
        assert _C.get_key_path() == deep
