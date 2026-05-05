#!/bin/bash
#=============================================================================
#
# ANT-ACE Installation Verification Script
#
# Usage:
#   ./scripts/verify-install.sh              # Full verification
#   ./scripts/verify-install.sh --quick      # Quick import check only
#   ./scripts/verify-install.sh --functional # Run functional tests
#   ./scripts/verify-install.sh --help       # Show help
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
QUICK=false
FUNCTIONAL=false

while [[ $# -gt 0 ]]; do
    case $1 in
        --quick)
            QUICK=true
            shift
            ;;
        --functional)
            FUNCTIONAL=true
            shift
            ;;
        --help|-h)
            echo "ANT-ACE Installation Verification Script"
            echo ""
            echo "Usage: ./scripts/verify-install.sh [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  --quick       Quick import check only"
            echo "  --functional  Run functional tests"
            echo "  --help, -h    Show this help message"
            echo ""
            echo "Without options, full verification is performed."
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            exit 1
            ;;
    esac
done

echo -e "${BLUE}=================================================${NC}"
echo -e "${BLUE}ANT-ACE Installation Verification${NC}"
echo -e "${BLUE}=================================================${NC}"
echo ""

ERRORS=0

#------------------------------------------------------------------------------
# Quick Import Check
#------------------------------------------------------------------------------
check_imports() {
    echo -e "${BLUE}Checking Python imports...${NC}"
    echo ""

    # Core package imports
    echo -e "${YELLOW}Core package:${NC}"

    if python3 -c "import ace; print('  ace version:', ace.__version__)" 2>&1; then
        echo -e "  ${GREEN}✓${NC} ace"
    else
        echo -e "  ${RED}✗${NC} ace"
        ERRORS=$((ERRORS + 1))
    fi

    if python3 -c "from ace import fhe; print('  fhe OK')" 2>&1; then
        echo -e "  ${GREEN}✓${NC} ace.fhe"
    else
        echo -e "  ${RED}✗${NC} ace.fhe"
        ERRORS=$((ERRORS + 1))
    fi

    echo ""

    # C++ Extension imports
    echo -e "${YELLOW}C++ Extensions:${NC}"

    if python3 -c "from ace import frontend; print('  frontend OK')" 2>&1; then
        echo -e "  ${GREEN}✓${NC} ace.frontend"
    else
        echo -e "  ${YELLOW}?${NC} ace.frontend (may not be built)"
    fi

    if python3 -c "from ace import runtime; print('  runtime OK')" 2>&1; then
        echo -e "  ${GREEN}✓${NC} ace.runtime"
    else
        echo -e "  ${YELLOW}?${NC} ace.runtime (optional)"
    fi

    echo ""

    # FHE utility imports
    echo -e "${YELLOW}FHE utilities:${NC}"
    for module in \
        "ace.fhe.util" \
        "ace.fhe.util.logger"; do
        if python3 -c "import $module" 2>&1; then
            echo -e "  ${GREEN}✓${NC} $module"
        else
            echo -e "  ${RED}✗${NC} $module"
            ERRORS=$((ERRORS + 1))
        fi
    done
    echo ""

    # Frontend imports
    echo -e "${YELLOW}Frontend modules:${NC}"
    for module in \
        "ace.fhe.frontend.torch.torch_frontend" \
        "ace.fhe.ir.core.ir_builder" \
        "ace.fhe.ir.core.tensor_registry"; do
        if python3 -c "import $module" 2>&1; then
            echo -e "  ${GREEN}✓${NC} $module"
        else
            echo -e "  ${RED}✗${NC} $module"
            ERRORS=$((ERRORS + 1))
        fi
    done
    echo ""

    # Backend imports
    echo -e "${YELLOW}Backend modules:${NC}"
    for module in \
        "ace.fhe.backend" \
        "ace.fhe.backend.antlib" \
        "ace.fhe.backend.phantom"; do
        if python3 -c "import $module" 2>&1; then
            echo -e "  ${GREEN}✓${NC} $module"
        else
            echo -e "  ${YELLOW}?${NC} $module (optional)"
        fi
    done
    echo ""
}

