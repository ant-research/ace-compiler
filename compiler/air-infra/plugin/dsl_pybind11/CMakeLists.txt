#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

cmake_minimum_required(VERSION 3.12)
project(air_dsl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# Find the Pybind11 library
find_package(pybind11 REQUIRED)

function(include_path dirname filename)
    find_path(${dirname} ${filename}
        PATHS /usr/local/include
        NO_DEFAULT_PATH
    )
    if(${dirname})
        message(STATUS "Found include dir for ${filename}: ${${dirname}}")
        include_directories(${${dirname}})
    else()
        message(FATAL_ERROR "Include dir for ${filename} not found!")
    endif()
endfunction()

include_path(CKKS_INCLUDE_DIR "fhe/ckks/ckks_ops.h")
include_path(NN_INCLUDE_DIR "nn/core/nn_ops.h")

# Build C++ code as a shared library
pybind11_add_module(air_dsl 
    src/air_dsl_bindings.cxx 
    src/py_airgen.cxx
    src/pycontext.cxx
    src/vector/vector_ops.cxx
    ${CKKS_INCLUDE_DIR}/fhe/ckks/ckks_ops.cxx
    ${NN_INCLUDE_DIR}/nn/core/nn_ops.cxx
)

if (pybind11_FOUND)
    message(STATUS "pybind11_INCLUDE_DIRS: ${pybind11_INCLUDE_DIRS}")
    message(STATUS "pybind11_LIBRARIES: ${pybind11_LIBRARIES}")
else ()
    message(FATAL_ERROR "pybind11 not found. Please make sure pybind11 is installed.")
endif ()

find_package(Python REQUIRED COMPONENTS Development)
if (Python_FOUND)
    message (STATUS "Python_INCLUDE_DIRS          : ${Python_INCLUDE_DIRS}")
    message (STATUS "Python_LIBRARIES             : ${Python_LIBRARIES}")	
else ()
    message(FATAL_ERROR "Python not found. Please make sure Python is installed.")
endif ()

# set include directories
include_directories(${Python_INCLUDE_DIRS})
include_directories(${pybind11_INCLUDE_DIRS}/include)
include_directories(${CMAKE_SOURCE_DIR}/include)

find_library (libvar AIRutil)
find_library (libvar AIRcore)
find_library (libvar AIRbase)
find_library (libvar NNvector)


if (${libvar} STREQUAL "libvar-NOTFOUND")
    message (FATAL_ERROR "required AIRutil library but not found!")
endif ()

set (AIR_LIBS ${AIR_LIBS} PUBLIC AIRbase PUBLIC AIRcore PUBLIC AIRutil)
set (NN_LIBS ${NN_LIBS} PUBLIC NNvector)

add_executable(add_vector_demo add_vector_demo.cxx)
target_link_options(add_vector_demo PRIVATE -Wl,--export-dynamic)
target_link_libraries(add_vector_demo PRIVATE ${AIR_LIBS} ${NN_LIBS} ${pybind11_LIBRARIES} ${Python_LIBRARIES})

# file(COPY ${CMAKE_BINARY_DIR}/air_dsl.cpython-38-aarch64-linux-gnu.so DESTINATION ${CMAKE_BINARY_DIR}/../)

#install(TARGETS air_dsl  DESTINATION ../)
install(FILES ${CMAKE_BINARY_DIR}/air_dsl.cpython-38-x86_64-linux-gnu.so DESTINATION ${CMAKE_BINARY_DIR}/../)
