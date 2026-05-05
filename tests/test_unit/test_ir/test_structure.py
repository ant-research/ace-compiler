# tests/test_unit/test_ir/test_structure.py
"""
Unit tests for IR data structures.

Tests for:
- CompilationUnit: Abstract base class
- IRNode: Individual operation node
- BasicBlock: Basic block container
- FHEGraph: Graph container
- FHEProgram: Program container
"""
import pytest
from abc import ABC

from ace.fhe.ir.base import CompilationUnit
from ace.fhe.ir.representations.graph import IRNode, BasicBlock, FHEGraph
from ace.fhe.ir import FHEProgram


# ============================================================================
# CompilationUnit Tests
# ============================================================================

class TestCompilationUnit:
    """Tests for CompilationUnit abstract base class."""

    def test_is_abstract(self):
        """Test that CompilationUnit is abstract."""
        assert issubclass(CompilationUnit, ABC)

    def test_cannot_instantiate_directly(self):
        """Test that CompilationUnit cannot be instantiated directly."""
        with pytest.raises(TypeError):
            CompilationUnit()

    def test_subclass_must_implement_format_type(self):
        """Test that subclass must implement format_type."""

        class IncompleteIR(CompilationUnit):
            @property
            def entry_name(self) -> str:
                return "test"

        with pytest.raises(TypeError):
            IncompleteIR()

    def test_subclass_must_implement_entry_name(self):
        """Test that subclass must implement entry_name."""

        class IncompleteIR(CompilationUnit):
            @property
            def format_type(self) -> str:
                return "memory"

        with pytest.raises(TypeError):
            IncompleteIR()

    def test_file_format_has_default_implementation(self):
        """Test that file_format has default implementation returning None."""

        class CompleteIR(CompilationUnit):
            @property
            def format_type(self) -> str:
                return "memory"

            @property
            def entry_name(self) -> str:
                return "test"

        ir = CompleteIR()
        assert ir.file_format is None

    def test_file_path_has_default_implementation(self):
        """Test that file_path has default implementation returning None."""

        class CompleteIR(CompilationUnit):
            @property
            def format_type(self) -> str:
                return "memory"

            @property
            def entry_name(self) -> str:
                return "test"

        ir = CompleteIR()
        assert ir.file_path is None


# ============================================================================
# IRNode Tests
# ============================================================================

class TestIRNode:
    """Tests for IRNode class."""

    def test_create_with_name(self):
        """Test IRNode creation with name."""
        node = IRNode("add_node")
        assert node.name == "add_node"

    def test_default_attributes(self):
        """Test IRNode default attributes."""
        node = IRNode("test")
        assert node.op_type == ""
        assert node.inputs == []
        assert node.outputs == []
        assert node.attributes == {}
        assert node.dtype is None
        assert node.shape is None

    def test_set_op_type(self):
        """Test setting op_type."""
        node = IRNode("test")
        node.op_type = "conv"
        assert node.op_type == "conv"

    def test_set_inputs_outputs(self):
        """Test setting inputs and outputs."""
        node = IRNode("test")
        node.inputs = ["x", "y"]
        node.outputs = ["z"]
        assert node.inputs == ["x", "y"]
        assert node.outputs == ["z"]

    def test_set_attributes(self):
        """Test setting attributes."""
        node = IRNode("test")
        node.attributes = {"kernel_size": 3, "stride": 1}
        assert node.attributes["kernel_size"] == 3
        assert node.attributes["stride"] == 1

    def test_set_shape_dtype(self):
        """Test setting shape and dtype."""
        node = IRNode("test")
        node.dtype = "float32"
        node.shape = [1, 3, 32, 32]
        assert node.dtype == "float32"
        assert node.shape == [1, 3, 32, 32]


class TestIRNodeWithFixture:
    """Tests for IRNode using fixtures."""

    def test_fixture_node_has_correct_attributes(self, simple_ir_node):
        """Test that fixture node has correct attributes."""
        assert simple_ir_node.name == "test_node"
        assert simple_ir_node.op_type == "add"
        assert simple_ir_node.inputs == ["x", "y"]
        assert simple_ir_node.outputs == ["z"]
        assert simple_ir_node.dtype == "float32"
        assert simple_ir_node.shape == [1, 4]


# ============================================================================
# BasicBlock Tests
# ============================================================================

class TestBasicBlock:
    """Tests for BasicBlock class."""

    def test_create_with_name(self):
        """Test BasicBlock creation with name."""
        block = BasicBlock("entry")
        assert block.name == "entry"

    def test_default_attributes(self):
        """Test BasicBlock default attributes."""
        block = BasicBlock("test")
        assert block.nodes == []
        assert block.successors == []
        assert block.predecessors == []

    def test_add_node(self):
        """Test adding a node to block."""
        block = BasicBlock("entry")
        node = {"op_type": "add", "inputs": ["x"], "outputs": ["y"]}
        block.add_node(node)
        assert len(block.nodes) == 1
        assert block.nodes[0] is node

    def test_add_multiple_nodes(self):
        """Test adding multiple nodes."""
        block = BasicBlock("entry")
        block.add_node({"op_type": "add"})
        block.add_node({"op_type": "mul"})
        block.add_node({"op_type": "relu"})
        assert len(block.nodes) == 3

    def test_successors_and_predecessors(self):
        """Test successor and predecessor relationships."""
        block1 = BasicBlock("block1")
        block2 = BasicBlock("block2")
        block1.successors.append(block2)
        block2.predecessors.append(block1)
        assert block2 in block1.successors
        assert block1 in block2.predecessors


