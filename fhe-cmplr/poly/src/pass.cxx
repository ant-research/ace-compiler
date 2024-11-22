//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/poly/pass.h"

#include "fhe/driver/fhe_cmplr.h"
#include "fhe/poly/poly_driver.h"

using namespace std;

namespace fhe {

namespace poly {

POLY_PASS::POLY_PASS() : _driver(nullptr) {}

R_CODE POLY_PASS::Init(driver::FHE_COMPILER* driver) {
  _driver = driver;
  _config.Register_options(driver->Context());
  return R_CODE::NORMAL;
}

R_CODE POLY_PASS::Pre_run() {
  _config.Update_options();
  return R_CODE::NORMAL;
}

R_CODE POLY_PASS::Run() {
  if (_config.Enable() == false) {
    CMPLR_WARN_MSG(_driver->Tfile(), "POLY GEN PASS is disabled.");
    return R_CODE::NORMAL;
  }
  fhe::poly::POLY_DRIVER poly_driver;
  air::base::GLOB_SCOPE* glob =
      poly_driver.Run(_config, _driver->Glob_scope(), _driver->Lower_ctx());
  _driver->Update_glob_scope(glob);
  return R_CODE::NORMAL;
}

void POLY_PASS::Post_run() {}

void POLY_PASS::Fini() {}

}  // namespace poly

}  // namespace fhe
