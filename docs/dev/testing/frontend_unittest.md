# Frontend Module Unit Test Design

## Overview

This document describes the unit test design for the ACE FHE frontend module. The frontend module provides multiple conversion strategies to transform PyTorch models, Python functions, and ONNX files into AIR IR (Abstract Intermediate Representation) for FHE compilation.

## Test File Structure

```
tests/test_unit/test_frontend/
├── conftest.py              # Shared fixtures and model definitions
├── test_registry_frontend.py         # Frontend registration mechanism tests (12 tests)
├── test_onnx.py             # ONNX file → AIR tests (1 test)
├── test_ast.py              # Python function → AIR (AST) tests (13 tests)
├── test_ast_via_onnx.py     # Python function → ONNX → AIR tests (7 tests)
├── test_torch_frontend.py   # PyTorch model → AIR (FX) tests (45 tests)
└── test_torch_via_onnx.py   # PyTorch model → ONNX → AIR tests (36 tests)

Total: 112 tests (111 passed, 1 skipped)
```

## Conversion Path Coverage Matrix

| Input Type | Direct to AIR | Via ONNX |
|------------|---------------|----------|
| **PyTorch Model** | `torch` (test_torch_frontend.py) | `torch-via-onnx` (test_torch_via_onnx.py) |
| **Python Function** | `ast` (test_ast.py) | `ast-via-onnx` (test_ast_via_onnx.py) |
| **ONNX File** | N/A | `onnx` (test_onnx.py) |

---

## Module 1: Frontend Registry Tests

**File**: `test_registry_frontend.py`
**Tests**: 12

### Test Cases

| Test Case | Description |
|-----------|-------------|
| `test_all_frontends_registered` | Verify all 5 frontends are registered: `torch`, `torch-via-onnx`, `onnx`, `ast`, `ast-via-onnx` |
| `test_list_frontends_returns_list` | `list_frontends()` returns a list |
| `test_get_frontend_works` | `get_frontend()` returns a valid frontend instance |
| `test_get_all_frontends` | All registered frontends can be retrieved |
| `test_get_unknown_frontend_raises_error` | Getting unregistered frontend raises `ValueError` |
| `test_frontend_has_required_methods` | All frontends have `to_ir` method |
| `test_frontend_has_required_class_methods` | All frontend classes have `name()` class method |
| `test_frontend_name_matches_registration` | `frontend.name()` returns the registered name |
| `test_frontend_returns_correct_class_type` | Each frontend returns the correct class type |
| `test_register_duplicate_frontend_raises_error` | Duplicate registration raises `ValueError` |
| `test_get_frontend_returns_new_instance` | Each `get_frontend()` call returns a new instance |
| `test_get_frontend_with_kwargs` | Supports passing kwargs to frontend constructor |

### Design Rationale

- **Single Responsibility**: Registry tests are isolated in one file to avoid duplication across frontend-specific tests
- **Interface Validation**: Ensures all frontends conform to the expected interface
- **Error Handling**: Validates proper error messages for invalid operations

---

## Module 2: ONNX Frontend Tests

**File**: `test_onnx.py`
**Tests**: 1

### Conversion Path

```
ONNX File → ONNX Parser → AIR IR
```

### Test Cases

| Test Case | Description |
|-----------|-------------|
| `test_onnx_file_to_air` | Convert ONNX file to AIR IR, verify Gemm operator presence |

### Design Rationale

- Uses `onnx_file` fixture from top-level `conftest.py` which creates a temporary ONNX file
- Simple model (Linear layer) exports as Gemm operator in ONNX

---

## Module 3: AST Frontend Tests

**File**: `test_ast.py`
**Tests**: 13

### Conversion Path

```
Python Function → AST Parser → AIR IR (CFG with BasicBlocks)
```

### Test Classes

#### TestASTFunctionToAir (6 tests)

| Test Case | Description |
|-----------|-------------|
| `test_add_function_to_air` | Add function → AIR |
| `test_mul_function_to_air` | Multiply function → AIR |
| `test_sub_function_to_air` | Subtract function → AIR |
| `test_div_function_to_air` | Divide function → AIR |
| `test_multiple_args_function_to_air` | Multi-argument function, verify input_nodes count |
| `test_complex_expression_to_air` | Complex expression `(x+y)*2-1` → AIR |

#### TestASTModelToAir (1 test)

| Test Case | Description |
|-----------|-------------|
| `test_simple_model_to_air` | Simple model → AIR, verify graphs structure |

#### TestASTToIRConverter (4 tests)

| Test Case | Description |
|-----------|-------------|
| `test_converter_convert_function` | Basic ASTToIRConverter function conversion |
| `test_converter_with_custom_name` | Custom graph name support |
| `test_converter_input_nodes` | Input nodes are correctly captured |
| `test_converter_output_nodes` | Output nodes are correctly captured |

### Design Rationale

- Test functions are defined at module level for `inspect.getsource` to work
- Direct converter testing isolates the AST-to-IR logic from frontend orchestration

---

## Module 4: AST Via ONNX Frontend Tests

**File**: `test_ast_via_onnx.py`
**Tests**: 7

### Conversion Path

