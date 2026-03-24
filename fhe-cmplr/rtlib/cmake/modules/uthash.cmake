#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

function(fetch_uthash)

  set(UTHASH_URL      "https://git:$ENV{CI_TOKEN}@code.alipay.com/fhe-cmplr/uthash.git")
  set(UTHASH_URL_SSH  "git@code.alipay.com:fhe-cmplr/uthash.git")
  if(EXTERNAL_URL_SSH)
    set(REPO_UTHASH_URL ${UTHASH_URL_SSH})
  else()
    set(REPO_UTHASH_URL ${UTHASH_URL})
  endif()

  message(STATUS "Cloning External Repository   : ${REPO_UTHASH_URL}")

  include(FetchContent)
  FetchContent_Declare(
      uthash
      GIT_REPOSITORY ${REPO_UTHASH_URL}
      GIT_TAG master
  )
  FetchContent_MakeAvailable(uthash)

  include_directories(${uthash_SOURCE_DIR}/include)

  install(FILES ${uthash_SOURCE_DIR}/include/uthash.h DESTINATION include/rtlib)
endfunction()

if(NOT TARGET uthash)
  fetch_uthash()
endif()



