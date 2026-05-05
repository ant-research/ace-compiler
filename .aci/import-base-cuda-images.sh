#!/bin/bash
# ============================================================
# ACE-FHE CUDA Base Images Import Script
#
# Flow: Pull nvidia/cuda:* -> Build Wrapper -> Push to opencc
#
# Usage:
#   CI_TOKEN=xxx ./.aci/import-base-cuda-images.sh
# ============================================================

set -euo pipefail

ACI_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Internal registry
OPENCC_REPO="reg.docker.alibaba-inc.com/opencc"

# CI Token (required, will be embedded in image)
if [ -z "${CI_TOKEN:-}" ]; then
    echo "Error: CI_TOKEN is required. Usage: CI_TOKEN=xxx ./.aci/import-base-cuda-images.sh"
    exit 1
fi

# CUDA base images: "external|internal"
BASE_IMAGES=(
    "nvidia/cuda:12.4.0-devel-ubuntu22.04|${OPENCC_REPO}/base-cuda:12.4.0-devel-ubuntu22.04"
    "nvidia/cuda:12.5.0-devel-ubuntu22.04|${OPENCC_REPO}/base-cuda:12.5.0-devel-ubuntu22.04"
    "nvidia/cuda:12.6.0-devel-ubuntu24.04|${OPENCC_REPO}/base-cuda:12.6.0-devel-ubuntu24.04"
)

echo "=============================================="
echo "Import CUDA Base Images"
echo "=============================================="

# Create build context
BUILD_DIR=$(mktemp -d)
trap "rm -rf ${BUILD_DIR}" EXIT

# Copy Dockerfile
cp "${ACI_DIR}/docker/Dockerfile.base" "${BUILD_DIR}/Dockerfile"

# Process each image
for mapping in "${BASE_IMAGES[@]}"; do
    IFS='|' read -r external internal <<< "$mapping"

    echo ""
    echo "=== ${external} -> ${internal} ==="

    # Pull
    docker pull "${external}"

    # Build wrapper (no cache)
    docker build \
        --no-cache \
        --build-arg BASE_IMAGE="${external}" \
        --build-arg CI_TOKEN="${CI_TOKEN}" \
        -t "${internal}" \
        -f "${BUILD_DIR}/Dockerfile" \
        "${BUILD_DIR}"

    # Push
    docker push "${internal}"

    echo "  OK Done"
done

echo ""
echo "=============================================="
echo "Import completed!"
echo "=============================================="