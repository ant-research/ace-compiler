#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

# Build external jsoncpp project dependent function
function(build_external_jsoncpp)

  set(JSONCPP_URL      "https://git:$ENV{CI_TOKEN}@code.alipay.com/air-infra/jsoncpp.git")
  set(JSONCPP_URL_SSH  "git@code.alipay.com:air-infra/jsoncpp.git")
  if(EXTERNAL_URL_SSH)
    set(REPO_JSONCPP_URL ${JSONCPP_URL_SSH})
  else()
    set(REPO_JSONCPP_URL ${JSONCPP_URL})
  endif()

  message(STATUS "Cloning External Repository    : ${REPO_JSONCPP_URL}")

  include(FetchContent)
  FetchContent_Declare(
    jsoncpp
    GIT_REPOSITORY ${REPO_JSONCPP_URL}
    GIT_TAG master
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
  set_property(TARGET jsoncpp_objects PROPERTY POSITION_INDEPENDENT_CODE 1)

  include_directories(${jsoncpp_SOURCE_DIR}/include)
  
  set(JSONCPP_INCLUDE_DIRS ${jsoncpp_SOURCE_DIR}/include PARENT_SCOPE)
endfunction()

if(NOT TARGET jsoncpp)
  build_external_jsoncpp()
endif()
