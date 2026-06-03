# ANT-ACE Testing Guide

This document describes the test organization, usage scenarios, and best practices for the ANT-ACE FHE framework.

## Directory Structure

```
tests/
├── conftest.py                    # Root fixtures and pytest configuration
├── utils/                         # Shared test utilities
│   ├── __init__.py               # Public API re-exports
│   └── dependencies.py           # Dependency checks, skip markers, provider params
│
├── unit/                          # Unit tests — individual modules, no FHE runtime
│   ├── backend/
│   │   └── test_provider.py      # Backend provider properties and build commands
│   ├── config/
│   │   └── test_option.py        # FHEConfig, CompileOptions, ComputeOptions
│   ├── driver/
│   │   ├── test_pipeline.py      # Driver compilation pipeline paths
│   │   └── test_registry.py      # Frontend/backend registry
│   ├── frontend/
│   │   ├── conftest.py           # Frontend test fixtures
│   │   ├── test_ast.py           # AST frontend tests
│   │   ├── test_onnx.py          # ONNX frontend tests
│   │   ├── test_torch.py         # Torch frontend (FX trace) tests
│   │   └── test_torch_via_onnx.py # Torch-via-ONNX frontend tests
│   └── ir/
│       ├── conftest.py           # IR test fixtures
│       ├── test_export.py        # IR export (ONNX, AIR) tests
│       ├── test_format.py        # IR format (FHEProgram, FileIR) tests
│       └── test_structure.py     # IR data structure tests
│
├── integration/                   # Integration tests — top-level API, needs compiler
│   ├── test_fhe_compile.py       # fhe.compile() functional API
│   ├── test_fhe_compute.py       # fhe.compute() functional API
│   ├── test_fhe_export.py        # fhe.export() functional API
│   └── test_decorator.py         # @fhe.compile/@compute/@export decorator API
│
├── regression/                    # Regression tests — needs full FHE stack
│   ├── conftest.py               # Regression fixtures (resnet_case)
│   ├── data/                     # Test data (sample images, etc.)
│   ├── resnet/
│   │   ├── test_resnet_model.py  # ResNet PyTorch validation
│   │   ├── test_resnet_compile.py # ResNet compilation pipeline + IR regression
│   │   ├── test_resnet_smoke.py  # ResNet FHE inference (antlib + phantom)
│   │   └── test_resnet_compile/  # IR YAML baselines (data_regression)
│   └── sample/
│       ├── test_sample_model.py  # Sample ops/funcs PyTorch validation
│       ├── test_sample_compile.py # Sample compilation pipeline + IR regression
│       ├── test_smoke_antlib.py  # Sample FHE inference on antlib-cpu
│       ├── test_smoke_phantom.py # Sample FHE inference on phantom-cuda
│       └── test_sample_compile/  # IR YAML baselines (data_regression)
│
└── _disabled/                     # Disabled/legacy tests
```

## Test Categories

### 1. Unit Tests (`unit/`)

Test individual modules in isolation. No FHE runtime or compiler toolchain required.

| Directory | File | Description | Tests |
|-----------|------|-------------|-------|
| `backend/` | `test_provider.py` | Provider properties, build commands, error handling | ~20 |
| `config/` | `test_option.py` | FHEConfig, CompileOptions, ComputeOptions, BaseOption | ~15 |
| `driver/` | `test_registry.py` | Frontend/backend registry and lookup | ~20 |
| `driver/` | `test_pipeline.py` | Driver compilation pipeline (torch/ast/onnx → backend) | ~10 |
| `frontend/` | `test_ast.py` | AST frontend: prepare, compile, export | ~15 |
| `frontend/` | `test_onnx.py` | ONNX frontend: prepare, compile, export | ~10 |
| `frontend/` | `test_torch.py` | Torch frontend: FX trace, compile, export | ~25 |
| `frontend/` | `test_torch_via_onnx.py` | Torch-via-ONNX frontend: prepare, compile | ~10 |
| `ir/` | `test_structure.py` | IRNode, BasicBlock, FHEGraph, FHEProgram | ~30 |
| `ir/` | `test_format.py` | FileIR, ONNXFileIR, AIRFileIR, backward compat | ~15 |
| `ir/` | `test_export.py` | ONNX/AIR export, op type mapping | ~10 |

