#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

file(GLOB_RECURSE FHECKKS_SRC_FILES CONFIGURE_DEPENDS src/*.cxx)

set(FHECKKS_INCLUDE_DIRS "")
list(APPEND FHECKKS_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
list(APPEND FHECKKS_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories(${FHECKKS_INCLUDE_DIRS})
set(FHECKKS_INCLUDE_DIRS "${FHECKKS_INCLUDE_DIRS}" CACHE INTERNAL "")

add_library(fheckks_obj OBJECT ${FHECKKS_SRC_FILES})
add_dependencies(fheckks_obj fhe_depend)
set_property(TARGET fheckks_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

# support ckks python dsl
find_nn_extralib(EXTRA_LIBS)

if(BUILD_STATIC)
  add_library(FHEckks STATIC $<TARGET_OBJECTS:fheckks_obj>)
  set_property(TARGET FHEckks PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  install(TARGETS FHEckks EXPORT FHETargets DESTINATION lib)
endif()

# generate sihe options
set(CKKS_OPTION_OUTPUT_DIR "${CMAKE_BINARY_DIR}/include/fhe/ckks")
set(CKKS_OPTION_INPUT_DIR "config")
gen_option(${PYTHON_EXECUTABLE} ckks_option ${CKKS_OPTION_OUTPUT_DIR} ${CKKS_OPTION_INPUT_DIR})
add_dependencies(fhe_depend ckks_option)

# generate domain opcodes for CKKS
# the target `ckks_od` will be created
gen_domain_od(
  "$ENV{PACKAGE_BASE_DIR}/irgen/od/odgen.py"
  "${PROJECT_SOURCE_DIR}/ckks/od/opcode_def.yml"
  "${CMAKE_BINARY_DIR}/include/fhe/ckks"
  ckks
)
add_dependencies(fhe_depend ckks_od)

add_custom_target(fheckks_all)

if(BUILD_STATIC)
  set(FHECKKS_LIBS ${FHECKKS_LIBS} PUBLIC FHEckks)
  add_dependencies(fheckks_all FHEckks)
endif()

if(FHE_BUILD_TEST)
  build_app_test(FHE_INSTALL_APP "${FHE_LIBS}")
endif()

if(FHE_BUILD_EXAMPLE)
  build_app_example(FHE_INSTALL_APP "${FHE_LIBS}")
endif()

if(BUILD_UNITTEST)
  build_app_unittest(FHE_INSTALL_APP "${FHERT_ANT_LIBS};${FHE_UTLIBS}" "ut_fheckks")
endif()