# ACE-Compiler GitHub Actions CI

## Workflow: Build Wheels

**File**: `.github/workflows/build-wheels.yml`

### Flow

```
nvidia/cuda:* (Docker Hub)          ubuntu:* (Docker Hub)
  |                                   |
  v                                   v
  build-dev-cuda-images.sh           build-dev-images.sh
  |                                   |
  v                                   v
ace/dev-cuda:* (local)              ace/dev:ubuntu* (local)
  |                                   |
  v                                   v
  pip wheel                           pip wheel
  |                                   |
  v                                   v
dist/*.whl                           dist/*.whl
  |                                   |
  +-----------------------------------+
  |
  v
upload artifact -> publish to PyPI (master only)
```

### Supported Combinations

#### GPU (CUDA)

| CUDA | Ubuntu | Base Image | Dev Image |
|------|--------|-----------|-----------|
| cu124 | 22.04 | `nvidia/cuda:12.4.0-devel-ubuntu22.04` | `ace/dev-cuda:12.4.0-devel-ubuntu22.04` |
| cu125 | 22.04 | `nvidia/cuda:12.5.0-devel-ubuntu22.04` | `ace/dev-cuda:12.5.0-devel-ubuntu22.04` |
| cu126 | 24.04 | `nvidia/cuda:12.6.0-devel-ubuntu24.04` | `ace/dev-cuda:12.6.0-devel-ubuntu24.04` |

#### CPU

| Ubuntu | Base Image | Dev Image |
|--------|-----------|-----------|
| 20.04 | `ubuntu:20.04` | `ace/dev:ubuntu20.04` |
| 22.04 | `ubuntu:22.04` | `ace/dev:ubuntu22.04` |
| 24.04 | `ubuntu:24.04` | `ace/dev:ubuntu24.04` |

### Triggers

- **Push to master**: Build all configured combinations
- **Pull request**: Build primary combination only
- **Manual dispatch**: Select specific CUDA version

## Build Scripts

### build-dev-cuda-images.sh

Build CUDA dev images locally:

```bash
# Build all CUDA versions
./.github/scripts/build-dev-cuda-images.sh

# Build specific CUDA version
CUDA_VERSIONS="cu124" ./.github/scripts/build-dev-cuda-images.sh
```

### build-dev-images.sh

Build CPU dev images locally:

```bash
# Build all Ubuntu versions
./.github/scripts/build-dev-images.sh

# Build specific Ubuntu version
CPU_VERSIONS="ubuntu22.04" ./.github/scripts/build-dev-images.sh
```

### build-matrix.sh / build-wheel.sh

Build wheels using pre-built dev images:

```bash
# Build wheels for all CUDA versions
./scripts/build-matrix.sh

# Build wheel for specific CUDA version
./scripts/build-wheel.sh --cuda 12.4

# Dry run
./scripts/build-wheel.sh --cuda 12.4 --dry-run
```

## Local Testing

```bash
# Build GPU dev image
docker build --no-cache \
  --build-arg BASE_IMAGE=nvidia/cuda:12.4.0-devel-ubuntu22.04 \
  --build-arg PIP_INDEX_URL=https://pypi.org/simple \
  --build-arg TORCH_VERSION=2.5.0 \
  --build-arg TORCHVISION_VERSION=0.20.0 \
  --build-arg TORCHAUDIO_VERSION=2.5.0 \
  --build-arg CU_VERSION=cu124 \
  --build-arg TORCH_INDEX_URL=https://download.pytorch.org/whl/cu124 \
  -f docker/Dockerfile.dev.cuda \
  -t ace/dev-cuda:12.4.0-devel-ubuntu22.04 .

# Build CPU dev image
docker build --no-cache \
  --build-arg BASE_IMAGE=ubuntu:22.04 \
  --build-arg PIP_INDEX_URL=https://pypi.org/simple \
  --build-arg TORCH_VERSION=2.5.0 \
  --build-arg TORCHVISION_VERSION=0.20.0 \
  --build-arg TORCHAUDIO_VERSION=2.5.0 \
  -f docker/Dockerfile.dev \
  -t ace/dev:ubuntu22.04 .

# Build wheel
docker run --rm -v $(pwd):/app ace/dev-cuda:12.4.0-devel-ubuntu22.04 \
  pip wheel . -w /app/dist --no-build-isolation
```

## Differences from Internal ACI

| Aspect | GitHub Actions | Internal ACI |
|--------|----------------|--------------|
| Registry | `ghcr.io` | `reg.docker.alibaba-inc.com` |
| PyPI | `pypi.org` | Internal artifact registry |
| GPU Base Image | `nvidia/cuda:*` (public) | `opencc/base-cuda:*` (with ENV) |
| CPU Base Image | `ubuntu:*` (public) | `opencc/base:ubuntu*` (with ENV) |
| Dev Image | Built on-demand | Pre-built by `build-dev-*-images.sh` |
| Config | `.github/workflows/` | `.aci.yml` |
| Dockerfile (GPU) | `docker/Dockerfile.dev.cuda` | Same (shared) |
| Dockerfile (CPU) | `docker/Dockerfile.dev` | Same (shared) |
| Requirements | `docker/requirements-*.txt` | Same (shared) |