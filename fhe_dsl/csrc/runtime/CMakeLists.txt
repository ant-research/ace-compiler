#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

file(GLOB_RECURSE RUNTIME_SOURCES "src/*.cxx")

pybind11_add_module(runtime pybind_extension.cxx ${RUNTIME_SOURCES})

add_dependencies(runtime fhe_rtlib)

# Define RTLIB_SUPPORT_DYNAMIC for Python runtime (uses callback mechanism)
target_compile_definitions(runtime PRIVATE RTLIB_SUPPORT_DYNAMIC)

target_include_directories(runtime PRIVATE
    ${Python_INCLUDE_DIRS}
    ${TORCH_INCLUDE_DIRS}
    $<TARGET_PROPERTY:compiler_shared,INTERFACE_INCLUDE_DIRECTORIES>
    include
)

# Link against rtlib shared library (for Python extension)
link_directories(${CMAKE_INSTALL_PREFIX}/ace/lib)

target_link_libraries(runtime PRIVATE
    "${TORCH_LIBRARIES}"
    "${TORCH_PYTHON_LIBRARY}"
    FHErt_common_shared
)

set_target_properties(runtime PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    INSTALL_RPATH "$ORIGIN/lib"
    BUILD_RPATH "${CMAKE_BINARY_DIR}/rtlib/build/common"
    INSTALL_RPATH_USE_LINK_PATH TRUE
)

install(TARGETS runtime
    LIBRARY DESTINATION ace
    COMPONENT runtime
)