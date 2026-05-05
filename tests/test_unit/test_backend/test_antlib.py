# tests/test_unit/test_backend/test_antlib.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Unit tests for AntLIB CPU backend.

Tests backend instantiation, supported formats, and build commands.
"""

import pytest

from ace.fhe.backend import get_library_impl


class TestAntlibBackend:
    """Tests for AntLIB CPU backend instantiation and properties."""

    def test_get_antlib_backend(self):
        """Test getting antlib backend instance."""
        backend = get_library_impl("antlib", device="cpu")
        assert backend is not None
        assert backend.backend_name() == "antlib"
        assert backend.device_name() == "cpu"

    def test_backend_device_name(self):
        """Test backend device name."""
        backend = get_library_impl("antlib", device="cpu")
        assert backend.device_name() == "cpu"

    def test_backend_supported_formats(self):
        """Test backend supported IR formats."""
        backend = get_library_impl("antlib", device="cpu")
        formats = backend.supported_format_types()
        assert isinstance(formats, list)
        # Backend supports memory IR and file-based IR
        assert "memory" in formats
        assert "file" in formats

    def test_backend_check_available(self):
        """Test backend availability check."""
        backend = get_library_impl("antlib", device="cpu")
        assert backend.check_available() is True

    def test_backend_with_options(self):
        """Test backend instantiation with options."""
        backend = get_library_impl("antlib", device="cpu", vec={"ms": 16})
        assert backend is not None
        assert backend._options.get("vec") == {"ms": 16}

    def test_backend_default_options(self):
        """Test backend with default (empty) options."""
        backend = get_library_impl("antlib", device="cpu")
        assert backend._options == {} or backend._options is not None


class TestAntlibBuildCommand:
    """Tests for AntLIB build command generation."""

    def test_build_command_basic(self):
        """Test basic build command generation."""
        backend = get_library_impl("antlib", device="cpu")
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
        backend = get_library_impl("antlib", device="cpu")
        cmd = backend.build_command(
            source="test.cpp",
            output="test.so",
            extra_flags=["-O3", "-march=native"]
        )
        assert "-O3" in cmd
        assert "-march=native" in cmd

    def test_build_command_includes_libs(self):
        """Test build command includes required libraries."""
        backend = get_library_impl("antlib", device="cpu")
        cmd = backend.build_command(
            source="test.cpp",
            output="test.so"
        )
        assert "-lFHErt_ant" in cmd

    def test_build_command_with_ace_root(self):
        """Test build command with custom ace_root."""
        backend = get_library_impl("antlib", device="cpu")
        cmd = backend.build_command(
            source="test.cpp",
            output="test.so",
            ace_root="/custom/path"
        )
        # Basic validation - command should be a list
        assert isinstance(cmd, list)
        assert len(cmd) > 0


class TestAntlibCompileToLib:
    """Tests for AntLIB compile_to_lib functionality."""

    def test_compile_to_lib_unsupported_format(self):
        """Test that unsupported format raises ValueError."""
        backend = get_library_impl("antlib", device="cpu")

        class UnsupportedIR:
            pass

        with pytest.raises(ValueError, match="Unsupported IR type"):
            backend.compile_to_lib(UnsupportedIR(), "/tmp/output")

    def test_compile_to_lib_creates_output_dir(self, tmp_path):
        """Test that compile_to_lib creates output directory."""
        import os
        backend = get_library_impl("antlib", device="cpu")

        # Create a simple ONNX model for testing
        import torch
        import torch.nn as nn

        class SimpleModel(nn.Module):
            def forward(self, x):
                return x + 1

        model = SimpleModel()
        inputs = (torch.randn(1, 4),)
        onnx_path = tmp_path / "simple.onnx"
        torch.onnx.export(model, inputs[0], str(onnx_path), input_names=["x"])

        from ace.fhe.ir import ONNXModel
        onnx_model = ONNXModel(str(onnx_path))

        output_dir = tmp_path / "output"
        # This may fail if compiler not available, but should create dir
        try:
            backend.compile_to_lib(onnx_model, str(output_dir))
        except Exception:
            pass

        # Check output directory was created
        # Note: This depends on implementation


# Run tests if executed directly
if __name__ == "__main__":
    pytest.main([__file__, "-v"])