class TestBasicBlockWithFixture:
    """Tests for BasicBlock using fixtures."""

    def test_fixture_block_has_nodes(self, simple_basic_block):
        """Test that fixture block has nodes."""
        assert len(simple_basic_block.nodes) == 2
        assert simple_basic_block.nodes[0].op_type == "add"
        assert simple_basic_block.nodes[1].op_type == "relu"


# ============================================================================
# FHEGraph Tests
# ============================================================================

class TestFHEGraph:
    """Tests for FHEGraph class."""

    def test_create_with_name(self):
        """Test FHEGraph creation with name."""
        graph = FHEGraph("forward")
        assert graph.name == "forward"

    def test_default_attributes(self):
        """Test FHEGraph default attributes."""
        graph = FHEGraph("test")
        assert graph.blocks == {}
        assert graph.entry_block is None
        assert graph.input_nodes == []
        assert graph.output_nodes == []
        assert graph.metadata == {}

    def test_add_block(self):
        """Test adding a basic block."""
        graph = FHEGraph("forward")
        block = BasicBlock("entry")
        graph.add_block(block)
        assert "entry" in graph.blocks
        assert graph.blocks["entry"] is block

    def test_entry_block(self):
        """Test setting entry block."""
        graph = FHEGraph("forward")
        block = BasicBlock("entry")
        graph.add_block(block)
        graph.entry_block = block
        assert graph.entry_block is block

    def test_generate_unique_name(self):
        """Test unique name generation."""
        graph = FHEGraph("forward")
        name1 = graph.generate_unique_name("tmp")
        name2 = graph.generate_unique_name("tmp")
        name3 = graph.generate_unique_name("node")

        assert name1 == "tmp_1"
        assert name2 == "tmp_2"
        assert name3 == "node_1"

    def test_generate_unique_name_different_prefixes(self):
        """Test unique name generation with different prefixes."""
        graph = FHEGraph("forward")
        names = [graph.generate_unique_name("add") for _ in range(3)]
        assert names == ["add_1", "add_2", "add_3"]

    def test_get_all_nodes_with_ir_nodes(self):
        """Test getting all nodes from all blocks with IRNode objects."""
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
        """Test input and output node management."""
        graph = FHEGraph("forward")
        graph.input_nodes = ["x", "y"]
        graph.output_nodes = ["z"]

        assert graph.input_nodes == ["x", "y"]
        assert graph.output_nodes == ["z"]

    def test_metadata(self):
        """Test metadata management."""
        graph = FHEGraph("forward")
        graph.metadata["input_shapes"] = {"x": [1, 4]}
        graph.metadata["output_shape"] = [1, 4]

        assert graph.metadata["input_shapes"] == {"x": [1, 4]}
        assert graph.metadata["output_shape"] == [1, 4]

    def test_to_dict(self):
        """Test to_dict method."""
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


class TestFHEGraphPrintMethods:
    """Tests for FHEGraph print methods."""

    def test_print_ir_method_exists(self):
        """Test that print_ir method exists."""
        graph = FHEGraph("forward")
        assert hasattr(graph, "print_ir")
        assert callable(graph.print_ir)

    def test_print_tabular_method_exists(self):
        """Test that print_tabular method exists."""
        graph = FHEGraph("forward")
        assert hasattr(graph, "print_tabular")
        assert callable(graph.print_tabular)


class TestFHEGraphWithFixture:
    """Tests for FHEGraph using fixtures."""

    def test_fixture_graph_has_correct_attributes(self, simple_fhe_graph):
        """Test that fixture graph has correct attributes."""
        assert simple_fhe_graph.name == "test_graph"
        assert simple_fhe_graph.input_nodes == ["x"]
        assert simple_fhe_graph.output_nodes == ["y"]
        assert "entry" in simple_fhe_graph.blocks


# ============================================================================
# FHEProgram Tests
# ============================================================================

class TestFHEProgramCreation:
    """Tests for FHEProgram creation."""

    def test_create_with_name(self):
        """Test FHEProgram creation with name."""
        program = FHEProgram(name="test_module")
        assert program.name == "test_module"
        assert program.graphs == {}
        assert program.global_vars == {}
        assert program.meta == {}

    def test_create_with_default_name(self):
        """Test FHEProgram creation with default name."""
        program = FHEProgram()
        assert program.name == "default_module"

    def test_name_setter(self):
        """Test name setter."""
        program = FHEProgram(name="old")
        program.name = "new"
        assert program.name == "new"


