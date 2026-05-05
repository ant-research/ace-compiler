#!/bin/bash
#=============================================================================
#
# Build script for ANT-ACE using scikit-build-core
#
# Usage:
#   ./scripts/build.sh              # Build and install
#   ./scripts/build.sh --clean      # Clean build before building
#   ./scripts/build.sh --help       # Show help
#
#=============================================================================

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Parse arguments
CLEAN=false

while [[ $# -gt 0 ]]; do
    case $1 in
        --clean)
            CLEAN=true
            shift
            ;;
        --help|-h)
            echo "ANT-ACE Build Script"
            echo ""
            echo "Usage: ./scripts/build.sh [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  --clean     Clean build directory before building"
            echo "  --help, -h  Show this help message"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

echo "================================================="
echo "ANT-ACE Build (scikit-build-core)"
echo "================================================="

# Clean if requested
if [ "$CLEAN" = true ]; then
    echo "Cleaning build directory..."
    rm -rf "$PROJECT_ROOT/build"
fi

# Build using pip
echo "Building with pip..."
cd "$PROJECT_ROOT"
pip install . --no-build-isolation

echo "================================================="
echo "Build complete!"
echo "================================================="