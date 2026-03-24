#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

file (GLOB_RECURSE NNONNX2AIR_SRC_FILES CONFIGURE_DEPENDS src/*.cxx)

set (NNONNX2AIR_INCLUDE_DIRS "")
list (APPEND NNONNX2AIR_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
list (APPEND NNONNX2AIR_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories (${NNONNX2AIR_INCLUDE_DIRS})
set (NNONNX2AIR_INCLUDE_DIRS "${NNONNX2AIR_INCLUDE_DIRS}" CACHE INTERNAL "")

# build onnx protobuf dependent libraries
include(onnx_pb)

add_library (nnonnx2air_obj OBJECT ${NNONNX2AIR_SRC_FILES})
add_dependencies(nnonnx2air_obj nn_depend)
set_property (TARGET nnonnx2air_obj PROPERTY POSITION_INDEPENDENT_CODE 1)


if (BUILD_STATIC)
	add_dependencies (nnonnx2air_obj NNutil NNonnx)
	add_library (NNonnx2air STATIC $<TARGET_OBJECTS:nnonnx2air_obj> $<TARGET_OBJECTS:onnx_objects>)
	set_property (TARGET NNonnx2air PROPERTY POSITION_INDEPENDENT_CODE 1)
	set_property (TARGET NNonnx2air PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	target_include_directories(NNonnx2air PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/onnx)

	install (TARGETS NNonnx2air EXPORT NNTargets DESTINATION lib)
endif ()

add_custom_target (nnonnx2air_all)

if (BUILD_STATIC)
	set (NNONNX2AIR_LIBS ${NNONNX2AIR_LIBS} PUBLIC NNonnx2air)
	add_dependencies (nnonnx2air_all NNonnx2air)
endif()

set (NNONNX2AIR_TESTAPP "")
if (NN_BUILD_TEST)
	file (GLOB NNONNX2AIR_TESTAPP_SRC_FILES CONFIGURE_DEPENDS test/*.cxx)
	foreach (app ${NNONNX2AIR_TESTAPP_SRC_FILES})
		get_filename_component (exe ${app} NAME_WE)
		add_executable (${exe} ${app})
		set_property (TARGET ${exe} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test)
		set (NNONNX2AIR_TESTAPP ${NNONNX2AIR_TESTAPP} ${exe})
		target_link_libraries (${exe} ${NN_LIBS} ${EXTRA_LIBS})
		add_test( NAME ${exe} COMMAND ${CMAKE_BINARY_DIR}/test/${exe})
	endforeach ()

	add_custom_target (nnonnx2air_test)
	add_dependencies (nnonnx2air_test ${NNONNX2AIR_TESTAPP})
	add_dependencies (nnonnx2air_all nnonnx2air_test)
endif()

if (NN_BUILD_EXAMPLE)
	build_app_example (NN_INSTALL_APP "${NN_LIBS}")
endif ()

if (BUILD_UNITTEST)
	build_app_unittest (NN_INSTALL_APP "${NN_UTLIBS}" "ut_nnonnx2air")
endif()

if (NN_INSTALL_APP)
	install (TARGETS ${NNONNX2AIR_TESTAPP} RUNTIME DESTINATION test)
endif ()
