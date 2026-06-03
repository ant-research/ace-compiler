# Frontend Module Unit Test Design

## Overview

This document describes the unit test design for the ACE FHE frontend module. The frontend module provides multiple conversion strategies to transform PyTorch models, Python functions, and ONNX files into AIR IR for FHE compilation.

## Test File Structure

```
tests/unit/frontend/
├── conftest.py              # Shared fixtures
├── test_ast.py              # AST frontend tests
├── test_onnx.py             # ONNX frontend tests
├── test_torch.py            # Torch frontend (FX trace) tests
└── test_torch_via_onnx.py   # Torch-via-ONNX frontend tests
```

## Conversion Path Coverage Matrix

| Input Type | Direct to AIR | Via ONNX |
|------------|---------------|----------|
| **PyTorch Model** | `torch` (test_torch.py) | `torch-via-onnx` (test_torch_via_onnx.py) |
| **Python Function** | `ast` (test_ast.py) | N/A |
| **ONNX File** | N/A | `onnx` (test_onnx.py) |

---

## Module 1: AST Frontend Tests

**File**: `test_ast.py`
**Skip marker**: `@skip_if_no_frontend`
**Test data**: `ADD_FUNC`, `MUL_FUNC`, `RELU_FUNC` from `ace.sample.funcs.specs`

### Test Classes

| Test Class | Skip | Tests | Description |
|------------|------|-------|-------------|
| `TestPrepare` | No | 4 | FHEProgram creation, memory mode, graph structure |
| `TestCompile` | `@skip_if_no_frontend` | 4 | Compile to AIR, format_type, file_path |
| `TestExportAir` | `@skip_if_no_frontend` | 1 | Export to AIR file |
| `TestIRProperties` | No | 3 | Entry name, main graph, I/O nodes |
| `TestEdgeCases` | `@skip_if_no_frontend` | 1 | Export to valid path |
| `TestFrontendMeta` | No | 1 | Frontend name property |

---

## Module 2: ONNX Frontend Tests

**File**: `test_onnx.py`
**Skip marker**: `@skip_if_no_onnx`
**Test data**: `LINEAR_OP`, `RELU_OP`, `ADD_OP` from `ace.sample.ops.specs`

### Test Classes

| Test Class | Skip | Tests | Description |
|------------|------|-------|-------------|
| `TestPrepare` | `@skip_if_no_onnx` | 4 | ONNXFileIR creation, file mode, input handling |
| `TestCompile` | `@skip_if_no_onnx` | 2 | Compile returns ONNXFileIR |
| `TestExportOnnx` | `@skip_if_no_onnx` | 3 | ONNX file export, content verification |
| `TestExportAir` | `@skip_if_no_onnx` + `@skip_if_no_frontend` | 1 | ONNX → AIR export |
| `TestIRProperties` | `@skip_if_no_onnx` | 1 | ONNXFileIR properties |
| `TestEdgeCases` | `@skip_if_no_onnx` | 3 | Nonexistent file, invalid ONNX, path handling |
| `TestFrontendMeta` | `@skip_if_no_onnx` | 1 | Frontend name property |

### Helper Function

```python
def _export_onnx(spec, tmp_path, input_names=None):
    """Export a ModelSpec to ONNX file."""
    model = spec.create_model()
    onnx_path = str(tmp_path / f"{spec.name}.onnx")
    torch.onnx.export(model, tuple(spec.example_inputs), onnx_path, ...)
    return onnx_path
```

---

## Module 3: Torch Frontend Tests

**File**: `test_torch.py`
**Skip marker**: `@requires_frontend` (requires `torch.fx` + C++ extension)
**Test data**: `ADD_OP`, `RELU_OP`, `LINEAR_OP`, `CONV2D_RELU_OP`, `GLOBAL_AVG_POOL_OP` + tensor ops from `ace.sample.tensor_ops`

### Test Classes

| Test Class | Tests | Description |
|------------|-------|-------------|
| `TestPrepare` | 4 | TorchTracedModel creation, memory mode, graph, input names |
| `TestCompile` | 3 | Compile to AIR, format_type, air_is_generated |
| `TestExportAir` | 2 | Export to AIR file, export_ir properties |
| `TestIRProperties` | 2 | Entry name, air_scopes |
| `TestTorchTracedModelMethods` | 6 | write_ir, print_graph, get_graph_code, __call__, export_ir |
| `TestStandardPyTorchModels` | 9 | Compile/export for standard models, graph transform rewrites |
| `TestFrontendMeta` | 1 | Frontend name property |

---

## Module 4: Torch-via-ONNX Frontend Tests

**File**: `test_torch_via_onnx.py`
**Skip marker**: `@skip_if_no_onnx`
**Test data**: `LINEAR_OP`, `RELU_OP`, `ADD_OP` from `ace.sample.ops.specs`, `ADD_FUNC` from `ace.sample.funcs.specs`

### Test Classes

| Test Class | Tests | Description |
|------------|-------|-------------|
| `TestPrepare` | 3 | ONNXFileIR creation, file mode, multi-input function |
| `TestCompile` | 1 | Compile returns ONNXFileIR |
| `TestIRProperties` | 1 | ONNXFileIR properties |
| `TestEdgeCases` | 3 | None input, empty inputs, wrong input count |
| `TestFrontendMeta` | 1 | Frontend name property |

---

## Shared Fixtures

**File**: `conftest.py`

| Fixture | Description |
|---------|-------------|
| `torch_frontend` | `get_frontend("torch")` instance |
| `torch_via_onnx_frontend` | `get_frontend("torch-via-onnx")` instance |
| `ast_frontend` | `get_frontend("ast")` instance |
| `onnx_frontend` | `get_frontend("onnx")` instance |

---

## Running Tests

```bash
# Run all frontend tests
pytest tests/unit/frontend/ -v

# Run specific frontend
pytest tests/unit/frontend/test_torch.py -v
pytest tests/unit/frontend/test_ast.py -v
pytest tests/unit/frontend/test_onnx.py -v
pytest tests/unit/frontend/test_torch_via_onnx.py -v

# Skip C++ extension-dependent tests
pytest tests/unit/frontend/ -k "not frontend"
```