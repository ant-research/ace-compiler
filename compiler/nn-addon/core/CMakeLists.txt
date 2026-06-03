#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

file (GLOB_RECURSE NNCORE_SRC_FILES CONFIGURE_DEPENDS src/*.cxx)

list (APPEND NNCORE_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
list (APPEND NNCORE_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories (${NNCORE_INCLUDE_DIRS})
set (NNCORE_INCLUDE_DIRS "${NNCORE_INCLUDE_DIRS}" CACHE INTERNAL "")


add_library (nncore_obj OBJECT ${NNCORE_SRC_FILES})
add_dependencies(nncore_obj nn_depend)
set_property (TARGET nncore_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

if (BUILD_STATIC)
	add_dependencies (nncore_obj NNutil)
	add_library (NNcore STATIC $<TARGET_OBJECTS:nncore_obj>)
	set_property (TARGET NNcore PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	install (TARGETS NNcore EXPORT NNTargets DESTINATION lib)
endif ()

add_custom_target (nncore_all)

if (BUILD_STATIC)
	set (NNCORE_LIBS ${NNCORE_LIBS} PUBLIC NNcore)
	add_dependencies (nncore_all NNutil NNcore)
endif()

if (NN_BUILD_TEST)
	build_app_test (NN_INSTALL_APP "${NN_LIBS}")
endif ()

if (NN_BUILD_EXAMPLE)
	build_app_example (NN_INSTALL_APP "${NN_LIBS}")
endif ()

if (BUILD_UNITTEST)
	build_app_unittest (NN_INSTALL_APP "${NN_UTLIBS}" "ut_nncore")
endif()
