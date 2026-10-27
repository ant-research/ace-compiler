//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================
#include "air/opt/dom.h"

#include "air/opt/cfg.h"

namespace air {

namespace opt {

// Returns true if id2 is found within the child nodes of id1(recursively)
bool DOM_INFO::Dominate(BB_ID id1, BB_ID id2) {
  BBID_VEC& dom_children = Get_dom_children(id1);
  for (auto child : dom_children) {
    if (child == id2 || Dominate(child, id2)) {
      return true;
    }
  }
  return false;
}

void DOM_BUILDER::Run() {
  Compute_post_order();
  Compute_imm_dom();
  Compute_dom_frontiers();
  Compute_iter_dom_frontiers();
  Compute_dom_tree();
  Compute_dom_tree_pre_order();
}

BB_PTR DOM_BUILDER::Entry_bb(void) const { return Cfg()->Entry_bb(); }

BB_PTR DOM_BUILDER::Exit_bb(void) const { return Cfg()->Exit_bb(); }

DOM_INFO& DOM_BUILDER::Dom_info(void) const { return Cfg()->Dom_info(); }

void DOM_BUILDER::Number_post_order(BB_PTR bb, int32_t& order_id,
                                    std::set<BB_ID>& visited) {
  if (visited.find(bb->Id()) != visited.end()) {
    return;
  }
  visited.insert(bb->Id());

  for (auto succ_id : bb->Succ()) {
    BB_PTR succ = Cfg()->Bb_ptr(succ_id);
    Number_post_order(succ, order_id, visited);
  }
  Dom_info().Set_post_order(bb, order_id++);
}

void DOM_BUILDER::Compute_post_order() {
  BB_PTR entry = Entry_bb();
  AIR_ASSERT(!entry->Is_null());

  std::set<BB_ID> visited;
  int32_t         order_id = 0;
  Number_post_order(entry, order_id, visited);

  AIR_ASSERT(order_id > 0);
  uint32_t  max_order_id   = (uint32_t)order_id - 1;
  I32_VEC&  post_order     = Dom_info().Post_order();
  BBID_VEC& rev_post_order = Dom_info().Rev_post_order();
  rev_post_order.resize(order_id);
  for (uint32_t idx = 0; idx < post_order.size(); idx++) {
    int32_t pid = post_order[idx];
    if (pid == -1) continue;
    AIR_ASSERT(pid >= 0);
    rev_post_order[max_order_id - pid] = BB_ID(idx);
  }
}

void DOM_BUILDER::Compute_imm_dom() {
  BB_PTR    entry    = Entry_bb();
  DOM_INFO& dom_info = Dom_info();
  dom_info.Set_imm_dom(entry, entry);
  bool changed;
  do {
    changed = false;
    for (size_t i = 0; i < dom_info.Rev_post_order().size(); i++) {
      BB_ID  bb_id   = dom_info.Get_rev_post_order(i);
      BB_PTR bb      = Cfg()->Bb_ptr(bb_id);
      BB_PTR pred_bb = bb->Pred().size() == 0 ? Entry_bb() : bb->Pred(0);
      BB_PTR imm_dom = pred_bb;
      for (size_t j = 1; j < bb->Pred().size(); j++) {
        if (dom_info.Get_imm_dom(pred_bb) == BB_ID() || pred_bb == bb) {
          pred_bb = bb->Pred(j);
        } else {
          imm_dom = Get_intersection(pred_bb, imm_dom);
        }
      }
      if (dom_info.Get_imm_dom(bb) != imm_dom->Id()) {
        dom_info.Set_imm_dom(bb, imm_dom);
        changed = true;
      }
    }
  } while (changed);
}

// Get the common intersection point of bb1 and bb2
BB_PTR DOM_BUILDER::Get_intersection(BB_PTR bb1, BB_PTR bb2) {
  DOM_INFO& dom_info = Dom_info();
  while (bb1 != bb2) {
    while (dom_info.Get_post_order(bb1) < dom_info.Get_post_order(bb2)) {
      bb1 = Cfg()->Bb_ptr(dom_info.Get_imm_dom(bb1));
    }
    while (dom_info.Get_post_order(bb2) < dom_info.Get_post_order(bb1)) {
      bb2 = Cfg()->Bb_ptr(dom_info.Get_imm_dom(bb2));
    }
  }
  return bb1;
}

void DOM_BUILDER::Compute_dom_frontiers() {
  auto compute_df = [](BB_PTR bb, BB_ID entry_id, DOM_INFO& dom) {
    const uint32_t cand_pred_size = 2;
    if (bb->Id() == entry_id) return;
    if (bb->Pred().size() < cand_pred_size) return;

    for (auto pred_id : bb->Pred()) {
      while (pred_id != dom.Get_imm_dom(bb) && pred_id != entry_id) {
        dom.Insert_dom_frontier(pred_id, bb->Id());
        pred_id = dom.Get_imm_dom(pred_id);
      }
    }
  };
  BB_LIST bb_list(Cfg(), Entry_bb()->Id());
  bb_list.For_each(compute_df, Entry_bb()->Id(), Dom_info());
}

void DOM_BUILDER::Gen_iter_dom_frontiers(BB_PTR bb, BB_ID orin_id,
                                         BBID_SET& visited) {
  if (visited.find(bb->Id()) != visited.end()) return;
  visited.insert(bb->Id());
  DOM_INFO& dom_info = Dom_info();
  for (auto df_id : dom_info.Get_dom_frontiers(bb)) {
    dom_info.Insert_iter_dom_frontier(orin_id, df_id);
    if (df_id < orin_id) {
      dom_info.Insert_iter_dom_frontier(orin_id,
                                        dom_info.Get_iter_dom_frontiers(df_id));
    } else {
      BB_PTR df_bb = Cfg()->Bb_ptr(df_id);
      Gen_iter_dom_frontiers(df_bb, orin_id, visited);
    }
  }
}

void DOM_BUILDER::Compute_iter_dom_frontiers(void) {
  auto compute_iter_df = [](BB_PTR bb, BB_ID exit_id, DOM_BUILDER* builder) {
    if (bb->Id() == exit_id) return;
    BBID_SET visited;
    builder->Gen_iter_dom_frontiers(bb, bb->Id(), visited);
  };
  BB_LIST bb_list(Cfg(), Entry_bb()->Id());
  bb_list.For_each(compute_iter_df, Exit_bb()->Id(), this);
}

void DOM_BUILDER::Compute_dom_tree(void) {
  DOM_INFO& dom_info = Dom_info();
  for (size_t i = 0; i < dom_info.Rev_post_order().size(); i++) {
    BB_ID  bb_id   = dom_info.Get_rev_post_order(i);
    BB_PTR bb      = Cfg()->Bb_ptr(bb_id);
    BB_ID  imm_dom = dom_info.Get_imm_dom(bb);
    if (imm_dom == BB_ID() || imm_dom == bb_id) continue;

    dom_info.Append_dom_child(imm_dom, bb_id);
  }
}

void DOM_BUILDER::Compute_dom_tree_pre_order() {
  size_t idx = 0;
  Compute_dom_tree_pre_order(Entry_bb(), idx);
}

void DOM_BUILDER::Compute_dom_tree_pre_order(BB_PTR bb, size_t& idx) {
  DOM_INFO& dom_info = Dom_info();
  BBID_VEC& children = dom_info.Get_dom_children(bb);
  dom_info.Set_dom_tree_pre_order(idx, bb->Id());
  dom_info.Set_dom_tree_pre_idx(bb->Id(), idx++);
  for (auto child_id : children) {
    Compute_dom_tree_pre_order(Cfg()->Bb_ptr(child_id), idx);
  }
}

}  // namespace opt
}  // namespace air
