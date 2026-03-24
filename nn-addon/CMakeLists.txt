#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

cmake_minimum_required(VERSION 3.13)

project(nn-addon C CXX)

set(NN_ADDON_VERSION_MAJOR 0)
set(NN_ADDON_VERSION_MINOR 0)
set(NN_ADDON_VERSION_PATCH 1)
set(NN_ADDON_VERSION ${NN_ADDON_VERSION_MAJOR}.${NN_ADDON_VERSION_MINOR}.${NN_ADDON_VERSION_PATCH})

set(CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)

include(cmake/function_option.cmake)
include(cmake/function_library.cmake)
include(cmake/function_extralib.cmake)

# Add path for custom CMake modules.
list(APPEND CMAKE_MODULE_PATH
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

#--------------------------------------------------------------------
# Build options
#--------------------------------------------------------------------
option(BUILD_STATIC        "Control whether to build the static libraries"  ON)
option(BUILD_SHARED        "Control whether to build the shared libraries" OFF)
option(BUILD_UNITTEST      "Control whether to build the unittest"          ON)
option(BUILD_BENCH         "Control whether to build the benmark"           ON)
option(BUILD_DOC           "Control whether to build API documentation"    OFF)
option(NN_CODE_CHECK       "Control whether to check coding style"          ON)
option(NN_BUILD_TEST       "Control whether to build the tests"             ON)
option(NN_BUILD_EXAMPLE    "Control whether to build the examples"          ON)
option(NN_INSTALL_APP      "Control whether to install the APPs"            ON)
option(EXTERNAL_URL_SSH    "Control choose external URL for repository"    OFF)

set(NN_WITH_SRC "" CACHE STRING "The default compilation depends")

# Add target nn_depend
add_custom_target(nn_depend)

# Check option : -DNN_WITH_SRC
if(NOT "${NN_WITH_SRC}" STREQUAL "")
  build_nn_with_src("${NN_WITH_SRC}")
  read_package_config("$ENV{PACKAGE_BASE_DIR}")
  add_dependencies (nn_depend ${AIR_LIBS})
else()
  # find depend libraries
  find_package(ant-ace REQUIRED)
  find_air_lib()
  set_package_env(${PACKAGE_BASE_DIR} ${PACKAGE_CONF_DIR} ${PACKAGE_DEVTOOLS_DIR})
endif()

# Check Package information
check_package_info(${PACKAGE_NAME})

# Check common program & libraries
check_program("python3" PYTHON_EXECUTABLE)

# Check commit : BUILD_GIT_COMMIT
string(TIMESTAMP BUILD_TIMESTAMP "%Y-%m-%d %H:%M:%S")
check_code_revision(${PROJECT_NAME} BUILD_GIT_COMMIT)

# Check option : -DCMAKE_BUILD_TYPE
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Set the default type of build" FORCE)
else()
  check_build_type(${CMAKE_BUILD_TYPE})
endif()

### Print options
message(STATUS "CMAKE_CURRENT_SOURCE_DIR      : ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "CMAKE_BUILD_TYPE              : ${CMAKE_BUILD_TYPE}")
message(STATUS "BUILD_STATIC                  : ${BUILD_STATIC}")
message(STATUS "BUILD_SHARED                  : ${BUILD_SHARED}")
message(STATUS "BUILD_UNITTEST                : ${BUILD_UNITTEST}")
message(STATUS "BUILD_BENCH                   : ${BUILD_BENCH}")
message(STATUS "BUILD_DOC                     : ${BUILD_DOC}")
message(STATUS "NN_CODE_CHECK                 : ${NN_CODE_CHECK}")
message(STATUS "NN_WITH_SRC                   : ${NN_WITH_SRC}")
message(STATUS "NN_BUILD_TEST                 : ${NN_BUILD_TEST}")
message(STATUS "NN_BUILD_EXAMPLE              : ${NN_BUILD_EXAMPLE}")
message(STATUS "NN_INSTALL_APP                : ${NN_INSTALL_APP}")
message(STATUS "BUILD_TIMESTAMP               : ${BUILD_TIMESTAMP}")
message(STATUS "BUILD_GIT_COMMIT              : ${BUILD_GIT_COMMIT}")

#--------------------------------------------------------------------
# Logic: Configure
#--------------------------------------------------------------------
# for check commit message
install_hooks($ENV{PACKAGE_DEVTOOLS_DIR}/hooks)

# check coding style
if(NN_CODE_CHECK)
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
  # exclude tests source from coding style check if not build
  if(NOT NN_BUILD_TEST)
    set(CODEING_STYLE_EXCLUDES ${CODEING_STYLE_EXCLUDES} "test")
  endif()

  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
  check_coding_style("$ENV{PACKAGE_DEVTOOLS_DIR}" "${CODEING_STYLE_EXCLUDES}")
endif()

# find extra libs : protobuf, python, pybind11
find_nn_extralib(EXTRA_LIBS)

# generate domain opcodes for NN
# the target `nn_od` will be created
gen_domain_od(
  "$ENV{PACKAGE_BASE_DIR}/irgen/od/odgen.py"
  "${PROJECT_SOURCE_DIR}/core/od/opcode_def.yml"
  "${CMAKE_BINARY_DIR}/include/nn/core"
  nn
)
add_dependencies(nn_depend nn_od)

#--------------------------------------------------------------------
# Logic: Target
#--------------------------------------------------------------------
# add glob include list
add_inc_list("${PROJECT_SOURCE_DIR}/include;${CMAKE_BINARY_DIR}/include")
include_directories("${PACKAGE_INC_DIR}")

# set target libraries
set_nn_lib()

# add glob depends for allnn
add_custom_target(allnn DEPENDS nn_depend)

if(NN_BUILD_TEST)
  enable_testing()
endif()

# build component libs
add_subdirectory(util)
add_subdirectory(core)
add_subdirectory(onnx)
add_subdirectory(onnx2air)
add_subdirectory(llama)
add_subdirectory(vector)
add_subdirectory(opt)

# build component executable
add_subdirectory(driver)

# build benchmark
if(BUILD_BENCH)
  add_subdirectory(benchmark)
endif()

# build e2e test
if(NN_BUILD_TEST)
  add_subdirectory(test)
endif()

# build docs
if(BUILD_DOC)
  build_app_doc(NN_INSTALL_APP ${PROJECT_NAME})
endif()

#--------------------------------------------------------------------
# Logic: Package
#--------------------------------------------------------------------
# install header file
install(DIRECTORY include/ DESTINATION include)

install(FILES
    ${PROJECT_SOURCE_DIR}/cmake/function_library.cmake
    ${PROJECT_SOURCE_DIR}/cmake/function_extralib.cmake
    DESTINATION share/${PACKAGE_NAME}/cmake/${PROJECT_NAME}
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_Version.cmake"
    VERSION ${NN_ADDON_VERSION}
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
