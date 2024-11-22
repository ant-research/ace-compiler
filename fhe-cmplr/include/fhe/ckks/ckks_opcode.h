//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_OPCODE_H
#define FHE_CKKS_OPCODE_H

#include "air/base/opcode.h"

namespace fhe {

namespace ckks {

enum CKKS_DOMAIN : uint32_t {
  ID = 4,
};

enum CKKS_OPERATOR : uint32_t {
#define CKKS_OPCODE(NAME, ...) NAME,
#include "fhe/ckks/opcode_def.inc"
  LAST_OPR,
};
#undef CKKS_OPCODE

#define CKKS_OPCODE(NAME, ...)                                   \
  static constexpr air::base::OPCODE OPC_##NAME(CKKS_DOMAIN::ID, \
                                                CKKS_OPERATOR::NAME);
#include "opcode_def.inc"
#undef CKKS_OPCODE

bool Register_ckks_domain();
}  // namespace ckks
}  // namespace fhe
#endif  // FHE_CKKS_OPCODE_H
