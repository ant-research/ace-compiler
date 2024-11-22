set(USER_MSG_PATH ${CMAKE_CURRENT_SOURCE_DIR}/devtools/err_msg)

execute_process(
  COMMAND mkdir -p ${CMAKE_BINARY_DIR}/include
  COMMAND
    python3 ${USER_MSG_PATH}/err_msg.py -i ${CMAKE_BINARY_DIR}/include  -s ${CMAKE_BINARY_DIR}/include
    RESULT_VARIABLE return_code
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
  WORKING_DIRECTORY ${USER_MSG_PATH})

if(return_code)
  message(STATUS "Output: ${output}")
  message(STATUS "Error: ${error}")
  message(FATAL_ERROR "Command failed: ${return_code}")
endif()

include_directories (${CMAKE_BINARY_DIR}/include)

install(DIRECTORY ${CMAKE_BINARY_DIR}/include/ DESTINATION include)