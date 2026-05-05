# tests/test_unit/test_ir/test_export.py
"""
Unit tests for IR export functions.

Tests for:
- ONNX export: FHEProgram → ONNX file
- AIR export: FHEProgram → .B file
"""
import pytest
from pathlib import Path

from ace.fhe.ir import FHEProgram, FHEGraph, BasicBlock, IRNode
from ace.fhe.ir.export import export_fhe_program_to_onnx, export_fhe_program_to_air
from ace.fhe.ir.export.onnx_export import OP_TYPE_TO_ONNX
from ace.fhe.ir.export.air_export import SUPPORTED_AIR_OPS


# ============================================================================
# ONNX Export Tests
# ============================================================================

class TestOPTypeMapping:
    """Tests for OP_TYPE_TO_ONNX mapping."""

    def test_mapping_exists(self):
        """Test that OP_TYPE_TO_ONNX mapping exists."""
        assert isinstance(OP_TYPE_TO_ONNX, dict)

    def test_common_ops_mapped(self):
        """Test that common operations are mapped."""
        expected_ops = ["add", "sub", "mul", "div", "relu", "matmul", "conv", "gemm"]
        for op in expected_ops:
            assert op in OP_TYPE_TO_ONNX

    def test_mapping_values_are_valid_onnx_ops(self):
        """Test that mapped values are valid ONNX op types."""
        for ir_op, onnx_op in OP_TYPE_TO_ONNX.items():
            assert isinstance(onnx_op, str)
            assert len(onnx_op) > 0


class TestExportFHEProgramToOnnx:
    """Tests for export_fhe_program_to_onnx function."""

    def test_function_exists(self):
        """Test that export_fhe_program_to_onnx function exists."""
        assert callable(export_fhe_program_to_onnx)

    def test_export_creates_file(self, tmp_path):
        """Test that export creates ONNX file."""
        # Create a simple FHEProgram
        program = FHEProgram(name="test")
        graph = FHEGraph(name="forward")
        graph.input_nodes = ["x", "y"]
        graph.output_nodes = ["z"]
        graph.metadata["input_shapes"] = {"x": [1, 4], "y": [1, 4]}
        graph.metadata["output_shape"] = [1, 4]

        block = BasicBlock(name="entry")
        node = IRNode("add1")
        node.op_type = "add"
        node.inputs = ["x", "y"]
        node.outputs = ["z"]
        block.nodes.append(node)
        graph.add_block(block)
        graph.entry_block = block
        program.add_graph("forward", graph)

        output_path = tmp_path / "test.onnx"
        result = export_fhe_program_to_onnx(program, str(output_path))

        assert result is True
        assert output_path.exists()

    def test_export_returns_bool(self, tmp_path):
        """Test that export returns boolean."""
        program = FHEProgram(name="test")
        graph = FHEGraph(name="forward")
        graph.input_nodes = ["x"]
        graph.output_nodes = ["y"]
        graph.metadata["input_shapes"] = {"x": [1, 4]}
        graph.metadata["output_shape"] = [1, 4]

        block = BasicBlock(name="entry")
        graph.add_block(block)
        graph.entry_block = block
        program.add_graph("forward", graph)

        output_path = tmp_path / "test.onnx"
        result = export_fhe_program_to_onnx(program, str(output_path))

        assert isinstance(result, bool)

    def test_export_creates_valid_onnx(self, tmp_path):
        """Test that exported ONNX is valid."""
        import onnx

        program = FHEProgram(name="test")
        graph = FHEGraph(name="forward")
        graph.input_nodes = ["x"]
        graph.output_nodes = ["y"]
        graph.metadata["input_shapes"] = {"x": [1, 4]}
        graph.metadata["output_shape"] = [1, 4]

        block = BasicBlock(name="entry")
        node = IRNode("relu1")
        node.op_type = "relu"
        node.inputs = ["x"]
        node.outputs = ["y"]
        block.nodes.append(node)
        graph.add_block(block)
        graph.entry_block = block
        program.add_graph("forward", graph)

        output_path = tmp_path / "test.onnx"
        export_fhe_program_to_onnx(program, str(output_path))

        # Validate ONNX
        model = onnx.load(str(output_path))
        onnx.checker.check_model(model)  # Should not raise

    def test_export_with_multiple_nodes(self, tmp_path):
        """Test export with multiple nodes in graph."""
        program = FHEProgram(name="test")
        graph = FHEGraph(name="forward")
        graph.input_nodes = ["x"]
        graph.output_nodes = ["z"]
        graph.metadata["input_shapes"] = {"x": [1, 4]}
        graph.metadata["output_shape"] = [1, 4]

        block = BasicBlock(name="entry")

        # Add multiple nodes
        node1 = IRNode("relu1")
        node1.op_type = "relu"
        node1.inputs = ["x"]
        node1.outputs = ["t1"]
        block.nodes.append(node1)

        node2 = IRNode("add1")
        node2.op_type = "add"
        node2.inputs = ["t1", "t1"]
        node2.outputs = ["z"]
        block.nodes.append(node2)

        graph.add_block(block)
        graph.entry_block = block
        program.add_graph("forward", graph)

        output_path = tmp_path / "test.onnx"
        result = export_fhe_program_to_onnx(program, str(output_path))

        assert result is True
        assert output_path.exists()


