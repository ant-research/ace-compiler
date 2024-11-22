//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/ckks/pass.h"

#include "fhe/ckks/ckks_gen.h"
#include "fhe/driver/fhe_cmplr.h"

namespace fhe {

namespace ckks {

CKKS_PASS::CKKS_PASS() : _driver(nullptr) {}

R_CODE CKKS_PASS::Init(driver::FHE_COMPILER* driver) {
  Set_driver(driver);
  CKKS_GEN(driver->Glob_scope(), &driver->Lower_ctx()).Register_ckks_types();
  Get_config().Register_options(driver->Context());
  return R_CODE::NORMAL;
}

R_CODE CKKS_PASS::Pre_run() {
  Get_config().Update_options();
  return R_CODE::NORMAL;
}

R_CODE CKKS_PASS::Run() {
  air::base::GLOB_SCOPE* glob =
      Ckks_driver(Get_driver()->Glob_scope(), &Get_driver()->Lower_ctx(),
                  Get_driver()->Context(), &_config);
  Get_driver()->Update_glob_scope(glob);
  return R_CODE::NORMAL;
}

void CKKS_PASS::Post_run() {}

void CKKS_PASS::Fini() {}

}  // namespace ckks

}  // namespace fhe
