//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================
#include "air/opt/bb.h"

#ifdef ENABLED_HSSA
#include <algorithm>
#include <sstream>

#include "air/opt/cfg.h"
#include "air/opt/hssa_container.h"
#include "air/opt/hssa_mu_chi.h"
#include "air/opt/hssa_stmt.h"
#include "air/util/node_list.h"
namespace air {

namespace opt {

BB::BB(CFG* cfg, BB_DATA_PTR data) : _cfg(cfg), _data(data) {}

BB_PTR BB::Next(void) const {
  return (Next_id() == BB_ID()) ? Null_ptr : Cfg()->Bb_ptr(Next_id());
}

BB_PTR BB::Pred(uint32_t idx) const { return Cfg()->Bb_ptr(Pred_id(idx)); }

BB_PTR BB::Succ(uint32_t idx) const { return Cfg()->Bb_ptr(Succ_id(idx)); }

HSTMT_PTR BB::Begin_stmt() const {
  if (Begin_stmt_id() != HSTMT_ID()) {
    return Cfg()->Hssa_cont()->Stmt_ptr(Begin_stmt_id());
  } else {
    return Null_ptr;
  }
}

HPHI_PTR BB::Begin_phi() const {
  if (Begin_phi_id() != HPHI_ID()) {
    return Cfg()->Hssa_cont()->Phi_ptr(Begin_phi_id());
  } else {
    return Null_ptr;
  }
}

LOOP_INFO_PTR BB::Loop_info() const {
  return Loop_info_id() == LOOP_INFO_ID() ? Null_ptr
                                          : _cfg->Loop_info_ptr(Loop_info_id());
}

void BB::Set_loop_info(LOOP_INFO_PTR info) { Set_loop_info_id(info->Id()); }

void BB::Prepend_stmt(HSTMT_PTR stmt) {
  stmt->Set_bb_id(Id());
  stmt->Set_next(Begin_stmt_id());
  Set_begin_stmt_id(stmt->Id());
}

void BB::Append_stmt(HSTMT_PTR stmt) {
  stmt->Set_bb_id(Id());
  if (Begin_stmt_id() == HSTMT_ID()) {
    Set_begin_stmt_id(stmt->Id());
    return;
  }
  HSTMT_LIST stmtlist(Cfg()->Hssa_cont(), Begin_stmt_id());
  stmtlist.Append(stmt->Id());
}

void BB::Remove_stmt(HSTMT_PTR stmt) {
  HSTMT_LIST stmtlist(Cfg()->Hssa_cont(), Begin_stmt_id());
  HSTMT_ID   new_head = stmtlist.Remove(stmt->Id());
  if (new_head != Begin_stmt_id()) {
    Set_begin_stmt_id(new_head);
  }
}

void BB::Insert_stmt_before(HSTMT_PTR stmt, HSTMT_PTR pos) {
  HSTMT_LIST stmtlist(Cfg()->Hssa_cont(), Begin_stmt_id());
  HSTMT_ID   new_head = stmtlist.Insert_before(stmt->Id(), pos->Id());
  if (new_head != Begin_stmt_id()) {
    Set_begin_stmt_id(new_head);
  }
}

void BB::Insert_stmt_after(HSTMT_PTR stmt, HSTMT_PTR pos) {
  AIR_ASSERT(pos != Null_ptr);
  stmt->Set_next(pos->Next_id());
  pos->Set_next(stmt->Id());
}

void BB::Add_succ(BB_PTR succ_bb) {
  BBID_VEC& succ_vec = Succ();
  if (std::find(succ_vec.begin(), succ_vec.end(), succ_bb->Id()) ==
      succ_vec.end()) {
    succ_vec.push_back(succ_bb->Id());
  }
}

void BB::Add_pred(BB_PTR pred_bb) {
  BBID_VEC& pred_vec = Pred();
  if (std::find(pred_vec.begin(), pred_vec.end(), pred_bb->Id()) ==
      pred_vec.end()) {
    pred_vec.push_back(pred_bb->Id());
  }
}

void BB::Append_phi(HPHI_PTR phi) {
  if (Begin_phi_id() == HPHI_ID()) {
    Set_begin_phi_id(phi->Id());
    return;
  }
  HCONTAINER* cont = Cfg()->Hssa_cont();
  HPHI_LIST   phi_list(cont, Begin_phi_id());
  phi_list.Append(phi->Id());
}

bool BB::Dominates(BB_PTR bb) {
  if (Id() == bb->Id()) return true;
  return Cfg()->Dom_info().Dominate(Id(), bb->Id());
}

const char* BB::Kind_name(void) const {
  switch (Kind()) {
    case BB_UNKNOWN:
      return "unk";
    case BB_PHI:
      return "phi";
    case BB_LOOP_START:
      return "loop_start";
    case BB_LOOP_END:
      return "loop_end";
    case BB_COND:
      return "cond";
    case BB_EXIT:
      return "exit";
    case BB_ENTRY:
      return "entry";
    default:
      AIR_ASSERT(false);
  }
  return "unknown";
}

void BB::Print(std::ostream& os, uint32_t indent) const {
  DOM_INFO& dom = Cfg()->Dom_info();
  os << "---- BB" << Id().Value();
  os << " (" << Kind_name() << ")";
  if (Loop_info_id() != LOOP_INFO_ID()) {
    os << "Loop info:" << Loop_info_id().Value();
  }
  os << std::endl;
  os << "Preds:";
  for (auto pred : Pred()) {
    os << pred.Value() << " ";
  }
  os << "\nSuccs:";
  for (auto succ : Succ()) {
    os << succ.Value() << " ";
  }
  os << std::endl;

  os << "Imm_dom:";
  if (dom.Get_imm_dom(Id()) != BB_ID()) {
    os << dom.Get_imm_dom(Id()).Value();
  }
  os << std::endl;

  os << "Dom_frontiers:";
  for (auto df_id : dom.Get_dom_frontiers(Id())) {
    os << df_id.Value() << " ";
  }
  os << std::endl;

  os << "Iter_dom_frontiers:";
  for (auto df_id : dom.Get_iter_dom_frontiers(Id())) {
    os << df_id.Value() << " ";
  }
  os << std::endl;

  os << "Dom_children:";
  for (auto dom_child : dom.Get_dom_children(Id())) {
    os << dom_child.Value() << " ";
  }
  os << std::endl;

  HCONTAINER* cont = Cfg()->Hssa_cont();
  HPHI_LIST   phi_list(cont, Begin_phi_id());
  phi_list.Print(os, indent);

  HSTMT_LIST stmt_list(cont, Begin_stmt_id());
  stmt_list.Print(os, indent);
}

void BB::Print() const {
  Print(std::cout, 0);
  std::cout << std::endl;
}

std::string BB::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

}  // namespace opt
}  // namespace air

#endif
