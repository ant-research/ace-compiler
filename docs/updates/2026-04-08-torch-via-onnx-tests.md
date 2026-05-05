# torch-via-onnx Unit Tests Update

**Date**: 2026-04-08
**Category**: test
**Branch**: refactor-ir-formats

## Summary

Rewrote unit tests for `torch-via-onnx` frontend using method-based organization with parametrization.

## Test Organization

采用方案A：按方法+参数化组织，每个测试类对应一个API方法。

```
TestPrepare          (10 tests) - prepare() 方法测试
TestCompile          (2 tests)  - compile() 方法测试
TestExportOnnx       (5 tests)  - export(format="onnx") 测试
TestExportAir        (3 tests)  - export(format="air") 测试
TestIRProperties     (2 tests)  - IR 对象属性测试
TestEdgeCases        (4 tests)  - 边界情况测试
TestFrontendMeta     (1 test)   - 前端元数据测试
```

## Test Coverage

### TestPrepare - Bypass Mode

| Test | Description |
|------|-------------|
| `test_returns_onnx_file_ir` | verify `prepare()` returns `ONNXFileIR` |
| `test_ir_format_type_is_file` | verify `format_type == "file"` |
| `test_ir_file_format_is_onnx` | verify `file_format == "onnx"` |
| `test_creates_valid_onnx_file` | verify ONNX file is valid |
| `test_with_conv_model` | test with Conv2d model |
| `test_with_multi_input_function` | test with multi-input function |

### TestCompile - Memory Mode

| Test | Description |
|------|-------------|
| `test_raises_not_implemented` | verify `compile()` raises `NotImplementedError` |

### TestExportOnnx - ONNX File Output

| Test | Description |
|------|-------------|
| `test_creates_onnx_file` | verify ONNX file is created |
| `test_creates_valid_onnx` | verify ONNX file is valid |
| `test_with_multi_input_function` | test with multi-input function |

### TestExportAir - AIR File Output

| Test | Description |
|------|-------------|
| `test_creates_air_file` | verify .B file is created (requires fhe_cmplr) |
| `test_with_relu_function` | test with ReLU function |

### TestIRProperties

| Test | Description |
|------|-------------|
| `test_onnx_file_ir_has_entry_name` | verify `entry_name` property |
| `test_onnx_file_ir_has_onnx_path` | verify backward compatibility |

### TestEdgeCases

| Test | Description |
|------|-------------|
| `test_prepare_with_none_input` | verify error on None input |
| `test_prepare_with_empty_inputs` | verify error on empty inputs |
| `test_prepare_with_wrong_input_count` | verify error on wrong input count |
| `test_export_onnx_to_invalid_path` | verify error on invalid path |

## Parametrization

Tests use `@pytest.mark.parametrize("source_type", ["model", "function"])` to test both input types:

- `model`: `nn.Module` instance
- `function`: Python callable

## Files Modified

- `tests/test_unit/test_frontend/test_torch_via_onnx.py` - Complete rewrite

## Test Results

```
27 passed, 1 warning in 0.30s
```

## Related

- [torch-via-onnx Review](2026-04-08-torch-via-onnx-review.md)
- [Frontend Design](../frontend_design.md)