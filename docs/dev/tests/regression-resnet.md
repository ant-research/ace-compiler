# Regression Tests — ResNet Models

## Overview

ResNet regression tests validate FHE compilation and inference for ResNet models on CIFAR-10 and CIFAR-100 datasets. These tests require the complete FHE stack (compiler + runtime).

## Test File Structure

```
tests/regression/resnet/
├── test_resnet_model.py      # PyTorch forward pass validation
├── test_resnet_compile.py    # Compilation pipeline + IR structure regression
├── test_resnet_smoke.py      # FHE compile + inference (antlib + phantom)
└── test_resnet_compile/      # IR YAML baselines (data_regression)
    ├── test_trace_resnet20_cifar10_.yml
    ├── test_bn_folded_resnet20_cifar10_.yml
    └── ...
```

---

## Module 1: Model Validation

**File**: `test_resnet_model.py`

PyTorch forward pass validation without FHE. Uses `resnet_case` fixture from `conftest.py`.

### Imports

```python
from utils import requires_torch
```

### Fixture

```python
# From tests/regression/conftest.py
@pytest.fixture(params=ALL_RESNET_SPECS, ids=lambda tc: tc.name)
def resnet_case(request):
    return request.param
```

### Test Class

#### TestResNetModel

**Skip**: `@requires_torch`

| Test Case | Description |
|-----------|-------------|
| `test_model_creation` | Model can be instantiated from spec |
| `test_model_forward_pass` | Forward pass produces valid output |
| `test_model_has_expected_ops` | Model contains expected operation types |
| `test_deterministic_output` | Same input produces same output |
| `test_batch_size_consistency` | Different batch sizes produce consistent shapes |

---

## Module 2: Compilation Pipeline

**File**: `test_resnet_compile.py`

Frontend trace, BN folding, and IR structure regression. Uses `data_regression` fixture for YAML baseline comparison.

### Imports

```python
from ace import fhe
from ace.fhe.frontend import get_frontend
from ace.fhe.ir import extract_ir_structure
from utils import requires_torch, TARGET_PARAMS
```

### Test Classes

#### TestResNetFrontendTorch

**Skip**: `@requires_torch`

| Test Case | Fixtures | Description |
|-----------|----------|-------------|
| `test_trace` | `resnet_case`, `data_regression` | Torch frontend trace → IR structure check |
| `test_bn_folded` | `resnet_case`, `data_regression` | BN folding applied, structure matches baseline |
| `test_compile` | `resnet_case` | Parametrized by `TARGET_PARAMS` — compiles to each backend |

#### TestResNetFrontendTorchViaOnnx

**Skip**: `@requires_torch`

| Test Case | Description |
|-----------|-------------|
| `test_to_air` | Torch-via-ONNX → AIR conversion |

#### TestResNetFrontendOnnx

**Skip**: `@requires_torch`

| Test Case | Fixtures | Description |
|-----------|----------|-------------|
| `test_to_air` | `resnet_case`, `tmp_path` | ONNX file → AIR conversion |

#### TestResNetBNFolding

**Skip**: `@requires_torch`

| Test Case | Description |
|-----------|-------------|
| `test_bn_folding` | BN layers are folded into conv weights |
| `test_bn_folding_output_equivalence` | Folding preserves output values |

#### TestResNetExport

**Skip**: `@requires_torch`

| Test Case | Fixtures | Description |
|-----------|----------|-------------|
| `test_export_air` | `resnet_case`, `tmp_path` | Export to AIR file |
| `test_export_onnx` | `resnet_case`, `tmp_path` | Export to ONNX file |

---

## Module 3: FHE Inference (Smoke Tests)

**File**: `test_resnet_smoke.py`

Full FHE compilation and inference with output validation against plaintext.

### Imports

```python
from ace import fhe
from ace.model.spec_resnet import ALL_RESNET_SPECS
from utils import requires_gpu
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

def _load_sample_image(spec):
    """Load bundled sample image for the spec's dataset."""
    data = np.load(_SAMPLE_DATA[spec.dataset])
    return torch.from_numpy(data["image"])  # [1, 3, 32, 32]

def _compile_resnet(spec, sample_input, library="antlib", device="cpu"):
    """Compile ResNet with pre-profiled VR data from ModelSpec."""
    ...

def _assert_fhe_matches_plaintext(compiled_model, spec, sample_input):
    """Assert FHE prediction matches plaintext prediction."""
    ...
```

### Test Functions

| Test | Markers | Description |
|------|---------|-------------|
| `test_smoke_antlib` | `@pytest.mark.slow` | ResNet FHE inference on antlib-cpu (slow) |
| `test_smoke_phantom` | `@requires_gpu`, `@pytest.mark.slow` | ResNet FHE inference on phantom-cuda |

**Parametrization**: Both tests use `@pytest.mark.parametrize("spec", ALL_RESNET_SPECS, ids=lambda s: s.name)`

**Phantom skip**: `test_smoke_phantom` internally calls `pytest.skip()` if `_phantom_available()` returns False.

---

## Data Files

```
tests/regression/data/
├── cifar10_sample.npz      # Sample image for CIFAR-10 models
└── cifar100_sample.npz     # Sample image for CIFAR-100 models
```

---

## Running Tests

```bash
# All resnet regression tests
pytest tests/regression/resnet/ -v

# Model tests only (fast, no FHE)
pytest tests/regression/resnet/test_resnet_model.py -v

# Compile tests (needs compiler)
pytest tests/regression/resnet/test_resnet_compile.py -v

# Smoke tests (needs full FHE stack, slow)
pytest tests/regression/resnet/test_resnet_smoke.py -v

# Skip slow tests
pytest tests/regression/resnet/ -m "not slow"

# Skip GPU tests
pytest tests/regression/resnet/ -k "not phantom"
```