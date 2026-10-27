#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

file(GLOB_RECURSE FHESIHE_SRC_FILES CONFIGURE_DEPENDS src/*.cxx)

set(FHESIHE_INCLUDE_DIRS "")
list(APPEND FHESIHE_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
list(APPEND FHESIHE_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories(${FHESIHE_INCLUDE_DIRS})
set(FHESIHE_INCLUDE_DIRS "${FHESIHE_INCLUDE_DIRS}" CACHE INTERNAL "")

add_library(fhesihe_obj OBJECT ${FHESIHE_SRC_FILES})
add_dependencies(fhesihe_obj fhe_depend)
set_property(TARGET fhesihe_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

if(BUILD_STATIC)
  add_library(FHEsihe STATIC $<TARGET_OBJECTS:fhesihe_obj>)
  set_property(TARGET FHEsihe PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  install(TARGETS FHEsihe EXPORT FHETargets DESTINATION lib)
endif()

# generate sihe options
set(SIHE_OPTION_OUTPUT_DIR "${CMAKE_BINARY_DIR}/include/fhe/sihe")
set(SIHE_OPTION_INPUT_DIR "config")
gen_option(${PYTHON_EXECUTABLE} sihe_option ${SIHE_OPTION_OUTPUT_DIR} ${SIHE_OPTION_INPUT_DIR})
add_dependencies(fhe_depend sihe_option)

add_custom_target(fhesihe_all)

if(BUILD_STATIC)
  set(FHESIHE_LIBS ${FHESIHE_LIBS} PUBLIC FHEsihe)
  add_dependencies(fhesihe_all FHEsihe)
endif()

if(FHE_BUILD_TEST)
  build_app_test(FHE_INSTALL_APP "${FHE_LIBS}")
endif()

if(FHE_BUILD_EXAMPLE)
  build_app_example(FHE_INSTALL_APP "${FHE_LIBS}")
endif()

if(BUILD_UNITTEST)
  build_app_unittest(FHE_INSTALL_APP "${FHE_UTLIBS}" "ut_fhesihe")
endif()
