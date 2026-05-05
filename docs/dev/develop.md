# Developer Guide

This guide describes how to set up the development environment, build the project, and work with the codebase.

## Quick Start

### Prerequisites

- Python 3.12+
- CMake 3.28+
- Ninja build system
- C++17 compiler

### Initial Setup

```bash
# Clone the repository
git clone <repository-url>
cd ace

# Install build dependencies
pip install scikit-build-core torch pybind11 cmake PyYAML ninja

# Build and install
./scripts/dev-build.sh
```

### Verify Installation

```bash
python -c "from ace import frontend, runtime; print('OK')"
```

## Build Types

The project supports two build types with different defaults depending on the build method:

| Method | Default | How to Override |
|--------|---------|-----------------|
| `dev-build.sh` | `Debug` | `CMAKE_BUILD_TYPE=Release ./scripts/dev-build.sh` |
| Makefile (`make`) | `Debug` | `make build CMAKE_BUILD_TYPE=Release` |
| `pip install` | `Release` | `--config-settings=cmake.define.CMAKE_BUILD_TYPE=Debug` |
| CMake (manual) | `Debug` | `-DCMAKE_BUILD_TYPE=Release` |

- Development builds default to **Debug** for debugging symbols, assertions, and verbose logging.
- Use **Release** for performance benchmarks and production-like testing.

> **Note:** `pip install -e .` is currently not supported due to CMake 4.x + Ninja compatibility issues with FetchContent. Use `make build` or `./scripts/dev-build.sh` instead.

## Build Commands

### Makefile (Recommended)

```bash
make               # Build and install (default: Debug)
make build         # Build C++ extensions only
make install       # Install to site-packages
make rebuild       # Clean, build, and install
make clean         # Remove build directory

# Build with Release
make build CMAKE_BUILD_TYPE=Release
```

### Development Build Script

```bash
./scripts/dev-build.sh              # Build and install (Debug)
./scripts/dev-build.sh --clean      # Clean and rebuild
./scripts/dev-build.sh --build-only # Build only (no install)
./scripts/dev-build.sh --install-only # Install from existing build

# Release build
CMAKE_BUILD_TYPE=Release ./scripts/dev-build.sh

# Custom install prefix
./scripts/dev-build.sh --prefix /custom/path
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

## Development Workflow

### Modifying Python Code

Python code is in `fhe_dsl/ace/`. After modifications:

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

### Environment Variables

| Variable | Default | Description |
|----------|---------|-------------|
| `CMAKE_BUILD_TYPE` | `Debug` | Debug / Release / RelWithDebInfo / MinSizeRel |
| `BUILD_EXTENSION` | `true` | Build Python extension |
| `BUILD_TESTS` | `false` | Build C++ tests |
| `ENABLE_LIB` | `phantom` | FHE backend (phantom, hyperfhe, seal, openfhe) |
| `COMPILE_MODULE` | `fhe-cmplr` | Compiler module |

### Enable GPU Backends

```bash
ENABLE_LIB="hyperfhe;phantom" ./scripts/dev-build.sh
```

## Running Tests

See [testing.md](testing/index.md) for detailed testing guide.

```bash
pytest tests/test_unit/test_frontend/ -v
pytest tests/test_regression/ -v
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