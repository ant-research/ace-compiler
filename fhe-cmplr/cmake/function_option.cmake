
#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

# Check option : -DFHE_WITH_SRC
function(build_fhe_with_src name)
  # message(STATUS "build_fhe_with_src : -DFHE_WITH_SRC = ${name}")

  string(FIND "${name}" "air-infra" CONF_AIR_INFRA)
  if(${CONF_AIR_INFRA} GREATER "-1")
    set(NN_WITH_SRC "air-infra" CACHE STRING "Add source code compilation dependencies")
  endif()

  string(FIND "${name}" "nn-addon" CONF_NN_ADDON)
  if(${CONF_NN_ADDON} GREATER "-1")
    set(NN_ADDON_PROJECT ${CMAKE_SOURCE_DIR}/../nn-addon)
    if(IS_DIRECTORY ${NN_ADDON_PROJECT})
      add_subdirectory(${NN_ADDON_PROJECT} nn-addon)
    else()
        message(FATAL_ERROR "** ERROR ** ${NN_ADDON_PROJECT} can't found")
    endif()
  else()
    find_nn_lib()
  endif()
endfunction()
