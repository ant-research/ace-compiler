#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

# cmake/AceDetect.cmake
# Centralized detection for CUDA, PyTorch ABI, and unified compile settings.
#
# Must be included BEFORE find_package(Torch) because it sets:
#   TORCH_CUDA_ARCH_LIST    — controls PyTorch's -gencode flag generation
#   CMAKE_CUDA_ARCHITECTURES — controls CMake's first-class CUDA arch selection
#   ACE_CUDA_AVAILABLE       — whether to build GPU targets (fhe_lib)
#
# After include(), the caller should do:
#   find_package(Torch REQUIRED)
#   # then restore CMAKE_CUDA_ARCHITECTURES (PyTorch sets it to OFF)
#
# Provides:
#   ACE_CUDA_AVAILABLE       — BOOL: whether CUDA toolkit is usable
#   ACE_CUDA_ARCHITECTURES   — List of CUDA arch numbers (e.g. 80;89;90)
#   USE_CXX11_ABI            — Always 1 (ABI=0 is no longer supported)
#   TORCH_PYTHON_LIBRARY     — Path to libtorch_python.so
#   ace_compile_settings     — INTERFACE library: ABI=1 + C++17 + PIC

# =========================================================================
# 1. CUDA detection
# =========================================================================
# Strategy:
#   - If CMAKE_CUDA_COMPILER is set (e.g. -DCMAKE_CUDA_COMPILER=... or
#     nvcc on PATH), try enable_language(CUDA) + find_package(CUDAToolkit).
#   - If nvcc is not found or toolkit detection fails, fall back to CPU-only.
#   - GPU build is opt-out: set -DACE_CUDA_AVAILABLE=OFF to force CPU-only.
#   - Supported GPUs: A100 (sm_80), 4090/L40 (sm_89), H20 (sm_90)
#     Note: sm_86 (A10/A30) is included as A100-compatible fallback.

if(DEFINED ACE_CUDA_AVAILABLE AND NOT ACE_CUDA_AVAILABLE)
    # User explicitly opted out
    message(STATUS "CUDA: disabled by user (-DACE_CUDA_AVAILABLE=OFF)")
    set(ACE_CUDA_AVAILABLE OFF CACHE BOOL "CUDA is available" FORCE)

else()
    # Auto-detect: check for nvcc, then try enable_language(CUDA)
    if(NOT CMAKE_CUDA_COMPILER)
        find_program(_ACE_NVCC nvcc PATHS /usr/local/cuda/bin NO_DEFAULT_PATH)
        find_program(_ACE_NVCC nvcc)
        if(_ACE_NVCC)
            set(CMAKE_CUDA_COMPILER "${_ACE_NVCC}" CACHE FILEPATH "CUDA compiler" FORCE)
        endif()
        unset(_ACE_NVCC CACHE)
    endif()

    if(CMAKE_CUDA_COMPILER)
        enable_language(CUDA)
        find_package(CUDAToolkit REQUIRED)
        set(ACE_CUDA_AVAILABLE ON CACHE BOOL "CUDA is available" FORCE)
        message(STATUS "CUDA: enabled (toolkit ${CUDAToolkit_VERSION})")
    else()
        message(STATUS "CUDA: nvcc not found — building CPU-only")
        set(ACE_CUDA_AVAILABLE OFF CACHE BOOL "CUDA is available" FORCE)
    endif()
endif()

# =========================================================================
# 2. CUDA architecture selection
# =========================================================================
# IMPORTANT: This MUST run before find_package(Torch) because PyTorch's
# cmake/cuda.cmake reads TORCH_CUDA_ARCH_LIST to generate -gencode flags.
# Without it, PyTorch auto-detects and may include deprecated archs
# (e.g. compute_50 removed in CUDA 13.0).
#
# Supported GPUs:
#   80 = A100 (SM80)
#   86 = A10, A30, A40 (SM86, compatible with SM80 binaries)
#   89 = RTX 4090, L40 (SM89)
#   90 = H100, H20 (SM90)

