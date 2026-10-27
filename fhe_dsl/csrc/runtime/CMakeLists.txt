#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

file(GLOB_RECURSE RUNTIME_SOURCES "src/*.cxx")

# Exclude log.cxx (replaced by spdlog)
list(FILTER RUNTIME_SOURCES EXCLUDE REGEX ".*log\\.cxx$")

pybind11_add_module(runtime pybind_extension.cxx ${RUNTIME_SOURCES})

add_dependencies(runtime fhe_rtlib)

# Define RTLIB_SUPPORT_DYNAMIC for Python runtime (uses callback mechanism)
target_compile_definitions(runtime PRIVATE RTLIB_SUPPORT_DYNAMIC)

# Enable GPU memory profiling and NVTX annotations when CUDA is available
if(ACE_CUDA_AVAILABLE)
    target_compile_definitions(runtime PRIVATE USE_CUDA)
    target_include_directories(runtime PRIVATE ${CUDAToolkit_INCLUDE_DIRS})
    target_link_libraries(runtime PRIVATE CUDA::cudart)
    # NVTX for profiling annotations (visible in nsys and torch.profiler)
    # CUDA 12.9+ removed libnvToolsExt; use the header-only nvtx3 target instead.
    if(TARGET CUDA::nvtx3)
        target_link_libraries(runtime PRIVATE CUDA::nvtx3)
    elseif(TARGET CUDA::nvToolsExt)
        target_link_libraries(runtime PRIVATE CUDA::nvToolsExt)
    else()
        message(WARNING "Neither CUDA::nvtx3 nor CUDA::nvToolsExt found; NVTX profiling will be disabled")
    endif()
endif()

find_package(OpenMP)
find_package(Threads REQUIRED)

# Unified compile settings (ABI, C++17) from AceDetect
target_link_libraries(runtime PRIVATE ace_compile_settings)

target_include_directories(runtime PRIVATE
    ${Python_INCLUDE_DIRS}
    ${TORCH_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../fhe_lib/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../fhe_lib/common
    include
)

# Link against rtlib shared library (for Python extension)
target_link_directories(runtime PRIVATE ${CMAKE_INSTALL_PREFIX}/ace/lib)

target_link_libraries(runtime PRIVATE
    "${TORCH_LIBRARIES}"
    "${TORCH_PYTHON_LIBRARY}"
    ace_compile_settings
    compiler_shared
    FHErt_common_shared
    FHErt_io
    ace_common
    Threads::Threads
    ${CMAKE_DL_LIBS}
)

if(OpenMP_CXX_FOUND)
    target_link_libraries(runtime PRIVATE OpenMP::OpenMP_CXX)
endif()

set_target_properties(runtime PROPERTIES
    INSTALL_RPATH "$ORIGIN/lib"
    BUILD_RPATH "${CMAKE_BINARY_DIR}/rtlib/build/common;${CMAKE_BINARY_DIR}/fhe_lib/common"
    INSTALL_RPATH_USE_LINK_PATH TRUE
)

install(TARGETS runtime
    LIBRARY DESTINATION ace
    COMPONENT runtime
)