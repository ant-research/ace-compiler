//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/opt/cfg.h"

#include <sstream>

namespace air {

namespace opt {

BB_PTR CFG::New_bb(BB_KIND k, const SPOS& spos) {
  BB_DATA_PTR bb_ptr = _bb_tab->Allocate<BB_DATA>();
  new (bb_ptr) BB_DATA(Mem_pool(), k, spos);
  if (k == BB_ENTRY) {
    _entry_bb = bb_ptr.Id();
  } else if (k == BB_EXIT) {
    _exit_bb = bb_ptr.Id();
  }
  return BB_PTR(BB(this, bb_ptr));
}

void CFG::Append_bb(BB_PTR bb) {
  BB_LIST list(this, _entry_bb);
  BB_PTR  tail = list.Tail();
  list.Append(bb->Id());
}

void CFG::Connect_with_succ(BB_PTR pred, BB_PTR succ) {
  pred->Add_succ(succ);
  succ->Add_pred(pred);
}

void CFG::Connect_with_pred(BB_PTR succ, BB_PTR pred) {
  succ->Add_pred(pred);
  pred->Add_succ(succ);
}

void CFG::Build_dom_info() {
  Dom_info().Init(Total_bb_cnt(), Mem_pool());
  DOM_BUILDER dom_builder(this);
  dom_builder.Run();
}

LOOP_INFO_PTR CFG::New_loop_info() {
  LOOP_INFO_DATA_PTR loop_info_ptr = _loop_info_tab->Allocate<LOOP_INFO_DATA>();
  new (loop_info_ptr) LOOP_INFO_DATA();
  return LOOP_INFO_PTR(LOOP_INFO(this, loop_info_ptr));
}

void CFG::Print(std::ostream& os, uint32_t indent) const {
  BB_LIST bb_list(this, _entry_bb);
  bb_list.Print(os, indent);
}

void CFG::Print() const {
  Print(std::cout, 0);
  std::cout << std::endl;
}

std::string CFG::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

}  // namespace opt
}  // namespace air
