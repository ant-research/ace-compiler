# Regression Tests — Sample Ops/Funcs

## Overview

Sample regression tests validate FHE compilation and inference for built-in operations and functions. These are the primary regression tests run in CI (E2E_TEST stage).

## Test File Structure

```
tests/regression/sample/
├── test_sample_model.py      # PyTorch forward pass validation
├── test_sample_compile.py    # Compilation pipeline + IR structure regression
├── test_smoke_antlib.py      # FHE compile + inference on antlib-cpu
├── test_smoke_phantom.py     # FHE compile + inference on phantom-cuda
└── test_sample_compile/      # IR YAML baselines (data_regression)
    ├── test_trace_add_op_.yml
    ├── test_trace_linear_op_.yml
    └── ... (30+ baseline files)
```

---

## Module 1: Model Validation

**File**: `test_sample_model.py`

PyTorch forward pass validation without FHE. Tests both ops (nn.Module) and funcs (Python functions).

### Imports

```python
from ace.sample.ops.specs import ALL_OPS_SPECS
from ace.sample.funcs.specs import ALL_FUNCS_SPECS
from utils import requires_torch
```

### Test Classes

#### TestOpModel

**Skip**: `@requires_torch`

| Test Case | Parametrize | Description |
|-----------|-------------|-------------|
| `test_model_creation` | `ALL_OPS_SPECS` | Model can be instantiated |
| `test_model_forward_pass` | `ALL_OPS_SPECS` | Forward pass produces valid output (no NaN/Inf) |
| `test_model_has_expected_ops` | `ALL_OPS_SPECS` | Model has expected_ops defined |
| `test_model_deterministic_output` | `ALL_OPS_SPECS` | Same input produces same output |

#### TestFuncModel

**Skip**: `@requires_torch`

| Test Case | Parametrize | Description |
|-----------|-------------|-------------|
| `test_function_execution` | `ALL_FUNCS_SPECS` | Function executes and returns valid tensor |
| `test_function_deterministic_output` | `ALL_FUNCS_SPECS` | Same input produces same output |
| `test_function_output_shape` | `ALL_FUNCS_SPECS` | Output shape is valid |
| `test_function_has_expected_ops` | `ALL_FUNCS_SPECS` | Function has expected_ops if defined |

---

## Module 2: Compilation Pipeline

**File**: `test_sample_compile.py`

Frontend trace and IR structure regression using `data_regression` fixture.

### Imports

```python
from ace.fhe.frontend import get_frontend
from ace.fhe.ir import extract_ir_structure
from ace.sample.ops.specs import ALL_OPS_SPECS
from ace.sample.funcs.specs import ALL_FUNCS_SPECS
from utils import requires_torch
```

### Test Classes

#### TestOpFrontendTorch

**Skip**: `@requires_torch`

| Test Case | Parametrize | Fixtures | Description |
|-----------|-------------|----------|-------------|
| `test_trace` | `ALL_OPS_SPECS` | `data_regression` | Torch frontend trace → IR structure → YAML baseline |

#### TestFuncFrontendAST

**Skip**: `@requires_torch`

| Test Case | Parametrize | Description |
|-----------|-------------|-------------|
| `test_trace` | `ALL_FUNCS_SPECS` | AST frontend trace (structure validation, no data_regression) |

---

## Module 3: FHE Inference — Antlib (CPU)

**File**: `test_smoke_antlib.py`

Full FHE compilation and inference on antlib-cpu backend.

### Imports

```python
from ace import fhe
from ace.sample.ops.specs import ALL_OPS_SPECS
from ace.sample.funcs.specs import ALL_FUNCS_SPECS
from utils import requires_torch
```

### Exclusion Lists

Known failing ops/funcs are excluded via module-level constants:

```python
# Ops that fail at FHE compile time
BROKEN_COMPILE_OPS = {
    "sub_op", "div_op", "sigmoid_op", "tanh_op", "sqrt_op", "softmax_op",
}

# Ops that compile OK but fail at FHE inference
BROKEN_SMOKE_OPS = {"conv2d_relu_op"}

# Funcs that compile OK but fail at FHE inference
BROKEN_SMOKE_FUNCS = {
    "abs_func", "neg_func", "square_func", "sqrt_func", "clamp_func",
    "log_func", "exp_func", "sigmoid_func", "tanh_func", "softmax_func",
    "conditional_relu_func", "loop_multiply_func", "loop_add_func",
    "nested_loop_func", "while_loop_func",
}

# Computed lists
COMPILE_OPS = [s for s in ALL_OPS_SPECS if s.name not in BROKEN_COMPILE_OPS]
SMOKE_OPS = [s for s in ALL_OPS_SPECS if s.name not in BROKEN_COMPILE_OPS and s.name not in BROKEN_SMOKE_OPS]
COMPILE_FUNCS = [s for s in ALL_FUNCS_SPECS if s.name not in BROKEN_COMPILE_FUNCS]
SMOKE_FUNCS = [s for s in ALL_FUNCS_SPECS if s.name not in BROKEN_COMPILE_FUNCS and s.name not in BROKEN_SMOKE_FUNCS]
```

### Helper Functions

