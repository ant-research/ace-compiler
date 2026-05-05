//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_DOM_H
#define AIR_UTIL_DOM_H

#include <map>
#include <set>
#include <vector>

#include "air/util/debug.h"

namespace air {
namespace util {

// forward declaration for DOM_BUILDER
template <typename CFG_CONTAINER>
class DOM_BUILDER;

//! @brief DOM_INFO holds the precomputed dominance data
class DOM_INFO {
public:
  template <typename CFG_CONTAINER>
  friend class DOM_BUILDER;

  using DOM_MPOOL     = air::util::MEM_POOL<4096>;
  using UI32_ALLOC    = air::util::CXX_MEM_ALLOCATOR<uint32_t, DOM_MPOOL>;
  using UI32_VEC      = std::vector<uint32_t, UI32_ALLOC>;
  using ID            = uint32_t;
  using ID_ALLOC      = air::util::CXX_MEM_ALLOCATOR<ID, DOM_MPOOL>;
  using ID_VEC        = std::vector<ID, ID_ALLOC>;
  using ID_VEC_ALLOC  = air::util::CXX_MEM_ALLOCATOR<ID_VEC, DOM_MPOOL>;
  using ID_VEC_VEC    = std::vector<ID_VEC, ID_VEC_ALLOC>;
  using ID_SET        = std::set<ID>;
  using ID_CSET       = std::set<ID, std::less<ID>, ID_ALLOC>;
  using ID_CSET_ALLOC = air::util::CXX_MEM_ALLOCATOR<ID_CSET, DOM_MPOOL>;
  using ID_SET_VEC    = std::vector<ID_CSET, ID_CSET_ALLOC>;

  DOM_INFO(void)
      : _post_order(UI32_ALLOC(&_mpool)),
        _rev_post_order(ID_ALLOC(&_mpool)),
        _imm_dom(ID_ALLOC(&_mpool)),
        _df(ID_CSET_ALLOC(&_mpool)),
        _iter_df(ID_CSET_ALLOC(&_mpool)),
        _dom_tree(ID_VEC_ALLOC(&_mpool)),
        _dom_tree_pre_order(ID_ALLOC(&_mpool)),
        _dom_tree_pre_idx(UI32_ALLOC(&_mpool)) {}

  //! @brief Clear each table in preparation for rebuilding DOM_INFO.
  void Clear(void) {
    _post_order.clear();
    _imm_dom.clear();
    _df.clear();
    _iter_df.clear();
    _dom_tree.clear();
    _dom_tree_pre_order.clear();
    _dom_tree_pre_idx.clear();
  }

  void Init(uint32_t total_node_cnt) {
    AIR_ASSERT(_post_order.empty());
    _post_order.resize(total_node_cnt, air::base::Null_prim_id);
    AIR_ASSERT(_imm_dom.empty());
    _imm_dom.resize(total_node_cnt, ID());
    AIR_ASSERT(_df.empty());
    _df.resize(total_node_cnt, ID_CSET(&_mpool));
    AIR_ASSERT(_iter_df.empty());
    _iter_df.resize(total_node_cnt, ID_CSET(&_mpool));
    AIR_ASSERT(_dom_tree.empty());
    _dom_tree.resize(total_node_cnt, ID_VEC(&_mpool));
    AIR_ASSERT(_dom_tree_pre_order.empty());
    _dom_tree_pre_order.resize(total_node_cnt, ID());
    AIR_ASSERT(_dom_tree_pre_idx.empty());
    _dom_tree_pre_idx.resize(total_node_cnt, -1);
  }

  // Post_order access methods
  const UI32_VEC& Post_order(void) const { return _post_order; }
  uint32_t        Get_post_order(ID id) const {
    AIR_ASSERT(id < _post_order.size());
    return _post_order[id];
  }

  template <typename NODE_PTR>
  uint32_t Get_post_order(NODE_PTR node) const {
    return Get_post_order(node->Id().Value());
  }

  // Rev_post_order access methods
  const ID_VEC& Rev_post_order(void) const { return _rev_post_order; }
  ID            Get_rev_post_order(ID id) const {
    AIR_ASSERT(id < _rev_post_order.size());
    return _rev_post_order[id];
  }

  // Immidiate dominator access methods
  const ID_VEC& Imm_dom(void) const { return _imm_dom; }

  ID Get_imm_dom(ID id) const {
    AIR_ASSERT(id < _imm_dom.size());
    return _imm_dom[id];
  }

  template <typename NODE_PTR>
  ID Get_imm_dom(NODE_PTR node) const {
    return Get_imm_dom(node->Id().Value());
  }

  // Dom_frontiers access methods
  const ID_SET_VEC& Dom_frontiers(void) const { return _df; }
  const ID_CSET&    Get_dom_frontiers(ID id) const {
    AIR_ASSERT(id < _df.size());
    return _df[id];
  }

  template <typename NODE_PTR>
  const ID_CSET& Get_dom_frontiers(NODE_PTR node) const {
    return Get_dom_frontiers(node->Id().Value());
  }

