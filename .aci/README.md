# ACE-Compiler Internal CI

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
.aci.yml  ->  Build / Test / Publish
```

## Scripts

### import-base-cuda-images.sh

Pull NVIDIA CUDA images, build wrapper with ENV, push to internal registry.

```bash
CI_TOKEN=xxx ./.aci/import-base-cuda-images.sh
```

### import-base-images.sh

Pull Ubuntu images, build wrapper with ENV, push to internal registry.

```bash
CI_TOKEN=xxx ./.aci/import-base-images.sh
```

### build-dev-cuda-images.sh

Build CUDA dev images on top of base images (adds system deps + Python + PyTorch).

```bash
./.aci/build-dev-cuda-images.sh
```

### build-dev-images.sh

Build CPU dev images on top of base images (adds system deps + Python + PyTorch CPU).

```bash
./.aci/build-dev-images.sh
```

## Image Naming

| Type | Layer | Registry | Pattern | Example |
|------|-------|----------|---------|---------|
| GPU | Base | opencc | `base-cuda:<cuda-tag>` | `opencc/base-cuda:12.4.0-devel-ubuntu22.04` |
| GPU | Dev | ace | `dev-cuda:<cuda-tag>` | `ace/dev-cuda:12.4.0-devel-ubuntu22.04` |
| CPU | Base | opencc | `base:ubuntu<ver>` | `opencc/base:ubuntu22.04` |
| CPU | Dev | ace | `dev:ubuntu<ver>` | `ace/dev:ubuntu22.04` |

## Supported Combinations

### GPU (CUDA)

| CUDA | Ubuntu | External Image | PyTorch |
|------|--------|---------------|---------|
| 12.4 | 22.04 | `nvidia/cuda:12.4.0-devel-ubuntu22.04` | torch 2.5.0 + cu124 |
| 12.5 | 22.04 | `nvidia/cuda:12.5.0-devel-ubuntu22.04` | torch 2.5.0 + cu125 |
| 12.6 | 24.04 | `nvidia/cuda:12.6.0-devel-ubuntu24.04` | torch 2.6.0 + cu126 |

### CPU

| Ubuntu | External Image | PyTorch |
|--------|---------------|---------|
| 20.04 | `ubuntu:20.04` | torch 2.5.0+cpu |
| 22.04 | `ubuntu:22.04` | torch 2.5.0+cpu |
| 24.04 | `ubuntu:24.04` | torch 2.6.0+cpu |

## File Structure

```
.aci/
├── docker/
│   └── Dockerfile.base              # Wrapper: adds ENV to external base image
├── import-base-cuda-images.sh       # Pull CUDA -> Build wrapper -> Push to opencc
├── import-base-images.sh            # Pull Ubuntu -> Build wrapper -> Push to opencc
├── build-dev-cuda-images.sh         # opencc/base-cuda -> ace/dev-cuda
├── build-dev-images.sh              # opencc/base -> ace/dev
└── README.md

docker/                              # Shared (internal + external)
├── Dockerfile.dev                   # CPU dev image (TORCH_VERSION build-arg)
├── Dockerfile.dev.cuda              # CUDA dev image (TORCH_VERSION + CU_VERSION build-args)
└── requirements-base.txt            # Common Python deps
```

## Environment Variables (baked into base image)

| Variable | Value |
|----------|-------|
| `PIP_INDEX_URL` | `https://mirrors.aliyun.com/pypi/simple/` |
| `ARTIFACT_URL` | `https://artifacts.antgroup-inc.cn/artifact/repositories/simple-dev` |
| `CMAKE_BUILD_TYPE` | `Release` |
| `CI_TOKEN` | (passed at build time) |