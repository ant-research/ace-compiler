# ACE DSL (acepy) - Top-level CMakeLists.txt
#
# This CMakeLists.txt:
#   1. Calls the shared bindings build (../bindings/)
#   2. Optionally installs Python package
#
# Usage:
#   mkdir build && cd build
#   cmake ..
#   make -j$(nproc)
#
# Or use the build.sh script:
#   ./build.sh
#
# Prerequisites:
#   - ACE compiler built with: cd ace-compiler && ./build.sh release
#   - Python with pybind11: pip install pybind11

cmake_minimum_required(VERSION 3.18)
project(ace-dsl-python LANGUAGES CXX)

# Find Python
find_package(Python REQUIRED COMPONENTS Interpreter Development)

# ACE compiler paths
set(ACE_COMPILER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." CACHE PATH "Path to ace-compiler source")

# Shared bindings directory
set(BINDINGS_DIR "${ACE_COMPILER_DIR}/bindings")

if(NOT EXISTS "${BINDINGS_DIR}/CMakeLists.txt")
    message(FATAL_ERROR 
        "Shared bindings not found at ${BINDINGS_DIR}\n"
        "Expected directory structure:\n"
        "  ace-compiler/bindings/  (shared C++ bindings)\n"
        "  ace-compiler/acepy/     (this directory)")
endif()

message(STATUS "")
message(STATUS "=== ACE DSL (acepy) Configuration ===")
message(STATUS "Using shared bindings: ${BINDINGS_DIR}")
message(STATUS "")
message(STATUS "To build bindings:")
message(STATUS "  cd ${BINDINGS_DIR}/build && cmake .. && make")
message(STATUS "")
message(STATUS "Or use: ./build.sh")
message(STATUS "")
