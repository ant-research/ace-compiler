#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

include(FetchContent)

# Check if ace-library source exists locally
set(ACE_LIBRARY_LOCAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ace-library)

if(EXISTS ${ACE_LIBRARY_LOCAL_DIR})
  # Use local ace-library source
  set(ACE_LIBRARY_SOURCE_DIR ${ACE_LIBRARY_LOCAL_DIR})
  message(STATUS "Using local ace-library: ${ACE_LIBRARY_SOURCE_DIR}")
else()
  # Fetch ace-library from remote
  FetchContent_Declare(
    ace_library
    GIT_REPOSITORY  ${DEP_HYPERFHE_URL}
    GIT_TAG         ${DEP_HYPERFHE_REF}
  )

  FetchContent_GetProperties(ace_library)
  if(NOT ace_library_POPULATED)
    FetchContent_Populate(ace_library)
  endif()

  set(ACE_LIBRARY_SOURCE_DIR ${ace_library_SOURCE_DIR})
  message(STATUS "Fetched ace-library from: ${ACE_LIBRARY_SOURCE_DIR}")
endif()

set(ACE_LIBRARY_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/ace_library_build)

# Build ace-library as an isolated external project (produces static libs)
# Assign install component so ace-library's install rules are grouped under
# the same component as the parent project (used by cmake --install --component).
set(ACE_LIBRARY_INSTALL "library")
add_subdirectory(${ACE_LIBRARY_SOURCE_DIR} ${ACE_LIBRARY_BINARY_DIR})