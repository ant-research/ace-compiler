#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

# Test APP correlation function
function(build_app_test condition lib)
    assert("${lib}" "lib can't be empty")

    file(GLOB SRC_FILES CONFIGURE_DEPENDS test/*.cxx)
    set(app "")
    foreach(it ${SRC_FILES})
        get_filename_component(exe ${it} NAME_WE)
        add_executable(${exe} ${it})
        set_property(TARGET ${exe} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test)
        target_link_libraries(${exe} ${lib})

        set(app ${app} ${exe})

        add_test( NAME ${exe} COMMAND ${CMAKE_BINARY_DIR}/test/${exe})
    endforeach()

    if(${condition})
        install(TARGETS ${app} RUNTIME DESTINATION test)
    endif()
endfunction()

# Example APP correlation function
function(build_app_example condition lib)
    assert("${lib}" "lib can't be empty")

    file(GLOB SRC_FILES CONFIGURE_DEPENDS example/*.cxx)
    set(app "")
    foreach(it ${SRC_FILES})
        get_filename_component(exe ${it} NAME_WE)
        add_executable(${exe} ${it})
        set_property(TARGET ${exe} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/example)
        target_link_libraries(${exe} ${lib})

        set(app ${app} ${exe})

        add_test( NAME ${exe} COMMAND ${CMAKE_BINARY_DIR}/example/${exe})
    endforeach()

    if(${condition})
        install(TARGETS ${app} RUNTIME DESTINATION example)
    endif()
endfunction()

# Unittest APP correlation function
function(build_app_unittest condition lib name)
    assert("${lib}" "lib can't be empty")

    file(GLOB SRC_FILES CONFIGURE_DEPENDS unittest/*.cxx)
    add_executable(${name} ${SRC_FILES} ${UNITTESTMAIN})
    set_property(TARGET ${name} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/unittest)
    set_target_properties(${name} PROPERTIES COMPILE_FLAGS "-DNDEBUG")
    target_link_libraries(${name} ${lib})
    target_link_options(${name} PRIVATE -Wl,--export-dynamic)
    target_include_directories(${name} PRIVATE $ENV{UNITTEST_INCLUDE_DIR})

    add_test( NAME ${name} COMMAND ${CMAKE_BINARY_DIR}/unittest/${name})

    if(${condition})
        install(TARGETS ${name} RUNTIME DESTINATION unittest)
    endif()
endfunction()

# Benchmark APP correlation function
function(build_app_bench condition lib)
    assert("${lib}" "lib can't be empty")

    file(GLOB SRC_FILES CONFIGURE_DEPENDS src/*.cxx)
    set(app "")
    foreach(it ${SRC_FILES})
        get_filename_component(exe ${it} NAME_WE)
        add_executable(${exe} ${it})
        set_property(TARGET ${exe} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bench)
        set_target_properties(${exe} PROPERTIES COMPILE_FLAGS
            "${REGEX_FLAG} -DHAVE_STEADY_CLOCK -DNDEBUG ${WARNING_FLAG}")
        target_link_libraries(${exe} ${lib})
        target_include_directories(${exe} PRIVATE $ENV{BENCHMARK_INCLUDE_DIR})

        set(app ${app} ${exe})

        # add_test( NAME ${exe} COMMAND ${CMAKE_BINARY_DIR}/bench/${exe})
    endforeach()

    if(${condition})
        install(TARGETS ${app} RUNTIME DESTINATION bench)
    endif()
endfunction()

# API Document correlation function
# https://vicrucann.github.io/tutorials/quick-cmake-doxygen/
function(build_app_doc condition project)
    assert("${project}" "project name can't be empty")

    # check if Doxygen is installed
    find_package(Doxygen)
     if(DOXYGEN_FOUND)
        # set input and output files
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile)

        # request to configure the file
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        message("Doxygen build started")

        get_filename_component(last_dir ${project} NAME)
        set(target_name "doc_doxygen_${last_dir}")

        # note the option ALL which allows to build the docs together with the application
        add_custom_target( ${target_name} ALL
                    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Generating API documentation with Doxygen"
                    VERBATIM )

    else(DOXYGEN_FOUND)
        message(FATAL_ERROR "** ERROR ** Doxygen need to be installed to generate the doxygen documentation")
    endif(DOXYGEN_FOUND)

    if(${condition})
        install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc/html/  DESTINATION   share/${PACKAGE_NAME}/html/${project})
    endif()
endfunction()