**Key patterns:**
- Use `PROVIDER_SPECS`, `CPU_PROVIDER`, `GPU_PROVIDER`, `TARGET_PARAMS` from `utils` for backend parametrization
- Use `_provider_available(name, device)` to skip when compiler toolchain is missing
- Use `@requires_torch`, `@requires_torch_fx`, `@skip_if_no_frontend`, `@skip_if_no_onnx` skip markers
- Use `ModelSpec`/`FuncSpec` from `ace.sample` for parametrized test inputs

### 2. Integration Tests (`integration/`)

Test top-level FHE APIs with real compiler toolchain. No FHE runtime required.

| File | Description | Tests |
|------|-------------|-------|
| `test_fhe_compile.py` | `fhe.compile()` functional API on models and functions | ~12 |
| `test_fhe_compute.py` | `fhe.compute()` functional API (compile + run) | ~4 |
| `test_fhe_export.py` | `fhe.export()` functional API (compile → file) | ~4 |
| `test_decorator.py` | `@fhe.compile/@compute/@export` decorator API | ~12 |

**Key patterns:**
- Test the public API (`fhe.compile`, `fhe.compute`, `fhe.export`) in functional form
- Decorator tests use inline model/function definitions (decorators bind at definition time)
- Use `ModelSpec`/`FuncSpec` from `ace.sample` for parametrized test inputs
- Skip with `_provider_available()` when backend toolchain is missing
- `@pytest.mark.skipif(not HAS_TORCH_FX or not HAS_FRONTEND, ...)` for torch-dependent tests
- `@pytest.mark.skipif(not HAS_FRONTEND, ...)` for AST-dependent tests

**Note:** Decorator + AST frontend is not compatible — `inspect.getsource()` resolves to `decorators.py` instead of the original function. Use `torch-via-onnx` frontend for decorator function tests.

**Note:** `fhe.compile()(ModelClass)` calls `ModelClass()` without args — only specs with no-arg constructors work (e.g. `ADD_OP`). Use `spec.create_model()` for specs that require constructor args (e.g. `LINEAR_OP`).

### 3. Regression Tests (`regression/`)

Test full FHE compilation and inference. Requires complete FHE stack.

| Subdirectory | File | Description |
|-------------|------|-------------|
| `resnet/` | `test_resnet_model.py` | ResNet PyTorch forward pass validation |
| `resnet/` | `test_resnet_compile.py` | ResNet frontend trace + IR structure regression (`data_regression`) |
| `resnet/` | `test_resnet_smoke.py` | ResNet FHE compile + inference (antlib-cpu, phantom-cuda) |
| `sample/` | `test_sample_model.py` | Ops/funcs PyTorch forward pass validation |
| `sample/` | `test_sample_compile.py` | Ops torch frontend trace + IR structure regression (`data_regression`) |
| `sample/` | `test_smoke_antlib.py` | Ops/funcs FHE compile + inference on antlib-cpu |
| `sample/` | `test_smoke_phantom.py` | Ops/funcs FHE compile + inference on phantom-cuda |

**Key patterns:**
- `test_sample_model.py` / `test_resnet_model.py` — PyTorch forward pass only, no FHE
- `test_sample_compile.py` / `test_resnet_compile.py` — Frontend trace + IR structure regression (`data_regression` fixture, baselines in `test_*_compile/` subdirectories)
- `test_smoke_antlib.py` / `test_smoke_phantom.py` — Full FHE compile + inference
- Phantom tests use `@requires_gpu` + `_phantom_available()` skip guard
- `BROKEN_COMPILE_OPS` / `BROKEN_SMOKE_OPS` exclusion lists track known failures per backend
- `@pytest.mark.slow` on resnet smoke tests (antlib inference is slow)

## Shared Test Utilities (`utils/`)

### Dependency Checks

```python
from utils import TORCH_AVAILABLE, HAS_TORCH_FX, HAS_FRONTEND, HAS_RUNTIME, FHE_AVAILABLE, ONNX_AVAILABLE
```

### Skip Markers

```python
from utils import requires_torch, requires_torch_fx, requires_gpu
from utils import skip_if_no_torch, skip_if_no_torch_fx, skip_if_no_frontend, skip_if_no_onnx, skip_if_no_fhe

@requires_torch       # Skip if torch not installed
@requires_torch_fx    # Skip if torch.fx not available
@requires_gpu         # Skip if torch.cuda.is_available() is False
@skip_if_no_frontend  # Skip if C++ extension not available
@skip_if_no_onnx      # Skip if onnx not installed
```

### Provider Parametrization

