//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/base/arena.h"

namespace air {
namespace base {

void* ARENA_ITEM_ARRAY::Allocate(size_t bytes, uint32_t* new_id) {
  BYTE_PTR addr = (BYTE_PTR)_allocator->Allocate(bytes, _align);
  *new_id       = Compute_new_id(addr, bytes);
  return addr;
}

uint32_t ARENA_ITEM_ARRAY::Compute_new_id(BYTE_PTR addr, size_t bytes) {
  size_t size = _id_array.size();
  AIR_ASSERT(size < UINT32_MAX);
  _id_array.push_back(addr);
  _sz_array.push_back(bytes);
  return size;
}
}  // namespace base
}  // namespace air
