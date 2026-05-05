# Driver Module Unit Test Design

## Overview

This document describes the unit test design for the ACE FHE driver module. The driver module orchestrates the complete compilation flow from source input through frontend conversion to backend compilation, producing executable FHE libraries.

## Test File Structure

```
tests/test_unit/test_driver/
├── conftest.py                 # Shared fixtures and model definitions
├── test_registry.py            # Frontend/Backend registration tests (26 tests)
└── test_compilation_paths.py   # Compilation path coverage tests (58 tests)

Total: 84 tests (31 passed, 53 skipped)
```

## Compilation Flow

```
Source → Frontend.to_ir() → IR → Backend.compile_to_lib() → Output
```

---

## Module 1: Registry Tests

**File**: `test_registry.py`
**Tests**: 26 (24 passed, 2 skipped)

### Test Classes

### TestFrontendRegistry (12 tests)

Frontend registration and retrieval tests.

| Test Case | Description |
|-----------|-------------|
| `test_list_frontends_returns_list` | `list_frontends()` returns a list |
| `test_all_frontends_registered` | Verify all 5 frontends are registered |
| `test_get_frontend_works` | `get_frontend()` returns a valid frontend |
| `test_get_all_frontends` | All registered frontends can be retrieved |
| `test_get_frontend_returns_new_instance` | Each call returns a new instance |
| `test_get_frontend_with_kwargs` | Supports passing kwargs to constructor |
| `test_get_unknown_frontend_raises_error` | Getting unregistered frontend raises `ValueError` |
| `test_frontend_has_required_methods` | All frontends have `to_ir` method |
| `test_frontend_has_required_class_methods` | All frontend classes have `name()` method |
| `test_frontend_name_matches_registration` | `frontend.name()` returns registered name |
| `test_frontend_returns_correct_class_type` | Each frontend returns correct class type |
| `test_register_duplicate_frontend_raises_error` | Duplicate registration raises `ValueError` |

### TestBackendRegistry (14 tests)

Backend registration and retrieval tests.

| Test Case | Description |
|-----------|-------------|
| `test_list_backends_returns_list` | `list_backends()` returns a list |
| `test_all_backends_registered` | Verify all 5 backends are registered |
| `test_list_backends_supported` | `list_backends_supported()` returns valid combos |
| `test_check_backend_antlib_cpu` | Check antlib CPU backend availability |
| `test_check_backend_antlib_cuda` | Check antlib CUDA backend availability |
| `test_check_backend_invalid_device` | Invalid device returns False |
| `test_check_backend_invalid` | Invalid backend returns False |
| `test_get_backend_antlib` | Get antlib backend instance |
| `test_get_backend_returns_new_instance` | Each call returns a new instance |
| `test_get_nonexistent_backend_raises` | Getting nonexistent backend raises error |
| `test_backend_has_required_methods` | All backends have required methods |
| `test_backend_has_required_class_methods` | All backend classes have required methods |
| `test_register_new_backend` | Register a new backend |
| `test_register_duplicate_backend_raises` | Duplicate registration raises `ValueError` |

---

## Module 2: Compilation Path Tests

**File**: `test_compilation_paths.py`
**Tests**: 58 (5 passed, 53 skipped)

### Coverage Matrix

The tests cover all combinations of 5 frontends × 5 backends = 25 paths:

| Frontend \ Backend | antlib (cpu) | seal (cpu) | openfhe (cpu) | phantom (cuda) | hyperfhe (cuda) |
|--------------------|--------------|------------|---------------|----------------|-----------------|
| **torch** | ✓ | ✓ | ✓ | ✓* | ✓* |
| **torch-via-onnx** | ✓ | ✓ | ✓ | ✓* | ✓* |
| **onnx** | ✓ | ✓ | ✓ | ✓* | ✓* |
| **ast** | ✓ | ✓ | ✓ | ✓* | ✓* |
| **ast-via-onnx** | ✓ | ✓ | ✓ | ✓* | ✓* |

*GPU backends (phantom, hyperfhe) are skipped if GPU not available

### Frontend Types

| Frontend | Input Type | Conversion Path | Requirements |
|----------|------------|-----------------|--------------|
| `torch` | PyTorch Model | torch.fx → AIR | torch.fx, ace_ext |
| `torch-via-onnx` | PyTorch Model | ONNX → AIR | None |
| `onnx` | ONNX File | ONNX → AIR | ONNX file |
| `ast` | Python Function | AST → AIR | ace_ext |
| `ast-via-onnx` | Python Function | ONNX → AIR | None |

### Backend Types

| Backend | Device | Description | Status |
|---------|--------|-------------|--------|
| `antlib` | cpu | Ant Group FHE library | Implemented |
| `seal` | cpu | Microsoft SEAL | Not implemented |
| `openfhe` | cpu | OpenFHE library | Partial |
| `phantom` | cuda | Phantom GPU library | Requires GPU |
| `hyperfhe` | cuda | HyperFHE GPU library | Requires GPU |

### Test Classes

#### TestDriverTorchPath (10 tests)

Tests for `torch` frontend → all backends.

