//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_RESULT_H
#define AIR_RESULT_H

#include "nn/onnx2air/onnx2air_decl.h"
#include "onnx.pb.h"

namespace nn {
namespace onnx2air {

enum RK { R_NONE, R_SYM, R_PREG, R_CST };

class RESULT {
private:
  ADDR_DATUM_PTR _sym;
  PREG_PTR       _preg;
  CONSTANT_PTR   _cst;
  uint16_t       _kind : 2;

  RESULT(RK kind) { _kind = kind; }
  RESULT(ADDR_DATUM_PTR sym) {
    _kind = R_SYM;
    _sym  = sym;
  }
  RESULT(PREG_PTR preg) {
    _kind = R_PREG;
    _preg = preg;
  }
  RESULT(CONSTANT_PTR cst) {
    _kind = R_CST;
    _cst  = cst;
  }

public:
  static RESULT New_none() { return RESULT(R_NONE); }
  static RESULT New_sym(ADDR_DATUM_PTR sym) { return RESULT(sym); }
  static RESULT New_preg(PREG_PTR preg) { return RESULT(preg); }
  static RESULT New_cst(CONSTANT_PTR cst) { return RESULT(cst); }

  RK             Kind() const { return (RK)_kind; }
  ADDR_DATUM_PTR Sym() const {
    AIR_ASSERT_MSG(_kind == R_SYM, ("not sym"));
    return _sym;
  }
  PREG_PTR Preg() const {
    AIR_ASSERT_MSG(_kind == R_PREG, ("not preg"));
    return _preg;
  }
  CONSTANT_PTR Cst() const {
    AIR_ASSERT_MSG(_kind == R_CST, ("not constant"));
    return _cst;
  }

  bool Is_none() const { return _kind == R_NONE; }
  bool Is_sym() const { return _kind == R_SYM; }
  bool Is_preg() const { return _kind == R_PREG; }
  bool Is_cst() const { return _kind == R_CST; }
};

}  // namespace onnx2air
}  // namespace nn

#endif /* AIR_RESULT_H */
