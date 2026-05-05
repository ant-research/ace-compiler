# Add add_air_operation API for Unified IR Generation

**Date**: 2026-04-08
**Category**: feature, refactor
**Branch**: master

## Summary

Added `add_air_operation()` API to air_gen module, enabling AST frontend (and other frontends) to generate AIR IR directly without going through ONNX. This unifies the IR generation architecture across all frontends.

## Architecture Before

```
torch 前端：torch.ops.tensor.xxx → C++ kernel → AddOperation() → AIR
AST 前端：  AST 分析 → FHEProgram → export_ir() → pickle (fallback)
ONNX 前端：ONNX → fhe_cmplr → AIR（编译器内部）
```

## Architecture After

```
torch 前端：torch.ops.tensor.xxx → C++ kernel → AddOperation() → AIR
AST 前端：  AST 分析 → FHEProgram → add_air_operation() → AIR
ONNX 前端：ONNX → fhe_cmplr → AIR
```

All frontends now use the same underlying air_gen API!

## Changes

### `csrc/air_gen/include/air/air_builder.h`

Added function declaration:

```cpp
std::string add_air_operation(
    const std::string& op_name,
    const std::vector<std::string>& input_names);
```

### `csrc/air_gen/src/air_builder.cxx`

1. Added `GetOpcodeFromName()` helper function with supported operations:
   - add, sub, mul, div (DIVIDE)
   - relu, silu
   - matmul, conv, gemm
   - max_pool, avg_pool (AVERAGE_POOL), global_avg_pool
   - flatten, concat, softmax
   - sqrt, transpose, reshape

2. Added `add_air_operation()` implementation

### `csrc/air_gen/pybind_extension.cxx`

Exposed `add_air_operation` to Python.

### `python/ace/fhe/ir/fhe_program.py`

Updated `_export_as_air_binary()` to:
1. Iterate through IR nodes
2. Call `add_air_operation()` for each operation
3. Use symbol table to track variable names

### `python/ace/fhe/ir/ast_conversion.py`

Added support for `Pass` statements (skip them).

## Supported Operations

| Operation | OPCODE |
|-----------|--------|
| add | NN::ADD |
| sub | NN::SUB |
| mul | NN::MUL |
| div | NN::DIVIDE |
| relu | NN::RELU |
| silu | NN::SILU |
| matmul | NN::MATMUL |
| conv | NN::CONV |
| gemm | NN::GEMM |
| max_pool | NN::MAX_POOL |
| avg_pool | NN::AVERAGE_POOL |
| global_avg_pool | NN::GLOBAL_AVERAGE_POOL |
| flatten | NN::FLATTEN |
| concat | NN::CONCAT |
| softmax | NN::SOFTMAX |
| sqrt | NN::SQRT |
| transpose | NN::TRANSPOSE |
| reshape | NN::RESHAPE |

## Test Results

```
tests/test_unit/test_frontend/: 135 passed, 4 skipped
```

## Usage Example

```python
from ace import air_gen

# Build AIR IR
air_gen.begin_air_function('my_func')
air_gen.add_air_input('x', [1, 4])
air_gen.add_air_input('y', [1, 4])
air_gen.end_air_function([1, 4])

# Add operations
result = air_gen.add_air_operation('add', ['x', 'y'])
air_gen.add_air_operation('relu', [result])

# Finalize and write
air_gen.finalize_air_function()
air_gen.write_air_ir('output.B')
```

## Related

- [AST Frontend Review](2026-04-08-ast-frontend-review.md)
- [Torch Frontend Review](2026-04-08-torch-frontend-review.md)