#!/bin/bash
# ============================================================
# ACE-FHE CPU Dev Images Build Script
#
# Flow: opencc/base:ubuntu* -> ace/dev:ubuntu* (add system deps + Python + pip)
#
# Usage:
#   ./.aci/build-dev-images.sh
# ============================================================

set -euo pipefail

ACI_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(dirname "$ACI_DIR")"

# Registries
OPENCC_REPO="reg.docker.alibaba-inc.com/opencc"
ACE_REPO="reg.docker.alibaba-inc.com/ace"

# CPU dev images: "base_image|dev_image|torch|torchvision|torchaudio"
DEV_IMAGES=(
    # "${OPENCC_REPO}/base:ubuntu20.04|${ACE_REPO}/dev:ubuntu20.04|2.5.0|0.20.0|2.5.0"
    "${OPENCC_REPO}/base:ubuntu22.04|${ACE_REPO}/dev:ubuntu22.04|2.5.0|0.20.0|2.5.0"
    "${OPENCC_REPO}/base:ubuntu24.04|${ACE_REPO}/dev:ubuntu24.04|2.6.0|0.21.0|2.6.0"
)

echo "=============================================="
echo "Build CPU Dev Images"
echo "=============================================="

# Create build context
BUILD_DIR=$(mktemp -d)
trap "rm -rf ${BUILD_DIR}" EXIT

# Copy Dockerfile and requirements
cp "${ROOT_DIR}/docker/Dockerfile.dev" "${BUILD_DIR}/Dockerfile"
cp "${ROOT_DIR}/docker/requirements-base.txt" "${BUILD_DIR}/"

# Process each image
for mapping in "${DEV_IMAGES[@]}"; do
    IFS='|' read -r base_image internal torch_ver tv_ver ta_ver <<< "$mapping"

    echo ""
    echo "=== ${base_image} -> ${internal} ==="
    echo "  PyTorch: ${torch_ver}+cpu"

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