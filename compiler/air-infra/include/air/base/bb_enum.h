//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_BB_ENUM_H
#define AIR_BASE_BB_ENUM_H

#include <cstdint>
namespace air {
namespace base {

//! @brief basic block attributes
//! entry -> preheader -> header -> ... -> back -> tail -> exit
//!                      ^      <LOOP_BODY>    |
//!                      |---------------------|
enum BB_ATTR : uint32_t {
  BB_INVALID_ATTR   = 0,
  BB_FALL_THRU      = 0x0001,
  BB_LOOP_BODY      = 0x0002,
  BB_LOOP_PREHEADER = 0x0004,
  BB_LOOP_HEADER    = 0x0008,
  BB_LOOP_BACK      = 0x0010,
  BB_LOOP_TAIL      = 0x0020,
  BB_GOTO           = 0x0040,
  BB_ENTRY          = 0x0080,
  BB_EXIT           = 0x0100,
};

}  // namespace base
}  // namespace air
#endif