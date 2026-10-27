# Developer Guide

This guide describes how to set up the development environment, build the project, and work with the codebase.

## Quick Start

This section gets you from a clean clone to passing tests in three steps:
**build → verify → smoke test**. If you are building inside the Ant Group
internal network (token, internal mirrors), follow
[.aci/ONBOARDING.md](../../.aci/ONBOARDING.md) instead.

### Docker (Fastest)

```bash
# Pull and run (CUDA image — used for all builds, GPU and CPU backends alike)
docker pull opencc/dev-cuda:12.8.0-devel-ubuntu24.04

# With a GPU: pass --gpus all so the container can see the CUDA device
docker run --gpus all -it opencc/dev-cuda:12.8.0-devel-ubuntu24.04 bash

# Without a GPU: building still works (CUDA toolkit is a compile-time only
# dependency), but drop --gpus all — there is no device to attach
docker run -it opencc/dev-cuda:12.8.0-devel-ubuntu24.04 bash

# Inside container
git clone https://github.com/ant-research/ace-compiler.git && cd ace-compiler
```

The CUDA dev images compile both GPU backends (Phantom/ACE) and CPU backends
(ANT/Seal/OpenFHE). The CUDA toolkit is a compile-time-only dependency, so a
CUDA image builds the CPU backends fine even on a host without a GPU. The
`--gpus all` run flag is only needed to actually execute GPU backends; build
and CPU-backend inference need no device.

Available images (see [Package Management](package.md) for the full list):

| Type | Tag | PyTorch |
|------|-----|---------|
| CUDA | `opencc/dev-cuda:12.8.0-devel-ubuntu24.04` | 2.11.0+cu128 |
| CUDA | `opencc/dev-cuda:13.0.0-devel-ubuntu24.04` | 2.11.0+cu130 |
| CUDA | `opencc/dev-cuda:13.1.0-devel-ubuntu24.04` | 2.11.0+cu130 (forward-compatible) |
| CPU | `opencc/dev:ubuntu24.04` | 2.11.0 |
| CPU | `opencc/dev:ubuntu22.04` | 2.5.1 |

> CUDA 13.1 hosts use cu130 PyTorch wheels (no cu131 wheel yet; cu130 is forward-compatible). CUDA dev images use PyTorch 2.11.0 on Ubuntu 24.04, so the image tag does not carry a torch suffix.

### Build → Verify → Smoke Test

Whether you are in Docker or on a local machine with the prerequisites below,
the three steps are the same:

```bash
# 1. Build + install to site-packages (Release by default)
./scripts/dev-build.sh
#   (or editable install:  pip install -e . --no-build-isolation)

# 2. Verify the package imports
python -c "from ace import frontend, runtime; print('OK')"

# 3. Run the minimal unit-test set (fast, no FHE runtime; skip GPU-only tests
#    on a host without a GPU)
pytest tests/unit/ -v -k "not cuda"
```

If step 2 prints `OK` and step 3 is green, your environment is ready. For
running GPU regression / benchmarks, see
[Testing Guide](testing.md) — those need a GPU, keys, and datasets.

### Local Setup

Prerequisites: Python 3.12+, CMake 3.28+, Ninja, C++17 compiler, CUDA Toolkit 12.8+ (for GPU backends)

```bash
pip install scikit-build-core torch pybind11 cmake PyYAML ninja
pip install -e . --no-build-isolation
python -c "from ace import frontend, runtime; print('OK')"
```

