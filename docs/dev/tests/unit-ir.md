# IR Module Unit Test Design

## Overview

This document describes the unit test design for the IR (Intermediate Representation) module. The IR module handles data structures, format conversion, file export, and structural validation.

Frontend input testing (Torch, ONNX, AST → internal IR) is covered in `tests/unit/frontend/`.

## Test File Structure

```
tests/unit/ir/
├── conftest.py          # Shared IR fixtures
├── test_structure.py    # IR data structure tests (IRNode, BasicBlock, FHEGraph, FHEProgram)
├── test_format.py       # IR file format tests (FileIR, ONNXFileIR, AIRFileIR)
└── test_export.py       # IR export tests (ONNX export, AIR export, op type mapping)
```

---

## Module 1: IR Structure Tests

**File**: `test_structure.py`

Tests for IR data structures: `CompilationUnit`, `IRNode`, `BasicBlock`, `FHEGraph`, `FHEProgram`.

### Test Classes

| Test Class | Tests | Description |
|------------|-------|-------------|
| `TestCompilationUnit` | 5 | Abstract base class, subclass contracts, `file_format` default |
| `TestIRNode` | 4 | Creation, defaults, attribute setting, fixture node |
| `TestBasicBlock` | 6 | Creation, defaults, add_node, successors/predecessors, fixture block |
| `TestFHEGraph` | 10 | Creation, add_block, entry_block, unique names, I/O nodes, metadata, to_dict, fixture graph |
| `TestFHEProgram` | 13 | Creation, format_type, file_format, file_path, add_graph/function, get/list functions, get_main_graph, nodes, write_ir alias, fixture program |

---

## Module 2: IR Format Tests

**File**: `test_format.py`

Tests for IR file format classes: `FileIR`, `ONNXFileIR`, `AIRFileIR`, backward compatibility aliases.

### Test Classes

| Test Class | Tests | Description |
|------------|-------|-------------|
| `TestONNXFileIR` | 6 | format_type, file_format, file_path, onnx_path alias, entry_name parsing |
| `TestAIRFileIR` | 5 | format_type, file_format, file_path, air_path alias, entry_name |
| `TestIRFormatComparison` | 2 | Different file formats, same entry name from same stem |
| `TestBackwardCompatibility` | 5 | ONNXModel/AIRModel/FileModel aliases, instance creation |

---

## Module 3: IR Export Tests

**File**: `test_export.py`

Tests for IR export functions and op type mappings.

### Helper Function

```python
def _make_program(name="test", op_type="add", input_names=None, output_names=None):
    """Create a minimal FHEProgram for export testing."""
    ...
```

### Test Classes

| Test Class | Tests | Description |
|------------|-------|-------------|
| `TestOPTypeMapping` | 3 | Mapping exists, common ops mapped, values are strings |
| `TestExportToOnnx` | 4 | Creates file, returns bool, valid ONNX, multiple nodes |
| `TestSupportedAirOps` | 3 | Set exists, common ops supported, all ops are strings |
| `TestExportToAir` | 2 | Function exists, returns bool |

---

## Shared Fixtures

**File**: `conftest.py`

| Fixture | Description |
|---------|-------------|
| `simple_fhe_graph` | FHEGraph with one add node |
| `simple_fhe_program` | FHEProgram with one forward graph |
| `simple_ir_node` | IRNode with add op |
| `simple_basic_block` | BasicBlock with add + relu nodes |

---

## Running Tests

```bash
# Run all IR tests
pytest tests/unit/ir/ -v

# Run specific module
pytest tests/unit/ir/test_structure.py -v
pytest tests/unit/ir/test_format.py -v
pytest tests/unit/ir/test_export.py -v
```