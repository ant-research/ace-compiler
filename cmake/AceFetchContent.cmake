#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

# AceFetchContent.cmake — Unified FetchContent wrapper with local-or-remote
# source detection.
#
# ace_fetch_content(<name>
#   GIT_REPOSITORY <url>       -- Remote git URL
#   GIT_TAG        <ref>       -- Git ref (branch / tag / commit)
#   [LOCAL_DIR     <path>]     -- Local source hint: if CMakeLists.txt exists
#                                 here, use it instead of cloning
#   [SOURCE_DIR    <path>]     -- Force FetchContent SOURCE_DIR (for in-tree
#                                 source when cloning)
#   [POPULATE_ONLY]            -- Call FetchContent_Populate instead of
#                                 FetchContent_MakeAvailable
#   [DECLARE_ONLY]             -- Only declare, do not populate or make
#                                 available.  Caller must call
#                                 FetchContent_MakeAvailable() later (after
#                                 setting options that the dependency reads).
# )
#
# Behaviour:
#   1. If LOCAL_DIR is given AND <LOCAL_DIR>/CMakeLists.txt exists, declares
#      the content with SOURCE_DIR only (offline, no git clone).
#   2. Otherwise, declares with GIT_REPOSITORY + GIT_TAG (optionally SOURCE_DIR).
#      If GIT_REPOSITORY is empty, issues a FATAL_ERROR with guidance.
#   3. Depending on flags:
#      - DECLARE_ONLY:  just declare, stop here
#      - POPULATE_ONLY: FetchContent_Populate (no add_subdirectory)
#      - default:       FetchContent_MakeAvailable (populate + add_subdirectory)
#
macro(ace_fetch_content DEP_NAME)
  cmake_parse_arguments(_AFC "POPULATE_ONLY;DECLARE_ONLY" "GIT_REPOSITORY;GIT_TAG;LOCAL_DIR;SOURCE_DIR" "" ${ARGN})

  if(_AFC_LOCAL_DIR AND EXISTS "${_AFC_LOCAL_DIR}/CMakeLists.txt")
    message(STATUS "ace_fetch_content: ${DEP_NAME} -- using local source: ${_AFC_LOCAL_DIR}")
    FetchContent_Declare(${DEP_NAME} SOURCE_DIR "${_AFC_LOCAL_DIR}")
  else()
    if(NOT _AFC_GIT_REPOSITORY)
      message(FATAL_ERROR
        "ace_fetch_content: ${DEP_NAME} -- no local source at '${_AFC_LOCAL_DIR}' "
        "and GIT_REPOSITORY is empty.\n"
        "  Provide .aci/config-internal.cmake or pass "
        "-DDEP_${DEP_NAME}_URL=<url> on the cmake command line."
      )
    endif()
    if(_AFC_SOURCE_DIR)
      FetchContent_Declare(${DEP_NAME}
        GIT_REPOSITORY "${_AFC_GIT_REPOSITORY}"
        GIT_TAG        "${_AFC_GIT_TAG}"
        SOURCE_DIR     "${_AFC_SOURCE_DIR}"
      )
    else()
      FetchContent_Declare(${DEP_NAME}
        GIT_REPOSITORY "${_AFC_GIT_REPOSITORY}"
        GIT_TAG        "${_AFC_GIT_TAG}"
      )
    endif()
  endif()

  if(NOT _AFC_DECLARE_ONLY)
    if(_AFC_POPULATE_ONLY)
      # FetchContent_Populate(<name>) is deprecated since CMake 3.30 (CMP0169),
      # but we need populate-only semantics (no add_subdirectory) for source
      # dependencies that are built inline by a parent project (e.g. air-infra
      # and nn-addon are built by fhe-cmplr via FHE_WITH_SRC).
      cmake_policy(PUSH)
      cmake_policy(SET CMP0169 OLD)
      FetchContent_Populate(${DEP_NAME})
      cmake_policy(POP)
    else()
      FetchContent_MakeAvailable(${DEP_NAME})
    endif()
  endif()
endmacro()
