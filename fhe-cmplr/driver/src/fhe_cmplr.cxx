//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/driver/fhe_cmplr.h"

#include <iostream>

using namespace std;

namespace fhe {

namespace driver {

FHE_COMPILER::FHE_COMPILER(bool standalone) : air::driver::DRIVER(standalone) {}

R_CODE FHE_COMPILER::Pre_run() { return _pass_mgr.Pre_run(this); }

R_CODE FHE_COMPILER::Run() { return _pass_mgr.Run(this); }

void FHE_COMPILER::Post_run() { _pass_mgr.Post_run(this); }

void FHE_COMPILER::Fini() { _pass_mgr.Fini(this); }

}  // namespace driver

}  // namespace fhe
