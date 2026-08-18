//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/opt/loop_info.h"

#include "air/opt/cfg.h"
namespace air {
namespace opt {
void LOOP_INFO::Init(BB_PTR init, BB_PTR phi, BB_PTR body, BB_PTR cond,
                     BB_PTR exit, HSTMT_PTR incr, HSTMT_PTR init_stmt,
                     HCR_PTR cond_expr, HCR_PTR iv, LOOP_INFO_PTR parent) {
  _data->Init(init->Id(), phi->Id(), body->Id(), cond->Id(), exit->Id(),
              incr->Id(), init_stmt->Id(), cond_expr->Id(), iv->Id(),
              parent->Id());
  phi->Set_loop_info_id(Id());
  body->Set_loop_info_id(Id());
  init->Set_loop_info_id(Id());
  cond->Set_loop_info_id(Id());
  exit->Set_loop_info_id(Id());
}

HSSA_CONTAINER& LOOP_INFO::Hssa_cont() const { return Cfg()->Hssa_cont(); }
BB_PTR LOOP_INFO::Init_bb() const { return Cfg()->Bb_ptr(Data()->Init_bb()); }

HSTMT_PTR LOOP_INFO::Init_stmt() const {
  return Hssa_cont().Stmt_ptr(Data()->Init_stmt());
}

HCR_PTR LOOP_INFO::Cond_expr() const {
  return Hssa_cont().Cr_ptr(Data()->Cond_expr());
}

HSTMT_PTR LOOP_INFO::Incr_stmt() const {
  return Hssa_cont().Stmt_ptr(Data()->Incr_stmt());
}
BB_PTR LOOP_INFO::Loop_body() const { return Cfg()->Bb_ptr(Data()->Body_bb()); }

BB_PTR LOOP_INFO::Exit_bb() const { return Cfg()->Bb_ptr(Data()->Exit_bb()); }

BB_PTR LOOP_INFO::Cond_bb() const { return Cfg()->Bb_ptr(Data()->Cond_bb()); }

HCR_PTR LOOP_INFO::Ind_expr() const {
  return Hssa_cont().Cr_ptr(Data()->Ind_expr());
}

LOOP_INFO_PTR LOOP_INFO::Parent(void) const {
  return Parent_id() == LOOP_INFO_ID() ? Null_ptr
                                       : Cfg()->Loop_info_ptr(Parent_id());
}

}  // namespace opt
}  // namespace air