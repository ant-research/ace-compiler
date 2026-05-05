# IR Unit Test Design

This document describes the unit test design for the IR (Intermediate Representation) module.

## 1. IR Module Responsibilities

```
┌─────────────────────────────────────────────────────────────────┐
│                      IR Module Responsibilities                  │
├─────────────────────────────────────────────────────────────────┤
│  1. Data Structures - IRNode, BasicBlock, FHEGraph, FHEProgram  │
│  2. Format Conversion - IR format transformations               │
│  3. File Export      - IR → ONNX/.B files                       │
│  4. Validation       - Structural integrity, semantic equivalence│
└─────────────────────────────────────────────────────────────────┘
```

**Note:** Frontend input testing (Torch, ONNX, AST → internal IR) is covered in `test_frontend/` directory. See [frontend_unittest.md](frontend_unittest.md) for details.

## 2. Directory Structure

```
tests/test_unit/test_ir/
│
├── __init__.py
├── conftest.py                    # Shared fixtures
│
├── test_structure.py              # IR data structure tests
│
├── test_formats.py                # IR file format tests
│
├── test_conversion.py             # IR conversion tests
│
├── test_export.py                 # IR export tests
│
└── test_validation/               # IR validation tests
    ├── __init__.py
    ├── test_integrity.py          # General IR integrity tests
    └── test_resnet20_structure.py # ResNet20 IR structure baseline
```

## 3. Test Files Description

### 3.1 test_structure.py

Tests for IR data structures: `CompilationUnit`, `IRNode`, `BasicBlock`, `FHEGraph`, `FHEProgram`.

```python
class TestCompilationUnit:
    """Tests for CompilationUnit abstract base class."""
    def test_is_abstract(self): ...
    def test_cannot_instantiate_directly(self): ...
    def test_subclass_must_implement_format_type(self): ...
    def test_subclass_must_implement_entry_name(self): ...

class TestIRNode:
    """Tests for IRNode class."""
    def test_create_with_name(self): ...
    def test_default_attributes(self): ...
    def test_set_op_type(self): ...
    def test_set_inputs_outputs(self): ...
    def test_set_shape_dtype(self): ...

class TestBasicBlock:
    """Tests for BasicBlock class."""
    def test_create_with_name(self): ...
    def test_add_node(self): ...
    def test_successors_predecessors(self): ...

class TestFHEGraph:
    """Tests for FHEGraph class."""
    def test_create_with_name(self): ...
    def test_add_block(self): ...
    def test_input_output_nodes(self): ...
    def test_get_all_nodes(self): ...
    def test_to_dict(self): ...

class TestFHEProgram:
    """Tests for FHEProgram class."""
    def test_create_with_name(self): ...
    def test_add_graph(self): ...
    def test_get_main_graph(self): ...
    def test_list_functions(self): ...
```

### 3.2 test_formats.py

Tests for IR file format classes: `FileIR`, `ONNXFileIR`, `AIRFileIR`.

```python
class TestFileIR:
    """Tests for FileIR base class."""
    def test_format_type_is_file(self): ...
    def test_file_path_property(self): ...

class TestONNXFileIR:
    """Tests for ONNXFileIR class."""
    def test_creation_with_path(self): ...
    def test_file_format_is_onnx(self): ...
    def test_onnx_path_attribute(self): ...
    def test_entry_name_from_path(self): ...

class TestAIRFileIR:
    """Tests for AIRFileIR class."""
    def test_creation_with_path(self): ...
    def test_file_format_is_air(self): ...
    def test_air_path_attribute(self): ...
    def test_entry_name_from_path(self): ...
```

### 3.3 test_conversion.py

Tests for IR conversion functions.

```python
class TestONNXToFHEProgram:
    """Tests for ONNX → FHEProgram conversion."""
    def test_function_exists(self): ...
    def test_convert_returns_fhe_program(self): ...
    def test_convert_preserves_graph_structure(self): ...

class TestONNXToAIR:
    """Tests for ONNX → AIR binary conversion."""
    def test_function_exists(self): ...
    def test_convert_with_compiler(self): ...

class TestFHEProgramToONNX:
    """Tests for FHEProgram → ONNX conversion."""
    def test_function_exists(self): ...
    def test_export_creates_file(self): ...
    def test_export_creates_valid_onnx(self): ...
```

### 3.4 test_export.py

Tests for IR export functions.

