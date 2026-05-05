# ANT-ACE Testing Guide

This document describes the test organization, usage scenarios, and best practices for the ANT-ACE FHE framework.

## Directory Structure

```
tests/
├── conftest.py                    # Shared fixtures and configuration
│
├── test_cases/                    # Test case definitions (shared data)
│   ├── __init__.py
│   ├── base.py                   # TestCase, ModelTestCase, FunctionTestCase
│   ├── input_utils.py            # Input generation utilities
│   ├── models/                   # Model test cases
│   │   ├── __init__.py
│   │   ├── ops.py               # Basic operations (add, mul, relu, etc.)
│   │   ├── conv.py              # Convolutional models
│   │   ├── gemm.py              # Linear/GEMM models
│   │   └── pool.py              # Pooling models
│   └── functions/                # Function test cases
│       ├── __init__.py
│       ├── basic.py             # Basic math operations
│       └── control_flow.py      # Functions with loops/conditionals
│
├── test_data/                     # Test data files
│   ├── golden/                   # Golden outputs for regression
│   │   ├── models/
│   │   └── functions/
│   ├── onnx/                     # ONNX model files for testing
│   └── inputs/                   # Pre-generated test inputs
│
├── test_unit/                    # Unit tests - test individual modules
│   ├── conftest.py              # Unit test fixtures
│   ├── test_config/             # Configuration options tests
│   ├── test_driver/             # Driver/compiler tests
│   ├── test_frontend/           # Frontend component tests
│   │   ├── conftest.py          # Frontend test fixtures
│   │   └── test_torch_frontend.py
│   ├── test_ir/                 # IR component tests
│   └── test_backend/            # Backend component tests
│
├── test_integration/             # Integration tests - test component combinations
│   ├── test_frontend_compiler/  # Frontend + Compiler
│   ├── test_compiler_runtime/   # Compiler + Runtime
│   └── test_torch_driver_pipeline.py  # Full pipeline test
│
├── test_decorators/              # Decorator and high-level API tests
│   ├── test_decorator_compile.py
│   └── test_decorator_compute.py
│
├── test_regression/              # Regression tests - deterministic inputs
│   ├── __init__.py
│   ├── conftest.py              # Regression test fixtures
│   ├── test_models.py           # Model regression tests
│   └── test_functions.py        # Function regression tests
│
└── test_coverage/                # Coverage tests - random inputs
    ├── __init__.py
    ├── conftest.py              # Coverage test fixtures
    ├── test_models.py           # Model coverage tests
    └── test_functions.py        # Function coverage tests
```

## Test Categories

### 1. Unit Tests (`test_unit/`)

Test individual modules in isolation without external dependencies.

| Directory | Description |
|-----------|-------------|
| `test_config/` | Test CompileOptions, ComputeOptions, and other config classes |
| `test_driver/` | Test Driver, registry, builder |
| `test_frontend/` | Test individual frontend implementations |
| `test_ir/` | Test IR components (FHEProgram, FileIR, serialization) |
| `test_backend/` | Test backend implementations |

**Usage Scenarios**:

| Scenario | Usage |
|----------|-------|
| Developer | Run during development to verify individual components |
| CI | Run on every PR to catch regressions early |
| Nightly | Run with extended coverage options |
| Release | Required gate before release |

### 2. Integration Tests (`test_integration/`)

Test combinations of components working together.

| Directory | Description |
|-----------|-------------|
| `test_frontend_compiler/` | Test that frontends correctly produce IR for the compiler |
| `test_compiler_runtime/` | Test compilation and runtime execution together |
| `test_torch_driver_pipeline.py` | Test the full pipeline from frontend to runtime |

**Usage Scenarios**:

| Scenario | Usage |
|----------|-------|
| Developer | Run before merging feature branches |
| CI | Run on every PR after unit tests pass |
| Nightly | Run with all backend combinations |
| Release | Required gate, must pass on all supported platforms |

### 3. Decorator Tests (`test_decorators/`)

Test high-level user-facing APIs and decorators.

| File | Description |
|------|-------------|
| `test_decorator_compile.py` | Test @fhe.compile decorator |
| `test_decorator_compute.py` | Test @fhe.compute decorator |

