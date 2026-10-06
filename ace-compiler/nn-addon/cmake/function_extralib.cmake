#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

# Find the Protobuf library
# https://cmake.org/cmake/help/latest/module/FindProtobuf.html
function (find_extralib_protobuf lib)
  find_package(Protobuf REQUIRED)

  if (Protobuf_FOUND)
    include_directories(${Protobuf_INCLUDE_DIRS})

    if (APPLE)
      find_library(AbslLogMessageLib absl_log_internal_message)
      find_library(AbslLogCheckOpLib absl_log_internal_check_op)
      set (Protobuf_LIBRARIES ${Protobuf_LIBRARIES} ${AbslLogMessageLib} ${AbslLogCheckOpLib})
    else ()
      set (Protobuf_LIBRARIES ${Protobuf_LIBRARIES})
    endif ()
  else ()
    message (FATAL_ERROR "** ERROR ** Protobuf is not installed")
  endif ()

  set (${lib} ${Protobuf_LIBRARIES} PARENT_SCOPE)
endfunction ()

# Find the nn-addon extra library
function (find_nn_extralib lib)
  find_extralib_python()
  find_extralib_protobuf(PROTOBUF_LIB)

  set (${lib} ${PROTOBUF_LIB} ${EXTRA_PYTHON_LIBS} PARENT_SCOPE)
endfunction ()