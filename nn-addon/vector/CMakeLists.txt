#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

file (GLOB_RECURSE NNVECTOR_SRC_FILES CONFIGURE_DEPENDS src/*.cxx)

set (NNVECTOR_INCLUDE_DIRS "")
list (APPEND NNVECTOR_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
list (APPEND NNVECTOR_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories (${NNVECTOR_INCLUDE_DIRS})
set (NNVECTOR_INCLUDE_DIRS "${NNVECTOR_INCLUDE_DIRS}" CACHE INTERNAL "")

# generate type ruls for vector
set(VECTOR_TYPE_DIR "${CMAKE_BINARY_DIR}/include/nn")
gen_type_checkers (${PYTHON_EXECUTABLE} "${CMAKE_CURRENT_LIST_DIR}/src/vector_type.py" ${VECTOR_TYPE_DIR})
include_directories (${VECTOR_TYPE_DIR})

add_library (nnvector_obj OBJECT ${NNVECTOR_SRC_FILES} ${VECTOR_TYPE_DIR}/vector_type.cxx)
add_dependencies(nnvector_obj nn_depend)
set_property (TARGET nnvector_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

if (BUILD_STATIC)
	add_library (NNvector STATIC $<TARGET_OBJECTS:nnvector_obj>)
	target_link_libraries (NNvector ${EXTRA_LIBS})
	set_property (TARGET NNvector PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	install (TARGETS NNvector EXPORT NNTargets DESTINATION lib)
endif ()

# generate vector options
set(VOPTION_OUTPUT_DIR "${CMAKE_BINARY_DIR}/include/nn/vector")
set(VOPTION_INPUT_DIR "config")
gen_option(${PYTHON_EXECUTABLE} voption ${VOPTION_OUTPUT_DIR} ${VOPTION_INPUT_DIR})
add_dependencies(nn_depend voption)

add_custom_target (nnvector_all)

if (BUILD_STATIC)
	set (NNVECTOR_LIBS ${NNVECTOR_LIBS} PUBLIC NNvector NNcore NNonnx2air)
	add_dependencies (nnvector_all NNvector)
endif()

if (NN_BUILD_TEST)
	build_app_test (NN_INSTALL_APP "${NN_LIBS}")
endif()

# Leave it here for now. Need to change example or ctest
set (NNVECTOR_EGAPP "")
if (NN_BUILD_EXAMPLE)
	file (GLOB NNVECTOR_EXAMPLE_SRC_FILES CONFIGURE_DEPENDS example/*.cxx)
	foreach (app ${NNVECTOR_EXAMPLE_SRC_FILES})
		get_filename_component (exe ${app} NAME_WE)
		add_executable (${exe} ${app})
		set_property (TARGET ${exe} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/example)
		set (NNVECTOR_EGAPP ${NNVECTOR_EGAPP} ${exe})
		target_link_libraries (${exe} ${NN_LIBS})
		target_link_options (${exe} PRIVATE -Wl,--export-dynamic)
		# add_test (NAME ${exe} COMMAND ${CMAKE_BINARY_DIR}/example/${exe})
	endforeach ()

	add_custom_target (nnvector_example)
	add_dependencies (nnvector_example ${NNVECTOR_EGAPP})
	add_dependencies (nnvector_all nnvector_example)
endif ()

if (NN_INSTALL_APP)
	install (TARGETS ${NNVECTOR_EGAPP} RUNTIME DESTINATION example)
endif ()

if (BUILD_UNITTEST)
	build_app_unittest (NN_INSTALL_APP "${NN_UTLIBS}" "ut_nnvector")
endif()