**Usage Scenarios**:

| Scenario | Usage |
|----------|-------|
| Developer | Run when modifying decorator or API code |
| CI | Run on every PR |
| Nightly | Run with different Python/PyTorch versions |
| Release | Required gate for API stability |

### 4. Regression Tests (`test_regression/`)

Test with deterministic inputs to ensure reproducible results.

Uses fixed input modes:
- **ONES**: All values set to 1.0
- **NEG_ONES**: All values set to -1.0
- **ARANGE**: Incremental values normalized to [0, 1)

**Usage Scenarios**:

| Scenario | Usage |
|----------|-------|
| Developer | Run when fixing bugs or modifying core algorithms |
| CI | Run on every PR for regression detection |
| Nightly | Run with golden output comparison |
| Release | Required gate, compare against golden outputs |

### 5. Coverage Tests (`test_coverage/`)

Test with random inputs to explore more scenarios and improve coverage.

**Usage Scenarios**:

| Scenario | Usage |
|----------|-------|
| Developer | Optional, run before major releases |
| CI | Not required (non-deterministic) |
| Nightly | Run with multiple random seeds |
| Release | Optional, but recommended for coverage metrics |

## Test Coverage Matrix

### Frontend Coverage

```
┌─────────────────────┬─────────────┬─────────────┬─────────────┐
│ Frontend            │ memory      │ file onnx   │ file air    │
├─────────────────────┼─────────────┼─────────────┼─────────────┤
│ torch               │     ✓       │     ✓       │     ✓       │
│ torch-via-onnx      │     ✓       │     ✓       │     ✓       │
│ ast                 │     ✓       │     -       │     ✓       │
│ ast-via-onnx        │     ✓       │     ✓       │     ✓       │
│ onnx                │     ✓       │     ✓       │     ✓       │
└─────────────────────┴─────────────┴─────────────┴─────────────┘
```

### Backend Coverage

```
┌─────────────────────┬─────────────┬─────────────┬─────────────┐
│ Backend             │ CPU         │ GPU         │ CI Default  │
├─────────────────────┼─────────────┼─────────────┼─────────────┤
│ antlib              │     ✓       │     -       │     ✓       │
│ phantom             │     -       │     ✓       │     -       │
│ hyperfhe            │     -       │     ✓       │     -       │
│ seal                │     ✓       │     -       │     -       │
│ openfhe             │     ✓       │     -       │     ✓       │
└─────────────────────┴─────────────┴─────────────┴─────────────┘
```

## Running Tests

### Prerequisites

```bash
# Set library path for runtime
export LD_LIBRARY_PATH=/root/.pyenv/lib/python3.12/site-packages/rtlib/lib:$LD_LIBRARY_PATH
```

### By Test Category

```bash
# Unit Tests - Developer/CI/Nightly/Release
pytest tests/test_unit/ -v

# Integration Tests - Developer/CI/Nightly/Release
pytest tests/test_integration/ -v

# Decorator Tests - Developer/CI/Nightly/Release
pytest tests/test_decorators/ -v

# Regression Tests - Developer/CI/Nightly/Release
pytest tests/test_regression/ -v

# Coverage Tests - Nightly/Release (optional for Developer)
pytest tests/test_coverage/ -v
```

### By Module

```bash
# Frontend tests
pytest tests/test_unit/test_frontend/ -v

# Driver/compiler tests
pytest tests/test_unit/test_driver/ -v

# Backend tests
pytest tests/test_unit/test_backend/ -v

# IR tests
pytest tests/test_unit/test_ir/ -v
```

### By Scenario

```bash
# Developer workflow - quick feedback
pytest tests/test_unit/ tests/test_decorators/ -v

# CI workflow - comprehensive but fast
pytest tests/test_unit/ tests/test_integration/ tests/test_decorators/ tests/test_regression/

# Nightly workflow - exhaustive testing
pytest tests/ --cov=ace.fhe --cov-report=html

# Release workflow - full validation
pytest tests/ -v --cov=ace.fhe --cov-fail-under=80
```

