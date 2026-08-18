//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_DOM_H
#define AIR_OPT_DOM_H

#include <map>
#include <vector>

#include "air/opt/bb.h"

using namespace air::base;
namespace air {
namespace opt {

class DOM_INFO {
  friend class DOM_BUILDER;

public:
  DOM_INFO(CFG_MEM_POOL& mpool)
      : _post_order(I32_ALLOC(&mpool)),
        _rev_post_order(BBID_ALLOC(&mpool)),
        _imm_dom(BBID_ALLOC(&mpool)),
        _dom_frontiers(BBID_CUS_SET_ALLOC(&mpool)),
        _iter_dom_frontiers(BBID_CUS_SET_ALLOC(&mpool)),
        _dom_tree(BBID_VEC_ALLOC(&mpool)),
        _dom_tree_pre_order(BBID_ALLOC(&mpool)),
        _dom_tree_pre_idx(I32_ALLOC(&mpool)) {}

  void Init(uint32_t total_bb_cnt, CFG_MEM_POOL& mpool) {
    _post_order.resize(total_bb_cnt, -1);
    _imm_dom.resize(total_bb_cnt, BB_ID());
    _dom_frontiers.resize(total_bb_cnt, BBID_CUS_SET(&mpool));
    _iter_dom_frontiers.resize(total_bb_cnt, BBID_CUS_SET(&mpool));
    _dom_tree.resize(total_bb_cnt, BBID_VEC(&mpool));
    _dom_tree_pre_order.resize(total_bb_cnt, BB_ID());
    _dom_tree_pre_idx.resize(total_bb_cnt, -1);
  }

  // return true if BB id1 dominates BB id2
  bool Dominate(BB_ID id1, BB_ID id2);
  bool Dominate(BB_PTR bb1, BB_PTR bb2) {
    return Dominate(bb1->Id(), bb2->Id());
  }

  // Post_order
  I32_VEC& Post_order(void) { return _post_order; }
  int32_t  Get_post_order(uint32_t idx) {
    AIR_ASSERT(idx < _post_order.size());
    return _post_order[idx];
  }
  int32_t Get_post_order(BB_PTR bb) { return Get_post_order(bb->Id().Value()); }

  // Rev_post_order
  BBID_VEC& Rev_post_order(void) { return _rev_post_order; }
  BB_ID     Get_rev_post_order(uint32_t idx) {
    AIR_ASSERT(idx < _rev_post_order.size());
    return _rev_post_order[idx];
  }

  // Imm_dom
  BBID_VEC& Imm_dom(void) { return _imm_dom; }
  BB_ID     Get_imm_dom(uint32_t idx) {
    AIR_ASSERT(idx < _imm_dom.size());
    return _imm_dom[idx];
  }
  BB_ID Get_imm_dom(BB_ID id) { return Get_imm_dom(id.Value()); }
  BB_ID Get_imm_dom(BB_PTR bb) { return Get_imm_dom(bb->Id().Value()); }

  // Dom_frontiers
  BBID_SET_VEC& Dom_frontiers(void) { return _dom_frontiers; }
  BBID_CUS_SET& Get_dom_frontiers(BB_ID id) {
    AIR_ASSERT(id.Value() < _dom_frontiers.size());
    return _dom_frontiers[id.Value()];
  }
  BBID_CUS_SET& Get_dom_frontiers(BB_PTR bb) {
    return Get_dom_frontiers(bb->Id());
  }

  // Iter Dom frontiers
  BBID_SET_VEC& Iter_dom_frontiers(void) { return _iter_dom_frontiers; }
  BBID_CUS_SET& Get_iter_dom_frontiers(BB_ID id) {
    AIR_ASSERT(id.Value() < _iter_dom_frontiers.size());
    return _iter_dom_frontiers[id.Value()];
  }
  BBID_CUS_SET& Get_iter_dom_frontiers(BB_PTR bb) {
    return Get_iter_dom_frontiers(bb->Id());
  }

  // Dom tree
  BBID_VEC_VEC& Dom_tree(void) { return _dom_tree; }
  BBID_VEC&     Get_dom_children(BB_ID id) {
    AIR_ASSERT(id.Value() < _dom_tree.size());
    return _dom_tree[id.Value()];
  }
  BBID_VEC& Get_dom_children(BB_PTR bb) { return Get_dom_children(bb->Id()); }

  // Dom tree pre order
  BBID_VEC& Dom_tree_pre_order(void) { return _dom_tree_pre_order; }
  BB_ID     Get_dom_tree_pre_order(BB_ID id) {
    AIR_ASSERT(id.Value() < _dom_tree_pre_order.size());
    return _dom_tree_pre_order[id.Value()];
  }
  BB_ID Get_dom_tree_pre_order(BB_PTR bb) {
    return Get_dom_tree_pre_order(bb->Id());
  }

  // Dom tree pre index
  I32_VEC& Dom_tree_pre_idx(void) { return _dom_tree_pre_idx; }
  int32_t  Get_dom_tree_pre_idx(BB_ID id) {
    AIR_ASSERT(id.Value() < _dom_tree_pre_idx.size());
    return _dom_tree_pre_idx[id.Value()];
  }
  int32_t Get_dom_tree_pre_idx(BB_PTR bb) {
    return Get_dom_tree_pre_idx(bb->Id());
  }

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

private:
  void Set_post_order(uint32_t idx, int32_t value) {
    AIR_ASSERT(idx < _post_order.size());
    _post_order[idx] = value;
  }

