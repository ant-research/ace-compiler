//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_SIHE_OPCODE_H
#define FHE_SIHE_OPCODE_H

#include "air/base/opcode.h"

namespace fhe {

namespace sihe {

enum SIHE_DOMAIN : uint32_t {
  ID = 3,
};

enum SIHE_OPERATOR : uint32_t {
#define SIHE_OPCODE(NAME, ...) NAME,
#include "fhe/sihe/opcode_def.inc"
  LAST_OPR,
};
#undef SIHE_OPCODE

#define SIHE_OPCODE(NAME, ...)                                   \
  static constexpr air::base::OPCODE OPC_##NAME(SIHE_DOMAIN::ID, \
                                                SIHE_OPERATOR::NAME);
#include "opcode_def.inc"
#undef SIHE_OPCODE

bool Register_sihe_domain();

}  // namespace sihe
}  // namespace fhe

#endif  // FHE_CORE_OPCODE_H
