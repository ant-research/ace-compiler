set(USER_MSG_PATH ${CMAKE_CURRENT_SOURCE_DIR}/devtools/err_msg)

add_custom_command (
  OUTPUT ${CMAKE_BINARY_DIR}/include/err_msg.inc.h ${CMAKE_BINARY_DIR}/include/err_msg.inc.c
  WORKING_DIRECTORY ${USER_MSG_PATH}
  COMMAND python3 err_msg.py -i ${CMAKE_BINARY_DIR}/include  -s ${CMAKE_BINARY_DIR}/include
  COMMENT "Generating error message file..."
)

add_custom_target (errmsg DEPENDS ${CMAKE_BINARY_DIR}/include/err_msg.inc.h ${CMAKE_BINARY_DIR}/include/err_msg.inc.c)

include_directories (${CMAKE_BINARY_DIR}/include)

install(DIRECTORY ${CMAKE_BINARY_DIR}/include/ DESTINATION include)