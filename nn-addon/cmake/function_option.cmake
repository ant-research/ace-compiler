#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

# check option : -DNN_WITH_SRC
function(build_nn_with_src name)
  set(AIR_INFRA_PROJECT ${CMAKE_SOURCE_DIR}/../air-infra CACHE STRING "The air-infra project path")

  string(FIND "${name}" "air-infra" CONF_AIR_INFRA)
  if(${CONF_AIR_INFRA} GREATER "-1")
    if(IS_DIRECTORY ${AIR_INFRA_PROJECT})
      add_subdirectory(${AIR_INFRA_PROJECT} air-infra)
    else()
      message(FATAL_ERROR "** ERROR ** ${AIR_INFRA_PROJECT} can't found")
    endif()
  else()
    find_package(ant-ace REQUIRED)
    find_air_lib()
  endif()
endfunction()
