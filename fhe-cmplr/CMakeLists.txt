#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

cmake_minimum_required(VERSION 3.13)

project(fhe-cmplr C CXX)

set(FHE_CMPLR_VERSION_MAJOR 0)
set(FHE_CMPLR_VERSION_MINOR 0)
set(FHE_CMPLR_VERSION_PATCH 1)
set(FHE_CMPLR_VERSION ${FHE_CMPLR_VERSION_MAJOR}.${FHE_CMPLR_VERSION_MINOR}.${FHE_CMPLR_VERSION_PATCH})

set(CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)

include(cmake/function_option.cmake)
include(cmake/function_library.cmake)
# include(cmake/function_extralib.cmake)
include(cmake/function_rtlibrary.cmake)

# Add path for custom CMake modules.
list(APPEND CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

#--------------------------------------------------------------------
# Build options
#--------------------------------------------------------------------
option(BUILD_STATIC        "Control whether to build the static libraries"     ON)
option(BUILD_SHARED        "Control whether to build the shared libraries"    OFF)
option(BUILD_UNITTEST      "Control whether to build the unittest"             ON)
option(BUILD_BENCH         "Control whether to build the benmark"              ON)
option(BUILD_DOC           "Control whether to build API documentation"       OFF)
option(FHE_CODE_CHECK      "Control whether to check coding style"             ON)
option(FHE_BUILD_TEST      "Control whether to build the tests"                ON)
option(FHE_BUILD_EXAMPLE   "Control whether to build the examples"             ON)
option(FHE_INSTALL_APP     "Control whether to install the APPs"               ON)
option(FHE_ENABLE_SEAL     "Control whether to build with SEAL libraries"      ON)
option(FHE_ENABLE_SEAL_BTS "Control whether to build with SEAL_BTS libraries" OFF)
option(FHE_ENABLE_OPENFHE  "Control whether to build with OPENFHE libraries"   ON)
option(FHE_ENABLE_CUDA     "Control whether to build with CUDA libraries"     OFF)
option(BUILD_WITH_OPENMP   "Control whether to build with OpenMP libraries"   OFF)
option(EXTERNAL_URL_SSH    "Control choose external URL for repository"       OFF)

set(FHE_WITH_SRC "" CACHE STRING "The default compilation depends")

if(NOT DEFINED FHE_SEED_MODE)
  set(FHE_SEED_MODE "prng" CACHE STRINGS "Default seed mode")
endif()

if(NOT DEFINED FHE_SUPPORT_HPU)
  set(FHE_SUPPORT_HPU OFF CACHE BOOL "Default not supported HPU features")
endif()

# Set target fhe_depend for pre-build
add_custom_target(fhe_depend)

# Check option : -DFHE_WITH_SRC
if(NOT "${FHE_WITH_SRC}" STREQUAL "")
  build_fhe_with_src("${FHE_WITH_SRC}")
  read_package_config("$ENV{PACKAGE_BASE_DIR}")
  add_dependencies(fhe_depend ${NN_LIBS})
else()
  # find fhe-cmplr depend libraries
  find_package(ant-ace REQUIRED)
  find_nn_lib()
  set_package_env(${PACKAGE_BASE_DIR} ${PACKAGE_CONF_DIR} ${PACKAGE_DEVTOOLS_DIR})
endif()

# Check Package information
check_package_info("${PACKAGE_NAME}")

# Check commit : BUILD_GIT_COMMIT
string(TIMESTAMP BUILD_TIMESTAMP "%Y-%m-%d %H:%M:%S")
check_code_revision("${PROJECT_NAME}" BUILD_GIT_COMMIT)

# Check option : -DCMAKE_BUILD_TYPE
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Set the default type of build" FORCE)
else()
  check_build_type(${CMAKE_BUILD_TYPE})
endif()

### Print options
message(STATUS "PROJECT_SOURCE_DIR            : ${PROJECT_SOURCE_DIR}")
message(STATUS "CMAKE_BUILD_TYPE              : ${CMAKE_BUILD_TYPE}")
message(STATUS "BUILD_STATIC                  : ${BUILD_STATIC}")
message(STATUS "BUILD_SHARED                  : ${BUILD_SHARED}")
message(STATUS "BUILD_UNITTEST                : ${BUILD_UNITTEST}")
message(STATUS "BUILD_BENCH                   : ${BUILD_BENCH}")
message(STATUS "BUILD_DOC                     : ${BUILD_DOC}")
message(STATUS "FHE_CODE_CHECK                : ${FHE_CODE_CHECK}")
message(STATUS "FHE_WITH_SRC                  : ${FHE_WITH_SRC}")
message(STATUS "FHE_BUILD_TEST                : ${FHE_BUILD_TEST}")
message(STATUS "FHE_BUILD_EXAMPLE             : ${FHE_BUILD_EXAMPLE}")
message(STATUS "FHE_ENABLE_SEAL               : ${FHE_ENABLE_SEAL}")
message(STATUS "FHE_ENABLE_SEAL_BTS           : ${FHE_ENABLE_SEAL_BTS}")
message(STATUS "FHE_ENABLE_OPENFHE            : ${FHE_ENABLE_OPENFHE}")
message(STATUS "FHE_ENABLE_PHANTOM            : ${FHE_ENABLE_PHANTOM}")
message(STATUS "FHE_INSTALL_APP               : ${FHE_INSTALL_APP}")
message(STATUS "BUILD_WITH_OPENMP             : ${BUILD_WITH_OPENMP}")
message(STATUS "BUILD_TIMESTAMP               : ${BUILD_TIMESTAMP}")
message(STATUS "BUILD_GIT_COMMIT              : ${BUILD_GIT_COMMIT}")

#--------------------------------------------------------------------
# Logic: Configure
#--------------------------------------------------------------------
# for check commit message
install_hooks($ENV{PACKAGE_DEVTOOLS_DIR}/hooks)

# for check coding style
if(FHE_CODE_CHECK)
  # codes from other parties are excluded from coding style checks
  set(CODEING_STYLE_EXCLUDES "third-party")
  # exclude unittest source from coding style check if not build
  if(NOT BUILD_UNITTEST)
    set(CODEING_STYLE_EXCLUDES ${CODEING_STYLE_EXCLUDES} "unittest")
  endif()
  # exclude benchmark source from coding style check if not build
  if(NOT BUILD_BENCH)
    set(CODEING_STYLE_EXCLUDES ${CODEING_STYLE_EXCLUDES} "benchmark")
  endif()
  # exclude test source from coding style check if not build
  if(NOT FHE_BUILD_TEST)
    set(CODEING_STYLE_EXCLUDES ${CODEING_STYLE_EXCLUDES} "test")
  endif()

  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
  check_coding_style("$ENV{PACKAGE_DEVTOOLS_DIR}" "${CODEING_STYLE_EXCLUDES}")
endif()

#--------------------------------------------------------------------
# Logic: Target
#--------------------------------------------------------------------
# add glob include list
add_inc_list("${PROJECT_SOURCE_DIR}/include;${CMAKE_BINARY_DIR}/include;${PROJECT_SOURCE_DIR}/rtlib/include")
include_directories("${PACKAGE_INC_DIR}")

# for target libraries
set_fhe_lib()

# add glob depends for allfhe
add_custom_target(allfhe DEPENDS fhe_depend)

if(FHE_BUILD_TEST)
  enable_testing()
endif()

# for build fhe runtime library support linux
include(fhe_rtlib)

# build component
add_subdirectory(util)
add_subdirectory(core)
add_subdirectory(opt)
add_subdirectory(cg)
add_subdirectory(sihe)
add_subdirectory(ckks)
add_subdirectory(poly)

# build executable
add_subdirectory(driver)

# build benchmark
if(BUILD_BENCH)
  add_subdirectory(benchmark)
endif()

# build e2e test
if(FHE_BUILD_TEST)
  add_subdirectory(test)
endif()

# build doc
if(BUILD_DOC)
  build_app_doc(FHE_INSTALL_APP ${PROJECT_NAME})
endif()

#--------------------------------------------------------------------
# Logic: Package
#--------------------------------------------------------------------
# install header file
install(DIRECTORY include/ DESTINATION include)

# ${PROJECT_SOURCE_DIR}/cmake/function_extralib.cmake

install(FILES
    ${PROJECT_SOURCE_DIR}/cmake/function_library.cmake
    ${PROJECT_SOURCE_DIR}/cmake/function_rtlibrary.cmake
    DESTINATION share/${PACKAGE_NAME}/cmake/${PROJECT_NAME}
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_Version.cmake"
    VERSION ${FHE_CMPLR_VERSION}
    COMPATIBILITY AnyNewerVersion
)

set(COMPONENT_CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}Config.cmake")
set(PACKAGE_CONFIG_FILE "$ENV{PACKAGE_CONF_DIR}/${PACKAGE_NAME}Config.cmake")

configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/package.cmake.in"
    "${COMPONENT_CONFIG_FILE}"
    @ONLY
)

install(CODE "
  file(READ ${COMPONENT_CONFIG_FILE} CMAKE_CONTENTS)
  file(APPEND ${PACKAGE_CONFIG_FILE} \"\${CMAKE_CONTENTS}\")
")

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_Version.cmake"
    DESTINATION share/${PACKAGE_NAME}
)
