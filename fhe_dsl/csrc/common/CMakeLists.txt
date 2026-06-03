#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

add_library(ace_common STATIC
    src/logging.cpp
)

target_include_directories(ace_common PUBLIC
    include
    ${Python_INCLUDE_DIRS}
)

target_link_libraries(ace_common PUBLIC
    spdlog::spdlog
)