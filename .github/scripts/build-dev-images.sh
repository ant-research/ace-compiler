#!/bin/bash
# ============================================================
# ACE-Compiler CPU Dev Images Build Script (External/GitHub)
#
# Flow: ubuntu:* -> ace/dev:ubuntu* (add system deps + Python + pip)
#
# Usage:
#   ./.github/scripts/build-dev-images.sh
#   TORCH_VERSIONS="2.5.1 2.11.0" ./.github/scripts/build-dev-images.sh
# ============================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$(dirname "$SCRIPT_DIR")/.." && pwd)"

# Default torch versions to build
TORCH_VERSIONS="${TORCH_VERSIONS:-2.5.1 2.11.0}"

# Version matrix: "base_image|dev_image|torch|torchvision|torchaudio"
declare -A VERSION_MAP=(
    ["2.5.1"]="ubuntu:22.04|ace/dev:ubuntu22.04|2.5.1|0.20.1|2.5.1"
    ["2.11.0"]="ubuntu:24.04|ace/dev:ubuntu24.04|2.11.0|0.26.0|2.11.0"
)

echo "=============================================="
echo "Build CPU Dev Images (External)"
echo "=============================================="

# Create build context
BUILD_DIR=$(mktemp -d)
trap "rm -rf ${BUILD_DIR}" EXIT

# Copy Dockerfile and requirements (preserve docker/ subdir for COPY paths)
mkdir -p "${BUILD_DIR}/docker"
cp "${ROOT_DIR}/docker/Dockerfile.dev" "${BUILD_DIR}/Dockerfile"
cp "${ROOT_DIR}/docker/requirements-base.txt" "${BUILD_DIR}/docker/"

# Process each torch version
for torch_ver in ${TORCH_VERSIONS}; do
    IFS='|' read -r base_image internal tv_ver ta_ver <<< "${VERSION_MAP[$torch_ver]}"

    echo ""
    echo "=== ${base_image} -> ${internal} ==="
    echo "  PyTorch: ${torch_ver}+cpu"

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
        -t "${internal}" \
        -f "${BUILD_DIR}/Dockerfile" \
        "${BUILD_DIR}"

    echo "  OK Done: ${internal}"
done

echo ""
echo "=============================================="
echo "Build completed!"
echo "=============================================="
