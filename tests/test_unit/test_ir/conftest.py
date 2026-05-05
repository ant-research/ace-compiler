# tests/test_unit/test_ir/conftest.py
"""
Shared fixtures for IR unit tests.
"""
import pytest
import torch


@pytest.fixture
def simple_linear_model():
    """Fixture providing a simple linear model (4 -> 4)."""
    from ace.samples.ops import LinearOp
    return LinearOp(4, 4)


@pytest.fixture
def simple_linear_inputs():
    """Fixture providing inputs for simple linear model."""
    return torch.randn(1, 4)


@pytest.fixture
def simple_fhe_graph():
    """Fixture providing a simple FHEGraph for testing."""
    from ace.fhe.ir import FHEGraph, BasicBlock, IRNode

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
    """Fixture providing a simple FHEProgram for testing."""
    from ace.fhe.ir import FHEProgram

    program = FHEProgram(name="test_program")
    program.add_graph("forward", simple_fhe_graph)

    return program


@pytest.fixture
def simple_ir_node():
    """Fixture providing a simple IRNode for testing."""
    from ace.fhe.ir import IRNode

    node = IRNode("test_node")
    node.op_type = "add"
    node.inputs = ["x", "y"]
    node.outputs = ["z"]
    node.dtype = "float32"
    node.shape = [1, 4]

    return node


@pytest.fixture
def simple_basic_block():
    """Fixture providing a simple BasicBlock for testing."""
    from ace.fhe.ir import BasicBlock, IRNode

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