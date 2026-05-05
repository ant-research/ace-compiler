#!/bin/bash
# ============================================================
# ACE-FHE CUDA Dev Images Build Script
#
# Flow: opencc/base-cuda:* -> ace/dev-cuda:* (add system deps + Python + pip)
#
# Usage:
#   ./.aci/build-dev-cuda-images.sh
# ============================================================

set -euo pipefail

ACI_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(dirname "$ACI_DIR")"

# Registries
OPENCC_REPO="reg.docker.alibaba-inc.com/opencc"
ACE_REPO="reg.docker.alibaba-inc.com/ace"

# CUDA dev images: "base_image|dev_image|torch|torchvision|torchaudio|cu_version|torch_index_url"
DEV_IMAGES=(
    "${OPENCC_REPO}/base-cuda:12.4.0-devel-ubuntu22.04|${ACE_REPO}/dev-cuda:12.4.0-devel-ubuntu22.04|2.5.0|0.20.0|2.5.0|cu124|https://download.pytorch.org/whl/cu124"
    "${OPENCC_REPO}/base-cuda:12.5.0-devel-ubuntu22.04|${ACE_REPO}/dev-cuda:12.5.0-devel-ubuntu22.04|2.5.0|0.20.0|2.5.0|cu125|https://download.pytorch.org/whl/cu125"
    "${OPENCC_REPO}/base-cuda:12.6.0-devel-ubuntu24.04|${ACE_REPO}/dev-cuda:12.6.0-devel-ubuntu24.04|2.6.0|0.21.0|2.6.0|cu126|https://download.pytorch.org/whl/cu126"
)

echo "=============================================="
echo "Build CUDA Dev Images"
echo "=============================================="

# Create build context
BUILD_DIR=$(mktemp -d)
trap "rm -rf ${BUILD_DIR}" EXIT

# Copy Dockerfile and requirements
cp "${ROOT_DIR}/docker/Dockerfile.dev.cuda" "${BUILD_DIR}/Dockerfile"
cp "${ROOT_DIR}/docker/requirements-base.txt" "${BUILD_DIR}/"

# Process each image
for mapping in "${DEV_IMAGES[@]}"; do
    IFS='|' read -r base_image internal torch_ver tv_ver ta_ver cu_ver torch_index <<< "$mapping"

    echo ""
    echo "=== ${base_image} -> ${internal} ==="
    echo "  PyTorch: ${torch_ver}+${cu_ver}"

    # Pull if not local
    if ! docker image inspect "${base_image}" &>/dev/null; then
        echo "  Pulling base image..."
        docker pull "${base_image}"
    fi

    # Build dev image (no cache)
    docker build \
        --no-cache \
        --build-arg BASE_IMAGE="${base_image}" \
        --build-arg TORCH_VERSION="${torch_ver}" \
        --build-arg TORCHVISION_VERSION="${tv_ver}" \
        --build-arg TORCHAUDIO_VERSION="${ta_ver}" \
        --build-arg CU_VERSION="${cu_ver}" \
        --build-arg TORCH_INDEX_URL="${torch_index}" \
        -t "${internal}" \
        -f "${BUILD_DIR}/Dockerfile" \
        "${BUILD_DIR}"

    # Push
    docker push "${internal}"

    echo "  OK Done"
done

echo ""
echo "=============================================="
echo "Build completed!"
echo "=============================================="