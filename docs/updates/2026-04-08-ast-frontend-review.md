# AST Frontend Review and Tests

**Date**: 2026-04-08
**Category**: fix, refactor, test
**Branch**: master

## Summary

Fixed AST frontend to add `_export_to_onnx_file()` (raises NotImplementedError), added `file_format` property to FHEProgram, fixed AST converter to skip Expr statements (docstrings), and rewrote unit tests with method-based organization.

## Changes

### `python/ace/fhe/frontend/ast_frontend.py`

| Change | Description |
|--------|-------------|
| Added `_export_to_onnx_file()` | Raises NotImplementedError (AST doesn't support ONNX) |

```python
def _export_to_onnx_file(self, ...):
    """Export to ONNX file - NOT SUPPORTED."""
    raise NotImplementedError(
        "ast frontend doesn't support ONNX export. "
        "Use 'ast-via-onnx' frontend for ONNX path."
    )
```

### `python/ace/fhe/ir/fhe_program.py`

| Change | Description |
|--------|-------------|
| Added `file_format` property | Returns `None` for memory IR |

### `python/ace/fhe/ir/ast_conversion.py`

| Change | Description |
|--------|-------------|
| Skip `Expr` statements | Allows functions with docstrings |

```python
elif isinstance(stmt, ast.Expr):
    # Skip expression statements (e.g., docstrings)
    pass
```

### `tests/test_unit/test_frontend/test_ast_frontend.py`

Complete rewrite using method-based organization:

```
TestPrepare          (8 tests)  - prepare() method tests
TestCompile          (3 tests)  - compile() method tests
TestExportAir        (2 tests)  - export(format="air") tests
TestExportOnnx       (1 test)   - export(format="onnx") raises NotImplementedError
TestIRProperties     (2 tests)  - IR object properties
TestASTToIRConverter (4 tests)  - Direct converter tests
TestEdgeCases        (2 tests)  - Edge case tests
TestFrontendMeta     (1 test)   - Frontend metadata
TestUnsupportedFeatures (2 tests) - Skipped tests for unsupported features

Total: 25 tests (21 passed, 4 skipped)
```

## Output Modes

| Mode | Method | Output | Status |
|------|--------|--------|--------|
| **Memory** | `compile()` | FHEProgram (AIR IR in memory) | ✓ Works |
| **AIR file** | `export(format="air")` | .B file | ✓ Works |
| **ONNX file** | `export(format="onnx")` | - | ✗ NotImplementedError |

## AST vs AST-via-ONNX

| Feature | ast | ast-via-onnx |
|---------|-----|--------------|
| `prepare()` | FHEProgram (memory) | ONNXFileIR (file/onnx) |
| `compile()` | FHEProgram (memory) | NotImplementedError |
| export(onnx) | NotImplementedError | ONNX file |
| export(air) | .B file | .B file (fhe_cmplr) |
| Pipeline | Direct AST → AIR | AST → ONNX → AIR |

## Test Results

```
tests/test_unit/test_frontend/: 100 passed, 39 skipped
```

## Related

- [ast-via-onnx Review](2026-04-08-ast-via-onnx-review.md)
- [torch-frontend Review](2026-04-08-torch-frontend-review.md)
- [Frontend Design](../frontend_design.md)