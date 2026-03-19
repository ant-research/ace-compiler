# ACE Bindings

Shared C++ Python bindings for the ACE compiler infrastructure via pybind11.

## Modules

| Module | Description |
|--------|-------------|
| `air_builder` | Core AIR infrastructure (GlobScope, FuncScope, Container, Node) |
| `nn_addon` | Neural network operations (nn::core, nn::vector) |
| `fhe_cmplr` | FHE compilation (fhe::sihe, fhe::ckks, fhe::poly) |
| `passmanager` | Pass infrastructure (PassManager, Pass) |

## Prerequisites

- Python 3.8+
- pybind11 (`pip install pybind11`)
- CMake 3.14+
- ACE compiler libraries built and installed in `ace_cmplr/`

## Build

```bash
cd ace-compiler/bindings
mkdir build && cd build

cmake .. \
    -Dpybind11_DIR=$(python -c "import pybind11; print(pybind11.get_cmake_dir())")

make -j$(nproc)
```

The `.so` files are automatically output to `../ace_bindings/`.

### CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `ACE_COMPILER_DIR` | `..` | Path to ace-compiler source |
| `ACE_INSTALL_DIR` | `${ACE_COMPILER_DIR}/ace_cmplr` | Path to ace_cmplr install |
| `ACE_BINDINGS_OUTPUT_DIR` | `${ACE_COMPILER_DIR}/ace_bindings` | Output directory for .so files |
| `ONNX_PROTO_DIR` | (auto-detected) | Path to generated ONNX proto files |

## Usage

### Option 1: Add to PYTHONPATH

```bash
export PYTHONPATH=/path/to/ace-compiler:$PYTHONPATH
```

### Option 2: Install in Development Mode

```bash
cd ace-compiler/ace_bindings
pip install -e .
```

### Import in Python

```python
from ace_bindings import air_builder, nn_addon, fhe_cmplr, passmanager

# Create a global scope
glob = air_builder.create_glob_scope()

# Create a function
func = glob.new_func("my_kernel")
container = func.container()

# Create parameters
a = func.new_param("a", air_builder.Type.make_float(32))
b = func.new_param("b", air_builder.Type.make_float(32))

# Build IR
result = container.new_add(a, b)
```

## Directory Structure

```
ace-compiler/
├── ace_bindings/           # Python package (output .so files go here)
│   ├── __init__.py
│   ├── setup.py
│   ├── air_builder.*.so
│   ├── nn_addon.*.so
│   ├── fhe_cmplr.*.so
│   └── passmanager.*.so
│
├── bindings/               # Build system (this directory)
│   ├── CMakeLists.txt
│   ├── README.md
│   ├── src/
│   │   ├── air_builder_bindings.cpp
│   │   ├── nn_addon_bindings.cpp
│   │   ├── fhe_cmplr_bindings.cpp
│   │   ├── passmanager_bindings.cpp
│   │   └── ...
│   └── build/              # CMake build directory
│
├── acepy/                  # Python DSL (AST parsing approach)
└── ace_edsl/               # Python EDSL (AST preprocessing + operator overloading)
```

## Type Remapping for Kernel Inlining

When inlining Python DSL kernels into a main model, type IDs from different `GLOB_SCOPE` objects must be remapped. The `inline_lowering_from_glob` function handles this automatically:

### Type Remapping Strategy

| Type Category | Remapping Method |
|---------------|------------------|
| Primitive types | Shared across scopes (same ID) |
| Record types (CIPHERTEXT, PLAINTEXT, CIPHERTEXT3) | Matched by type name |
| Array types | Created in target scope with remapped element type |
| Pointer types | Created in target scope with remapped base type |

### Key Functions

```cpp
// Find or create a matching type in target glob_scope
std::function<TYPE_ID(TYPE_PTR)> find_or_create_type;

// Remap type ID from source to target scope
auto remap_type_id = [&](TYPE_ID src_type_id) -> TYPE_ID;

// Get remapped TYPE_PTR
auto remap_type = [&](TYPE_PTR src_type) -> TYPE_PTR;
```

### Usage in Node Cloning

When cloning nodes from a lowering kernel, binary/unary arithmetic nodes are created with remapped types:

```cpp
// Clone with remapped type
TYPE_PTR remapped_type = remap_type(src_node->Rtype());
new_node = new_cntr.New_bin_arith(src_opc, remapped_type, child0, child1, spos);
```

## Troubleshooting

### "ACE libraries not found"

Ensure ACE compiler is built:
```bash
cd ace-compiler
mkdir -p release && cd release
cmake ..
make -j$(nproc)
make install
```

### "pybind11 not found"

Install pybind11:
```bash
pip install pybind11
```

### Import errors at runtime

Check that `.so` files exist in `ace_bindings/`:
```bash
ls ace-compiler/ace_bindings/*.so
```

Verify PYTHONPATH includes `ace-compiler/`:
```bash
python -c "from ace_bindings import air_builder; print('OK')"
```
