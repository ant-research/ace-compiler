# Decorators Design

## Overview

The decorators module provides user-facing APIs for FHE compilation and execution. It offers three decorators:

- `@compile` - FHE compilation decorator
- `@compute` - Compilation + immediate execution (transparent FHE)
- `@export` - Frontend export IR to file

## Architecture

```
+------------------+     +------------------+     +------------------+
|   @compile()     |     |   @compute()     |     |   @export()      |
|                  |     |                  |     |                  |
| Returns:         |     | Returns:         |     | Returns:         |
| Compiled program |     | Wrapped function |     | Exported file    |
| (for later use)  |     | (runs in FHE)    |     | (AIR/ONNX)       |
+------------------+     +------------------+     +------------------+
        |                        |                        |
        +------------------------+------------------------+
                                 |
                                 v
                    +------------------------+
                    |   Driver Driver   |
                    +------------------------+
                                 |
        +------------------------+------------------------+
        |                        |                        |
        v                        v                        v
+---------------+        +---------------+        +---------------+
|   Frontend    |        |    Backend    |        |    Runtime    |
|   (torch,     |        |   (antlib,    |        |   (encrypt,   |
|    onnx,      |        |    phantom,   |        |    run,       |
|    ast)       |        |    hyperfhe)  |        |    decrypt)   |
+---------------+        +---------------+        +---------------+
```

## Decorator Details

### @compile

**Purpose**: Compile a function or model to FHE, returns a compiled program object.

**Usage**:
```python
from ace.fhe.decorators import compile

@compile(frontend="torch", library="antlib", encrypt_inputs=["x"])
def square(x):
    return x * x

# Compile with example inputs
prog = square.compile([input_tensor])

# Use with runtime
runner = FHERuntime(prog)
result = runner.inference(encrypted_input)
```

**Options**:
- `frontend`: Frontend strategy name (default: "torch")
- `backend`: Backend name (default: "antlib")
- `device`: Device name (default: "cpu")
- `encrypt_inputs`: List of input names or indices to encrypt
- Other `CompileOptions` fields

**Returns**: The decorated function/model with attached `_fhe_compiler`, `_fhe_options`, and `compile` method.

### @compute

**Purpose**: Compile and immediately execute in FHE (transparent FHE).

**Usage**:
```python
from ace.fhe.decorators import compute

@compute(frontend="torch", library="antlib", encrypt_inputs=[0])
def add(a, b):
    return a + b

# Runs in FHE transparently
result = add(input0, input1)
```

**Options**:
- Same as `@compile` plus:
- `validate`: Enable result validation (default: True)
- `compare_plaintext`: Compare with plaintext execution (default: False)

**Returns**: A wrapped function that runs in FHE transparently.

**Internal Flow**:
1. Wraps the target function/model
2. On first call with given input shapes:
   - Compile the function
   - Create FHERuntime
   - Cache the runner
3. On subsequent calls:
   - Reuse cached runner
   - Run inference
   - Validate if enabled

### @export

**Purpose**: Export frontend IR to file without full compilation.

**Usage**:
```python
from ace.fhe.decorators import export

@export(frontend="torch", format="air", output_path="model.B")
def model(x):
    return x * x

# Export to file
model.export([input_tensor])
```

**Options**:
- Same as `@compile` plus:
- `format`: Output format ("air" for .B file, "onnx" for .onnx file)
- `output_path`: Output file path

**Returns**: The decorated function/model with attached `export` method.

## Implementation Details

### Helper Functions

| Function | Purpose |
|----------|---------|
| `_validate_frontend_backend()` | Validate frontend/backend availability |
| `_validate_kwargs()` | Validate decorator kwargs |
| `_resolve_encrypted_inputs()` | Convert input indices/names to parameter names |
| `_get_param_names()` | Extract parameter names from function/model |
| `_create_wrapped_function()` | Create wrapped function for compilation |

### Decorator Flow

```
@compile / @compute / @export
          |
          v
1. Validate frontend/backend
          |
          v
2. Validate kwargs
          |
          v
3. Create options (CompileOptions / ComputeOptions)
          |
          v
4. Wrap target (function/model)
          |
          v
5. Create Driver driver
          |
          v
6. Attach compiler/options/methods to target
          |
          v
7. Return decorated target
```

### Caching Strategy (@compute)

`@compute` uses a shape-based cache to avoid recompilation:

