#!/bin/bash
# ============================================================
# ACE-Compiler Wheel Build Script
# Builds wheels for all CUDA combinations using dev images
#
# Usage:
#   ./scripts/build-matrix.sh
#   CUDA_VERSIONS="cu124 cu125" ./scripts/build-matrix.sh
#
# Prerequisites:
#   Dev images must be built first (see .aci/build-dev-cuda-images.sh
#   or .github/scripts/build-dev-cuda-images.sh)
# ============================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"
DIST_DIR="$ROOT_DIR/dist"

# Default CUDA versions
CUDA_VERSIONS="${CUDA_VERSIONS:-cu124 cu125 cu126}"

# Dev image registry and tag mapping
DEV_REGISTRY="${DEV_REGISTRY:-reg.docker.alibaba-inc.com/ace/dev-cuda:}"

# CUDA version to full tag mapping
declare -A TAG_MAP=(
    ["cu124"]="12.4.0-devel-ubuntu22.04"
    ["cu125"]="12.5.0-devel-ubuntu22.04"
    ["cu126"]="12.6.0-devel-ubuntu24.04"
)

# Output directory
mkdir -p "$DIST_DIR"

echo "=============================================="
echo "ACE-Compiler Wheel Build Matrix"
echo "=============================================="
echo "CUDA versions: $CUDA_VERSIONS"
echo "Output:        $DIST_DIR"
echo "=============================================="

for cuda_ver in ${CUDA_VERSIONS}; do
    tag="${TAG_MAP[$cuda_ver]}"
    image="${DEV_REGISTRY}${tag}"

    echo ""
    echo "=== Building wheel for CUDA ${cuda_ver} ==="
    echo "Image: ${image}"

    if ! docker image inspect "${image}" &>/dev/null; then
        echo "Error: Image ${image} not found. Build dev image first."
        echo "  Internal: ./.aci/build-dev-cuda-images.sh"
        echo "  External: ./.github/scripts/build-dev-cuda-images.sh"
        exit 1
    fi

    docker run --rm \
        -v "$ROOT_DIR:/app" \
        -v "$DIST_DIR:/wheels" \
        -e ACE_LOCAL_VERSION="${cuda_ver}" \
        "${image}" \
        /bin/bash -c 'pip wheel --no-deps --no-build-isolation /app -w /wheels'

    echo "  OK Done"
done

echo ""
echo "=============================================="
echo "All builds completed!"
echo "=============================================="
echo "Wheels location: $DIST_DIR"
ls -la "$DIST_DIR"/*.whl 2>/dev/null || echo "No wheels found"