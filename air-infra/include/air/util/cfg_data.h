//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_CFG_DATA_H
#define AIR_UTIL_CFG_DATA_H

#include <stdint.h>

#include "air/base/id_wrapper.h"

namespace air {

namespace util {

template <typename N, typename E, typename C>
class CFG;

//! @brief CFG_NODE_INFO
//   integrate into CFG node to record a node's predecessors (_preds) and
//   successors (_succs)
class CFG_NODE_INFO {
  template <typename N, typename E, typename C>
  friend class CFG;

public:
  CFG_NODE_INFO()
      : _preds(air::base::Null_prim_id), _succs(air::base::Null_prim_id) {}

private:
  uint32_t _preds;  //!< EDGE_ID, first predecessor
  uint32_t _succs;  //!< EDGE_ID, first successor

};  // CFG_NODE_INFO

//! @brief LAYOUT_INFO
//   struct to layout basic blocks
class LAYOUT_INFO {
public:
  LAYOUT_INFO()
      : _prev(air::base::Null_prim_id), _next(air::base::Null_prim_id) {}

  uint32_t Prev() const { return _prev; }
  uint32_t Next() const { return _next; }
  void     Set_prev(uint32_t id) { _prev = id; }
  void     Set_next(uint32_t id) { _next = id; }

private:
  uint32_t _prev;  //!< NODE_ID, previous node
  uint32_t _next;  //!< NODE_ID, next node

};  // LAYOUT_INFO

//! @brief BB_DOM_INFO
//   integrate into CFG node to record the node's immediate dominator, nodes
//   dominated by this node, and the node's dominance frontier.
//   same struct also used in Reverse CFG (RCFG) for ipdom/pdom_list/cd_list
class BB_DOM_INFO {
public:
  BB_DOM_INFO()
      : _idom(air::base::Null_prim_id),
        _dom_list(air::base::Null_prim_id),
        _df_list(air::base::Null_prim_id) {}

private:
  uint32_t _idom;      //!< NODE_ID, immediate dominator
  uint32_t _dom_list;  //!< LIST_ID, list of nodes that are dominated
  uint32_t _df_list;   //!< LIST_ID, list of dominance frontier

};  // BB_DOM_INFO

//! @brief CFG_EDGE_INFO
//   CFG edge with predecessor and successor nodes, next edge of preds/succs
class CFG_EDGE_INFO {
  template <typename N, typename E, typename C>
  friend class CFG;

public:
  CFG_EDGE_INFO()
      : _pred(air::base::Null_prim_id),
        _succ(air::base::Null_prim_id),
        _pred_succ_next(air::base::Null_prim_id),
        _succ_pred_next(air::base::Null_prim_id) {}

private:
  uint32_t _pred;            //!< NODE_ID, predecessor of this edge
  uint32_t _succ;            //!< NODE_ID, successor of this edge
  uint32_t _pred_succ_next;  //!< EDGE_ID, next edge of predecessor's successor
  uint32_t _succ_pred_next;  //!< EDGE_ID, next edge of successor's predecessor

};  // CFG_EDGE_INFO

//! @brief NODE_LIST_INFO
//   single linked list for CFG nodes
class NODE_LIST_INFO {
public:
  NODE_LIST_INFO()
      : _node(air::base::Null_prim_id), _next(air::base::Null_prim_id) {}

private:
  uint32_t _node;  //!< NODE_ID, node
  uint32_t _next;  //!< LIST_ID, next node

};  // NODE_LIST_INFO

//! @brief define a macro to help integrate CFG_NODE_INDO to user defined
//! NODE_DATA
#define ENABLE_CFG_DATA_INFO()                                                \
  air::util::CFG_NODE_INFO        _cfg_node_info;                             \
  air::util::CFG_NODE_INFO*       Cfg_node_info() { return &_cfg_node_info; } \
  const air::util::CFG_NODE_INFO* Cfg_node_info() const {                     \
    return &_cfg_node_info;                                                   \
  }

//! @brief define a macro to help integrate CFG_NODE_INDO to user defined
//! NODE_DATA
#define ENABLE_LAYOUT_INFO()                                            \
  air::util::LAYOUT_INFO        _layour_info;                           \
  air::util::LAYOUT_INFO*       Layout_info() { return &_layour_info; } \
  const air::util::LAYOUT_INFO* Layout_info() const { return &_layour_info; }

//! @brief define a macro to help integrate DOM_INDO to user defined NODE_DATA
#define ENABLE_BB_DOM_INFO()                                         \
  air::util::BB_DOM_INFO        _dom_info;                           \
  air::util::BB_DOM_INFO*       Bb_dom_info() { return &_dom_info; } \
  const air::util::BB_DOM_INFO* Bb_dom_info() const { return &_dom_info; }

//! @brief define a macro to help integrate PDOM to user defined NODE_DATA
#define ENABLE_BB_PDOM_INFO()                                       \
  air::util::BB_DOM_INFO        _pdom_info;                         \
  air::util::BB_DOM_INFO*       Pdom_info() { return &_pdom_info; } \
  const air::util::BB_DOM_INFO* Pdom_info() const { return &_pdom_info; }

//! @brief define a macro to help integrate CFG_EDGE_INDO to user defined
//! EDGE_DATA
#define ENABLE_CFG_EDGE_INFO()                                                \
  air::util::CFG_EDGE_INFO        _cfg_edge_info;                             \
  air::util::CFG_EDGE_INFO*       Cfg_edge_info() { return &_cfg_edge_info; } \
  const air::util::CFG_EDGE_INFO* Cfg_edge_info() const {                     \
    return &_cfg_edge_info;                                                   \
  }

}  // namespace util

}  // namespace air

#endif  // AIR_UTIL_CFG_DATA_H
