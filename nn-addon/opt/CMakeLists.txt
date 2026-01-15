#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

file (GLOB_RECURSE NNOPT_SRC_FILES CONFIGURE_DEPENDS src/*.cxx)

set (NNOPT_INCLUDE_DIRS "")
list (APPEND NNOPT_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
list (APPEND NNOPT_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories (${NNOPT_INCLUDE_DIRS})
set (NNOPT_INCLUDE_DIRS "${NNOPT_INCLUDE_DIRS}" CACHE INTERNAL "")

add_library (nnopt_obj OBJECT ${NNOPT_SRC_FILES})
add_dependencies(nnopt_obj nn_depend)
set_property (TARGET nnopt_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

if (BUILD_STATIC)
	add_dependencies (nnopt_obj NNutil)
	add_library (NNopt STATIC $<TARGET_OBJECTS:nnopt_obj>)
	set_property (TARGET NNopt PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	install (TARGETS NNopt EXPORT NNTargets DESTINATION lib)
endif ()

add_custom_target (nnopt_all)

if (BUILD_STATIC)
	set (NNOPT_LIBS ${NNOPT_LIBS} PUBLIC NNutil NNopt)
	add_dependencies (nnopt_all NNutil NNopt)
endif()

if (NN_BUILD_TEST)
	build_app_test (NN_INSTALL_APP "${NN_LIBS}")
endif()

if (NN_BUILD_EXAMPLE)
	build_app_example (NN_INSTALL_APP "${NN_LIBS}")
endif ()

if (BUILD_UNITTEST)
	build_app_unittest (NN_INSTALL_APP "${NN_UTLIBS}" "ut_nnopt")
endif()