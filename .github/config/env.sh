#!/bin/bash
# ============================================================
# ACE-Compiler GitHub Actions Environment Configuration
# External/Public CI settings
# ============================================================

# PyPI index (official PyPI)
export PIP_INDEX_URL="https://pypi.org/simple"

# Docker registry (GitHub Container Registry)
export DOCKER_REGISTRY="ghcr.io/${GITHUB_REPOSITORY:-ant-group/ace-fhe}"

# Base images (official NVIDIA CUDA images)
export BASE_IMAGE_CUDA_128="nvidia/cuda:12.8.0-devel-ubuntu24.04"
export BASE_IMAGE_CUDA_130="nvidia/cuda:13.0.0-devel-ubuntu24.04"
export BASE_IMAGE_CUDA_131="nvidia/cuda:13.1.0-devel-ubuntu24.04"

# Build options
export CMAKE_BUILD_TYPE="Release"
export MAX_JOBS="${MAX_JOBS:-$(nproc)}"
