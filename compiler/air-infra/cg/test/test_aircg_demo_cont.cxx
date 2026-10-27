//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "test_aircg_demo_cont.inc"

#include "air/base/st.h"
#include "air/util/debug.h"
#include "test_aircg_demo_isa.inc"

int main() {
  air::cg::CGIR_CONTAINER cntr(nullptr, true);
  air::cg::demo::Init_container(cntr);
  cntr.Print();
  return 0;
}