```python
def _compile_op(spec, device="cpu"):
    """Compile an op model with built-in ReLU VR profiling."""
    model = spec.create_model()
    compiled_model = fhe.compile(
        frontend="torch",
        library="antlib",
        device=device,
        encrypt_inputs=spec.encrypt_inputs,
        profile_relu=True,
    )(model)
    return compiled_model.fhe_compile(spec.example_inputs)

def _compile_func(spec, device="cpu"):
    """Compile a function with built-in ReLU VR profiling."""
    compiled = fhe.compile(
        frontend="ast",
        library="antlib",
        device=device,
        encrypt_inputs=spec.encrypt_inputs,
        profile_relu=True,
    )(spec.func)
    return compiled.compile(spec.example_inputs)
```

### Test Functions

| Test | Parametrize | Description |
|------|-------------|-------------|
| `test_op_compile` | `COMPILE_OPS` | Op FHE compilation on antlib-cpu |
| `test_op_smoke` | `SMOKE_OPS` | Op FHE inference on antlib-cpu |
| `test_func_compile` | `COMPILE_FUNCS` | Function FHE compilation on antlib-cpu |
| `test_func_smoke` | `SMOKE_FUNCS` | Function FHE inference on antlib-cpu |

---

## Module 4: FHE Inference — Phantom (GPU)

**File**: `test_smoke_phantom.py`

Full FHE compilation and inference on phantom-cuda backend.

### Imports

```python
from ace import fhe
from ace.sample.ops.specs import ALL_OPS_SPECS
from ace.sample.funcs.specs import ALL_FUNCS_SPECS
from utils import requires_torch, requires_gpu, HAS_FRONTEND
```

### Exclusion Lists

More extensive exclusions due to phantom backend limitations:

```python
# Same compile-time failures as antlib
BROKEN_COMPILE_OPS = {
    "sub_op", "div_op", "sigmoid_op", "tanh_op", "sqrt_op", "softmax_op",
}

# Many more inference failures on phantom
BROKEN_SMOKE_OPS = {
    "conv2d_relu_op",
    "linear_op", "linear_relu_op", "relu_linear_op", "mlp_op",
    "flatten_op", "conv2d_op", "depthwise_conv2d_op",
    "avg_pool2d_op", "max_pool2d_op", "global_avg_pool_op",
    "relu_op", "gemm_49x3", "relu_gemm", "conv2d",
    "avg_pool_2d", "avg_pool_2d_with_stride", "global_avg_pool",
    "relu_avg_pool", "avg_pool_flatten",
}

# Funcs with inference failures
BROKEN_SMOKE_FUNCS = {
    "abs_func", "neg_func", "square_func", "sqrt_func", "clamp_func",
    "log_func", "exp_func", "sigmoid_func", "tanh_func", "softmax_func",
    "relu_func",  # passes on antlib, fails on phantom
    "conditional_relu_func", "loop_multiply_func", "loop_add_func",
    "nested_loop_func", "while_loop_func",
}
```

### Helper Functions

```python
def _phantom_available():
    """Check if phantom-cuda compiler is available."""
    try:
        from ace.fhe.backend import get_library_impl
        return get_library_impl("phantom", device="cuda").check_available()
    except Exception:
        return False

def _compile_op(spec, device="cuda"): ...
def _compile_func(spec, device="cuda"): ...
```

### Test Functions

| Test | Markers | Parametrize | Description |
|------|---------|-------------|-------------|
| `test_op_compile` | `@requires_torch`, `@requires_gpu` | `COMPILE_OPS` | Op FHE compilation on phantom-cuda |
| `test_op_smoke` | `@requires_torch`, `@requires_gpu` | `SMOKE_OPS` | Op FHE inference on phantom-cuda |
| `test_func_compile` | `@requires_torch`, `@requires_gpu` | `COMPILE_FUNCS` | Function FHE compilation on phantom-cuda |
| `test_func_smoke` | `@requires_torch`, `@requires_gpu` | `SMOKE_FUNCS` | Function FHE inference on phantom-cuda |

**Runtime skip**: All phantom tests call `pytest.skip()` if `_phantom_available()` returns False.

---

## Test Data Sources

| Spec Source | Description |
|-------------|-------------|
| `ace.sample.ops.specs.ALL_OPS_SPECS` | ~30 nn.Module specs (Linear, Conv, Pool, Activations) |
| `ace.sample.funcs.specs.ALL_FUNCS_SPECS` | ~30 function specs (Math, Control Flow, Loops) |

Each spec provides:
- `name`: Unique identifier
- `create_model()` / `func`: Model class or function
- `example_inputs`: Tuple of example tensors
- `encrypt_inputs`: List of input names to encrypt
- `expected_ops`: List of expected IR operations

---

## Running Tests

```bash
# All sample regression tests
pytest tests/regression/sample/ -v

# Model tests (fast, no FHE)
pytest tests/regression/sample/test_sample_model.py -v

# Compile tests (needs compiler)
pytest tests/regression/sample/test_sample_compile.py -v

# Smoke tests (needs full FHE stack)
pytest tests/regression/sample/test_smoke_antlib.py -v
pytest tests/regression/sample/test_smoke_phantom.py -v

# CI regression (E2E_TEST stage)
pytest tests/regression/sample/ -k "not cuda" -v

# Skip slow tests
pytest tests/regression/sample/ -m "not slow"
```