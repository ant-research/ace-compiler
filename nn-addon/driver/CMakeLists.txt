#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

file (GLOB_RECURSE NNDRIVER_SRC_FILES CONFIGURE_DEPENDS src/*.cxx)

set (NNDRIVER_INCLUDE_DIRS "")
list (APPEND NNDRIVER_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
list (APPEND NNDRIVER_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories (${NNDRIVER_INCLUDE_DIRS})
set (NNDRIVER_INCLUDE_DIRS "${NNDRIVER_INCLUDE_DIRS}" CACHE INTERNAL "")

add_library (nndriver_obj OBJECT ${NNDRIVER_SRC_FILES})
add_dependencies(nndriver_obj nn_depend)
set_property (TARGET nndriver_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

if (BUILD_STATIC)
	add_dependencies (nndriver_obj NNutil NNonnx2air)
	add_library (NNdriver STATIC $<TARGET_OBJECTS:nndriver_obj>)
	set_property (TARGET NNdriver PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	install (TARGETS NNdriver EXPORT NNTargets DESTINATION lib)
endif ()

if (BUILD_STATIC)
	add_executable (onnx_cmplr ${NNDRIVER_SRC_FILES})
	target_link_libraries (onnx_cmplr ${NN_LIBS} ${EXTRA_LIBS})
	target_link_options (onnx_cmplr PRIVATE -Wl,--export-dynamic)
	add_dependencies (onnx_cmplr NNutil NNonnx2air)
	install (TARGETS onnx_cmplr RUNTIME DESTINATION bin)
endif ()

add_custom_target (nndriver_all)

if (BUILD_STATIC)
	set (NNDRIVER_LIBS ${NNDRIVER_LIBS} PUBLIC NNdriver)
	add_dependencies (nndriver_all NNdriver)
endif()

if (NN_BUILD_TEST)
	build_app_test (NN_INSTALL_APP "${NN_LIBS}")
endif()

if (NN_BUILD_EXAMPLE)
	build_app_example (NN_INSTALL_APP "${NN_LIBS}")
endif ()

if (BUILD_UNITTEST)
	build_app_unittest (NN_INSTALL_APP "${NN_UTLIBS}" "ut_nndriver")
endif()
