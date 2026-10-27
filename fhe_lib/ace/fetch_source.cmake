#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

include(FetchContent)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/AceFetchContent.cmake)

# Fetch ace-library (ckks-infra / hyperfhe)
# Remote fetch goes to _deps/; local ace-library/ directory is used if present.
ace_fetch_content(ace_library
  GIT_REPOSITORY ${DEP_HYPERFHE_URL}
  GIT_TAG        ${DEP_HYPERFHE_REF}
  LOCAL_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/ace-library
  POPULATE_ONLY
)

set(ACE_LIBRARY_SOURCE_DIR ${ace_library_SOURCE_DIR})
set(ACE_LIBRARY_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/ace_library_build)

# ace-library is added as a subdirectory (not an isolated ExternalProject), so
# its top-level option() defaults leak into this build. Its he-modules/example
# sub-project references a `k_way_sorting` directory that is absent from the
# fetched source (upstream gap) and is not needed by fhe_lib/ace — disable the
# example/bench/testing sub-projects before add_subdirectory, mirroring the
# rtlib path (rtlib/cmake/modules/ace_library.cmake passes the same OFF set).
set(ACE_LIBRARY_BUILD_EXAMPLES OFF CACHE BOOL "Build examples" FORCE)
set(ACE_LIBRARY_BUILD_BENCH    OFF CACHE BOOL "Build benchmarks" FORCE)
set(ACE_LIBRARY_BUILD_TESTING  OFF CACHE BOOL "Build tests" FORCE)
set(ACE_LIBRARY_USE_OPENCC     CI_TOKEN  CACHE STRING "Third-party source auth mode: OFF, SSH, CI_TOKEN" FORCE)
set(BUILD_EXAMPLES             OFF CACHE BOOL "" FORCE)
set(BUILD_BENCH                OFF CACHE BOOL "" FORCE)
set(BUILD_TEST                 OFF CACHE BOOL "" FORCE)

# Build ace-library as an isolated external project (produces static libs).
# Assign install component so ace-library's install rules are grouped under
# the same component as the parent project (used by cmake --install --component).
set(ACE_LIBRARY_INSTALL "library")
add_subdirectory(${ACE_LIBRARY_SOURCE_DIR} ${ACE_LIBRARY_BINARY_DIR})
