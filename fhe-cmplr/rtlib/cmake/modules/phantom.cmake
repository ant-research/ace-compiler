#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

# Build external Phantom project dependent function
function(build_external_phantom)
  
  set(PHANTOM_URL      "https://git:$ENV{CI_TOKEN}@code.alipay.com/zhanggongliang.zgl/phantom-fhe.git")
  set(PHANTOM_URL_SSH  "git@code.alipay.com:zhanggongliang.zgl/phantom-fhe.git")
  if(EXTERNAL_URL_SSH)
    set(REPO_PHANTOM_URL ${PHANTOM_URL_SSH})
  else()
    set(REPO_PHANTOM_URL ${PHANTOM_URL})
  endif()

  message(STATUS "Cloning External Repository   : ${REPO_PHANTOM_URL}")

  include(ExternalProject)
  ExternalProject_Add(
    phantom_external
    GIT_REPOSITORY ${REPO_PHANTOM_URL}
    GIT_TAG master
    PREFIX ${CMAKE_BINARY_DIR}/external
    UPDATE_COMMAND ""
    BUILD_ALWAYS OFF
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release
    BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR>
    INSTALL_COMMAND ""
    BUILD_BYPRODUCTS ${CMAKE_BINARY_DIR}/external/src/phantom_external-build/lib/libphantom.a
  )
  ExternalProject_Get_Property(phantom_external SOURCE_DIR BINARY_DIR)

  find_library(NTL_LIBRARY ntl)
  find_library(GMP_LIBRARY gmp)
  find_library(GMPXX_LIBRARY gmpxx)

  if(NOT NTL_LIBRARY OR NOT GMP_LIBRARY OR NOT GMPXX_LIBRARY)
    message(FATAL_ERROR "NTL or GMP libraries not found")
  endif()

  add_library(phantom IMPORTED STATIC GLOBAL)
  set_target_properties(phantom PROPERTIES
    IMPORTED_LOCATION ${BINARY_DIR}/lib/libphantom.a
  )
  include_directories(${SOURCE_DIR}/include)
  add_dependencies(phantom phantom_external)

  set(phantom phantom PARENT_SCOPE)
  set(ENV{PHANTOM_INCLUDE_DIR} ${SOURCE_DIR}/include)
  set(PHANTOM_LIBS phantom ${NTL_LIBRARY} ${GMPXX_LIBRARY} ${GMP_LIBRARY} PARENT_SCOPE)
  
  
endfunction()

