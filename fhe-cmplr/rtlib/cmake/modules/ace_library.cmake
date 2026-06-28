#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

# Build external AceLibrary project dependent function
function(build_external_ace_library)
  # Default: internal repository; override ACE_LIBRARY_URL via .aci/ or cmake -D.
  if(NOT DEFINED ACE_LIBRARY_URL)
    set(ACE_LIBRARY_URL "https://git:$ENV{CI_TOKEN}@code.alipay.com/ace-fhe/ace-library.git")
  endif()
  set(ACE_LIBRARY_GIT_TAG  "master" CACHE STRING "ckks-framework git tag/branch")
  set(ACE_LIBRARY_DATA_TYPE "64" CACHE STRING "ACE-Library word size")
  set(ACE_LIBRARY_CUDA_ARCHITECTURES "80;89" CACHE STRING "ACE-Library CUDA architectures")
  set(ACE_LIBRARY_NTT_COMPILE_STRATEGY "MINIMAL" CACHE STRING "ACE-Library NTT compile strategy")
  set_property(CACHE ACE_LIBRARY_NTT_COMPILE_STRATEGY PROPERTY STRINGS "MINIMAL" "CUDA_ALL" "ALL")
  set(ACE_LIBRARY_NTT_ENABLE_SHUFFLE "AUTO" CACHE STRING "Build Shuffle NTT backend")
  set(ACE_LIBRARY_NTT_ENABLE_TENSOR "AUTO" CACHE STRING "Build Tensor NTT backend")
  set(ACE_LIBRARY_NTT_ENABLE_FP64_TENSOR "AUTO" CACHE STRING "Build FP64 Tensor NTT backend")
  set_property(CACHE ACE_LIBRARY_NTT_ENABLE_SHUFFLE PROPERTY STRINGS "AUTO" "ON" "OFF")
  set_property(CACHE ACE_LIBRARY_NTT_ENABLE_TENSOR PROPERTY STRINGS "AUTO" "ON" "OFF")
  set_property(CACHE ACE_LIBRARY_NTT_ENABLE_FP64_TENSOR PROPERTY STRINGS "AUTO" "ON" "OFF")

  set(BASE_PREFIX "${CMAKE_BINARY_DIR}/external/src")
  set(STAGING_DIR "${BASE_PREFIX}/staging") # install dir

  # --------------------------------------------------------
  # Step 1: Prepare source code (ace_library_repo / ckks-framework)
  # --------------------------------------------------------
  include(ExternalProject)
  set(SHARED_SOURCE_DIR "${BASE_PREFIX}/ace_library") # repo dir
  message(STATUS "Cloning External Repository   : ${ACE_LIBRARY_URL}")
  ExternalProject_Add(
    ace_library_repo
    GIT_REPOSITORY ${ACE_LIBRARY_URL}
    GIT_TAG ${ACE_LIBRARY_GIT_TAG}
    PREFIX ${BASE_PREFIX}
    SOURCE_DIR ${SHARED_SOURCE_DIR}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    UPDATE_COMMAND ""
  )

  # --------------------------------------------------------
  # Step2: Build ckks-framework through the unified root entrypoint
  # --------------------------------------------------------
  ExternalProject_Add(
    ace_library_build
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${SHARED_SOURCE_DIR}
    PREFIX ${CMAKE_BINARY_DIR}/external/ace_library_build
    UPDATE_COMMAND ""
    BUILD_ALWAYS OFF
    INSTALL_DIR ${STAGING_DIR}
    DEPENDS ace_library_repo
    CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=Release
        -DACE_LIBRARY_DATA_TYPE=${ACE_LIBRARY_DATA_TYPE}
        -DACE_LIBRARY_BUILD_APP=ON
        -DACE_LIBRARY_BUILD_EXAMPLES=OFF
        -DACE_LIBRARY_BUILD_BENCH=OFF
        -DACE_LIBRARY_BUILD_TESTING=OFF
        -DACE_LIBRARY_ENABLE_TOML_CONFIG=OFF
        -DACE_LIBRARY_USE_NVTX=ON
        -DACE_LIBRARY_NTT_COMPILE_STRATEGY=${ACE_LIBRARY_NTT_COMPILE_STRATEGY}
        -DACE_LIBRARY_NTT_ENABLE_SHUFFLE=${ACE_LIBRARY_NTT_ENABLE_SHUFFLE}
        -DACE_LIBRARY_NTT_ENABLE_TENSOR=${ACE_LIBRARY_NTT_ENABLE_TENSOR}
        -DACE_LIBRARY_NTT_ENABLE_FP64_TENSOR=${ACE_LIBRARY_NTT_ENABLE_FP64_TENSOR}
        "-DACE_LIBRARY_CUDA_ARCHITECTURES=${ACE_LIBRARY_CUDA_ARCHITECTURES}"
        "-DCMAKE_CUDA_ARCHITECTURES=${ACE_LIBRARY_CUDA_ARCHITECTURES}"
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        "-DCMAKE_CUDA_FLAGS=-DLIBCUDACXX_ENABLE_EXPERIMENTAL_MEMORY_RESOURCE"
        "-DCMAKE_CXX_FLAGS=-DLIBCUDACXX_ENABLE_EXPERIMENTAL_MEMORY_RESOURCE"
    BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR>
    INSTALL_COMMAND ${CMAKE_COMMAND} --install <BINARY_DIR>
    BUILD_BYPRODUCTS
        ${STAGING_DIR}/lib/libace_library_engine.a
        ${STAGING_DIR}/lib/libace_library_modules.a
  )

  ExternalProject_Get_Property(ace_library_build BINARY_DIR)
  set(APP_BINARY_DIR ${BINARY_DIR})
  set(ACE_LIBRARY_BUILD_INCLUDE_DIRS
      "${APP_BINARY_DIR}/he-engine/include"
      "${APP_BINARY_DIR}/he-engine/include/ace/he_engine"
      "${APP_BINARY_DIR}/_deps/random123-src/include"
      "${APP_BINARY_DIR}/_deps/rmm-src/cpp/include"
      "${APP_BINARY_DIR}/_deps/rmm-build/include"
      "${APP_BINARY_DIR}/_deps/cccl-src/thrust"
      "${APP_BINARY_DIR}/_deps/cccl-src/libcudacxx/include"
      "${APP_BINARY_DIR}/_deps/cccl-src/cub"
      "${APP_BINARY_DIR}/_deps/nvtx3-src/c/include"
      "${APP_BINARY_DIR}/_deps/rapids_logger-src/include"
      "${APP_BINARY_DIR}/_deps/fmt-src/include"
      "${APP_BINARY_DIR}/_deps/spdlog-src/include")

  # --------------------------------------------------------
  # Step 4: Locate system dependencies
  # --------------------------------------------------------
  find_library(NTL_LIBRARY ntl)
  find_library(GMP_LIBRARY gmp)
  find_library(GMPXX_LIBRARY gmpxx)

  if(NOT NTL_LIBRARY OR NOT GMP_LIBRARY OR NOT GMPXX_LIBRARY)
    message(FATAL_ERROR "NTL or GMP libraries not found")
  endif()

  if(NOT EXISTS "${STAGING_DIR}/include")
    file(MAKE_DIRECTORY "${STAGING_DIR}/include")
  endif()

  if(NOT EXISTS "${STAGING_DIR}/include/ace/he_engine")
    file(MAKE_DIRECTORY "${STAGING_DIR}/include/ace/he_engine")
  endif()

  if(NOT EXISTS "${STAGING_DIR}/include/GPULIB")
    file(MAKE_DIRECTORY "${STAGING_DIR}/include/GPULIB")
  endif()

  if(NOT EXISTS "${STAGING_DIR}/ace/include/lib_ace/public")
    file(MAKE_DIRECTORY "${STAGING_DIR}/ace/include/lib_ace/public")
  endif()

  if(NOT EXISTS "${STAGING_DIR}/ace/include/lib_ace/cccl")
    file(MAKE_DIRECTORY "${STAGING_DIR}/ace/include/lib_ace/cccl")
  endif()

  if(NOT EXISTS "${STAGING_DIR}/lib")
    file(MAKE_DIRECTORY "${STAGING_DIR}/lib")
  endif()

  foreach(include_dir ${ACE_LIBRARY_BUILD_INCLUDE_DIRS})
    if(NOT EXISTS "${include_dir}")
      file(MAKE_DIRECTORY "${include_dir}")
    endif()
  endforeach()

  # --------------------------------------------------------
  # Step 5: Define IMPORTED targets
  # --------------------------------------------------------

  if(NOT EXISTS "${STAGING_DIR}/include/ace_library_modules")
    file(MAKE_DIRECTORY "${STAGING_DIR}/include/ace_library_modules")
  endif()

  if(NOT EXISTS "${STAGING_DIR}/include/ace_library_modules/common")
    file(MAKE_DIRECTORY "${STAGING_DIR}/include/ace_library_modules/common")
  endif()

  # import engine
  add_library(ace_library_gpu IMPORTED STATIC GLOBAL)
  set_target_properties(ace_library_gpu PROPERTIES
    IMPORTED_LOCATION ${STAGING_DIR}/lib/libace_library_engine.a
    INTERFACE_INCLUDE_DIRECTORIES "${STAGING_DIR}/include;${STAGING_DIR}/include/ace/he_engine;${STAGING_DIR}/include/GPULIB;${STAGING_DIR}/ace/include/lib_ace/public;${STAGING_DIR}/ace/include/lib_ace/cccl;${ACE_LIBRARY_BUILD_INCLUDE_DIRS}"
  )
  add_dependencies(ace_library_gpu ace_library_build)

  # import app
  add_library(ace_library_app IMPORTED STATIC GLOBAL)
  set_target_properties(ace_library_app PROPERTIES
    IMPORTED_LOCATION ${STAGING_DIR}/lib/libace_library_modules.a
  )

  # import rmm
  add_library(rmm IMPORTED STATIC GLOBAL)
  set_target_properties(rmm PROPERTIES
    IMPORTED_LOCATION ${STAGING_DIR}/lib/librmm.a
  )

  # Declare dependency: App requires gpu when linking
  set_target_properties(ace_library_app PROPERTIES
    INTERFACE_LINK_LIBRARIES ace_library_gpu
  )
  add_dependencies(ace_library_app ace_library_build)

  # --------------------------------------------------------
  # Step 6: Export variables to parent scope
  # --------------------------------------------------------
  set(ACE_LIBRARY_INCLUDE_DIR_VALUE
      "${STAGING_DIR}/include;${STAGING_DIR}/include/ace/he_engine;${STAGING_DIR}/include/ace_library_modules;${STAGING_DIR}/include/ace_library_modules/common;${STAGING_DIR}/include/GPULIB;${STAGING_DIR}/include/rapids;${STAGING_DIR}/ace/include/lib_ace/public;${STAGING_DIR}/ace/include/lib_ace/cccl;${ACE_LIBRARY_BUILD_INCLUDE_DIRS}")
  string(REPLACE ";" ":" ACE_LIBRARY_INCLUDE_ENV_VALUE "${ACE_LIBRARY_INCLUDE_DIR_VALUE}")
  set(ace_library ace_library PARENT_SCOPE)
  set(ACE_LIBRARY_INCLUDE_DIR "${ACE_LIBRARY_INCLUDE_DIR_VALUE}" PARENT_SCOPE)
  set(ENV{ACE_LIBRARY_INCLUDE_DIR} "${ACE_LIBRARY_INCLUDE_ENV_VALUE}")
  include_directories(${STAGING_DIR}/include)
  include_directories(${STAGING_DIR}/include/ace/he_engine)
  include_directories(${STAGING_DIR}/include/ace_library_modules)
  include_directories(${STAGING_DIR}/include/ace_library_modules/common)
  include_directories(${STAGING_DIR}/include/GPULIB)
  include_directories(${STAGING_DIR}/include/rapids)
  include_directories(${STAGING_DIR}/ace/include/lib_ace/public)
  include_directories(${STAGING_DIR}/ace/include/lib_ace/cccl)
  include_directories(${ACE_LIBRARY_BUILD_INCLUDE_DIRS})

  set(ACE_LIBRARY_LIBS ace_library_app ace_library_gpu rmm ${NTL_LIBRARY} ${GMPXX_LIBRARY} ${GMP_LIBRARY} PARENT_SCOPE)

  add_custom_target(ace_library_external)
  add_dependencies(ace_library_external ace_library_build)

endfunction()
