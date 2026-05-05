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
export BASE_IMAGE_CUDA_124="nvidia/cuda:12.4.0-devel-ubuntu22.04"
export BASE_IMAGE_CUDA_125="nvidia/cuda:12.5.0-devel-ubuntu22.04"
export BASE_IMAGE_CUDA_126="nvidia/cuda:12.6.0-devel-ubuntu24.04"

# Build options
export CMAKE_BUILD_TYPE="Release"
export MAX_JOBS="${MAX_JOBS:-$(nproc)}"