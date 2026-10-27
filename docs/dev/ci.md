# ACE CI/CD

This document describes the CI/CD architecture for ACE, covering both the
internal AntGroup ACI pipeline and the external GitHub Actions workflow.

## Image Pipeline

```
External (Docker Hub)
  |                        |
  | nvidia/cuda:*          | ubuntu:*
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

Dev images are built directly from official `nvidia/cuda` / `ubuntu` images.
Environment variables (timezone, pip mirror, CMake build type, artifact URL)
are set in the Dockerfiles, not in an intermediate base layer.

> `CI_TOKEN` is **not** embedded in images. Inject at runtime via
> `docker run -e CI_TOKEN=xxx` or CI pipeline secret injection.

## Supported Image Combinations

### GPU (CUDA)

| CUDA | Ubuntu | PyTorch | CU Version | Docker Hub | Internal |
|------|--------|---------|------------|------------|----------|
| 13.1 | 24.04 | 2.11.0 | cu130 | `opencc/dev-cuda:13.1.0-devel-ubuntu24.04` | `reg.docker.alibaba-inc.com/ace/dev-cuda:13.1.0-devel-ubuntu24.04` |
| 13.0 | 24.04 | 2.11.0 | cu130 | `opencc/dev-cuda:13.0.0-devel-ubuntu24.04` | `reg.docker.alibaba-inc.com/ace/dev-cuda:13.0.0-devel-ubuntu24.04` |
| 12.8 | 24.04 | 2.11.0 | cu128 | `opencc/dev-cuda:12.8.0-devel-ubuntu24.04` | `reg.docker.alibaba-inc.com/ace/dev-cuda:12.8.0-devel-ubuntu24.04` |

> CUDA 13.1 hosts use cu130 (no cu131 PyTorch wheel yet; cu130 is forward-compatible).
> CUDA 12.8 hosts use cu128 (matched PyTorch wheel available).

### CPU

| Ubuntu | PyTorch | Docker Hub | Internal |
|--------|---------|------------|----------|
| 24.04 | 2.11.0 | `opencc/dev:ubuntu24.04` | `reg.docker.alibaba-inc.com/ace/dev:ubuntu24.04` |
| 22.04 | 2.5.1 | `opencc/dev:ubuntu22.04` | `reg.docker.alibaba-inc.com/ace/dev:ubuntu22.04` |

## Image Environment

The following are baked into dev images at build time:

| Variable | Default (Internal) | Default (External) | Description |
|----------|--------------------|--------------------|-------------|
| `PIP_INDEX_URL` | `https://mirrors.aliyun.com/pypi/simple/` | `https://pypi.org/simple` | PyPI mirror |
| `ARTIFACT_URL` | `https://artifacts.antgroup-inc.cn/...` | *(not set)* | Internal artifact repository |
| `CMAKE_BUILD_TYPE` | `Release` | `Release` | Default CMake build type |
| `TZ` | `Asia/Shanghai` | `Asia/Shanghai` | Timezone |
| `CI_TOKEN` | *(not set)* | *(not set)* | Inject at runtime for internal git repos |

## Dockerfiles

| File | Purpose |
|------|---------|
| `docker/Dockerfile.dev` | CPU dev image (system deps + Python + PyTorch CPU) |
| `docker/Dockerfile.dev.cuda` | CUDA dev image (system deps + Python + PyTorch CUDA) |
| `docker/requirements-base.txt` | Common Python deps (scikit-build-core, pybind11, cmake, ruff, pytest, etc.) |

### Build Arguments

Both Dockerfiles accept:

| Argument | Default | Description |
|----------|---------|-------------|
| `BASE_IMAGE` | *(required)* | Base Docker image (`nvidia/cuda:*` or `ubuntu:*`) |
| `PIP_INDEX_URL` | `https://pypi.org/simple` | PyPI mirror URL |
| `ARTIFACT_URL` | `https://artifacts.antgroup-inc.cn/...` | Internal artifact URL |
| `TORCH_VERSION` | `2.11.0` | PyTorch version |
| `TORCHVISION_VERSION` | `0.26.0` | TorchVision version |
| `TORCHAUDIO_VERSION` | `2.11.0` | TorchAudio version |
| `CU_VERSION` | `cu128` | PyTorch wheel CUDA tag (CUDA image only) |
| `TORCH_INDEX_URL` | `https://download.pytorch.org/whl/cu128` | PyTorch wheel index URL |

## CI Systems

### Internal (AntGroup ACI)

