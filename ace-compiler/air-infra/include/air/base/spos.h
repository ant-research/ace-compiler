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

#include <ostream>
#include <string>

#include "air/util/debug.h"

namespace air {
namespace base {

//! 64 bits source position data structure
class SPOS {
public:
  SPOS()
      : _file(0), _line(0), _col(0), _count(0), _stmt_begin(0), _bb_begin(0) {}
  SPOS(uint64_t f, uint64_t line, uint64_t col, uint64_t count) {
    Set_file(f);
    Set_line(line);
    Set_col(col);
    Set_count(count);
    _stmt_begin = 0;
    _bb_begin   = 0;
  }
  SPOS(const SPOS& o) {
    Set_file(o.File());
    Set_line(o.Line());
    Set_col(o.Col());
    Set_count(o.Count());
    _stmt_begin = o._stmt_begin;
    _bb_begin   = o._bb_begin;
  }

  uint64_t File() const { return _file; }
  uint64_t Line() const { return _line; }
  uint64_t Col() const { return _col; }
  uint64_t Count() const { return _count; }

  bool Is_stmt_beg() const { return _stmt_begin; }
  bool Is_bb_beg() const { return _bb_begin; }

  void Set_file(uint64_t f) {
    AIR_ASSERT(f <= UINT16_MAX);
    _file = f;
  }
  void Set_line(uint64_t l) {
    AIR_ASSERT(l <= 0xFFFFFFU);
    _line = l;
  }
  void Set_col(uint64_t c) {
    AIR_ASSERT(c <= 0xFFFU);
    _col = c;
  }
  void Set_count(uint64_t c) {
    AIR_ASSERT(c <= 0x3FFU);
    _count = c;
  }
  void Set_stmt_beg(bool s) { _stmt_begin = s; }
  void Set_bb_beg(bool b) { _bb_begin = b; }

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

  bool Is_same_pos(const SPOS& o) {
    return File() == o.File() && Line() == o.Line() && Col() == o.Col() &&
           Count() == o.Count() && Is_stmt_beg() == o.Is_stmt_beg() &&
           Is_bb_beg() == o.Is_bb_beg();
  }

private:
  uint64_t _file : 16;
  uint64_t _line : 24;
  uint64_t _col : 12;
  uint64_t _count : 10;
  uint64_t _stmt_begin : 1;
  uint64_t _bb_begin : 1;
};

}  // namespace base
}  // namespace air

#endif
