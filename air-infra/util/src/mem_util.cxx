//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/util/mem_util.h"

namespace air {

namespace util {

// memory consumption stats for mem_pool
MEM_STATS* MEM_POOL_MANAGER::Mem_pool_stats;
// memory consumption stats for air_malloc/air_free
MEM_STATS MEM_POOL_MANAGER::Non_pool_stats;

// Print memory consumption details
void MEM_POOL_MANAGER::Print() {}  // MEM_POOL_MANAGER::Print

}  // namespace util

}  // namespace air
