//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/sihe/pass.h"

#include <iostream>

#include "fhe/driver/fhe_cmplr.h"
#include "fhe/sihe/sihe_gen.h"

namespace fhe {

namespace sihe {

SIHE_PASS::SIHE_PASS() : _driver(nullptr) {}

R_CODE SIHE_PASS::Init(driver::FHE_COMPILER* driver) {
  Set_driver(driver);
  SIHE_GEN(driver->Glob_scope(), &driver->Lower_ctx()).Register_sihe_types();
  Get_config().Register_options(driver->Context());
  return R_CODE::NORMAL;
}

R_CODE SIHE_PASS::Pre_run() {
  Get_config().Update_options();
  return R_CODE::NORMAL;
}

R_CODE SIHE_PASS::Run() {
  air::base::GLOB_SCOPE* glob =
      Sihe_driver(Get_driver()->Glob_scope(), &Get_driver()->Lower_ctx(),
                  Get_driver()->Context(), Config());
  Get_driver()->Update_glob_scope(glob);
  return R_CODE::NORMAL;
}

void SIHE_PASS::Post_run() {}

void SIHE_PASS::Fini() {}

}  // namespace sihe

}  // namespace fhe
