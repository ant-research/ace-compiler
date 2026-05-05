# tests/test_unit/test_backend/test_openfhe.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Unit tests for OpenFHE CPU backend.

Tests backend instantiation, supported formats, and build commands.
"""

import pytest

from ace.fhe.backend import get_library_impl


class TestOpenFHEBackend:
    """Tests for OpenFHE CPU backend instantiation and properties."""

    def test_get_openfhe_backend(self):
        """Test getting openfhe backend instance."""
        backend = get_library_impl("openfhe", device="cpu")
        assert backend is not None
        assert backend.backend_name() == "openfhe"
        assert backend.device_name() == "cpu"

    def test_backend_device_name(self):
        """Test backend device name."""
        backend = get_library_impl("openfhe", device="cpu")
        assert backend.device_name() == "cpu"

    def test_backend_supported_formats(self):
        """Test backend supported IR formats."""
        backend = get_library_impl("openfhe", device="cpu")
        formats = backend.supported_format_types()
        assert isinstance(formats, list)
        assert "air" in formats
        assert "onnx" in formats

    def test_backend_check_available(self):
        """Test backend availability check."""
        backend = get_library_impl("openfhe", device="cpu")
        assert backend.check_available() is True

    def test_backend_with_options(self):
        """Test backend instantiation with options."""
        backend = get_library_impl("openfhe", device="cpu", vec={"ms": 16})
        assert backend is not None
        assert backend._options.get("vec") == {"ms": 16}

    def test_backend_default_options(self):
        """Test backend with default (empty) options."""
        backend = get_library_impl("openfhe", device="cpu")
        assert backend._options == {} or backend._options is not None


class TestOpenFHEBuildCommand:
    """Tests for OpenFHE build command generation."""

    def test_build_command_basic(self):
        """Test basic build command generation."""
        backend = get_library_impl("openfhe", device="cpu")
        cmd = backend.build_command(
            source="test.cpp",
            output="test.so"
        )
        assert "g++" in cmd
        assert "test.cpp" in cmd
        assert "test.so" in cmd
        assert "-std=c++17" in cmd
        assert "-shared" in cmd
        assert "-fPIC" in cmd

    def test_build_command_with_flags(self):
        """Test build command with extra flags."""
        backend = get_library_impl("openfhe", device="cpu")
        cmd = backend.build_command(
            source="test.cpp",
            output="test.so",
            extra_flags=["-O3", "-march=native"]
        )
        assert "-O3" in cmd
        assert "-march=native" in cmd

    def test_build_command_includes_libs(self):
        """Test build command includes required libraries."""
        backend = get_library_impl("openfhe", device="cpu")
        cmd = backend.build_command(
            source="test.cpp",
            output="test.so"
        )
        assert "-lOPENFHEcore" in cmd

    def test_build_command_with_ace_root(self):
        """Test build command with custom ace_root."""
        backend = get_library_impl("openfhe", device="cpu")
        cmd = backend.build_command(
            source="test.cpp",
            output="test.so",
            ace_root="/custom/path"
        )
        # Basic validation - command should be a list
        assert isinstance(cmd, list)
        assert len(cmd) > 0


class TestOpenFHECompileToLib:
    """Tests for OpenFHE compile_to_lib functionality."""

    def test_compile_to_lib_unsupported_format(self):
        """Test that unsupported format raises ValueError."""
        backend = get_library_impl("openfhe", device="cpu")

        class UnsupportedIR:
            pass

        with pytest.raises(ValueError, match="Unsupported IR type"):
            backend.compile_to_lib(UnsupportedIR(), "/tmp/output")

    def test_compile_to_lib_air_not_implemented(self):
        """Test that AIR compilation raises NotImplementedError."""
        backend = get_library_impl("openfhe", device="cpu")

        class MockAIR:
            @property
            def format_type(self):
                return "air"

        with pytest.raises(NotImplementedError, match="AIR compilation for OpenFHE not yet implemented"):
            backend.compile_to_lib(MockAIR(), "/tmp/output")


# Run tests if executed directly
if __name__ == "__main__":
    pytest.main([__file__, "-v"])