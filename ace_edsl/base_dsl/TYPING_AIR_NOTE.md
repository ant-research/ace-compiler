# Typing.py Adaptation for AIR

This file (`typing.py`) has been adapted for AIR instead of MLIR.

## Key Changes Made:

1. **Removed MLIR imports**: Removed all `from .._mlir import ir` and related MLIR imports
2. **Removed MLIR operations**: Replaced `arith.*`, `T.*`, and `ir.*` references with `NotImplementedError` or Python operations
3. **Updated protocol methods**: 
   - `__extract_mlir_values__` → `__extract_air_values__` (with compatibility wrapper)
   - `__new_from_mlir_values__` → `__new_from_air_values__` (with compatibility wrapper)
4. **Updated DslType metaclass**:
   - `mlir_type` → `air_type` (with compatibility property)
   - Updated documentation to refer to AIR instead of MLIR
5. **Removed MLIR type system**: All `T.i32()`, `T.f32()`, etc. references removed
6. **Removed MLIR-specific logic**: Binary operations, type conversions, etc. no longer use MLIR

## Current Status:

- ✅ Protocol methods updated for AIR
- ✅ DslType metaclass updated for AIR
- ✅ MLIR imports removed
- ✅ MLIR operations removed
- ✅ Type system decoupled from MLIR

## Note:

This file is part of `base_dsl` which was originally designed for MLIR. Since ace_edsl generates AIR, all MLIR-specific code has been removed. The key protocol methods have been updated to support AIR, but many implementation details are stubbed out since ace_edsl uses its own type mapping system (`edsl/core/type_mapping.py`) to convert Python types to AIR types.

## ace_edsl Type Mapping

ace_edsl uses `edsl/core/type_mapping.py` to map Python abstract types to AIR types:

```python
from ace_edsl.edsl.core.type_mapping import python_type_to_air_type
from ace_edsl.edsl.core.types import Tensor

# Python type → AIR type
air_type = python_type_to_air_type(Tensor[64], domain="tensor")
# Returns: air_builder.Type.make_array([64], air_builder.Type.make_float(32))
```

This mapping happens automatically when using domain decorators (`@nn_kernel`, `@tensor_kernel`, etc.).
