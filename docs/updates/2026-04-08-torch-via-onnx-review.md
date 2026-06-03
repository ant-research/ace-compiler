# torch-via-onnx Frontend Review

**Date**: 2026-04-08
**Category**: fix
**Branch**: refactor-ir-formats

## Summary

Reviewed `torch-via-onnx` frontend implementation and fixed issues with output modes. Clarified that memory output is not implemented.

## Problem Analysis

The `torch-via-onnx` frontend should support three output modes:

| Mode | Method | Expected Output | Previous Status |
|------|--------|-----------------|-----------------|
| Bypass | `prepare()` | `ONNXFileIR` | ✅ Works |
| AIR file | `export(format="air")` | `.B` file | ❌ Used wrong API |
| Memory | `compile()` | `FHEProgram` | ❌ Returned wrong `format_type` |

### Issues Found

1. **`compile()` returned `format_type="file"`**: When `_onnx_path` was set on `FHEProgram`, it returned "file" instead of "memory"

2. **`_export_to_air_file()` used wrong API**: Called `FHEProgram.export_ir()` instead of `convert_onnx_to_air()` with fhe_cmplr

3. **Memory path not properly documented**: No clear indication that memory IR is not implemented

## Changes

### `fhe_dsl/python/fhe/frontend/torch_via_onnx.py`

```python
class TorchViaOnnxFrontend(Frontend):
    """PyTorch Model/Function → ONNX → AIR frontend.

    Output Modes:
    - Bypass: prepare() → ONNXFileIR → backend (ONNX file passed directly)
    - AIR file: export(format="air") → .B file → backend
    - Memory: compile() → FHEProgram → backend (NOT IMPLEMENTED)
    """

    def compile(self, source, example_inputs, input_names=None) -> FHEProgram:
        """Export PyTorch to AIR IR in memory.

        NOT IMPLEMENTED - Memory IR compilation is not yet supported.
        """
        raise NotImplementedError(
            "torch-via-onnx 'memory' output is not implemented. "
            "Use export(format='air') for .B file output, "
            "or prepare() for ONNX bypass mode."
        )

    def _export_to_air_file(self, source, example_inputs, input_names=None, output_path=None) -> str:
        """Export to ONNX, convert to AIR binary using fhe_cmplr."""
        onnx_model = self.prepare(source, example_inputs, input_names)
        air_path = convert_onnx_to_air(onnx_model.onnx_path, output_path)
        Path(onnx_model.onnx_path).unlink(missing_ok=True)
        return air_path
```

### `fhe_dsl/python/fhe/ir/fhe_program.py`

Removed `_onnx_path` attribute and fixed `format_type`:

```python
class FHEProgram(CompilationUnit):
    def __init__(self, name: str = "default_module"):
        self._name = name
        self.graphs: Dict[str, FHEGraph] = {}
        self.global_vars: Dict[str, Any] = {}
        self.meta: Dict[str, Any] = {}
        # Removed: self._onnx_path = None

    @property
    def format_type(self) -> str:
        """Return 'memory' for in-memory IR."""
        return "memory"

    @property
    def file_path(self) -> Optional[str]:
        """Return None for memory IR."""
        return None
```

## Current Status

| Mode | Method | Output | Status |
|------|--------|--------|--------|
| **Bypass** | `prepare()` | `ONNXFileIR` (file/onnx) | ✅ Works |
| **AIR file** | `export(format="air")` | `.B` file via fhe_cmplr | ✅ Works |
| **Memory** | `compile()` | `FHEProgram` | ❌ NotImplementedError |

## Usage Examples

```python
from ace.fhe.frontend.torch_via_onnx import TorchViaOnnxFrontend

frontend = TorchViaOnnxFrontend()

# Mode 1: Bypass - ONNX file passed directly to backend
onnx_model = frontend.prepare(model, inputs, input_names)
# onnx_model.format_type = "file", file_format = "onnx"

# Mode 2: AIR file - Convert to .B file
frontend.export(model, inputs, format="air", output_path="model.B")

# Mode 3: Memory - NOT IMPLEMENTED
# frontend.compile(model, inputs)  # Raises NotImplementedError
```

## Impact

### API Changes

- `compile()` now raises `NotImplementedError` instead of returning incorrect IR
- `_export_to_air_file()` now correctly uses fhe_cmplr for conversion

### Documentation Updated

- `docs/frontend_design.md` - Updated torch-via-onnx section
- `docs/ir_design.md` - Updated FHEProgram section

## Related

- [Frontend Design](../frontend_design.md)
- [IR Design](../ir_design.md)
- [IR Formats Refactor](2026-04-08-ir-formats-refactor.md)