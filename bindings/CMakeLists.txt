#
# CMakeLists.txt for ACE Bindings (Shared C++ Python Bindings)
#
# Usage:
#   mkdir build && cd build
#   cmake .. -DACE_COMPILER_DIR=/path/to/ace-compiler \
#            -DACE_INSTALL_DIR=/path/to/ace_cmplr \
#            -Dpybind11_DIR=$(python -c "import pybind11; print(pybind11.get_cmake_dir())")
#   make
#
# This will build and install to ace_bindings/:
#   - air_builder.so    : AIR node builder bindings
#   - nn_addon.so       : NN domain operations bindings
#   - fhe_cmplr.so      : FHE compilation passes bindings
#   - passmanager.so    : Pass manager bindings

cmake_minimum_required(VERSION 3.14)
project(ace_bindings LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Find pybind11
find_package(pybind11 REQUIRED)

# ACE-compiler paths
set(ACE_COMPILER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." CACHE PATH "Path to ace-compiler source")
set(ACE_INSTALL_DIR "${ACE_COMPILER_DIR}/ace_cmplr" CACHE PATH "Path to ace_cmplr install")

# Output directory for .so files - directly into ace_bindings package
set(ACE_BINDINGS_OUTPUT_DIR "${ACE_COMPILER_DIR}/ace_bindings" CACHE PATH "Output directory for .so files")

set(AIR_INFRA_DIR "${ACE_COMPILER_DIR}/air-infra")
set(NN_ADDON_DIR "${ACE_COMPILER_DIR}/nn-addon")
set(FHE_CMPLR_DIR "${ACE_COMPILER_DIR}/fhe-cmplr")
set(ACE_LIB_DIR "${ACE_INSTALL_DIR}/lib")
set(ACE_INCLUDE_DIR "${ACE_INSTALL_DIR}/include")

# Check if ACE libraries are available
if(EXISTS "${ACE_LIB_DIR}/libAIRbase.a")
    set(ACE_BINDINGS_ENABLED ON)
    message(STATUS "Found ACE libraries at ${ACE_LIB_DIR}")
    message(STATUS "Building ACE bindings")
else()
    set(ACE_BINDINGS_ENABLED OFF)
    message(WARNING "ACE libraries not found at ${ACE_LIB_DIR}")
    message(FATAL_ERROR "ACE libraries required for building bindings")
endif()

# Find protobuf for ONNX support
find_package(Protobuf QUIET)
if(Protobuf_FOUND)
    message(STATUS "Found Protobuf: ${Protobuf_VERSION}")
    set(ONNX_SUPPORT_ENABLED ON)
else()
    message(STATUS "Protobuf not found - ONNX loading will be disabled")
    set(ONNX_SUPPORT_ENABLED OFF)
endif()

# ONNX proto generated files location (from nn-addon build)
# Auto-detect from common build directories, or allow user override
set(ONNX_PROTO_DIR "" CACHE PATH "Path to generated ONNX proto files (auto-detected if empty)")

if(NOT ONNX_PROTO_DIR OR NOT EXISTS "${ONNX_PROTO_DIR}/onnx.pb.h")
    # Search common build directories
    set(_ONNX_SEARCH_PATHS
        "${CMAKE_CURRENT_BINARY_DIR}/../../nn-addon/onnx2air"  # If building inside ace-compiler build tree
        "${ACE_COMPILER_DIR}/release/nn-addon/onnx2air"
        "${ACE_COMPILER_DIR}/release_openmp/nn-addon/onnx2air"
        "${ACE_COMPILER_DIR}/debug/nn-addon/onnx2air"
        "${ACE_COMPILER_DIR}/debug_openmp/nn-addon/onnx2air"
        "${ACE_COMPILER_DIR}/build/nn-addon/onnx2air"
    )
    foreach(_path ${_ONNX_SEARCH_PATHS})
        # Some builds generate onnx.pb.h directly under onnx2air/
        if(EXISTS "${_path}/onnx.pb.h")
            set(ONNX_PROTO_DIR "${_path}")
            message(STATUS "Found onnx.pb.h at: ${ONNX_PROTO_DIR}")
            break()
        endif()
        # Other builds generate it under onnx2air/onnx/
        if(EXISTS "${_path}/onnx/onnx.pb.h")
            set(ONNX_PROTO_DIR "${_path}/onnx")
            message(STATUS "Found onnx.pb.h at: ${ONNX_PROTO_DIR}")
            break()
        endif()
    endforeach()
endif()

if(NOT EXISTS "${ONNX_PROTO_DIR}/onnx.pb.h")
    message(WARNING "onnx.pb.h not found. ONNX loading may fail. Set ONNX_PROTO_DIR to the directory containing onnx.pb.h")
endif()

# Include directories
include_directories(
    ${AIR_INFRA_DIR}/include
    ${NN_ADDON_DIR}/include
    ${FHE_CMPLR_DIR}/include
    ${ACE_INCLUDE_DIR}
    ${ONNX_PROTO_DIR}
)

# Set output directory for all modules
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ACE_BINDINGS_OUTPUT_DIR})

