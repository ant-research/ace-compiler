#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

file (GLOB_RECURSE AIROPT_SRC_FILES CONFIGURE_DEPENDS src/*.cxx)

set (AIROPT_INCLUDE_DIRS "")
list (APPEND AIROPT_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
list (APPEND AIROPT_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories (${AIROPT_INCLUDE_DIRS})
set (AIROPT_INCLUDE_DIRS "${AIROPT_INCLUDE_DIRS}" CACHE INTERNAL "")

add_library (airopt_obj OBJECT ${AIROPT_SRC_FILES})
add_dependencies (airopt_obj air_depend)
set_property (TARGET airopt_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

if (BUILD_STATIC)
	add_dependencies (airopt_obj AIRutil AIRbase AIRcore)
	add_library (AIRopt STATIC $<TARGET_OBJECTS:airopt_obj>)
	set_property (TARGET AIRopt PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	install (TARGETS AIRopt EXPORT AIRTargets DESTINATION lib)
endif ()

add_custom_target (airopt_all)

if (BUILD_STATIC)
	set (AIROPT_LIBS ${AIROPT_LIBS} PUBLIC AIRutil AIRbase AIRopt)
	add_dependencies (airopt_all AIRopt)
endif()

if (AIR_BUILD_TEST)
	build_app_test (AIR_INSTALL_APP "${AIR_LIBS}")
endif()

if (AIR_BUILD_EXAMPLE)
	build_app_example (AIR_INSTALL_APP "${AIR_LIBS}")
endif ()

if (BUILD_UNITTEST)
	build_app_unittest (AIR_INSTALL_APP "${AIR_UTLIBS}" "ut_airopt")
endif()
