# IR Formats Refactor

**Date**: 2026-04-08
**Category**: refactor
**Branch**: refactor-ir-formats

## Summary

Refactored IR format classes with better naming and added `file_format` property to distinguish ONNX vs AIR file types.

## Changes

### File Renames

| Old Name | New Name |
|----------|----------|
| `external_model.py` | `ir_formats.py` |

### Class Renames

| Old Class | New Class | Description |
|-----------|-----------|-------------|
| `FileModel` | `FileIR` | Base class for file-based IR |
| `ONNXModel` | `ONNXFileIR` | ONNX file wrapper |
| `AIRModel` | `AIRFileIR` | AIR binary file wrapper |

### New Properties

```python
class FileIR(CompilationUnit):
    @property
    def format_type(self) -> str:
        return "file"

    @property
    def file_format(self) -> str:
        """Return 'onnx' or 'air' - subclasses must implement."""
        raise NotImplementedError

class ONNXFileIR(FileIR):
    @property
    def file_format(self) -> str:
        return "onnx"

class AIRFileIR(FileIR):
    @property
    def file_format(self) -> str:
        return "air"
```

### Files Modified

- `fhe_dsl/python/fhe/ir/ir_formats.py` - Renamed and updated classes
- `fhe_dsl/python/fhe/ir/__init__.py` - Updated exports, added backward compatibility aliases
- `fhe_dsl/python/fhe/frontend/onnx_frontend.py` - Updated to use new class names
- `fhe_dsl/python/fhe/frontend/onnx_tools.py` - Added `output_path` parameter to `convert_onnx_to_air()`
- `fhe_dsl/python/fhe/backend/antlib.py` - Fixed to not set `_config_path` on string input

## Technical Details

### IR Format Types

| Format | Class | `format_type` | `file_format` | Use Case |
|--------|-------|---------------|---------------|----------|
| Memory | `FHEProgram` | "memory" | - | In-memory IR |
| ONNX File | `ONNXFileIR` | "file" | "onnx" | ONNX bypass to backend |
| AIR File | `AIRFileIR` | "file" | "air" | AIR binary (.B file) |

### Backend Integration

Backends use `format_type` and `file_format` to determine compilation strategy:

```python
def compile_to_lib(self, ir, output_dir: str) -> str:
    if ir.format_type == "file":
        if ir.file_format == "onnx":
            # Compile ONNX file with fhe_cmplr
        elif ir.file_format == "air":
            # Compile AIR file with fhe_cmplr
    elif ir.format_type == "memory":
        # Memory IR (not yet implemented)
```

## Impact

### API Changes

- New class names: `FileIR`, `ONNXFileIR`, `AIRFileIR`
- Backward compatibility aliases maintained: `FileModel`, `ONNXModel`, `AIRModel`
- New property: `file_format` on file-based IR classes

### Documentation Updated

- `docs/frontend_design.md`
- `docs/ir_design.md`
- `docs/backend_design.md`
- `docs/claude.md`

### Tests

- Existing tests continue to work with backward compatibility aliases
- New tests should use new class names

## Related

- [IR Design](../ir_design.md)
- [Frontend Design](../frontend_design.md)
- [Backend Design](../backend_design.md)