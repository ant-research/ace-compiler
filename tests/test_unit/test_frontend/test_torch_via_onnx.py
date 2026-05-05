# tests/test_unit/test_frontend/test_torch_via_onnx.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Unit tests for torch-via-onnx frontend.

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

from ace.samples.ops import LinearOp as _LinearOp
from ace.samples.funcs import add_func, relu_func


class LinearOp(_LinearOp):
    """Linear layer with 4 -> 4 dimensions."""
    def __init__(self):
        super().__init__(4, 4)


def simple_function(x):
    """Simple function that adds 1."""
    return x + 1


# =============================================================================
# Fixtures
# =============================================================================

@pytest.fixture
def frontend():
    """Fixture for torch-via-onnx frontend."""
    return get_frontend("torch-via-onnx")


@pytest.fixture
def model():
    """Fixture for nn.Module."""
    return LinearOp()


@pytest.fixture
def function():
    """Fixture for callable function."""
    return simple_function


@pytest.fixture
def model_inputs():
    """Fixture for model inputs."""
    return [torch.randn(1, 4)]


@pytest.fixture
def function_inputs():
    """Fixture for function inputs."""
    return [torch.randn(1, 4), torch.randn(1, 4)]


# =============================================================================
# Test: prepare() - Bypass Mode
# =============================================================================

class TestPrepare:
    """Tests for prepare() method - Bypass mode."""

    def test_returns_onnx_file_ir(self, frontend, model, model_inputs):
        """Test that prepare() returns ONNXFileIR."""
        result = frontend.prepare(model, model_inputs, ["x"])
        assert isinstance(result, ONNXFileIR)

    def test_format_type_is_file(self, frontend, model, model_inputs):
        """Test that format_type is 'file'."""
        result = frontend.prepare(model, model_inputs, ["x"])
        assert result.format_type == "file"

    def test_file_format_is_onnx(self, frontend, model, model_inputs):
        """Test that file_format is 'onnx'."""
        result = frontend.prepare(model, model_inputs, ["x"])
        assert result.file_format == "onnx"

    def test_with_multi_input_function(self, frontend, function_inputs):
        """Test prepare() with multi-input function."""
        result = frontend.prepare(add_func, function_inputs, ["x", "y"])
        assert isinstance(result, ONNXFileIR)


# =============================================================================
# Test: compile() - Memory Mode (NOT IMPLEMENTED)
# =============================================================================

class TestCompile:
    """Tests for compile() method - returns ONNXFileIR."""

    def test_returns_onnx_file_ir(self, frontend, model, model_inputs):
        """Test that compile() returns ONNXFileIR."""
        result = frontend.compile(model, model_inputs, ["x"])
        assert isinstance(result, ONNXFileIR)

    def test_compile_with_example_inputs(self, frontend, model, model_inputs):
        """Test that compile() with example_inputs works."""
        result = frontend.compile(model, model_inputs, ["x"])
        assert isinstance(result, ONNXFileIR)


# =============================================================================
# Test: export(format="onnx")
# =============================================================================

class TestExportOnnx:
    """Tests for export(format="onnx") method."""

    @pytest.mark.skip(reason="torch-via-onnx ONNX export not working")
    def test_creates_onnx_file(self, frontend, model, model_inputs, tmp_path):
        """Test that export(format="onnx") creates ONNX file."""
        output_path = str(tmp_path / "output.onnx")
        result_path = frontend.export(model, model_inputs, ["x"],
                                       format="onnx", output_path=output_path)
        assert result_path == output_path
        assert os.path.exists(output_path)

    @pytest.mark.skip(reason="torch-via-onnx ONNX export not working")
    def test_creates_valid_onnx(self, frontend, model, model_inputs, tmp_path):
        """Test that exported ONNX file is valid."""
        output_path = str(tmp_path / "output.onnx")
        frontend.export(model, model_inputs, ["x"],
                        format="onnx", output_path=output_path)
        import onnx
        onnx_model = onnx.load(output_path)
        onnx.checker.check_model(onnx_model)


# =============================================================================
# Test: export(format="air")
# =============================================================================

class TestExportAir:
    """Tests for export(format="air") method."""

    @pytest.fixture
    def fhe_cmplr_available(self):
        """Check if fhe_cmplr is available."""
        return os.path.exists("./build/_deps/fhe-cmplr-build/driver/fhe_cmplr")

    @pytest.mark.skip(reason="torch-via-onnx AIR export not working")
    def test_creates_air_file(self, frontend, model, model_inputs, tmp_path, fhe_cmplr_available):
        """Test that export(format="air") creates .B file."""
        if not fhe_cmplr_available:
            pytest.skip("fhe_cmplr not available")
        output_path = str(tmp_path / "output.B")
        result_path = frontend.export(model, model_inputs, ["x"],
                                       format="air", output_path=output_path)
        assert result_path == output_path
        assert os.path.exists(output_path)


# =============================================================================
# Test: IR Properties
# =============================================================================

class TestIRProperties:
    """Tests for IR object properties."""

    def test_onnx_file_ir_has_entry_name(self, frontend, model, model_inputs):
        """Test that ONNXFileIR has entry_name."""
        result = frontend.prepare(model, model_inputs, ["x"])
        assert hasattr(result, "entry_name")
        assert result.entry_name

    def test_onnx_file_ir_has_onnx_path(self, frontend, model, model_inputs):
        """Test that ONNXFileIR has onnx_path (backward compatibility)."""
        result = frontend.prepare(model, model_inputs, ["x"])
        assert hasattr(result, "onnx_path")
        assert result.onnx_path == result.file_path


# =============================================================================
# Test: Edge Cases
# =============================================================================

class TestEdgeCases:
    """Tests for edge cases and error handling."""

    def test_prepare_with_none_input(self, frontend):
        """Test prepare() with None input raises appropriate error."""
        from torch.onnx._internal.exporter._errors import TorchExportError
        with pytest.raises((TypeError, AttributeError, TorchExportError)):
            frontend.prepare(None, [torch.randn(1, 4)], ["x"])

    def test_prepare_with_empty_inputs(self, frontend, model):
        """Test prepare() with empty inputs list raises AssertionError."""
        with pytest.raises(AssertionError):
            frontend.prepare(model, [], ["x"])

    def test_prepare_with_wrong_input_count(self, frontend, model):
        """Test prepare() with wrong number of inputs."""
        inputs = [torch.randn(1, 4), torch.randn(1, 4)]
        with pytest.raises(AssertionError):
            frontend.prepare(model, inputs, ["x"])


# =============================================================================
# Test: Frontend Metadata
# =============================================================================

class TestFrontendMeta:
    """Tests for frontend metadata."""

    def test_frontend_name(self, frontend):
        """Test frontend name."""
        assert frontend.name() == "torch-via-onnx"


# Run tests if executed directly
if __name__ == "__main__":
    pytest.main([__file__, "-v"])