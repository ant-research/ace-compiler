# Developer Guide

This guide describes how to set up the development environment, build the project, and work with the codebase.

## Quick Start

### Docker (Fastest)

```bash
# Pull and run (CPU)
docker pull opencc/dev:ubuntu22.04-torch2.5.1
docker run -it opencc/dev:ubuntu22.04-torch2.5.1

# Pull and run (CUDA)
docker pull opencc/dev-cuda:12.4.0-devel-ubuntu22.04-torch2.5.1
docker run --gpus all -it opencc/dev-cuda:12.4.0-devel-ubuntu22.04-torch2.5.1

# Inside container
git clone https://github.com/ant-research/ace-compiler.git && cd /app/ace-compiler
pip install -e . --no-build-isolation
python -c "from ace import frontend, runtime; print('OK')"
```

Available images:

### GPU (CUDA)

| CUDA | Ubuntu | External Image | PyTorch |
|------|--------|---------------|---------|
| 13.0 | 24.04 | `opencc/dev-cuda:13.0.0-devel-ubuntu24.04-torch2.11.0` | 2.11.0 |
| 12.6 | 24.04 | `opencc/dev-cuda:12.6.0-devel-ubuntu24.04-torch2.6.0` | 2.6.0 |
| 12.4 | 22.04 | `opencc/dev-cuda:12.4.0-devel-ubuntu22.04-torch2.5.1` | 2.5.1 |

### CPU

| Ubuntu | External Image | PyTorch |
|--------|---------------|---------|
| 24.04 | `opencc/dev:ubuntu24.04-torch2.11.0` | 2.11.0 |
| 24.04 | `opencc/dev:ubuntu24.04-torch2.6.0` | 2.6.0 |
| 22.04 | `opencc/dev:ubuntu22.04-torch2.5.1` | 2.5.1 |

### Local Setup

Prerequisites: Python 3.10+, CMake 3.28+, Ninja, C++17 compiler

```bash
pip install scikit-build-core torch pybind11 cmake PyYAML ninja
pip install -e . --no-build-isolation
python -c "from ace import frontend, runtime; print('OK')"
```

## Build & Install

### Editable Install (Recommended for Development)

Editable install lets Python code changes take effect immediately without reinstalling. For C++ changes, use `dev-build.sh` for incremental rebuilds.

```bash
# Initial setup: editable install
pip install -e . --no-build-isolation

# After Python-only changes: no rebuild needed

# After C++ changes: incremental rebuild (editable mode picks up the new .so)
./scripts/dev-build.sh
```

> **Note:** `pip install -e .` does a full rebuild each time. For iterative C++ development, prefer `./scripts/dev-build.sh` which supports incremental builds.

### Development Build Script

```bash
./scripts/dev-build.sh                    # Build and install (Release by default)
./scripts/dev-build.sh --clean            # Clean and rebuild
./scripts/dev-build.sh --build-only       # Build only (no install)
./scripts/dev-build.sh --install-only     # Install from existing build

# Debug build
CMAKE_BUILD_TYPE=Debug ./scripts/dev-build.sh

# Custom install prefix
./scripts/dev-build.sh --prefix /custom/path
```

### Makefile

```bash
make                    # Build and install (default: Debug)
make build              # Build C++ extensions only
make install            # Install to site-packages
make rebuild            # Clean, build, and install
make clean              # Remove build directory
make quick-build        # Incremental rebuild (faster)
make check-install      # Verify installation

# Build with Release
make build CMAKE_BUILD_TYPE=Release
```

### CMake Build (Manual)

```bash
# Configure
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_INSTALL_PREFIX=$(python -c "import sysconfig; print(sysconfig.get_path('purelib'))") \
  -DBUILD_EXTENSION=ON

# Build & Install
cmake --build build -j$(nproc)
cmake --install build --component frontend
cmake --install build --component core
cmake --install build --component runtime
```

### Build Type Defaults

