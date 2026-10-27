#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

#[=======================================================================[.rst:
FindcuFile
----------

Find the NVIDIA cuFile (GPUDirect Storage / GDS) headers.

This module is used to locate the cuFile headers at **compile time**.
cuFile is linked at **runtime** via dlopen (see cufile_api.h), so this
module only provides include directories, never link libraries.

Result Variables
^^^^^^^^^^^^^^^^

``cuFile_FOUND``
  True if the cuFile header was found.
``CUFILE_INCLUDE_DIR``
  Directory containing cufile.h.

Imported Targets
^^^^^^^^^^^^^^^^

``cuFile::cufile``
  INTERFACE IMPORTED target — only adds include directories.
  Does NOT link libcufile.so (dlopen handles runtime loading).

Search Strategy
^^^^^^^^^^^^^^^

1. If CMake 3.28+ and ``find_package(CUDAToolkit)`` has already been
   called, check for the ``CUDA::cufile`` imported target.
2. Otherwise, search under ``CUDAToolkit_INCLUDE_DIRS`` and common
   CUDA toolkit paths for ``cufile.h``.
#]=======================================================================]

# Strategy 1: CMake 3.28+ provides CUDA::cufile imported target
if(TARGET CUDA::cufile)
    if(NOT TARGET cuFile::cufile)
        add_library(cuFile::cufile ALIAS CUDA::cufile)
    endif()
    set(cuFile_FOUND TRUE)
    set(CUFILE_INCLUDE_DIR "")
    return()
endif()

# Strategy 2: Search for cufile.h under CUDA toolkit paths
# Ensure CUDAToolkit has been found (for CUDAToolkit_INCLUDE_DIRS)
if(NOT CUDAToolkit_FOUND)
    find_package(CUDAToolkit QUIET)
endif()

# Search paths: CUDAToolkit directories + common locations
set(_cufile_search_paths)
if(CUDAToolkit_INCLUDE_DIRS)
    list(APPEND _cufile_search_paths ${CUDAToolkit_INCLUDE_DIRS})
endif()
if(CUDAToolkit_ROOT_DIR)
    list(APPEND _cufile_search_paths
        "${CUDAToolkit_ROOT_DIR}/include"
        "${CUDAToolkit_ROOT_DIR}/gds/include"
    )
endif()
if(DEFINED ENV{CUDA_HOME})
    list(APPEND _cufile_search_paths
        "$ENV{CUDA_HOME}/include"
        "$ENV{CUDA_HOME}/gds/include"
    )
endif()
# Standard CUDA install locations
list(APPEND _cufile_search_paths
    "/usr/local/cuda/include"
    "/usr/local/cuda/gds/include"
    "/usr/include"
)

find_path(CUFILE_INCLUDE_DIR
    NAMES cufile.h
    PATHS ${_cufile_search_paths}
    PATH_SUFFIXES gds
    NO_DEFAULT_PATH
)

# Also search default paths as a last resort
if(NOT CUFILE_INCLUDE_DIR)
    find_path(CUFILE_INCLUDE_DIR
        NAMES cufile.h
        PATH_SUFFIXES gds
    )
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(cuFile
    REQUIRED_VARS CUFILE_INCLUDE_DIR
)

mark_as_advanced(CUFILE_INCLUDE_DIR)

# Create INTERFACE IMPORTED target — include only, no link
# cuFile is loaded at runtime via dlopen, so we never link libcufile.so
if(cuFile_FOUND AND NOT TARGET cuFile::cufile)
    add_library(cuFile::cufile INTERFACE IMPORTED)
    if(CUFILE_INCLUDE_DIR)
        set_target_properties(cuFile::cufile PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${CUFILE_INCLUDE_DIR}"
        )
    endif()
endif()

# Clean up
unset(_cufile_search_paths)
