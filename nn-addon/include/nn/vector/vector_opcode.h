//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_OPCODE_H
#define NN_VECTOR_OPCODE_H

#include <cstdint>

#include "air/base/opcode.h"
#include "air/base/opcode_gen.h"

namespace nn {
namespace vector {

static constexpr uint32_t VECTOR = 2;

enum VECTOR_DOMAIN : uint32_t {
  ID = 2,
};

enum VECTOR_OPCODE : uint32_t {
#define DEF_OPCODE(NAME, name, cat, kid_num, fld_num, prop) \
  OPCODE_ENUM_GEN(NAME, name, cat, kid_num, fld_num, prop)
#include "nn/vector/opcode_def.inc"
#undef DEF_OPCODE

  // last pesudo opcode
  LAST = 0xff
};

//! @brief VECTOR' opcodes
#define DEF_OPCODE(NAME, name, cat, kid_num, fld_num, prop)        \
  static constexpr air::base::OPCODE OPC_##NAME(VECTOR_DOMAIN::ID, \
                                                VECTOR_OPCODE::NAME);
#include "nn/vector/opcode_def.inc"
#undef DEF_OPCODE

bool Register_vector_domain();

}  // namespace vector
}  // namespace nn

#endif  // NN_VECTOR_OPCODE_H
