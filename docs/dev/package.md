# Package Management

This document describes how to install, build, and use the ACE FHE package.

## Quick Start

```bash
# Install from PyPI
pip install ace

# Verify installation
python -c "import ace; print(ace.__version__)"
```

## Installation

### From PyPI (Recommended)

```bash
pip install ace
```

### From GitHub Release (TBD)

```bash
# Install specific version
pip install https://github.com/ant-group/ace/releases/download/v0.2.0/ace-0.2.0-cp310-cp310-linux_x86_64.whl

# Install latest from main branch
pip install --extra-index-url https://ant-group.github.io/ace/ ace
```

### From Local Source

```bash
# Install build dependencies
pip install scikit-build-core torch pybind11 cmake PyYAML ninja

# Build and install (Debug by default)
./scripts/dev-build.sh

# Or use Makefile
make build && make install

# Install in editable mode
pip install -e . --no-build-isolation
```

### From Local Wheel

```bash
# Build wheel package
pip wheel . -w dist/

# Install from local wheel
pip install dist/ace-*.whl
```

## Building Wheel Package

### Basic Build

```bash
# Build wheel (Release by default)
pip wheel . -w dist/

# Output: dist/ace-{version}-{pyver}-{abi}-{platform}.whl
```

### Build Variants

```bash
# CPU-only
pip wheel . -w dist/

# GPU with Phantom backend
ENABLE_LIB="phantom" pip wheel . -w dist/

# GPU with Acelib backend
ENABLE_LIB="acelib" pip wheel . -w dist/

# Multiple backends
ENABLE_LIB="acelib;phantom" pip wheel . -w dist/

# Debug build
pip wheel . -w dist/ --config-settings=cmake.define.CMAKE_BUILD_TYPE=Debug

# Clean build
rm -rf build/ dist/ _skbuild/
```

### Using Build Script

```bash
# Build wheel with auto version detection (adds CUDA version suffix)
./scripts/build-wheel.sh

# Clean and rebuild
./scripts/build-wheel.sh --clean
```

### GitHub Actions Build

Wheels are built inside pre-built Docker dev images and published as GitHub artifacts.

#### Docker Dev Images

| Type | Image Tag | PyTorch | Base |
|------|-----------|---------|------|
| CPU | `opencc/dev:ubuntu22.04-torch2.5.1` | 2.5.1 | Ubuntu 22.04 |
| CPU | `opencc/dev:ubuntu24.04-torch2.6.0` | 2.6.0 | Ubuntu 24.04 |
| CPU | `opencc/dev:ubuntu24.04-torch2.11.0` | 2.11.0 | Ubuntu 24.04 |
| CUDA | `opencc/dev-cuda:12.4.0-devel-ubuntu22.04-torch2.5.1` | 2.5.1+cu124 | CUDA 12.4.0 |
| CUDA | `opencc/dev-cuda:12.6.0-devel-ubuntu24.04-torch2.6.0` | 2.6.0+cu126 | CUDA 12.6.0 |
| CUDA | `opencc/dev-cuda:13.0.0-devel-ubuntu24.04-torch2.11.0` | 2.11.0+cu130 | CUDA 13.0.0 |

Note: CUDA 12.5 is covered by the cu124 image (PyTorch has no cu125 wheels; CUDA 12.5 is forward-compatible with cu124).

#### Wheel Naming

```bash
# CPU wheels
ACE_LOCAL_VERSION=cpu
# Output: ace_fhe-0.2.0+cpu-cp310-cp310-linux_x86_64.whl

# CUDA wheels
ACE_LOCAL_VERSION=cu124  # CUDA 12.4.0
ACE_LOCAL_VERSION=cu126  # CUDA 12.6.0
ACE_LOCAL_VERSION=cu130  # CUDA 13.0.0
# Output: ace_fhe-0.2.0+cu124-cp310-cp310-linux_x86_64.whl
```

The local version tag format is `+cpu` or `+cuXXX` where `XXX` is the CUDA major/minor version without dots (e.g., `124` for CUDA 12.4.0).

#### Version Selection Rationale

Only PyTorch versions with corresponding CUDA wheels on the official index are supported:

| PyTorch | cu124 | cu126 | cu130 |
|---------|-------|-------|-------|
| 2.5.1 | Yes | - | - |
| 2.6.0 | Yes | Yes | - |
| 2.11.0 | - | - | Yes |

**Selection strategy: 3 tiers (old-stable / mainstream / latest)**

We maintain exactly 3 image variants per platform (CPU/CUDA), each serving a distinct purpose:

