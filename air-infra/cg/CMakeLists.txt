#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

file (GLOB_RECURSE AIRCG_SRC_FILES CONFIGURE_DEPENDS src/*.cxx)

set (AIRCG_INCLUDE_DIRS "")
list (APPEND AIRCG_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
list (APPEND AIRCG_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories (${AIRCG_INCLUDE_DIRS})
set (AIRCG_INCLUDE_DIRS "${AIRCG_INCLUDE_DIRS}" CACHE INTERNAL "")

add_library (aircg_obj OBJECT ${AIRCG_SRC_FILES})
add_dependencies (aircg_obj air_depend)
set_property (TARGET aircg_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

# generate code_gen option files
set(AIR2HPU_OPTION_OUTPUT_DIR "${CMAKE_BINARY_DIR}/include/air/cg")
set(AIR2HPU_OPTION_INPUT_DIR "src")
gen_option(${PYTHON_EXECUTABLE} air_cg_option ${AIR2HPU_OPTION_OUTPUT_DIR} ${AIR2HPU_OPTION_INPUT_DIR})
add_dependencies(aircg_obj air_cg_option)

if (BUILD_STATIC)
	add_dependencies (aircg_obj AIRutil AIRbase)
	add_library (AIRcg STATIC $<TARGET_OBJECTS:aircg_obj>)
	set_property (TARGET AIRcg PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	install (TARGETS AIRcg EXPORT AIRTargets DESTINATION lib)
endif ()

add_custom_target (aircg_all)

if (BUILD_STATIC)
	set (AIRCG_LIBS ${AIRCG_LIBS} PUBLIC AIRutil AIRcg)
	add_dependencies (aircg_all AIRutil)
endif()

if (AIR_BUILD_TEST)
	build_app_test (AIR_INSTALL_APP "${AIR_LIBS}")
endif()

if (AIR_BUILD_EXAMPLE)
	build_app_example (AIR_INSTALL_APP "${AIR_LIBS}")
endif ()

if (BUILD_UNITTEST)
	build_app_unittest (AIR_INSTALL_APP "${AIR_UTLIBS}" "ut_aircg")
endif()