- **Config**: `.aci.yml`
- **Registry**: `reg.docker.alibaba-inc.com/ace`
- **PyPI**: `https://artifacts.antgroup-inc.cn/artifact/repositories/simple-dev/`
- **Build scripts**: `.aci/build-dev-cuda-images.sh`, `.aci/build-dev-images.sh`

### External (GitHub Actions)

- **Config**: `.github/workflows/`
- **Registry**: Docker Hub (`opencc/`)
- **PyPI**: `https://pypi.org/simple`
- **Build scripts**: `.github/scripts/build-dev-cuda-images.sh`, `.github/scripts/build-dev-images.sh`

### Differences

| Aspect | GitHub Actions | Internal ACI |
|--------|----------------|--------------|
| Registry | Docker Hub (`opencc/`) | `reg.docker.alibaba-inc.com/ace` |
| Image tag suffix | *(none)* | *(none)* |
| PyPI | `pypi.org` | Internal artifact registry |
| PIP_INDEX_URL | `https://pypi.org/simple` | `https://mirrors.aliyun.com/pypi/simple/` |
| GPU base image | `nvidia/cuda:*` (public) | `nvidia/cuda:*` (public, pulled directly) |
| Dev image | Built & pushed to Docker Hub by workflow | Built & pushed by `.aci/build-dev-*.sh` |
| CI_TOKEN | N/A | Injected at runtime via environment variable |

## Open-Source Sync (Internal to GitHub)

The internal repository and the public GitHub repository differ in content,
not just CI. A dedicated **export/sync** step produces the GitHub tree from
the internal tree, rather than committing to both independently.

### Why a separate sync

Two sets of files must differ between the two repos:

| Content | Internal repo | Public GitHub repo |
|---------|---------------|---------------------|
| `.aci/` | Kept — internal onboarding, image-build scripts, `config-internal.cmake` (internal mirrors + `CI_TOKEN`) | **Excluded** — no internal URLs/tokens leak |
| `compiler/air-infra/`, `compiler/nn-addon/`, `compiler/fhe-cmplr/` | Fetched at build time (`.gitignore`d, local-only) | **Checked in directly** — external builds need no fetch |

Keeping the internal repo as the single source of truth and generating the
public tree on publish satisfies all three constraints: the internal repo
keeps `.aci/`; the public repo never sees it; and merges flow GitHub →
internal only over files that exist in both, so `.aci/` never conflicts.

### Mechanism (design)

A publish-time script (`scripts/sync-to-github.sh`, **TBD**) performs:

1. Clone the internal repo to a clean working tree.
2. Strip `.aci/` (and any other internal-only paths) from the tree.
3. Inject the compiler backend sources (`air-infra`, `nn-addon`,
   `fhe-cmplr`) into `compiler/` from their internal mirror at the pinned
   refs, so the public tree is self-contained for source builds.
4. Force-push the resulting tree to `github.com/ant-research/ace-compiler`.

### Merge direction

- **GitHub → internal**: always safe. The internal repo has everything the
  public repo has, plus `.aci/`. Pull public changes and merge; `.aci/` is
  untouched on both sides.
- **Internal → GitHub**: only via the sync script. Never `git push` the
  internal repo directly to GitHub.

> **TBD:** the sync script itself, and whether it runs on demand or is
> triggered automatically by internal CI on push to the default branch.

## Build Flow

### Internal (ACI)

```
1. ./.aci/build-dev-cuda-images.sh    -> ace/dev-cuda:*
   ./.aci/build-dev-images.sh          -> ace/dev:ubuntu*
2. .aci.yml uses dev images to:
   a. Build C++ extension (Debug + Release)
   b. Run unit tests
   c. Run E2E tests
   d. Build wheels
   e. Publish to internal PyPI (master only)
```

### External (GitHub Actions)

**Triggers:**

- **Push to master**: Build all configured combinations
- **Pull request**: Build primary combination only
- **Manual dispatch**: Select specific CUDA version

**Flow:**

```
nvidia/cuda:* (Docker Hub)          ubuntu:* (Docker Hub)
  |                                   |
  v                                   v
  build-dev-cuda-images.sh           build-dev-images.sh
  |                                   |
  v                                   v
opencc/dev-cuda:* (Docker Hub)      opencc/dev:* (Docker Hub)
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

## Build Scripts

| Script | Scope | Purpose |
|--------|-------|---------|
| `.aci/build-dev-cuda-images.sh` | Internal | Build & push CUDA dev images to internal registry |
| `.aci/build-dev-images.sh` | Internal | Build & push CPU dev images to internal registry |
| `.github/scripts/build-dev-cuda-images.sh` | External | Build CUDA dev images for GitHub Actions |
| `.github/scripts/build-dev-images.sh` | External | Build CPU dev images for GitHub Actions |
| `scripts/build-matrix.sh` | Shared | Build wheels for all CUDA combinations |
| `scripts/build-wheel.sh` | Shared | Build single wheel for specific CUDA version |

### Usage

```bash
# --- Internal: build dev images ---

