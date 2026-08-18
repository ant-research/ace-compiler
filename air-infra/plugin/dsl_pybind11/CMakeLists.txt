cmake_minimum_required(VERSION 3.12)
project(air_dsl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# Find the Pybind11 library
find_package(pybind11 REQUIRED)

# Build C++ code as a shared library
pybind11_add_module(air_dsl 
    src/air_dsl_bindings.cxx 
    src/dsl.cxx
    src/vector/vector_ops.cpp
)

# set include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

find_library (libvar AIRutil)
find_library (libvar AIRbase)
find_library (libvar AIRcore)
find_library (libvar AIRdriver)
find_library (libvar AIRopt)
find_library (libvar AIRcg)
find_library (libvar AIRplugin)
find_library (libvar NNvector)

if (${libvar} STREQUAL "libvar-NOTFOUND")
    message (FATAL_ERROR "required AIRutil library but not found!")
endif ()

set (AIR_LIBS ${AIR_LIBS} PUBLIC AIRplugin AIRcg PUBLIC AIRopt PUBLIC AIRdriver PUBLIC AIRcore PUBLIC AIRbase PUBLIC AIRutil)
set (NN_LIBS ${NN_LIBS} PUBLIC NNvector)
target_link_libraries (air_dsl PRIVATE ${AIR_LIBS} ${NN_LIBS})

# file(COPY ${CMAKE_BINARY_DIR}/air_dsl.cpython-38-aarch64-linux-gnu.so DESTINATION ${CMAKE_BINARY_DIR}/../)

#install(TARGETS air_dsl  DESTINATION ../)

install(FILES ${CMAKE_BINARY_DIR}/air_dsl.cpython-38-aarch64-linux-gnu.so DESTINATION ${CMAKE_BINARY_DIR}/../)