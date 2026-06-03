# tests/unit/ir/conftest.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Shared fixtures for IR unit tests.
"""

import pytest

from ace.fhe.ir import FHEProgram, FHEGraph, BasicBlock, IRNode


@pytest.fixture
def simple_fhe_graph():
    """FHEGraph with one add node."""
    graph = FHEGraph("test_graph")
    graph.input_nodes = ["x"]
    graph.output_nodes = ["y"]

    block = BasicBlock("entry")
    node = IRNode("add_node")
    node.op_type = "add"
    node.inputs = ["x"]
    node.outputs = ["y"]
    block.nodes.append(node)

    graph.add_block(block)
    graph.entry_block = block
    return graph


@pytest.fixture
def simple_fhe_program(simple_fhe_graph):
    """FHEProgram with one forward graph."""
    program = FHEProgram(name="test_program")
    program.add_graph("forward", simple_fhe_graph)
    return program


@pytest.fixture
def simple_ir_node():
    """IRNode with add op."""
    node = IRNode("test_node")
    node.op_type = "add"
    node.inputs = ["x", "y"]
    node.outputs = ["z"]
    node.dtype = "float32"
    node.shape = [1, 4]
    return node


@pytest.fixture
def simple_basic_block():
    """BasicBlock with add + relu nodes."""
    block = BasicBlock("test_block")

    node1 = IRNode("node1")
    node1.op_type = "add"
    node1.inputs = ["x", "y"]
    node1.outputs = ["t1"]
    block.nodes.append(node1)

    node2 = IRNode("node2")
    node2.op_type = "relu"
    node2.inputs = ["t1"]
    node2.outputs = ["z"]
    block.nodes.append(node2)
    return block