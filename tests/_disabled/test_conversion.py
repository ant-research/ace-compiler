# tests/test_unit/test_ir/test_conversion.py
"""
Unit tests for IR conversion functions.

Tests for:
- ONNX → FHEProgram conversion
- ONNX → AIR binary conversion
"""
import pytest
import torch
from pathlib import Path

from ace.fhe.ir import FHEProgram, FHEGraph
from ace.fhe.ir.frontends.onnx import convert_onnx_to_fhe_program
from ace.fhe.ir.io.onnx_tools import convert_onnx_to_air_binary
from ace.fhe.ir import export_model_to_onnx

# Import test models from ace.samples
from ace.samples.ops import LinearOp as _LinearOp


class SimpleModel(_LinearOp):
    """Linear layer with 4 -> 4 dimensions."""
    def __init__(self):
        super().__init__(4, 4)


# ============================================================================
# ONNX to FHEProgram Tests
# ============================================================================

class TestONNXToFHEProgram:
    """Tests for ONNX → FHEProgram conversion."""

    def test_function_exists(self):
        """Test that convert_onnx_to_fhe_program function exists."""
        assert callable(convert_onnx_to_fhe_program)

    def test_convert_returns_fhe_program(self, tmp_path):
        """Test that convert returns FHEProgram."""
        model = SimpleModel()
        inputs = torch.randn(1, 4)
        onnx_path = tmp_path / "model.onnx"
        export_model_to_onnx(model, inputs, onnx_path)

        result = convert_onnx_to_fhe_program(onnx_path)

        assert isinstance(result, FHEProgram)

    def test_convert_preserves_graph_structure(self, tmp_path):
        """Test that convert preserves graph structure."""
        model = SimpleModel()
        inputs = torch.randn(1, 4)
        onnx_path = tmp_path / "model.onnx"
        export_model_to_onnx(model, inputs, onnx_path)

        result = convert_onnx_to_fhe_program(onnx_path)

        assert len(result.graphs) > 0
        main_graph = result.get_main_graph()
        assert main_graph is not None
        assert len(main_graph.input_nodes) > 0

    def test_convert_with_input_names(self, tmp_path):
        """Test conversion with custom input names."""
        model = SimpleModel()
        inputs = torch.randn(1, 4)
        onnx_path = tmp_path / "model.onnx"
        export_model_to_onnx(model, inputs, onnx_path, input_names=["data"])

        result = convert_onnx_to_fhe_program(onnx_path)

        assert isinstance(result, FHEProgram)


# ============================================================================
# ONNX to AIR Binary Tests
# ============================================================================

class TestONNXToAIR:
    """Tests for ONNX → AIR binary conversion."""

    def test_function_exists(self):
        """Test that convert_onnx_to_air_binary function exists."""
        assert callable(convert_onnx_to_air_binary)

    def test_convert_with_compiler(self, tmp_path):
        """Test that convert works when compiler is available."""
        model = SimpleModel()
        inputs = torch.randn(1, 4)
        onnx_path = tmp_path / "model.onnx"
        output_path = tmp_path / "model.B"
        export_model_to_onnx(model, inputs, onnx_path)

        # If compiler is available, this should succeed
        # If not available, it will raise
        try:
            result = convert_onnx_to_air_binary(onnx_path, str(output_path))
            assert result == str(output_path)
        except (FileNotFoundError, RuntimeError):
            # Compiler not available, skip this test
            pytest.skip("fhe_cmplr not available")

    def test_convert_creates_air_file(self, tmp_path):
        """Test that convert creates AIR binary file."""
        model = SimpleModel()
        inputs = torch.randn(1, 4)
        onnx_path = tmp_path / "model.onnx"
        output_path = tmp_path / "model.B"
        export_model_to_onnx(model, inputs, onnx_path)

        try:
            convert_onnx_to_air_binary(onnx_path, str(output_path))
            assert output_path.exists()
        except (FileNotFoundError, RuntimeError):
            pytest.skip("fhe_cmplr not available")


# ============================================================================
# ONNX Converter Helper Tests
# ============================================================================

class TestONNXConverterHelpers:
    """Tests for ONNX converter helper functions."""

    def test_extract_attributes_exists(self):
        """Test that _extract_attributes helper exists."""
        from ace.fhe.ir.frontends.onnx.onnx_converter import _extract_attributes
        assert callable(_extract_attributes)

    def test_validate_onnx_model_exists(self):
        """Test that _validate_onnx_model helper exists."""
        from ace.fhe.ir.frontends.onnx.onnx_converter import _validate_onnx_model
        assert callable(_validate_onnx_model)


# ============================================================================
# FHEProgram to ONNX Tests (Reverse Conversion)
# ============================================================================

class TestFHEProgramToONNX:
    """Tests for FHEProgram → ONNX conversion (via export)."""

    def test_fhe_program_can_export_to_onnx(self, tmp_path, simple_fhe_program):
        """Test that FHEProgram can be exported to ONNX."""
        from ace.fhe.ir.export import export_fhe_program_to_onnx

        # Add required metadata
        graph = simple_fhe_program.get_main_graph()
        graph.metadata["input_shapes"] = {"x": [1, 4]}
        graph.metadata["output_shape"] = [1, 4]

        output_path = tmp_path / "test.onnx"
        result = export_fhe_program_to_onnx(simple_fhe_program, str(output_path))

        assert isinstance(result, bool)


# ============================================================================
# TODO: Torch to AIR Direct Conversion Tests
# ============================================================================

class TestTorchToAIR:
    """Tests for Torch → AIR direct conversion.

    TODO: Implement end-to-end tests for torch → AIR conversion.
    """

    @pytest.mark.skip(reason="TODO: Implement torch → AIR end-to-end test")
    def test_linear_model_conversion(self):
        """Test simple Linear model conversion."""
        pass

    @pytest.mark.skip(reason="TODO: Implement torch → AIR end-to-end test")
    def test_conv_model_conversion(self):
        """Test Conv model conversion."""
        pass

    @pytest.mark.skip(reason="TODO: Implement torch → AIR end-to-end test")
    def test_resnet_conversion(self):
        """Test ResNet model conversion."""
        pass


# ============================================================================
# TODO: Torch via ONNX Conversion Tests
# ============================================================================

class TestTorchViaONNX:
    """Tests for Torch → ONNX → AIR indirect conversion.

    TODO: Implement end-to-end tests for torch → ONNX → AIR conversion.
    """

    @pytest.mark.skip(reason="TODO: Implement torch → ONNX → AIR end-to-end test")
    def test_linear_model_via_onnx(self):
        """Test simple Linear model conversion via ONNX."""
        pass

    @pytest.mark.skip(reason="TODO: Implement torch → ONNX → AIR end-to-end test")
    def test_resnet_via_onnx(self):
        """Test ResNet model conversion via ONNX."""
        pass


# ============================================================================
# TODO: Semantic Equivalence Tests
# ============================================================================

class TestSemanticEquivalence:
    """Tests for semantic equivalence between different conversion paths.

    TODO: Implement tests to verify that different conversion paths
    produce semantically equivalent results.
    """

    @pytest.mark.skip(reason="TODO: Implement semantic equivalence test")
    def test_torch_direct_vs_via_onnx(self):
        """Test that torch → AIR and torch → ONNX → AIR produce equivalent results."""
        pass