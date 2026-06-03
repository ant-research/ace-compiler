# Backend Module Unit Test Design

## Overview

This document describes the unit test design for the ACE FHE backend module. The backend module provides FHE execution engines that compile AIR IR to shared libraries for different hardware platforms.

## Test File Structure

```
tests/unit/backend/
└── test_provider.py     # Backend provider properties and build commands
```

## Registered Backends

| Backend | Device | Description |
|---------|--------|-------------|
| `antlib` | cpu | Ant Group FHE runtime library |
| `seal` | cpu | Microsoft SEAL integration |
| `openfhe` | cpu | OpenFHE library integration |
| `phantom` | cuda | GPU-accelerated FHE (CUDA) |
| `acelib` | cuda | High-performance GPU FHE (H100/sm_90) |

---

## Test Classes

### TestProviderProperties

Tests backend provider properties and options. Uses parametrized `ALL_PROVIDER` from `utils`.

| Test Case | Description |
|-----------|-------------|
| `test_properties[name-device]` | Provider has required properties (name, device, library) |
| `test_options[name-device]` | Provider compile options are accessible |

### TestBuildCommand

Tests backend build command generation. Uses `_BUILD_PROVIDERS` (providers where `implemented=True`).

| Test Case | Description |
|-----------|-------------|
| `test_build_command_basic[name-device]` | Basic build command structure |
| `test_build_command_with_flags[name-device]` | Build command with additional flags |
| `test_build_command_with_ace_root[name-device]` | Build command with ACE_ROOT override |

### TestCompileToLibErrors

Tests error handling for unsupported IR formats. Uses `_IR_VALIDATING_PROVIDERS` (excludes acelib and seal).

| Test Case | Description |
|-----------|-------------|
| `test_compile_to_lib_unsupported_ir[name-device]` | Unsupported IR format raises error |
| `test_openfhe_compile_to_lib_air_not_implemented` | OpenFHE AIR compilation not implemented |
| `test_acelib_compile_to_lib_not_implemented` | Acelib compilation not implemented |

---

## Provider Parametrization

Tests use centralized provider specs from `utils`:

```python
from utils import PROVIDER_SPECS, ALL_PROVIDER, CPU_PROVIDER, GPU_PROVIDER

# Module-level filtered params
_BUILD_PROVIDERS = [
    pytest.param(name, spec["device"], ...)
    for name, spec in PROVIDER_SPECS if spec["implemented"]
]

_IR_VALIDATING_PROVIDERS = [
    pytest.param(name, spec["device"], ...)
    for name, spec in PROVIDER_SPECS if name not in ("acelib", "seal")
]
```

---

## Running Tests

```bash
# Run all backend tests
pytest tests/unit/backend/ -v

# Run specific test class
pytest tests/unit/backend/test_provider.py::TestBuildCommand -v

# Run with coverage
pytest tests/unit/backend/ --cov=ace.fhe.backend --cov-report=html
```