#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

# Source files from reorganized directories (core, ops, layers)
file(GLOB CORE_SOURCES   "core/*.cxx")
file(GLOB OPS_SOURCES    "ops/*.cxx")
file(GLOB LAYERS_SOURCES "layers/*.cxx")
set(DSL_SOURCES ${CORE_SOURCES} ${OPS_SOURCES} ${LAYERS_SOURCES})

pybind11_add_module(frontend pybind_extension.cxx ${DSL_SOURCES})

# ABI compatibility: must match PyTorch
target_compile_options(frontend PRIVATE
    -D_GLIBCXX_USE_CXX11_ABI=${USE_CXX11_ABI}
    -fno-stack-protector
)

# Match compiler_shared visibility settings for ABI compatibility
set_target_properties(frontend PROPERTIES
    CXX_VISIBILITY_PRESET default
    VISIBILITY_INLINES_HIDDEN OFF
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    INSTALL_RPATH "$ORIGIN/lib"
    BUILD_RPATH "${CMAKE_BINARY_DIR}/rtlib/build/common;${CMAKE_BINARY_DIR}/rtlib/build/ant"
    INSTALL_RPATH_USE_LINK_PATH TRUE
)

# nn_od generates nn/core/opcode.h at build time
add_dependencies(frontend nn_od)
add_dependencies(frontend fhe_rtlib)

target_include_directories(frontend PRIVATE
    ${Python_INCLUDE_DIRS}
    ${TORCH_INCLUDE_DIRS}
    $<TARGET_PROPERTY:compiler_shared,INTERFACE_INCLUDE_DIRECTORIES>
    include
)

target_link_libraries(frontend PRIVATE
    "${TORCH_LIBRARIES}"
    "${TORCH_PYTHON_LIBRARY}"
    -Wl,--no-as-needed
    FHEdriver
    FHEpoly
    FHEckks
    FHEsihe
    FHEopt
    FHEcore
    FHEutil
    NNdriver
    NNvector
    NNopt
    NNcore
    NNutil
    AIRdriver
    AIRopt
    AIRcore
    AIRbase
    AIRutil
    FHErt_ant
    FHErt_common
    gmp
    m
    -Wl,--as-needed
    ace_common
)

install(TARGETS frontend
    LIBRARY DESTINATION ace
    COMPONENT core
)