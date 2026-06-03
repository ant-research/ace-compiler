#=============================================================================
#
# ACE-Compiler Development Makefile
#
# Usage:
#   make          - Build and install (default)
#   make build    - Build only
#   make install  - Install to site-packages
#   make clean    - Clean build directory
#   make test     - Run tests
#   make help     - Show help
#
#=============================================================================

# Configuration
PROJECT_ROOT := $(shell pwd)

# Allow override: make BUILD=build-debug
BUILD       ?= build
BUILD_DIR   := $(BUILD)

# Build type: Debug for development, Release for production
CMAKE_BUILD_TYPE ?= Debug

SITE_PACKAGES := $(shell python3 -c 'import sysconfig; print(sysconfig.get_path("purelib"))')
PYTHON := python3
PIP    := pip

# Colors
BLUE   := \033[0;34m
GREEN  := \033[0;32m
YELLOW := \033[1;33m
NC     := \033[0m

.PHONY: all build install clean test test-unit test-frontend test-regression test-integration test-full help configure rebuild setup-deps verify verify-quick verify-functional quick-build check-path check-install

#------------------------------------------------------------------------------
# Default target
#------------------------------------------------------------------------------
all: build install

#------------------------------------------------------------------------------
# Build targets
#------------------------------------------------------------------------------

configure:
	@echo -e "$(BLUE)Configuring CMake...$(NC)"
	@cmake -S $(PROJECT_ROOT) -B $(BUILD_DIR) \
	    -DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE) \
	    -DCMAKE_INSTALL_PREFIX=$(SITE_PACKAGES) \
	    -DBUILD_EXTENSION=ON \
	    -DBUILD_TESTS=OFF

build: configure
	@echo -e "$(BLUE)Building...$(NC)"
	@cmake --build $(BUILD_DIR) -j$$(nproc)

rebuild: clean build install

install:
	@echo -e "$(BLUE)Installing to $(SITE_PACKAGES)/ace...$(NC)"
	@mkdir -p $(SITE_PACKAGES)/ace
	@cmake --install $(BUILD_DIR) --prefix $(SITE_PACKAGES) --component frontend
	@cmake --install $(BUILD_DIR) --prefix $(SITE_PACKAGES) --component core
	@cmake --install $(BUILD_DIR) --prefix $(SITE_PACKAGES) --component runtime
	@echo -e "$(GREEN)Install complete.$(NC)"
	@echo ""
	@echo -e "$(YELLOW)Installed files:$(NC)"
	@ls -la $(SITE_PACKAGES)/ace/ 2>/dev/null || echo "  ace/ not found"
	@echo ""
	@echo -e "$(YELLOW)C++ extensions:$(NC)"
	@ls -la $(SITE_PACKAGES)/ace/*.so 2>/dev/null || echo "  No .so files found"

clean:
	@echo -e "$(YELLOW)Cleaning $(BUILD_DIR)...$(NC)"
	@rm -rf $(BUILD_DIR)
	@echo -e "$(GREEN)Clean complete.$(NC)"

#------------------------------------------------------------------------------
# Test targets
#------------------------------------------------------------------------------

test: test-unit

test-unit:
	@echo -e "$(BLUE)Running unit tests...$(NC)"
	@$(PYTHON) -m pytest tests/unit/ -v --tb=short

test-frontend:
	@echo -e "$(BLUE)Running frontend tests...$(NC)"
	@$(PYTHON) -m pytest tests/unit/frontend/ -v --tb=short

test-regression:
	@echo -e "$(BLUE)Running regression tests...$(NC)"
	@$(PYTHON) -m pytest tests/regression/ -v --tb=short

test-integration:
	@echo -e "$(BLUE)Running integration tests...$(NC)"
	@$(PYTHON) -m pytest tests/integration/ -v --tb=short

test-full:
	@echo -e "$(BLUE)Running all tests...$(NC)"
	@$(PYTHON) -m pytest tests/ -v --tb=short

#------------------------------------------------------------------------------
# Development helpers
#------------------------------------------------------------------------------

# Setup dependencies
setup-deps:
	@echo -e "$(BLUE)Setting up dependencies...$(NC)"
	@./scripts/setup-deps.sh

# Verify installation
verify:
	@echo -e "$(BLUE)Verifying installation...$(NC)"
	@./scripts/verify-install.sh

verify-quick:
	@./scripts/verify-install.sh --quick

verify-functional:
	@./scripts/verify-install.sh --functional

# Quick rebuild (for when only C++ code changed)
quick-build:
	@echo -e "$(BLUE)Quick rebuild (incremental)...$(NC)"
	@cmake --build $(BUILD_DIR) -j$$(nproc)
	@cmake --install $(BUILD_DIR) --prefix $(SITE_PACKAGES) --component frontend
	@cmake --install $(BUILD_DIR) --prefix $(SITE_PACKAGES) --component core
	@cmake --install $(BUILD_DIR) --prefix $(SITE_PACKAGES) --component runtime
	@echo -e "$(GREEN)Quick build complete.$(NC)"

# Check PYTHONPATH
check-path:
	@echo -e "$(YELLOW)Current PYTHONPATH:$(NC)"
	@echo "$$PYTHONPATH"
	@echo ""
	@echo -e "$(YELLOW)Python search paths:$(NC)"
	@$(PYTHON) -c "import sys; print('\n'.join(sys.path))"

# Verify installation
check-install:
	@echo -e "$(BLUE)Checking installation...$(NC)"
	@echo -e "$(YELLOW)Core package:$(NC)"
	@$(PYTHON) -c "import ace; print('  ace:', ace.__version__)" 2>&1 || echo "  Failed to import ace"
	@$(PYTHON) -c "from ace import fhe; print('  ace.fhe: OK')" 2>&1 || echo "  Failed to import ace.fhe"
	@echo -e "$(YELLOW)FHE utilities:$(NC)"
	@$(PYTHON) -c "from ace.fhe.util import get_logger; print('  ace.fhe.util: OK')" 2>&1 || echo "  Failed to import ace.fhe.util"
	@echo -e "$(YELLOW)C++ extensions:$(NC)"
	@$(PYTHON) -c "from ace import frontend; print('  ace.frontend: OK')" 2>&1 || echo "  ace.frontend: not available"
	@$(PYTHON) -c "from ace import runtime; print('  ace.runtime: OK')" 2>&1 || echo "  ace.runtime: not available"

#------------------------------------------------------------------------------
# Help
#------------------------------------------------------------------------------

help:
	@echo "ACE-Compiler Development Makefile"
	@echo ""
	@echo "Usage: make [target]"
	@echo ""
	@echo "Build Targets:"
	@echo "  all            - Build and install (default)"
	@echo "  configure      - Run CMake configuration"
	@echo "  build          - Build C++ extensions only"
	@echo "  install        - Install to site-packages"
	@echo "  rebuild        - Clean, build, and install"
	@echo "  clean          - Remove build directory"
	@echo "  quick-build    - Incremental rebuild (faster)"
	@echo ""
	@echo "Setup & Verify:"
	@echo "  setup-deps     - Install all dependencies"
	@echo "  verify         - Full installation verification"
	@echo "  verify-quick   - Quick import check"
	@echo "  verify-functional - Run functional tests"
	@echo ""
	@echo "Test Targets:"
	@echo "  test           - Run unit tests (default)"
	@echo "  test-unit      - Run unit tests only"
	@echo "  test-frontend  - Run frontend tests only"
	@echo "  test-regression - Run regression tests only"
	@echo "  test-integration - Run integration tests only"
	@echo "  test-full      - Run all tests"
	@echo ""
	@echo "Helper Targets:"
	@echo "  check-path     - Show PYTHONPATH and Python search paths"
	@echo "  check-install  - Verify installation"
	@echo "  help           - Show this help"
	@echo ""
	@echo "Examples:"
	@echo "  make                        # Full build and install"
	@echo "  make setup-deps             # Install dependencies"
	@echo "  make verify                 # Verify installation"
	@echo "  make quick-build            # Fast incremental build"
	@echo "  make test-frontend          # Test frontend"
	@echo "  make BUILD=build-rel        # Use different build dir"
	@echo "  make CMAKE_BUILD_TYPE=Release  # Release build"