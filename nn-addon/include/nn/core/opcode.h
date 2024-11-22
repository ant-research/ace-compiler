//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_CORE_OPCODE_H
#define NN_CORE_OPCODE_H

#include <cstdint>

#include "air/base/opcode.h"
#include "air/base/opcode_gen.h"

namespace nn {

namespace core {

static constexpr uint32_t NN = 1;

enum OPCODE : uint32_t {
// enum definition from opcode_def.inc
#define DEF_OPCODE(NAME, name, cat, kid_num, fld_num, prop) \
  OPCODE_ENUM_GEN(NAME, name, cat, kid_num, fld_num, prop)
#include "opcode_def.inc"
#undef DEF_OPCODE

  // last pesudo opcode
  LAST = 0xff
};

//! @brief VECTOR' opcodes
#define DEF_OPCODE(NAME, name, cat, kid_num, fld_num, prop) \
  static constexpr air::base::OPCODE OPC_##NAME(NN, OPCODE::NAME);
#include "opcode_def.inc"
#undef DEF_OPCODE

bool Register_nn();

}  // namespace core
}  // namespace nn

#endif  // NN_CORE_OPCODE_H
