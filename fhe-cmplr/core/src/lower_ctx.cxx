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
static const char* Fhe_attr_name[] = {"scale",         "level",    "mul_depth",
                                      "rescale_level", "extended", "mul_ciph",
                                      "precompute"};

static const char* Func_name_tab[] = {
    "App_relu",       "Rns_add",     "Rns_sub",     "Rns_mul",
    "Rns_rotate",     "Rns_add_ext", "Rns_sub_ext", "Rns_mul_ext",
    "Rns_rotate_ext", "Rotate",      "Relin"};

const char* LOWER_CTX::Attr_name(FHE_ATTR_KIND attr) const {
  AIR_ASSERT(static_cast<uint32_t>(attr) <
             static_cast<uint32_t>(FHE_ATTR_KIND::LAST));
  return Fhe_attr_name[static_cast<uint32_t>(attr)];
}

const char* LOWER_CTX::Func_name(FHE_FUNC func) const {
  AIR_ASSERT(static_cast<uint32_t>(func) <
             static_cast<uint32_t>(FHE_FUNC::FHE_FUNC_END));
  return Func_name_tab[static_cast<uint32_t>(func)];
}

}  // namespace core
}  // namespace fhe