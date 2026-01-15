#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

file (GLOB_RECURSE AIRPLUGIN_SRC_FILES CONFIGURE_DEPENDS src/*.cxx)

find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
if (Python3_FOUND)
    message (STATUS "Python3_INCLUDE_DIRS           : ${Python3_INCLUDE_DIRS}")
    message (STATUS "Python3_LIBRARIES              : ${Python3_LIBRARIES}")
else()
    message(FATAL_ERROR "Python not found. Please make sure Python is installed.")
endif()

set (AIRPLUGIN_INCLUDE_DIRS "")
list (APPEND AIRPLUGIN_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
list (APPEND AIRPLUGIN_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
list (APPEND AIRPLUGIN_INCLUDE_DIRS "${Python3_INCLUDE_DIRS}")
include_directories (${AIRPLUGIN_INCLUDE_DIRS})
set (AIRPLUGIN_INCLUDE_DIRS "${AIRPLUGIN_INCLUDE_DIRS}" CACHE INTERNAL "")

add_library (airplugin_obj OBJECT ${AIRPLUGIN_SRC_FILES})
add_dependencies (airplugin_obj air_depend)
set_property (TARGET airplugin_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

if (BUILD_STATIC)
    add_library (AIRplugin STATIC $<TARGET_OBJECTS:airplugin_obj>)
    set_property (TARGET AIRplugin PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    target_link_libraries (AIRplugin ${Python3_LIBRARIES})
    install (TARGETS AIRplugin EXPORT AIRTargets DESTINATION lib)
endif ()

add_custom_target (airplugin_all)

if (BUILD_STATIC)
    set (AIRPLUGIN_LIBS ${AIRPLUGIN_LIBS} PUBLIC AIRplugin)
    add_dependencies (airplugin_all AIRplugin)
endif()

if (AIR_BUILD_TEST)
    build_app_test (AIR_INSTALL_APP "${AIR_LIBS}")
endif()

if (AIR_BUILD_EXAMPLE)
    build_app_example (AIR_INSTALL_APP "${AIR_LIBS}")
endif ()

if (BUILD_UNITTEST)
    build_app_unittest (AIR_INSTALL_APP "${AIR_UTLIBS}" "ut_airplugin")
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/unittest/pybase.py DESTINATION ${CMAKE_BINARY_DIR}/unittest)
endif()