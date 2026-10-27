//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "hpoly_attr_prop.h"

#include "air/opt/ssa_build.h"
#include "air/opt/ssa_container.h"

using namespace air::opt;
using namespace air::base;

namespace fhe {
namespace poly {

void ATTR_PGTR::Run() {
  Ctx().Trace(ATTR_PROP, "#### IR trace before ATTR_PROP\n");
  Ctx().Trace_obj(ATTR_PROP, Func_scope());

  // 1. build ssa
  Build_ssa();
  // 2. traverse IR and prop attribute
  ATTR_VISITOR trav(Ctx());
  NODE_PTR     entry = Func_scope()->Container().Entry_node();
  trav.Visit<ATTR_INFO>(entry);

  Ctx().Trace(ATTR_PROP, "#### IR trace after ATTR_PROP\n");
  Ctx().Trace_obj(ATTR_PROP, Func_scope());
}

void ATTR_PGTR::Build_ssa() {
  SSA_BUILDER ssa_builder(Func_scope(), Ssa_cntr(), Ctx().Driver_ctx());

  // update SSA_CONFIG
  const POLY_CONFIG& poly_conf = Ctx().Config();
  SSA_CONFIG&        ssa_conf  = ssa_builder.Ssa_config();
  ssa_conf.Set_trace_ir_before_ssa(poly_conf.Is_trace(IR_BEFORE_SSA));
  ssa_conf.Set_trace_ir_after_insert_phi(
      poly_conf.Is_trace(IR_AFTER_SSA_INSERT_PHI));
  ssa_conf.Set_trace_ir_after_ssa(poly_conf.Is_trace(IR_AFTER_SSA));

  ssa_builder.Perform();
}

}  // namespace poly
}  // namespace fhe