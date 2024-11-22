file (GLOB FHE_TESTAPP_SRC_FILES CONFIGURE_DEPENDS *.cxx)

set (FHE_TESTAPP_INCLUDE_DIRS "")
list (APPEND FHE_TESTAPP_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
include_directories (${FHE_TESTAPP_INCLUDE_DIRS})
set (FHE_TESTAPP_INCLUDE_DIRS "${FHE_TESTAPP_INCLUDE_DIRS}" CACHE INTERNAL "")

set (FHE_TESTAPP "")
foreach (app ${FHE_TESTAPP_SRC_FILES})
    get_filename_component (exe ${app} NAME_WE)
    add_executable (${exe} ${app})
    set_property (TARGET ${exe} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test)
    set (FHE_TESTAPP ${FHE_TESTAPP} ${exe})
    target_link_libraries (${exe} ${FHE_LIBS})
    add_test( NAME ${exe} COMMAND ${CMAKE_BINARY_DIR}/test/${exe})
endforeach ()

add_custom_target (fhe_test)
add_dependencies (fhe_test ${FHE_TESTAPP})
add_dependencies (allfhe fhe_test)

if (FHE_INSTALL_APP)
    install (TARGETS ${FHE_TESTAPP} RUNTIME DESTINATION test)
endif ()

add_test( NAME test_fhe_cmplr COMMAND ${PROJECT_SOURCE_DIR}/test/test_fhe_cmplr.sh "${CMAKE_BINARY_DIR}")