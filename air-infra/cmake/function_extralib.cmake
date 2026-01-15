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

# Find the python library
function(find_python_lib MESSAGE)
  find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
  if(Python3_FOUND)
    message(STATUS "Found Python3                 : ${Python3_EXECUTABLE} (found version \"${Python3_VERSION}\")")
    message(STATUS "Python3_INCLUDE_DIRS          : ${Python3_INCLUDE_DIRS}")
    message(STATUS "Python3_LIBRARIES             : ${Python3_LIBRARIES}")
  else()
    message(FATAL_ERROR "Python not found. Please make sure Python is installed.")
  endif()

  include_directories(${Python3_INCLUDE_DIRS})
  set(Python3_LIBRARIES ${Python3_LIBRARIES} PARENT_SCOPE)
endfunction()

# Find the Pybind11 library
function(find_pybind11_lib MESSAGE)
  find_package(pybind11 REQUIRED)

  if(pybind11_FOUND)
    message(STATUS "pybind11_INCLUDE_DIRS         : ${pybind11_INCLUDE_DIRS}")
    message(STATUS "pybind11_LIBRARIES            : ${pybind11_LIBRARIES}")
  else()
    message(FATAL_ERROR "pybind11 not found. Please make sure pybind11 is installed.")
  endif()

  include_directories(${pybind11_INCLUDE_DIRS}/include)
  set(pybind11_LIBRARIES ${pybind11_LIBRARIES} PARENT_SCOPE)
endfunction()

# Find the air-infra extra libraries
function(find_extralib_python)
  find_python_lib("find libs: python")
  find_pybind11_lib("find libs: pybind11")

  set(EXTRA_PYTHON_LIBS ${EXTRA_PYTHON_LIBS} ${pybind11_LIBRARIES} ${Python3_LIBRARIES})
  set(EXTRA_PYTHON_LIBS ${EXTRA_PYTHON_LIBS} CACHE STRING "Global common extra libs")
endfunction()

# Find the fhe-cmplr extra library
function(find_extralib_math lib)
  find_extralib_m(M_LIB)
  find_extralib_gmp(GMP_LIB)

  set(${lib}  ${GMP_LIB} ${M_LIB} PARENT_SCOPE)
endfunction()