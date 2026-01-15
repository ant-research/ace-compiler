#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

set(RT_ANT_SRC_FILES)
file(GLOB_RECURSE RT_ANT_SRC_FILES CONFIGURE_DEPENDS
      context/src/*.c
      ckks/src/*.c
      poly/src/*.c
      lpoly/src/*.c
      hal/src/*.c
      util/src/*.c )

# Because the Bare system does not support file sytem, So let's do that for now
if(RTLIB_ENABLE_LINUX)
  file(GLOB_RECURSE LINUX_SRC_FILES CONFIGURE_DEPENDS porting/pt_mgr.c)
  list(APPEND RT_ANT_SRC_FILES ${LINUX_SRC_FILES})
else()
  file(GLOB_RECURSE BARE_SRC_FILES CONFIGURE_DEPENDS porting/pt_mgr_bare.c)
  list(APPEND RT_ANT_SRC_FILES ${BARE_SRC_FILES})
endif()

file(GLOB_RECURSE RT_ANT_ENCODE_FILES CONFIGURE_DEPENDS
      encode/src/encode_context.c
      context/src/cmplr_context.c
      ckks/src/plain.c
      ckks/src/encoder.c
      ckks/src/param.c
      poly/src/rns_poly_impl.c
      poly/src/rns_poly.c
      util/src/crt.c
      util/src/std_param.c
      util/src/type.c
      util/src/ntt.c
      util/src/number_theory.c
      util/src/random_sample.c
      util/src/prng.c
      hal/src/creg.c)

set(RT_ANT_INCLUDE_DIRS "")
list(APPEND RT_ANT_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
list(APPEND RT_ANT_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/rtlib/ant/include")
list(APPEND RT_ANT_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/util/include")
list(APPEND RT_ANT_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/hal/include")
list(APPEND RT_ANT_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/poly/include")
list(APPEND RT_ANT_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/ckks/include")
include_directories(${RT_ANT_INCLUDE_DIRS})
set(RT_ANT_INCLUDE_DIRS "${RT_ANT_INCLUDE_DIRS}" CACHE INTERNAL "")

# for external dependent libraries
include(uthash)
include(blake2)

add_library(fhert_ant_obj OBJECT ${RT_ANT_SRC_FILES} ${BLAKE2_SRC_FILES})
set_property(TARGET fhert_ant_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

if(BUILD_STATIC)
  add_library(FHErt_ant STATIC $<TARGET_OBJECTS:fhert_ant_obj>)
  set_property(TARGET FHErt_ant PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${RTLIB_INSTALL_PATH})
  target_link_libraries(FHErt_ant ${MATH_LIBS})
  add_dependencies(FHErt_ant rtlib_depend)
  install(TARGETS FHErt_ant EXPORT RTLIBTargets LIBRARY DESTINATION ${RTLIB_INSTALL_PATH})

  add_library(FHErt_ant_encode STATIC ${RT_ANT_ENCODE_FILES} ${BLAKE2_SRC_FILES})
  set_target_properties(FHErt_ant_encode PROPERTIES
  POSITION_INDEPENDENT_CODE ON
  RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/${RTLIB_INSTALL_PATH}
  )
  target_link_libraries(FHErt_ant_encode PUBLIC ${MATH_LIBS})
  add_dependencies(FHErt_ant_encode rtlib_depend)
  install(TARGETS FHErt_ant_encode EXPORT RTLIBTargets LIBRARY DESTINATION ${RTLIB_INSTALL_PATH})
endif()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION rtlib/include/ant)

add_custom_target(fhert_ant_all)

if(BUILD_STATIC)
  set(FHERT_ANT_LIB ${FHERT_ANT_LIB} PUBLIC FHErt_ant FHErt_common)
  add_dependencies(fhert_ant_all FHErt_ant)
endif()

if(BUILD_UNITTEST AND NOT RTLIB_SEED_MODE STREQUAL "fixed")
  build_app_unittest(RTLIB_INSTALL_APP "${FHERT_ANT_LIB};${UT_LIBS}" "ut_fhert_ant")
endif()

if(FHE_BUILD_TEST)
  add_executable(test_ant_encode ${CMAKE_CURRENT_SOURCE_DIR}/test/test_ant_encode.c)
  target_include_directories(test_ant_encode PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ant/include)
  target_link_libraries(test_ant_encode FHErt_ant_encode FHErt_common)
  set_property(TARGET test_ant_encode PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test)
  add_test(NAME test_ant_encode COMMAND ${CMAKE_BINARY_DIR}/test/test_ant_encode)
endif()

set(FHERT_ANT_EGAPP "")
if(FHE_BUILD_EXAMPLE)
  file(GLOB FHERT_ANT_EXAMPLE_SRC_FILES CONFIGURE_DEPENDS example/*.c example/*.cxx)
  foreach(app ${FHERT_ANT_EXAMPLE_SRC_FILES})
    get_filename_component(exe ${app} NAME_WE)
    add_executable(${exe} ${app})
    set_property(TARGET ${exe} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/example)
    set(FHERT_ANT_EGAPP ${FHERT_ANT_EGAPP} ${exe})
    target_link_libraries(${exe} ${FHERT_ANT_LIB} ${FHE_LIBS})
    add_test( NAME ${exe} COMMAND ${CMAKE_BINARY_DIR}/example/${exe})
  endforeach()

  add_custom_target(fhert_ant_example)
  add_dependencies(fhert_ant_example ${FHERT_ANT_EGAPP})
  add_dependencies(fhert_ant_all fhert_ant_example)
endif()

if(RTLIB_BUILD_EXAMPLE)
  file(GLOB FHERT_ANT_DATASET_SRC_FILES CONFIGURE_DEPENDS dataset/*.cxx)
  foreach(app ${FHERT_ANT_DATASET_SRC_FILES})
    get_filename_component(exe ${app} NAME_WE)
    add_executable(${exe} ${app})
    set_property(TARGET ${exe} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dataset)
    target_link_libraries(${exe} ${FHERT_ANT_LIB})
  endforeach()
endif()

if(FHE_BUILD_EXAMPLE)
  add_subdirectory(imagenet)
endif()

if(BUILD_BENCH)
  add_subdirectory(benchmark)
endif()

if(RTLIB_INSTALL_APP)
  install(TARGETS ${FHERT_ANT_UTAPP} RUNTIME DESTINATION unittest)
  install(TARGETS ${FHERT_ANT_EGAPP} RUNTIME DESTINATION example)
endif()
