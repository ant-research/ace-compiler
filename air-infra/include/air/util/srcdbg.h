//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_SRCDBG_H
#define AIR_UTIL_SRCDBG_H

namespace air {

namespace util {

class SRCDBG {
  // src info carried inside compiler phases for eventual
  // debug (compiler phases and also user executable) purposes
  unsigned short _column;
  unsigned short _filenum;
  unsigned int   _linenum;

public:
  /**
   * @brief Construct a new SRCDBG object
   *
   */
  SRCDBG() { _column = _filenum = _linenum = 0; }

  /**
   * @brief Construct a new SRCDBG object
   *
   * @param c
   * @param f
   * @param l
   */
  SRCDBG(unsigned short c, unsigned short f, unsigned int l) {
    _column = c, _filenum = f, _linenum = l;
  }

  /**
   * @brief
   *
   * @param c
   */
  void Col(int c) { _column = c; }

  /**
   * @brief
   *
   * @return unsigned short
   */
  unsigned short Col(void) { return _column; }

  /**
   * @brief
   *
   * @param f
   */
  void Filenum(int f) { _filenum = f; }

  /**
   * @brief
   *
   * @return unsigned short
   */
  unsigned short Filenum(void) { return _filenum; }

  /**
   * @brief
   *
   * @param l
   */
  void Linenum(unsigned int l) { _linenum = l; }

  /**
   * @brief
   *
   * @return unsigned int
   */
  unsigned int Linenum(void) { return _linenum; }
};

}  // namespace util

}  // namespace air

#endif  // AIR_UTIL_SRCDBG_H
