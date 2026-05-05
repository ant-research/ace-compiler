#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

# --- Dependency source switch ---
# Auto-detected: .aci/ exists -> internal, otherwise -> external
set(ACE_DEP_SOURCE "" CACHE STRING
    "Dependency source: 'internal' or 'external'. Auto-detected if empty.")
if(NOT ACE_DEP_SOURCE)
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.aci/config-internal.cmake")
    set(ACE_DEP_SOURCE "internal")
  else()
    set(ACE_DEP_SOURCE "external")
  endif()
endif()

# --- FHE library dependencies ---
set(DEP_PHANTOM_NAME  "phantom")
set(DEP_HYPERFHE_NAME "ckks-infra")

# URL need confirm
# External (GitHub) URLs -- default
set(DEP_PHANTOM_URL   "https://github.com/ant-research/ace-phantom-fhe.git")
set(DEP_PHANTOM_REF   "master")
set(DEP_HYPERFHE_URL  "https://github.com/ant-research/ace-ckks-infra.git")
set(DEP_HYPERFHE_REF  "master")

# Internal overrides (file only exists in internal repo)
if(ACE_DEP_SOURCE STREQUAL "internal")
  include("${CMAKE_CURRENT_SOURCE_DIR}/.aci/config-internal.cmake")
endif()

# Build shared libraries for Python extension support
option(BUILD_SHARED "Build shared libraries for rtlib" ON)