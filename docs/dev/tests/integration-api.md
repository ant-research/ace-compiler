# Integration Tests — Top-Level API

## Overview

Integration tests validate the user-facing top-level FHE APIs (`fhe.compile()`, `fhe.compute()`, `fhe.export()`) with real compiler toolchain. These tests require the C++ extension and compiler but do not require FHE runtime execution.

## Test File Structure

```
tests/integration/
├── test_fhe_compile.py       # fhe.compile() functional API
├── test_fhe_compute.py       # fhe.compute() functional API
├── test_fhe_export.py        # fhe.export() functional API
└── test_decorator.py         # @fhe.compile/@compute/@export decorator API
```

---

## Module 1: fhe.compile() API

**File**: `test_fhe_compile.py`

Tests the functional form: `fhe.compile(...)(target) → .fhe_compile(inputs) → CompiledProgram`

### Imports and Data

```python
from ace import fhe
from ace.sample.ops.specs import LINEAR_OP, ADD_OP
from ace.sample.funcs.specs import ADD_FUNC
from utils import HAS_TORCH_FX, HAS_FRONTEND, TARGET_PARAMS

_OP_SPECS = [LINEAR_OP, ADD_OP]
_FUNC_SPECS = [ADD_FUNC]
_NO_ARG_MODEL_SPECS = [ADD_OP]  # Only specs with no-arg constructors
```

### Helper Function

```python
def _provider_available(name, device):
    """Check if a backend's compiler toolchain is available."""
    try:
        from ace.fhe.backend import get_library_impl
        return get_library_impl(name, device=device).check_available()
    except Exception:
        return False
```

### Test Classes

#### TestCompileModel

**Skip**: `@pytest.mark.skipif(not HAS_TORCH_FX or not HAS_FRONTEND, ...)`

| Test Case | Parametrize | Description |
|-----------|-------------|-------------|
| `test_compile_model_class` | `TARGET_PARAMS` × `_NO_ARG_MODEL_SPECS` | `fhe.compile()(ModelClass)` attaches `fhe_compile` and `compile` |
| `test_compile_model_instance` | `TARGET_PARAMS` | `fhe.compile()(model_instance)` attaches `fhe_compile` |

**Note**: `test_compile_model_class` uses `_NO_ARG_MODEL_SPECS` because `fhe.compile()(ModelClass)` calls `ModelClass()` without arguments. Only specs like `ADD_OP` work; `LINEAR_OP` requires constructor args.

#### TestCompileFunction

**Skip**: `@pytest.mark.skipif(not HAS_FRONTEND, ...)`

| Test Case | Parametrize | Description |
|-----------|-------------|-------------|
| `test_compile_function` | `TARGET_PARAMS` × `_FUNC_SPECS` | `fhe.compile()(func)` attaches `compile` and `_fhe_compiler` |

#### TestCompileTorchViaOnnx

**Skip**: None (uses `_provider_available()` in test body)

| Test Case | Parametrize | Description |
|-----------|-------------|-------------|
| `test_compile_model` | `TARGET_PARAMS` × `_OP_SPECS` | `fhe.compile` with `frontend="torch-via-onnx"` |

#### TestCompileErrors

**Skip**: None

| Test Case | Description |
|-----------|-------------|
| `test_invalid_frontend_raises` | `ValueError: Unknown frontend` |
| `test_invalid_library_raises` | `ValueError: Unknown library` |

---

## Module 2: fhe.compute() API

**File**: `test_fhe_compute.py`

Tests the functional form: `fhe.compute(...)(target) → call with inputs → FHE result`

### Imports and Data

```python
from ace import fhe
from ace.sample.ops.specs import ADD_OP
from ace.sample.funcs.specs import ADD_FUNC
from utils import HAS_TORCH_FX, HAS_FRONTEND, TARGET_PARAMS
```

### Test Classes

#### TestComputeModel

**Skip**: `@pytest.mark.skipif(not HAS_TORCH_FX or not HAS_FRONTEND, ...)`

| Test Case | Parametrize | Description |
|-----------|-------------|-------------|
| `test_compute_model` | `TARGET_PARAMS` | `fhe.compute()(model)` runs FHE inference and returns result |