  void Set_post_order(BB_PTR bb, int32_t value) {
    Set_post_order(bb->Id().Value(), value);
  }

  void Set_rev_post_order(uint32_t idx, BB_ID value) {
    AIR_ASSERT(idx < _rev_post_order.size());
    _rev_post_order[idx] = value;
  }

  void Set_imm_dom(uint32_t idx, BB_ID value) {
    AIR_ASSERT(idx < _imm_dom.size());
    _imm_dom[idx] = value;
  }

  void Set_imm_dom(BB_PTR bb, BB_PTR dom_bb) {
    Set_imm_dom(bb->Id().Value(), dom_bb->Id());
  }

  void Insert_dom_frontier(BB_ID bb_id, BB_ID df_id) {
    AIR_ASSERT(bb_id.Value() < _dom_frontiers.size());
    BBID_CUS_SET& id_set = Get_dom_frontiers(bb_id);
    id_set.insert(df_id);
  }

  void Insert_dom_frontier(BB_PTR bb, BB_PTR df_bb) {
    Insert_dom_frontier(bb->Id(), df_bb->Id());
  }

  void Insert_iter_dom_frontier(BB_ID bb_id, BB_ID df_id) {
    AIR_ASSERT(bb_id.Value() < _iter_dom_frontiers.size());
    BBID_CUS_SET& id_set = Get_iter_dom_frontiers(bb_id);
    id_set.insert(df_id);
  }

  void Insert_iter_dom_frontier(BB_PTR bb, BB_PTR df_bb) {
    Insert_dom_frontier(bb->Id(), df_bb->Id());
  }

  void Insert_iter_dom_frontier(BB_ID bb_id, BBID_CUS_SET& df_set) {
    for (auto df_id : df_set) {
      Insert_iter_dom_frontier(bb_id, df_id);
    }
  }

  void Append_dom_child(BB_ID bb_id, BB_ID child_id) {
    AIR_ASSERT(bb_id.Value() < _dom_tree.size());
    _dom_tree[bb_id.Value()].push_back(child_id);
  }

  void Append_dom_child(BB_PTR bb, BB_PTR child) {
    Append_dom_child(bb->Id(), child->Id());
  }

  void Set_dom_tree_pre_order(uint32_t idx, BB_ID bb_id) {
    AIR_ASSERT(idx < _dom_tree_pre_order.size());
    _dom_tree_pre_order[idx] = bb_id;
  }
  void Set_dom_tree_pre_order(uint32_t idx, BB_PTR bb) {
    Set_dom_tree_pre_order(idx, bb->Id());
  }

  void Set_dom_tree_pre_idx(BB_ID bb_id, int32_t seq_idx) {
    AIR_ASSERT(bb_id.Value() < _dom_tree_pre_idx.size());
    AIR_ASSERT(seq_idx < _dom_tree_pre_order.size());
    _dom_tree_pre_idx[bb_id.Value()] = seq_idx;
  }

private:
  I32_VEC _post_order;          // numbering BB's order in post order traverse
                                // _post_order[BB_ID.Value()] = POST Index
  BBID_VEC _rev_post_order;     // the reverse post order BB sequence
                                // _rev_post_order[rev_post_idx] = BB_ID
  BBID_VEC _imm_dom;            // the immediate dominator of bb
                                // _imm_dom[bb_id] = BB_ID
  BBID_SET_VEC _dom_frontiers;  // the dominate frontier set
                                // _dom_frontier[bb_id] = {BB_ID,...}
  BBID_SET_VEC _iter_dom_frontiers;  // iterative dominance frontier set
  BBID_VEC_VEC _dom_tree;            // the dominate tree
                                     // _dom_tree[bb_id] = {BB_ID, ...}
  BBID_VEC _dom_tree_pre_order;      // preorder of dominate tree
                                     //  _dom_tree_pre_order[idx] = BB_ID
  I32_VEC _dom_tree_pre_idx;  // BB's sequence number in preorder of dominate
                              // tree _dom_tree_pre_idx[BB_ID.Value()] = seq_idx
};

class DOM_BUILDER {
public:
  DOM_BUILDER(CFG* cfg) { _cfg = cfg; }
  void Run();

private:
  CFG*      Cfg(void) const { return _cfg; }
  BB_PTR    Entry_bb(void) const;
  BB_PTR    Exit_bb(void) const;
  DOM_INFO& Dom_info(void) const;

  void Compute_post_order(void);
  void Compute_imm_dom(void);
  void Compute_dom_frontiers(void);
  void Compute_iter_dom_frontiers(void);
  void Compute_dom_tree(void);

  void Compute_dom_tree_pre_order();

  BB_PTR Get_intersection(BB_PTR bb1, BB_PTR bb2);
  void   Number_post_order(BB_PTR bb, int32_t& order_id,
                           std::set<BB_ID>& visited);

  void Gen_iter_dom_frontiers(BB_PTR bb, BB_ID orin_id, BBID_SET& visited);
  void Compute_dom_tree_pre_order(BB_PTR bb, size_t& idx);

private:
  CFG* _cfg;
};

}  // namespace opt
}  // namespace air

#endif
