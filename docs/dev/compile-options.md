# FHE Compiler Options Design and Usage Guide

> **See also:** [Compile Options API](../api/compile-options.md) — quick reference for compile option parameters.

## Overview

FHE compiler options (`vec`, `ckks`, `sihe`, `p2c`) control the compilation behavior of `fhe_cmplr`, such as polynomial modulus degree (N), max slots (ms), hardware parameters (hw), etc.

This document describes the design architecture, propagation paths, and usage patterns for compiler options.

## Option Types

| Option | Description | Common Parameters |
|--------|-------------|-------------------|
| `vec` | Vectorization options | `ms` (max_slots) |
| `ckks` | CKKS scheme parameters | `N` (poly_modulus_degree), `hw` (hamming_weight), `q0`, `sf` |
| `sihe` | SIHE options | `relu_vr_def`, `relu_vr`, `relu_mul_depth` |
| `p2c` | Poly2C options | `fp` (float_precision), `df` (data_file) |

## Architecture Design

### Option Propagation Path

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           User Call Layer                                   │
├─────────────────────────────────────────────────────────────────────────────┤
│  fhe.compile(encrypt_inputs=["x"], vec={"ms":256}, ckks={"N":4096})         │
│                              │                                               │
│                              ▼                                               │
│  decorators.py:                                                             │
│    options = CompileOptions(**kwargs)                     ← create options   │
│    backend_config = {"device": device, **compiler_options} ← pass options   │
└─────────────────────────────────────────────────────────────────────────────┘
                               │
                               ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                           Driver Layer                                      │
├─────────────────────────────────────────────────────────────────────────────┤
│  driver.py:                                                                 │
│    FHEDriverCompiler(frontend_name, backend_name, backend_config)           │
│    self.backend_impl = FHELibraryBuilder(backend_name, **backend_config)    │
└─────────────────────────────────────────────────────────────────────────────┘
                               │
                               ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                           Backend Layer                                     │
├─────────────────────────────────────────────────────────────────────────────┤
│  antlib.py:                                                                 │
│    def __init__(self, device, **kwargs):                                    │
│        self._options = kwargs  ← store compiler options                     │
│                                                                             │
│    def _compile_file(self, ir, output_dir):                                 │
│        option_args = _dict_to_cmd_args(self._resolved_options()) ← +defaults│
│        full_cmd = base_cmd + option_args                                    │
│        # fhe_cmplr input.onnx -VEC:ms=256 -CKKS:N=4096 ...                  │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Core Components

| Component | File | Responsibility |
|-----------|------|----------------|
| `CompileOptions` | `fhe_dsl/python/fhe/config/compile_options.py` | Define option fields, validate parameters |
| `_dict_to_cmd_args()` | `fhe_dsl/python/fhe/config/compile_options.py` | Convert dict to command-line arguments |
| `decorators.py` | `fhe_dsl/python/fhe/decorators.py` | Receive user options, pass to backend |
| `Backend` | `fhe_dsl/python/fhe/backend/antlib.py` | Store options, invoke compiler |

## Usage

### 1. fhe.compile / fhe.compute

Pass options directly when calling the API:

```python
import ace.fhe as fhe

# Using fhe.compute
compute_model = fhe.compute(
    frontend='torch',
    library='ant',
    device='cpu',
    encrypt_inputs=['x'],
    vec={'ms': 256},
    ckks={'N': 4096, 'hw': 192}
)(model)

result = compute_model(*inputs)

# Using fhe.compile
compiled = fhe.compile(
    frontend='torch',
    library='ant',
    encrypt_inputs=['x'],
    sihe={'relu_vr_def': 2, 'relu_mul_depth': 13}
)(model)

prog = compiled.compile(inputs)
```

### 2. Default Options for Test Cases

Configure default compile options for test cases. Three methods are supported:

#### Method 1: Set in test case definition (recommended for special models)

```python
# tests/unit/config/test_option.py
from ace.fhe.config import CompileOptions

MODEL_CONV_TEST_CASES = [
    ModelTestCase(
        name="conv2d",
        model_class=Conv2dModel,
        example_inputs=(torch.randn(1, 3, 16, 16),),
        encrypt_inputs=["x"],
        compile_options={
            "vec": {"ms": 256},
            "ckks": {"N": 4096}
        }
    ),
]
```

#### Method 2: Centralized config file (recommended for batch configuration)

