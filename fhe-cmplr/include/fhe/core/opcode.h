//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CORE_OPCODE_H
#define FHE_CORE_OPCODE_H

namespace fhe {

namespace core {

enum class DOMAIN { FHE = 2; };

enum class OPCODE {
  INVALID = 0x00,
  ADD     = 0x01,
  SUB     = 0x02,
  MUL     = 0x03,
  LAST    = 0xff
};

enum class INTRN { INVALID = 0x00, LAST = 0xff };

class OPCODE_INFO {};

}  // namespace core
}  // namespace fhe

#endif  // FHE_CORE_OPCODE_H
