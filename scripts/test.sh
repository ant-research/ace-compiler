#!/bin/bash
#
# ACE Test Runner Script
# Usage: ./scripts/test.sh [scenario] [options]
#
# Scenarios:
#   unit        - Run unit tests only
#   integration - Run integration tests only
#   regression  - Run regression tests only
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
#   -h, --help  - Show help message

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
        eval "$(build_cmd 'tests/unit/')"
        ;;

    integration)
        echo -e "${GREEN}Running Integration Tests...${NC}"
        eval "$(build_cmd 'tests/integration/')"
        ;;

    regression)
        echo -e "${GREEN}Running Regression Tests...${NC}"
        eval "$(build_cmd 'tests/regression/')"
        ;;

    dev)
        echo -e "${GREEN}Running Developer Workflow...${NC}"
        eval "$(build_cmd 'tests/unit/ tests/regression/')"
        ;;

    ci)
        echo -e "${GREEN}Running CI Workflow...${NC}"
        eval "$(build_cmd 'tests/unit/ tests/integration/ tests/regression/')"
        ;;

    nightly)
        echo -e "${GREEN}Running Nightly Workflow...${NC}"
        COVERAGE="--cov=ace --cov-report=html --cov-report=term"
        eval "$(build_cmd 'tests/')"
        ;;

    release)
        echo -e "${GREEN}Running Release Workflow...${NC}"
        COVERAGE="--cov=ace --cov-report=html --cov-fail-under=60"
        eval "$(build_cmd 'tests/ -v')"
        ;;

    all)
        echo -e "${GREEN}Running All Tests...${NC}"
        eval "$(build_cmd 'tests/')"
        ;;

    *)
        echo -e "${RED}Unknown scenario: $SCENARIO${NC}"
        echo "Available scenarios: unit, integration, regression, dev, ci, nightly, release, all"
        exit 1
        ;;
esac

echo -e "${GREEN}Tests completed successfully!${NC}"
