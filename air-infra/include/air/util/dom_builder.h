//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_DOM_BUILDER_H
#define AIR_UTIL_DOM_BUILDER_H

#include "air/util/dom.h"

namespace air {
namespace util {

//! @brief DOM_BUILDER holds the wrapper for building the DOM_INFO
template <typename CFG_CONTAINER>
class DOM_BUILDER {
  using CFG      = typename CFG_CONTAINER::CFG;
  using NODE     = typename CFG::NODE;
  using NODE_PTR = air::base::PTR<NODE>;
  using NODE_ID  = typename CFG::NODE_ID;

public:
  DOM_BUILDER(CFG_CONTAINER* cfg_cont, DOM_INFO* dom_info) {
    _cfg_cont = cfg_cont;
    _dom_info = dom_info;
  }

  //! @brief Main entry for DOM_BUILDER
  void Run() {
    _dom_info->Init(_cfg_cont->Cfg().Node_count());
    Compute_post_order();
    Compute_imm_dom();
    Compute_dom_frontiers();
    Compute_iter_dom_frontiers();
    Compute_dom_tree();
    Compute_dom_tree_pre_order();
  }

private:
  CFG_CONTAINER* Cfg_cont(void) const { return _cfg_cont; }

  NODE_PTR Entry(void) const { return Cfg_cont()->Entry(); }

  NODE_PTR Exit(void) const { return Cfg_cont()->Exit(); }

  DOM_INFO* Dom_info(void) const { return _dom_info; }

  void Compute_post_order(void) {
    NODE_PTR entry = Entry();
    AIR_ASSERT(entry != air::base::Null_ptr);

    std::set<uint32_t> visited;
    uint32_t           order_id = 0;
    Number_post_order(entry, order_id, visited);

    AIR_ASSERT(order_id > 0);
    uint32_t            max_order_id   = (uint32_t)order_id - 1;
    DOM_INFO::UI32_VEC& post_order     = Dom_info()->Post_order();
    DOM_INFO::ID_VEC&   rev_post_order = Dom_info()->Rev_post_order();
    rev_post_order.resize(order_id);
    for (uint32_t idx = 0; idx < post_order.size(); idx++) {
      uint32_t pid = post_order[idx];
      if (pid == air::base::Null_prim_id) continue;
      rev_post_order[max_order_id - pid] = idx;
    }
  }

  // Algorithm: A Simple, Fast Dominance Algorithm, Keith D. Cooper
  void Compute_imm_dom(void) {
    NODE_PTR  entry    = Entry();
    DOM_INFO* dom_info = Dom_info();
    dom_info->Set_imm_dom(entry, entry);
    bool changed;
    do {
      changed = false;
      for (size_t i = 1; i < dom_info->Rev_post_order().size(); i++) {
        DOM_INFO::ID node_id = dom_info->Get_rev_post_order(i);
        NODE_PTR     node    = Cfg_cont()->Node(NODE_ID(node_id));
        NODE_PTR pred = node->Pred_cnt() == 0 ? Entry() : node->Pred()->Pred();
        // find the pred node that has a valid dominator
        auto iter = node->Begin_pred();
        iter++;
        while (iter != node->End_pred()) {
          if (dom_info->Get_imm_dom(pred) == air::base::Null_prim_id ||
              pred == node) {
            pred = *iter;
            iter++;
          } else {
            break;
          }
        }
        NODE_PTR imm_dom = pred;
        for (; iter != node->End_pred(); iter++) {
          pred = *iter;
          if (dom_info->Get_imm_dom(pred) != air::base::Null_prim_id &&
              pred != node) {
            imm_dom = Get_intersection(pred, imm_dom);
          }
        }
        if (dom_info->Get_imm_dom(node) != imm_dom->Id().Value()) {
          dom_info->Set_imm_dom(node, imm_dom);
          changed = true;
        }
      }
    } while (changed);
  }

  // Algorithm: A Simple, Fast Dominance Algorithm, Keith D. Cooper
  void Compute_dom_frontiers(void) {
    auto compute_df = [](NODE_PTR node, NODE_PTR entry, DOM_INFO* dom) {
      const uint32_t cand_pred_size = 2;
      if (node == entry) return;
      if (node->Pred_cnt() < cand_pred_size) return;

      DOM_INFO::ID node_id  = node->Id().Value();
      DOM_INFO::ID entry_id = entry->Id().Value();
      auto         iter     = node->Begin_pred();
      for (; iter != node->End_pred(); iter++) {
        DOM_INFO::ID pred_id = iter->Id().Value();
        while (pred_id != dom->Get_imm_dom(node) && pred_id != entry_id) {
          dom->Insert_dom_frontier(pred_id, node_id);
          pred_id = dom->Get_imm_dom(pred_id);
        }
      }
    };
    Cfg_cont()->Cfg().For_each_node(compute_df, Entry(), Dom_info());
  }