Then continue with the three steps in [Build → Verify → Smoke Test](#build--verify--smoke-test) above.

## Build & Install

### Development Build Script

```bash
./scripts/dev-build.sh                    # Build + install (Release by default)
./scripts/dev-build.sh --clean            # Clean and rebuild
./scripts/dev-build.sh --build-only       # Build only (no install)
./scripts/dev-build.sh --install-only     # Install from existing build
./scripts/dev-build.sh --prefix /path     # Custom install prefix

# Debug build
CMAKE_BUILD_TYPE=Debug ./scripts/dev-build.sh

# Enable unit tests
BUILD_UNITTEST=ON ./scripts/dev-build.sh
```

> **Tip:** For iterative C++ development, `./scripts/dev-build.sh` is preferred over `pip install -e .` because it supports incremental builds.

### Makefile Shortcuts

```bash
make                    # Build and install (default: Debug)
make build              # Build C++ extensions only
make install            # Install to site-packages
make rebuild            # Clean, build, and install
make quick-build        # Incremental rebuild (faster)
make lint               # ruff + clang-format check
make test-unit          # Run unit tests
```

### CMake Build (Manual)

```bash
# Configure
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_INSTALL_PREFIX=$(python -c "import sysconfig; print(sysconfig.get_path('purelib'))")

# Build
cmake --build build -j$(nproc)

# Install (all components)
cmake --install build --component frontend
cmake --install build --component core
cmake --install build --component runtime
cmake --install build --component library
```

### Build Type Defaults

| Method | Default | How to Override |
|--------|---------|-----------------|
| `dev-build.sh` | `Release` | `CMAKE_BUILD_TYPE=Debug ./scripts/dev-build.sh` |
| Makefile (`make`) | `Debug` | `make build CMAKE_BUILD_TYPE=Release` |
| `pip install` | `Release` | `--config-settings=cmake.define.CMAKE_BUILD_TYPE=Debug` |

## CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_UNITTEST` | `OFF` | Build C++ unit tests |
| `BUILD_BENCH` | `OFF` | Build C++ benchmarks |
| `BUILD_EXAMPLE` | `OFF` | Build examples |
| `WITH_PHANTOM` | `ON` | Build Phantom GPU backend |
| `WITH_ACELIB` | `ON` | Build ACE GPU backend |
| `BUILD_SHARED` | `ON` | Build shared libraries for rtlib |

```bash
# Example: build only Phantom backend, with unit tests
cmake -S . -B build -G Ninja \
  -DWITH_PHANTOM=ON -DWITH_ACELIB=OFF \
  -DBUILD_UNITTEST=ON
```

## CMake Dependencies

The project fetches several git repositories at configure time. The dependency system is centralized in `cmake/`:

| File | Purpose |
|------|---------|
| `cmake/AceDependencies.cmake` | All dependency URLs/refs; auto-detects internal vs external source |
| `cmake/AceFetchContent.cmake` | Unified `ace_fetch_content()` macro with local-source fallback |
| `cmake/AceDetect.cmake` | CUDA detection, ABI settings, PyTorch integration |
| `cmake/AceApplyPatches.cmake` | Reusable `ace_apply_patches()` for post-fetch patching |

### Dependency Registry

All `DEP_*` variables are defined in `cmake/AceDependencies.cmake`:

| Variable | Default (external) | Description |
|----------|--------------------|-------------|
| `DEP_AIR_URL` / `DEP_AIR_REF` | *(empty — needs `.aci/` or `-D`)* | air-infra (compiler backend) |
| `DEP_NN_URL` / `DEP_NN_REF` | *(empty — needs `.aci/` or `-D`)* | nn-addon (compiler backend) |
| `DEP_FHE_URL` / `DEP_FHE_REF` | *(empty — needs `.aci/` or `-D`)* | fhe-cmplr (compiler backend) |
| `DEP_PHANTOM_URL` / `DEP_PHANTOM_REF` | `github.com/ant-research/ace-phantom-fhe` | Phantom GPU backend |
| `DEP_HYPERFHE_URL` / `DEP_HYPERFHE_REF` | `github.com/ant-research/ace-ckks-infra` | ACE GPU backend |
| `DEP_SPDLOG_URL` / `DEP_SPDLOG_REF` | `github.com/gabime/spdlog` v1.15.1 | Logging library |

All variables use `CACHE STRING`, so they can be overridden on the command line:

```bash
cmake -DDEP_PHANTOM_REF=v1.3.0 -DDEP_FHE_URL=https://my-mirror/fhe-cmplr.git ...
```

### Internal vs External Source

`cmake/AceDependencies.cmake` auto-detects the source mode:

- **Internal** (`.aci/config-internal.cmake` exists): Uses internal git mirrors. The override file contains `set(DEP_*_URL ...)` calls that replace the defaults.
- **External** (no `.aci/` directory): Uses public GitHub URLs for the runtime backends (phantom, ckks-infra, spdlog). The compiler backend URLs (air-infra, nn-addon, fhe-cmplr) default to empty — in the public release these sources are **checked in directly** under `compiler/` (see [Local Source Override](#local-source-override) below), so no fetch is needed. **TBD:** if you are building from a tree where `compiler/{air-infra,nn-addon,fhe-cmplr}/` are absent, the compiler backend git URLs are not yet published; pass them via `-DDEP_<name>_URL=...` or restore the checked-in sources.

### Local Source Override

Each fetched dependency checks for a local source directory before cloning. If the directory exists with a `CMakeLists.txt`, the local copy is used directly:

```
compiler/air-infra/       → used instead of cloning DEP_AIR_URL
compiler/nn-addon/        → used instead of cloning DEP_NN_URL
compiler/fhe-cmplr/       → used instead of cloning DEP_FHE_URL
fhe_lib/ace/ace-library/  → used instead of cloning DEP_HYPERFHE_URL
fhe_lib/phantom/phantom/  → used instead of cloning DEP_PHANTOM_URL
```

This is useful for development: clone a dependency manually, make changes, and rebuild without touching the git ref.

### Architecture Overview

```
ace-compiler (this repo — frontend: Python → AIR IR)
  │
  ├── compiler/fetch_source.cmake    ← fetches air-infra, nn-addon, fhe-cmplr
  │   (compiler backend: AIR → FHE IR → C/CU)
  │
  ├── fhe_lib/ace/fetch_source.cmake     ← fetches ace-library (ckks-infra)
  ├── fhe_lib/phantom/fetch_source.cmake ← fetches phantom-fhe
  │   (GPU runtime backends: CKKS encryption/decryption)
  │
  └── fhe_dsl/csrc/CMakeLists.txt    ← fetches spdlog
      (C++ extension logging)
```

All `fetch_source.cmake` files use `ace_fetch_content()` from `cmake/AceFetchContent.cmake`, ensuring a consistent fetch pattern across the project.

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

## Testing

```bash
pytest tests/unit/ -v                     # Unit tests
pytest tests/integration/ -v              # Integration tests
pytest tests/regression/ -v               # Regression tests
CUDA_VISIBLE_DEVICES=0 pytest tests/unit  # Specify GPU
make test-unit                            # Via Makefile
```

Full test guide: [Testing Guide](testing.md)

## Debugging

### Runtime Library Issues

If `libFHErt_common.so not found`:

```bash
export LD_LIBRARY_PATH=$(python -c "import sysconfig; print(sysconfig.get_path('platlib'))")/ace/lib:$LD_LIBRARY_PATH
```

### CMake Configure Issues

Stale cache is the most common cause. Clean and reconfigure:

```bash
rm build/CMakeCache.txt
cmake -S . -B build -G Ninja
```

Or for a full clean: `rm -rf build && ./scripts/dev-build.sh`

## Code Style

- **Python:** Follow PEP 8, use type hints for public APIs
- **C++:** Formatting follows the LLVM coding style via `clang-format` (root `.clang-format`). Naming conventions are enforced by `clang-tidy` (`readability-identifier-naming`): `fhe_lib/` is governed by `fhe_lib/.clang-tidy` (Google-flavored custom rule set — CamelCase types, lower_case namespaces/locals, **private members use a leading underscore `_`**); `fhe_dsl/csrc/` follows the air-infra dialect via `compiler/air-infra/devtools/.clang-tidy`. Run `make lint` (clang-format) and `make lint-tidy` (clang-tidy, after a build) locally.

## Related Documentation

- [Package Management](package.md) - Package structure, wheel builds, Docker images
- [Testing Guide](testing.md) - How to write and run tests
- [Release Process](release.md) - Versioning and distribution
- [Overall Design](../design/overall.md) - Architecture overview
- [CLI Tools](../design/cli.md) - `ace_tool` auxiliary command-line tools
