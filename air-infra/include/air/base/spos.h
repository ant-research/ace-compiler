//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_SPOS_H
#define AIR_BASE_SPOS_H

#include <stddef.h>

#include "air/util/debug.h"

namespace air {
namespace base {

//! 64 bits source position data structure
class SPOS {
public:
  SPOS()
      : _file(0), _line(0), _col(0), _count(0), _stmt_begin(0), _bb_begin(0) {}
  SPOS(uint32_t f, uint32_t line, uint32_t col, uint32_t count) {
    Set_file(f);
    Set_line(line);
    Set_col(col);
    Set_count(count);
    _stmt_begin = 0;
    _bb_begin   = 0;
  }

  uint32_t File() const { return _file; }
  uint32_t Line() const { return _line; }
  uint32_t Col() const { return _col; }
  uint32_t Count() const { return _count; }

  bool Is_stmt_beg() const { return _stmt_begin; }
  bool Is_bb_beg() const { return _bb_begin; }

  void Set_file(uint32_t f) {
    AIR_ASSERT(f <= UINT16_MAX);
    _file = f;
  }
  void Set_line(uint32_t l) {
    AIR_ASSERT(l <= 0xFFFFFFU);
    _line = l;
  }
  void Set_col(uint32_t c) {
    AIR_ASSERT(c <= 0xFFFU);
    _col = c;
  }
  void Set_count(uint32_t c) {
    AIR_ASSERT(c <= 0x3FFU);
    _count = c;
  }
  void Set_stmt_beg(bool s) { _stmt_begin = s; }
  void Set_bb_beg(bool b) { _bb_begin = b; }

private:
  uint32_t _file : 16;
  uint32_t _line : 24;
  uint32_t _col : 12;
  uint32_t _count : 10;
  uint32_t _stmt_begin : 1;
  uint32_t _bb_begin : 1;
};

}  // namespace base
}  // namespace air

#endif
