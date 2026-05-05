#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

# Phantom is internal-only (requires internal repo access)
if(ACE_DEP_SOURCE STREQUAL "internal")
    add_subdirectory(phantom)
endif()
# add_subdirectory(hyperfhe)
# add_subdirectory(seal)
# add_subdirectory(openfhe)
