//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_PASS_H
#define FHE_CKKS_PASS_H

#include "air/driver/pass.h"
#include "config.h"

namespace fhe {

namespace driver {
// forward declaration for FHE_COMPILER
class FHE_COMPILER;
}  // namespace driver

namespace ckks {

class CKKS_PASS : public air::driver::PASS<CKKS_CONFIG> {
public:
  CKKS_PASS();

  R_CODE Init(driver::FHE_COMPILER* driver);

  R_CODE Pre_run();

  R_CODE Run();

  void Post_run();

  void Fini();

  const char* Name() const { return "CKKS"; }

private:
  void Set_driver(driver::FHE_COMPILER* driver) { _driver = driver; }

  driver::FHE_COMPILER* Get_driver() const { return _driver; }

  CKKS_CONFIG& Get_config() { return _config; }

  driver::FHE_COMPILER* _driver;
};  // CKKS_PASS

}  // namespace ckks

}  // namespace fhe

#endif  // FHE_CKKS_PASS_H
