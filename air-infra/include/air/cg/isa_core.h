//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_CG_ISA_CORE_H
#define AIR_CG_ISA_CORE_H

#include <stdint.h>

namespace air {

namespace cg {

//! @brief CG CORE ISA ID
static constexpr uint32_t ISA_CORE = 0;

enum OPERATOR : uint32_t {
  INVALID,
  CHI,  //<! res = CHI(opnd), defines a pseudo/dedicated register
  PHI   //<! res = PHI(op0, op1, ...), selects value from operands
};

#define MAKE_OPCODE(isa, oper) ((isa << 16) + oper)

static constexpr uint32_t CORE_CHI = MAKE_OPCODE(ISA_CORE, CHI);
static constexpr uint32_t CORE_PHI = MAKE_OPCODE(ISA_CORE, PHI);

void Register_core_isa();

}  // namespace cg

}  // namespace air

#endif  // AIR_CG_ISA_CORE_H
