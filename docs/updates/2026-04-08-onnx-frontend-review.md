# ONNX Frontend Review and Tests

**Date**: 2026-04-08
**Category**: fix, test
**Branch**: refactor-ir-formats

## Summary

Fixed onnx frontend to match other frontends behavior, reorganized test files, and rewrote unit tests.

## Changes

### File Operations

| Operation | File |
|-----------|------|
| Deleted | `tests/test_unit/frontend/test_frontend_common.py` |
| Renamed | `test_onnx.py` → `test_onnx_frontend.py` |
| Renamed | `test_ast.py` → `test_ast_frontend.py` |

### `fhe_dsl/python/fhe/frontend/onnx_frontend.py`

| Method | Before | After |
|--------|--------|-------|
| `compile()` | Returned `FHEProgram` (Python wrapper) | Raises `NotImplementedError` |

### `tests/test_unit/frontend/test_onnx_frontend.py`

Complete rewrite using method-based organization:

```
TestPrepare          (7 tests)  - prepare() 方法测试
TestCompile          (2 tests)  - compile() 方法测试
TestExportOnnx       (4 tests)  - export(format="onnx") 测试
TestExportAir        (2 tests)  - export(format="air") 测试
TestIRProperties     (2 tests)  - IR 对象属性测试
TestEdgeCases        (3 tests)  - 边界情况测试
TestFrontendMeta     (1 test)   - 前端元数据测试

总计: 21 tests ✓
```

## Output Modes

| Mode | Method | Output | Status |
|------|--------|--------|--------|
| **Bypass** | `prepare()` | `ONNXFileIR` (file/onnx) | ✓ Works |
| **AIR file** | `export(format="air")` | `.B` file via fhe_cmplr | ✓ Works |
| **Memory** | `compile()` | `FHEProgram` | ✗ NotImplementedError |

## All Frontends Now Consistent

| Method | torch-via-onnx | ast-via-onnx | onnx |
|--------|----------------|--------------|------|
| `prepare()` | `ONNXFileIR` | `ONNXFileIR` | `ONNXFileIR` |
| `compile()` | `NotImplementedError` | `NotImplementedError` | `NotImplementedError` |
| `export(format="onnx")` | ONNX 文件 | ONNX 文件 | ONNX 文件 |
| `export(format="air")` | .B 文件 (fhe_cmplr) | .B 文件 (fhe_cmplr) | .B 文件 (fhe_cmplr) |

## Test File Naming Convention

| Frontend | Test File |
|----------|-----------|
| torch | test_torch_frontend.py |
| torch-via-onnx | test_torch_via_onnx.py |
| ast | test_ast_frontend.py |
| ast-via-onnx | test_ast_via_onnx.py |
| onnx | test_onnx_frontend.py |

## Test Results

```
tests/test_unit/frontend/: 87 passed, 56 skipped
```

## Related

- [torch-via-onnx Review](2026-04-08-torch-via-onnx-review.md)
- [ast-via-onnx Review](2026-04-08-ast-via-onnx-review.md)
- [Frontend Design](../frontend_design.md)