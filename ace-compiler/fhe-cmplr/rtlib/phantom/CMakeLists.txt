#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules/phantom.cmake)

option(ENABLE_PERFORMANCE_STATS "Enable performance statistics" ON)

if(ENABLE_PERFORMANCE_STATS)
    add_definitions(-DENABLE_PERFORMANCE_STATS)
    message(STATUS "ENABLE_PERFORMANCE_STATS is: ${ENABLE_PERFORMANCE_STATS}")
endif()


build_external_phantom()

file(GLOB_RECURSE RT_PHANTOM_SRC_FILES CONFIGURE_DEPENDS src/*.cu)

set(RT_PHANTOM_INCLUDE_DIRS "")
list(APPEND RT_PHANTOM_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
list(APPEND RT_PHANTOM_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
list(APPEND RT_PHANTOM_INCLUDE_DIRS "${PHANTOM_INCLUDE_DIR}")
list(APPEND RT_PHANTOM_INCLUDE_DIRS ${CUDA_INCLUDE_DIRS})
include_directories(${RT_PHANTOM_INCLUDE_DIRS})
set(RT_PHANTOM_INCLUDE_DIRS "${RT_PHANTOM_INCLUDE_DIRS}" CACHE INTERNAL "")

add_library(fhert_phantom_obj OBJECT ${RT_PHANTOM_SRC_FILES})
set_property(TARGET fhert_phantom_obj PROPERTY POSITION_INDEPENDENT_CODE 1)
set_target_properties(fhert_phantom_obj PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
add_dependencies(fhert_phantom_obj phantom_external)

if(BUILD_STATIC)
  add_library(FHErt_phantom STATIC $<TARGET_OBJECTS:fhert_phantom_obj>)
  set_property(TARGET FHErt_phantom PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/rtlib/lib)
  add_dependencies(FHErt_phantom rtlib_depend)
  install(TARGETS FHErt_phantom EXPORT FHETargets DESTINATION rtlib/lib)
endif()

add_custom_target(fhert_phantom_all)

if(BUILD_STATIC)
  set(FHERT_PHANTOM_LIB ${FHERT_PHANTOM_LIB} PUBLIC   
    FHErt_phantom   
    ${PHANTOM_LIBS} 
    FHErt_common
  )
  add_dependencies(fhert_phantom_all FHErt_phantom)
endif()

set(FHERT_PHANTOM_EGAPP "")
if(RTLIB_BUILD_EXAMPLE)
  file(GLOB FHERT_PHANTOM_EXAMPLE_SRC_FILES CONFIGURE_DEPENDS example/*.cu)
  foreach(app ${FHERT_PHANTOM_EXAMPLE_SRC_FILES})
    get_filename_component(exe ${app} NAME_WE)
    add_executable(${exe} ${app})
    set_property(TARGET ${exe} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/example)
    set(FHERT_PHANTOM_EGAPP ${FHERT_PHANTOM_EGAPP} ${exe})
    target_link_libraries(${exe}
      ${FHERT_PHANTOM_LIB}
      ${NN_LIBS}
      ${CUDA_LIBRARIES}
    )
  endforeach()

  add_custom_target(fhert_phantom_example)
  add_dependencies(fhert_phantom_example ${FHERT_PHANTOM_EGAPP})
  add_dependencies(fhert_phantom_all fhert_phantom_example)
endif()

if(RTLIB_INSTALL_APP)
  install(TARGETS ${FHERT_PHANTOM_UTAPP} RUNTIME DESTINATION unittest)
  install(TARGETS ${FHERT_PHANTOM_EGAPP} RUNTIME DESTINATION example)
endif()