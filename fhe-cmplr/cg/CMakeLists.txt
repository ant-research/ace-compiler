#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

file (GLOB_RECURSE FHECG_SRC_FILES CONFIGURE_DEPENDS src/*.cxx)

set (FHECG_INCLUDE_DIRS "")
list (APPEND FHECG_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
list (APPEND FHECG_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories (${FHECG_INCLUDE_DIRS})
set (FHECG_INCLUDE_DIRS "${FHECG_INCLUDE_DIRS}" CACHE INTERNAL "")

add_library (fhecg_obj OBJECT ${FHECG_SRC_FILES})
add_dependencies(fhecg_obj fhe_depend)
set_property (TARGET fhecg_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

if (BUILD_STATIC)
	add_library (FHEcg STATIC $<TARGET_OBJECTS:fhecg_obj>)
	set_property (TARGET FHEcg PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	install (TARGETS FHEcg EXPORT FHETargets DESTINATION lib)
endif ()

add_custom_target (fhecg_all)

if (BUILD_STATIC)
	set (FHECG_LIBS ${FHECG_LIBS} PUBLIC FHEcg)
	add_dependencies (fhecg_all FHEcg)
endif()


if (FHE_BUILD_TEST)
	build_app_test (FHE_INSTALL_APP "${FHE_LIBS}")
endif ()

if (FHE_BUILD_EXAMPLE)
	build_app_example (FHE_INSTALL_APP "${FHE_LIBS}")
endif ()

if (BUILD_UNITTEST)
	build_app_unittest (FHE_INSTALL_APP "${FHE_UTLIBS}" "ut_fhecg")
endif()