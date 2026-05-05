#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

file (GLOB_RECURSE LLAMA_SRC_FILES CONFIGURE_DEPENDS src/*.cxx)

set (LLAMA_INCLUDE_DIRS "")
list (APPEND LLAMA_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
list (APPEND LLAMA_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories (${LLAMA_INCLUDE_DIRS})
set (LLAMA_INCLUDE_DIRS "${LLAMA_INCLUDE_DIRS}" CACHE INTERNAL "")

add_library (llama_obj OBJECT ${LLAMA_SRC_FILES})
add_dependencies(llama_obj nn_depend)
set_property (TARGET llama_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

if (BUILD_STATIC)
	add_library (NNllama STATIC $<TARGET_OBJECTS:llama_obj>)
	set_property (TARGET NNllama PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	install (TARGETS NNllama EXPORT NNTargets DESTINATION lib)
endif ()

add_custom_target (nnllama_all)

if (BUILD_STATIC)
	set (LLAMA_LIBS ${LLAMA_LIBS} PUBLIC NNllama NNcore NNvector)
	add_dependencies (nnllama_all NNllama)
endif()

if (NN_BUILD_TEST)
	build_app_test (NN_INSTALL_APP "${NN_LIBS}")
endif()

if (NN_BUILD_EXAMPLE)
	build_app_example (NN_INSTALL_APP "${NN_LIBS}")
endif ()

if (BUILD_UNITTEST)
	build_app_unittest (NN_INSTALL_APP "${NN_UTLIBS}" "ut_nnllama")
endif()