| Method | Default | How to Override |
|--------|---------|-----------------|
| `dev-build.sh` | `Release` | `CMAKE_BUILD_TYPE=Debug ./scripts/dev-build.sh` |
| Makefile (`make`) | `Debug` | `make build CMAKE_BUILD_TYPE=Release` |
| `pip install` | `Release` | `--config-settings=cmake.define.CMAKE_BUILD_TYPE=Debug` |
| CMake (manual) | `Debug` | `-DCMAKE_BUILD_TYPE=Release` |

## Development Workflow

### Modifying Python Code

Python code is in `fhe_dsl/python/`. After modifications:

```bash
./scripts/dev-build.sh
```

### Modifying C++ Code

C++ extensions are in `fhe_dsl/csrc/`. After modifications:

```bash
# Incremental rebuild
cmake --build build -j$(nproc)
cmake --install build --component frontend
cmake --install build --component core
cmake --install build --component runtime

# Full rebuild
./scripts/dev-build.sh --clean
```

## Configuration

### Environment Variables

| Variable | Default | Description |
|----------|---------|-------------|
| `CMAKE_BUILD_TYPE` | `Debug` | Debug / Release / RelWithDebInfo / MinSizeRel |
| `BUILD_EXTENSION` | `true` | Build Python extension |
| `BUILD_TESTS` | `false` | Build C++ tests |
| `ENABLE_LIB` | `phantom` | FHE backend (phantom, acelib, seal, openfhe) |
| `COMPILE_MODULE` | `fhe-cmplr` | Compiler module |

### Enable GPU Backends

```bash
ENABLE_LIB="acelib;phantom" ./scripts/dev-build.sh
```

## Docker

### Pull Pre-built Images

See [Quick Start](#docker-fastest) for available images and basic usage.

The pre-built images are hosted on Docker Hub (`opencc/`). For internal mirrors, replace `opencc/` with your registry prefix.

### Build Custom Images

If you need a custom configuration, build from Dockerfile:

```bash
# CPU
docker build --build-arg BASE_IMAGE=ubuntu:22.04 \
             -f docker/Dockerfile.dev -t ace/dev:cpu .

# CUDA
docker build --build-arg BASE_IMAGE=nvidia/cuda:12.4.0-devel-ubuntu22.04 \
             --build-arg CU_VERSION=cu124 \
             -f docker/Dockerfile.dev.cuda -t ace/dev:cuda .
```

Build arguments:

| Argument | Default | Description |
|----------|---------|-------------|
| `BASE_IMAGE` | - | Base Docker image |
| `TORCH_VERSION` | `2.5.0` | PyTorch version |
| `CU_VERSION` | `cu124` | PyTorch wheel CUDA version (`cu118`, `cu121`, `cu124`, `cu126`) |
| `PIP_INDEX_URL` | `https://pypi.org/simple` | pip mirror URL |

## Testing

```bash
pytest tests/unit/ -v                   # Unit tests
pytest tests/unit/frontend/ -v          # Frontend tests
pytest tests/regression/ -v             # Regression tests
pytest tests/integration/ -v            # Integration tests
```

## Debugging

### Runtime Library Issues

If `libFHErt_common.so not found`:

```bash
export LD_LIBRARY_PATH=$(python -c "import sysconfig; print(sysconfig.get_path('platlib'))")/ace/lib:$LD_LIBRARY_PATH
```

### CMake Issues

```bash
rm -rf build/CMakeCache.txt
cmake -S . -B build -G Ninja
```

## Code Style

- **Python:** Follow PEP 8, use type hints for public APIs
- **C++:** Follow LLVM coding style, use `clang-format` for formatting

## Related Documentation

- [Package Management](package.md) - Package structure and dependencies
- [Testing Guide](testing/index.md) - How to write and run tests
- [Release Process](release.md) - Versioning and distribution
- [Overall Design](../design/overall.md) - Architecture overview
- [CLI Tools](../design/cli.md) - `ace_tool` auxiliary command-line tools