#!/bin/bash
#
# ANT-ACE Test Runner Script
# Usage: ./scripts/test.sh [scenario] [options]
#
# Scenarios:
#   unit        - Run unit tests only
#   integration - Run integration tests only
#   regression  - Run regression tests only
#   coverage    - Run coverage tests only
#   dev         - Developer workflow (unit + regression)
#   ci          - CI workflow (unit + integration + regression)
#   nightly     - Nightly workflow (all tests with coverage)
#   release     - Release workflow (full validation with coverage gate)
#   all         - Run all tests
#
# Options:
#   -v          - Verbose output
#   --cov       - Enable coverage reporting
#   -m MARKER   - Run tests with specific marker
#   -h, --help  - Show this help message

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Default values
VERBOSE=""
COVERAGE=""
MARKER=""

# Help function
show_help() {
    head -20 "$0" | tail -18 | sed 's/^#//' | sed 's/^ //'
    exit 0
}

# Parse arguments
SCENARIO="${1:-dev}"
shift || true

while [[ $# -gt 0 ]]; do
    case $1 in
        -v|--verbose)
            VERBOSE="-v"
            shift
            ;;
        --cov)
            COVERAGE="--cov=ace --cov-report=html --cov-report=term"
            shift
            ;;
        -m)
            MARKER="-m $2"
            shift 2
            ;;
        -h|--help)
            show_help
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            show_help
            ;;
    esac
done

# Build pytest command
build_cmd() {
    local tests="$1"
    echo "pytest $tests $VERBOSE $COVERAGE $MARKER"
}

# Run tests based on scenario
case $SCENARIO in
    unit)
        echo -e "${GREEN}Running Unit Tests...${NC}"
        echo "# Fast, isolated tests for individual components"
        eval "$(build_cmd 'tests/test_unit/')"
        ;;

    integration)
        echo -e "${GREEN}Running Integration Tests...${NC}"
        echo "# Component interaction tests"
        eval "$(build_cmd 'tests/test_integration/')"
        ;;

    
    regression)
        echo -e "${GREEN}Running Regression Tests...${NC}"
        echo "# Deterministic tests with golden output comparison"
        eval "$(build_cmd 'tests/test_regression/')"
        ;;

    coverage)
        echo -e "${GREEN}Running Coverage Tests...${NC}"
        echo "# Random input tests for edge case discovery"
        eval "$(build_cmd 'tests/test_coverage/')"
        ;;

    dev)
        echo -e "${GREEN}Running Developer Workflow...${NC}"
        echo "# Quick feedback: unit + frontend tests"
        eval "$(build_cmd 'tests/test_unit/ tests/test_regression/')"
        ;;

    ci)
        echo -e "${GREEN}Running CI Workflow...${NC}"
        echo "# Comprehensive but fast: unit + integration + regression"
        eval "$(build_cmd 'tests/test_unit/ tests/test_integration/ tests/test_regression/')"
        ;;

    nightly)
        echo -e "${GREEN}Running Nightly Workflow...${NC}"
        echo "# Exhaustive testing with coverage"
        COVERAGE="--cov=ace --cov-report=html --cov-report=term"
        eval "$(build_cmd 'tests/')"
        ;;

    release)
        echo -e "${GREEN}Running Release Workflow...${NC}"
        echo "# Full validation with coverage gate (60%)"
        COVERAGE="--cov=ace --cov-report=html --cov-fail-under=60"
        eval "$(build_cmd 'tests/ -v')"
        ;;

    all)
        echo -e "${GREEN}Running All Tests...${NC}"
        eval "$(build_cmd 'tests/')"
        ;;

    *)
        echo -e "${RED}Unknown scenario: $SCENARIO${NC}"
        echo "Available scenarios: unit, integration, decorator, regression, coverage, dev, ci, nightly, release, all"
        exit 1
        ;;
esac

echo -e "${GREEN}Tests completed successfully!${NC}"