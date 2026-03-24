#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

cmake_minimum_required(VERSION 3.13)

include_guard(GLOBAL)

project(air-infra C CXX)

set(AIR_VERSION_MAJOR 0)
set(AIR_VERSION_MINOR 0)
set(AIR_VERSION_PATCH 1)
set(AIR_VERSION ${AIR_VERSION_MAJOR}.${AIR_VERSION_MINOR}.${AIR_VERSION_PATCH})

set(CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)

# Enable creating a dependency graph
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# Generate graphviz files
# set_property(GLOBAL PROPERTY GLOBAL_DEPENDS_DEBUG_MODE 1)

# Policy CMP0148 is not set: The FindPythonInterp and FindPythonLibs modules are removed.
# But the pybind11 package is still useful, suppressing the warning for now.
if(POLICY CMP0148)
  cmake_policy(SET CMP0148 OLD)
endif()

include(cmake/function_common.cmake)
include(cmake/function_library.cmake)
include(cmake/function_extralib.cmake)
include(cmake/function_app.cmake)

# Add path for custom CMake modules.
list(APPEND CMAKE_MODULE_PATH
	"${PROJECT_SOURCE_DIR}/cmake/modules")

# Read package configure information
read_package_config(${PROJECT_SOURCE_DIR})
set(PACKAGE_VERSION
  ${PACKAGE_VERSION_MAJOR}.${PACKAGE_VERSION_MINOR}.${PACKAGE_VERSION_PATCH})
message(STATUS "PACKAGE_NAME                   : ${PACKAGE_NAME}")
message(STATUS "PACKAGE_VERION                 : ${PACKAGE_VERSION}")

set_package_env(${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/devtools)

#--------------------------------------------------------------------
# Build options
#--------------------------------------------------------------------
# Set project location
if(NOT PACKAGE_BASE_DIR)
  set(PACKAGE_BASE_DIR ${PROJECT_SOURCE_DIR})
endif()

option(BUILD_STATIC        "Control whether to build the static libraries"  ON)
option(BUILD_SHARED        "Control whether to build the shared libraries" OFF)
option(BUILD_UNITTEST      "Control whether to build the unittest"          ON)
option(BUILD_BENCH         "Control whether to build the benmark"           ON)
option(BUILD_DOC           "Control whether to build API documentation"    OFF)
option(AIR_CODE_CHECK      "Control whether to check coding style"          ON)
option(AIR_BUILD_TEST      "Control whether to build the tests"             ON)
option(AIR_BUILD_EXAMPLE   "Control whether to build the examples"          ON)
option(AIR_INSTALL_APP     "Control whether to install the APPs"            ON)
option(BUILD_TCM           "Control whether to analysis memory"            OFF)
option(EXTERNAL_URL_SSH    "Control choose external URL for repository"    OFF)

# Check commit : BUILD_GIT_COMMIT
string(TIMESTAMP BUILD_TIMESTAMP "%Y-%m-%d %H:%M:%S")
check_code_revision("${PROJECT_NAME}" BUILD_GIT_COMMIT)

# Check option : -DCMAKE_BUILD_TYPE
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
        "Set the default type of build" FORCE)
else()
  check_build_type(${CMAKE_BUILD_TYPE})
endif()

### Print options
message(STATUS "PROJECT_SOURCE_DIR             : ${PROJECT_SOURCE_DIR}")
message(STATUS "CMAKE_BUILD_TYPE               : ${CMAKE_BUILD_TYPE}")
message(STATUS "BUILD_STATIC                   : ${BUILD_STATIC}")
message(STATUS "BUILD_SHARED                   : ${BUILD_SHARED}")
message(STATUS "BUILD_UNITTEST                 : ${BUILD_UNITTEST}")
message(STATUS "BUILD_BENCH                    : ${BUILD_BENCH}")
message(STATUS "BUILD_DOC                      : ${BUILD_DOC}")
message(STATUS "AIR_CODE_CHECK                 : ${AIR_CODE_CHECK}")
message(STATUS "AIR_BUILD_TEST                 : ${AIR_BUILD_TEST}")
message(STATUS "AIR_BUILD_EXAMPLE              : ${AIR_BUILD_EXAMPLE}")
message(STATUS "AIR_INSTALL_APP                : ${AIR_INSTALL_APP}")
message(STATUS "BUILD_TCM                      : ${BUILD_TCM}")
message(STATUS "BUILD_TIMESTAMP                : ${BUILD_TIMESTAMP}")
message(STATUS "BUILD_GIT_COMMIT               : ${BUILD_GIT_COMMIT}")

