//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "nn/core/opcode.h"

#include "air/base/meta_info.h"

using namespace air::base;

namespace nn {
namespace core {

// Operator info for all operators in core
static const air::base::OPR_INFO Opr_info[] = {
#define DEF_OPCODE(NAME, name, cat, kid_num, fld_num, prop) \
  OPCODE_META_GEN(NAME, name, cat, kid_num, fld_num, prop)
#include "nn/core/opcode_def.inc"
#undef DEF_OPCODE
};

// Domain info for call
static const air::base::DOMAIN_INFO Domain_info = {
    "NN", nn::core::NN, sizeof(Opr_info) / sizeof(Opr_info[0]), Opr_info};

// Register domain 'nn' to meta_info registry
bool Register_nn() {
  return air::base::META_INFO::Register_domain(&Domain_info);
}

}  // namespace core
}  // namespace nn