class TestFHEProgramProperties:
    """Tests for FHEProgram properties."""

    def test_format_type_is_memory(self):
        """Test that format_type returns 'memory'."""
        program = FHEProgram()
        assert program.format_type == "memory"

    def test_file_format_is_none(self):
        """Test that file_format returns None for memory IR."""
        program = FHEProgram()
        assert program.file_format is None

    def test_file_path_is_none(self):
        """Test that file_path returns None for memory IR."""
        program = FHEProgram()
        assert program.file_path is None

    def test_entry_name(self):
        """Test that entry_name returns program name."""
        program = FHEProgram(name="my_program")
        assert program.entry_name == "my_program"


class TestFHEProgramGraphs:
    """Tests for FHEProgram graph management."""

    def test_add_graph(self):
        """Test adding a graph."""
        graph = FHEGraph(name="forward")
        program = FHEProgram()
        program.add_graph("forward", graph)
        assert "forward" in program.graphs
        assert program.graphs["forward"] is graph

    def test_add_function(self):
        """Test adding a function."""
        graph = FHEGraph(name="forward")
        program = FHEProgram()
        program.add_function("forward", graph)
        assert "forward" in program.graphs

    def test_add_duplicate_function_raises(self):
        """Test that adding duplicate function raises ValueError."""
        graph = FHEGraph(name="forward")
        program = FHEProgram()
        program.add_function("forward", graph)
        with pytest.raises(ValueError, match="already exists"):
            program.add_function("forward", graph)

    def test_get_function(self):
        """Test getting a function by name."""
        graph = FHEGraph(name="forward")
        program = FHEProgram()
        program.add_graph("forward", graph)
        assert program.get_function("forward") is graph
        assert program.get_function("nonexistent") is None

    def test_list_functions(self):
        """Test listing all functions."""
        program = FHEProgram()
        program.add_graph("forward", FHEGraph(name="forward"))
        program.add_graph("backward", FHEGraph(name="backward"))
        assert set(program.list_functions()) == {"forward", "backward"}


class TestFHEProgramMainGraph:
    """Tests for FHEProgram main graph access."""

    def test_get_main_graph_with_forward(self):
        """Test getting main graph when 'forward' exists."""
        graph = FHEGraph(name="forward")
        program = FHEProgram()
        program.add_graph("forward", graph)
        assert program.get_main_graph() is graph

    def test_get_main_graph_single_graph(self):
        """Test getting main graph when only one graph exists."""
        graph = FHEGraph(name="single")
        program = FHEProgram()
        program.add_graph("single", graph)
        assert program.get_main_graph() is graph

    def test_get_main_graph_no_graph_raises(self):
        """Test that get_main_graph raises when no graph exists."""
        program = FHEProgram()
        with pytest.raises(ValueError, match="No main function found"):
            program.get_main_graph()


class TestFHEProgramNodes:
    """Tests for FHEProgram node access."""

    @pytest.fixture
    def program_with_graph(self):
        """Create a program with a graph containing nodes."""
        graph = FHEGraph(name="forward")
        graph.input_nodes = ["x"]
        graph.output_nodes = ["y"]

        block = BasicBlock(name="entry")
        block.add_node({"op_type": "add", "inputs": ["x"], "outputs": ["y"]})
        graph.add_block(block)
        graph.entry_block = block

        program = FHEProgram()
        program.add_graph("forward", graph)
        return program

    def test_nodes_property(self, program_with_graph):
        """Test nodes property returns nodes from main graph."""
        assert len(program_with_graph.nodes) == 1
        assert program_with_graph.nodes[0]["op_type"] == "add"

    def test_inputs_property(self, program_with_graph):
        """Test inputs property returns input nodes."""
        assert program_with_graph.inputs == ["x"]

    def test_outputs_property(self, program_with_graph):
        """Test outputs property returns output nodes."""
        assert program_with_graph.outputs == ["y"]


class TestFHEProgramExport:
    """Tests for FHEProgram export methods."""

    def test_export_ir_method_exists(self):
        """Test that export_ir method exists."""
        program = FHEProgram()
        assert hasattr(program, "export_ir")
        assert callable(program.export_ir)

    def test_write_ir_method_exists(self):
        """Test that write_ir method exists."""
        program = FHEProgram()
        assert hasattr(program, "write_ir")
        assert callable(program.write_ir)

    def test_write_ir_is_alias_for_export_ir(self, tmp_path):
        """Test that write_ir is an alias for export_ir."""
        program = FHEProgram()
        # Both should return same result (pickle for no extension)
        result1 = program.export_ir(str(tmp_path / "test1.pkl"))
        result2 = program.write_ir(str(tmp_path / "test2.pkl"))
        assert result1 == result2


class TestFHEProgramWithFixture:
    """Tests for FHEProgram using fixtures."""

    def test_fixture_program_has_graph(self, simple_fhe_program):
        """Test that fixture program has a graph."""
        assert simple_fhe_program.name == "test_program"
        assert "forward" in simple_fhe_program.graphs
        assert simple_fhe_program.get_main_graph() is not None