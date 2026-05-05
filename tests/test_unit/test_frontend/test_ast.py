# tests/test_unit/test_frontend/test_ast.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Unit tests for AST frontend.

Tests AST frontend functionality:
1. prepare()  - Python function/model → FHEProgram via AST analysis
2. compile()   - Generate AIR IR via IRBuilder → .B file
3. export()    - Export to .B file or .onnx file

Pipeline:
- AST frontend directly analyzes Python AST (no ONNX conversion)
- Uses IRBuilder to generate AIR IR (like Torch frontend)
"""
import pytest
import torch
import torch.nn as nn
import os

from test_utils import skip_if_no_frontend
from ace.fhe.frontend import get_frontend
from ace.fhe.ir import FHEProgram

# Import test functions from ace.samples.funcs
from ace.samples.funcs import (
    add_func,
    sub_func,
    mul_func,
    div_func,
)

# Single-argument helper function for edge case tests
def single_arg_add(x):
    """Single argument add function."""
    return x + 1


# =============================================================================
# Fixtures
# =============================================================================

@pytest.fixture
def frontend():
    """Fixture for ast frontend."""
    return get_frontend("ast")


@pytest.fixture
def placeholder_inputs():
    """Placeholder inputs (AST doesn't need actual tensors)."""
    return [None, None]


@pytest.fixture
def actual_inputs():
    """Actual tensor inputs for shape inference."""
    return [torch.randn(1, 4), torch.randn(1, 4)]


# =============================================================================
# Test: prepare() - Memory Mode
# =============================================================================

class TestPrepare:
    """Tests for prepare() method - Memory mode."""

    def test_returns_fhe_program(self, frontend, placeholder_inputs):
        """Test that prepare() returns FHEProgram."""
        result = frontend.prepare(add_func, placeholder_inputs, ["x", "y"])
        assert isinstance(result, FHEProgram)

    def test_format_type_is_memory(self, frontend, placeholder_inputs):
        """Test that format_type is 'memory'."""
        result = frontend.prepare(add_func, placeholder_inputs, ["x", "y"])
        assert result.format_type == "memory"

    def test_file_format_is_none(self, frontend, placeholder_inputs):
        """Test that file_format is None for memory IR."""
        result = frontend.prepare(add_func, placeholder_inputs, ["x", "y"])
        assert result.file_format is None

    def test_file_path_is_none(self, frontend, placeholder_inputs):
        """Test that file_path is None for memory IR."""
        result = frontend.prepare(add_func, placeholder_inputs, ["x", "y"])
        assert result.file_path is None

    def test_has_graphs(self, frontend, placeholder_inputs):
        """Test that FHEProgram has graphs."""
        result = frontend.prepare(add_func, placeholder_inputs, ["x", "y"])
        assert len(result.graphs) > 0

    def test_graph_name(self, frontend, placeholder_inputs):
        """Test that graph name is derived from function name."""
        result = frontend.prepare(add_func, placeholder_inputs, ["x", "y"])
        assert "forward" in result.graphs

    def test_with_two_args_function(self, frontend, placeholder_inputs):
        """Test prepare() with two-argument function."""
        result = frontend.prepare(add_func, placeholder_inputs, ["x", "y"])
        assert isinstance(result, FHEProgram)
        main_graph = result.get_main_graph()
        assert len(main_graph.input_nodes) == 2

    def test_with_single_arg_function(self, frontend):
        """Test prepare() with single-argument function."""
        placeholder = [None]
        result = frontend.prepare(single_arg_add, placeholder, ["x"])
        assert isinstance(result, FHEProgram)
        main_graph = result.get_main_graph()
        assert len(main_graph.input_nodes) == 1


# =============================================================================
# Test: compile() - Generates AIR IR
# =============================================================================

class TestCompile:
    """Tests for compile() method - Generates AIR IR via IRBuilder."""

    def test_returns_fhe_program(self, frontend, actual_inputs):
        """Test that compile() returns FHEProgram."""
        result = frontend.compile(add_func, actual_inputs, ["x", "y"])
        assert isinstance(result, FHEProgram)

    def test_format_type_is_file(self, frontend, actual_inputs):
        """Test that format_type is 'file' after compile."""
        result = frontend.compile(add_func, actual_inputs, ["x", "y"])
        assert result.format_type == "file"

    def test_file_path_set(self, frontend, actual_inputs):
        """Test that file_path is set after compile."""
        result = frontend.compile(add_func, actual_inputs, ["x", "y"])
        assert result.file_path is not None
        assert result.file_path.endswith(".B")

    def test_with_placeholder_inputs(self, frontend, placeholder_inputs):
        """Test compile() with placeholder inputs (None values)."""
        result = frontend.compile(add_func, placeholder_inputs, ["x", "y"])
        assert isinstance(result, FHEProgram)
        assert result.format_type == "file"


# =============================================================================
# Test: export(format="air")
# =============================================================================

class TestExportAir:
    """Tests for export(format="air") method."""

    def test_creates_air_file(self, frontend, actual_inputs, tmp_path):
        """Test that export(format="air") creates .B file."""
        output_path = str(tmp_path / "output.B")
        result_path = frontend.export(add_func, actual_inputs,
                                      format="air", output_path=output_path,
                                      input_names=["x", "y"])
        assert result_path == output_path
        assert os.path.exists(output_path)

    def test_with_placeholder_inputs(self, frontend, placeholder_inputs, tmp_path):
        """Test export AIR with placeholder inputs."""
        output_path = str(tmp_path / "output.B")
        result_path = frontend.export(add_func, placeholder_inputs,
                                      format="air", output_path=output_path,
                                      input_names=["x", "y"])
        assert result_path == output_path

    def test_with_single_arg(self, frontend, tmp_path):
        """Test export AIR with single argument."""
        output_path = str(tmp_path / "single_arg.B")
        actual_input = [torch.randn(1, 4)]
        result_path = frontend.export(single_arg_add, actual_input,
                                      format="air", output_path=output_path,
                                      input_names=["x"])
        assert result_path == output_path
        assert os.path.exists(output_path)


# =============================================================================
# Test: export(format="onnx")
# =============================================================================

class TestExportOnnx:
    """Tests for export(format="onnx")."""

    def test_creates_onnx_file(self, frontend, actual_inputs, tmp_path):
        """Test that export(format="onnx") creates ONNX file."""
        output_path = str(tmp_path / "output.onnx")
        result_path = frontend.export(add_func, actual_inputs,
                                     format="onnx", output_path=output_path,
                                     input_names=["x", "y"])
        assert result_path == output_path
        assert os.path.exists(output_path)

    def test_onnx_file_is_valid(self, frontend, actual_inputs, tmp_path):
        """Test that exported ONNX file is valid."""
        import onnx
        output_path = str(tmp_path / "output.onnx")
        frontend.export(add_func, actual_inputs,
                       format="onnx", output_path=output_path,
                       input_names=["x", "y"])
        model = onnx.load(output_path)
        onnx.checker.check_model(model)


# =============================================================================
# Test: IR Properties
# =============================================================================

class TestIRProperties:
    """Tests for IR object properties."""

    def test_entry_name_exists(self, frontend, placeholder_inputs):
        """Test that FHEProgram has entry_name."""
        result = frontend.prepare(add_func, placeholder_inputs, ["x", "y"])
        assert hasattr(result, "entry_name")
        assert result.entry_name

    def test_has_main_graph(self, frontend, placeholder_inputs):
        """Test that FHEProgram has main graph."""
        result = frontend.prepare(add_func, placeholder_inputs, ["x", "y"])
        main_graph = result.get_main_graph()
        assert main_graph is not None

    def test_graph_has_input_nodes(self, frontend, placeholder_inputs):
        """Test that graph has input nodes."""
        result = frontend.prepare(add_func, placeholder_inputs, ["x", "y"])
        main_graph = result.get_main_graph()
        assert len(main_graph.input_nodes) > 0

    def test_graph_has_output_nodes(self, frontend, placeholder_inputs):
        """Test that graph has output nodes."""
        result = frontend.prepare(add_func, placeholder_inputs, ["x", "y"])
        main_graph = result.get_main_graph()
        assert len(main_graph.output_nodes) > 0


# =============================================================================
# Test: Edge Cases
# =============================================================================

@skip_if_no_frontend
class TestEdgeCases:
    """Tests for edge cases."""

    def test_export_to_valid_path(self, frontend, placeholder_inputs, tmp_path):
        """Test export to valid path."""
        output_path = str(tmp_path / "model.B")
        result_path = frontend.export(add_func, placeholder_inputs,
                                      format="air", output_path=output_path,
                                      input_names=["x", "y"])
        assert os.path.exists(result_path)


# =============================================================================
# Test: Frontend Metadata
# =============================================================================

class TestFrontendMeta:
    """Tests for frontend metadata."""

    def test_frontend_name(self, frontend):
        """Test frontend name."""
        assert frontend.name() == "ast"


# Run tests if executed directly
if __name__ == "__main__":
    pytest.main([__file__, "-v"])