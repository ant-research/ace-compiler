# Driver Module Unit Test Design

## Overview

This document describes the unit test design for the ACE FHE driver module. The driver module orchestrates the complete compilation flow from source input through frontend conversion to backend compilation, producing executable FHE libraries.

## Test File Structure

```
tests/unit/driver/
├── test_registry.py     # Frontend/backend registration and lookup tests
└── test_pipeline.py     # Driver compilation pipeline path tests
```

## Compilation Flow

```
Source → Frontend.to_ir() → IR → Backend.compile_to_lib() → Output
```

---

## Module 1: Registry Tests

**File**: `test_registry.py`

Tests frontend/backend registration, lookup, and provider properties. Uses parametrized `PROVIDER_SPECS` and `ALL_PROVIDER` from `utils`.

### Test Classes

#### TestFrontendRegistry

Frontend registration and retrieval tests.

| Test Case | Description |
|-----------|-------------|
| `test_list_frontends_returns_list` | `list_frontends()` returns a list |
| `test_all_frontends_registered` | Verify all 5 frontends are registered |
| `test_get_frontend_returns_instance` | `get_frontend()` returns a valid instance |
| `test_get_frontend_each` | All registered frontends can be retrieved (parametrized) |
| `test_get_frontend_returns_new_instance` | Each call returns a new instance |
| `test_get_unknown_frontend_raises` | Getting unregistered frontend raises `ValueError` |
| `test_frontend_has_required_methods` | All frontends have `to_ir` method (parametrized) |
| `test_frontend_name_matches_registration` | `frontend.name()` returns registered name (parametrized) |
| `test_frontend_class_type` | Each frontend returns correct class type (parametrized) |
| `test_register_duplicate_frontend_raises` | Duplicate registration raises `ValueError` |

#### TestLibraryRegistry

Backend library registration, lookup, and provider property tests.

| Test Case | Description |
|-----------|-------------|
| `test_list_libraries_returns_list` | `list_libraries()` returns a list |
| `test_all_providers_registered` | Verify all providers are registered |
| `test_list_supported_combos` | `list_supported_combos()` returns valid combos |
| `test_check_library_antlib_cpu` | Check antlib CPU availability |
| `test_check_library_invalid_device` | Invalid device returns False |
| `test_check_library_invalid_name` | Invalid name returns False |
| `test_get_library_impl_each` | All providers can be retrieved (parametrized with `ALL_PROVIDER`) |
| `test_get_library_impl_returns_new_instance` | Each call returns a new instance |
| `test_get_nonexistent_library_raises` | Getting nonexistent library raises error |
| `test_impl_has_required_methods` | All impls have required methods (parametrized with `ALL_PROVIDER`) |
| `test_register_duplicate_library_raises` | Duplicate registration raises `ValueError` |

---

## Module 2: Pipeline Tests

**File**: `test_pipeline.py`

Tests the Driver's handling of different frontend → IR → backend compilation paths. Uses `ModelSpec`/`FuncSpec` from `ace.sample` and `TARGET_PARAMS` from `utils`.

Skipped automatically when compiler toolchain is not available via `_provider_available()`.

### Test Data

```python
from ace.sample.ops.specs import LINEAR_OP, ADD_OP
from ace.sample.funcs.specs import ADD_FUNC

_OP_SPECS = [LINEAR_OP, ADD_OP]
_FUNC_SPECS = [ADD_FUNC]
```

### Helper Functions

```python
def _provider_available(name, device):
    """Check if a backend's compiler toolchain is available."""
    try:
        from ace.fhe.backend import get_library_impl
        return get_library_impl(name, device=device).check_available()
    except Exception:
        return False

def _export_onnx(spec, tmp_path):
    """Export a ModelSpec to ONNX file."""
    ...
```

### Test Classes

#### TestTorchCompile

Torch frontend compilation: FX trace → AIR file → backend build.

**Skip**: `@pytest.mark.skipif(not HAS_TORCH_FX or not HAS_FRONTEND, ...)`

| Test Case | Description |
|-----------|-------------|
| `test_air_path[spec-name-device]` | Torch → FX trace → AIR → backend build |

#### TestTorchViaOnnxCompile

Torch-via-ONNX frontend compilation paths. No class-level skip; uses `_provider_available()` in tests.

| Test Case | Description |
|-----------|-------------|
| `test_onnx_path[spec-name-device]` | Torch-via-ONNX → ONNX → backend build |
| `test_air_path[spec-name-device]` | Torch-via-ONNX → ONNX → AIR → backend build |

#### TestASTCompile

AST frontend compilation.

**Skip**: `@pytest.mark.skipif(not HAS_FRONTEND, ...)`

| Test Case | Description |
|-----------|-------------|
| `test_compile[spec-name-device]` | AST frontend → Driver.compile() |

#### TestOnnxCompile

ONNX frontend compilation paths. No class-level skip; uses `_provider_available()` in tests.

| Test Case | Description |
|-----------|-------------|
| `test_onnx_path[spec-name-device]` | ONNX file → ONNXFileIR → backend build |
| `test_air_path[spec-name-device]` | ONNX → AIR → backend build |

#### TestDriverErrors

Error handling tests. No skip, no parametrize.

| Test Case | Description |
|-----------|-------------|
| `test_invalid_frontend_raises` | Invalid frontend raises `ValueError` |
| `test_invalid_backend_raises` | Invalid backend raises `ValueError` |

---

## Conditional Skip Logic

### Provider Availability Check

```python
def _provider_available(name, device):
    try:
        from ace.fhe.backend import get_library_impl
        pro = get_library_impl(name, device=device)
        return pro.check_available()
    except Exception:
        return False
```

### Frontend Dependency Skip

```python
@pytest.mark.skipif(not HAS_TORCH_FX or not HAS_FRONTEND,
                    reason="torch.fx or frontend not available")
class TestTorchCompile:
    ...
```

### GPU Backend Skip

GPU backends use `TARGET_PARAMS` with conditional skip markers:

```python
TARGET_PARAMS = [
    pytest.param("antlib", "cpu", id="antlib-cpu"),
    pytest.param("phantom", "cuda",
                 marks=pytest.mark.skipif(not gpu_available(), reason="GPU not available"),
                 id="phantom-cuda"),
]
```

---

## Running Tests

```bash
# Run all driver tests
pytest tests/unit/driver/ -v

# Run registry tests only
pytest tests/unit/driver/test_registry.py -v

# Run pipeline tests only
pytest tests/unit/driver/test_pipeline.py -v

# Run specific frontend tests
pytest tests/unit/driver/test_pipeline.py -k "Torch" -v
```