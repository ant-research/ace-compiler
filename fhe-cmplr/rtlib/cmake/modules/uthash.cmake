#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

function(fetch_uthash)

  set(REPO_UTHASH_URL      "${PROJECT_SOURCE_DIR}/cmake/modules/uthash")
  message(STATUS "Cloning External Repository   : ${REPO_UTHASH_URL}")

  include(FetchContent)
  FetchContent_Declare(
      uthash
      SOURCE_DIR ${REPO_UTHASH_URL}
  )
  FetchContent_MakeAvailable(uthash)

  include_directories(${uthash_SOURCE_DIR}/include)

  install(FILES ${uthash_SOURCE_DIR}/include/uthash.h DESTINATION include/rtlib)
endfunction()

if(NOT TARGET uthash)
  fetch_uthash()
endif()



