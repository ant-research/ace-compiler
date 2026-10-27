//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/ckks/ckks2py_pass.h"

#include <iostream>
#include <sstream>

#include "fhe/driver/fhe_cmplr.h"
#include "fhe/ckks/ckks2py_driver.h"

using namespace std;

namespace fhe {

namespace ckks {

CKKS2PY_PASS::CKKS2PY_PASS() : _driver(nullptr), _active(false) {}

R_CODE CKKS2PY_PASS::Init(driver::FHE_COMPILER* driver) {
  _driver = driver;
  _config.Register_options(driver->Context());
  return R_CODE::NORMAL;
}

R_CODE CKKS2PY_PASS::Pre_run() {
  _config.Update_options();
  _config.Set_ifile(_driver->Context()->Ifile());

  // Pre_run activates the pass when -CKKS2PY:rt=<module> is non-empty. A
  // -CKKS2PY:lib= token is not part of the registered option table, so the
  // air parser rejects it. With rt= empty the pass stays inactive and the
  // default C path runs unchanged (byte-identical to the pure C path).
  if (_config.Py_rt_empty()) {
    _active = false;
    return R_CODE::NORMAL;
  }

  _active = true;
  // Keep CKKS IR un-lowered (suppress the CKKS->poly IR pass) and suppress the
  // C emitter, mirroring what POLY2C already does for non-ant providers
  // (poly2c_pass.cxx:84-87). PASS_MANAGER skips a disabled pass's Run()
  // automatically (pass_manager.h:110-113).
  _driver->Disable_poly_pass();
  _driver->Disable_poly2c_pass();
  return R_CODE::NORMAL;
}

R_CODE CKKS2PY_PASS::Run() {
  if (!_active) {
    return R_CODE::NORMAL;
  }

  // setup output .py file
  std::string pyfile_name(_driver->Context()->Ofile());
  if (pyfile_name.empty()) {
    pyfile_name = _driver->Context()->Ifile_basename() + ".py";
  }
  std::ofstream of(pyfile_name);
  if (!of.is_open()) {
    CMPLR_USR_MSG(U_CODE::Output_File_Open_Err, pyfile_name);
  }

  air::base::GLOB_SCOPE*    glob = _driver->Glob_scope();
  fhe::ckks::CKKS2PY_DRIVER poly2py(of, _driver->Lower_ctx(), _config);
  if (_driver->Poly_pass_disabled()) {
    // Non-ant path: poly lowering is disabled, so flatten before emission
    // (same step POLY2C performs in poly2c_pass.cxx:54-56).
    glob = poly2py.Flatten(glob);
    _driver->Update_glob_scope(glob);
  }

  CKKS2PY_VISITOR visitor(poly2py.Ctx());
  poly2py.template Run<CKKS2PY_VISITOR>(glob, visitor);

  of.close();
  return R_CODE::NORMAL;
}

void CKKS2PY_PASS::Post_run() {}

void CKKS2PY_PASS::Fini() {}

}  // namespace ckks

}  // namespace fhe