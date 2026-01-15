#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

function(seal_find_package version) 
  if(${version} STREQUAL BTS)
    set(SEALBTS_URL      "https://code.alipay.com/fhe/SEAL.git")
    set(SEALBTS_URL_SSH  "git@code.alipay.com:fhe/SEAL.git")
    if(SEALBTS_URL_SSH)
      set(REPO_SEALBTS_URL ${SEALBTS_URL_SSH})
    else()
      set(REPO_SEALBTS_URL ${SEALBTS_URL})
    endif()

    message(STATUS "Cloning External Repository   : ${REPO_SEALBTS_URL}")
  
    include(ExternalProject)
    ExternalProject_Add(
      SEAL_BTS_Project
      GIT_REPOSITORY ${REPO_SEALBTS_URL}
      GIT_TAG feat-seal-bts-v3.6.6
      PREFIX ${CMAKE_BINARY_DIR}/external
      BUILD_ALWAYS OFF
      BUILD_COMMAND         ${CMAKE_COMMAND} --build <BINARY_DIR>
      INSTALL_COMMAND       ""
      BUILD_BYPRODUCTS 
        ${CMAKE_BINARY_DIR}/external/src/SEAL_BTS_Project-build/libSEAL_BTS.a
        ${CMAKE_BINARY_DIR}/external/src/SEAL_BTS_Project-build/external/SEAL/install/lib/libseal-3.6.a
        ${CMAKE_BINARY_DIR}/external/src/SEAL_BTS_Project-build/external/NTL/src/NTL/src/ntl.a
    )
    ExternalProject_Get_Property(SEAL_BTS_Project SOURCE_DIR BINARY_DIR)
    add_library(NTL IMPORTED STATIC GLOBAL)
    set_target_properties(NTL PROPERTIES
      IMPORTED_LOCATION ${BINARY_DIR}/external/NTL/src/NTL/src/ntl.a)

    add_library(SEAL IMPORTED STATIC GLOBAL)
    set_target_properties(SEAL PROPERTIES
        IMPORTED_LOCATION ${BINARY_DIR}/external/SEAL/install/lib/libseal-3.6.a)
    
    add_library(SEAL_BTS IMPORTED STATIC GLOBAL)
    set_target_properties(SEAL_BTS PROPERTIES
      IMPORTED_LOCATION ${BINARY_DIR}/libSEAL_BTS.a)

    include_directories(${SOURCE_DIR}/ckks_bootstrapping/include)
    include_directories(${BINARY_DIR}/external/SEAL/install/include/SEAL-3.6)
    set(ENV{SEALBTS_INCLUDE_DIR} ${BINARY_DIR}/external/SEAL/install/include/SEAL-3.6)
    add_dependencies(SEAL_BTS SEAL_BTS_Project)
    
    set(ENABLE_BTS true PARENT_SCOPE)
    set(SEAL_BTS_LIBS SEAL_BTS SEAL NTL pthread gmp PARENT_SCOPE)
    add_compile_definitions(SEAL_BTS_MACRO)
  else()
    find_package(SEAL ${version} QUIET)
    if(SEAL_FOUND)
      get_target_property(SEAL_INCLUDE_DIR SEAL::seal INTERFACE_INCLUDE_DIRECTORIES)
      set(RT_SEAL_INCLUDE_DIRS "${SEAL_INCLUDE_DIR}" PARENT_SCOPE)
    else ()
      message (WARNING "SEAL library not found, no SEAL support in FHErtlib")
    endif()
  endif()
endfunction()