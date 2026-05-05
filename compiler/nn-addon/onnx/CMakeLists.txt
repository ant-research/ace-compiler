#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

file (GLOB_RECURSE NNONNX_SRC_FILES CONFIGURE_DEPENDS src/*.cxx)

set (NNONNX_INCLUDE_DIRS "")
list (APPEND NNONNX_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
list (APPEND NNONNX_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories (${NNONNX_INCLUDE_DIRS})
set (NNONNX_INCLUDE_DIRS "${NNONNX_INCLUDE_DIRS}" CACHE INTERNAL "")

add_library (nnonnx_obj OBJECT ${NNONNX_SRC_FILES})
add_dependencies(nnonnx_obj nn_depend)
set_property (TARGET nnonnx_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

if (BUILD_STATIC)
	add_dependencies (nnonnx_obj NNutil)
	add_library (NNonnx STATIC $<TARGET_OBJECTS:nnonnx_obj>)
	set_property (TARGET NNonnx PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	install (TARGETS NNonnx EXPORT NNTargets DESTINATION lib)
endif ()

add_custom_target (nnonnx_all)

if (BUILD_STATIC)
	set (NNONNX_LIBS ${NNONNX_LIBS} PUBLIC NNonnx)
	add_dependencies (nnonnx_all NNutil NNonnx)
endif()

if (NN_BUILD_TEST)
	build_app_test (NN_INSTALL_APP "${NN_LIBS}")
endif()

if (NN_BUILD_EXAMPLE)
	build_app_example (NN_INSTALL_APP "${NN_LIBS}")
endif ()

if (BUILD_UNITTEST)
	build_app_unittest (NN_INSTALL_APP "${NN_UTLIBS}" "ut_nnonnx")
endif()