#### TestComputeFunction

**Skip**: `@pytest.mark.skipif(not HAS_FRONTEND, ...)`

| Test Case | Parametrize | Description |
|-----------|-------------|-------------|
| `test_compute_function` | `TARGET_PARAMS` | `fhe.compute()(func)` runs FHE inference and returns result |

---

## Module 3: fhe.export() API

**File**: `test_fhe_export.py`

Tests the functional form: `fhe.export(...)(target) → .export(inputs) → IR file`

### Imports and Data

```python
from ace import fhe
from ace.sample.ops.specs import LINEAR_OP
from ace.sample.funcs.specs import ADD_FUNC
from utils import HAS_TORCH_FX, HAS_FRONTEND, TARGET_PARAMS
```

### Test Classes

#### TestExportModel

**Skip**: `@pytest.mark.skipif(not HAS_TORCH_FX or not HAS_FRONTEND, ...)`

| Test Case | Parametrize | Description |
|-----------|-------------|-------------|
| `test_export_air` | `TARGET_PARAMS` | `fhe.export()(model)` with `format='air'` produces AIR file |

#### TestExportFunction

**Skip**: `@pytest.mark.skipif(not HAS_FRONTEND, ...)`

| Test Case | Parametrize | Description |
|-----------|-------------|-------------|
| `test_export_function_air` | `TARGET_PARAMS` | `fhe.export()(func)` with `format='air'` produces AIR file |

---

## Module 4: Decorator API

**File**: `test_decorator.py`

Tests the decorator form: `@fhe.compile`, `@fhe.compute`, `@fhe.export`

### Key Limitation

**Decorator + AST frontend is not compatible.** `@fhe.compile`/`@fhe.compute`/`@fhe.export` wrap functions in `FunctionWrapper(nn.Module)`. AST frontend uses `inspect.getsource(func)` which resolves to `decorators.py` instead of the original function, causing `IndentationError`.

**Solution**: Decorator function tests use `torch-via-onnx` frontend instead of `ast`.

### Imports

```python
from ace import fhe
from utils import HAS_TORCH_FX, HAS_FRONTEND, TARGET_PARAMS
```

### Test Classes

#### TestCompileDecorator

**Skip**: `@pytest.mark.skipif(not HAS_TORCH_FX or not HAS_FRONTEND, ...)`

| Test Case | Parametrize | Description |
|-----------|-------------|-------------|
| `test_compile_model_class` | `TARGET_PARAMS` | `@fhe.compile` on nn.Module class attaches `compile`/`fhe_compile` |
| `test_compile_function` | `TARGET_PARAMS` | `@fhe.compile` on function with `torch-via-onnx` frontend |

#### TestComputeDecorator

**Skip**: `@pytest.mark.skipif(not HAS_TORCH_FX or not HAS_FRONTEND, ...)`

| Test Case | Parametrize | Description |
|-----------|-------------|-------------|
| `test_compute_model` | `TARGET_PARAMS` | `@fhe.compute` on nn.Module: calling with inputs runs FHE inference |
| `test_compute_function` | `TARGET_PARAMS` | `@fhe.compute` on function with `torch-via-onnx` frontend |

#### TestExportDecorator

**Skip**: `@pytest.mark.skipif(not HAS_TORCH_FX or not HAS_FRONTEND, ...)`

| Test Case | Parametrize | Description |
|-----------|-------------|-------------|
| `test_export_model_air` | `TARGET_PARAMS` | `@fhe.export` on nn.Module: `.export(inputs)` writes AIR file |
| `test_export_function_air` | `TARGET_PARAMS` | `@fhe.export` on function with `torch-via-onnx` frontend |

---

## Running Tests

```bash
# Run all integration tests
pytest tests/integration/ -v

# Run specific API
pytest tests/integration/test_fhe_compile.py -v
pytest tests/integration/test_fhe_compute.py -v
pytest tests/integration/test_fhe_export.py -v
pytest tests/integration/test_decorator.py -v

# Skip GPU tests
pytest tests/integration/ -k "not cuda"
```