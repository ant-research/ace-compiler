# ast-via-onnx Frontend Review and Tests

**Date**: 2026-04-08
**Category**: fix, test
**Branch**: refactor-ir-formats

## Summary

Fixed ast-via-onnx frontend to match torch-via-onnx behavior and rewrote unit tests.

## Changes

### `python/ace/fhe/frontend/ast_via_onnx.py`

| Method | Before | After |
|--------|--------|-------|
| `compile()` | Returned `FHEProgram` (Python wrapper) | Raises `NotImplementedError` |
| `_export_to_air_file()` | Used `FHEProgram.export_ir()` | Uses `fhe_cmplr` via `convert_onnx_to_air()` |

### Issues Fixed

1. **`compile()` returned incorrect IR**: Previously returned Python `FHEProgram` which is not usable by backend. Now raises `NotImplementedError` like torch-via-onnx.

2. **`_export_to_air_file()` used wrong API**: Previously called `FHEProgram.export_ir()` which doesn't generate valid AIR binary. Now uses `convert_onnx_to_air()` with `fhe_cmplr`.

### `tests/test_unit/test_frontend/test_ast_via_onnx.py`

Complete rewrite using method-based organization:

```
TestPrepare          (10 tests) - prepare() 方法测试
TestCompile          (2 tests)  - compile() 方法测试
TestExportOnnx       (5 tests)  - export(format="onnx") 测试
TestExportAir        (3 tests)  - export(format="air") 测试
TestIRProperties     (2 tests)  - IR 对象属性测试
TestEdgeCases        (4 tests)  - 边界情况测试
TestFrontendMeta     (1 test)   - 前端元数据测试

总计: 27 tests ✓
```

## Output Modes

| Mode | Method | Output | Status |
|------|--------|--------|--------|
| **Bypass** | `prepare()` | `ONNXFileIR` (file/onnx) | ✓ Works |
| **AIR file** | `export(format="air")` | `.B` file via fhe_cmplr | ✓ Works |
| **Memory** | `compile()` | `FHEProgram` | ✗ NotImplementedError |

## Consistency with torch-via-onnx

Both frontends now have identical behavior:

| Aspect | torch-via-onnx | ast-via-onnx |
|--------|----------------|--------------|
| `prepare()` | Returns `ONNXFileIR` | Returns `ONNXFileIR` |
| `compile()` | Raises `NotImplementedError` | Raises `NotImplementedError` |
| `_export_to_air_file()` | Uses `fhe_cmplr` | Uses `fhe_cmplr` |
| Test structure | Method-based + parametrized | Method-based + parametrized |

## Related

- [torch-via-onnx Review](2026-04-08-torch-via-onnx-review.md)
- [torch-via-onnx Tests](2026-04-08-torch-via-onnx-tests.md)
- [Frontend Design](../frontend_design.md)