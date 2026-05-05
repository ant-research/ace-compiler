# tests/test_unit/test_frontend/test_onnx_frontend.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Unit tests for onnx frontend.

Tests three output modes:
1. Bypass: prepare() -> ONNXFileIR -> backend
2. AIR file: export(format="air") -> .B file -> backend
3. Memory: compile() -> FHEProgram -> backend (NOT IMPLEMENTED)
"""
import pytest
import torch
import torch.nn as nn
import os

from ace.fhe.frontend import get_frontend
from ace.fhe.ir import ONNXFileIR

from ace.samples.ops import LinearOp as _LinearOp, AddOp as AddModel


class LinearOp(_LinearOp):
    """Linear layer with 4 -> 4 dimensions."""
    def __init__(self):
        super().__init__(4, 4)


# =============================================================================
# Fixtures
# =============================================================================

@pytest.fixture
def frontend():
    """Fixture for onnx frontend."""
    return get_frontend("onnx")


@pytest.fixture
def onnx_file(tmp_path):
    """Fixture that creates an ONNX file for testing."""
    model = LinearOp()
    onnx_path = str(tmp_path / "test_model.onnx")
    torch.onnx.export(
        model, torch.randn(1, 4), onnx_path,
        input_names=["input"], output_names=["output"], opset_version=14
    )
    return onnx_path


@pytest.fixture
def multi_input_onnx_file(tmp_path):
    """Fixture that creates an ONNX file with multiple inputs."""
    model = AddModel()
    onnx_path = str(tmp_path / "multi_input.onnx")
    torch.onnx.export(
        model, (torch.randn(1, 4), torch.randn(1, 4)), onnx_path,
        input_names=["x", "y"], output_names=["output"], opset_version=14
    )
    return onnx_path


@pytest.fixture
def fhe_cmplr_available():
    """Check if fhe_cmplr is available."""
    return os.path.exists("./build/_deps/fhe-cmplr-build/driver/fhe_cmplr")


# =============================================================================
# Test: prepare() - Bypass Mode
# =============================================================================

class TestPrepare:
    """Tests for prepare() method - Bypass mode."""

    def test_returns_onnx_file_ir(self, frontend, onnx_file):
        """Test that prepare() returns ONNXFileIR."""
        result = frontend.prepare(onnx_file)
        assert isinstance(result, ONNXFileIR)

    def test_ir_format_type_is_file(self, frontend, onnx_file):
        """Test that format_type is 'file'."""
        result = frontend.prepare(onnx_file)
        assert result.format_type == "file"

    def test_ir_file_format_is_onnx(self, frontend, onnx_file):
        """Test that file_format is 'onnx'."""
        result = frontend.prepare(onnx_file)
        assert result.file_format == "onnx"

    def test_ir_has_valid_file_path(self, frontend, onnx_file):
        """Test that file_path points to existing file."""
        result = frontend.prepare(onnx_file)
        assert result.file_path is not None
        assert os.path.exists(result.file_path)

    def test_with_multi_input_onnx(self, frontend, multi_input_onnx_file):
        """Test prepare() with multi-input ONNX file."""
        result = frontend.prepare(multi_input_onnx_file)
        assert isinstance(result, ONNXFileIR)
        assert os.path.exists(result.file_path)

    def test_example_inputs_ignored(self, frontend, onnx_file):
        """Test that example_inputs parameter is ignored."""
        result = frontend.prepare(onnx_file, example_inputs=[torch.randn(1, 4)])
        assert isinstance(result, ONNXFileIR)

    def test_input_names_ignored(self, frontend, onnx_file):
        """Test that input_names parameter is ignored."""
        result = frontend.prepare(onnx_file, input_names=["x"])
        assert isinstance(result, ONNXFileIR)


# =============================================================================
# Test: compile() - Memory Mode
# =============================================================================

class TestCompile:
    """Tests for compile() method - returns ONNXFileIR."""

    def test_returns_onnx_file_ir(self, frontend, onnx_file):
        """Test that compile() returns ONNXFileIR."""
        from ace.fhe.ir.io.file_ir import ONNXFileIR
        result = frontend.compile(onnx_file)
        assert isinstance(result, ONNXFileIR)

    def test_compile_with_example_inputs(self, frontend, onnx_file):
        """Test that compile() with example_inputs works."""
        from ace.fhe.ir.io.file_ir import ONNXFileIR
        result = frontend.compile(onnx_file, example_inputs=[torch.randn(1, 4)])
        assert isinstance(result, ONNXFileIR)


# =============================================================================
# Test: export(format="onnx")
# =============================================================================

class TestExportOnnx:
    """Tests for export(format="onnx") method."""

    def test_creates_onnx_file(self, frontend, onnx_file, tmp_path):
        """Test that export(format="onnx") creates ONNX file."""
        output_path = str(tmp_path / "copied.onnx")
        result_path = frontend.export(onnx_file, format="onnx", output_path=output_path)
        assert result_path == output_path
        assert os.path.exists(output_path)

    def test_returns_original_path_when_no_output_path(self, frontend, onnx_file):
        """Test that export returns original path when output_path not specified."""
        result_path = frontend.export(onnx_file, format="onnx", output_path=None)
        assert result_path == onnx_file

    def test_copies_file_content(self, frontend, onnx_file, tmp_path):
        """Test that export copies file content correctly."""
        output_path = str(tmp_path / "copied.onnx")
        frontend.export(onnx_file, format="onnx", output_path=output_path)
        import onnx
        original = onnx.load(onnx_file)
        copied = onnx.load(output_path)
        assert original.graph.name == copied.graph.name

    def test_with_multi_input_onnx(self, frontend, multi_input_onnx_file, tmp_path):
        """Test export ONNX with multi-input file."""
        output_path = str(tmp_path / "multi_copied.onnx")
        result_path = frontend.export(multi_input_onnx_file, format="onnx", output_path=output_path)
        assert os.path.exists(result_path)


# =============================================================================
# Test: export(format="air")
# =============================================================================

class TestExportAir:
    """Tests for export(format="air") method."""

    def test_creates_air_file(self, frontend, onnx_file, tmp_path, fhe_cmplr_available):
        """Test that export(format="air") creates .B file."""
        if not fhe_cmplr_available:
            pytest.skip("fhe_cmplr not available")
        output_path = str(tmp_path / "output.B")
        result_path = frontend.export(onnx_file, format="air", output_path=output_path)
        assert result_path == output_path
        assert os.path.exists(output_path)

    def test_with_multi_input_onnx(self, frontend, multi_input_onnx_file, tmp_path, fhe_cmplr_available):
        """Test export AIR with multi-input ONNX file."""
        if not fhe_cmplr_available:
            pytest.skip("fhe_cmplr not available")
        output_path = str(tmp_path / "multi.B")
        result_path = frontend.export(multi_input_onnx_file, format="air", output_path=output_path)
        assert os.path.exists(result_path)


# =============================================================================
# Test: IR Properties
# =============================================================================

class TestIRProperties:
    """Tests for IR object properties."""

    def test_onnx_file_ir_has_entry_name(self, frontend, onnx_file):
        """Test that ONNXFileIR has entry_name."""
        result = frontend.prepare(onnx_file)
        assert hasattr(result, "entry_name")
        assert result.entry_name

    def test_onnx_file_ir_has_onnx_path(self, frontend, onnx_file):
        """Test that ONNXFileIR has onnx_path (backward compatibility)."""
        result = frontend.prepare(onnx_file)
        assert hasattr(result, "onnx_path")
        assert result.onnx_path == result.file_path


# =============================================================================
# Test: Edge Cases
# =============================================================================

class TestEdgeCases:
    """Tests for edge cases and error handling."""

    def test_prepare_with_nonexistent_file(self, frontend):
        """Test prepare() with nonexistent file."""
        result = frontend.prepare("/nonexistent/path/model.onnx")
        assert isinstance(result, ONNXFileIR)
        assert result.file_path == "/nonexistent/path/model.onnx"

    def test_prepare_with_invalid_onnx_file(self, frontend, tmp_path):
        """Test prepare() with invalid ONNX file."""
        invalid_path = str(tmp_path / "invalid.onnx")
        with open(invalid_path, "w") as f:
            f.write("not a valid onnx file")
        result = frontend.prepare(invalid_path)
        assert isinstance(result, ONNXFileIR)

    def test_export_onnx_to_valid_path(self, frontend, onnx_file, tmp_path):
        """Test export ONNX to valid path."""
        output_path = str(tmp_path / "output.onnx")
        result_path = frontend.export(onnx_file, format="onnx", output_path=output_path)
        assert os.path.exists(result_path)


# =============================================================================
# Test: Frontend Metadata
# =============================================================================

class TestFrontendMeta:
    """Tests for frontend metadata."""

    def test_frontend_name(self, frontend):
        """Test frontend name."""
        assert frontend.name() == "onnx"


# Run tests if executed directly
if __name__ == "__main__":
    pytest.main([__file__, "-v"])