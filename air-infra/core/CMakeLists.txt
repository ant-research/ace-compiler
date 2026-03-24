#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

file (GLOB_RECURSE AIRCORE_SRC_FILES CONFIGURE_DEPENDS src/*.cxx)

set (AIRCORE_INCLUDE_DIRS "")
list (APPEND AIRCORE_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
list (APPEND AIRCORE_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories (${AIRCORE_INCLUDE_DIRS})
set (AIRCORE_INCLUDE_DIRS "${AIRCORE_INCLUDE_DIRS}" CACHE INTERNAL "")

add_library (aircore_obj OBJECT ${AIRCORE_SRC_FILES})
add_dependencies (aircore_obj air_depend)
set_property (TARGET aircore_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

if (BUILD_STATIC)
	add_dependencies (aircore_obj AIRbase AIRutil)
	add_library (AIRcore STATIC $<TARGET_OBJECTS:aircore_obj>)
	set_property (TARGET AIRcore PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	install (TARGETS AIRcore EXPORT AIRTargets DESTINATION lib)
endif ()

add_custom_target (aircore_all)

if (BUILD_STATIC)
	set (AIRCORE_LIBS ${AIRCORE_LIBS} PUBLIC AIRcore AIRbase AIRutil)
	add_dependencies (aircore_all AIRutil)
endif()

if (AIR_BUILD_TEST)
	build_app_test (AIR_INSTALL_APP "${AIR_LIBS}")
endif()

if (AIR_BUILD_EXAMPLE)
	build_app_example (AIR_INSTALL_APP "${AIR_LIBS}")
endif ()

if (BUILD_UNITTEST)
	build_app_unittest (AIR_INSTALL_APP "${AIR_UTLIBS}" "ut_aircore")
endif()