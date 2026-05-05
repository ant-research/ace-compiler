#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

file(GLOB_RECURSE FHEDRIVER_SRC_FILES CONFIGURE_DEPENDS src/*.cxx)

set(FHEDRIVER_INCLUDE_DIRS "")
list(APPEND FHEDRIVER_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
list(APPEND FHEDRIVER_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories(${FHEDRIVER_INCLUDE_DIRS})
set(FHEDRIVER_INCLUDE_DIRS "${FHEDRIVER_INCLUDE_DIRS}" CACHE INTERNAL "")

add_library(fhedriver_obj OBJECT ${FHEDRIVER_SRC_FILES})
add_dependencies(fhedriver_obj fhe_depend)
set_property(TARGET fhedriver_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

if("${EXTRA_LIBS}" STREQUAL "")
  find_nn_extralib(EXTRA_LIBS)
endif()

if(BUILD_STATIC)
  add_library(FHEdriver STATIC $<TARGET_OBJECTS:fhedriver_obj>)
  set_property(TARGET FHEdriver PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  install(TARGETS FHEdriver EXPORT FHETargets DESTINATION lib)
endif()

if(BUILD_STATIC)
  add_executable(fhe_cmplr ${FHEDRIVER_SRC_FILES})
  target_link_libraries(fhe_cmplr ${FHE_LIBS} ${FHERT_ANT_LIBS} ${EXTRA_LIBS} ${PERF_LIBS})
  target_link_options(fhe_cmplr PRIVATE -Wl,--export-dynamic)
  install(TARGETS fhe_cmplr RUNTIME DESTINATION bin)
endif()

add_custom_target(fhedriver_all)

if(BUILD_STATIC)
  set(FHEDRIVER_LIBS ${FHEDRIVER_LIBS} PUBLIC FHEdriver)
add_dependencies(fhedriver_all FHEdriver)
endif()

if(FHE_BUILD_TEST)
  build_app_test(FHE_INSTALL_APP "${FHE_LIBS};${FHERT_ANT_LIBS}")
endif()

set(FHEDRIVER_EGAPP "")
if(FHE_BUILD_EXAMPLE)
  file(GLOB FHEDRIVER_EXAMPLE_SRC_FILES CONFIGURE_DEPENDS example/*.cxx)
  foreach(app ${FHEDRIVER_EXAMPLE_SRC_FILES})
    get_filename_component(exe ${app} NAME_WE)
	add_executable(${exe} ${app})
	set_property(TARGET ${exe} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/example)
	set(FHEDRIVER_EGAPP ${FHEDRIVER_EGAPP} ${exe})
	target_link_libraries(${exe} ${FHE_LIBS} ${FHERT_ANT_LIBS} ${EXTRA_LIBS})
	# add_test( NAME ${exe} COMMAND ${CMAKE_BINARY_DIR}/example/${exe})
  endforeach()
  
  add_custom_target(fhedriver_example)
  add_dependencies(fhedriver_example ${FHEDRIVER_EGAPP})
  add_dependencies(fhedriver_all fhedriver_example)
endif()

if(FHE_INSTALL_APP)
  install(TARGETS ${FHEDRIVER_EGAPP} RUNTIME DESTINATION example)
endif()

if(BUILD_UNITTEST)
  build_app_unittest(FHE_INSTALL_APP "${FHE_UTLIBS}" "ut_fhedriver")
endif()