if(ACE_CUDA_AVAILABLE)
    set(CMAKE_CUDA_STANDARD 17)
    set(CMAKE_CUDA_STANDARD_REQUIRED ON)

    # Single source of truth for architectures.
    # User can override: -DACE_CUDA_ARCHITECTURES="80;89;90"
    set(ACE_CUDA_ARCHITECTURES "80;86;89;90" CACHE STRING
        "CUDA architectures: 80=A100, 86=A10/A40, 89=4090/L40, 90=H100/H20")

    # CMAKE_CUDA_ARCHITECTURES: controls CMake's first-class CUDA arch
    # selection (used by add_library/executable, sub-projects, etc.)
    # Derive from ACE_CUDA_ARCHITECTURES.  Use CACHE FORCE so that:
    #   - CMake's auto-detection (e.g. "75" from host GPU) is overridden
    #   - PyTorch's "set CMAKE_CUDA_ARCHITECTURES OFF" is overridden later
    set(CMAKE_CUDA_ARCHITECTURES ${ACE_CUDA_ARCHITECTURES} CACHE STRING
        "CUDA architectures (derived from ACE_CUDA_ARCHITECTURES)" FORCE)

    # TORCH_CUDA_ARCH_LIST: PyTorch's own mechanism (format: "8.0;8.6;8.9;9.0")
    # Must be set BEFORE find_package(Torch) to take effect.
    if(NOT DEFINED TORCH_CUDA_ARCH_LIST)
        set(TORCH_CUDA_ARCH_LIST "")
        foreach(_arch ${ACE_CUDA_ARCHITECTURES})
            # Convert "80" → "8.0", "89" → "8.9", "90" → "9.0"
            string(REGEX REPLACE "^([0-9])([0-9])$" "\\1.\\2" _arch_dot "${_arch}")
            list(APPEND TORCH_CUDA_ARCH_LIST "${_arch_dot}")
        endforeach()
    endif()

    message(STATUS "CUDA: toolkit ${CUDAToolkit_VERSION} at ${CUDAToolkit_INCLUDE_DIRS}")
    message(STATUS "CUDA: ACE_CUDA_ARCHITECTURES  = ${ACE_CUDA_ARCHITECTURES}")
    message(STATUS "CUDA: CMAKE_CUDA_ARCHITECTURES = ${CMAKE_CUDA_ARCHITECTURES}")
    message(STATUS "CUDA: TORCH_CUDA_ARCH_LIST     = ${TORCH_CUDA_ARCH_LIST}")
else()
    set(ACE_CUDA_ARCHITECTURES "" CACHE STRING "CUDA architectures (N/A — CPU-only build)")
endif()

# =========================================================================
# 3. CXX11 ABI — always 1
# =========================================================================
# The project only supports the CXX11 ABI (GLIBCXX_USE_CXX11_ABI=1).
# All pre-built PyTorch wheels from PyPI use ABI=1 since PyTorch 2.0+.
set(USE_CXX11_ABI 1 CACHE STRING "CXX11 ABI value (only 1 is supported)" FORCE)
message(STATUS "USE_CXX11_ABI: ${USE_CXX11_ABI} (fixed)")

# =========================================================================
# 4. Locate torch_python library
# =========================================================================
# Not provided by find_package(Torch); needed for pybind11 extensions.
# Done here (before find_package(Torch)) so it's available to sub-projects.
find_library(TORCH_PYTHON_LIBRARY
    NAMES torch_python
    PATHS "${TORCH_INSTALL_PREFIX}/lib"
    NO_DEFAULT_PATH
)
if(NOT TORCH_PYTHON_LIBRARY)
    # Retry after find_package(Torch) sets TORCH_INSTALL_PREFIX
    # Store flag so caller can retry.
    set(_ACE_TORCH_PYTHON_PENDING ON)
else()
    message(STATUS "torch_python: ${TORCH_PYTHON_LIBRARY}")
endif()

# =========================================================================
# 5. Global C++ standard & PIC
# =========================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Keep CMAKE_CXX_EXTENSIONS at compiler default (GNU extensions ON for GCC).
# Setting OFF (-std=c++17 instead of -std=gnu++17) breaks air-infra's
# CXX_MEM_ALLOCATOR which relies on GNU extensions for allocator compat.

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# =========================================================================
# 6. INTERFACE library: unified compile settings
# =========================================================================
# Targets only need: target_link_libraries(<target> PRIVATE ace_compile_settings)
# to inherit _GLIBCXX_USE_CXX11_ABI and C++17.
add_library(ace_compile_settings INTERFACE)
target_compile_definitions(ace_compile_settings INTERFACE
    _GLIBCXX_USE_CXX11_ABI=${USE_CXX11_ABI}
)

message(STATUS "ace_compile_settings: ABI=${USE_CXX11_ABI}, C++17, PIC=ON")
