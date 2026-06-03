# tests/unit/ir/test_export.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Unit tests for IR export functions.

Tests ONNX export, AIR export, and op type mappings.
"""

import pytest

from ace.fhe.ir import FHEProgram, FHEGraph, BasicBlock, IRNode
from ace.fhe.ir.export import export_fhe_program_to_onnx, export_fhe_program_to_air
from ace.fhe.ir.export.onnx_export import OP_TYPE_TO_ONNX
from ace.fhe.ir.export.air_export import SUPPORTED_AIR_OPS


def _make_program(name="test", op_type="add", input_names=None, output_names=None):
    """Helper: create a minimal FHEProgram with one node."""
    if input_names is None:
        input_names = ["x", "y"] if op_type in ("add", "sub", "mul", "div") else ["x"]
    if output_names is None:
        output_names = ["z"]

    graph = FHEGraph(name="forward")
    graph.input_nodes = input_names
    graph.output_nodes = output_names
    graph.metadata["input_shapes"] = {n: [1, 4] for n in input_names}
    graph.metadata["output_shape"] = [1, 4]

    block = BasicBlock(name="entry")
    node = IRNode(f"{op_type}1")
    node.op_type = op_type
    node.inputs = input_names
    node.outputs = output_names
    block.nodes.append(node)

    graph.add_block(block)
    graph.entry_block = block

    program = FHEProgram(name=name)
    program.add_graph("forward", graph)
    return program


# =============================================================================
# ONNX Op Type Mapping
# =============================================================================

class TestOPTypeMapping:
    """Tests for OP_TYPE_TO_ONNX mapping."""

    def test_mapping_exists(self):
        assert isinstance(OP_TYPE_TO_ONNX, dict)

    @pytest.mark.parametrize("op", ["add", "sub", "mul", "div", "relu", "matmul", "conv", "gemm"])
    def test_common_ops_mapped(self, op):
        assert op in OP_TYPE_TO_ONNX

    def test_mapping_values_are_strings(self):
        for ir_op, onnx_op in OP_TYPE_TO_ONNX.items():
            assert isinstance(onnx_op, str)
            assert len(onnx_op) > 0


# =============================================================================
# ONNX Export
# =============================================================================

class TestExportToOnnx:
    """Tests for export_fhe_program_to_onnx."""

    def test_export_creates_file(self, tmp_path):
        program = _make_program()
        output_path = tmp_path / "test.onnx"
        result = export_fhe_program_to_onnx(program, str(output_path))
        assert result is True
        assert output_path.exists()

    def test_export_returns_bool(self, tmp_path):
        program = _make_program(op_type="relu", input_names=["x"], output_names=["y"])
        output_path = tmp_path / "test.onnx"
        result = export_fhe_program_to_onnx(program, str(output_path))
        assert isinstance(result, bool)

    def test_export_creates_valid_onnx(self, tmp_path):
        import onnx

        program = _make_program(op_type="relu", input_names=["x"], output_names=["y"])
        output_path = tmp_path / "test.onnx"
        export_fhe_program_to_onnx(program, str(output_path))

        model = onnx.load(str(output_path))
        onnx.checker.check_model(model)

    def test_export_multiple_nodes(self, tmp_path):
        program = FHEProgram(name="test")
        graph = FHEGraph(name="forward")
        graph.input_nodes = ["x"]
        graph.output_nodes = ["z"]
        graph.metadata["input_shapes"] = {"x": [1, 4]}
        graph.metadata["output_shape"] = [1, 4]

        block = BasicBlock(name="entry")
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


# =============================================================================
# AIR Export
# =============================================================================

class TestSupportedAirOps:
    """Tests for SUPPORTED_AIR_OPS set."""

    def test_set_exists(self):
        assert isinstance(SUPPORTED_AIR_OPS, set)

    @pytest.mark.parametrize("op", ["add", "sub", "mul", "div", "relu", "matmul", "conv", "gemm"])
    def test_common_ops_supported(self, op):
        assert op in SUPPORTED_AIR_OPS

    def test_all_ops_are_strings(self):
        for op in SUPPORTED_AIR_OPS:
            assert isinstance(op, str)


class TestExportToAir:
    """Tests for export_fhe_program_to_air."""

    def test_function_exists(self):
        assert callable(export_fhe_program_to_air)

    def test_export_returns_bool(self, tmp_path):
        program = _make_program(op_type="relu", input_names=["x"], output_names=["y"])
        output_path = tmp_path / "test.B"
        try:
            result = export_fhe_program_to_air(program, str(output_path))
            assert isinstance(result, bool)
            if result:
                assert output_path.exists()
        except (FileNotFoundError, RuntimeError):
            pytest.skip("C++ extension not available")