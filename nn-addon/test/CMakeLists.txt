file (GLOB NN_TESTAPP_SRC_FILES CONFIGURE_DEPENDS *.cxx)

set (NN_TESTAPP_INCLUDE_DIRS "")
list (APPEND NN_TESTAPP_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
include_directories (${NN_TESTAPP_INCLUDE_DIRS})
set (NN_TESTAPP_INCLUDE_DIRS "${NN_TESTAPP_INCLUDE_DIRS}" CACHE INTERNAL "")

set (NN_TESTAPP "")
foreach (app ${NN_TESTAPP_SRC_FILES})
    get_filename_component (exe ${app} NAME_WE)
    add_executable (${exe} ${app})
    set_property (TARGET ${exe} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test)
    set (NN_TESTAPP ${NN_TESTAPP} ${exe})
    target_link_libraries (${exe} ${NN_LIBS})
    add_test( NAME ${exe} COMMAND ${CMAKE_BINARY_DIR}/test/${exe})
endforeach ()

if (NN_INSTALL_APP)
    install (TARGETS ${NN_TESTAPP} RUNTIME DESTINATION test)
endif ()

add_test( NAME test_onnx_cmplr COMMAND ${PROJECT_SOURCE_DIR}/test/test_onnx_cmplr.sh "${CMAKE_BINARY_DIR}")
