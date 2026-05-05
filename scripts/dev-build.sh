#!/bin/bash
#=============================================================================
#
# Development Build Script for ANT-ACE
#
# Usage:
#   ./scripts/dev-build.sh              # Build and install to site-packages
#   ./scripts/dev-build.sh --build-only # Only build, don't install
#   ./scripts/dev-build.sh --install-only # Only install from existing build
#   ./scripts/dev-build.sh --clean      # Clean build directory
#   ./scripts/dev-build.sh --help       # Show help
#
#=============================================================================

set -e

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"
SITE_PACKAGES="$(python3 -c 'import sysconfig; print(sysconfig.get_path("purelib"))')"

# Build type: default Debug for development, override with CMAKE_BUILD_TYPE=Release
CMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE:-Debug}"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Parse arguments
BUILD_ONLY=false
INSTALL_ONLY=false
CLEAN=false
HELP=false
INSTALL_PREFIX="$SITE_PACKAGES"

while [[ $# -gt 0 ]]; do
    case $1 in
        --build-only)
            BUILD_ONLY=true
            shift
            ;;
        --install-only)
            INSTALL_ONLY=true
            shift
            ;;
        --clean)
            CLEAN=true
            shift
            ;;
        --prefix)
            INSTALL_PREFIX="$2"
            shift 2
            ;;
        --help|-h)
            HELP=true
            shift
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

if [ "$HELP" = true ]; then
    echo "Development Build Script for ANT-ACE"
    echo ""
    echo "Usage:"
    echo "  ./scripts/dev-build.sh [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  --build-only          Only build, don't install to site-packages"
    echo "  --install-only        Only install from existing build directory"
    echo "  --clean               Clean build directory before building"
    echo "  --prefix PATH         Install prefix (default: \$SITE_PACKAGES)"
    echo "  --help, -h            Show this help message"
    echo ""
    echo "Environment Variables:"
    echo "  CMAKE_BUILD_TYPE      Build type (default: Debug; set Release for production)"
    echo ""
    echo "Examples:"
    echo "  ./scripts/dev-build.sh                          # Build and install (Debug)"
    echo "  ./scripts/dev-build.sh --build-only             # Build only"
    echo "  ./scripts/dev-build.sh --install-only           # Install from existing build"
    echo "  ./scripts/dev-build.sh --clean                  # Clean and rebuild"
    echo "  ./scripts/dev-build.sh --prefix /custom/path    # Custom install prefix"
    echo "  CMAKE_BUILD_TYPE=Release ./scripts/dev-build.sh # Release build"
    exit 0
fi

# Install destination (CMake install rules use ace/ prefix within INSTALL_PREFIX)
ACE_DIR="$INSTALL_PREFIX/ace"

echo -e "${BLUE}=================================================${NC}"
echo -e "${BLUE}ANT-ACE Development Build${NC}"
echo -e "${BLUE}=================================================${NC}"
echo -e "${YELLOW}Project Root:  ${PROJECT_ROOT}${NC}"
echo -e "${YELLOW}Build Dir:     ${BUILD_DIR}${NC}"
echo -e "${YELLOW}Build Type:    ${CMAKE_BUILD_TYPE}${NC}"
echo -e "${YELLOW}Install To:    ${ACE_DIR}${NC}"
echo ""

# --install-only: skip configure and build, go straight to install
if [ "$INSTALL_ONLY" = true ]; then
    if [ ! -d "$BUILD_DIR" ]; then
        echo -e "${RED}Build directory not found: $BUILD_DIR${NC}"
        echo "Run ./scripts/dev-build.sh first to configure and build."
        exit 1
    fi

    echo -e "${BLUE}Installing to ${ACE_DIR}...${NC}"
    mkdir -p "$ACE_DIR"

    # Install all components via CMake
    cmake --install "$BUILD_DIR" --prefix "$INSTALL_PREFIX" --component frontend 2>&1 | tee "$BUILD_DIR/install-frontend.log"
    cmake --install "$BUILD_DIR" --prefix "$INSTALL_PREFIX" --component core 2>&1 | tee "$BUILD_DIR/install-core.log"
    cmake --install "$BUILD_DIR" --prefix "$INSTALL_PREFIX" --component runtime 2>&1 | tee "$BUILD_DIR/install-runtime.log"
    cmake --install "$BUILD_DIR" --prefix "$INSTALL_PREFIX" --component library 2>&1 | tee "$BUILD_DIR/install-library.log"

    echo -e "${GREEN}Install complete.${NC}"
    echo ""

    # Copy .so files to source directory for editable install
    echo -e "${BLUE}Copying .so files to source directory for editable install...${NC}"
    for so_file in "$ACE_DIR/"*.so; do
        if [ -f "$so_file" ]; then
            cp -f "$so_file" "$PROJECT_ROOT/fhe_dsl/ace/"
            echo -e "${GREEN}  Copied: $(basename $so_file)${NC}"
        fi
    done
    if [ -f "$ACE_DIR/lib/libFHErt_common.so" ]; then
        mkdir -p "$PROJECT_ROOT/fhe_dsl/ace/lib/"
        cp -f "$ACE_DIR/lib/libFHErt_common.so" "$PROJECT_ROOT/fhe_dsl/ace/lib/"
        echo -e "${GREEN}  Copied: libFHErt_common.so${NC}"
    fi

    echo -e "${GREEN}=================================================${NC}"
    echo -e "${GREEN}Install Successful!${NC}"
    echo -e "${GREEN}=================================================${NC}"
    exit 0