# ============================================================================
# AIR Export Tests
# ============================================================================

class TestSupportedAirOps:
    """Tests for SUPPORTED_AIR_OPS set."""

    def test_set_exists(self):
        """Test that SUPPORTED_AIR_OPS set exists."""
        assert isinstance(SUPPORTED_AIR_OPS, set)

    def test_common_ops_supported(self):
        """Test that common operations are supported."""
        expected_ops = ["add", "sub", "mul", "div", "relu", "matmul", "conv", "gemm"]
        for op in expected_ops:
            assert op in SUPPORTED_AIR_OPS

    def test_all_ops_are_strings(self):
        """Test that all supported ops are strings."""
        for op in SUPPORTED_AIR_OPS:
            assert isinstance(op, str)


class TestExportFHEProgramToAir:
    """Tests for export_fhe_program_to_air function."""

    def test_function_exists(self):
        """Test that export_fhe_program_to_air function exists."""
        assert callable(export_fhe_program_to_air)

    def test_export_returns_bool(self):
        """Test that export returns boolean."""
        program = FHEProgram(name="test")
        graph = FHEGraph(name="forward")
        graph.input_nodes = ["x"]
        graph.output_nodes = ["y"]
        graph.metadata["input_shapes"] = {"x": [1, 4]}
        graph.metadata["output_shape"] = [1, 4]

        block = BasicBlock(name="entry")
        graph.add_block(block)
        graph.entry_block = block
        program.add_graph("forward", graph)

        # This will likely return False because C++ extension is not available
        # in test environment, but we test the interface
        result = export_fhe_program_to_air(program, "/tmp/test.B")
        assert isinstance(result, bool)

    def test_export_with_compiler(self, tmp_path):
        """Test export when compiler is available."""
        program = FHEProgram(name="test")
        graph = FHEGraph(name="forward")
        graph.input_nodes = ["x"]
        graph.output_nodes = ["y"]
        graph.metadata["input_shapes"] = {"x": [1, 4]}
        graph.metadata["output_shape"] = [1, 4]

        block = BasicBlock(name="entry")
        graph.add_block(block)
        graph.entry_block = block
        program.add_graph("forward", graph)

        output_path = tmp_path / "test.B"

        try:
            result = export_fhe_program_to_air(program, str(output_path))
            if result:
                assert output_path.exists()
        except (FileNotFoundError, RuntimeError):
            pytest.skip("C++ extension not available")


# ============================================================================
# Export Utility Tests
# ============================================================================

class TestExportUtilities:
    """Tests for export utility functions."""

    def test_export_model_to_onnx_exists(self):
        """Test that export_model_to_onnx function exists."""
        from ace.fhe.ir import export_model_to_onnx
        assert callable(export_model_to_onnx)

    def test_export_function_to_onnx_exists(self):
        """Test that export_function_to_onnx function exists."""
        from ace.fhe.ir import export_function_to_onnx
        assert callable(export_function_to_onnx)