//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_SCC_BUILDER_H
#define AIR_OPT_SCC_BUILDER_H

#include <map>
#include <stack>

#include "air/opt/dfg_node.h"
#include "air/opt/scc_container.h"
#include "air/opt/scc_node.h"

namespace air {
namespace opt {

//! @brief SCC_BUILDER utilizes Tarjan's algorithm to identify strongly
//! connected components (SCCs) in a directed graph (_graph), storing the
//! resulting SCCs in _scc_graph.
//! _node_id records the discovery time of each node.
//! _node_low_id records the minimum _id of the nodes that can be reached from
//! current node.
template <typename GRAPH>
class SCC_BUILDER {
public:
  using SCC_GRAPH                      = util::GRAPH<SCC_CONTAINER>;
  using ELEM_ID                        = typename GRAPH::NODE_ID;
  using ELEM_PTR                       = typename GRAPH::NODE_PTR;
  using ELEM_ITER                      = typename GRAPH::NODE_ITER;
  using ELEM_EDGE_ID                   = typename GRAPH::EDGE_ID;
  using ELEM_EDGE_PTR                  = typename GRAPH::EDGE_PTR;
  using ELEM_EDGE_ITER                 = typename GRAPH::EDGE_ITER;
  using ID_MAP                         = std::map<ELEM_ID, uint32_t>;
  constexpr static uint32_t INVALID_ID = UINT32_MAX;

  SCC_BUILDER(const GRAPH* graph, SCC_GRAPH* scc_graph)
      : _graph(graph), _scc_graph(scc_graph) {}
  ~SCC_BUILDER() {}

  //! @brief Traverse each node lacking predecessors and construct SCCs
  //! reachable from these nodes.
  void Perform() {
    // 1. Create SCCs
    for (ELEM_ITER iter = _graph->Begin_node(); iter != _graph->End_node();
         ++iter) {
      iter->Set_trav_state(TRAV_STATE_RAW);
    }

    for (uint32_t id = 0; id < _graph->Container().Entry_cnt(); ++id) {
      ELEM_PTR entry = _graph->Container().Entry(id);
      Visit(entry);
    }
    for (ELEM_ITER iter = _graph->Begin_node(); iter != _graph->End_node();
         ++iter) {
      if (iter->Has_valid_pred()) continue;
      if (Id(iter->Id()) != INVALID_ID) continue;
      Visit(*iter);
    }

    // 2. Link SCCs
    Link_scc();
  }

private:
  void     Set_id(ELEM_ID node, uint32_t id) { _node_id[node] = id; }
  uint32_t Id(ELEM_ID node) {
    typename ID_MAP::iterator iter = _node_id.find(node);
    if (iter == _node_id.end()) return INVALID_ID;
    return iter->second;
  }
  void     Set_low_id(ELEM_ID node, uint32_t id) { _node_low_id[node] = id; }
  uint32_t Low_id(ELEM_ID node) {
    typename ID_MAP::iterator iter = _node_low_id.find(node);
    if (iter == _node_id.end()) return INVALID_ID;
    return iter->second;
  }

  //! @brief Visit is a recursive function that finds and constructs SCCs
  //! using DFS traversal.
  void Visit(ELEM_PTR node);
  void Link_scc(void);

  ELEM_PTR Top() { return _node_stack.top(); }
  void     Push(ELEM_PTR node) { _node_stack.push(node); }
  void     Pop() {
    AIR_ASSERT(!_node_stack.empty());
    _node_stack.pop();
  }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  SCC_BUILDER(void);
  SCC_BUILDER(const SCC_BUILDER&);
  SCC_BUILDER& operator=(const SCC_BUILDER&);

  SCC_CONTAINER& Scc_cntr() const { return _scc_graph->Container(); }

  const GRAPH*         _graph;
  SCC_GRAPH*           _scc_graph;
  std::stack<ELEM_PTR> _node_stack;
  ID_MAP               _node_id;
  ID_MAP               _node_low_id;
  uint32_t             _id = 0;
};

//! @brief A recursive function that finds and constructs SCCs
//! using DFS traversal.
template <typename GRAPH>
void SCC_BUILDER<GRAPH>::Visit(ELEM_PTR node) {
  // 1. Initialize discovery time of node
  Set_id(node->Id(), _id);
  Set_low_id(node->Id(), _id);
  Push(node);
  node->Set_trav_state(TRAV_STATE_PROCESSING);
  ++_id;

  // 2. Traverse successors of node
  for (auto iter = node->Begin_succ(); iter != node->End_succ(); ++iter) {
    ELEM_PTR dst = iter->Dst();
    // If dst has not been visited yet, recursively proceed with it.
    if (Id(dst->Id()) == INVALID_ID) {
      Visit(dst);
      uint32_t low_id = std::min(Low_id(node->Id()), Low_id(dst->Id()));
      Set_low_id(node->Id(), low_id);
    } else if (dst->Trav_state() == TRAV_STATE_PROCESSING) {
      uint32_t low_id = std::min(Low_id(node->Id()), Id(dst->Id()));
      Set_low_id(node->Id(), low_id);
    }
  }

  // 3. Pop node stack and construct SCCs.
  if (Low_id(node->Id()) == Id(node->Id())) {
    SCC_NODE_PTR scc_node = _scc_graph->Container().New_scc();
    while (true) {
      ELEM_PTR sibling = Top();
      Pop();
      sibling->Set_trav_state(TRAV_STATE_VISITED);
      Scc_cntr().Set_scc_node(scc_node->Id(), sibling->Id());
      scc_node->Add_elem(sibling->Id());
      if (sibling == node) break;
    }
  }
}

template <typename GRAPH>
void SCC_BUILDER<GRAPH>::Link_scc() {
  for (ELEM_ITER iter = _graph->Begin_node(); iter != _graph->End_node();
       ++iter) {
    SCC_NODE_ID  scc_node_id = Scc_cntr().Scc_node(iter->Id());
    SCC_NODE_PTR scc_node    = Scc_cntr().Node(scc_node_id);
    for (auto elem_iter = iter->Begin_succ(); elem_iter != iter->End_succ();
         ++elem_iter) {
      ELEM_ID     dst           = elem_iter->Dst_id();
      SCC_NODE_ID succ_scc_node = Scc_cntr().Scc_node(dst);
      AIR_ASSERT_MSG(succ_scc_node != base::Null_id, "invalid SCC node index");
      if (succ_scc_node == scc_node_id) continue;
      scc_node->Add_succ(succ_scc_node);
    }
  }
}

}  // namespace opt
}  // namespace air
#endif  // AIR_OPT_SCC_BUILDER_H