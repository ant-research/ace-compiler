//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "nn/vector/vector_opcode.h"

#include "air/base/meta_info.h"

namespace nn {
namespace vector {
using namespace air::base;

bool Register_vector_domain() {
  static const struct OPR_INFO vector_opr_info[] = {
#define DEF_OPCODE(NAME, name, cat, kid_num, fld_num, prop) \
  OPCODE_META_GEN(NAME, name, cat, kid_num, fld_num, prop)
#include "nn/vector/opcode_def.inc"
#undef DEF_OPCODE
  };

  static const DOMAIN_INFO vector_domain_info = {
      "VECTOR", VECTOR_DOMAIN::ID, sizeof(vector_opr_info) / sizeof(OPR_INFO),
      vector_opr_info};

  bool reg_res = META_INFO::Register_domain(&vector_domain_info);
  CMPLR_ASSERT(reg_res, "FAILED in registering domain VECTOR");
  return reg_res;
}
}  // namespace vector
}  // namespace nn