  // Iterative Dom frontiers access methods
  const ID_SET_VEC& Iter_dom_frontiers(void) const { return _iter_df; }
  const ID_CSET&    Get_iter_dom_frontiers(uint32_t id) const {
    AIR_ASSERT(id < _iter_df.size());
    return _iter_df[id];
  }

  template <typename NODE_PTR>
  const ID_CSET& Get_iter_dom_frontiers(NODE_PTR node) const {
    return Get_iter_dom_frontiers(node->Id().Value());
  }

  // Dom tree access methods
  const ID_VEC_VEC& Dom_tree(void) const { return _dom_tree; }
  const ID_VEC&     Get_dom_children(ID id) const {
    AIR_ASSERT(id < _dom_tree.size());
    return _dom_tree[id];
  }
  template <typename NODE_PTR>
  const ID_VEC& Get_dom_children(NODE_PTR node) const {
    return Get_dom_children(node->Id().Value());
  }

  // Dom tree pre order access methods
  const ID_VEC& Dom_tree_pre_order(void) const { return _dom_tree_pre_order; }
  const ID      Get_dom_tree_pre_order(uint32_t idx) const {
    AIR_ASSERT(idx < _dom_tree_pre_order.size());
    return _dom_tree_pre_order[idx];
  }
  template <typename NODE_PTR>
  const ID Get_dom_tree_pre_order(NODE_PTR node) const {
    return Get_dom_tree_pre_order(node->Id().Value());
  }

  // Dom tree pre-order access methods
  const UI32_VEC& Dom_tree_pre_idx(void) const { return _dom_tree_pre_idx; }
  uint32_t        Get_dom_tree_pre_idx(ID id) const {
    AIR_ASSERT(id < _dom_tree_pre_idx.size());
    return _dom_tree_pre_idx[id];
  }
  template <typename NODE_PTR>
  uint32_t Get_dom_tree_pre_idx(NODE_PTR node) const {
    return Get_dom_tree_pre_idx(node->Id().Value());
  }

  //! @brief Check if node with id1 dominate node with id2
  //! @return true if BB id1 dominates BB id2, otherwise return false
  bool Dominate(ID id1, ID id2) const {
    const ID_VEC& dom_children = Get_dom_children(id1);
    for (auto child : dom_children) {
      if (child == id2 || Dominate(child, id2)) {
        return true;
      }
    }
    return false;
  }

  //! @brief Check if node1 dominate node2
  //! @return true if node1 dominates node2, otherwise return false
  template <typename NODE_PTR>
  bool Dominate(NODE_PTR node1, NODE_PTR node2) const {
    return Dominate(node1->Id(), node2->Id());
  }

  void Print(std::ostream& os, uint32_t indent = 0) const {
    os << "[Post order]: ";
    for (auto iter = _post_order.begin(); iter != _post_order.end(); iter++) {
      os << *iter << " ";
    }
    os << std::endl;

    os << "[Rev post order]:";
    for (auto iter = _rev_post_order.begin(); iter != _rev_post_order.end();
         iter++) {
      os << "n" << *iter << " ";
    }
    os << std::endl;

    os << "[Immediate dominator]:";
    uint32_t node_id = 0;
    for (auto iter = _imm_dom.begin(); iter != _imm_dom.end();
         iter++, node_id++) {
      os << "(n" << node_id << ",n" << *iter << ")";
    }
    os << std::endl;

    os << "[Dominate frontiers]:";
    node_id = 0;
    for (auto iter = _df.begin(); iter != _df.end(); iter++, node_id++) {
      os << "(n" << node_id << ": {";
      const ID_CSET& df_set = *iter;
      for (auto iter_set = df_set.begin(); iter_set != df_set.end();
           iter_set++) {
        os << "n" << *iter_set << " ";
      }
      os << "})";
    }
    os << std::endl;

    os << "[Iterative dominate frontiers]:";
    node_id = 0;
    for (auto iter = _iter_df.begin(); iter != _iter_df.end();
         iter++, node_id++) {
      os << "(n" << node_id << ": {";
      const ID_CSET& df_set = *iter;
      for (auto iter_set = df_set.begin(); iter_set != df_set.end();
           iter_set++) {
        os << "n" << *iter_set << " ";
      }
      os << "})";
    }
    os << std::endl;

    os << "[Dom tree]:\n";
    node_id = 0;
    for (auto iter = _dom_tree.begin(); iter != _dom_tree.end();
         iter++, node_id++) {
      for (auto iter_vec = iter->begin(); iter_vec != iter->end(); iter_vec++) {
        os << "  n" << node_id << "-->n" << *iter_vec << std::endl;
      }
    }
    os << std::endl;

    os << "[Dom tree pre order]:";
    for (auto iter = _dom_tree_pre_order.begin();
         iter != _dom_tree_pre_order.end(); iter++) {
      os << "n" << *iter;
      if (iter != _dom_tree_pre_order.end() - 1) os << "->";
    }
    os << std::endl;

    os << "[Dom tree pre order index]:";
    node_id = 0;
    for (auto iter = _dom_tree_pre_order.begin();
         iter != _dom_tree_pre_order.end(); iter++) {
      os << "n" << *iter;
      if (iter != _dom_tree_pre_order.end() - 1) os << "->";
    }
    os << std::endl;
  }

