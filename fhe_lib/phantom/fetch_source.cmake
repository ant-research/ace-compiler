#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

include(FetchContent)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/AceFetchContent.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/AceApplyPatches.cmake)

# Backward compat: CUSTOM_FETCHCONTENT_DIR is deprecated.
# Prefer placing source in fhe_lib/phantom/phantom/ or using -DDEP_PHANTOM_URL.
if(CUSTOM_FETCHCONTENT_DIR)
  message(DEPRECATION "CUSTOM_FETCHCONTENT_DIR is deprecated. "
    "Place source in ${CMAKE_CURRENT_SOURCE_DIR}/${DEP_PHANTOM_NAME}/ "
    "or use -DDEP_PHANTOM_URL=<path>")
  set(FETCHCONTENT_SOURCE_DIR_CKKS_INFRA ${CUSTOM_FETCHCONTENT_DIR})
endif()

# Fetch phantom FHE library
# 1. If fhe_lib/phantom/phantom/ exists (source vendored for open source, or a
#    previous fetch), it is used directly — no network access.
# 2. Otherwise, the remote (internal alipay, or public GitHub) is cloned into
#    fhe_lib/phantom/phantom/ itself, so offline/open-source builds pick the
#    vendored source next time.
ace_fetch_content(phantom
  GIT_REPOSITORY ${DEP_PHANTOM_URL}
  GIT_TAG        ${DEP_PHANTOM_REF}
  LOCAL_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/${DEP_PHANTOM_NAME}
  SOURCE_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/${DEP_PHANTOM_NAME}
)

# Apply patches from the patches/ directory (if any)
ace_apply_patches("${CMAKE_CURRENT_SOURCE_DIR}/patches" "${phantom_SOURCE_DIR}")

# Install phantom dependency headers
install(
    DIRECTORY ${phantom_SOURCE_DIR}/include/
    DESTINATION ace/include/lib_phantom
    COMPONENT library
    FILES_MATCHING PATTERN "*.h" PATTERN "*.cuh"
)