  void Compute_iter_dom_frontiers(void) {
    auto compute_iter_df = [](NODE_PTR node, NODE_PTR exit,
                              DOM_BUILDER* builder) {
      if (node->Id() == exit->Id()) return;
      DOM_INFO::ID_SET visited;
      builder->Gen_iter_dom_frontiers(node, node->Id().Value(), visited);
    };
    Cfg_cont()->Cfg().For_each_node(compute_iter_df, Exit(), this);
  }

  void Compute_dom_tree(void) {
    DOM_INFO* dom_info = Dom_info();
    for (size_t i = 0; i < dom_info->Rev_post_order().size(); i++) {
      DOM_INFO::ID node_id = dom_info->Get_rev_post_order(i);
      DOM_INFO::ID imm_dom = dom_info->Get_imm_dom(node_id);
      if (imm_dom == air::base::Null_prim_id || imm_dom == node_id) continue;

      dom_info->Append_dom_child(imm_dom, node_id);
    }
  }

  void Compute_dom_tree_pre_order() {
    uint32_t idx = 0;
    Compute_dom_tree_pre_order(Entry(), idx);
  }

  void Compute_dom_tree_pre_order(NODE_PTR node, uint32_t& idx) {
    DOM_INFO*               dom_info = Dom_info();
    const DOM_INFO::ID_VEC& children = dom_info->Get_dom_children(node);
    dom_info->Set_dom_tree_pre_order(idx, node);
    dom_info->Set_dom_tree_pre_idx(node->Id().Value(), idx++);
    for (auto child_id : children) {
      Compute_dom_tree_pre_order(Cfg_cont()->Node(NODE_ID(child_id)), idx);
    }
  }

  NODE_PTR Get_intersection(NODE_PTR node1, NODE_PTR node2) {
    DOM_INFO* dom_info = Dom_info();
    while (node1 != node2) {
      while (dom_info->Get_post_order(node1) <
             dom_info->Get_post_order(node2)) {
        node1 = Cfg_cont()->Node(NODE_ID(dom_info->Get_imm_dom(node1)));
      }
      while (dom_info->Get_post_order(node2) <
             dom_info->Get_post_order(node1)) {
        node2 = Cfg_cont()->Node(NODE_ID(dom_info->Get_imm_dom(node2)));
      }
    }
    return node1;
  }

  void Number_post_order(NODE_PTR node, uint32_t& order_id,
                         std::set<uint32_t>& visited) {
    if (visited.find(node->Id().Value()) != visited.end()) {
      return;
    }
    visited.insert(node->Id().Value());

    for (auto iter = node->Begin_succ(); iter != node->End_succ(); iter++) {
      NODE_PTR succ = *iter;
      Number_post_order(succ, order_id, visited);
    }
    Dom_info()->Set_post_order(node, order_id++);
  }

  template <typename NODE_PTR>
  void Gen_iter_dom_frontiers(NODE_PTR node, uint32_t orin_id,
                              std::set<uint32_t>& visited) {
    if (visited.find(node->Id().Value()) != visited.end()) return;
    visited.insert(node->Id().Value());
    DOM_INFO* dom_info = Dom_info();
    for (auto df_id : dom_info->Get_dom_frontiers(node)) {
      dom_info->Insert_iter_dom_frontier(orin_id, df_id);
      if (df_id < orin_id) {
        dom_info->Insert_iter_dom_frontier(
            orin_id, dom_info->Get_iter_dom_frontiers(df_id));
      } else {
        NODE_PTR df_bb = Cfg_cont()->Node(NODE_ID(df_id));
        Gen_iter_dom_frontiers(df_bb, orin_id, visited);
      }
    }
  }

private:
  CFG_CONTAINER* _cfg_cont;  //!< input CFG for DOM_INFO building
  DOM_INFO*      _dom_info;  //!< generated dominace information
};

}  // namespace util
}  // namespace air

#endif