| Tier | Purpose | Criteria | Example |
|------|---------|----------|---------|
| Old-stable | Compatibility | Last patch of previous minor release; users on older CUDA/Ubuntu | CUDA 12.4 + torch 2.5.1 |
| Mainstream | Recommended | Current minor release with broadest CUDA coverage | CUDA 12.6 + torch 2.6.0 |
| Latest | Bleeding edge | Newest release for early adopters and testing | CUDA 13.0 + torch 2.11.0 |

Rules:
- One image per tier, no duplicates
- Each image must have a matching PyTorch wheel on the official index (no missing cuXXX wheels)
- CUDA versions without a dedicated PyTorch wheel (e.g., CUDA 12.5) are covered by backward-compatible images (cu124 works on CUDA 12.5)
- When a new PyTorch minor release stabilizes, the old-stable tier rotates out and mainstream becomes old-stable

## Package Structure

After installation, the package structure is:

```
site-packages/ace/
├── __init__.py              # Package entry
├── _version.py              # Version info
├── fhe/                     # Python modules
│   ├── __init__.py
│   ├── decorators.py        # @compile, @compute, @export
│   ├── driver.py            # Driver class
│   ├── frontend/            # Frontend implementations
│   ├── ir/                  # Intermediate representations
│   ├── backend/             # Backend implementations
│   ├── compiler/            # Compiler
│   ├── runtime/             # Runtime
│   └── util.py              # Utilities
├── model/                   # Pre-built models (ResNet, datasets, profiling)
├── sample/                  # Sample operators and functions
├── bin/
│   └── fhe_cmplr            # FHE compiler binary
├── lib/
│   ├── libFHErt_common.so   # Common runtime
│   ├── libFHErt_ant.so      # Ant CPU runtime
│   ├── libFHErt_phantom.so  # Phantom GPU runtime
│   └── libFHErt_ace.so # Acelib GPU runtime
├── include/                 # C++ headers (development)
├── frontend.cpython-*.so    # PyTorch extension (frontend)
└── runtime.cpython-*.so     # PyTorch extension (runtime)
```

## Python Usage

### Basic Import

```python
import ace
print(ace.__version__)  # e.g., "0.2.0"
```

### High-Level API

```python
from ace import fhe

@fhe.compile(frontend="torch", library="antlib", device="cpu")
def add(x, y):
    return x + y

prog = add.compile([input0, input1])
runner = fhe.JITRunner(prog)
runner.inference(*inputset)
```

### Using Driver Directly

```python
from ace.fhe.driver import Driver

compiler = Driver(
    frontend="torch",
    library="antlib",
    device="cpu"
)

result = compiler.compile(model, inputs)
```

### Frontend and Backend

```python
from ace.fhe.frontend import get_frontend
from ace.fhe.backend import get_backend

# Get frontend
frontend = get_frontend("torch")

# Get backend
backend = get_backend("antlib", device="cpu")
```

## Dependencies

### Runtime Dependencies

| Package | Version | Purpose |
|---------|---------|---------|
| `torch` | >=2.0 | PyTorch frontend support |
| `numpy` | >=1.20 | Numerical operations |
| `PyYAML` | >=5.3 | Configuration parsing |

### Optional Dependencies

| Package | Version | Purpose |
|---------|---------|---------|
| `onnx` | >=1.10 | ONNX model handling |

### Build Dependencies

| Package | Version | Purpose |
|---------|---------|---------|
| `scikit-build-core` | >=0.11 | Build backend |
| `cmake` | >=3.28 | CMake build system |
| `pybind11` | >=2.10 | C++ extension binding |
| `torch` | >=2.0 | Required for extension build |

## Version Management

Version is defined in `fhe_dsl/python/_version.py`:

```python
__version__ = "0.2.0"
```

## Environment Variables

| Variable | Default | Description |
|----------|---------|-------------|
| `ENABLE_LIB` | `phantom` | FHE backend (phantom, acelib, seal, openfhe) |
| `CMAKE_BUILD_TYPE` | `Release` | Build type (Debug/Release) |
| `LD_LIBRARY_PATH` | - | Runtime library path |

### Setting Library Path

If you encounter library loading issues:

```bash
export LD_LIBRARY_PATH=$(python -c "import sysconfig; print(sysconfig.get_path('platlib'))")/ace/lib:$LD_LIBRARY_PATH
```

## Related Documentation

- [Developer Guide](develop.md) - Build from source for development
- [Release Process](release.md) - Versioning and distribution
- [Testing Guide](testing/index.md) - How to write and run tests
- [Overall Design](../design/overall.md) - Architecture overview