#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

file (GLOB_RECURSE FHEOPT_SRC_FILES CONFIGURE_DEPENDS src/*.cxx)

set (FHEOPT_INCLUDE_DIRS "")
list (APPEND FHEOPT_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
list (APPEND FHEOPT_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories (${FHEOPT_INCLUDE_DIRS})
set (FHEOPT_INCLUDE_DIRS "${FHEOPT_INCLUDE_DIRS}" CACHE INTERNAL "")

add_library (fheopt_obj OBJECT ${FHEOPT_SRC_FILES})
add_dependencies(fheopt_obj fhe_depend)
set_property (TARGET fheopt_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

if (BUILD_STATIC)
	add_library (FHEopt STATIC $<TARGET_OBJECTS:fheopt_obj>)
	set_property (TARGET FHEopt PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	install (TARGETS FHEopt EXPORT FHETargets DESTINATION lib)
endif ()

add_custom_target (fheopt_all)

if (BUILD_STATIC)
	set (FHEOPT_LIBS ${FHEOPT_LIBS} PUBLIC FHEopt)
	add_dependencies (fheopt_all FHEopt)
endif()

if (FHE_BUILD_TEST)
	build_app_test (FHE_INSTALL_APP "${FHE_LIBS}")
endif ()

if (FHE_BUILD_EXAMPLE)
	build_app_example (FHE_INSTALL_APP "${FHE_LIBS}")
endif ()

if (BUILD_UNITTEST)
	build_app_unittest (FHE_INSTALL_APP "${FHE_UTLIBS}" "ut_fheopt")
endif()
