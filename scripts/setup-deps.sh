#!/bin/bash
#=============================================================================
#
# ANT-ACE Dependencies Setup Script
#
# Usage:
#   ./scripts/setup-deps.sh              # Install all dependencies
#   ./scripts/setup-deps.sh --python     # Install Python dependencies only
#   ./scripts/setup-deps.sh --system     # Install system dependencies only
#   ./scripts/setup-deps.sh --dev        # Install dev dependencies only
#   ./scripts/setup-deps.sh --help       # Show help
#
#=============================================================================

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Parse arguments
INSTALL_PYTHON=false
INSTALL_SYSTEM=false
INSTALL_DEV=false
INSTALL_ALL=true

while [[ $# -gt 0 ]]; do
    case $1 in
        --python)
            INSTALL_PYTHON=true
            INSTALL_ALL=false
            shift
            ;;
        --system)
            INSTALL_SYSTEM=true
            INSTALL_ALL=false
            shift
            ;;
        --dev)
            INSTALL_DEV=true
            INSTALL_ALL=false
            shift
            ;;
        --help|-h)
            echo "ANT-ACE Dependencies Setup Script"
            echo ""
            echo "Usage: ./scripts/setup-deps.sh [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  --python    Install Python dependencies only"
            echo "  --system    Install system dependencies only"
            echo "  --dev       Install development dependencies only"
            echo "  --help, -h  Show this help message"
            echo ""
            echo "Without options, all dependencies are installed."
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            exit 1
            ;;
    esac
done

echo -e "${BLUE}=================================================${NC}"
echo -e "${BLUE}ANT-ACE Dependencies Setup${NC}"
echo -e "${BLUE}=================================================${NC}"
echo ""

#------------------------------------------------------------------------------
# System Dependencies
#------------------------------------------------------------------------------
install_system_deps() {
    echo -e "${BLUE}Installing system dependencies...${NC}"

    # Detect OS
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        OS=$ID
    else
        OS=$(uname -s)
    fi

    case $OS in
        ubuntu|debian)
            echo -e "${YELLOW}Detected Debian/Ubuntu system${NC}"
            sudo apt-get update
            sudo apt-get install -y \
                build-essential \
                cmake \
                ninja-build \
                git \
                pkg-config \
                libssl-dev \
                python3-dev \
                python3-pip \
                python3-venv
            ;;
        centos|rhel|rocky)
            echo -e "${YELLOW}Detected RHEL/CentOS system${NC}"
            sudo yum groupinstall -y "Development Tools"
            sudo yum install -y \
                cmake \
                ninja-build \
                git \
                pkg-config \
                openssl-devel \
                python3-devel \
                python3-pip
            ;;
        darwin)
            echo -e "${YELLOW}Detected macOS system${NC}"
            if command -v brew &> /dev/null; then
                brew install cmake ninja git openssl python3
            else
                echo -e "${RED}Homebrew not found. Please install Homebrew first.${NC}"
                exit 1
            fi
            ;;
        *)
            echo -e "${YELLOW}Unknown OS: $OS. Skipping system dependencies.${NC}"
            ;;
    esac

    echo -e "${GREEN}System dependencies installed.${NC}"
    echo ""
}

#------------------------------------------------------------------------------
# Python Dependencies
#------------------------------------------------------------------------------
install_python_deps() {
    echo -e "${BLUE}Installing Python dependencies...${NC}"

    cd "$PROJECT_ROOT"

    # Check Python version
    PYTHON_VERSION=$(python3 -c "import sys; print(f'{sys.version_info.major}.{sys.version_info.minor}')")
    echo -e "${YELLOW}Python version: $PYTHON_VERSION${NC}"

    # Upgrade pip
    python3 -m pip install --upgrade pip setuptools wheel

    # Install build dependencies
    pip install \
        scikit-build-core>=0.11 \
        pybind11>=2.10 \
        cmake>=3.28 \
        PyYAML

    # Install runtime dependencies
    pip install \
        torch \
        numpy \
        onnx

    echo -e "${GREEN}Python dependencies installed.${NC}"
    echo ""
}

#------------------------------------------------------------------------------
# Development Dependencies
#------------------------------------------------------------------------------
install_dev_deps() {
    echo -e "${BLUE}Installing development dependencies...${NC}"

    cd "$PROJECT_ROOT"

    # Install test dependencies
    pip install \
        pytest \
        pytest-cov \
        pytest-xdist \
        pytest-timeout

    # Install linting/formatting tools
    pip install \
        black \
        isort \
        flake8 \
        mypy

    # Install documentation tools (optional)
    pip install \
        sphinx \
        sphinx-rtd-theme 2>/dev/null || echo -e "${YELLOW}sphinx install skipped${NC}"

    echo -e "${GREEN}Development dependencies installed.${NC}"
    echo ""
}

#------------------------------------------------------------------------------
# Verify Dependencies
#------------------------------------------------------------------------------
verify_deps() {
    echo -e "${BLUE}Verifying dependencies...${NC}"
    echo ""

    local FAILED=0

    # Check system tools
    echo -e "${YELLOW}System tools:${NC}"
    for cmd in cmake ninja git python3; do
        if command -v $cmd &> /dev/null; then
            VERSION=$($cmd --version 2>&1 | head -1)
            echo -e "  ${GREEN}✓${NC} $cmd: $VERSION"
        else
            echo -e "  ${RED}✗${NC} $cmd: not found"
            FAILED=1
        fi
    done
    echo ""

    # Check Python packages
    echo -e "${YELLOW}Python packages:${NC}"
    for pkg in torch numpy onnx pytest; do
        if python3 -c "import $pkg" 2>/dev/null; then
            VERSION=$(python3 -c "import $pkg; print(getattr($pkg, '__version__', 'unknown'))" 2>/dev/null)
            echo -e "  ${GREEN}✓${NC} $pkg: $VERSION"
        else
            echo -e "  ${RED}✗${NC} $pkg: not found"
            FAILED=1
        fi
    done
    echo ""

    # Check cmake version
    CMAKE_VERSION=$(cmake --version 2>&1 | head -1 | awk '{print $3}')
    if [ "$(printf '%s\n' "3.28" "$CMAKE_VERSION" | sort -V | head -1)" = "3.28" ]; then
        echo -e "${GREEN}CMake version OK (>= 3.28)${NC}"
    else
        echo -e "${RED}CMake version too old. Need >= 3.28, got $CMAKE_VERSION${NC}"
        FAILED=1
    fi
    echo ""

    if [ $FAILED -eq 0 ]; then
        echo -e "${GREEN}All dependencies verified successfully!${NC}"
    else
        echo -e "${RED}Some dependencies are missing. Please install them.${NC}"
        exit 1
    fi
}

#------------------------------------------------------------------------------
# Main
#------------------------------------------------------------------------------
if [ "$INSTALL_ALL" = true ]; then
    install_system_deps
    install_python_deps
    install_dev_deps
else
    [ "$INSTALL_SYSTEM" = true ] && install_system_deps
    [ "$INSTALL_PYTHON" = true ] && install_python_deps
    [ "$INSTALL_DEV" = true ] && install_dev_deps
fi

verify_deps

echo -e "${GREEN}=================================================${NC}"
echo -e "${GREEN}Dependencies setup complete!${NC}"
echo -e "${GREEN}=================================================${NC}"
echo ""
echo -e "${YELLOW}Next steps:${NC}"
echo "  1. Build the project:  ./scripts/dev-build.sh"
echo "  2. Run tests:          ./scripts/test.sh dev"
echo ""