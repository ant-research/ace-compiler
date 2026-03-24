#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

file (GLOB_RECURSE NNUTIL_SRC_FILES CONFIGURE_DEPENDS src/*.cxx)

set (NNUTIL_INCLUDE_DIRS "")
list (APPEND NNUTIL_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
list (APPEND NNUTIL_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories (${NNUTIL_INCLUDE_DIRS})
set (NNUTIL_INCLUDE_DIRS "${NNUTIL_INCLUDE_DIRS}" CACHE INTERNAL "")

add_library (nnutil_obj OBJECT ${NNUTIL_SRC_FILES})
add_dependencies(nnutil_obj nn_depend)
set_property (TARGET nnutil_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

if (BUILD_STATIC)
#	add_dependencies (nnutil_obj NNutil)
	add_library (NNutil STATIC $<TARGET_OBJECTS:nnutil_obj>)
	set_property (TARGET NNutil PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	install (TARGETS NNutil EXPORT NNTargets DESTINATION lib)
endif ()

add_custom_target (nnutil_all)

if (BUILD_STATIC)
	set (NNUTILE_LIBS ${NNUTILE_LIBS} PUBLIC NNutil)
	add_dependencies (nnutil_all NNutil)
endif()

if (NN_BUILD_TEST)
	build_app_test (NN_INSTALL_APP "${NN_LIBS}")
endif()

if (NN_BUILD_EXAMPLE)
	build_app_example (NN_INSTALL_APP "${NN_LIBS}")
endif ()

if (BUILD_UNITTEST)
	build_app_unittest (NN_INSTALL_APP "${NN_UTLIBS}" "ut_nnutil")
endif()