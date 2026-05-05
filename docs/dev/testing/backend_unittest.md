# Backend Module Unit Test Design

## Overview

This document describes the unit test design for the ACE FHE backend module. The backend module provides FHE execution engines that compile AIR IR to shared libraries for different hardware platforms.

## Test File Structure

```
tests/test_unit/test_backend/
├── conftest.py               # Shared fixtures and model definitions
├── test_registry_backend.py  # Backend registration tests (18 tests)
├── test_antlib.py            # AntLIB backend tests (12 tests)
└── test_openfhe.py           # OpenFHE backend tests (12 tests)

Total: 42 tests (40 passed, 2 skipped)
```

## Registered Backends

| Backend | Device | Description |
|---------|--------|-------------|
| `antlib` | cpu | Ant Group FHE runtime library |
| `seal` | cpu | Microsoft SEAL integration |
| `openfhe` | cpu | OpenFHE library integration |
| `phantom` | cuda | GPU-accelerated FHE (CUDA) |
| `hyperfhe` | cuda | High-performance GPU FHE (H100/sm_90) |

---

## Module 1: Backend Registry Tests

**File**: `test_registry_backend.py`
**Tests**: 18 (16 passed, 2 skipped)

### Test Classes

#### TestBackendRegistry (16 tests)

| Category | Test Case | Description |
|----------|-----------|-------------|
| **List Backends** | `test_list_backends_returns_list` | `list_backends()` returns a list |
| | `test_all_backends_registered` | Verify all 4 backends are registered |
| | `test_list_backends_supported` | `list_backends_supported()` returns combos |
| **Check Backend** | `test_check_backend_antlib_cpu` | Check antlib CPU availability |
| | `test_check_backend_antlib_cuda` | Check antlib CUDA availability |
| | `test_check_backend_invalid` | Invalid backend returns False |
| | `test_check_backend_invalid_device` | Invalid device returns False |
| **Get Backend** | `test_get_backend_antlib` | Get antlib backend instance |
| | `test_get_backend_returns_new_instance` | Each call returns new instance |
| | `test_get_nonexistent_backend_raises` | Getting invalid backend raises ValueError |
| **Interface** | `test_backend_has_required_methods` | All backends have required methods |
| | `test_backend_has_required_class_methods` | All backend classes have required class methods |

#### TestBackendRegistration (2 tests)

| Test Case | Description |
|-----------|-------------|
| `test_register_new_backend` | Register a new backend class |
| `test_register_duplicate_backend_raises` | Duplicate registration raises ValueError |

### Design Rationale

- **Registry Isolation**: Registry tests are isolated to avoid duplication
- **Interface Validation**: Ensures all backends conform to expected interface
- **Error Handling**: Validates proper error messages for invalid operations

---

## Module 2: AntLIB Backend Tests

**File**: `test_antlib.py`
**Tests**: 12

### Test Classes

#### TestAntlibBackend (6 tests)

| Test Case | Description |
|-----------|-------------|
| `test_get_antlib_backend` | Get antlib backend instance |
| `test_backend_device_name` | Verify device name is "cpu" |
| `test_backend_supported_formats` | Verify supported formats: air, onnx, torch_traced |
| `test_backend_check_available` | Backend availability check |
| `test_backend_with_options` | Backend with custom options |
| `test_backend_default_options` | Backend with default options |

#### TestAntlibBuildCommand (4 tests)

| Test Case | Description |
|-----------|-------------|
| `test_build_command_basic` | Basic g++ command generation |
| `test_build_command_with_flags` | Build command with extra flags |
| `test_build_command_includes_libs` | Command includes required libraries |
| `test_build_command_with_ace_root` | Build command with custom ace_root |

#### TestAntlibCompileToLib (2 tests)

| Test Case | Description |
|-----------|-------------|
| `test_compile_to_lib_unsupported_format` | Unsupported format raises ValueError |
| `test_compile_to_lib_creates_output_dir` | Output directory creation |

### Design Rationale

- **Two-Phase Testing**: Separates backend instantiation from compilation
- **Build Command Validation**: Ensures correct g++ command generation
- **Format Support**: Validates supported IR formats

---

## Module 3: OpenFHE Backend Tests

**File**: `test_openfhe.py`
**Tests**: 12

### Test Classes

#### TestOpenFHEBackend (6 tests)

| Test Case | Description |
|-----------|-------------|
| `test_get_openfhe_backend` | Get openfhe backend instance |
| `test_backend_device_name` | Verify device name is "cpu" |
| `test_backend_supported_formats` | Verify supported formats: air, onnx |
| `test_backend_check_available` | Backend availability check |
| `test_backend_with_options` | Backend with custom options |
| `test_backend_default_options` | Backend with default options |

#### TestOpenFHEBuildCommand (4 tests)

| Test Case | Description |
|-----------|-------------|
| `test_build_command_basic` | Basic g++ command generation |
| `test_build_command_with_flags` | Build command with extra flags |
| `test_build_command_includes_libs` | Command includes OpenFHE libraries |
| `test_build_command_with_ace_root` | Build command with custom ace_root |

#### TestOpenFHECompileToLib (2 tests)

| Test Case | Description |
|-----------|-------------|
| `test_compile_to_lib_unsupported_format` | Unsupported format raises ValueError |
| `test_compile_to_lib_air_not_implemented` | AIR compilation not yet implemented |

### Design Rationale

- **Consistent Interface**: Follows same testing pattern as AntLIB
- **OpenFHE Libraries**: Validates correct library linking
- **NotImplemented Handling**: Tests for unimplemented features

---

## Shared Fixtures

**File**: `conftest.py`

### Model Fixtures

| Fixture | Model Class | Description |
|---------|-------------|-------------|
| `simple_add_model` | `SimpleAddModel` | `x + 1` operation |
| `gemm_model` | `GemmModel` | `nn.Linear(4, 2)` |
| `conv_model` | `ConvModel` | `nn.Conv2d(3, 1, 3)` |
| `relu_model` | `ReluModel` | `torch.relu(x)` |

### Input Tensor Fixtures

| Fixture | Shape | Description |
|---------|-------|-------------|
| `input_1d` | `(1, 4)` | 1D input for GEMM |
| `input_2d` | `(1, 1, 4, 4)` | 2D input (NCHW) |
| `input_4d` | `(1, 3, 8, 8)` | 4D input for Conv |

---

## Test Summary

| Module | Tests | Description |
|--------|-------|-------------|
| Registry | 18 | Registration, interface, error handling |
| AntLIB Backend | 12 | Backend instantiation, build commands |
| OpenFHE Backend | 12 | Backend instantiation, build commands |
| **Total** | **42** | 40 passed, 2 skipped |

---

## Running Tests

```bash
# Run all backend tests
pytest tests/test_unit/test_backend/ -v

# Run specific test file
pytest tests/test_unit/test_backend/test_antlib.py -v

# Run specific test class
pytest tests/test_unit/test_backend/test_antlib.py::TestAntlibBackend -v

# Run with coverage
pytest tests/test_unit/test_backend/ --cov=python/ace/fhe/backend --cov-report=html
```

---

## Known Issues

1. **SealLIB Missing Implementation**: The `seal` backend is registered but missing `supported_format_types` method, causing instantiation errors. Tests skip backends that are not fully implemented.

2. **Build Command ace_root**: The `build_command` method in `antlib.py` does not properly handle custom `ace_root` parameter. Include paths are only added when `ace_root` is None.