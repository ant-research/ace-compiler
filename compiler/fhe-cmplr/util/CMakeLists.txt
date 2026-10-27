#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

file (GLOB_RECURSE FHEUTIL_SRC_FILES CONFIGURE_DEPENDS src/*.cxx)

set (FHEUTIL_INCLUDE_DIRS "")
list (APPEND FHEUTIL_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
list (APPEND FHEUTIL_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories (${FHEUTIL_INCLUDE_DIRS})
set (FHEUTIL_INCLUDE_DIRS "${FHEUTIL_INCLUDE_DIRS}" CACHE INTERNAL "")

add_library (fheutil_obj OBJECT ${FHEUTIL_SRC_FILES})
add_dependencies(fheutil_obj fhe_depend)
set_property (TARGET fheutil_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

if (BUILD_STATIC)
	add_library (FHEutil STATIC $<TARGET_OBJECTS:fheutil_obj>)
	set_property (TARGET FHEutil PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	install (TARGETS FHEutil EXPORT FHETargets DESTINATION lib)
endif ()

add_custom_target (fheutil_all)

if (BUILD_STATIC)
	set (FHEUTIL_LIBS ${FHEUTIL_LIBS} PUBLIC FHEutil)
	add_dependencies (fheutil_all FHEutil)
endif()

if (FHE_BUILD_TEST)
	build_app_test (FHE_INSTALL_APP "${FHE_LIBS}")
endif ()

if (FHE_BUILD_EXAMPLE)
	build_app_example (FHE_INSTALL_APP "${FHE_LIBS}")
endif ()

if (BUILD_UNITTEST)
	build_app_unittest (FHE_INSTALL_APP "${FHE_UTLIBS}" "ut_fheutil")
endif()
