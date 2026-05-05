# ACE-Compiler CI/CD Architecture

## Image Pipeline

```
External (Docker Hub)
  |                        |                        |
  | nvidia/cuda:*          | ubuntu:*               |
  v                        v
  import-base-cuda-images.sh   import-base-images.sh
  |                        |
  v                        v
opencc/base-cuda:*       opencc/base:ubuntu*
  |                        |
  v                        v
  build-dev-cuda-images.sh   build-dev-images.sh
  |                        |
  v                        v
ace/dev-cuda:*           ace/dev:ubuntu*
  |                        |
  +------------------------+
  |
  v
CI Pipeline  ->  Build / Test / Publish
```

## Image Layers

### Layer 1: Base (Manual Import)

External images with environment variables baked in.

#### GPU (CUDA)

| CUDA | Ubuntu | External Image | Internal Image |
|------|--------|---------------|----------------|
| 12.4 | 22.04 | `nvidia/cuda:12.4.0-devel-ubuntu22.04` | `opencc/base-cuda:12.4.0-devel-ubuntu22.04` |
| 12.5 | 22.04 | `nvidia/cuda:12.5.0-devel-ubuntu22.04` | `opencc/base-cuda:12.5.0-devel-ubuntu22.04` |
| 12.6 | 24.04 | `nvidia/cuda:12.6.0-devel-ubuntu24.04` | `opencc/base-cuda:12.6.0-devel-ubuntu24.04` |

#### CPU

| Ubuntu | External Image | Internal Image |
|--------|---------------|----------------|
| 20.04 | `ubuntu:20.04` | `opencc/base:ubuntu20.04` |
| 22.04 | `ubuntu:22.04` | `opencc/base:ubuntu22.04` |
| 24.04 | `ubuntu:24.04` | `opencc/base:ubuntu24.04` |

Baked-in environment variables:

| Variable | Value |
|----------|-------|
| `PIP_INDEX_URL` | `https://mirrors.aliyun.com/pypi/simple/` |
| `ARTIFACT_URL` | `https://artifacts.antgroup-inc.cn/artifact/repositories/simple-dev` |
| `CMAKE_BUILD_TYPE` | `Release` |
| `CI_TOKEN` | (passed at build time) |

### Layer 2: Dev (Script Build)

Base image + system dependencies + Python venv + (optional) PyTorch.

#### GPU (CUDA)

| Base Image | Dev Image | PyTorch |
|-----------|-----------|---------|
| `opencc/base-cuda:12.4.0-devel-ubuntu22.04` | `ace/dev-cuda:12.4.0-devel-ubuntu22.04` | torch 2.5.0 + cu124 |
| `opencc/base-cuda:12.5.0-devel-ubuntu22.04` | `ace/dev-cuda:12.5.0-devel-ubuntu22.04` | torch 2.5.0 + cu125 |
| `opencc/base-cuda:12.6.0-devel-ubuntu24.04` | `ace/dev-cuda:12.6.0-devel-ubuntu24.04` | torch 2.6.0 + cu126 |

#### CPU

| Base Image | Dev Image | PyTorch |
|-----------|-----------|---------|
| `opencc/base:ubuntu20.04` | `ace/dev:ubuntu20.04` | torch 2.5.0+cpu |
| `opencc/base:ubuntu22.04` | `ace/dev:ubuntu22.04` | torch 2.5.0+cpu |
| `opencc/base:ubuntu24.04` | `ace/dev:ubuntu24.04` | torch 2.6.0+cpu |

## Dockerfiles

| File | Scope | Purpose |
|------|-------|---------|
| `.aci/docker/Dockerfile.base` | Internal | Wrapper: adds ENV to external base image |
| `docker/Dockerfile.dev` | Shared | CPU dev image: system deps + Python + PyTorch CPU |
| `docker/Dockerfile.dev.cuda` | Shared | CUDA dev image: system deps + Python + PyTorch CUDA |

## Requirements

| File | Purpose |
|------|---------|
| `docker/requirements-base.txt` | Common Python deps (scikit-build-core, pybind11, cmake, etc.) |

## CI Systems

### Internal (AntGroup ACI)

- **Config**: `.aci.yml`
- **Registry**: `reg.docker.alibaba-inc.com/opencc` (base), `reg.docker.alibaba-inc.com/ace` (dev)
- **PyPI**: `https://artifacts.antgroup-inc.cn/artifact/repositories/simple-dev/`

### External (GitHub Actions)

- **Config**: `.github/workflows/build-wheels.yml`
- **Registry**: `ghcr.io/${{ github.repository }}`
- **PyPI**: `https://pypi.org/simple`

## Build Scripts

| Script | Purpose |
|--------|---------|
| `scripts/build-matrix.sh` | Build wheels for all CUDA combinations |
| `scripts/build-wheel.sh` | Build single wheel for specific CUDA version |

### Usage

```bash
# Build wheels for all CUDA versions
./scripts/build-matrix.sh

# Build wheel for specific CUDA version
./scripts/build-wheel.sh --cuda 12.4

# Dry run (show commands without building)
./scripts/build-wheel.sh --cuda 12.4 --dry-run

# Specify dev image registry (internal)
DEV_REGISTRY="reg.docker.alibaba-inc.com/ace/dev-cuda:" ./scripts/build-matrix.sh

# Specify specific CUDA versions
CUDA_VERSIONS="12.4 12.5" ./scripts/build-matrix.sh
```

## Build Flow

### Internal (ACI)

```
1. ./.aci/import-base-cuda-images.sh  -> opencc/base-cuda:*
   ./.aci/import-base-images.sh       -> opencc/base:ubuntu*
2. ./.aci/build-dev-cuda-images.sh    -> ace/dev-cuda:*
   ./.aci/build-dev-images.sh         -> ace/dev:ubuntu*
3. .aci.yml uses dev images to:
   a. Build C++ extension (Debug + Release)
   b. Run unit tests
   c. Run E2E tests
   d. Build wheels
   e. Publish to internal PyPI (master only)
```

### External (GitHub Actions)

```
1. Build base image
2. Build dev image (with PyTorch)
3. Build wheel for matrix combinations
4. Upload as artifacts
5. Publish to PyPI (on master)
```

## File Structure

```
.aci/
├── docker/
│   └── Dockerfile.base              # Base image wrapper (internal only)
├── import-base-cuda-images.sh       # Pull CUDA -> Build wrapper -> Push to opencc
├── import-base-images.sh            # Pull Ubuntu -> Build wrapper -> Push to opencc
├── build-dev-cuda-images.sh         # opencc/base-cuda -> ace/dev-cuda
├── build-dev-images.sh              # opencc/base -> ace/dev
└── README.md

docker/                              # Shared (internal + external)
├── Dockerfile.dev                   # CPU dev image (TORCH_VERSION build-arg)
├── Dockerfile.dev.cuda              # CUDA dev image (TORCH_VERSION + CU_VERSION build-args)
└── requirements-base.txt            # Common Python deps

scripts/
├── build-matrix.sh                  # Build wheels for all CUDA combinations
└── build-wheel.sh                   # Build single wheel

.aci.yml                             # Main CI pipeline
.github/
├── config/
│   └── env.sh                       # External CI environment variables
├── scripts/
│   ├── build-dev-cuda-images.sh     # Build CUDA dev images (external)
│   └── build-dev-images.sh          # Build CPU dev images (external)
└── workflows/
    └── build-wheels.yml             # GitHub Actions
```