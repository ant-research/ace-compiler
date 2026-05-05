#!/bin/bash
# ============================================================
# ACE-Compiler Single Wheel Build Script
# Builds a single wheel for a specific CUDA version
#
# Usage:
#   ./scripts/build-wheel.sh --cuda cu124
#   ./scripts/build-wheel.sh --cuda cu124 --output ./dist
#   ./scripts/build-wheel.sh --local-version cpu --output ./dist
#
# Options:
#   --cuda VERSION      CUDA version (e.g. cu124, cu125, cu126)
#   --local-version TAG Local version tag (e.g. cu124, cpu)
#   --output DIR        Output directory (default: ./dist)
#   --dry-run           Show what would be done without building
# ============================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"
DIST_DIR="$ROOT_DIR/dist"

# Default values
LOCAL_VERSION=""
DRY_RUN=false

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --cuda)
            LOCAL_VERSION="$2"
            shift 2
            ;;
        --local-version)
            LOCAL_VERSION="$2"
            shift 2
            ;;
        --output)
            DIST_DIR="$2"
            shift 2
            ;;
        --dry-run)
            DRY_RUN=true
            shift
            ;;
        -h|--help)
            head -15 "$0" | tail -10
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Dev image
DEV_REGISTRY="${DEV_REGISTRY:-reg.docker.alibaba-inc.com/ace/dev-cuda:}"

# CUDA version to full tag mapping
declare -A TAG_MAP=(
    ["cu124"]="12.4.0-devel-ubuntu22.04"
    ["cu125"]="12.5.0-devel-ubuntu22.04"
    ["cu126"]="12.6.0-devel-ubuntu24.04"
)

if [ -n "$LOCAL_VERSION" ]; then
    # Determine image from local version
    if [[ "$LOCAL_VERSION" == cu* ]]; then
        tag="${TAG_MAP[$LOCAL_VERSION]:-$LOCAL_VERSION}"
        IMAGE="${DEV_REGISTRY}${tag}"
    else
        IMAGE="${DEV_REGISTRY}${LOCAL_VERSION}"
    fi
else
    LOCAL_VERSION="cu124"
    IMAGE="${DEV_REGISTRY}12.4.0-devel-ubuntu22.04"
fi

mkdir -p "$DIST_DIR"

echo "=============================================="
echo "ACE-Compiler Wheel Build"
echo "=============================================="
echo "Local version: +${LOCAL_VERSION}"
echo "Image:         $IMAGE"
echo "Output:        $DIST_DIR"
echo "=============================================="

if [ "$DRY_RUN" = true ]; then
    echo ""
    echo "[DRY RUN] Would execute:"
    echo "  docker run --rm \\"
    echo "    -v $ROOT_DIR:/app \\"
    echo "    -v $DIST_DIR:/wheels \\"
    echo "    -e ACE_LOCAL_VERSION=${LOCAL_VERSION} \\"
    echo "    $IMAGE \\"
    echo "    'pip wheel --no-deps --no-build-isolation /app -w /wheels'"
    exit 0
fi

if ! docker image inspect "${IMAGE}" &>/dev/null; then
    echo "Error: Image ${IMAGE} not found. Build dev image first."
    echo "  Internal: ./.aci/build-dev-cuda-images.sh"
    echo "  External: ./.github/scripts/build-dev-cuda-images.sh"
    exit 1
fi

docker run --rm \
    -v "$ROOT_DIR:/app" \
    -v "$DIST_DIR:/wheels" \
    -e ACE_LOCAL_VERSION="${LOCAL_VERSION}" \
    "${IMAGE}" \
    /bin/bash -c 'pip wheel --no-deps --no-build-isolation /app -w /wheels'

echo ""
echo "=============================================="
echo "Build completed!"
echo "=============================================="
echo "Wheel location: $DIST_DIR"
ls -la "$DIST_DIR"/*.whl 2>/dev/null || echo "No wheels found"