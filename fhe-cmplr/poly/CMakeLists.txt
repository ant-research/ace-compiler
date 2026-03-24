#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

file(GLOB_RECURSE FHEPOLY_SRC_FILES CONFIGURE_DEPENDS src/*.cxx)

set(FHEPOLY_INCLUDE_DIRS "")
list(APPEND FHEPOLY_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
list(APPEND FHEPOLY_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories(${FHEPOLY_INCLUDE_DIRS})
set(FHEPOLY_INCLUDE_DIRS "${FHEPOLY_INCLUDE_DIRS}" CACHE INTERNAL "")

add_library(fhepoly_obj OBJECT ${FHEPOLY_SRC_FILES})
add_dependencies(fhepoly_obj fhe_depend)
set_property(TARGET fhepoly_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

if(BUILD_STATIC)
  add_library(FHEpoly STATIC $<TARGET_OBJECTS:fhepoly_obj>)
  set_property(TARGET FHEpoly PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	target_link_libraries(FHEpoly PRIVATE ${FHERT_ANT_LIBS})
  install(TARGETS FHEpoly EXPORT FHETargets DESTINATION lib)
endif()

# generate poly options
set(CKKS_OPTION_OUTPUT_DIR "${CMAKE_BINARY_DIR}/include/fhe/poly")
set(CKKS_OPTION_INPUT_DIR "config")
gen_option(${PYTHON_EXECUTABLE} poly_option ${CKKS_OPTION_OUTPUT_DIR} ${CKKS_OPTION_INPUT_DIR})
add_dependencies(fhe_depend poly_option)

add_custom_target(fhepoly_all)

if(BUILD_STATIC)
	set(FHEPOLY_LIBS ${FHEPOLY_LIBS} PUBLIC FHEpoly)
	add_dependencies(fhepoly_all FHEpoly)
endif()

if(FHE_BUILD_TEST)
	build_app_test(FHE_INSTALL_APP "${FHE_LIBS};${FHERT_ANT_LIBS}")
endif()

if(FHE_BUILD_EXAMPLE)
	build_app_example(FHE_INSTALL_APP "${FHE_LIBS}")
endif()

if(BUILD_UNITTEST)
	build_app_unittest(FHE_INSTALL_APP "${FHE_UTLIBS};${FHERT_ANT_LIBS}" "ut_fhepoly")
endif()