fi

# Clean if requested
if [ "$CLEAN" = true ]; then
    echo -e "${YELLOW}Cleaning build directory...${NC}"
    rm -rf "$BUILD_DIR"
    echo -e "${GREEN}Clean complete.${NC}"
    echo ""
fi

# Create build directory
mkdir -p "$BUILD_DIR"

# Step 1: Configure with CMake
echo -e "${BLUE}Step 1: Configuring CMake...${NC}"
cmake -S "$PROJECT_ROOT" -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE="$CMAKE_BUILD_TYPE" \
    -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" \
    -DBUILD_EXTENSION=ON \
    -DBUILD_TESTS=OFF \
    -G Ninja \
    2>&1 | tee "$BUILD_DIR/configure.log"

echo -e "${GREEN}CMake configuration complete.${NC}"
echo ""

# Step 2: Build
echo -e "${BLUE}Step 2: Building...${NC}"
cmake --build "$BUILD_DIR" -j$(nproc) 2>&1 | tee "$BUILD_DIR/build.log"
BUILD_STATUS=${PIPESTATUS[0]}

if [ $BUILD_STATUS -ne 0 ]; then
    echo -e "${RED}Build failed with status $BUILD_STATUS${NC}"
    exit 1
fi

echo -e "${GREEN}Build complete.${NC}"
echo ""

# Step 3: Install (if not build-only)
if [ "$BUILD_ONLY" = false ]; then
    echo -e "${BLUE}Step 3: Installing to ${ACE_DIR}...${NC}"

    # Create target directory
    mkdir -p "$ACE_DIR"

    # Install all components via CMake (no --prefix: use configure-time prefix)
    cmake --install "$BUILD_DIR" --component frontend 2>&1 | tee "$BUILD_DIR/install-frontend.log"
    cmake --install "$BUILD_DIR" --component core 2>&1 | tee "$BUILD_DIR/install-core.log"
    cmake --install "$BUILD_DIR" --component runtime 2>&1 | tee "$BUILD_DIR/install-runtime.log"
    cmake --install "$BUILD_DIR" --component library 2>&1 | tee "$BUILD_DIR/install-library.log"

    echo -e "${GREEN}Install complete.${NC}"
    echo ""

    # Copy .so files to source directory for editable install
    echo -e "${BLUE}Copying .so files to source directory for editable install...${NC}"
    for so_file in "$ACE_DIR/"*.so; do
        if [ -f "$so_file" ]; then
            cp -f "$so_file" "$PROJECT_ROOT/fhe_dsl/ace/"
            echo -e "${GREEN}  Copied: $(basename $so_file)${NC}"
        fi
    done
    # Also copy libFHErt_common.so if it exists in ace/lib
    if [ -f "$ACE_DIR/lib/libFHErt_common.so" ]; then
        mkdir -p "$PROJECT_ROOT/fhe_dsl/ace/lib/"
        cp -f "$ACE_DIR/lib/libFHErt_common.so" "$PROJECT_ROOT/fhe_dsl/ace/lib/"
        echo -e "${GREEN}  Copied: libFHErt_common.so${NC}"
    fi

    # Verify installation
    echo -e "${BLUE}Verifying installation...${NC}"
    echo -e "${YELLOW}Installed files:${NC}"
    ls -la "$ACE_DIR/" 2>/dev/null || echo "  ace/ not found"
    echo ""
    echo -e "${YELLOW}C++ extensions:${NC}"
    ls -la "$ACE_DIR/"*.so 2>/dev/null || echo "  No .so files found"
    echo ""
fi

echo -e "${GREEN}=================================================${NC}"
echo -e "${GREEN}Build Successful!${NC}"
echo -e "${GREEN}=================================================${NC}"
echo ""
echo -e "${YELLOW}Next steps:${NC}"
echo "  1. Test the installation:"
echo "     python3 -c 'from ace import frontend, runtime; print(\"OK\")'"
echo ""
echo "  2. Run tests:"
echo "     pytest tests/test_unit/test_frontend/ -v"
echo ""
echo "  Note: Runtime libraries are in ${ACE_DIR}/lib/"
echo ""