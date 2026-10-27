#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

# Shared I/O layer (GDS, CUDA utilities) — used by GPU backends
add_subdirectory(common)

# GPU library backends (require CUDA)
if(ACE_CUDA_AVAILABLE)
    if(WITH_PHANTOM)
        add_subdirectory(phantom)
    endif()

    if(WITH_ACELIB)
        add_subdirectory(ace)
    endif()
else()
    if(WITH_PHANTOM OR WITH_ACELIB)
        message(WARNING "WITH_PHANTOM/WITH_ACELIB ignored — no CUDA available")
    endif()
endif()

# CPU library backends (no CUDA required) — uncomment when fhe_lib/ directories are ready
# if(WITH_SEAL)
#     add_subdirectory(seal)
# endif()
# if(WITH_OPENFHE)
#     add_subdirectory(openfhe)
# endif()

# ---------------------------------------------------------------------------
# Unit tests (controlled by top-level BUILD_UNITTEST option)
# ---------------------------------------------------------------------------
if(BUILD_UNITTEST)
    add_subdirectory(unittest)
endif()
