//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_DRIVER_FHE_CMPLR_H
#define FHE_DRIVER_FHE_CMPLR_H

#include "air/driver/driver.h"
#include "fhe/core/lower_ctx.h"
#include "fhe_pipeline.h"

namespace fhe {

namespace driver {

class FHE_COMPILER : public air::driver::DRIVER {
public:
  FHE_COMPILER(bool standalone);

  template <typename UP_DRV>
  R_CODE Init(UP_DRV* drv) {
    air::driver::DRIVER::Init(drv);
    return _pass_mgr.Init(this);
  }

  R_CODE Init(int argc, char** argv) {
    air::driver::DRIVER::Init(argc, argv);
    return _pass_mgr.Init(this);
  }

  R_CODE Pre_run();

  R_CODE Run();

  void Post_run();

  void Fini();

  fhe::core::LOWER_CTX& Lower_ctx() { return _lower_ctx; }

  void Disable_poly_pass() { _pass_mgr.Set_pass_enable<PASS_ID::POLY>(false); }

  bool Poly_pass_disabled() const {
    return !_pass_mgr.Pass_enable<PASS_ID::POLY>();
  }

private:
  FHE_PASS_MANAGER     _pass_mgr;
  fhe::core::LOWER_CTX _lower_ctx;
};  // FHE_COMPILER

}  // namespace driver

}  // namespace fhe

#endif  // FHE_DRIVER_FHE_CMPLR_H
