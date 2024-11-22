//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/core/opcode.h"

#include "air/base/meta_info.h"

using namespace air::base;

namespace air {
namespace core {

// Operator info for all operators in core
static const OPR_INFO Opr_info[] = {
#define DEF_OPCODE(NAME, name, cat, kid_num, fld_num, prop) \
  OPCODE_META_GEN(NAME, name, cat, kid_num, fld_num, prop)
#include "air/core/opcode_def.inc"
#undef DEF_OPCODE
};

// Domain info for call
static const DOMAIN_INFO Domain_info = {
    "CORE", CORE, sizeof(Opr_info) / sizeof(Opr_info[0]), Opr_info};

// Register domain 'core' to meta_info registry
bool Register_core() { return META_INFO::Register_domain(&Domain_info); }

}  // namespace core
}  // namespace air