```
Python Function → ONNX Export → ONNX Parser → AIR IR
```

### Test Classes

#### TestASTViaOnnxToIR (6 tests)

| Test Case | Description |
|-----------|-------------|
| `test_function_to_air` | Function → AIR, verify Add operator |
| `test_function_to_onnx_ir` | Function → ONNX IR (target_ir="onnx") |
| `test_mul_function_to_air` | Multiply function, verify Mul operator |
| `test_relu_function_to_air` | ReLU function, verify Relu operator |
| `test_complex_function_to_air` | Complex function conversion |
| `test_auto_generate_input_names` | Auto-generate input names when not provided |

#### TestASTViaOnnxWithModel (1 test)

| Test Case | Description |
|-----------|-------------|
| `test_model_to_air` | PyTorch model → AIR via ONNX, verify Gemm |

### Design Rationale

- Tests both `target_ir="air"` and `target_ir="onnx"` output modes
- Validates that Python functions can be exported via torch.onnx.export

---

## Module 5: Torch Frontend Tests

**File**: `test_torch_frontend.py`
**Tests**: 45 (44 passed, 1 skipped)

### Conversion Path

```
PyTorch Model → FX Tracing → TorchTracedModel → C++ Kernels → AIR IR
```

### Test Classes

#### TestTorchFrontendToIR (18 tests)

FX tracing functionality tests.

| Category | Test Cases |
|----------|------------|
| **Basic** | `test_simple_model_to_ir`, `test_traced_model_format_type`, `test_auto_generate_input_names` |
| **Binary Ops** | `test_custom_op_model_to_ir` (add), `test_sub_model_to_ir`, `test_mul_model_to_ir`, `test_div_model_to_ir`, `test_matmul_model_to_ir`, `test_concat_model_to_ir` |
| **Unary Ops** | `test_relu_model_to_ir`, `test_softmax_model_to_ir`, `test_max_pool_model_to_ir`, `test_flatten_model_to_ir`, `test_sqrt_model_to_ir`, `test_silu_model_to_ir` |
| **Ternary Ops** | `test_conv_model_to_ir`, `test_gemm_model_to_ir` |
| **Composite** | `test_composite_model_to_ir` |

#### TestTorchTracedModelExecution (20 tests)

AIR IR generation via C++ kernel execution tests.

| Category | Test Cases |
|----------|------------|
| **Basic** | `test_execute_generates_air`, `test_call_equivalent_to_execute`, `test_get_air_scopes` |
| **Binary Ops** | `test_execute_add`, `test_execute_sub`, `test_execute_mul`, `test_execute_div`, `test_execute_matmul`, `test_execute_concat` |
| **Unary Ops** | `test_execute_relu`, `test_execute_softmax`, `test_execute_max_pool`, `test_execute_avg_pool`, `test_execute_global_avg_pool`, `test_execute_flatten`, `test_execute_sqrt`, `test_execute_silu` |
| **Ternary Ops** | `test_execute_conv`, `test_execute_gemm` |
| **Composite** | `test_execute_composite` |

#### TestTorchTracedModelMethods (4 tests)

Helper method tests.

| Test Case | Description |
|-----------|-------------|
| `test_write_ir_success` | Write AIR IR to file successfully |
| `test_write_ir_before_execute_raises` | Write before execute raises `RuntimeError` |
| `test_print_graph` | Print FX graph without error |
| `test_get_graph_code` | Get graph code as string |

#### TestTorchFrontendWithBackend (3 tests)

Backend integration tests.

| Test Case | Description |
|-----------|-------------|
| `test_backend_supports_torch_traced_format` | AntLIB supports `torch_traced` format type |
| `test_traced_model_with_backend` | Backend can process traced model |
| `test_backend_compile_torch_traced` | End-to-end compilation (SKIPPED: C++ extension segfault) |

### Design Rationale

- **Two-Phase Testing**: Separates FX tracing (`to_ir`) from AIR generation (`execute`)
- **Custom Operators**: Uses `torch.ops.tensor.*` custom C++ operators for AIR IR generation
- **Backend Integration**: Validates format type compatibility with AntLIB backend

---

## Module 6: Torch Via ONNX Frontend Tests

**File**: `test_torch_via_onnx.py`
**Tests**: 36

### Conversion Path

```
PyTorch Model → torch.onnx.export → ONNX → AIR IR
```

### Test Classes

#### TestTorchViaOnnxToIR (4 tests)

Basic conversion tests.

| Test Case | Description |
|-----------|-------------|
| `test_model_to_air_via_onnx` | Model → AIR, verify Gemm operator |
| `test_function_to_air_via_onnx` | Function → AIR, verify Relu operator |
| `test_model_with_relu` | ReLU model conversion |
| `test_model_with_conv_relu` | Conv+ReLU model conversion |

#### TestTorchViaOnnxModelCases (32 tests)

Parametrized test cases from `tests/test_cases/`.

