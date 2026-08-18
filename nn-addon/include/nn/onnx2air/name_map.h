//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NAME_MAP_H
#define NAME_MAP_H

#include "nn/onnx2air/onnx2air_decl.h"
#include "onnx.pb.h"

namespace nn {
namespace onnx2air {

enum RK { R_NONE, R_SYM, R_PREG, R_CST };

class NAME_MAP {
private:
  NAME_MAP(void);                        // REQUIRED UNDEFINED UNWANTED methods
  NAME_MAP& operator=(const NAME_MAP&);  // REQUIRED UNDEFINED UNWANTED methods
  ADDR_DATUM_PTR _sym;
  PREG_PTR       _preg;
  CONSTANT_PTR   _cst;
  uint16_t       _kind : 2;

  NAME_MAP(RK kind) { _kind = kind; }
  NAME_MAP(ADDR_DATUM_PTR sym) {
    _kind = R_SYM;
    _sym  = sym;
  }
  NAME_MAP(PREG_PTR preg) {
    _kind = R_PREG;
    _preg = preg;
  }
  NAME_MAP(CONSTANT_PTR cst) {
    _kind = R_CST;
    _cst  = cst;
  }

public:
  static NAME_MAP New_none() { return NAME_MAP(R_NONE); }
  static NAME_MAP New_sym(ADDR_DATUM_PTR sym) { return NAME_MAP(sym); }
  static NAME_MAP New_preg(PREG_PTR preg) { return NAME_MAP(preg); }
  static NAME_MAP New_cst(CONSTANT_PTR cst) { return NAME_MAP(cst); }

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
  NAME_MAP(const NAME_MAP& inp) {
    this->_kind = inp.Kind();
    switch (inp.Kind()) {
      case R_NONE:
        break;
      case R_SYM:
        this->_sym = inp.Sym();
        break;
      case R_PREG:
        this->_preg = inp.Preg();
        break;
      case R_CST:
        this->_cst = inp.Cst();
        break;
    }
  }
};

}  // namespace onnx2air
}  // namespace nn

#endif /* NAME_MAP_H */
