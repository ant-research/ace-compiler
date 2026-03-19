#!/bin/bash
#
# ACE DSL (acepy) Build Script
#
# This script uses the SHARED bindings at ../bindings/
#
# Usage:
#   ./build.sh           # Build shared bindings and install Python package
#   ./build.sh clean     # Clean build directory
#   ./build.sh bindings  # Only build shared bindings
#   ./build.sh install   # Only install Python package
#   ./build.sh test      # Run tests
#   ./build.sh all       # Build, install, and test
#
# Directory structure:
#   ace-compiler/
#   ├── bindings/        # Shared C++ bindings (built here)
#   ├── ace_bindings/    # Python package with .so files (output)
#   ├── acepy/           # This directory
#   └── ace_edsl/        # Also uses ace_bindings

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ACE_COMPILER_DIR="${SCRIPT_DIR}/.."
BINDINGS_DIR="${ACE_COMPILER_DIR}/bindings"
BINDINGS_BUILD_DIR="${BINDINGS_DIR}/build"
ACE_BINDINGS_DIR="${ACE_COMPILER_DIR}/ace_bindings"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

error() {
    echo -e "${RED}[ERROR]${NC} $1"
    exit 1
}

check_prerequisites() {
    info "Checking prerequisites..."
    
    # Check Python
    if ! command -v python3 &> /dev/null; then
        error "Python3 not found. Please install Python 3.8+"
    fi
    
    # Check pybind11
    if ! python3 -c "import pybind11" &> /dev/null; then
        warn "pybind11 not found. Installing..."
        pip install pybind11
    fi
    
    # Check ACE libraries
    if [ ! -f "${ACE_COMPILER_DIR}/ace_cmplr/lib/libAIRbase.a" ]; then
        error "ACE libraries not found at ${ACE_COMPILER_DIR}/ace_cmplr/lib/\n" \
              "Please build ACE first:\n" \
              "  cd ${ACE_COMPILER_DIR} && ./build.sh release"
    fi
    
    # Check shared bindings directory
    if [ ! -f "${BINDINGS_DIR}/CMakeLists.txt" ]; then
        error "Shared bindings not found at ${BINDINGS_DIR}\n" \
              "Expected: ${BINDINGS_DIR}/CMakeLists.txt"
    fi
    
    info "Prerequisites OK"
}

build_bindings() {
    info "Building shared bindings at ${BINDINGS_DIR}..."
    
    mkdir -p "${BINDINGS_BUILD_DIR}"
    cd "${BINDINGS_BUILD_DIR}"
    
    # Get pybind11 cmake directory
    PYBIND11_DIR=$(python3 -c "import pybind11; print(pybind11.get_cmake_dir())")
    
    cmake .. \
        -DACE_COMPILER_DIR="${ACE_COMPILER_DIR}" \
        -DACE_INSTALL_DIR="${ACE_COMPILER_DIR}/ace_cmplr" \
        -Dpybind11_DIR="${PYBIND11_DIR}"
    
    make -j$(nproc)
    
    cd "${SCRIPT_DIR}"
    
    # Verify .so files were created
    if [ ! -f "${ACE_BINDINGS_DIR}/air_builder"*.so ]; then
        error "Bindings build failed - .so files not found in ${ACE_BINDINGS_DIR}"
    fi
    
    info "Shared bindings built successfully"
    info "Output: ${ACE_BINDINGS_DIR}/"
}

install_package() {
    info "Installing Python package..."
    cd "${SCRIPT_DIR}"
    
    # Install in editable mode
    pip install -e . --quiet
    
    info "Python package installed"
}

run_tests() {
    info "Running tests..."
    cd "${SCRIPT_DIR}"
    
    # Set PYTHONPATH
    export PYTHONPATH="${SCRIPT_DIR}:${ACE_COMPILER_DIR}:${PYTHONPATH}"
    
    # Run pytest tests (only if test files exist)
    if [ -d "tests" ] && ls tests/test_*.py 1>/dev/null 2>&1; then
        info "Running unit tests..."
        python3 -m pytest tests/ -v --tb=short || warn "Some unit tests failed"
    else
        info "No unit tests found, skipping pytest"
    fi
    
    # Run example tests
    info "Running integration tests..."
    for test in examples/test_*.py; do
        if [ -f "$test" ]; then
            echo -n "  $(basename $test): "
            if timeout 120 python3 "$test" > /dev/null 2>&1; then
                echo -e "${GREEN}✓${NC}"
            else
                echo -e "${RED}✗${NC}"
            fi
        fi
    done
    
    info "Tests completed"
}

clean_build() {
    info "Cleaning build directories..."
    rm -rf "${BINDINGS_BUILD_DIR}"
    rm -rf "${SCRIPT_DIR}/build"
    rm -rf "${SCRIPT_DIR}/*.egg-info"
    rm -rf "${SCRIPT_DIR}/ace_dsl.egg-info"
    find "${SCRIPT_DIR}" -type d -name "__pycache__" -exec rm -rf {} + 2>/dev/null || true
    info "Clean completed"
}

show_usage() {
    echo "ACE DSL (acepy) Build Script"
    echo ""
    echo "Uses shared bindings at: ${BINDINGS_DIR}"
    echo ""
    echo "Usage: $0 [command]"
    echo ""
    echo "Commands:"
    echo "  (default)   Build shared bindings and install Python package"
    echo "  bindings    Only build shared bindings"
    echo "  install     Only install Python package"
    echo "  test        Run tests"
    echo "  all         Build, install, and test"
    echo "  clean       Clean build directories"
    echo "  help        Show this help"
    echo ""
    echo "Directory structure:"
    echo "  ${ACE_COMPILER_DIR}/"
    echo "  ├── bindings/        # Shared C++ bindings"
    echo "  ├── ace_bindings/    # Python package with .so files"
    echo "  ├── acepy/           # This directory"
    echo "  └── ace_edsl/        # Also uses ace_bindings"
}

# Main
case "${1:-}" in
    "")
        check_prerequisites
        build_bindings
        install_package
        info "Build completed successfully!"
        echo ""
        echo "To run tests:"
        echo "  ./build.sh test"
        echo ""
        echo "To use:"
        echo "  export PYTHONPATH=\"${ACE_COMPILER_DIR}:\$PYTHONPATH\""
        echo "  python3 -c 'from ace_bindings import air_builder; print(\"OK\")'"
        ;;
    "bindings")
        check_prerequisites
        build_bindings
        ;;
    "install")
        install_package
        ;;
    "test")
        run_tests
        ;;
    "all")
        check_prerequisites
        build_bindings
        install_package
        run_tests
        ;;
    "clean")
        clean_build
        ;;
    "help"|"-h"|"--help")
        show_usage
        ;;
    *)
        error "Unknown command: $1\nRun '$0 help' for usage"
        ;;
esac