| Category | Test Cases |
|----------|------------|
| **Basic Ops** | `add_model`, `add_1_dimension`, `add_const`, `mult_model`, `mult_const` |
| **Activations** | `relu`, `sigmoid`, `tanh` |
| **Shape Ops** | `flatten`, `loop` |
| **Convolution** | `conv2d`, `conv2d_relu`, `conv2d_bn_relu`, `depthwise_conv2d`, `separable_conv2d`, `conv_transpose2d` |
| **GEMM** | `gemm_49x3`, `gemm_fixed_weights`, `gemm_relu`, `small_gemm_relu`, `relu_gemm`, `small_relu_gemm`, `mlp` |
| **Pooling** | `avg_pool_2d`, `avg_pool_2d_with_stride`, `max_pool_2d`, `global_avg_pool`, `global_max_pool` |
| **Combinations** | `avg_pool_conv2d`, `conv2d_avg_pool`, `relu_avg_pool`, `avg_pool_flatten` |

### ONNX Operator Naming Handling

The test handles ONNX export differences:

- **BatchNorm**: May be fused into Conv during ONNX export (skipped in assertion)
- **GlobalMaxPool**: `adaptive_max_pool2d` exports as `MaxPool` in ONNX (mapped assertion)

### Design Rationale

- **Parametrized Testing**: Uses `model_case` fixture for comprehensive model coverage
- **Expected Ops Validation**: Each test case defines expected operators for verification

---

## Operator Coverage Summary

| Operator Type | Operators | Test Files |
|---------------|-----------|------------|
| **Binary** | Add, Sub, Mul, Div, MatMul, Concat | test_torch_frontend.py, test_torch_via_onnx.py |
| **Unary** | ReLU, Softmax, Sqrt, Silu | test_torch_frontend.py, test_torch_via_onnx.py |
| **Pooling** | MaxPool, AvgPool, GlobalAvgPool, GlobalMaxPool | test_torch_frontend.py, test_torch_via_onnx.py |
| **Convolution** | Conv, ConvTranspose, DepthwiseConv | test_torch_frontend.py, test_torch_via_onnx.py |
| **GEMM** | Gemm (Linear) | test_torch_frontend.py, test_torch_via_onnx.py |
| **Shape** | Flatten | test_torch_frontend.py, test_torch_via_onnx.py |
| **Activation** | Sigmoid, Tanh | test_torch_via_onnx.py |

---

## Shared Fixtures

**File**: `conftest.py`

### Model Fixtures (Custom C++ Operators)

Used by `test_torch.py`:

| Fixture | Model Class | Description |
|---------|-------------|-------------|
| `add_model` | `AddTensorOp` | `torch.ops.tensor.add` |
| `sub_model` | `SubTensorOp` | `torch.ops.tensor.sub` |
| `mul_model` | `MulTensorOp` | `torch.ops.tensor.mul` |
| `div_model` | `DivTensorOp` | `torch.ops.tensor.div` |
| `matmul_model` | `MatmulTensorOp` | `torch.ops.tensor.matmul` |
| `concat_model` | `ConcatTensorOp` | `torch.ops.tensor.concat` |
| `relu_model` | `ReLUTensorOp` | `torch.ops.tensor.relu` |
| `softmax_model` | `SoftmaxTensorOp` | `torch.ops.tensor.softmax` |
| ... | ... | ... |

### Model Fixtures (Standard PyTorch)

Used by `test_torch_via_onnx.py`:

| Fixture | Model Class | Description |
|---------|-------------|-------------|
| `relu_model_std` | `ReluModel` | `torch.relu(x)` |
| `conv_relu_model` | `ConvReluModel` | `nn.Conv2d` + `torch.relu` |

### Input Tensor Fixtures

| Fixture | Shape | Description |
|---------|-------|-------------|
| `input_1d` | `(3,)` | 1D tensor |
| `input_1d_another` | `(3,)` | Another 1D tensor |
| `input_2d` | `(2, 2)` | 2D tensor |
| `input_2d_another` | `(2, 2)` | Another 2D tensor |
| `input_4d` | `(1, 1, 2, 2)` | 4D tensor (NCHW) |
| `conv_weight` | `(1, 1, 2, 2)` | Convolution weight |
| `conv_bias` | `(1,)` | Convolution bias |
| `gemm_bias` | `(2, 2)` | GEMM bias |

---

## Test Summary

| Module | Tests | Description |
|--------|-------|-------------|
| Registry | 12 | Registration, interface, error handling |
| ONNX Frontend | 1 | ONNX file → AIR |
| AST Frontend | 13 | Function → AIR (AST analysis) |
| AST Via ONNX Frontend | 7 | Function → ONNX → AIR |
| Torch Frontend | 45 | FX tracing, AIR generation, methods |
| Torch Via ONNX Frontend | 36 | Model/Function → ONNX → AIR |
| **Total** | **112** | 111 passed, 1 skipped |

---

## Running Tests

```bash
# Run all frontend tests
pytest tests/test_unit/test_frontend/ -v

# Run specific test file
pytest tests/test_unit/test_frontend/test_torch_frontend.py -v

# Run specific test class
pytest tests/test_unit/test_frontend/test_torch_frontend.py::TestTorchFrontendToIR -v

# Run with coverage
pytest tests/test_unit/test_frontend/ --cov=python/ace/fhe/frontend --cov-report=html
```