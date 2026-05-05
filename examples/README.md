# ACE FHE Examples

Fully Homomorphic Encryption (FHE) compilation examples for the ACE framework.

## Quick Start

### Prerequisites

```bash
# Install ACE package
pip install -e .

# Verify installation
python -c "from ace import fhe; print(fhe.__version__)"
```

### Run Your First Example

```bash
# Compute function (one-line, integration)
python example/01_quickstart/01_compute_function.py

# Compile and run function (separation)
python example/01_quickstart/03_compile_function.py
```

## Example Structure

```
example/
├── 01_quickstart/              # Get started in 5 minutes
│   ├── 01_compute_function.py  # Compute function (integration)
│   ├── 02_compute_model.py     # Compute model (integration)
│   ├── 03_compile_function.py  # Compile function (separation)
│   └── 04_compile_model.py     # Compile model (separation)
│
├── 02_frontend_examples/       # Different frontend strategies
│   ├── torch_function/         # PyTorch functions
│   │   ├── 01_add_func.py
│   │   ├── 02_relu_func.py
│   │   └── 03_multi_input_func.py
│   ├── torch_module/           # PyTorch nn.Modules
│   │   ├── 01_linear_model.py
│   │   └── 02_conv_model.py
│   ├── onnx_file/              # Load external ONNX models
│   │   └── 01_load_onnx.py
│   └── ast_function/           # AST frontend (Python to FHE)
│       └── 01_simple_ast.py
│
├── 03_backend_examples/        # Different FHE libraries
│   ├── antlib_cpu/             # AntLib CPU library
│   │   └── 01_basic_example.py
│   ├── phantom_cuda/           # Phantom GPU library
│   │   └── 01_gpu_example.py
│   └── hyperfhe_cuda/          # HyperFHE GPU library
│       └── 01_hyperfhe_example.py
│
├── 04_ir_formats/              # IR format options
│   ├── 01_onnx_export.py       # Export to ONNX
│   ├── 02_air_export.py        # Export to AIR (.B file)
│   └── 03_memory_compile.py    # Memory IR (default)
│
├── 05_real_world_models/       # Practical models
│   ├── 01_linear_regression.py    # House price prediction
│   ├── 02_logistic_regression.py  # Binary classification
│   ├── 03_mlp_classifier.py       # Multi-layer perceptron
│   └── 04_simple_cnn.py           # Convolutional neural network
│
├── 06_advanced/                # Advanced features
│   ├── 01_custom_encryption_params.py  # Custom CKKS parameters
│   ├── 02_multi_input_encryption.py    # Partial encryption
│   ├── 03_model_export_only.py         # Export without compile
│   └── 04_runtime_api.py               # Direct runtime API
│
└── samples/                      # Reusable components
    ├── models.py                 # Example models
    ├── functions.py              # Example functions
    └── input_generators.py       # Input tensor generators
```

## Usage Patterns

### Pattern 1: Compute Function (Integration)

Compile and run in one step using `@fhe.compute` decorator.

```python
import torch
from ace import fhe

@fhe.compute(frontend="torch", library="antlib", device="cpu", validate=True)
def add(x, y):
    return x + y

# Automatically compiles and runs in FHE
result = add(torch.ones(1, 4), torch.ones(1, 4) * 2)
```

### Pattern 2: Compute Model (Integration)

```python
import torch
import torch.nn as nn
from ace import fhe

@fhe.compute(frontend="torch", library="antlib", device="cpu", validate=True)
class LinearModel(nn.Module):
    def __init__(self):
        super().__init__()
        self.linear = nn.Linear(4, 3)

    def forward(self, x):
        return self.linear(x)

# Automatically compiles and runs in FHE
result = LinearModel(torch.randn(1, 4))
```

### Pattern 3: Compile Function (Separation)

Separate compile and run steps using `@fhe.compile` decorator.

```python
import torch
from ace import fhe

@fhe.compile(frontend="torch", library="antlib", device="cpu")
def add(x, y):
    return x + y

# Step 1: Compile - returns a CompiledProgram
program = add.compile([torch.ones(1, 4), torch.ones(1, 4) * 2])

# Step 2: Run - call program directly
result = program(x, y)

# Step 3: Validate - internal validation
is_valid = program.validate()
```

### Pattern 4: Compile Model (Separation)

```python
import torch
import torch.nn as nn
from ace import fhe

@fhe.compile(frontend="torch", library="antlib", device="cpu")
class LinearModel(nn.Module):
    def __init__(self):
        super().__init__()
        self.linear = nn.Linear(4, 4)

    def forward(self, x):
        return self.linear(x)

# Step 1: Compile - returns a CompiledProgram
program = LinearModel.compile([torch.randn(1, 4)])

# Step 2: Run - call program directly
result = program(x)

# Step 3: Validate - internal validation
is_valid = program.validate()
```

### Pattern 5: Export Only

```python
import torch
from ace import fhe

@fhe.export(frontend="torch", format="onnx", output_path="model.onnx")
def model(x):
    return x * 2

# Export without full compilation
model.export([torch.ones(1, 4)])
```

## Available Frontends

| Frontend | Description | Use Case |
|----------|-------------|----------|
| `torch` | Direct PyTorch tracing | Standard PyTorch models |
| `torch-via-onnx` | PyTorch → ONNX → FHE | Better ONNX compatibility |
| `onnx` | Load external ONNX file | Pre-exported models |
| `ast` | Python AST analysis | Simple Python functions |

## Available Libraries

| Library | Device | Description |
|---------|--------|-------------|
| `antlib` | CPU | Ant Group FHE library |
| `phantom` | CUDA | GPU-accelerated FHE |
| `hyperfhe` | CUDA | Alternative GPU library |
| `seal` | CPU | Microsoft SEAL (limited) |
| `openfhe` | CPU | OpenFHE (limited) |

## Configuration Options

### Encryption Parameters (`ckks`)

```python
@fhe.compile(
    frontend="torch",
    library="antlib",
    device="cpu",
    ckks={
        "N": 8192,       # Polynomial modulus degree
        "scale": 2**40,  # Scaling factor
        "level": 3,      # Multiplicative depth
    },
)
```

### Partial Encryption

```python
# Encrypt only specific inputs
@fhe.compile(
    frontend="torch",
    library="antlib",
    encrypt_inputs=["x"],  # Only encrypt 'x'
)
def add(x, y):
    return x + y  # y stays in plaintext
```

## Troubleshooting

### GPU Backend Not Available

```python
from ace.fhe.util import gpu_available

if not gpu_available():
    print("GPU not available, falling back to CPU")
    library, device = "antlib", "cpu"
else:
    library, device = "phantom", "cuda"
```

### fhe_cmplr Not Found

The FHE compiler (`fhe_cmplr`) is required for AIR export and some libraries.
If not available, use memory IR compilation (default):

```python
# This works without fhe_cmplr
@fhe.compile(frontend="torch", library="antlib", device="cpu")
def add(x, y):
    return x + y
```

## Learn More

- [ACE Documentation](../docs/)
- [FHE Concepts](../docs/fhe_basics.md)
- [API Reference](../docs/api.md)

## Contributing

Contributions welcome! When adding new examples:

1. Follow the naming convention: `NN_description.py`
2. Include docstrings and comments
3. Add validation step
4. Test with `python example/path/to/example.py`