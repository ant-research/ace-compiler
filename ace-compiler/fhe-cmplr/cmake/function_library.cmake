#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

# Set the list of libraries to build component fhe-cmplr
function(set_fhe_lib)
  set(FHE_LIBS ${FHE_LIBS} FHEdriver FHEpoly FHEckks FHEsihe FHEcg FHEopt FHEcore FHEutil)
  set(FHE_LIBS ${FHE_LIBS} ${NN_LIBS})
  set(FHE_UTLIBS ${FHE_LIBS} ${UT_LIBS} PARENT_SCOPE)
  set(FHE_BMLIBS ${FHE_LIBS} ${BM_LIBS} PARENT_SCOPE)

  set(FHE_LIBS ${FHE_LIBS} CACHE STRING "Global libs of fhe-cmplr")
endfunction()

# Found libraries to import component fhe-cmplr
function(find_fhe_lib)
  find_nn_lib()

  find_library(FHEUTIL NAMES FHEutil)
  find_library(FHECORE NAMES FHEcore)
  find_library(FHECG NAMES FHEcg)
  find_library(FHESIHE NAMES FHEsihe)
  find_library(FHECKKS NAMES FHEckks)
  find_library(FHEPOLY NAMES FHEpoly)
  find_library(FHEDRIVER NAMES FHEdriver)

  if(FHEUTIL AND FHECORE AND FHECG AND FHESIHE AND FHECKKS AND FHEPOLY AND FHEDRIVER)
    set(FHE_LIBS ${FHE_LIBS} ${FHEDRIVER} ${FHEPOLY} ${FHECKKS} ${FHESIHE} ${FHECG} ${FHECORE} ${FHEUTIL})
    set(FHE_LIBS ${FHE_LIBS} ${NN_LIBS})
    set(FHE_LIBS ${FHE_LIBS} CACHE STRING "Global common libs of fhe-cmplr")
  else()
    message(FATAL_ERROR "** ERROR ** nn-addon is not installed")
  endif()

  if(BUILD_UNITTEST)
    set(FHE_UTLIBS ${FHE_LIBS} ${UT_LIBS} PARENT_SCOPE)
  endif()

  if(BUILD_BENCH)
    set(FHE_BMLIBS ${FHE_LIBS} ${BM_LIBS} PARENT_SCOPE)
  endif()
endfunction()
