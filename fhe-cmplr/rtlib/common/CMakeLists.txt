#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

set(RTLIB_COMMON_SRC_FILES)

file(GLOB_RECURSE COMMON_SRC_FILES CONFIGURE_DEPENDS src/*.c)
list(APPEND RTLIB_COMMON_SRC_FILES ${COMMON_SRC_FILES})

if(RTLIB_ENABLE_LINUX)
  file(GLOB_RECURSE LINUX_SRC_FILES CONFIGURE_DEPENDS linux/*.c)
  list(APPEND RTLIB_COMMON_SRC_FILES ${LINUX_SRC_FILES})
else()
  message(STATUS "Bare Enviroment does not support OS, file read|write")
endif()

set(FHERTLIB_INCLUDE_DIRS "")
list(APPEND FHERTLIB_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
list(APPEND FHERTLIB_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories(${FHERTLIB_INCLUDE_DIRS})
set(FHERTLIB_INCLUDE_DIRS "${FHERTLIB_INCLUDE_DIRS}" CACHE INTERNAL "")

add_definitions(-DOPENFHE_COMPAT)
add_library(fhert_common_obj OBJECT ${RTLIB_COMMON_SRC_FILES})
set_property(TARGET fhert_common_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

if(BUILD_STATIC)
  add_library(FHErt_common STATIC $<TARGET_OBJECTS:fhert_common_obj>)
  set_property(TARGET FHErt_common PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${RTLIB_INSTALL_PATH})
  add_dependencies(FHErt_common rtlib_depend)
  install(TARGETS FHErt_common EXPORT RTLIBTargets DESTINATION ${RTLIB_INSTALL_PATH})
endif()

# install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include/rtlib)

set(FHERT_COMMON_UTAPP "")
if(BUILD_UNITTEST)
  file(GLOB FHERT_COMMON_UNITTEST_SRC_FILES CONFIGURE_DEPENDS unittest/*.cxx)
  set(FHERT_COMMON_UNITTEST_SRC_FILES ${FHERT_COMMON_UNITTEST_SRC_FILES})
  add_executable(ut_fhert_common ${FHERT_COMMON_UNITTEST_SRC_FILES} ${UNITTESTMAIN})
  set_property(TARGET ut_fhert_common PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/unittest)
  target_link_libraries(ut_fhert_common PUBLIC ${UT_LIBS} ${FHERT_ANT_LIBS})
  set(FHERT_COMMON_UTAPP ${FHERT_COMMON_UTAPP} ut_fhert_common)
  # Add it here for now, this block of code should be merged into build_app_unittest
  target_include_directories(ut_fhert_common PRIVATE $ENV{UNITTEST_INCLUDE_DIR})

  add_custom_command(OUTPUT run_fhert_common_utapp WORKING_DIRECTORY ${CMAKE_BINARY_DIR} COMMAND ${CMAKE_BINARY_DIR}/unittest/ut_fhert_common)
  add_custom_target(test_fhert_common_ut DEPENDS ut_fhert_common run_fhert_common_utapp)
  # add_test(NAME ut_fhert_common COMMAND ${CMAKE_BINARY_DIR}/unittest/ut_fhert_common)    # temp rm with FHERT_COMMON.PT_MGR
endif()


if(RTLIB_INSTALL_APP)
  install(TARGETS ${FHERT_COMMON_UTAPP} RUNTIME DESTINATION unittest)
endif()