# ═══════════════════════════════════════════════════════════════════════════════
# air_builder module - Core AIR infrastructure bindings
# ═══════════════════════════════════════════════════════════════════════════════
pybind11_add_module(air_builder 
    src/air_builder_bindings.cpp
    src/onnx_loader.cpp
)
target_include_directories(air_builder PRIVATE 
    ${AIR_INFRA_DIR}/include
    ${ACE_INCLUDE_DIR}
)

if(ACE_BINDINGS_ENABLED)
    target_compile_definitions(air_builder PRIVATE ACE_BINDINGS_ENABLED=1)
    target_link_directories(air_builder PRIVATE ${ACE_LIB_DIR} ${ACE_INSTALL_DIR}/rtlib/lib)
    # Order matters for static libraries - dependent libs come first
    # Include NNonnx2air and NNonnx for ONNX model loading
    target_link_libraries(air_builder PRIVATE 
        NNonnx2air NNonnx NNopt NNutil
        NNvector NNcore NNdriver
        FHEsihe FHEckks FHEpoly FHEcore FHEutil FHEdriver
        FHErt_ant_encode FHErt_common
        AIRopt AIRdriver AIRcore AIRutil AIRbase
        gmp
        -lstdc++
    )
    # Add protobuf for ONNX support
    if(ONNX_SUPPORT_ENABLED)
        target_link_libraries(air_builder PRIVATE ${Protobuf_LIBRARIES})
        target_include_directories(air_builder PRIVATE ${Protobuf_INCLUDE_DIRS})
    endif()
endif()

# ═══════════════════════════════════════════════════════════════════════════════
# nn_addon module - Neural network domain bindings
# ═══════════════════════════════════════════════════════════════════════════════
pybind11_add_module(nn_addon src/nn_addon_bindings.cpp)
target_include_directories(nn_addon PRIVATE 
    ${AIR_INFRA_DIR}/include
    ${NN_ADDON_DIR}/include
    ${ACE_INCLUDE_DIR}
)

if(ACE_BINDINGS_ENABLED)
    target_compile_definitions(nn_addon PRIVATE ACE_BINDINGS_ENABLED=1)
    target_link_directories(nn_addon PRIVATE ${ACE_LIB_DIR})
    target_link_libraries(nn_addon PRIVATE 
        NNcore NNvector NNdriver
        AIRbase AIRcore AIRutil
    )
endif()

# ═══════════════════════════════════════════════════════════════════════════════
# fhe_cmplr module - FHE compilation passes bindings
# ═══════════════════════════════════════════════════════════════════════════════
pybind11_add_module(fhe_cmplr src/fhe_cmplr_bindings.cpp)
target_include_directories(fhe_cmplr PRIVATE 
    ${AIR_INFRA_DIR}/include
    ${FHE_CMPLR_DIR}/include
    ${ACE_INCLUDE_DIR}
)

if(ACE_BINDINGS_ENABLED)
    target_compile_definitions(fhe_cmplr PRIVATE ACE_BINDINGS_ENABLED=1)
    target_link_directories(fhe_cmplr PRIVATE ${ACE_LIB_DIR})
    target_link_libraries(fhe_cmplr PRIVATE 
        FHEsihe FHEckks FHEpoly FHEcore FHEdriver
        AIRbase AIRcore AIRutil
        gmp
    )
endif()

# ═══════════════════════════════════════════════════════════════════════════════
# passmanager module - Pass manager bindings
# ═══════════════════════════════════════════════════════════════════════════════
pybind11_add_module(passmanager src/passmanager_bindings.cpp)
target_include_directories(passmanager PRIVATE 
    ${AIR_INFRA_DIR}/include
    ${ACE_INCLUDE_DIR}
)

if(ACE_BINDINGS_ENABLED)
    target_compile_definitions(passmanager PRIVATE ACE_BINDINGS_ENABLED=1)
    target_link_directories(passmanager PRIVATE ${ACE_LIB_DIR})
    target_link_libraries(passmanager PRIVATE 
        AIRdriver AIRbase AIRutil
    )
endif()

# ═══════════════════════════════════════════════════════════════════════════════
# Installation (optional - modules are already output to ace_bindings/)
# ═══════════════════════════════════════════════════════════════════════════════
install(TARGETS air_builder nn_addon fhe_cmplr passmanager
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/ace_bindings
)

# Summary
message(STATUS "")
message(STATUS "=== ACE Bindings Configuration ===")
message(STATUS "ACE_COMPILER_DIR:       ${ACE_COMPILER_DIR}")
message(STATUS "ACE_INSTALL_DIR:        ${ACE_INSTALL_DIR}")
message(STATUS "ACE_LIB_DIR:            ${ACE_LIB_DIR}")
message(STATUS "ACE_BINDINGS_OUTPUT:    ${ACE_BINDINGS_OUTPUT_DIR}")
message(STATUS "ACE_BINDINGS:           ${ACE_BINDINGS_ENABLED}")
message(STATUS "")

