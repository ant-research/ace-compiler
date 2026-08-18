find_package (Python3 COMPONENTS Interpreter)

set (IRGEN_PACKAGE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "")

set (TEST_DIR_LIST "")
list (APPEND TEST_DIR_LIST ${IRGEN_PACKAGE_DIR}/od/test)

add_custom_target (InstallIRGenPackage
    COMMAND ${Python3_EXECUTABLE} -m pip install --user -e ${IRGEN_PACKAGE_DIR}
    WORKING_DIRECTORY ${IRGEN_PACKAGE_DIR}
)

set (COMMANDS_LIST "")

foreach (TEST_DIR IN LISTS TEST_DIR_LIST)
    list (APPEND COMMANDS_LIST ${Python3_EXECUTABLE} -m unittest discover -s ${TEST_DIR})
endforeach ()

add_custom_target(IRGenTestTarget
    COMMAND ${COMMANDS_LIST}
    WORKING_DIRECTORY ${IRGEN_PACKAGE_DIR}
    DEPENDS InstallIRGenPackage
)

add_test(NAME IRGenTest
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target IRGenTestTarget
)

function (add_domain_description OD_PATH OUTPUT_DIR STAMP)
    set(PACKAGE_NAME "od")
    execute_process(
        COMMAND ${Python3_EXECUTABLE} -c "import ${PACKAGE_NAME}"
        RESULT_VARIABLE PACKAGE_RESULT
        OUTPUT_VARIABLE PACKAGE_OUTPUT
        ERROR_VARIABLE PACKAGE_ERROR
    )

    if (NOT ${PACKAGE_RESULT} EQUAL 0)
        execute_process (
            COMMAND ${Python3_EXECUTABLE} -m pip install --user -e ${IRGEN_PACKAGE_DIR}
        )
    endif ()

    file (GLOB_RECURSE OD_FILES "${OD_PATH}/*.od")

    set (COMMANDS_LIST "")

    foreach (OD_FILE IN LISTS OD_FILES)
        list (APPEND COMMANDS_LIST
            ${Python3_EXECUTABLE} ${OD_FILE} "-o" "${OUTPUT_DIR}")
    endforeach ()

    execute_process (COMMAND ${COMMANDS_LIST})
endfunction ()

install (DIRECTORY ${PROJECT_SOURCE_DIR}/irgen/ DESTINATION irgen)

include (CMakePackageConfigHelpers)

configure_package_config_file (
    "${PROJECT_SOURCE_DIR}/irgen/IRGenPackageConfig.cmake.in"
    "${CMAKE_BINARY_DIR}/IRGenPackageConfig.cmake"
    INSTALL_DESTINATION lib/cmake/IRGenPackage
)

install (FILES
    "${CMAKE_BINARY_DIR}/IRGenPackageConfig.cmake"
    DESTINATION lib/cmake/IRGenPackageConfig
)