```python
class TestONNXExport:
    """Tests for ONNX export functions."""
    def test_op_type_mapping_exists(self): ...
    def test_common_ops_mapped(self): ...
    def test_export_creates_file(self): ...
    def test_export_returns_bool(self): ...
    def test_export_creates_valid_onnx(self): ...

class TestAIRExport:
    """Tests for AIR export functions."""
    def test_supported_ops_exists(self): ...
    def test_common_ops_supported(self): ...
    def test_function_exists(self): ...
    def test_export_returns_bool(self): ...
```

### 3.5 test_validation/

Tests for IR validation and integrity checking.

#### test_integrity.py

General IR integrity validation tests.

```python
class TestIRIntegrity:
    """Tests for IR structural integrity."""
    
    @pytest.mark.skip(reason="TODO: Implement graph connectivity check")
    def test_graph_connectivity(self):
        """Test that all nodes are reachable from inputs."""
        pass
    
    @pytest.mark.skip(reason="TODO: Implement shape propagation check")
    def test_shape_propagation(self):
        """Test that shapes propagate correctly through the graph."""
        pass
    
    @pytest.mark.skip(reason="TODO: Implement node validity check")
    def test_node_validity(self):
        """Test that all nodes have valid op types and connections."""
        pass
```

#### test_resnet20_structure.py

ResNet20 IR structure baseline validation.

```python
class TestResNet20IRStructure:
    """Test ResNet20 IR structure against baseline."""
    
    def test_ir_structure_baseline(self):
        """Validate ResNet20 IR structure matches the baseline."""
        ...
    
    def test_operation_counts(self):
        """Test that operation counts match baseline."""
        ...
    
    def test_conv_layer_count(self):
        """Test that conv layer count matches baseline."""
        ...
    
    def test_constant_count(self):
        """Test that constant count matches baseline."""
        ...
    
    def test_input_output_shapes(self):
        """Test that input/output shapes match baseline."""
        ...
```

## 4. Migration Plan

### 4.1 Files to Merge

| Current File | Target | Action |
|--------------|--------|--------|
| `test_base.py` | `test_structure.py` | Merge |
| `test_graph.py` | `test_structure.py` | Merge |
| `test_fhe_program.py` | `test_structure.py` | Merge |
| `test_ir_formats.py` | `test_formats.py` | Rename |
| `test_torch_trace.py` | - | Delete (covered by `test_frontend/`) |
| `test_onnx_tools.py` | `test_export.py` + `test_conversion.py` | Split |
| `test_conversion/test_onnx_converter.py` | `test_conversion.py` | Merge |
| `test_conversion/test_ast_converter.py` | - | Delete (covered by `test_frontend/`) |
| `test_export/test_onnx_export.py` | `test_export.py` | Merge |
| `test_export/test_air_export.py` | `test_export.py` | Merge |
| `test_resnet20_ir_structure.py` | `test_validation/test_resnet20_structure.py` | Move |

### 4.2 Directories to Remove

| Directory | Reason |
|-----------|--------|
| `test_conversion/` | Merged into `test_conversion.py` |
| `test_export/` | Merged into `test_export.py` |

### 4.3 Final Structure (7 files)

```
tests/test_unit/test_ir/
├── __init__.py
├── conftest.py
├── test_structure.py
├── test_formats.py
├── test_conversion.py
├── test_export.py
└── test_validation/
    ├── __init__.py
    ├── test_integrity.py
    └── test_resnet20_structure.py
```

## 5. Shared Fixtures

The `conftest.py` file should provide shared fixtures for IR tests:

```python
# conftest.py
import pytest
import torch
import torch.nn as nn


class SimpleLinearModel(nn.Module):
    """Simple linear model for testing."""
    def __init__(self, in_features=4, out_features=4):
        super().__init__()
        self.linear = nn.Linear(in_features, out_features)
    
    def forward(self, x):
        return self.linear(x)


@pytest.fixture
def simple_linear_model():
    """Fixture providing a simple linear model."""
    return SimpleLinearModel()


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
```

## 6. Relationship with Other Test Directories

| Directory | Responsibility |
|-----------|----------------|
| `test_unit/test_frontend/` | Frontend input testing (Torch, ONNX, AST → IR) |
| `test_unit/test_ir/` | IR data structures, conversion, export, validation |
| `test_regression/` | End-to-end compilation and execution tests |

The IR unit tests focus on the IR module itself, independent of frontend input methods. Frontend-specific tests are in `test_frontend/`, while full compilation pipeline tests are in `test_regression/`.