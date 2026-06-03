//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CORE_SCHEME_INFO_PASS_H
#define FHE_CORE_SCHEME_INFO_PASS_H

#include "air/driver/driver.h"
#include "air/driver/pass.h"
#include "air/util/error.h"
#include "fhe/core/scheme_info_ana.h"

namespace fhe {
namespace core {

class SCHEME_INFO_PASS : public air::driver::PASS<SCHEME_INFO_CONFIG> {
public:
  SCHEME_INFO_PASS() {}
  ~SCHEME_INFO_PASS() {}

  R_CODE Init(air::driver::DRIVER* driver) {
    _driver = driver;
    _config.Register_options(driver->Context());
    return R_CODE::NORMAL;
  }

  R_CODE Pre_run() {
    _config.Update_options();
    return R_CODE::NORMAL;
  }

  R_CODE Run() {
    core::SCHEME_INFO_ANA info_analyzer(_driver->Glob_scope(),
                                        _driver->Context(), &_config);
    return info_analyzer.Run();
  }

  const char* Name() { return "SCHEME_INFO_ANALYZE"; }

  DECLARE_SCHEME_INFO_CONFIG_ACCESS_API(_config)

private:
  air::driver::DRIVER* _driver;
  SCHEME_INFO_CONFIG   _config;
};

}  // namespace core
}  // namespace fhe

#endif  // FHE_CORE_SCHEME_INFO_PASS_H
