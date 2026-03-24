#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

file (GLOB_RECURSE AIRUTIL_SRC_FILES CONFIGURE_DEPENDS src/*.cxx)

set (AIRUTIL_INCLUDE_DIRS "")
list (APPEND AIRUTIL_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
list (APPEND AIRUTIL_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories (${AIRUTIL_INCLUDE_DIRS})
set (AIRUTIL_INCLUDE_DIRS "${AIRUTIL_INCLUDE_DIRS}" CACHE INTERNAL "")

# build jsoncpp dependent libraries
include(jsoncpp)

add_library (airutil_obj OBJECT ${AIRUTIL_SRC_FILES})
add_dependencies (airutil_obj air_depend)
set_property (TARGET airutil_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

if (BUILD_STATIC)
	add_library (AIRutil STATIC $<TARGET_OBJECTS:airutil_obj> $<TARGET_OBJECTS:jsoncpp_objects>)
	set_property (TARGET AIRutil PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	target_include_directories(AIRutil PRIVATE ${JSONCPP_INCLUDE_DIRS})
	install (TARGETS AIRutil EXPORT AIRTargets DESTINATION lib)
endif ()

add_custom_target (airutil_all)

if (BUILD_STATIC)
	set (AIRUTIL_LIBS ${AIRUTIL_LIBS} PUBLIC AIRutil)
	add_dependencies (airutil_all AIRutil)
endif()

if (AIR_BUILD_TEST)
	build_app_test (AIR_INSTALL_APP "${AIR_LIBS}")
endif()

if (AIR_BUILD_EXAMPLE)
	build_app_example (AIR_INSTALL_APP "${AIR_LIBS}")
endif ()

if (BUILD_UNITTEST)
	build_app_unittest (AIR_INSTALL_APP "${AIR_UTLIBS}" "ut_airutil")
endif()