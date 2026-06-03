# Config Module Unit Test Design

## Overview

This document describes the unit test design for the FHE configuration options module. The config module provides dataclasses for compile-time and runtime options.

## Test File Structure

```
tests/unit/config/
└── test_option.py     # FHEConfig, CompileOptions, ComputeOptions, BaseOption tests
```

---

## Module: Configuration Options

**File**: `test_option.py`

Tests for configuration dataclasses. No external dependencies — pure Python tests.

### Test Classes

#### TestFHEConfig

Tests for `FHEConfig` defaults and customization.

| Test Case | Description |
|-----------|-------------|
| `test_defaults` | Default values: scheme="CKKS", poly_modulus_degree=8192, multiplication_depth=2, backend="CPU" |
| `test_custom` | Custom values can be set via constructor |

#### TestCompileOptions

Tests for `CompileOptions` — compile-time configuration.

| Test Case | Description |
|-----------|-------------|
| `test_defaults` | All fields default to None/False, config has defaults |
| `test_encrypt_inputs_by_name` | encrypt_inputs accepts list of strings |
| `test_encrypt_inputs_by_index` | encrypt_inputs accepts list of integers |
| `test_custom_config` | Custom FHEConfig can be passed |
| `test_compiler_options` | vec, ckks, sihe, p2c, o2a options |
| `test_relu_options` | relu_vr_data, relu_vr_file, profile_relu options |

**CompileOptions fields tested:**
- `encrypt_inputs` — List of input names or indices to encrypt
- `config` — FHEConfig instance
- `vec`, `ckks`, `sihe`, `p2c`, `o2a` — Compiler-specific options
- `fhe_scheme`, `poly` — Scheme options
- `relu_vr_data`, `relu_vr_file`, `profile_relu` — ReLU VR profiling options

#### TestComputeOptions

Tests for `ComputeOptions` — extends CompileOptions with runtime options.

| Test Case | Description |
|-----------|-------------|
| `test_defaults` | Inherits CompileOptions defaults, validate=True, server_url=None |
| `test_inherits_compile_options` | All CompileOptions fields work in ComputeOptions |
| `test_compute_specific_fields` | validate and server_url can be set |
| `test_all_fields` | Complex options with all fields set |

**ComputeOptions-specific fields:**
- `validate` — Whether to validate FHE result against plaintext
- `server_url` — Server URL for remote FHE execution

#### TestBaseOption

Tests for `BaseOption.to_compiler_options()` method.

| Test Case | Description |
|-----------|-------------|
| `test_empty` | Empty options produce empty dict |
| `test_verbose_only` | verbose=True does not affect compiler options |
| `test_compiler_options_extracted` | Only non-None compiler options are extracted |
| `test_none_options_excluded` | None values are excluded from result |

---

## Configuration Class Hierarchy

```
BaseOption
├── CompileOptions
│   └── ComputeOptions
└── FHEConfig (used by CompileOptions)
```

---

## Running Tests

```bash
# Run all config tests
pytest tests/unit/config/ -v

# Run specific test class
pytest tests/unit/config/test_option.py::TestFHEConfig -v
pytest tests/unit/config/test_option.py::TestCompileOptions -v
pytest tests/unit/config/test_option.py::TestComputeOptions -v
pytest tests/unit/config/test_option.py::TestBaseOption -v
```