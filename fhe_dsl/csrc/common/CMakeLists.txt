#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

add_library(ace_common STATIC
    src/logging.cpp
)

target_include_directories(ace_common
    PUBLIC
      include
    PRIVATE
      ${Python_INCLUDE_DIRS}
)

# ace_compile_settings is PUBLIC so all consumers (runtime, frontend) inherit ABI
# ace_logging provides spdlog::spdlog transitively (PUBLIC link).
# python_sink.h includes <spdlog/spdlog.h> — satisfied by ace_logging.
target_link_libraries(ace_common
    PUBLIC
      ace_compile_settings
      ace_logging
)