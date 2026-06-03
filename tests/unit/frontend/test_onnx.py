# tests/unit/frontend/test_onnx.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Unit tests for ONNX frontend.

Tests the ONNX file input path:
1. prepare() -> ONNXFileIR (bypass mode)
2. compile() -> ONNXFileIR (same as prepare for this frontend)
3. export(format="onnx") -> ONNX file copy
4. export(format="air") -> AIR file (requires C++ extension)
"""
import os
import pytest
import torch

from utils import skip_if_no_frontend, skip_if_no_onnx
from ace.fhe.ir import ONNXFileIR
from ace.sample.ops.specs import LINEAR_OP, ADD_OP, RELU_OP

# Representative specs: single-input (linear, relu) + multi-input (add)
_OP_SPECS = [LINEAR_OP, RELU_OP, ADD_OP]


def _export_onnx(spec, tmp_path, input_names=None):
    """Helper: create an ONNX file from a ModelSpec."""
    import warnings
    model = spec.create_model()
    model.eval()
    onnx_path = str(tmp_path / f"{spec.name}.onnx")
    if input_names is None:
        input_names = spec.encrypt_inputs
    with warnings.catch_warnings():
        warnings.filterwarnings("ignore", category=FutureWarning,
                                message=".*isinstance.*LeafSpec.*")
        warnings.filterwarnings("ignore", category=DeprecationWarning,
                                message=".*legacy TorchScript-based ONNX export.*")
        warnings.filterwarnings("ignore", category=DeprecationWarning,
                                message=".*feature will be removed.*")
        torch.onnx.export(
            model, tuple(spec.example_inputs), onnx_path,
            input_names=input_names, output_names=["output"], opset_version=14
        )
    return onnx_path


# =============================================================================
# Test: prepare() - Bypass Mode
# =============================================================================

@skip_if_no_onnx
class TestPrepare:
    """Tests for prepare() method - Bypass mode."""

    @pytest.mark.parametrize("spec", _OP_SPECS, ids=lambda s: s.name)
    def test_returns_onnx_file_ir(self, onnx_frontend, spec, tmp_path):
        onnx_file = _export_onnx(spec, tmp_path)
        result = onnx_frontend.prepare(onnx_file)
        assert isinstance(result, ONNXFileIR)

    @pytest.mark.parametrize("spec", _OP_SPECS, ids=lambda s: s.name)
    def test_file_mode_properties(self, onnx_frontend, spec, tmp_path):
        """prepare() returns file-mode ONNX IR with valid path."""
        onnx_file = _export_onnx(spec, tmp_path)
        result = onnx_frontend.prepare(onnx_file)
        assert result.format_type == "file"
        assert result.file_format == "onnx"
        assert result.file_path is not None
        assert os.path.exists(result.file_path)

    def test_example_inputs_ignored(self, onnx_frontend, tmp_path):
        onnx_file = _export_onnx(LINEAR_OP, tmp_path)
        result = onnx_frontend.prepare(onnx_file, example_inputs=[torch.randn(1, 4)])
        assert isinstance(result, ONNXFileIR)

    def test_input_names_ignored(self, onnx_frontend, tmp_path):
        onnx_file = _export_onnx(LINEAR_OP, tmp_path)
        result = onnx_frontend.prepare(onnx_file, input_names=["x"])
        assert isinstance(result, ONNXFileIR)


# =============================================================================
# Test: compile() - Returns ONNXFileIR
# =============================================================================

@skip_if_no_onnx
class TestCompile:
    """Tests for compile() method - returns ONNXFileIR."""

    @pytest.mark.parametrize("spec", _OP_SPECS, ids=lambda s: s.name)
    def test_returns_onnx_file_ir(self, onnx_frontend, spec, tmp_path):
        onnx_file = _export_onnx(spec, tmp_path)
        result = onnx_frontend.compile(onnx_file)
        assert isinstance(result, ONNXFileIR)

    def test_compile_with_example_inputs(self, onnx_frontend, tmp_path):
        onnx_file = _export_onnx(LINEAR_OP, tmp_path)
        result = onnx_frontend.compile(onnx_file, example_inputs=[torch.randn(1, 4)])
        assert isinstance(result, ONNXFileIR)


# =============================================================================
# Test: export(format="onnx")
# =============================================================================

@skip_if_no_onnx
class TestExportOnnx:
    """Tests for export(format="onnx") method."""

    @pytest.mark.parametrize("spec", _OP_SPECS, ids=lambda s: s.name)
    def test_creates_onnx_file(self, onnx_frontend, spec, tmp_path):
        onnx_file = _export_onnx(spec, tmp_path)
        output_path = str(tmp_path / "copied.onnx")
        result_path = onnx_frontend.export(onnx_file, format="onnx", output_path=output_path)
        assert result_path == output_path
        assert os.path.exists(output_path)

    def test_returns_original_path_when_no_output_path(self, onnx_frontend, tmp_path):
        onnx_file = _export_onnx(LINEAR_OP, tmp_path)
        result_path = onnx_frontend.export(onnx_file, format="onnx", output_path=None)
        assert result_path == onnx_file

    def test_copies_file_content(self, onnx_frontend, tmp_path):
        import onnx
        onnx_file = _export_onnx(LINEAR_OP, tmp_path)
        output_path = str(tmp_path / "copied.onnx")
        onnx_frontend.export(onnx_file, format="onnx", output_path=output_path)
        original = onnx.load(onnx_file)
        copied = onnx.load(output_path)
        assert original.graph.name == copied.graph.name


# =============================================================================
# Test: export(format="air")
# =============================================================================

@skip_if_no_onnx
@skip_if_no_frontend
class TestExportAir:
    """Tests for export(format="air") method."""

    @pytest.mark.parametrize("spec", _OP_SPECS, ids=lambda s: s.name)
    def test_creates_air_file(self, onnx_frontend, spec, tmp_path):
        onnx_file = _export_onnx(spec, tmp_path)
        output_path = str(tmp_path / "output.B")
        result_path = onnx_frontend.export(onnx_file, format="air", output_path=output_path)
        assert result_path == output_path
        assert os.path.exists(output_path)


# =============================================================================
# Test: IR Properties
# =============================================================================

@skip_if_no_onnx
class TestIRProperties:
    """Tests for IR object properties."""

    @pytest.mark.parametrize("spec", _OP_SPECS, ids=lambda s: s.name)
    def test_onnx_file_ir_properties(self, onnx_frontend, spec, tmp_path):
        onnx_file = _export_onnx(spec, tmp_path)
        result = onnx_frontend.prepare(onnx_file)
        assert hasattr(result, "entry_name")
        assert result.entry_name
        assert hasattr(result, "onnx_path")
        assert result.onnx_path == result.file_path


# =============================================================================
# Test: Edge Cases
# =============================================================================

@skip_if_no_onnx
class TestEdgeCases:
    """Tests for edge cases and error handling."""

    def test_prepare_with_nonexistent_file(self, onnx_frontend, tmp_path):
        nonexistent = str(tmp_path / "nonexistent.onnx")
        result = onnx_frontend.prepare(nonexistent)
        assert isinstance(result, ONNXFileIR)
        assert result.file_path == nonexistent

    def test_prepare_with_invalid_onnx_file(self, onnx_frontend, tmp_path):
        invalid_path = str(tmp_path / "invalid.onnx")
        with open(invalid_path, "w") as f:
            f.write("not a valid onnx file")
        result = onnx_frontend.prepare(invalid_path)
        assert isinstance(result, ONNXFileIR)

    def test_export_onnx_to_valid_path(self, onnx_frontend, tmp_path):
        onnx_file = _export_onnx(LINEAR_OP, tmp_path)
        output_path = str(tmp_path / "output.onnx")
        result_path = onnx_frontend.export(onnx_file, format="onnx", output_path=output_path)
        assert os.path.exists(result_path)


# =============================================================================
# Test: Frontend Metadata
# =============================================================================

@skip_if_no_onnx
class TestFrontendMeta:
    """Tests for frontend metadata."""

    def test_frontend_name(self, onnx_frontend):
        assert onnx_frontend.name() == "onnx"