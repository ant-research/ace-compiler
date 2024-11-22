//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_POLY_OPCODE_H
#define FHE_POLY_OPCODE_H

#include <cstdint>

#include "air/base/opcode.h"
#include "air/base/opcode_gen.h"

namespace fhe {

namespace poly {

// @brief Polynomial DOMAIN ID
static constexpr uint32_t POLYNOMIAL_DID = 5;

// @brief Polynomial Operators
enum OPCODE : uint32_t {
// enum definition from opcode_def.inc
#define DEF_OPCODE(NAME, name, cat, kid_num, fld_num, prop) \
  OPCODE_ENUM_GEN(NAME, name, cat, kid_num, fld_num, prop)
#include "opcode_def.inc"
#undef DEF_OPCODE

  LAST_VALID,
  LAST = 0xff
};

//! @brief Polynomial opcodes
#define DEF_OPCODE(NAME, name, cat, kid_num, fld_num, prop) \
  static constexpr air::base::OPCODE OPC_##NAME(POLYNOMIAL_DID, OPCODE::NAME);
#include "opcode_def.inc"
#undef DEF_OPCODE

// @brief Register polynomial domain to META_INFO
bool Register_polynomial();

}  // namespace poly
}  // namespace fhe

#endif  // FHE_POLY_OPCODE_H
