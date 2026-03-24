#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

# Set the list of libraries to build component air-infra
function(set_nn_lib)
  set(NN_LIBS ${NN_LIBS} NNllama NNdriver NNvector NNonnx2air NNopt NNonnx NNcore NNutil)
  set(NN_LIBS   ${NN_LIBS} ${AIR_LIBS})
  set(NN_UTLIBS ${NN_LIBS} ${UT_LIBS} PARENT_SCOPE)
  set(NN_BMLIBS ${NN_LIBS} ${BM_LIBS} PARENT_SCOPE)

  set(NN_LIBS ${NN_LIBS} CACHE STRING "Global common libs of nn-addon")
endfunction()

# Found libraries to import component nn-addon
function(find_nn_lib)
  find_air_lib()

  find_library(NNUTIL NAMES NNutil)
  find_library(NNCORE NAMES NNcore)
  find_library(NNONNX NAMES NNonnx)
  find_library(NNOPT NAMES NNopt)
  find_library(NNONNX2AIR NAMES NNonnx2air)
  find_library(NNVECTOR NAMES NNvector)
  find_library(NNDRIVER NAMES NNdriver)

  if(NNUTIL AND NNCORE AND NNONNX AND NNOPT AND NNONNX2AIR AND NNVECTOR AND NNDRIVER)
    set(NN_LIBS ${NN_LIBS} ${NNDRIVER} ${NNVECTOR} ${NNONNX2AIR} ${NNOPT} ${NNONNX} ${NNCORE} ${NNUTIL})
    set(NN_LIBS ${NN_LIBS} ${AIR_LIBS})
    set(NN_LIBS ${NN_LIBS} CACHE STRING "Global common libs of nn-addon")
  else()
    message(FATAL_ERROR "** ERROR ** nn-addon is not installed")
  endif()

  if(BUILD_UNITTEST)
    set(NN_UTLIBS ${NN_LIBS} ${UT_LIBS} PARENT_SCOPE)
  endif()

  if(BUILD_BENCH)
    set(NN_BMLIBS ${NN_LIBS} ${BM_LIBS} PARENT_SCOPE)
  endif()
endfunction()