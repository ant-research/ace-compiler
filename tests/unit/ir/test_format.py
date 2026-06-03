# tests/unit/ir/test_format.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Unit tests for IR file format classes.

Tests FileIR, ONNXFileIR, AIRFileIR, and backward compatibility aliases.
"""

import pytest

from ace.fhe.ir import FileIR, ONNXFileIR, AIRFileIR
from ace.fhe.ir.io.file_ir import ONNXModel, AIRModel, FileModel


# =============================================================================
# ONNXFileIR
# =============================================================================

class TestONNXFileIR:
    """Tests for ONNXFileIR."""

    def test_format_type_is_file(self, tmp_path):
        ir = ONNXFileIR(str(tmp_path / "model.onnx"))
        assert ir.format_type == "file"

    def test_file_format_is_onnx(self, tmp_path):
        ir = ONNXFileIR(str(tmp_path / "model.onnx"))
        assert ir.file_format == "onnx"

    def test_file_path(self, tmp_path):
        path = str(tmp_path / "model.onnx")
        ir = ONNXFileIR(path)
        assert ir.file_path == path

    def test_onnx_path_alias(self, tmp_path):
        path = str(tmp_path / "model.onnx")
        ir = ONNXFileIR(path)
        assert ir.onnx_path == path

    @pytest.mark.parametrize("filename,expected", [
        ("resnet20.onnx", "resnet20"),
        ("model.v2.onnx", "model.v2"),
    ])
    def test_entry_name(self, tmp_path, filename, expected):
        ir = ONNXFileIR(str(tmp_path / filename))
        assert ir.entry_name == expected


# =============================================================================
# AIRFileIR
# =============================================================================

class TestAIRFileIR:
    """Tests for AIRFileIR."""

    def test_format_type_is_file(self, tmp_path):
        ir = AIRFileIR(str(tmp_path / "model.B"))
        assert ir.format_type == "file"

    def test_file_format_is_air(self, tmp_path):
        ir = AIRFileIR(str(tmp_path / "model.B"))
        assert ir.file_format == "air"

    def test_file_path(self, tmp_path):
        path = str(tmp_path / "model.B")
        ir = AIRFileIR(path)
        assert ir.file_path == path

    def test_air_path_alias(self, tmp_path):
        path = str(tmp_path / "model.B")
        ir = AIRFileIR(path)
        assert ir.air_path == path

    def test_entry_name(self, tmp_path):
        ir = AIRFileIR(str(tmp_path / "compiled_model.B"))
        assert ir.entry_name == "compiled_model"


# =============================================================================
# Cross-format
# =============================================================================

class TestIRFormatComparison:
    """Tests comparing ONNX and AIR IR formats."""

    def test_different_file_formats(self, tmp_path):
        onnx_ir = ONNXFileIR(str(tmp_path / "model.onnx"))
        air_ir = AIRFileIR(str(tmp_path / "model.B"))
        assert onnx_ir.file_format != air_ir.file_format
        assert onnx_ir.format_type == air_ir.format_type == "file"

    def test_same_entry_name_from_same_stem(self, tmp_path):
        onnx_ir = ONNXFileIR(str(tmp_path / "test_model.onnx"))
        air_ir = AIRFileIR(str(tmp_path / "test_model.B"))
        assert onnx_ir.entry_name == air_ir.entry_name == "test_model"


# =============================================================================
# Backward Compatibility
# =============================================================================

class TestBackwardCompatibility:
    """Tests for backward compatibility aliases."""

    def test_onnx_model_alias(self):
        assert ONNXModel is ONNXFileIR

    def test_air_model_alias(self):
        assert AIRModel is AIRFileIR

    def test_file_model_alias(self):
        assert FileModel is FileIR

    def test_onnx_model_creates_instance(self, tmp_path):
        ir = ONNXModel(str(tmp_path / "test.onnx"))
        assert isinstance(ir, ONNXFileIR)
        assert ir.file_format == "onnx"

    def test_air_model_creates_instance(self, tmp_path):
        ir = AIRModel(str(tmp_path / "test.B"))
        assert isinstance(ir, AIRFileIR)
        assert ir.file_format == "air"