```python
from utils import TARGET_PARAMS, CPU_PROVIDER, GPU_PROVIDER, PROVIDER_SPECS, ALL_PROVIDER

# All providers with GPU auto-skip: antlib-cpu, phantom-cuda, acelib-cuda
@pytest.mark.parametrize("name,device", TARGET_PARAMS)
def test_something(name, device):
    if not _provider_available(name, device):
        pytest.skip(f"{name}/{device} compiler not available")
    ...

# CPU only: antlib-cpu, seal-cpu, openfhe-cpu
@pytest.mark.parametrize("name,device", CPU_PROVIDER, ...)

# GPU only: phantom-cuda, acelib-cuda
@pytest.mark.parametrize("name,device", GPU_PROVIDER, ...)
```

### Spec-Based Test Data

```python
from ace.sample.ops.specs import ALL_OPS_SPECS, LINEAR_OP, ADD_OP
from ace.sample.funcs.specs import ALL_FUNCS_SPECS, ADD_FUNC

# ModelSpec provides: name, model_class, create_model(), example_inputs, encrypt_inputs, expected_ops
@pytest.mark.parametrize("spec", ALL_OPS_SPECS, ids=lambda s: s.name)
def test_model(spec):
    model = spec.create_model()
    output = model(*spec.example_inputs)

# FuncSpec provides: name, func, example_inputs, encrypt_inputs, expected_ops
@pytest.mark.parametrize("spec", ALL_FUNCS_SPECS, ids=lambda s: s.name)
def test_function(spec):
    output = spec.func(*spec.example_inputs)
```

## CI Configuration

The CI pipeline (`.aci.yml`) has two test stages:

| Stage | Directories | Description |
|-------|------------|-------------|
| UNIT_TEST | `tests/unit/ tests/integration/` | Unit + integration (needs compiler, no FHE runtime) |
| E2E_TEST | `tests/regression/sample/` | Regression (needs full FHE stack) |

```bash
# CI unit + integration
pytest -v tests/unit/ tests/integration/ -k "not cuda" --junit-xml=report.xml

# CI regression
pytest -v tests/regression/sample/ -k "not cuda" --junit-xml=report.xml
```

## Running Tests

```bash
# Unit tests (fast, no FHE runtime)
pytest tests/unit/ -v

# Integration tests (needs compiler)
pytest tests/integration/ -v

# Regression tests (needs full FHE stack)
pytest tests/regression/ -v

# Specific layers
pytest tests/unit/backend/ -v
pytest tests/unit/frontend/ -v
pytest tests/unit/ir/ -v
pytest tests/unit/driver/ -v

# Skip GPU/phantom tests
pytest tests/ -k "not cuda"

# Skip slow regression tests
pytest tests/ -m "not slow"

# Specific test file
pytest tests/unit/frontend/test_torch.py -v

# With LD_LIBRARY_PATH (for runtime)
export LD_LIBRARY_PATH=/path/to/rtlib/lib:$LD_LIBRARY_PATH
pytest tests/regression/ -v
```

## Test Naming Conventions

| Layer | Pattern | Example |
|-------|---------|---------|
| Unit | `test_<module>.py` | `test_provider.py`, `test_option.py` |
| Integration | `test_fhe_<api>.py` | `test_fhe_compile.py`, `test_fhe_export.py` |
| Integration (decorator) | `test_decorator.py` | `test_decorator.py` |
| Regression | `test_<scope>_<type>.py` | `test_resnet_compile.py`, `test_sample_model.py` |

**Avoid** duplicate module names across directories — pytest uses module name for imports. Use prefixes like `test_resnet_`, `test_sample_`, `test_fhe_` to prevent conflicts.

## Related Documentation

### Unit Tests

- [Backend Unit Tests](unit-backend.md)
- [Config Unit Tests](unit-config.md) — FHEConfig, CompileOptions, ComputeOptions
- [Driver Unit Tests](unit-driver.md)
- [Frontend Unit Tests](unit-frontend.md)
- [IR Unit Tests](unit-ir.md)

### Integration Tests

- [Top-Level API Tests](integration-api.md) — `fhe.compile()`, `fhe.compute()`, `fhe.export()`, decorators

### Regression Tests

- [ResNet Tests](regression-resnet.md) — ResNet model FHE validation
- [Sample Tests](regression-sample.md) — Built-in ops/funcs FHE validation

### Other

- [Developer Guide](../develop.md)
- [Overall Design](../../design/overall.md)