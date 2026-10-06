//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_CKKS2PY_PASS_H
#define FHE_CKKS_CKKS2PY_PASS_H

#include "air/driver/pass.h"
#include "ckks2py_config.h"

namespace fhe {

namespace driver {
// forward declaration for FHE_COMPILER
class FHE_COMPILER;
}  // namespace driver

namespace ckks {

class CKKS2PY_PASS : public air::driver::PASS<CKKS2PY_CONFIG> {
public:
  CKKS2PY_PASS();

  R_CODE Init(driver::FHE_COMPILER* driver);

  R_CODE Pre_run();

  R_CODE Run();

  void Post_run();

  void Fini();

  const char* Name() const { return "CKKS2PY"; }

private:
  driver::FHE_COMPILER* _driver;

  //! `_active` is true once `Pre_run` sees a non-empty `-CKKS2PY:rt=<module>`.
  //! The pass activates on `rt=` and stays inactive (default-C path) when
  //! `rt=` is empty. Drives the short-circuit in Run() so the default C path
  //! is untouched otherwise.
  bool _active;
};  // CKKS2PY_PASS

}  // namespace ckks

}  // namespace fhe

#endif  // FHE_CKKS_CKKS2PY_PASS_H