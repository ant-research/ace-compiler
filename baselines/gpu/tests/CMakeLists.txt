# Find all test source files
file(GLOB TEST_SOURCES "*.cu")

# Create test executables
foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} PRIVATE 
        phantom
        gtest 
        gtest_main
        ${NTL_LIBRARY} 
        ${GMP_LIBRARY} 
        ${GMPXX_LIBRARY}
    )
    target_include_directories(${TEST_NAME} PRIVATE 
        ${CMAKE_SOURCE_DIR}/include
    )
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()
