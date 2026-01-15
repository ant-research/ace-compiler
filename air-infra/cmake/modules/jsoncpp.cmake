#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

# Build external jsoncpp project dependent function
function(build_external_jsoncpp)

  set(REPO_JSONCPP_URL      "${PROJECT_SOURCE_DIR}/cmake/modules/jsoncpp")
  message(STATUS "Cloning External Repository   : ${REPO_JSONCPP_URL}")

  include(FetchContent)
  FetchContent_Declare(
    jsoncpp
    SOURCE_DIR ${REPO_JSONCPP_URL}
    SOURCE_SUBDIR cmake
    CMAKE_ARGS
      -DCMAKE_BUILD_TYPE=Release
      -DBUILD_SHARED_LIBS=OFF
      -DJSONCPP_WITH_TESTS=OFF
      -DJSONCPP_WITH_POST_BUILD_UNITTEST=OFF
      -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/jsoncpp-install
  )

  FetchContent_MakeAvailable(jsoncpp)

  add_library(jsoncpp_objects OBJECT
    ${jsoncpp_SOURCE_DIR}/src/lib_json/json_reader.cpp
    ${jsoncpp_SOURCE_DIR}/src/lib_json/json_value.cpp
    ${jsoncpp_SOURCE_DIR}/src/lib_json/json_writer.cpp
  )

  include_directories(${jsoncpp_SOURCE_DIR}/include)
  
  set(JSONCPP_INCLUDE_DIRS ${jsoncpp_SOURCE_DIR}/include PARENT_SCOPE)
endfunction()

if(NOT TARGET jsoncpp)
  build_external_jsoncpp()
endif()
