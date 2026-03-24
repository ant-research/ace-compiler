//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/poly/poly2c_pass.h"

#include <iostream>
#include <sstream>

#include "fhe/driver/fhe_cmplr.h"
#include "fhe/poly/poly2c_driver.h"

using namespace std;

namespace fhe {

namespace poly {

POLY2C_PASS::POLY2C_PASS() : _driver(nullptr) {}

R_CODE POLY2C_PASS::Init(driver::FHE_COMPILER* driver) {
  _driver = driver;
  _config.Register_options(driver->Context());
  return R_CODE::NORMAL;
}

R_CODE POLY2C_PASS::Pre_run() {
  _config.Update_options();
  core::PROVIDER provider = _config.Provider();
  if (Check_provider_supported(provider) == false) {
    return R_CODE::USER;
  }
  _config.Set_ifile(_driver->Context()->Ifile());
  return R_CODE::NORMAL;
}

R_CODE POLY2C_PASS::Run() {
  // setup output file
  std::string cfile_name(_driver->Context()->Ofile());
  if (cfile_name.empty()) {
    cfile_name = _driver->Context()->Def_cfile();
  }
  std::ofstream of(cfile_name);
  if (!of.is_open()) {
    CMPLR_USR_MSG(U_CODE::Output_File_Open_Err, cfile_name);
  }

  air::base::GLOB_SCOPE*   glob = _driver->Glob_scope();
  fhe::poly::POLY2C_DRIVER poly2c(of, _driver->Lower_ctx(), _config);
  if (_driver->Poly_pass_disabled()) {
    glob = poly2c.Flatten(glob);
    _driver->Update_glob_scope(glob);
  }

  if (_config.Provider() == core::PROVIDER::ANT) {
    POLY2C_VISITOR visitor(poly2c.Ctx());
    poly2c.template Run<POLY2C_VISITOR>(glob, visitor);
  } else {
    CKKS2C_VISITOR visitor(poly2c.Ctx());
    poly2c.template Run<CKKS2C_VISITOR>(glob, visitor);
  }
  // close the file
  of.close();
  return R_CODE::NORMAL;
}

void POLY2C_PASS::Post_run() {}

void POLY2C_PASS::Fini() {}

bool POLY2C_PASS::Check_provider_supported(core::PROVIDER provider) {
  if (provider == core::PROVIDER::INVALID) {
    std::stringbuf buf;
    std::ostream   os(&buf);
    os << "-P2C:lib=" << _config.Prov_str()
       << ". Only support -P2C:lib=ant|openfhe|seal.";
    CMPLR_USR_MSG(U_CODE::Incorrect_Option, buf.str().c_str());
    return false;
  }
  if (provider != core::PROVIDER::ANT) {
    // disable POLY_GEN pass if use seal or openfhe
    _driver->Disable_poly_pass();
  }
  return true;
}

}  // namespace poly

}  // namespace fhe
