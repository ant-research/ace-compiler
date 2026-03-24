//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_SIHE_PASS_H
#define FHE_SIHE_PASS_H

#include "air/driver/pass.h"
#include "config.h"

namespace fhe {

namespace driver {
// forward declaration for FHE_COMPILER
class FHE_COMPILER;
}  // namespace driver

namespace sihe {

class SIHE_PASS : public air::driver::PASS<SIHE_CONFIG> {
public:
  SIHE_PASS();

  R_CODE Init(driver::FHE_COMPILER* driver);

  R_CODE Pre_run();

  R_CODE Run();

  void Post_run();

  void Fini();

  const char* Name() const { return "SIHE"; }

private:
  void Set_driver(driver::FHE_COMPILER* driver) { _driver = driver; }

  driver::FHE_COMPILER* Get_driver() const { return _driver; }

  SIHE_CONFIG& Get_config() { return _config; }

  driver::FHE_COMPILER* _driver;
};  // SIHE_PASS

}  // namespace sihe

}  // namespace fhe

#endif  // FHE_SIHE_PASS_H
