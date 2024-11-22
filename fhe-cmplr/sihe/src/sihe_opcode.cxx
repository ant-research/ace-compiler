//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/sihe/sihe_opcode.h"

#include "air/base/meta_info.h"

namespace fhe {
namespace sihe {
using namespace air::base;

bool Register_sihe_domain() {
  static const struct OPR_INFO sihe_opr_info[] = {
#define SIHE_OPCODE(NAME, name, cat, kid_num, fld_num, prop) \
  {#name, (cat), kid_num, fld_num, (prop)},
#include "fhe/sihe/opcode_def.inc"
  };

  static const DOMAIN_INFO sihe_domain_info = {
      "SIHE", SIHE_DOMAIN::ID, sizeof(sihe_opr_info) / sizeof(OPR_INFO),
      sihe_opr_info};

  bool reg_res = META_INFO::Register_domain(&sihe_domain_info);
  CMPLR_ASSERT(reg_res, "FAILED in registering domain SIHE");
  return reg_res;
}

}  // namespace sihe
}  // namespace fhe
