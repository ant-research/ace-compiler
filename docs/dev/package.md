# Package Management

This document describes how to install, build, and use the ACE FHE package.

## Quick Start

```bash
# Install from PyPI
pip install ace-fhe

# Verify installation
python -c "import ace; print(ace.__version__)"
```

## Installation

### From PyPI (Recommended)

```bash
pip install ace-fhe
```

### From GitHub Release (TBD)

```bash
# Install specific version
pip install https://github.com/ant-research/ace-compiler/releases/download/v0.2.2/ace_fhe-0.2.2-cp310-cp310-linux_x86_64.whl

# Install latest from main branch
pip install --extra-index-url https://ant-research.github.io/ace-compiler/ ace-fhe
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
| CUDA | `opencc/dev-cuda:12.8.0-devel-ubuntu24.04` | 2.11.0+cu128 | CUDA 12.8.0 |
| CUDA | `opencc/dev-cuda:13.0.0-devel-ubuntu24.04` | 2.11.0+cu130 | CUDA 13.0.0 |
| CUDA | `opencc/dev-cuda:13.1.0-devel-ubuntu24.04` | 2.11.0+cu130 | CUDA 13.1.0 (forward-compatible) |
| CPU | `opencc/dev:ubuntu22.04` | 2.5.1 | Ubuntu 22.04 |
| CPU | `opencc/dev:ubuntu24.04` | 2.11.0 | Ubuntu 24.04 |

Note: CUDA 13.1 uses cu130 PyTorch wheels (no cu131 wheel is published yet; cu130 is forward-compatible with CUDA 13.1). All dev images use the same baseline PyTorch on Ubuntu 24.04, so the CUDA image tag does not carry a torch suffix.

#### Wheel Naming

```bash
# CPU wheels
ACE_LOCAL_VERSION=cpu
# Output: ace_fhe-0.2.2+cpu-cp310-cp310-linux_x86_64.whl

# CUDA wheels
ACE_LOCAL_VERSION=cu128  # CUDA 12.8.0
ACE_LOCAL_VERSION=cu130  # CUDA 13.0.0
ACE_LOCAL_VERSION=cu131  # CUDA 13.1.0 (uses cu130 PyTorch wheels)
# Output: ace_fhe-0.2.2+cu128-cp310-cp310-linux_x86_64.whl
```

The local version tag format is `+cpu` or `+cuXXX` where `XXX` is the CUDA major/minor version without dots (e.g., `cu128` for CUDA 12.8.0).

#### Version Selection Rationale

We track a single PyTorch version (2.11.0) across all CUDA images and offer two CPU images for different Ubuntu baselines. CUDA coverage is tiered by CUDA release:

| CUDA | PyTorch | CU Wheel | Notes |
|------|---------|----------|-------|
| 12.8.0 | 2.11.0 | cu128 | Previous CUDA release |
| 13.0.0 | 2.11.0 | cu130 | Current CUDA release |
| 13.1.0 | 2.11.0 | cu130 | Latest; forward-compatible (no cu131 wheel yet) |

Rules:
- One PyTorch minor version across all CUDA images (here 2.11.0), pinned to whichever cuXXX wheel exists
- Each CUDA image must have a matching PyTorch wheel on the official index; CUDA releases without a dedicated wheel (e.g., 13.1) reuse the nearest forward-compatible wheel (cu130)
- CPU images pin to the OS-version-appropriate PyTorch (2.5.1 on Ubuntu 22.04 LTS, 2.11.0 on Ubuntu 24.04)
- When a new CUDA or PyTorch minor stabilizes, tiers rotate: previous becomes old-stable, current becomes previous

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
print(ace.__version__)  # e.g., "0.2.2"
```

### High-Level API

```python
from ace import fhe

@fhe.compile(frontend="torch", library="ant", device="cpu")
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
    library="ant",
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

## Build Dependencies

### C++ Build Dependencies

| Package | Version | Purpose |
|---------|---------|---------|
| `cmake` | >=3.28 | Build system |
| `ninja` | any | Build backend (recommended) |
| `pybind11` | >=2.10 | C++ extension binding |
| `torch` | >=2.0 | Required for extension build |
| `scikit-build-core` | >=0.11 | Python build backend |
| CUDA Toolkit | 12.8+ | GPU backends (phantom, acelib) |

### Git Dependencies (fetched at configure time)

The project fetches several git repositories during CMake configure. All URLs and refs are defined in `cmake/AceDependencies.cmake`:

| Repository | Variable | Description |
|------------|----------|-------------|
| air-infra | `DEP_AIR_URL` | Compiler backend (AIR IR layer) |
| nn-addon | `DEP_NN_URL` | Compiler backend (NN vectorization) |
| fhe-cmplr | `DEP_FHE_URL` | Compiler backend (FHE lowering) |
| phantom-fhe | `DEP_PHANTOM_URL` | GPU runtime backend (Phantom) |
| ace-library | `DEP_HYPERFHE_URL` | GPU runtime backend (ACE) |
| spdlog | `DEP_SPDLOG_URL` | Logging library |

Internal builds use `.aci/config-internal.cmake` to override URLs with internal mirrors. See [develop.md](develop.md#cmake-dependencies) for details.

> In the **public release**, the compiler backends (`air-infra`,
> `nn-addon`, `fhe-cmplr`) are checked in directly under `compiler/` and
> not fetched at configure time. The runtime backends (`phantom`,
> `ace-library`, `spdlog`) are still fetched from public GitHub. See
> [ci.md](ci.md#open-source-sync-internal-to-github) for how the public tree
> is produced.

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
__version__ = "0.2.2"
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
- [Testing Guide](testing.md) - How to write and run tests
- [Overall Design](../design/overall.md) - Architecture overview