```python
_cache_key = tuple(t.shape for t in args)

if cache_key not in _compiled_cache:
    # Compile and create runner
    package = compiler.compile(wrapped_func, inputset, ...)
    runner = FHERuntime(package)
    _compiled_cache[cache_key] = runner
else:
    runner = _compiled_cache[cache_key]

result = runner.inference(*inputset)
```

## Options Classes

### CompileOptions

```python
@dataclass
class CompileOptions:
    encrypt_inputs: Optional[List[Union[str, int]]] = None
    output_dir: Optional[str] = None
    keep_intermediate: bool = False
    verbose: bool = False
    # ... additional options
```

### ComputeOptions

```python
@dataclass
class ComputeOptions(CompileOptions):
    validate: bool = True
    compare_plaintext: bool = False
```

## Target Types

The decorators support three target types:

| Target Type | Handling |
|-------------|----------|
| Model class (`type[nn.Module]`) | Create instance, wrap `forward()` |
| Model instance (`nn.Module`) | Wrap `forward()` directly |
| Function (`Callable`) | Wrap function directly |

### Example: Model Class

```python
@compile(frontend="torch", library="antlib")
class AddModel(nn.Module):
    def forward(self, x, y):
        return x + y

# Usage
model = AddModel()
prog = model.compile([input_x, input_y])
```

### Example: Function

```python
@compile(frontend="torch", library="antlib")
def add(x, y):
    return x + y

# Usage
prog = add.compile([input_x, input_y])
```

## Encrypted Inputs Resolution

The `encrypt_inputs` parameter can be:

| Format | Example | Resolution |
|--------|---------|------------|
| `None` | `None` | All parameters |
| Empty list | `[]` | No parameters |
| Names | `["x", "y"]` | Use named parameters |
| Indices | `[0, 1]` | Use positional parameters |

### Implementation

```python
def _resolve_encrypted_inputs(encrypt_inputs, param_names):
    if encrypt_inputs is None:
        return param_names[:]  # All parameters
    
    if not encrypt_inputs:
        return []  # No parameters
    
    if isinstance(encrypt_inputs[0], int):
        # Indices
        return [param_names[i] for i in encrypt_inputs]
    
    # Names
    for name in encrypt_inputs:
        if name not in param_names:
            raise ValueError(f"Parameter '{name}' not found")
    
    return list(encrypt_inputs)
```

## Compiler Integration

The decorators use `Driver` from the driver module:

```python
compiler = Driver(
    frontend_name=frontend,
    backend_name=backend,
    backend_config={"device": device}
)

# Compile
package = compiler.compile(wrapped_func, inputset, input_names=encrypt_inputs)
```

### Backend Config

Backend-specific options are passed via `backend_config`:

```python
backend_config = {"device": device}
for key in ['vec', 'ckks', 'sihe', 'p2c']:
    if getattr(options, key, None):
        backend_config[key] = getattr(options, key)
```

## Runtime Integration

`@compute` uses `FHERuntime` for execution:

```python
runner = FHERuntime(package, verify="array")
result = runner.inference(*inputset)

# Optional validation
if options.validate:
    if not runner.validate():
        raise RuntimeError("Result validation failed")
```

## Error Handling

| Error Type | Cause | Handling |
|------------|-------|----------|
| `ValueError` | Unknown frontend/backend | Raised in `_validate_frontend_backend()` |
| `TypeError` | Unexpected kwargs | Raised in `_validate_kwargs()` |
| `ValueError` | Invalid parameter index/name | Raised in `_resolve_encrypted_inputs()` |
| `RuntimeError` | Result validation failed | Raised in `@compute` wrapper |

## Testing

```python
# Test @compile
def test_compile_decorator():
    @compile(frontend="torch", library="antlib")
    def add(x, y):
        return x + y
    
    prog = add.compile([torch.randn(1, 3), torch.randn(1, 3)])
    assert prog is not None

# Test @compute
def test_compute_decorator():
    @compute(frontend="torch", library="antlib")
    def add(x, y):
        return x + y
    
    result = add(torch.randn(1, 3), torch.randn(1, 3))
    assert result is not None

# Test @export
def test_export_decorator():
    @export(frontend="torch", format="air")
    def model(x):
        return x * 2
    
    model.export([torch.randn(1, 3)], output_path="test.B")
    assert os.path.exists("test.B")
```

## Related Documents

- [Driver Design](driver.md) - Driver implementation
- [Frontend Design](frontend.md) - Frontend implementations
- [Backend Design](backend.md) - Backend implementations