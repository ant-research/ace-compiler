#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

# Set the list of libraries to build component air-infra
function(set_air_lib)
  set(AIR_LIBS ${AIR_LIBS} AIRplugin AIRdriver AIRopt AIRcg AIRcore AIRbase AIRutil)
  set(AIR_UTLIBS ${AIR_LIBS} ${UT_LIBS} PARENT_SCOPE)
  set(AIR_BMLIBS ${AIR_LIBS} ${BM_LIBS} PARENT_SCOPE)

  set(AIR_LIBS ${AIR_LIBS} CACHE STRING "Global common libs of air-infra")
endfunction()

# Found libraries to import component air-infra
function(find_air_lib)

  find_library(AIRUTIL NAMES AIRutil)
  find_library(AIRBASE NAMES AIRbase)
  find_library(AIRCORE NAMES AIRcore)
  find_library(AIRCG NAMES AIRcg)
  find_library(AIROPT NAMES AIRopt)
  find_library(AIRDRIVER NAMES AIRdriver)
  if(AIRUTIL AND AIRBASE AND AIRCORE AND AIRCG AND AIROPT AND AIRDRIVER)
    set(AIR_LIBS ${AIR_LIBS} ${AIRDRIVER} ${AIROPT} ${AIRCG} ${AIRCORE} ${AIRBASE} ${AIRUTIL})
    set(AIR_LIBS ${AIR_LIBS} CACHE STRING "Global common libs of air-infra")
  else()
    message(FATAL_ERROR "** ERROR ** component air-infra is not installed")
  endif()

  if(BUILD_UNITTEST)
    find_library(LIBRARY_GTEST NAMES gtest)
    if(LIBRARY_GTEST)
      set(UT_LIBS "gtest;pthread" CACHE STRING "Global common libs of unittest")
    else()
      message(FATAL_ERROR "** ERROR ** library gtest is not installed")
    endif()
  endif()

  if(BUILD_BENCH)
    find_library(LIBRARY_BENCHMARK NAMES benchmark)
    if(LIBRARY_BENCHMARK)
      set(BM_LIBS "benchmark;pthread" CACHE STRING "Global common libs of benchmark")
    else()
      message(FATAL_ERROR "** ERROR ** library benchmark is not installed")
    endif()
  endif()
endfunction()