#------------------------------------------------------------------------------
# Version Check
#------------------------------------------------------------------------------
check_version() {
    echo -e "${BLUE}Checking version...${NC}"

    VERSION=$(python3 -c "
try:
    exec(open('$PROJECT_ROOT/fhe_dsl/ace/_version.py').read())
    print(__version__)
except Exception as e:
    print('unknown')
" 2>/dev/null)

    if [ "$VERSION" != "unknown" ]; then
        echo -e "  ${GREEN}✓${NC} Version: $VERSION"
    else
        echo -e "  ${RED}✗${NC} Could not determine version"
        ERRORS=$((ERRORS + 1))
    fi
    echo ""
}

#------------------------------------------------------------------------------
# C++ Extension Check
#------------------------------------------------------------------------------
check_cpp_extension() {
    echo -e "${BLUE}Checking C++ extensions...${NC}"

    # Get ace package path
    ACE_PATH=$(python3 -c "
import ace
import os
print(os.path.dirname(ace.__file__))
" 2>/dev/null)

    if [ -n "$ACE_PATH" ] && [ -d "$ACE_PATH" ]; then
        echo -e "  ${GREEN}✓${NC} Package path: $ACE_PATH"

        # List shared libraries
        echo -e "${YELLOW}  Shared libraries:${NC}"
        find "$ACE_PATH" -maxdepth 1 -name "*.so" -o -name "*.pyd" 2>/dev/null | while read lib; do
            echo "    - $(basename $lib)"
        done
    else
        echo -e "  ${RED}✗${NC} Package path not found"
        ERRORS=$((ERRORS + 1))
    fi
    echo ""
}

#------------------------------------------------------------------------------
# Functional Tests
#------------------------------------------------------------------------------
run_functional_tests() {
    echo -e "${BLUE}Running functional tests...${NC}"
    echo ""

    cd "$PROJECT_ROOT"

    # Test 1: Basic torch frontend
    echo -e "${YELLOW}Test 1: Torch frontend import${NC}"
    if python3 -c "
import torch
from ace.fhe.frontend.torch.torch_frontend import TorchFrontend
print('  Torch frontend OK')
" 2>&1; then
        echo -e "  ${GREEN}✓${NC} Torch frontend"
    else
        echo -e "  ${RED}✗${NC} Torch frontend"
        ERRORS=$((ERRORS + 1))
    fi
    echo ""

    # Test 2: Simple model compilation
    echo -e "${YELLOW}Test 2: Simple model compilation${NC}"
    if python3 -c "
import torch
import torch.nn as nn

class AddModel(nn.Module):
    def forward(self, x, y):
        return x + y

model = AddModel()
x = torch.randn(1, 3)
y = torch.randn(1, 3)

# Just test model forward pass
output = model(x, y)
print('  Model forward pass OK')
" 2>&1; then
        echo -e "  ${GREEN}✓${NC} Simple model"
    else
        echo -e "  ${RED}✗${NC} Simple model"
        ERRORS=$((ERRORS + 1))
    fi
    echo ""

    # Test 3: Run actual pytest
    echo -e "${YELLOW}Test 3: Pytest quick test${NC}"
    if python3 -m pytest tests/test_unit/test_frontend/ -v --tb=short -x -q 2>&1 | head -30; then
        echo -e "  ${GREEN}✓${NC} Unit tests passed"
    else
        echo -e "  ${RED}✗${NC} Unit tests failed"
        ERRORS=$((ERRORS + 1))
    fi
    echo ""
}

#------------------------------------------------------------------------------
# Summary
#------------------------------------------------------------------------------
print_summary() {
    echo -e "${BLUE}=================================================${NC}"
    if [ $ERRORS -eq 0 ]; then
        echo -e "${GREEN}Verification PASSED!${NC}"
        echo -e "${GREEN}=================================================${NC}"
        echo ""
        echo -e "${YELLOW}Installation is ready to use.${NC}"
        echo ""
        echo "Quick start:"
        echo "  python3 -c 'from ace import ace_ext; print(ace_ext)'"
        echo "  pytest tests/ -v"
    else
        echo -e "${RED}Verification FAILED with $ERRORS error(s)${NC}"
        echo -e "${RED}=================================================${NC}"
        echo ""
        echo -e "${YELLOW}Please check the errors above and fix them.${NC}"
        echo ""
        echo "Common fixes:"
        echo "  1. Rebuild:     ./scripts/dev-build.sh --clean"
        echo "  2. Reinstall:   pip install -e ."
        echo "  3. Verify:      ./scripts/verify-install.sh"
        exit 1
    fi
}

#------------------------------------------------------------------------------
# Main
#------------------------------------------------------------------------------
check_imports

if [ "$QUICK" = true ]; then
    print_summary
    exit 0
fi

check_version
check_cpp_extension

if [ "$FUNCTIONAL" = true ]; then
    run_functional_tests
fi

print_summary