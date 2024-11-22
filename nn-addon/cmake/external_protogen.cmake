# https://cmake.org/cmake/help/latest/module/FindProtobuf.html

# check if Protobuf is installed
find_package(Protobuf REQUIRED)

if (Protobuf_FOUND)
  include_directories(${Protobuf_INCLUDE_DIRS})

  set(ONNX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/third-party/onnx/onnx/)
  set(ONNX_PROTO ${CMAKE_CURRENT_SOURCE_DIR}/third-party/onnx/onnx/onnx.proto)
  set(ONNX_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/onnx2air/)

  execute_process(
    COMMAND mkdir -p ${ONNX_OUTPUT}
    COMMAND
      protoc --proto_path=${ONNX_PATH} --cpp_out=${ONNX_OUTPUT} ${ONNX_PROTO}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    message( STATUS "Generating onnx proto with Protobuf")

  if (APPLE)
    find_library(AbslLogMessageLib absl_log_internal_message)
    find_library(AbslLogCheckOpLib absl_log_internal_check_op)
    set (EXTRA_LIBS ${EXTRA_LIBS} ${Protobuf_LIBRARIES} ${AbslLogMessageLib} ${AbslLogCheckOpLib})
  else ()
    set (EXTRA_LIBS ${EXTRA_LIBS} ${Protobuf_LIBRARIES})
  endif ()
else ()
  message (FATAL_ERROR "Protobuf need to be installed to generate the onnx proto")
endif ()