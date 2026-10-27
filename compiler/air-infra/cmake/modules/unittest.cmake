#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

# Build external unittest project dependent function
function(build_external_unittest)

  # Default: upstream GitHub; internal override via .aci/
  if(NOT DEFINED UNITTEST_URL)
    set(UNITTEST_URL "https://github.com/google/googletest.git")
  endif()

  message(STATUS "Cloning External Repository    : ${UNITTEST_URL}")

  include(ExternalProject)
  ExternalProject_Add(
    unittest
    GIT_REPOSITORY ${UNITTEST_URL}
    GIT_TAG main
    PREFIX ${CMAKE_BINARY_DIR}/external
    CMAKE_ARGS  -DCMAKE_BUILD_TYPE=Release
    UPDATE_COMMAND ""
    BUILD_ALWAYS OFF
    INSTALL_COMMAND ""
    BUILD_BYPRODUCTS ${CMAKE_BINARY_DIR}/external/src/unittest-build/lib/libgtest.a
  )
  ExternalProject_Get_Property(unittest SOURCE_DIR BINARY_DIR)

  add_library(gtest IMPORTED STATIC GLOBAL)
  set_target_properties(gtest PROPERTIES
    IMPORTED_LOCATION ${BINARY_DIR}/lib/libgtest.a
  )
  add_inc_list("${SOURCE_DIR}/googletest/include")
  add_dependencies(gtest unittest)

  set(gtest gtest PARENT_SCOPE)
  set(ENV{UNITTEST_INCLUDE_DIR} ${SOURCE_DIR}/googletest/include)

  install(DIRECTORY ${SOURCE_DIR}/googletest/include/ DESTINATION include)
  install(FILES ${BINARY_DIR}/lib/libgtest.a DESTINATION lib)
endfunction()

if(NOT TARGET gtest)
	build_external_unittest()
  add_dependencies(air_depend gtest)
  set(UT_LIBS "gtest;pthread" CACHE STRING "Global common libs of unittest")
endif()