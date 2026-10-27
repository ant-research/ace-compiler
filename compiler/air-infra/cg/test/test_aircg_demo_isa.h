//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef CG_TEST_TEST_CGIR_DEMO_ISA_H
#define CG_TEST_TEST_CGIR_DEMO_ISA_H

#include <stdint.h>

namespace air {

namespace cg {

namespace demo {

static constexpr uint8_t ISA_ID = 1;
static constexpr uint8_t ABI_ID = 1;

enum REG_CLASS : uint8_t {
  UNKNOWN = 0,  // Unknown register class
  CSR,          // Control and status register
  GPR,          // General purpose register
  FPR,          // Floating point register
  VER,          // Vector extension register
};

enum GPR : uint8_t {
  X0,
  X1,
  X2,
  X3,
  X4,
  X5,
  X6,
  X7,
  X8,
  X9,
  X10,
  X11,
  X12,
  X13,
  X14,
  X15,
  X16,
  X17,
  X18,
  X19,
  X20,
  X21,
  X22,
  X23,
  X24,
  X25,
  X26,
  X27,
  X28,
  X29,
  X30,
  X31,
};

enum FPR : uint8_t {
  F0,
  F1,
  F2,
  F3,
  F4,
  F5,
  F6,
  F7,
  F8,
  F9,
  F10,
  F11,
  F12,
  F13,
  F14,
  F15,
  F16,
  F17,
  F18,
  F19,
  F20,
  F21,
  F22,
  F23,
  F24,
  F25,
  F26,
  F27,
  F28,
  F29,
  F30,
  F31,
};

enum VER : uint8_t {
  V0,
  V1,
  V2,
  V3,
  V4,
  V5,
  V6,
  V7,
  V8,
  V9,
  V10,
  V11,
  V12,
  V13,
  V14,
  V15,
  V16,
  V17,
  V18,
  V19,
  V20,
  V21,
  V22,
  V23,
  V24,
  V25,
  V26,
  V27,
  V28,
  V29,
  V30,
  V31,
};

enum OPCODE : uint16_t {
  OPC_UNDEF,
  OPC_ADD,
  OPC_ADDI,
  OPC_BNEZ,
  OPC_BGE,
  OPC_CALL,
  OPC_LW,
  OPC_LD,
  OPC_J,
  OPC_MV,
  OPC_RET,
  OPC_SW,
  OPC_SD,
  OPC_LI,
  OPC_BLT,
  OPC_MUL,
  OPC_SUBI,
};

enum OPND_FLAG_KIND : uint16_t {
  LO = 0x0001,
  HI = 0x0002,
};

}  // namespace demo

}  // namespace cg

}  // namespace air

#endif  // CG_TEST_TEST_CGIR_DEMO_ISA_H
