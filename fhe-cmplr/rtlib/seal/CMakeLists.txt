#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules/sealbts.cmake)

if(RTLIB_ENABLE_SEAL_BTS)
  #Enable the seal_bts library.
  seal_find_package(BTS)
else()
  #Enable the native seal library(version 4.1).
  seal_find_package(4.1)
endif()

file(GLOB_RECURSE RT_SEAL_SRC_FILES CONFIGURE_DEPENDS src/*.cxx)

list(APPEND RT_SEAL_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
list(APPEND RT_SEAL_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
list(APPEND RT_SEAL_INCLUDE_DIRS "${SEAL_INCLUDE_DIR}")
include_directories(${RT_SEAL_INCLUDE_DIRS})
set(RT_SEAL_INCLUDE_DIRS "${RT_SEAL_INCLUDE_DIRS}" CACHE INTERNAL "")

add_library(fhert_seal_obj OBJECT ${RT_SEAL_SRC_FILES})
set_property(TARGET fhert_seal_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

if (ENABLE_BTS)
    add_dependencies(fhert_seal_obj SEAL_BTS)
endif()

if(BUILD_STATIC)
  add_library(FHErt_seal STATIC $<TARGET_OBJECTS:fhert_seal_obj>)
  set_property(TARGET FHErt_seal PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/rtlib/lib)
  add_dependencies(FHErt_seal rtlib_depend)
  install(TARGETS FHErt_seal EXPORT FHETargets DESTINATION rtlib/lib)
endif()

add_custom_target(fhert_seal_all)

if(BUILD_STATIC)
  if (ENABLE_BTS) 
    set(FHERT_SEAL_LIB ${FHERT_SEAL_LIB} PUBLIC FHErt_seal ${SEAL_BTS_LIBS} FHErt_common)
  else()  
    set(FHERT_SEAL_LIB ${FHERT_SEAL_LIB} PUBLIC FHErt_seal SEAL::seal FHErt_common)
  endif()
  add_dependencies(fhert_seal_all FHErt_seal)
endif()

set(FHERT_SEAL_UTAPP "")
if(BUILD_UNITTEST)
  file(GLOB FHERT_SEAL_UNITTEST_SRC_FILES CONFIGURE_DEPENDS unittest/*.cxx)
  set(FHERT_SEAL_UNITTEST_SRC_FILES ${FHERT_SEAL_UNITTEST_SRC_FILES})
  add_executable(ut_fhert_seal ${FHERT_SEAL_UNITTEST_SRC_FILES} ${UNITTESTMAIN})
  set_property(TARGET ut_fhert_seal PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/unittest)
  target_link_libraries(ut_fhert_seal ${FHERT_SEAL_LIB} ${UT_LIBS})
  set(FHERT_SEAL_UTAPP ${FHERT_SEAL_UTAPP} ut_fhert_seal)
  # Add it here for now, this block of code should be merged into build_app_unittest
  target_include_directories(ut_fhert_seal PRIVATE $ENV{UNITTEST_INCLUDE_DIR})

  add_dependencies(fhert_seal_all ut_fhert_seal)

  add_custom_command(OUTPUT run_fhert_seal_utapp WORKING_DIRECTORY ${CMAKE_BINARY_DIR} COMMAND ${CMAKE_BINARY_DIR}/unittest/ut_fhert_seal)
  add_custom_target(test_fhert_seal_ut DEPENDS ut_fhertlib run_fhert_seal_utapp)
  add_test( NAME ut_fhert_seal COMMAND ${CMAKE_BINARY_DIR}/unittest/ut_fhert_seal)
endif()


set(FHERT_SEAL_EGAPP "")
if(RTLIB_BUILD_EXAMPLE)
  file(GLOB FHERT_SEAL_EXAMPLE_SRC_FILES CONFIGURE_DEPENDS example/*.cxx)
  foreach(app ${FHERT_SEAL_EXAMPLE_SRC_FILES})
    get_filename_component(exe ${app} NAME_WE)
    add_executable(${exe} ${app})
    set_property(TARGET ${exe} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/example)
    set(FHERT_SEAL_EGAPP ${FHERT_SEAL_EGAPP} ${exe})
    target_link_libraries(${exe} ${FHERT_SEAL_LIB} ${NN_LIBS})
    # add_test( NAME ${exe} COMMAND ${CMAKE_BINARY_DIR}/example/${exe})
  endforeach()

  add_custom_target(fhert_seal_example)
  add_dependencies(fhert_seal_example ${FHERT_SEAL_EGAPP})
  add_dependencies(fhert_seal_all fhert_seal_example)
endif()

if(RTLIB_INSTALL_APP)
  install(TARGETS ${FHERT_SEAL_UTAPP} RUNTIME DESTINATION unittest)
  install(TARGETS ${FHERT_SEAL_EGAPP} RUNTIME DESTINATION example)
endif()