### Common Options

```bash
# Run all tests
pytest tests/

# Run specific test file
pytest tests/test_unit/test_frontend/test_torch_frontend.py

# Run with verbose output
pytest tests/test_unit/ -v

# Run with coverage
pytest tests/ --cov=ace.fhe

# Run only fast tests (skip slow markers)
pytest tests/ -m "not slow"

# Run GPU tests only
pytest tests/ -m gpu

# Run specific test
pytest tests/test_decorators/test_decorator_compile.py::test_compile_add_cc -v -s
```

## Test Naming Conventions

- Unit tests: `test_<module>_<functionality>.py`
- Integration tests: `test_<component1>_<component2>.py`
- Regression tests: `test_models.py`, `test_functions.py`
- Coverage tests: `test_models.py`, `test_functions.py`

## Test Markers

| Marker | Description | Usage Scenario |
|--------|-------------|----------------|
| `@pytest.mark.gpu` | Tests requiring GPU | Nightly, Release (GPU platforms) |
| `@pytest.mark.slow` | Slow running tests (>30s) | Nightly, Release |
| `@pytest.mark.integration` | Integration tests | CI, Nightly, Release |
| `@pytest.mark.e2e` | End-to-end tests | Nightly, Release |
| `@pytest.mark.backend_antlib` | Antlib backend tests | CI, Nightly, Release |
| `@pytest.mark.backend_phantom` | Phantom backend tests | Nightly, Release (GPU) |
| `@pytest.mark.backend_hyperfhe` | HyperFHE backend tests | Nightly, Release (H100) |

```python
import pytest

@pytest.mark.gpu
@pytest.mark.slow
def test_large_model_inference():
    """Test large model inference on GPU - Nightly only."""
    pass

@pytest.mark.backend_phantom
def test_phantom_backend_compilation():
    """Test Phantom backend compilation - requires GPU."""
    pass
```

## Test Fixtures

### Shared Fixtures (`conftest.py`)

```python
# tests/conftest.py
import pytest
import torch
from test_cases.input_utils import InputMode, generate_inputs_by_mode

@pytest.fixture
def example_input():
    """Provide a standard example input tensor."""
    return torch.randn(1, 3, 32, 32)

@pytest.fixture
def deterministic_input():
    """Provide deterministic input for regression tests."""
    torch.manual_seed(42)
    return torch.randn(1, 3, 32, 32)

@pytest.fixture(params=[InputMode.ONES, InputMode.NEG_ONES, InputMode.ARANGE])
def input_by_mode(request, example_input):
    """Parametrized fixture for different input modes."""
    return generate_inputs_by_mode(example_input, request.param)

@pytest.fixture
def temp_output_dir(tmp_path):
    """Provide temporary directory for output files."""
    return tmp_path / "output"

@pytest.fixture
def mock_backend():
    """Mock backend for unit testing without actual FHE libraries."""
    from unittest.mock import MagicMock
    backend = MagicMock()
    backend.check_available.return_value = True
    return backend
```

### Frontend Fixtures

```python
# tests/test_unit/test_frontend/conftest.py
import pytest
import torch

@pytest.fixture
def simple_add_model():
    """Simple addition model for testing."""
    class AddModel(torch.nn.Module):
        def forward(self, x, y):
            return x + y
    return AddModel()

@pytest.fixture
def simple_conv_model():
    """Simple convolutional model for testing."""
    class ConvModel(torch.nn.Module):
        def __init__(self):
            super().__init__()
            self.conv = torch.nn.Conv2d(3, 16, 3)
        def forward(self, x):
            return self.conv(x)
    return ConvModel()
```

### Backend Fixtures

```python
# tests/conftest.py
import pytest

@pytest.fixture
def antlib_backend():
    """Provide Antlib backend instance."""
    from ace.fhe.backend import get_backend_strategy
    backend = get_backend_strategy("antlib", {"device": "cpu"})
    if not backend.check_available():
        pytest.skip("Antlib backend not available")
    return backend

@pytest.fixture
def phantom_backend():
    """Provide Phantom backend instance (requires GPU)."""
    from ace.fhe.backend import get_backend_strategy
    backend = get_backend_strategy("phantom", {"device": "cuda"})
    if not backend.check_available():
        pytest.skip("Phantom backend not available (requires CUDA)")
    return backend
```

