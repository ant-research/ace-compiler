#!/bin/bash
# ============================================================
# ACE-Compiler CUDA Dev Images Build Script (External/GitHub)
#
# Flow: nvidia/cuda:* -> ace/dev-cuda:* (add system deps + Python + pip)
#
# Usage:
#   ./.github/scripts/build-dev-cuda-images.sh
#   CUDA_VERSIONS="cu124 cu125" ./.github/scripts/build-dev-cuda-images.sh
# ============================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$(dirname "$SCRIPT_DIR")/.." && pwd)"

# Default CUDA versions to build
CUDA_VERSIONS="${CUDA_VERSIONS:-cu124 cu125 cu126}"

# Version matrix: "base_image|dev_image|torch|torchvision|torchaudio|torch_index_url"
declare -A VERSION_MAP=(
    ["cu124"]="nvidia/cuda:12.4.0-devel-ubuntu22.04|ace/dev-cuda:12.4.0-devel-ubuntu22.04|2.5.0|0.20.0|2.5.0|https://download.pytorch.org/whl/cu124"
    ["cu125"]="nvidia/cuda:12.5.0-devel-ubuntu22.04|ace/dev-cuda:12.5.0-devel-ubuntu22.04|2.5.0|0.20.0|2.5.0|https://download.pytorch.org/whl/cu125"
    ["cu126"]="nvidia/cuda:12.6.0-devel-ubuntu24.04|ace/dev-cuda:12.6.0-devel-ubuntu24.04|2.6.0|0.21.0|2.6.0|https://download.pytorch.org/whl/cu126"
)

echo "=============================================="
echo "Build CUDA Dev Images (External)"
echo "=============================================="

# Create build context
BUILD_DIR=$(mktemp -d)
trap "rm -rf ${BUILD_DIR}" EXIT

# Copy Dockerfile and requirements
cp "${ROOT_DIR}/docker/Dockerfile.dev.cuda" "${BUILD_DIR}/Dockerfile"
cp "${ROOT_DIR}/docker/requirements-base.txt" "${BUILD_DIR}/"

# Process each CUDA version
for cu_ver in ${CUDA_VERSIONS}; do
    IFS='|' read -r base_image internal torch_ver tv_ver ta_ver torch_index <<< "${VERSION_MAP[$cu_ver]}"

    echo ""
    echo "=== ${base_image} -> ${internal} ==="
    echo "  PyTorch: ${torch_ver}+${cu_ver}"

    # Pull base image
    docker pull "${base_image}"

    # Build dev image (no cache)
    docker build \
        --no-cache \
        --build-arg BASE_IMAGE="${base_image}" \
        --build-arg PIP_INDEX_URL="https://pypi.org/simple" \
        --build-arg TORCH_VERSION="${torch_ver}" \
        --build-arg TORCHVISION_VERSION="${tv_ver}" \
        --build-arg TORCHAUDIO_VERSION="${ta_ver}" \
        --build-arg CU_VERSION="${cu_ver}" \
        --build-arg TORCH_INDEX_URL="${torch_index}" \
        -t "${internal}" \
        -f "${BUILD_DIR}/Dockerfile" \
        "${BUILD_DIR}"

    echo "  OK Done: ${internal}"
done

echo ""
echo "=============================================="
echo "Build completed!"
echo "=============================================="