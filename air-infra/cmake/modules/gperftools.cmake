#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

# Build external gperftools project dependent function
function(build_external_gperf)

  set(GPERF_URL      "https://code.alipay.com/opencc/gperftools.git")
  set(GPERF_URL_SSH  "git@code.alipay.com:opencc/gperftools.git")
  if(EXTERNAL_URL_SSH)
    set(REPO_GPERF_URL ${GPERF_URL_SSH})
  else()
    set(REPO_GPERF_URL ${GPERF_URL})
  endif()

  message(STATUS "Cloning External Repository    : ${REPO_GPERF_URL}")

  include(ExternalProject)
  ExternalProject_Add(
    gperftools
    GIT_REPOSITORY ${REPO_GPERF_URL}
    GIT_TAG master
    PREFIX ${CMAKE_BINARY_DIR}/external
    CMAKE_ARGS  -DCMAKE_BUILD_TYPE=Release 
                -DBUILD_TESTING=OFF
                -DGPERFTOOLS_BUILD_STATIC=ON
    UPDATE_COMMAND ""
    BUILD_ALWAYS OFF
    INSTALL_COMMAND ""
    BUILD_BYPRODUCTS ${CMAKE_BINARY_DIR}/external/src/gperftools-build/libtcmalloc.a
  )
  ExternalProject_Get_Property(gperftools SOURCE_DIR BINARY_DIR)

  add_library(tcmalloc STATIC IMPORTED)
  set_target_properties(tcmalloc PROPERTIES
    IMPORTED_LOCATION ${BINARY_DIR}/libtcmalloc.a
  )
  include_directories(${SOURCE_DIR}/include)
  add_dependencies(tcmalloc gperftools)

  set(tcmalloc tcmalloc PARENT_SCOPE)

  install(
    FILES ${SOURCE_DIR}/src/pprof
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
    DESTINATION bin)
  install(FILES ${BINARY_DIR}/libtcmalloc.a DESTINATION lib)
endfunction()

if(NOT TARGET tcmalloc)
	build_external_gperf()
  add_dependencies(air_depend tcmalloc)
  set(PERF_LIBS "benchmark;pthread" CACHE STRING "Global common libs of perftools")
endif()