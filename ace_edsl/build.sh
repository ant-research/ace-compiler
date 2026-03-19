#!/bin/bash
#
# ACE EDSL Build Script
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
#   ├── acepy/           # Also uses ace_bindings
#   └── ace_edsl/        # This directory

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
BLUE='\033[0;34m'
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

header() {
    echo -e "${BLUE}======================================================================${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}======================================================================${NC}"
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
    local FULL_TEST="${1:-full}"
    # Bootstrap harness checks can exceed 4 minutes on debug/openmp builds.
    local PYTEST_TIMEOUT="${ACE_EDSL_PYTEST_TIMEOUT:-600}"
    
    header "ACE EDSL Test Suite"
    cd "${SCRIPT_DIR}"
    
    # Set PYTHONPATH
    export PYTHONPATH="${SCRIPT_DIR}:${ACE_COMPILER_DIR}:${PYTHONPATH}"
    
    PASSED=0
    FAILED=0
    
    # Run pytest tests (quick ones only, skip slow codegen tests)
    if [ -d "tests" ]; then
        info "Running unit tests..."
        if timeout "${PYTEST_TIMEOUT}" python3 -m pytest tests/test_domain_kernels.py tests/test_bootstrap_stage_ops.py tests/test_bootstrap_full.py -v --tb=short 2>&1; then
            info "Unit tests passed"
            PASSED=$((PASSED + 1))
        else
            warn "Some unit tests failed"
            FAILED=$((FAILED + 1))
        fi
    fi
    
    echo ""
    header "Integration Tests (examples/)"
    
    # Run bootstrap_full.py (quick, ~2s)
    echo -n "  bootstrap_full: "
    if python3 examples/bootstrap_full.py >/dev/null 2>&1; then
        echo -e "${GREEN}✓${NC}"
        PASSED=$((PASSED + 1))
    else
        echo -e "${RED}✗${NC}"
        FAILED=$((FAILED + 1))
    fi
    
    # test_deferred_lowering - single mode (quick, pure ace_edsl, ~2s)
    echo -n "  test_deferred_lowering (single): "
    if python3 examples/test_deferred_lowering.py single >/dev/null 2>&1; then
        echo -e "${GREEN}✓${NC}"
        PASSED=$((PASSED + 1))
    else
        echo -e "${RED}✗${NC}"
        FAILED=$((FAILED + 1))
    fi
    
    # test_deferred_lowering - onnx mode (slow, needs acepy, ~2min)
    if [ "$FULL_TEST" = "full" ]; then
        echo -n "  test_deferred_lowering (onnx): "
        if python3 examples/test_deferred_lowering.py onnx >/dev/null 2>&1; then
            echo -e "${GREEN}✓${NC}"
            PASSED=$((PASSED + 1))
        else
            echo -e "${RED}✗${NC}"
            FAILED=$((FAILED + 1))
        fi

    else
        echo -e "  test_deferred_lowering (onnx): ${YELLOW}skipped${NC} (use './build.sh test-quick' to skip)"
    fi
    
    # Summary
    echo ""
    header "Test Summary"
    echo -e "  ${GREEN}Passed:${NC}  $PASSED"
    echo -e "  ${RED}Failed:${NC}  $FAILED"
    
    if [ $FAILED -gt 0 ]; then
        warn "Some tests failed"
        return 1
    else
        info "All tests passed!"
        return 0
    fi
}

clean_build() {
    info "Cleaning build directories..."
    rm -rf "${BINDINGS_BUILD_DIR}"
    rm -rf "${SCRIPT_DIR}/build"
    rm -rf "${SCRIPT_DIR}/*.egg-info"
    rm -rf "${SCRIPT_DIR}/ace_edsl.egg-info"
    rm -rf "${SCRIPT_DIR}/examples/output"
    find "${SCRIPT_DIR}" -type d -name "__pycache__" -exec rm -rf {} + 2>/dev/null || true
    info "Clean completed"
}

show_usage() {
    echo "ACE EDSL Build Script"
    echo ""
    echo "Uses shared bindings at: ${BINDINGS_DIR}"
    echo ""
    echo "Usage: $0 [command]"
    echo ""
    echo "Commands:"
    echo "  (default)   Build shared bindings and install Python package"
    echo "  bindings    Only build shared bindings"
    echo "  install     Only install Python package"
    echo "  test        Run all tests including ONNX pipeline (~3 min)"
    echo "  test-quick  Run quick tests (skips slow ONNX test)"
    echo "  test-full   Same as 'test' (kept for compatibility)"
    echo "  all         Build, install, and test"
    echo "  clean       Clean build directories"
    echo "  help        Show this help"
    echo ""
    echo "Individual test modes (examples/test_deferred_lowering.py):"
    echo "  python3 examples/test_deferred_lowering.py single   # ~2s"
    echo "  python3 examples/test_deferred_lowering.py multiple # ~3s"
    echo "  python3 examples/test_deferred_lowering.py onnx     # ~2min"
    echo ""
    echo "Directory structure:"
    echo "  ${ACE_COMPILER_DIR}/"
    echo "  ├── bindings/        # Shared C++ bindings"
    echo "  ├── ace_bindings/    # Python package with .so files"
    echo "  ├── acepy/           # Also uses ace_bindings"
    echo "  └── ace_edsl/        # This directory"
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
        run_tests full
        ;;
    "test-quick")
        run_tests quick
        ;;
    "test-full")
        run_tests full
        ;;
    "all")
        check_prerequisites
        build_bindings
        install_package
        run_tests full
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
