file (GLOB AIR_TESTAPP_SRC_FILES CONFIGURE_DEPENDS *.cxx)

set (AIR_TESTAPP_INCLUDE_DIRS "")
list (APPEND AIR_TESTAPP_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
include_directories (${AIR_TESTAPP_INCLUDE_DIRS})
set (AIR_TESTAPP_INCLUDE_DIRS "${AIR_TESTAPP_INCLUDE_DIRS}" CACHE INTERNAL "")

set (AIR_TESTAPP "")
foreach (app ${AIR_TESTAPP_SRC_FILES})
    get_filename_component (exe ${app} NAME_WE)
    add_executable (${exe} ${app})
    set_property (TARGET ${exe} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test)
    set (AIR_TESTAPP ${AIR_TESTAPP} ${exe})
    target_link_libraries (${exe} ${AIR_LIBS})
    add_test( NAME ${exe} COMMAND ${CMAKE_BINARY_DIR}/test/${exe})
endforeach ()

add_custom_target (air_test)
add_dependencies (air_test ${AIR_TESTAPP})
add_dependencies (allair air_test)

if (AIR_INSTALL_APP)
    install (TARGETS ${AIR_TESTAPP} RUNTIME DESTINATION test)
endif ()