  void Print() const {
    Print(std::cout, 0);
    std::cout << std::endl;
  }

  std::string To_str() const {
    std::stringbuf buf;
    std::ostream   os(&buf);
    Print(os, 0);
    return buf.str();
  }

private:
  // builder methods
  UI32_VEC& Post_order(void) { return _post_order; }

  ID_VEC& Rev_post_order(void) { return _rev_post_order; }

  ID_CSET& Dom_frontiers(ID id) {
    AIR_ASSERT(id < _df.size());
    return _df[id];
  }

  ID_CSET& Iter_dom_frontiers(uint32_t id) {
    AIR_ASSERT(id < _iter_df.size());
    return _iter_df[id];
  }

  void Set_post_order(uint32_t idx, uint32_t value) {
    AIR_ASSERT(idx < _post_order.size());
    _post_order[idx] = value;
  }

  template <typename NODE_PTR>
  void Set_post_order(NODE_PTR node, uint32_t value) {
    Set_post_order(node->Id().Value(), value);
  }

  void Set_rev_post_order(uint32_t idx, ID value) {
    AIR_ASSERT(idx < _rev_post_order.size());
    _rev_post_order[idx] = value;
  }

  void Set_imm_dom(ID idx, ID value) {
    AIR_ASSERT(idx < _imm_dom.size());
    _imm_dom[idx] = value;
  }

  template <typename NODE_PTR>
  void Set_imm_dom(NODE_PTR node, NODE_PTR dom_node) {
    Set_imm_dom(node->Id().Value(), dom_node->Id().Value());
  }

  void Insert_dom_frontier(ID id, ID df_id) {
    AIR_ASSERT(id < _df.size());
    ID_CSET& id_set = Dom_frontiers(id);
    id_set.insert(df_id);
  }

  template <typename NODE_PTR>
  void Insert_dom_frontier(NODE_PTR node, NODE_PTR df_node) {
    Insert_dom_frontier(node->Id().Value(), df_node->Id().Value());
  }

  void Insert_iter_dom_frontier(ID id, ID df_id) {
    AIR_ASSERT(id < _iter_df.size());
    ID_CSET& id_set = Iter_dom_frontiers(id);
    id_set.insert(df_id);
  }

  template <typename NODE_PTR>
  void Insert_iter_dom_frontier(NODE_PTR node, NODE_PTR df_node) {
    Insert_dom_frontier(node->Id().Value(), df_node->Id().Value());
  }

  void Insert_iter_dom_frontier(ID id, const ID_CSET& df_set) {
    for (auto df_id : df_set) {
      Insert_iter_dom_frontier(id, df_id);
    }
  }

  void Append_dom_child(ID id, ID child_id) {
    AIR_ASSERT(id < _dom_tree.size());
    _dom_tree[id].push_back(child_id);
  }

  template <typename NODE_PTR>
  void Append_dom_child(NODE_PTR node, NODE_PTR child) {
    Append_dom_child(node->Id().Value(), child->Id().Value());
  }

  void Set_dom_tree_pre_order(uint32_t idx, ID id) {
    AIR_ASSERT(idx < _dom_tree_pre_order.size());
    _dom_tree_pre_order[idx] = id;
  }

  template <typename NODE_PTR>
  void Set_dom_tree_pre_order(uint32_t idx, NODE_PTR node) {
    Set_dom_tree_pre_order(idx, node->Id().Value());
  }

  void Set_dom_tree_pre_idx(ID id, uint32_t seq_idx) {
    AIR_ASSERT(id < _dom_tree_pre_idx.size());
    AIR_ASSERT(seq_idx < _dom_tree_pre_order.size());
    _dom_tree_pre_idx[id] = seq_idx;
  }

private:
  DOM_MPOOL _mpool;            //!< Memory pool
  UI32_VEC  _post_order;       //!< numbering BB's order in post order
                               //!< _post_order[id] = POST Index
  ID_VEC _rev_post_order;      //!< the reverse post order BB sequence
                               //!< _rev_post_order[rev_post_idx] = ID
  ID_VEC _imm_dom;             //!< the immediate dominator of node
                               //!< _imm_dom[id] = ID
  ID_SET_VEC _df;              //!< the dominate frontier set
                               //!< _dom_frontier[id] = {ID,...}
  ID_SET_VEC _iter_df;         //!< iterative dominance frontier set
  ID_VEC_VEC _dom_tree;        //!< the dominate tree
                               //!< _dom_tree[id] = {ID, ...}
  ID_VEC _dom_tree_pre_order;  //!< preorder of dominate tree
                               //!<  _dom_tree_pre_order[idx] = ID
  UI32_VEC _dom_tree_pre_idx;  //!< BB's sequence number in preorder dom
                               //!< tree _dom_tree_pre_idx[id] =seq_idx
};

}  // namespace util
}  // namespace air

#endif
