#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

file (GLOB_RECURSE FHECORE_SRC_FILES CONFIGURE_DEPENDS src/*.cxx)

set (FHECORE_INCLUDE_DIRS "")
list (APPEND FHECORE_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
list (APPEND FHECORE_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories (${FHECORE_INCLUDE_DIRS})
set (FHECORE_INCLUDE_DIRS "${FHECORE_INCLUDE_DIRS}" CACHE INTERNAL "")

add_library (fhecore_obj OBJECT ${FHECORE_SRC_FILES})
add_dependencies(fhecore_obj fhe_depend)
set_property (TARGET fhecore_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

if (BUILD_STATIC)
	add_library (FHEcore STATIC $<TARGET_OBJECTS:fhecore_obj>)
	set_property (TARGET FHEcore PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	install (TARGETS FHEcore EXPORT FHETargets DESTINATION lib)
endif ()

add_custom_target (fhecore_all)

if (BUILD_STATIC)
	set (FHECORE_LIBS ${FHECORE_LIBS} PUBLIC FHEcore)
	add_dependencies (fhecore_all FHEcore)
endif()

if (FHE_BUILD_TEST)
	build_app_test (FHE_INSTALL_APP "${FHE_LIBS}")
endif ()

if (FHE_BUILD_EXAMPLE)
	build_app_example (FHE_INSTALL_APP "${FHE_LIBS}")
endif ()

if (BUILD_UNITTEST)
	build_app_unittest (FHE_INSTALL_APP "${FHE_UTLIBS}" "ut_fhecore")
endif()
