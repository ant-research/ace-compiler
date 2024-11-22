//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/core/lower_ctx.h"

namespace fhe {
namespace core {
static const char* Fhe_attr_name[] = {"scale", "level", "mul_depth"};

const char* LOWER_CTX::Attr_name(FHE_ATTR_KIND attr) const {
  AIR_ASSERT(static_cast<uint32_t>(attr) <
             static_cast<uint32_t>(FHE_ATTR_KIND::LAST));
  return Fhe_attr_name[static_cast<uint32_t>(attr)];
}

}  // namespace core
}  // namespace fhe