```python
# tests/conftest.py or test config module

# Global default options
DEFAULT_COMPILE_OPTIONS = {
    # "vec": {"ms": 256},
    # "ckks": {"N": 8192},
}

# Model-specific options
MODEL_SPECIFIC_OPTIONS = {
    "conv2d": {
        "vec": {"ms": 256},
        "ckks": {"N": 4096},
    },
    "conv2d_relu": {
        "vec": {"ms": 256},
        "ckks": {"N": 4096},
    },
    "resnet20_cifar10": {
        "ckks": {"N": 8192, "hw": 192},
        "sihe": {"relu_vr_def": 2},
    },
}
```

#### Method 3: Environment variable override (for debugging)

```bash
# Set environment variables from the command line
ACE_COMPILE_OPTIONS='{"vec": {"ms": 512}, "ckks": {"N": 8192}}' \
    pytest -v tests/regression/ -k "conv2d"

# Or set in Python code
import os

# Set options
set_env_options({"vec": {"ms": 256}, "ckks": {"N": 4096}})

# Run tests...

# Clear options
clear_env_options()
```

### Option Priority

```
Environment variable ACE_COMPILE_OPTIONS  (highest priority)
              ↓
ModelTestCase.compile_options
              ↓
MODEL_SPECIFIC_OPTIONS[model_name]
              ↓
DEFAULT_COMPILE_OPTIONS  (lowest priority)
```

## Test Framework Integration

### Usage in Test Code

```python
# tests/regression/test_sample_compile.py
from ace.fhe.config import CompileOptions

def test_compute_success(self, model_case, model_inputs, backend, device):
    model = model_case.create_model()
    encrypt_inputs = model_case.encrypt_inputs

    # Get compiler options (auto-merge by priority)
    compile_opts = get_compile_options(model_case.name, model_case.compile_options)

    # Pass to fhe.compute
    compute_model = fhe.compute(
        frontend="torch",
        library=backend,
        device=device,
        encrypt_inputs=encrypt_inputs,
        **compile_opts
    )(model)

    result = compute_model(*model_inputs)
```

### Adding a New Model Test

1. Define test case in `tests/regression/`
2. If special options are needed, add to `default_options.py`:

```python
MODEL_SPECIFIC_OPTIONS = {
    # Existing options...
    "my_new_model": {
        "vec": {"ms": 256},
        "ckks": {"N": 4096},
    },
}
```

## Option Parameter Reference

### vec (Vectorization Options)

| Parameter | Type | Description |
|-----------|------|-------------|
| `ms` | int | Max slots (max_slots) |

### ckks (CKKS Scheme Options)

| Parameter | Type | Description |
|-----------|------|-------------|
| `N` | int | Polynomial modulus degree (poly_modulus_degree), common values: 512, 1024, 2048, 4096, 8192, 16384 |
| `hw` | int | Hamming weight, affects number of rotation keys |
| `q0` | int | Bit size of first prime |
| `sf` | int | Scaling factor bit size |
| `sbm` | bool | Enable sub-batch multiplication |

### sihe (SIHE Options)

| Parameter | Type | Description |
|-----------|------|-------------|
| `relu_vr_def` | int | Default ReLU value range |
| `relu_vr` | str | Per-operation ReLU value range, format: `/path/op=val;/path/op2=val2` |
| `relu_mul_depth` | int | ReLU polynomial multiplication depth |

### p2c (Poly2C Options)

| Parameter | Type | Description |
|-----------|------|-------------|
| `fp` | bool | Enable floating-point precision |
| `df` | str | Data file path |

## FAQ

### Q: How to determine the N value needed for a model?

A: The N value depends on:
1. Input data size
2. Model complexity
3. Precision requirements

Common configurations:
- Small models (conv2d, linear): N=4096
- Medium models (resnet20): N=8192
- Large models: N=16384

### Q: Getting "slot size is too small" error during compilation?

A: The N value is too small and needs to be increased. Try N=4096 or N=8192.

### Q: How to debug compiler options?

A: Use environment variables for quick testing:

```bash
ACE_COMPILE_OPTIONS='{"ckks": {"N": 8192}}' pytest -v tests/... -s
```

Compilation logs will show the actual command-line arguments used:
```
[SUCCESS] Compilation succeeded: fhe_cmplr ... -VEC:ms=256 -CKKS:N=8192
```

## Related Files

| File | Description |
|------|-------------|
| `fhe_dsl/python/fhe/config/compile_options.py` | CompileOptions definition and conversion functions |
| `fhe_dsl/python/fhe/decorators.py` | fhe.compile/compute decorators |
| `fhe_dsl/python/fhe/backend/antlib.py` | AntLIB backend implementation |
| `tests/test_cases/base.py` | ModelTestCase definition |
| `tests/test_cases/default_options.py` | Default options configuration |
