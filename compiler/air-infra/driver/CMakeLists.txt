#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

file (GLOB_RECURSE AIRDRIVER_SRC_FILES CONFIGURE_DEPENDS src/*.cxx)

set (AIRDRIVER_INCLUDE_DIRS "")
list (APPEND AIRDRIVER_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
list (APPEND AIRDRIVER_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories (${AIRDRIVER_INCLUDE_DIRS})
set (AIRDRIVER_INCLUDE_DIRS "${AIRDRIVER_INCLUDE_DIRS}" CACHE INTERNAL "")

add_library (airdriver_obj OBJECT ${AIRDRIVER_SRC_FILES})
add_dependencies (airdriver_obj air_depend)
set_property (TARGET airdriver_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

if (BUILD_STATIC)
	add_dependencies (airdriver_obj AIRutil AIRbase AIRcore)
	add_library (AIRdriver STATIC $<TARGET_OBJECTS:airdriver_obj>)
	set_property (TARGET AIRdriver PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	install (TARGETS AIRdriver EXPORT AIRTargets DESTINATION lib)
endif ()

add_custom_target (airdriver_all)

if (BUILD_STATIC)
	set (DRIVERLIBS ${DRIVERLIBS} PUBLIC AIRutil AIRdriver AIRbase)
	add_dependencies (airdriver_all AIRdriver)
endif()

if (AIR_BUILD_TEST)
	build_app_test (AIR_INSTALL_APP "${AIR_LIBS}")
endif()

if (AIR_BUILD_EXAMPLE)
	build_app_example (AIR_INSTALL_APP "${AIR_LIBS}")
endif ()

if (BUILD_UNITTEST)
	build_app_unittest (AIR_INSTALL_APP "${AIR_UTLIBS}" "ut_airdriver")
endif()