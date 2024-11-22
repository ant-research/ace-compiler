//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_CORE_OPCODE_H
#define AIR_CORE_OPCODE_H

#include <cstdint>

#include "air/base/opcode.h"
#include "air/base/opcode_gen.h"

namespace air {

namespace core {

//! @brief Core's DOMAIN ID, always be 0
static constexpr uint32_t CORE = 0;

//! @brief Core's operators
enum OPCODE : uint32_t {
// enum definition from opcode_def.inc
#define DEF_OPCODE(NAME, name, cat, kid_num, fld_num, prop) \
  OPCODE_ENUM_GEN(NAME, name, cat, kid_num, fld_num, prop)
#include "opcode_def.inc"
#undef DEF_OPCODE

  // last pesudo opcode
  LAST = 0xff
};

//! @brief Core' opcodes
#define DEF_OPCODE(NAME, name, cat, kid_num, fld_num, prop) \
  static constexpr air::base::OPCODE OPC_##NAME(CORE, OPCODE::NAME);
#include "opcode_def.inc"
#undef DEF_OPCODE

//! @brief Register core domain to META_INFO
bool Register_core();

}  // namespace core
}  // namespace air

#endif
