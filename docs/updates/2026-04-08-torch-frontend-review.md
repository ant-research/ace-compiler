# Torch Frontend Review and Tests

**Date**: 2026-04-08
**Category**: fix, refactor, test
**Branch**: master

## Summary

Fixed torch frontend to add `file_format` property, implement ONNX export, fix circular import issue, and rewrote unit tests with method-based organization.

## Key Finding: Torch Frontend is Unique

Torch frontend is fundamentally different from other frontends:

| Aspect | torch-via-onnx/ast-via-onnx/onnx | torch |
|--------|----------------------------------|-------|
| `prepare()` | Returns ONNXFileIR (file/onnx) | Returns TorchTracedModel (not executed) |
| `compile()` | NotImplementedError | Returns TorchTracedModel (executed, AIR generated) |
| AIR export | fhe_cmplr (external compiler) | air_gen.write_air_ir() (C++ extension) |
| Memory Mode | NOT IMPLEMENTED | IMPLEMENTED ✓ |
| Input | ONNX file or Python function | PyTorch nn.Module |

## Changes

### `python/ace/fhe/frontend/torch_frontend.py`

| Change | Description |
|--------|-------------|
| Added `file_format` property | Returns `"air"` after export, `None` before |
| Fixed import check | Use `_air_gen is not None` to avoid circular import |
| Added `_export_to_onnx_file()` | Now supports ONNX export |

```python
def _export_to_onnx_file(self, model, inputs, input_names=None, output_path=None):
    """Export PyTorch model to ONNX file."""
    export_model_to_onnx(model, inputs, output_path, input_names)
    return output_path
```

### `tests/test_unit/test_frontend/test_torch_frontend.py`

Complete rewrite using method-based organization:

```
TestPrepare          (8 tests)  - prepare() method tests (requires ace_ext)
TestCompile          (5 tests)  - compile() method tests (requires ace_ext)
TestExportAir        (4 tests)  - export(format="air") tests (requires ace_ext)
TestExportOnnx       (3 tests)  - export(format="onnx") tests (requires torch.fx only)
TestIRProperties     (2 tests)  - IR object properties (requires ace_ext)
TestTorchTracedModelMethods (5 tests) - Helper method tests (requires ace_ext)
TestBinaryOperators  (3 tests)  - Parametrized binary op tests (requires ace_ext)
TestUnaryOperators   (3 tests)  - Unary operator tests (requires ace_ext)
TestEdgeCases        (3 tests)  - Edge case tests (requires ace_ext)
TestFrontendMeta     (1 test)   - Frontend metadata (no deps)
TestBackendIntegration (2 tests) - Backend integration (requires ace_ext)

Total: 39 tests
```

## Output Modes

| Mode | Method | Output | Status |
|------|--------|--------|--------|
| **Memory** | `compile()` | TorchTracedModel (AIR IR generated) | ✓ Works |
| **AIR file** | `export(format="air")` | .B file via air_gen | ✓ Works |
| **ONNX file** | `export(format="onnx")` | ONNX file | ✓ Works (new) |

## All Frontends Summary

| Frontend | prepare() | compile() | export(onnx) | export(air) |
|----------|-----------|-----------|--------------|-------------|
| **torch** | TorchTracedModel | TorchTracedModel (AIR生成) | ONNX 文件 | .B 文件 (air_gen) |
| **torch-via-onnx** | ONNXFileIR | NotImplementedError | ONNX 文件 | .B 文件 (fhe_cmplr) |
| **ast** | FHEProgram | FHEProgram | - | .B 文件 |
| **ast-via-onnx** | ONNXFileIR | NotImplementedError | ONNX 文件 | .B 文件 (fhe_cmplr) |
| **onnx** | ONNXFileIR | NotImplementedError | ONNX 文件 | .B 文件 (fhe_cmplr) |

## Test Results

```
tests/test_unit/test_frontend/: 91 passed, 41 skipped
```

Note: Tests requiring ace_ext are skipped when C++ extension is not available. ONNX export tests only require torch.fx.

## Related

- [torch-via-onnx Review](2026-04-08-torch-via-onnx-review.md)
- [ast-via-onnx Review](2026-04-08-ast-via-onnx-review.md)
- [onnx-frontend Review](2026-04-08-onnx-frontend-review.md)
- [Frontend Design](../frontend_design.md)