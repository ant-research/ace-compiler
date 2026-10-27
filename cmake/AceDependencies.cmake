#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

# AceDependencies.cmake — Single source of truth for all dependency URLs/refs.
#
# External (GitHub) URLs are the defaults.  Internal overrides are loaded from
# .aci/config-internal.cmake when present.  Any variable can be overridden on
# the cmake command line:  cmake -DDEP_PHANTOM_REF=v1.3.0 ...

# ---------------------------------------------------------------------------
# Auto-detect internal vs external source
# ---------------------------------------------------------------------------
if(NOT DEFINED ACE_DEP_SOURCE)
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.aci/config-internal.cmake")
    set(ACE_DEP_SOURCE "internal")
  else()
    set(ACE_DEP_SOURCE "external")
  endif()
endif()

# ---------------------------------------------------------------------------
# Compiler backend dependencies (internal repos — need .aci/ or -D override)
# This repo (ace-compiler) is the frontend: Python → AIR IR.
# The three sub-repos below handle backend lowering: AIR → FHE IR → C/CU,
# which is then compiled into shared libraries by this repo.
# ---------------------------------------------------------------------------
set(DEP_AIR_NAME     "air-infra")
set(DEP_NN_NAME      "nn-addon")
set(DEP_FHE_NAME     "fhe-cmplr")

set(DEP_AIR_URL      "" CACHE STRING "air-infra git URL (needs .aci/ or -D override)")
set(DEP_AIR_REF      "master" CACHE STRING "air-infra git ref")
set(DEP_NN_URL       "" CACHE STRING "nn-addon git URL (needs .aci/ or -D override)")
set(DEP_NN_REF       "master" CACHE STRING "nn-addon git ref")
set(DEP_FHE_URL      "" CACHE STRING "fhe-cmplr git URL (needs .aci/ or -D override)")
set(DEP_FHE_REF      "feat/galois-key-lifecycle-clean" CACHE STRING "fhe-cmplr git ref")

# ---------------------------------------------------------------------------
# FHE runtime backends — GPU-accelerated CKKS libraries (public GitHub)
# ---------------------------------------------------------------------------
set(DEP_PHANTOM_NAME  "phantom")
set(DEP_HYPERFHE_NAME "ckks-infra")

set(DEP_PHANTOM_URL   "https://github.com/ant-research/ace-phantom-fhe.git" CACHE STRING "phantom git URL")
set(DEP_PHANTOM_REF   "master" CACHE STRING "phantom git ref")
set(DEP_HYPERFHE_URL  "https://github.com/ant-research/ace-ckks-infra.git"  CACHE STRING "ace-library git URL")
set(DEP_HYPERFHE_REF  "master" CACHE STRING "ace-library git ref")

# ---------------------------------------------------------------------------
# Third-party dependencies
# ---------------------------------------------------------------------------
set(DEP_SPDLOG_URL    "https://github.com/gabime/spdlog.git"  CACHE STRING "spdlog git URL")
set(DEP_SPDLOG_REF    "v1.15.1" CACHE STRING "spdlog git ref")

# ---------------------------------------------------------------------------
# Internal overrides — loaded when ACE_DEP_SOURCE == "internal"
# The override file contains plain set() calls that replace the defaults above.
# ---------------------------------------------------------------------------
if(ACE_DEP_SOURCE STREQUAL "internal")
  include("${CMAKE_CURRENT_SOURCE_DIR}/.aci/config-internal.cmake" OPTIONAL)
endif()
