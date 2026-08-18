//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "test_aircg_demo_isa.inc"

#include "air/cg/targ_info.h"
#include "air/util/debug.h"

int main() {
  air::cg::TARG_INFO_MGR::Print();
  return 0;
}