# Build all internal CUDA dev images (pip mirror defaults to Aliyun)
./.aci/build-dev-cuda-images.sh

# Build all internal CPU dev images
./.aci/build-dev-images.sh

# Override pip mirror
PIP_INDEX_URL=https://pypi.org/simple ./.aci/build-dev-cuda-images.sh

# --- External: build dev images locally ---

# Build all external CUDA dev images
./.github/scripts/build-dev-cuda-images.sh

# Build specific CUDA version
CUDA_VERSIONS="cu128 cu130" ./.github/scripts/build-dev-cuda-images.sh

# Build all external CPU dev images
./.github/scripts/build-dev-images.sh

# --- Build wheels ---

# Build wheels for all CUDA versions
./scripts/build-matrix.sh

# Build wheel for a specific CUDA version
./scripts/build-wheel.sh --cuda cu128

# Dry run
./scripts/build-wheel.sh --cuda cu128 --dry-run
```

### Local Testing

```bash
# Build GPU dev image manually
docker build --no-cache \
  --build-arg BASE_IMAGE=nvidia/cuda:12.8.0-devel-ubuntu24.04 \
  --build-arg TORCH_VERSION=2.11.0 \
  --build-arg TORCHVISION_VERSION=0.26.0 \
  --build-arg TORCHAUDIO_VERSION=2.11.0 \
  --build-arg CU_VERSION=cu128 \
  --build-arg TORCH_INDEX_URL=https://download.pytorch.org/whl/cu128 \
  --build-arg PIP_INDEX_URL=https://pypi.org/simple \
  -f docker/Dockerfile.dev.cuda \
  -t ace/dev-cuda:12.8.0-devel-ubuntu24.04 .

# Build CPU dev image manually (Ubuntu 24.04 / torch 2.11.0)
docker build --no-cache \
  --build-arg BASE_IMAGE=ubuntu:24.04 \
  --build-arg TORCH_VERSION=2.11.0 \
  --build-arg TORCHVISION_VERSION=0.26.0 \
  --build-arg TORCHAUDIO_VERSION=2.11.0 \
  --build-arg PIP_INDEX_URL=https://pypi.org/simple \
  -f docker/Dockerfile.dev \
  -t ace/dev:ubuntu24.04 .

# Build CPU dev image manually (Ubuntu 22.04 / torch 2.5.1)
docker build --no-cache \
  --build-arg BASE_IMAGE=ubuntu:22.04 \
  --build-arg TORCH_VERSION=2.5.1 \
  --build-arg TORCHVISION_VERSION=0.20.1 \
  --build-arg TORCHAUDIO_VERSION=2.5.1 \
  --build-arg PIP_INDEX_URL=https://pypi.org/simple \
  -f docker/Dockerfile.dev \
  -t ace/dev:ubuntu22.04 .

# Build wheel inside container
docker run --rm -v $(pwd):/app ace/dev-cuda:12.8.0-devel-ubuntu24.04 \
  pip wheel . -w /app/dist --no-build-isolation
```

## File Structure

```
.aci/
├── build-dev-cuda-images.sh         # nvidia/cuda -> ace/dev-cuda (internal)
├── build-dev-images.sh              # ubuntu -> ace/dev (internal)
├── config-internal.cmake            # Internal dependency URLs (uses $ENV{CI_TOKEN})
└── BUILD.md                         # Internal build script quick reference

docker/                              # Shared (internal + external)
├── Dockerfile.dev                   # CPU dev image
├── Dockerfile.dev.cuda              # CUDA dev image
└── requirements-base.txt            # Common Python deps

scripts/
├── build-matrix.sh                  # Build wheels for all CUDA combinations
└── build-wheel.sh                   # Build single wheel

.github/
├── config/
│   └── env.sh                       # External CI environment variables
├── scripts/
│   ├── build-dev-cuda-images.sh     # Build CUDA dev images (external)
│   └── build-dev-images.sh          # Build CPU dev images (external)
└── workflows/
    ├── build-dev-image-cuda.yml     # Build & push CUDA images to Docker Hub
    ├── build-dev-image-cpu.yml      # Build & push CPU images to Docker Hub
    └── build-wheels.yml             # Build & publish wheels

.aci.yml                             # Main CI pipeline (internal)
```
