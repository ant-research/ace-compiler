//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/driver/driver_ctx.h"

#include "air/base/meta_info.h"

namespace air {

namespace driver {

void DRIVER_CTX::Handle_global_options() {
  if (_config.Print_meta()) {
    air::base::META_INFO::Print(std::cout);
    Teardown(R_CODE::NORMAL);
  } else if (_config.Print_pass() || _config.Help()) {
    _option_mgr.Print();
    Teardown(R_CODE::NORMAL);
  }
}

void DRIVER_CTX::Teardown(R_CODE rc) { exit((int)rc); }

}  // namespace driver

}  // namespace air
