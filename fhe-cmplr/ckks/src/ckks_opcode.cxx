//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/ckks/ckks_opcode.h"

#include "air/base/meta_info.h"

namespace fhe {
namespace ckks {
using namespace air::base;

bool Register_ckks_domain() {
  static const struct OPR_INFO ckks_opr_info[] = {
#define CKKS_OPCODE(NAME, name, cat, kid_num, fld_num, prop) \
  {#name, (cat), kid_num, fld_num, (prop)},
#include "fhe/ckks/opcode_def.inc"
  };

  static const DOMAIN_INFO ckks_domain_info = {
      "CKKS", CKKS_DOMAIN::ID, sizeof(ckks_opr_info) / sizeof(OPR_INFO),
      ckks_opr_info};

  bool reg_res = META_INFO::Register_domain(&ckks_domain_info);
  CMPLR_ASSERT(reg_res, "FAILED in registering domain CKKS");
  return reg_res;
}

}  // namespace ckks
}  // namespace fhe
