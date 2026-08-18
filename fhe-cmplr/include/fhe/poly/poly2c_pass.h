//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_POLY_POLY2C_PASS_H
#define FHE_POLY_POLY2C_PASS_H

#include "air/driver/pass.h"
#include "poly2c_config.h"

namespace fhe {

namespace driver {
// forward declaration for FHE_COMPILER
class FHE_COMPILER;
}  // namespace driver

namespace poly {

class POLY2C_PASS : public air::driver::PASS<POLY2C_CONFIG> {
public:
  POLY2C_PASS();

  R_CODE Init(driver::FHE_COMPILER* driver);

  R_CODE Pre_run();

  R_CODE Run();

  void Post_run();

  void Fini();

  const char* Name() const { return "POLY2C"; }

private:
  bool Check_provider_supported(core::PROVIDER provider);

  driver::FHE_COMPILER* _driver;
};  // POLY2C_PASS

}  // namespace poly

}  // namespace fhe

#endif  // FHE_POLY_POLY2C_PASS_H
