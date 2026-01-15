#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

file (GLOB_RECURSE AIRBASE_SRC_FILES CONFIGURE_DEPENDS src/*.cxx)

set (AIRBASE_INCLUDE_DIRS "")
list (APPEND AIRBASE_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
list (APPEND AIRBASE_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories (${AIRBASE_INCLUDE_DIRS})
set (AIRBASE_INCLUDE_DIRS "${AIRBASE_INCLUDE_DIRS}" CACHE INTERNAL "")

add_library (airbase_obj OBJECT ${AIRBASE_SRC_FILES})
add_dependencies (airbase_obj air_depend)
set_property (TARGET airbase_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

if (BUILD_STATIC)
	add_dependencies (airbase_obj AIRutil)
	add_library (AIRbase STATIC $<TARGET_OBJECTS:airbase_obj>)
	set_property (TARGET AIRbase PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	install (TARGETS AIRbase EXPORT AIRTargets DESTINATION lib)
endif ()

add_custom_target (airbase_all)

if (BUILD_STATIC)
	set (AIRBASE_LIBS ${AIRBASE_LIBS} PUBLIC AIRutil AIRbase)
	add_dependencies (airbase_all AIRutil)
endif()

if (AIR_BUILD_TEST)
	build_app_test (AIR_INSTALL_APP "${AIR_LIBS}")
endif()

if (AIR_BUILD_EXAMPLE)
	build_app_example (AIR_INSTALL_APP "${AIR_LIBS}")
endif ()

if (BUILD_UNITTEST)
	build_app_unittest (AIR_INSTALL_APP "${AIR_UTLIBS}" "ut_airbase")
endif()