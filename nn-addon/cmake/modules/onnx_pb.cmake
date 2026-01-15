#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

# Build external onnx project dependent function
function(build_external_proto)

  set(REPO_ONNX_URL      "${PROJECT_SOURCE_DIR}/cmake/modules/onnx")
  message(STATUS "Cloning External Repository   : ${REPO_ONNX_URL}")

  include(FetchContent)
  FetchContent_Declare(
    onnx
    SOURCE_DIR  ${REPO_ONNX_URL}
  )

  FetchContent_GetProperties(onnx)
  if(NOT onnx_POPULATED)
    FetchContent_Populate(onnx)

    # Path to onnx.proto
    set(ONNX_PROTO ${onnx_SOURCE_DIR}/onnx/onnx.proto)

    # Modify onnx.proto
    execute_process(
      COMMAND ${CMAKE_COMMAND} -E echo "Modifying onnx.proto..."
      COMMAND ${CMAKE_COMMAND} -E copy ${ONNX_PROTO} ${ONNX_PROTO}.bak
      COMMAND ${CMAKE_COMMAND} -E sed -i.bak 's/option optimize_for = LITE_RUNTIME;//' ${ONNX_PROTO}
    )
  endif()

  set(ONNX_PATH   ${onnx_SOURCE_DIR}/onnx/)
  set(ONNX_PROTO  ${onnx_SOURCE_DIR}/onnx/onnx.proto)
  set(ONNX_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/onnx)

  execute_process(
    COMMAND mkdir -p ${ONNX_OUTPUT}
    DEPEND  onnx
    COMMAND protoc --proto_path=${ONNX_PATH} --cpp_out=${ONNX_OUTPUT} ${ONNX_PROTO}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    message(STATUS "Generating onnx proto with Protobuf")

  add_library(onnx_objects OBJECT
    ${ONNX_OUTPUT}/onnx.pb.cc
  )

  include_directories (${ONNX_OUTPUT})
endfunction()

if(NOT TARGET onnx_objects)
  build_external_proto()
endif()
