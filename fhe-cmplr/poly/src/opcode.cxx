//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/poly/opcode.h"

#include "air/base/meta_info.h"

using namespace air::base;

namespace fhe {
namespace poly {

// Operator info for all operators in polynomial
static const OPR_INFO Opr_info[] = {
#define DEF_OPCODE(NAME, name, cat, kid_num, fld_num, prop) \
  OPCODE_META_GEN(NAME, name, cat, kid_num, fld_num, prop)
#include "fhe/poly/opcode_def.inc"
#undef DEF_OPCODE
};

// Domain info for polynomial
static const DOMAIN_INFO Poly_domain_info = {
    "POLY", POLYNOMIAL_DID, sizeof(Opr_info) / sizeof(Opr_info[0]), Opr_info};

// Register domain 'poly' to meta_info registry
bool Register_polynomial() {
  return META_INFO::Register_domain(&Poly_domain_info);
}

}  // namespace poly

}  // namespace fhe