| Test Case | Description |
|-----------|-------------|
| `test_torch_simple_model[antlib]` | Simple model to antlib |
| `test_torch_simple_model[seal]` | Simple model to seal (SKIPPED) |
| `test_torch_simple_model[openfhe]` | Simple model to openfhe (SKIPPED) |
| `test_torch_simple_model[phantom]` | Simple model to phantom (SKIPPED: GPU) |
| `test_torch_simple_model[hyperfhe]` | Simple model to hyperfhe (SKIPPED: GPU) |
| `test_torch_linear_model[antlib]` | Linear model to antlib |
| `test_torch_linear_model[seal]` | Linear model to seal (SKIPPED) |
| `test_torch_linear_model[openfhe]` | Linear model to openfhe (SKIPPED) |
| `test_torch_linear_model[phantom]` | Linear model to phantom (SKIPPED: GPU) |
| `test_torch_linear_model[hyperfhe]` | Linear model to hyperfhe (SKIPPED: GPU) |

#### TestDriverTorchViaOnnxPath (15 tests)

Tests for `torch-via-onnx` frontend → all backends.

| Test Case | Description |
|-----------|-------------|
| `test_torch_via_onnx_simple_model[backend]` | Simple model to each backend |
| `test_torch_via_onnx_linear_model[backend]` | Linear model to each backend |
| `test_torch_via_onnx_function[backend]` | Python function to each backend |

#### TestDriverOnnxPath (10 tests)

Tests for `onnx` frontend → all backends.

| Test Case | Description |
|-----------|-------------|
| `test_onnx_simple_model[backend]` | Simple ONNX model to each backend |
| `test_onnx_linear_model[backend]` | Linear ONNX model to each backend |

#### TestDriverASTPath (10 tests)

Tests for `ast` frontend → all backends.

| Test Case | Description |
|-----------|-------------|
| `test_ast_add_function[backend]` | Add function to each backend |
| `test_ast_relu_function[backend]` | ReLU function to each backend |

#### TestDriverASTViaOnnxPath (10 tests)

Tests for `ast-via-onnx` frontend → all backends.

| Test Case | Description |
|-----------|-------------|
| `test_ast_via_onnx_add_function[backend]` | Add function to each backend |
| `test_ast_via_onnx_relu_function[backend]` | ReLU function to each backend |

#### TestDriverErrorHandling (3 tests)

Error handling tests.

| Test Case | Description |
|-----------|-------------|
| `test_invalid_frontend_raises` | Invalid frontend raises `ValueError` |
| `test_invalid_backend_raises` | Invalid backend raises `ValueError` |
| `test_input_count_mismatch` | Input count mismatch raises error |

---

## Conditional Skip Logic

### GPU Backend Skip

GPU backends use `pytest.param` with conditional skip markers:

```python
GPU_BACKENDS = [
    pytest.param("phantom", "cuda",
                 marks=pytest.mark.skipif(not gpu_available(), reason="GPU not available"),
                 id="phantom"),
    pytest.param("hyperfhe", "cuda",
                 marks=pytest.mark.skipif(not gpu_available(), reason="GPU not available"),
                 id="hyperfhe"),
]
```

### GPU Availability Check

```python
def gpu_available() -> bool:
    """Check if GPU (CUDA) is available."""
    try:
        result = subprocess.run(["nvcc", "--version"], capture_output=True, text=True)
        return result.returncode == 0
    except FileNotFoundError:
        return False
```

### Frontend Dependency Skip

```python
@pytest.mark.skipif(not HAS_TORCH_FX or not HAS_ACE_EXT,
                    reason="torch.fx or ace_ext not available")
class TestDriverTorchPath:
    ...
```

---

## Shared Fixtures

**File**: `conftest.py`

### Dependency Checks

| Variable | Description |
|----------|-------------|
| `HAS_TORCH_FX` | `torch.fx` module available |
| `HAS_ACE_EXT` | `ace_ext.air_gen` module available |

### Test Models

| Fixture | Model Class | Description |
|---------|-------------|-------------|
| `SimpleModel` | `nn.Module` | Simple `x + 1` model |
| `LinearModel` | `nn.Module` | Linear layer model |

### Test Functions

| Function | Description |
|----------|-------------|
| `add_function(x, y)` | Simple add function |
| `relu_function(x)` | ReLU function |

---

## Skip Reasons Summary

| Skip Reason | Cause |
|-------------|-------|
| GPU not available | nvcc not found or CUDA not installed |
| torch.fx or ace_ext not available | Missing dependencies for torch frontend |
| Backend X not implemented | Backend.compile_to_lib raises NotImplementedError |
| Compilation failed | Backend-specific compilation error |

---

## Running Tests

```bash
# Run all driver tests
pytest tests/test_unit/test_driver/ -v

# Run registry tests only
pytest tests/test_unit/test_driver/test_registry.py -v

# Run compilation path tests only
pytest tests/test_unit/test_driver/test_compilation_paths.py -v

# Run specific frontend tests
pytest tests/test_unit/test_driver/test_compilation_paths.py -k "TorchPath" -v

# Run with skip reasons
pytest tests/test_unit/test_driver/ -v -rs

# Run with coverage
pytest tests/test_unit/test_driver/ --cov=python/ace/fhe/compiler --cov-report=html
```

---

## Design Rationale

### Test Organization

1. **Separation of Concerns**: Registry tests isolated from compilation path tests
2. **Parametrized Testing**: Backend combinations use `pytest.param` for clean skip handling
3. **Fixture Reuse**: Shared models and functions defined in `conftest.py`

### Skip Strategy

1. **GPU Backends**: Skip at parametrization level, not in test body
2. **Missing Dependencies**: Skip at class level for clean test collection
3. **Not Implemented**: Skip in test body with clear error message

### Coverage Goals

1. **Path Coverage**: All 25 frontend × backend combinations
2. **Error Handling**: Invalid inputs and configurations
3. **Interface Validation**: Registry operations and instance creation