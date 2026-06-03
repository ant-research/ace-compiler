# Decorators API

ACE provides three decorators for FHE compilation: `compile`, `compute`, and `export`.

## `fhe.compile`

Compile a function/model to an FHE program. Returns a decorator that attaches a `.compile(inputs)` method.

```python
@fhe.compile(frontend="torch", library="antlib", device="cpu", **kwargs)
def my_func(x, y):
    return x + y

program = my_func.compile([example_x, example_y])
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `frontend` | `str` | `"torch"` | Frontend strategy (see below) |
| `library` | `str` | `"antlib"` | FHE backend library |
| `device` | `str` | `"cpu"` | Target device |
| `**kwargs` | | | Compile options (see [compile_options.md](compile_options.md)) |

### Frontend Options

| Frontend | Input | Description |
|----------|-------|-------------|
| `"torch"` | `nn.Module` or function | FX trace, custom ops |
| `"torch-via-onnx"` | `nn.Module` or function | Export to ONNX first |
| `"ast"` | Python function | AST analysis |
| `"ast-via-onnx"` | Python function | AST to ONNX |
| `"onnx"` | ONNX file path | Direct ONNX input |

### Library + Device Combinations

| Library | Device | Status |
|---------|--------|--------|
| `"antlib"` | `"cpu"` | Available |
| `"phantom"` | `"cuda"` | Available (requires GPU) |
| `"acelib"` | `"cuda"` | Available (requires GPU) |
| `"seal"` | `"cpu"` | Not implemented |
| `"openfhe"` | `"cpu"` | Not implemented |

### Decorated Object Methods

After decoration, the target gains:

| Method | Description |
|--------|-------------|
| `.fhe_compile(inputs)` | Compile with example inputs, returns `CompiledProgram` |
| `.compile(inputs)` | Alias for `fhe_compile` (not added to `nn.Module` instances) |

---

## `fhe.compute`

Compile and immediately execute in FHE. The decorated function behaves like the original but runs homomorphically.

```python
@fhe.compute(frontend="torch", library="antlib", device="cpu", validate=True)
def add(x, y):
    return x + y

result = add(x, y)  # Encrypts inputs, runs FHE, decrypts result
```

### Parameters

All parameters from `compile`, plus:

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `validate` | `bool` | `True` | Compare FHE result against plaintext; raise on mismatch |
| `server_url` | `str` | `None` | Reserved for client-server mode |

### Caching

Results are cached by input tensor shapes. Repeated calls with the same shape skip recompilation.

---

## `fhe.export`

Export frontend IR to file without full FHE compilation.

```python
@fhe.export(frontend="torch", format="air", output_path="model.B")
def my_func(x):
    return x * 2

my_func.export([example_x])
```

### Parameters

All parameters from `compile`, plus:

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `format` | `str` | `"air"` | Output format: `"air"` (.B file) or `"onnx"` (.onnx file) |
| `output_path` | `str` | `"exported.ir"` | Destination file path |