#--------------------------------------------------------------------
# Logic: Configure
#--------------------------------------------------------------------
# for check commit message
install_hooks($ENV{PACKAGE_DEVTOOLS_DIR}/hooks)

add_custom_target(air_depend)

# build external dependent libraries
if(BUILD_UNITTEST)
  include(unittest)
endif()
if(BUILD_BENCH)
  include(benchmark)
endif()
if(BUILD_TCM)
  include(gperftools)
endif()
include(CheckSymbolExists)

# for check coding style
if(AIR_CODE_CHECK)
  # codes from other parties are excluded from coding style checks
  set(CODEING_STYLE_EXCLUDES " ")
  # exclude unittest source from coding style check if not build
  if(NOT BUILD_UNITTEST)
    set(CODEING_STYLE_EXCLUDES ${CODEING_STYLE_EXCLUDES} "unittest")
  endif()
  # exclude benchmark source from coding style check if not build
  if(NOT BUILD_BENCH)
    set(CODEING_STYLE_EXCLUDES ${CODEING_STYLE_EXCLUDES} "benchmark")
  endif()
  # exclude test source from coding style check if not build
  if(NOT AIR_BUILD_TEST)
    set(CODEING_STYLE_EXCLUDES ${CODEING_STYLE_EXCLUDES} "test")
  endif()

  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES
      ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
  check_coding_style("$ENV{PACKAGE_DEVTOOLS_DIR}" "${CODEING_STYLE_EXCLUDES}")
endif()

# for generate configure
configure_file(
  "${PROJECT_SOURCE_DIR}/config.in"
  "${CMAKE_BINARY_DIR}/include/config.h"
)

# for generate user message
gen_errmsg(${PYTHON_EXECUTABLE})

#--------------------------------------------------------------------
# Logic: Target
#--------------------------------------------------------------------
# for glob include list
set(PACKAGE_INC_DIR "${PROJECT_SOURCE_DIR}/include" CACHE STRING "List of glob include")
add_inc_list("${PROJECT_SOURCE_DIR}/include;${CMAKE_BINARY_DIR}/include")
include_directories("${PACKAGE_INC_DIR}")

# set global libraries
set_air_lib()

# add glob depends for allair
add_custom_target(allair DEPENDS air_depend)

# enable ctest
if(AIR_BUILD_TEST OR AIR_BUILD_EXAMPLE OR BUILD_UNITTEST OR BUILD_BENCH)
  enable_testing()
endif()

# build component
add_subdirectory(util)
add_subdirectory(base)
add_subdirectory(core)
add_subdirectory(driver)
add_subdirectory(opt)
add_subdirectory(cg)
add_subdirectory(irgen)
add_subdirectory(plugin)

# build benchmark
if(BUILD_BENCH)
  add_subdirectory(benchmark)
endif()

# build e2e test
if(AIR_BUILD_TEST)
  add_subdirectory(test)
endif()

# build doc
if(BUILD_DOC)
  build_app_doc(AIR_INSTALL_APP "${PROJECT_NAME}")
endif()

#--------------------------------------------------------------------
# Logic: Package
#--------------------------------------------------------------------
# install header file
install(DIRECTORY include/ DESTINATION include)

install(FILES
    ${PROJECT_SOURCE_DIR}/cmake/function_common.cmake
    ${PROJECT_SOURCE_DIR}/cmake/function_app.cmake
    ${PROJECT_SOURCE_DIR}/cmake/function_library.cmake
    ${PROJECT_SOURCE_DIR}/cmake/function_extralib.cmake
    DESTINATION share/${PACKAGE_NAME}/cmake/${PROJECT_NAME}
)

install(DIRECTORY
    ${PROJECT_SOURCE_DIR}/devtools/
    DESTINATION share/${PACKAGE_NAME}/devtools
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_Version.cmake"
    VERSION ${AIR_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/package.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}Config.cmake"
    @ONLY
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_Version.cmake"
    DESTINATION share/${PACKAGE_NAME}
)

