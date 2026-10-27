#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

# ace_logging — pure-C++ logging library over spdlog.
#
# Provides a default logger with stderr_color sink for use in fhe_lib
# (no Python dependency).  The csrc/common/ layer replaces the default
# sink with a PythonSink when loaded from a pybind11 extension module.

include(FetchContent)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY ${DEP_SPDLOG_URL}
    GIT_TAG        ${DEP_SPDLOG_REF}
)
set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(spdlog)

add_library(ace_logging STATIC
    src/logging.cpp
)

# Header lives at fhe_lib/include/lib/util/logging.h — add fhe_lib/include
# so that #include "util/logging.h" resolves correctly.
target_include_directories(ace_logging
    PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

# PUBLIC link: consumers get spdlog headers and compile definitions.
# This matches the pattern used by csrc/common/ace_common.
target_link_libraries(ace_logging
    PUBLIC
      spdlog::spdlog
)
