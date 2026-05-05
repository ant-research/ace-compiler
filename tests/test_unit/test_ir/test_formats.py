# tests/test_unit/test_ir/test_formats.py
"""
Unit tests for IR file format classes.

Tests for:
- FileIR: Base class for file-based IR
- ONNXFileIR: ONNX file format
- AIRFileIR: AIR file format
"""
import pytest

from ace.fhe.ir import FileIR, ONNXFileIR, AIRFileIR
from ace.fhe.ir.io.file_ir import ONNXModel, AIRModel, FileModel


# ============================================================================
# FileIR Tests
# ============================================================================

class TestFileIR:
    """Tests for FileIR base class."""

    def test_format_type_is_file(self):
        """Test that format_type returns 'file'."""
        ir = ONNXFileIR("/tmp/test.onnx")
        assert ir.format_type == "file"

    def test_file_path_property(self):
        """Test that file_path returns the file path."""
        ir = ONNXFileIR("/tmp/test.onnx")
        assert ir.file_path == "/tmp/test.onnx"

    def test_entry_name_from_path(self):
        """Test that entry_name is derived from file path."""
        ir = ONNXFileIR("/tmp/my_model.onnx")
        assert ir.entry_name == "my_model"


# ============================================================================
# ONNXFileIR Tests
# ============================================================================

class TestONNXFileIR:
    """Tests for ONNXFileIR class."""

    def test_create_with_path(self):
        """Test ONNXFileIR creation with path."""
        ir = ONNXFileIR("/tmp/model.onnx")
        assert ir.file_path == "/tmp/model.onnx"

    def test_file_format_is_onnx(self):
        """Test that file_format returns 'onnx'."""
        ir = ONNXFileIR("/tmp/model.onnx")
        assert ir.file_format == "onnx"

    def test_onnx_path_attribute(self):
        """Test that onnx_path attribute exists for backward compatibility."""
        ir = ONNXFileIR("/tmp/model.onnx")
        assert ir.onnx_path == "/tmp/model.onnx"

    def test_entry_name(self):
        """Test entry_name derived from ONNX file."""
        ir = ONNXFileIR("/path/to/resnet20.onnx")
        assert ir.entry_name == "resnet20"

    def test_entry_name_with_dots(self):
        """Test entry_name with multiple dots in filename."""
        ir = ONNXFileIR("/path/to/model.v2.onnx")
        assert ir.entry_name == "model.v2"


# ============================================================================
# AIRFileIR Tests
# ============================================================================

class TestAIRFileIR:
    """Tests for AIRFileIR class."""

    def test_create_with_path(self):
        """Test AIRFileIR creation with path."""
        ir = AIRFileIR("/tmp/model.B")
        assert ir.file_path == "/tmp/model.B"

    def test_file_format_is_air(self):
        """Test that file_format returns 'air'."""
        ir = AIRFileIR("/tmp/model.B")
        assert ir.file_format == "air"

    def test_air_path_attribute(self):
        """Test that air_path attribute exists for backward compatibility."""
        ir = AIRFileIR("/tmp/model.B")
        assert ir.air_path == "/tmp/model.B"

    def test_entry_name(self):
        """Test entry_name derived from AIR file."""
        ir = AIRFileIR("/path/to/compiled_model.B")
        assert ir.entry_name == "compiled_model"


# ============================================================================
# IR Formats Comparison Tests
# ============================================================================

class TestIRFormatsComparison:
    """Tests comparing different IR formats."""

    def test_onnx_vs_air_file_format(self):
        """Test that ONNX and AIR have different file formats."""
        onnx_ir = ONNXFileIR("/tmp/model.onnx")
        air_ir = AIRFileIR("/tmp/model.B")

        assert onnx_ir.file_format == "onnx"
        assert air_ir.file_format == "air"
        assert onnx_ir.format_type == air_ir.format_type  # Both are "file"

    def test_both_have_file_path(self):
        """Test that both ONNX and AIR have file_path."""
        onnx_ir = ONNXFileIR("/tmp/model.onnx")
        air_ir = AIRFileIR("/tmp/model.B")

        assert onnx_ir.file_path is not None
        assert air_ir.file_path is not None

    def test_both_derive_entry_name_from_path(self):
        """Test that both derive entry_name from path."""
        onnx_ir = ONNXFileIR("/models/test_model.onnx")
        air_ir = AIRFileIR("/models/test_model.B")

        assert onnx_ir.entry_name == "test_model"
        assert air_ir.entry_name == "test_model"


# ============================================================================
# Backward Compatibility Tests
# ============================================================================

class TestBackwardCompatibility:
    """Tests for backward compatibility aliases."""

    def test_onnx_model_alias(self):
        """Test that ONNXModel is an alias for ONNXFileIR."""
        assert ONNXModel is ONNXFileIR

    def test_air_model_alias(self):
        """Test that AIRModel is an alias for AIRFileIR."""
        assert AIRModel is AIRFileIR

    def test_file_model_alias(self):
        """Test that FileModel is an alias for FileIR."""
        assert FileModel is FileIR

    def test_onnx_model_creates_instance(self):
        """Test that ONNXModel can create instance."""
        ir = ONNXModel("/tmp/test.onnx")
        assert isinstance(ir, ONNXFileIR)
        assert ir.file_format == "onnx"

    def test_air_model_creates_instance(self):
        """Test that AIRModel can create instance."""
        ir = AIRModel("/tmp/test.B")
        assert isinstance(ir, AIRFileIR)
        assert ir.file_format == "air"