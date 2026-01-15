#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

# check if gmp is installed
function(find_extralib_gmp lib)
  find_library(GMP_LIBRARY NAMES gmp libgmp)
  if(GMP_LIBRARY)
    set(${lib} ${GMP_LIBRARY} PARENT_SCOPE)
  else()
    message(FATAL_ERROR "** ERROR ** libgmp is not installed")
  endif()
endfunction()

# check if math is installed
function(find_extralib_m lib)
  find_library(M_LIBRARY NAMES m)
  if(M_LIBRARY)
    set(${lib} ${M_LIBRARY} PARENT_SCOPE)
  else()
    message(FATAL_ERROR "** ERROR ** math is not installed")
  endif()
endfunction()

# check if openmp is installed
macro(find_extralib_openmp MESSAGE)
  if(BUILD_WITH_OPENMP)
    find_package(OpenMP)
    # OpenMP_CXX_FOUND was added in cmake 3.9.x so we are also checking
    # the OpenMP_FOUND flag
    if(OpenMP_CXX_FOUND OR OpenMP_FOUND)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
      set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    else()
      message(SEND_ERROR "** ERROR ** OpenMP is not installed")
    endif()

    if(OpenMP_C_FOUND OR OpenMP_FOUND)
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    endif()
  else()
    # Disable unknown #pragma omp warning
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unknown-pragmas")
  endif()
endmacro()

# Find the fhe-cmplr extra library
function(find_extralib_math lib)
  find_extralib_m(M_LIB)
  find_extralib_gmp(GMP_LIB)

  set(${lib}  ${GMP_LIB} ${M_LIB} PARENT_SCOPE)
endfunction()