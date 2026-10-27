//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/cg/cgir_util.h"
#include "air/cg/isa_core.h"
#include "air/util/debug.h"
#include "test_air2cgir_demo_a2c.inc"
#include "test_air2cgir_demo_air.inc"
#include "test_aircg_demo_isa.inc"

int main() {
  air::cg::Register_core_isa();

  air::base::CONTAINER* air_cont = air::cg::demo::Init_air_cont();
  air_cont->Print();

  air::cg::CGIR_CONTAINER cg_cont(air_cont->Parent_func_scope(), true);
  air::cg::demo::Air_to_cgir(air_cont, &cg_cont);

  cg_cont.Build_ssa();
  cg_cont.Print();

  return 0;
}
