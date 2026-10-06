#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

# Build external benchmark project dependent function
function(build_external_benchmark)

  # Default: upstream GitHub; internal override via .aci/
  if(NOT DEFINED BENCH_URL)
    set(BENCH_URL "https://github.com/google/benchmark.git")
  endif()

  message(STATUS "Cloning External Repository    : ${BENCH_URL}")

  include(ExternalProject)
  ExternalProject_Add(
    googlebenchmark
    GIT_REPOSITORY ${BENCH_URL}
    GIT_TAG v1.8.0
    PREFIX ${CMAKE_BINARY_DIR}/external
    CMAKE_ARGS  -DCMAKE_BUILD_TYPE=Release
                -DBENCHMARK_DOWNLOAD_DEPENDENCIES=OFF
                -DBENCHMARK_ENABLE_TESTING=OFF
                -DBENCHMARK_ENABLE_GTEST_TESTS=OFF
    UPDATE_COMMAND ""
    BUILD_ALWAYS OFF
    INSTALL_COMMAND ""
    BUILD_BYPRODUCTS ${CMAKE_BINARY_DIR}/external/src/googlebenchmark-build/src/libbenchmark.a
  )
  ExternalProject_Get_Property(googlebenchmark SOURCE_DIR BINARY_DIR)

  add_library(benchmark STATIC IMPORTED GLOBAL)
  set_target_properties(benchmark PROPERTIES
    IMPORTED_LOCATION ${BINARY_DIR}/src/libbenchmark.a
  )
  add_dependencies(benchmark googlebenchmark)

  add_inc_list("${SOURCE_DIR}/include")
  set(benchmark benchmark PARENT_SCOPE)
  set(ENV{BENCHMARK_INCLUDE_DIR} ${SOURCE_DIR}/include)

  install(DIRECTORY ${SOURCE_DIR}/include/ DESTINATION include)
  install(FILES ${BINARY_DIR}/src/libbenchmark.a DESTINATION lib)
endfunction()

if(NOT TARGET benchmark)
  build_external_benchmark()
  add_dependencies(air_depend benchmark)
  set(BM_LIBS "benchmark;pthread" CACHE STRING "Global common libs of benchmark")
endif()