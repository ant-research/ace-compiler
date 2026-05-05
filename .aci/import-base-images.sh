#!/bin/bash
# ============================================================
# ACE-FHE CPU Base Images Import Script
#
# Flow: Pull ubuntu:* -> Build Wrapper -> Push to opencc
#
# Usage:
#   CI_TOKEN=xxx ./.aci/import-base-images.sh
# ============================================================

set -euo pipefail

ACI_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Internal registry
OPENCC_REPO="reg.docker.alibaba-inc.com/opencc"

# CI Token (required, will be embedded in image)
if [ -z "${CI_TOKEN:-}" ]; then
    echo "Error: CI_TOKEN is required. Usage: CI_TOKEN=xxx ./.aci/import-base-images.sh"
    exit 1
fi

# CPU base images: "external|internal"
BASE_IMAGES=(
    "ubuntu:20.04|${OPENCC_REPO}/base:ubuntu20.04"
    "ubuntu:22.04|${OPENCC_REPO}/base:ubuntu22.04"
    "ubuntu:24.04|${OPENCC_REPO}/base:ubuntu24.04"
)

echo "=============================================="
echo "Import CPU Base Images"
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