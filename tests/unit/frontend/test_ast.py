# tests/unit/frontend/test_ast.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Unit tests for AST frontend.

Tests AST frontend functionality:
1. prepare()  - Python function → FHEProgram via AST analysis
2. compile()   - Generate AIR IR via IRBuilder → .B file
3. export()    - Export to .B file or .onnx file

Pipeline:
- AST frontend directly analyzes Python AST (no ONNX conversion)
- Uses IRBuilder to generate AIR IR (like Torch frontend)
"""
import pytest
import os

from utils import skip_if_no_frontend
from ace.fhe.ir import FHEProgram
from ace.sample.funcs.specs import ADD_FUNC, MUL_FUNC, RELU_FUNC

# Representative specs: binary arithmetic (add, mul) + unary activation (relu)
_AST_SPECS = [ADD_FUNC, MUL_FUNC, RELU_FUNC]


# =============================================================================
# Test: prepare() - Memory Mode
# =============================================================================

class TestPrepare:
    """Tests for prepare() method - Memory mode."""

    @pytest.mark.parametrize("spec", _AST_SPECS, ids=lambda s: s.name)
    def test_returns_fhe_program(self, ast_frontend, spec):
        """Test that prepare() returns FHEProgram."""
        result = ast_frontend.prepare(spec.func, list(spec.example_inputs), spec.encrypt_inputs)
        assert isinstance(result, FHEProgram)

    @pytest.mark.parametrize("spec", _AST_SPECS, ids=lambda s: s.name)
    def test_memory_mode_properties(self, ast_frontend, spec):
        """prepare() returns memory-mode result: no file."""
        result = ast_frontend.prepare(spec.func, list(spec.example_inputs), spec.encrypt_inputs)
        assert result.format_type == "memory"
        assert result.file_format is None
        assert result.file_path is None

    @pytest.mark.parametrize("spec", _AST_SPECS, ids=lambda s: s.name)
    def test_has_graphs(self, ast_frontend, spec):
        """FHEProgram has graphs with 'forward' entry."""
        result = ast_frontend.prepare(spec.func, list(spec.example_inputs), spec.encrypt_inputs)
        assert len(result.graphs) > 0
        assert "forward" in result.graphs

    def test_with_placeholder_inputs(self, ast_frontend):
        """Test prepare() with placeholder inputs (None values)."""
        result = ast_frontend.prepare(ADD_FUNC.func, [None, None], ADD_FUNC.encrypt_inputs)
        assert isinstance(result, FHEProgram)


# =============================================================================
# Test: compile() - Generates AIR IR
# =============================================================================

@skip_if_no_frontend
class TestCompile:
    """Tests for compile() method - Generates AIR IR via IRBuilder."""

    @pytest.mark.parametrize("spec", _AST_SPECS, ids=lambda s: s.name)
    def test_returns_fhe_program(self, ast_frontend, spec):
        result = ast_frontend.compile(spec.func, list(spec.example_inputs), spec.encrypt_inputs)
        assert isinstance(result, FHEProgram)

    @pytest.mark.parametrize("spec", _AST_SPECS, ids=lambda s: s.name)
    def test_format_type_is_file(self, ast_frontend, spec):
        result = ast_frontend.compile(spec.func, list(spec.example_inputs), spec.encrypt_inputs)
        assert result.format_type == "file"

    @pytest.mark.parametrize("spec", _AST_SPECS, ids=lambda s: s.name)
    def test_file_path_set(self, ast_frontend, spec):
        result = ast_frontend.compile(spec.func, list(spec.example_inputs), spec.encrypt_inputs)
        assert result.file_path is not None
        assert result.file_path.endswith(".B")

    def test_with_placeholder_inputs(self, ast_frontend):
        """Test compile() with placeholder inputs (None values)."""
        result = ast_frontend.compile(ADD_FUNC.func, [None, None], ADD_FUNC.encrypt_inputs)
        assert isinstance(result, FHEProgram)
        assert result.format_type == "file"


# =============================================================================
# Test: export(format="air")
# =============================================================================

@skip_if_no_frontend
class TestExportAir:
    """Tests for export(format="air") method."""

    @pytest.mark.parametrize("spec", _AST_SPECS, ids=lambda s: s.name)
    def test_creates_air_file(self, ast_frontend, spec, tmp_path):
        output_path = str(tmp_path / "output.B")
        result_path = ast_frontend.export(spec.func, list(spec.example_inputs),
                                          format="air", output_path=output_path,
                                          input_names=spec.encrypt_inputs)
        assert result_path == output_path
        assert os.path.exists(output_path)

    # =============================================================================
# Test: IR Properties
# =============================================================================

class TestIRProperties:
    """Tests for IR object properties."""

    @pytest.mark.parametrize("spec", _AST_SPECS, ids=lambda s: s.name)
    def test_entry_name_exists(self, ast_frontend, spec):
        result = ast_frontend.prepare(spec.func, list(spec.example_inputs), spec.encrypt_inputs)
        assert hasattr(result, "entry_name")
        assert result.entry_name

    @pytest.mark.parametrize("spec", _AST_SPECS, ids=lambda s: s.name)
    def test_has_main_graph(self, ast_frontend, spec):
        result = ast_frontend.prepare(spec.func, list(spec.example_inputs), spec.encrypt_inputs)
        assert result.get_main_graph() is not None

    @pytest.mark.parametrize("spec", _AST_SPECS, ids=lambda s: s.name)
    def test_graph_has_input_output_nodes(self, ast_frontend, spec):
        result = ast_frontend.prepare(spec.func, list(spec.example_inputs), spec.encrypt_inputs)
        assert len(result.get_main_graph().input_nodes) > 0
        assert len(result.get_main_graph().output_nodes) > 0


# =============================================================================
# Test: Edge Cases
# =============================================================================

@skip_if_no_frontend
class TestEdgeCases:
    """Tests for edge cases."""

    def test_export_to_valid_path(self, ast_frontend, tmp_path):
        output_path = str(tmp_path / "model.B")
        result_path = ast_frontend.export(ADD_FUNC.func, list(ADD_FUNC.example_inputs),
                                          format="air", output_path=output_path,
                                          input_names=ADD_FUNC.encrypt_inputs)
        assert os.path.exists(result_path)


# =============================================================================
# Test: Frontend Metadata
# =============================================================================

class TestFrontendMeta:
    """Tests for frontend metadata."""

    def test_frontend_name(self, ast_frontend):
        """Test frontend name."""
        assert ast_frontend.name() == "ast"