## Input Generation Utilities

Located in `tests/test_cases/input_utils.py`:

```python
from test_cases.input_utils import InputMode, generate_inputs_by_mode

# Available modes:
InputMode.ONES       # All ones
InputMode.NEG_ONES   # All negative ones
InputMode.ARANGE     # Incremental values
InputMode.RANDOM     # Random values (seeded)

# Generate inputs:
inputs = generate_inputs_by_mode(example_inputs, InputMode.ONES)
```

## Adding New Test Cases

### Adding a Model Test Case

1. Define the model class in `tests/test_cases/models/`
2. Create a `ModelTestCase` in the appropriate file:

```python
from test_cases.base import ModelTestCase

MODEL_NEW_TEST_CASES = [
    ModelTestCase(
        name="new_model",
        model_class=NewModel,
        example_inputs=(torch.randn(1, 3, 16, 16),),
        encrypt_inputs=["x"],
        expected_ops=["Conv"]
    ),
]
```

3. Export in `tests/test_cases/models/__init__.py`

### Adding a Function Test Case

1. Define the function in `tests/test_cases/functions/`
2. Create a `FunctionTestCase`:

```python
from test_cases.base import FunctionTestCase

FUNCTION_NEW_TEST_CASES = [
    FunctionTestCase("new_func", new_func, (torch.randn(1, 10),)),
]
```

3. Export in `tests/test_cases/functions/__init__.py`

## CI Configuration

### GitHub Actions Example

```yaml
# .github/workflows/test.yml
name: Tests

on:
  pull_request:
    branches: [main, master]
  push:
    branches: [main, master]

jobs:
  unit-and-integration:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Install dependencies
        run: pip install -e .[dev]
      - name: Run Unit Tests
        run: pytest tests/test_unit/ -v
      - name: Run Integration Tests
        run: pytest tests/test_integration/ -v
      - name: Run Decorator Tests
        run: pytest tests/test_decorators/ -v
      - name: Run Regression Tests
        run: pytest tests/test_regression/ -v

  nightly:
    runs-on: [self-hosted, gpu]
    if: github.event_name == 'schedule'
    steps:
      - name: Run Full Test Suite
        run: pytest tests/ --cov=ace.fhe --cov-report=xml
      - name: Run Coverage Tests
        run: pytest tests/test_coverage/ -v
      - name: Run GPU Tests
        run: pytest tests/ -m gpu -v

  release:
    runs-on: [self-hosted, gpu]
    if: startsWith(github.ref, 'refs/tags/v')
    steps:
      - name: Full Validation
        run: pytest tests/ -v --cov=ace.fhe --cov-fail-under=80
      - name: Golden Output Comparison
        run: pytest tests/test_regression/ --compare-golden
```

### Test Stage Summary

| Stage | Tests | Environment | Trigger |
|-------|-------|-------------|---------|
| PR Check | Unit + Integration + Decorator + Regression | CPU | Every PR |
| Merge Check | Unit + Integration + Decorator + Regression | CPU | Merge to main |
| Nightly | All tests + Coverage + GPU | CPU + GPU | Scheduled (daily) |
| Release | All tests + Golden comparison + Coverage gate | CPU + GPU | Version tag |

## Test Data Management

### Golden Outputs

Golden outputs are stored in `tests/test_data/golden/`:

```
tests/test_data/
├── golden/
│   ├── models/
│   │   ├── add_model_v1.json
│   │   └── conv_model_v1.json
│   └── functions/
│       └── basic_ops_v1.json
├── onnx/
│   └── test_models/
└── inputs/
    └── regression/
```

### Golden Output Format

```json
{
    "test_case": "add_model",
    "version": "1.0",
    "input_mode": "ONES",
    "input_shapes": [[1, 3, 32, 32], [1, 3, 32, 32]],
    "output_shape": [1, 3, 32, 32],
    "output_checksum": "sha256:abc123...",
    "created_at": "2024-01-15T10:30:00Z"
}
```

