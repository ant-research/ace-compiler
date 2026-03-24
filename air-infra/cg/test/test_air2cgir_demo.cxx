//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/cg/cgir_util.h"
#include "air/util/debug.h"
#include "test_air2cgir_demo_a2c.inc"
#include "test_air2cgir_demo_air.inc"
#include "test_aircg_demo_isa.inc"

int main() {
  air::base::CONTAINER* air_cont = air::cg::demo::Init_air_cont();
  air_cont->Print();

  air::cg::CGIR_CONTAINER cg_cont(air_cont->Parent_func_scope(), true);
  air::cg::demo::Air_to_cgir(air_cont, &cg_cont);
  for (auto&& bb : air::cg::LAYOUT_ITER(&cg_cont)) {
    printf("%d\n", bb->Id().Value());
    for (auto&& inst : air::cg::INST_ITER(bb)) {
      inst->Print();
    }
  }
  return 0;
}
