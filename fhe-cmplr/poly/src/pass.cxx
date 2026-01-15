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
  fhe::poly::POLY_DRIVER poly_driver;
  air::base::GLOB_SCOPE* glob = poly_driver.Run(
      _config, _driver->Glob_scope(), _driver->Lower_ctx(), _driver->Context());
  _driver->Update_glob_scope(glob);
  return R_CODE::NORMAL;
}

void POLY_PASS::Post_run() {}

void POLY_PASS::Fini() {}

void POLY_PASS::Set_lower_to_hpoly(bool v) { _config._lower_to_hpoly = v; }

void POLY_PASS::Set_lower_to_hpoly2(bool v) { _config._lower_to_hpoly2 = v; }

void POLY_PASS::Set_prop_attr(bool v) { _config._prop_attr = v; }
}  // namespace poly

}  // namespace fhe