## Mocking Strategies

### Mocking FHE Backends

```python
from unittest.mock import MagicMock, patch

def test_compilation_without_backend():
    """Test compilation logic without actual FHE backend."""
    with patch('ace.fhe.backend.AntLIB') as mock_backend:
        mock_backend.return_value.check_available.return_value = True
        mock_backend.return_value.build_command.return_value = ["g++", "..."]

        # Test compilation logic
        from ace.fhe.driver import Driver
        compiler = Driver(frontend_name="torch-via-onnx", backend_name="antlib")
        # ... test logic
```

### Mocking PyTorch Models

```python
def test_frontend_with_mock_model():
    """Test frontend with a mock model."""
    mock_model = MagicMock()
    mock_model.return_value = torch.randn(1, 10)

    from ace.fhe.frontend import get_frontend
    frontend = get_frontend("torch")
    # ... test with mock_model
```

## Testing Best Practices

### General Guidelines

1. **Test Isolation**: Each test should be independent and not rely on other tests
2. **Deterministic**: Use fixed seeds for random operations in regression tests
3. **Clear Names**: Test names should describe what is being tested
4. **Single Responsibility**: Each test should verify one behavior
5. **Fast Feedback**: Unit tests should complete in milliseconds

### Test Naming Convention

```python
# Good: descriptive and specific
def test_torch_via_onnx_converts_conv2d_model_correctly():
    pass

def test_compile_decorator_raises_error_for_invalid_backend():
    pass

# Bad: vague and unclear
def test_model():
    pass

def test_error():
    pass
```

### Test Structure (AAA Pattern)

```python
def test_frontend_produces_valid_ir():
    # Arrange - set up test data
    model = SimpleAddModel()
    inputs = [torch.randn(1, 3, 32, 32)]
    frontend = get_frontend("torch")

    # Act - execute the behavior
    ir = frontend.compile(model, inputs, ["x"])

    # Assert - verify the result
    assert ir is not None
    assert ir.format_type in ["memory", "file"]
```

### Skipping Tests Appropriately

```python
import pytest

@pytest.mark.skipif(not torch.cuda.is_available(), reason="CUDA not available")
def test_phantom_backend_gpu_operations():
    pass

@pytest.mark.skip(reason="Feature not yet implemented")
def test_future_feature():
    pass
```

### Parametrized Tests

```python
@pytest.mark.parametrize("input_shape", [
    (1, 3, 32, 32),
    (1, 1, 64, 64),
    (4, 3, 224, 224),
])
def test_model_with_different_input_shapes(input_shape):
    """Test model handles various input shapes."""
    model = ConvModel()
    inputs = [torch.randn(*input_shape)]
    # ... test logic
```

## Troubleshooting

### Common Issues

| Issue | Cause | Solution |
|-------|-------|----------|
| Import errors | Missing dependencies | Run `pip install -e .[dev]` |
| CUDA out of memory | GPU memory exhausted | Use smaller batch sizes or `pytest -m "not gpu"` |
| Slow tests | Running all tests | Use specific test categories |
| Flaky tests | Non-deterministic code | Check random seeds, mock external dependencies |
| `libFHErt_common.so not found` | Missing LD_LIBRARY_PATH | `export LD_LIBRARY_PATH=/path/to/rtlib/lib:$LD_LIBRARY_PATH` |

### Debug Mode

```bash
# Run with detailed output
pytest tests/test_unit/ -v -s --tb=long

# Run single test with debugger
pytest tests/test_unit/test_driver/test_compilation_paths.py::test_torch_via_onnx_path -s --pdb

# Show print statements
pytest tests/ -s
```

### Test Discovery Issues

```bash
# Force rediscovery of tests
pytest --collect-only tests/

# Check which tests will run
pytest tests/test_unit/ --collect-only
```

## Related Documentation

- [Developer Guide](develop.md) - Build and development workflow
- [Package Management](package.md) - Package structure and dependencies
- [Release Process](release.md) - Versioning and distribution
- [Overall Design](../design/overall.md) - Architecture overview