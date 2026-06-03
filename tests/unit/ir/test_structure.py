# tests/unit/ir/test_structure.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Unit tests for IR data structures.

Tests CompilationUnit, IRNode, BasicBlock, FHEGraph, FHEProgram.
"""

import pytest
from abc import ABC

from ace.fhe.ir.base import CompilationUnit
from ace.fhe.ir.representations.graph import IRNode, BasicBlock, FHEGraph
from ace.fhe.ir import FHEProgram


# =============================================================================
# CompilationUnit
# =============================================================================

class TestCompilationUnit:
    """Tests for CompilationUnit abstract base class."""

    def test_is_abstract(self):
        assert issubclass(CompilationUnit, ABC)

    def test_cannot_instantiate_directly(self):
        with pytest.raises(TypeError):
            CompilationUnit()

    def test_subclass_must_implement_format_type(self):
        class IncompleteIR(CompilationUnit):
            @property
            def entry_name(self):
                return "test"

        with pytest.raises(TypeError):
            IncompleteIR()

    def test_subclass_must_implement_entry_name(self):
        class IncompleteIR(CompilationUnit):
            @property
            def format_type(self):
                return "memory"

        with pytest.raises(TypeError):
            IncompleteIR()

    def test_file_format_defaults_to_none(self):
        class CompleteIR(CompilationUnit):
            @property
            def format_type(self):
                return "memory"

            @property
            def entry_name(self):
                return "test"

        ir = CompleteIR()
        assert ir.file_format is None
        assert ir.file_path is None


# =============================================================================
# IRNode
# =============================================================================

class TestIRNode:
    """Tests for IRNode."""

    def test_create_with_name(self):
        node = IRNode("add_node")
        assert node.name == "add_node"

    def test_defaults(self):
        node = IRNode("test")
        assert node.op_type == ""
        assert node.inputs == []
        assert node.outputs == []
        assert node.attributes == {}
        assert node.dtype is None
        assert node.shape is None

    def test_set_attributes(self):
        node = IRNode("test")
        node.op_type = "conv"
        node.inputs = ["x", "y"]
        node.outputs = ["z"]
        node.attributes = {"kernel_size": 3, "stride": 1}
        node.dtype = "float32"
        node.shape = [1, 3, 32, 32]
        assert node.op_type == "conv"
        assert node.inputs == ["x", "y"]
        assert node.outputs == ["z"]
        assert node.attributes["kernel_size"] == 3
        assert node.dtype == "float32"
        assert node.shape == [1, 3, 32, 32]

    def test_fixture_node(self, simple_ir_node):
        assert simple_ir_node.name == "test_node"
        assert simple_ir_node.op_type == "add"
        assert simple_ir_node.inputs == ["x", "y"]
        assert simple_ir_node.outputs == ["z"]
        assert simple_ir_node.dtype == "float32"
        assert simple_ir_node.shape == [1, 4]


# =============================================================================
# BasicBlock
# =============================================================================

class TestBasicBlock:
    """Tests for BasicBlock."""

    def test_create_with_name(self):
        block = BasicBlock("entry")
        assert block.name == "entry"

    def test_defaults(self):
        block = BasicBlock("test")
        assert block.nodes == []
        assert block.successors == []
        assert block.predecessors == []

    def test_add_node(self):
        block = BasicBlock("entry")
        node = {"op_type": "add", "inputs": ["x"], "outputs": ["y"]}
        block.add_node(node)
        assert len(block.nodes) == 1
        assert block.nodes[0] is node

    def test_add_multiple_nodes(self):
        block = BasicBlock("entry")
        block.add_node({"op_type": "add"})
        block.add_node({"op_type": "mul"})
        block.add_node({"op_type": "relu"})
        assert len(block.nodes) == 3

    def test_successors_and_predecessors(self):
        block1 = BasicBlock("block1")
        block2 = BasicBlock("block2")
        block1.successors.append(block2)
        block2.predecessors.append(block1)
        assert block2 in block1.successors
        assert block1 in block2.predecessors

    def test_fixture_block(self, simple_basic_block):
        assert len(simple_basic_block.nodes) == 2
        assert simple_basic_block.nodes[0].op_type == "add"
        assert simple_basic_block.nodes[1].op_type == "relu"


# =============================================================================
# FHEGraph
# =============================================================================

class TestFHEGraph:
    """Tests for FHEGraph."""

    def test_create_with_name(self):
        graph = FHEGraph("forward")
        assert graph.name == "forward"

    def test_defaults(self):
        graph = FHEGraph("test")
        assert graph.blocks == {}
        assert graph.entry_block is None
        assert graph.input_nodes == []
        assert graph.output_nodes == []
        assert graph.metadata == {}

    def test_add_block(self):
        graph = FHEGraph("forward")
        block = BasicBlock("entry")
        graph.add_block(block)
        assert "entry" in graph.blocks
        assert graph.blocks["entry"] is block

    def test_entry_block(self):
        graph = FHEGraph("forward")
        block = BasicBlock("entry")
        graph.add_block(block)
        graph.entry_block = block
        assert graph.entry_block is block

    def test_generate_unique_name(self):
        graph = FHEGraph("forward")
        assert graph.generate_unique_name("tmp") == "tmp_1"
        assert graph.generate_unique_name("tmp") == "tmp_2"
        assert graph.generate_unique_name("node") == "node_1"

    def test_generate_unique_name_different_prefixes(self):
        graph = FHEGraph("forward")
        names = [graph.generate_unique_name("add") for _ in range(3)]
        assert names == ["add_1", "add_2", "add_3"]

    def test_get_all_nodes_with_ir_nodes(self):
        graph = FHEGraph("forward")

        block1 = BasicBlock("block1")
        node1 = IRNode("node1")
        node1.op_type = "add"
        block1.nodes.append(node1)

        block2 = BasicBlock("block2")
        node2 = IRNode("node2")
        node2.op_type = "mul"
        block2.nodes.append(node2)

        graph.add_block(block1)
        graph.add_block(block2)

        all_nodes = graph.get_all_nodes()
        assert len(all_nodes) == 2
        assert "node1" in all_nodes
        assert "node2" in all_nodes

    def test_input_output_nodes(self):
        graph = FHEGraph("forward")
        graph.input_nodes = ["x", "y"]
        graph.output_nodes = ["z"]
        assert graph.input_nodes == ["x", "y"]
        assert graph.output_nodes == ["z"]

    def test_metadata(self):
        graph = FHEGraph("forward")
        graph.metadata["input_shapes"] = {"x": [1, 4]}
        graph.metadata["output_shape"] = [1, 4]
        assert graph.metadata["input_shapes"] == {"x": [1, 4]}
        assert graph.metadata["output_shape"] == [1, 4]

    def test_to_dict(self):
        graph = FHEGraph("forward")
        graph.input_nodes = ["x"]
        graph.output_nodes = ["y"]

        block = BasicBlock("entry")
        node = IRNode("add_node")
        node.op_type = "add"
        node.inputs = ["x"]
        node.outputs = ["y"]
        block.nodes.append(node)
        graph.add_block(block)

        result = graph.to_dict()
        assert result["name"] == "forward"
        assert result["input_nodes"] == ["x"]
        assert result["output_nodes"] == ["y"]
        assert "entry" in result["blocks"]

    def test_fixture_graph(self, simple_fhe_graph):
        assert simple_fhe_graph.name == "test_graph"
        assert simple_fhe_graph.input_nodes == ["x"]
        assert simple_fhe_graph.output_nodes == ["y"]
        assert "entry" in simple_fhe_graph.blocks


# =============================================================================
# FHEProgram
# =============================================================================

class TestFHEProgram:
    """Tests for FHEProgram."""

    def test_create_with_name(self):
        program = FHEProgram(name="test_module")
        assert program.name == "test_module"
        assert program.graphs == {}
        assert program.global_vars == {}
        assert program.meta == {}

    def test_default_name(self):
        program = FHEProgram()
        assert program.name == "default_module"

    def test_name_setter(self):
        program = FHEProgram(name="old")
        program.name = "new"
        assert program.name == "new"

    def test_format_type_is_memory(self):
        assert FHEProgram().format_type == "memory"

    def test_file_format_is_none(self):
        assert FHEProgram().file_format is None

    def test_file_path_is_none(self):
        assert FHEProgram().file_path is None

    def test_entry_name(self):
        program = FHEProgram(name="my_program")
        assert program.entry_name == "my_program"

    def test_add_graph(self):
        graph = FHEGraph(name="forward")
        program = FHEProgram()
        program.add_graph("forward", graph)
        assert "forward" in program.graphs
        assert program.graphs["forward"] is graph

    def test_add_function(self):
        graph = FHEGraph(name="forward")
        program = FHEProgram()
        program.add_function("forward", graph)
        assert "forward" in program.graphs

    def test_add_duplicate_function_raises(self):
        graph = FHEGraph(name="forward")
        program = FHEProgram()
        program.add_function("forward", graph)
        with pytest.raises(ValueError, match="already exists"):
            program.add_function("forward", graph)

    def test_get_function(self):
        graph = FHEGraph(name="forward")
        program = FHEProgram()
        program.add_graph("forward", graph)
        assert program.get_function("forward") is graph
        assert program.get_function("nonexistent") is None

    def test_list_functions(self):
        program = FHEProgram()
        program.add_graph("forward", FHEGraph(name="forward"))
        program.add_graph("backward", FHEGraph(name="backward"))
        assert set(program.list_functions()) == {"forward", "backward"}

    def test_get_main_graph_with_forward(self):
        graph = FHEGraph(name="forward")
        program = FHEProgram()
        program.add_graph("forward", graph)
        assert program.get_main_graph() is graph

    def test_get_main_graph_single_graph(self):
        graph = FHEGraph(name="single")
        program = FHEProgram()
        program.add_graph("single", graph)
        assert program.get_main_graph() is graph

    def test_get_main_graph_no_graph_raises(self):
        program = FHEProgram()
        with pytest.raises(ValueError, match="No main function found"):
            program.get_main_graph()

    def test_nodes_inputs_outputs(self):
        graph = FHEGraph(name="forward")
        graph.input_nodes = ["x"]
        graph.output_nodes = ["y"]
        block = BasicBlock(name="entry")
        block.add_node({"op_type": "add", "inputs": ["x"], "outputs": ["y"]})
        graph.add_block(block)
        graph.entry_block = block

        program = FHEProgram()
        program.add_graph("forward", graph)
        assert len(program.nodes) == 1
        assert program.nodes[0]["op_type"] == "add"
        assert program.inputs == ["x"]
        assert program.outputs == ["y"]

    def test_write_ir_is_alias_for_export_ir(self, tmp_path):
        program = FHEProgram()
        result1 = program.export_ir(str(tmp_path / "test1.pkl"))
        result2 = program.write_ir(str(tmp_path / "test2.pkl"))
        assert result1 == result2

    def test_fixture_program(self, simple_fhe_program):
        assert simple_fhe_program.name == "test_program"
        assert "forward" in simple_fhe_program.graphs
        assert simple_fhe_program.get_main_graph() is not None