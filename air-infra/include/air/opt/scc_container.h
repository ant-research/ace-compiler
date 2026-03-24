//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_SCC_CONTAINER_H
#define AIR_OPT_SCC_CONTAINER_H
#include <list>
#include <ostream>

#include "air/base/arena.h"
#include "air/base/arena_core.h"
#include "air/base/id_wrapper.h"
#include "air/base/ptr_wrapper.h"
#include "air/opt/ssa_container.h"
#include "air/util/debug.h"
#include "air/util/graph_base.h"
#include "dfg_data.h"
#include "scc_node.h"

namespace air {
namespace opt {

//! @brief SCC_CONTAINER contains tables for both SCC node and edge,
//! and provides APIs for traversing, finding, and creating SCCs.
class SCC_CONTAINER {
public:
  using SCC_NODE_TAB =
      base::ARENA<sizeof(SCC_NODE_ID), sizeof(SCC_NODE_ID), false>;
  using SCC_EDGE_TAB =
      base::ARENA<sizeof(SCC_EDGE_ID), sizeof(SCC_EDGE_ID), false>;
  using SCC_GRAPH = util::GRAPH<SCC_CONTAINER>;
  using NODE_MAP  = std::map<uint32_t, SCC_NODE_ID>;
  using NODE_ID   = SCC_NODE_ID;
  using NODE_PTR  = SCC_NODE_PTR;
  using EDGE_ID   = SCC_EDGE_ID;
  using EDGE_PTR  = SCC_EDGE_PTR;
  //! @brief Iterator for traversing SCC node table (_node_tab).
  class NODE_ITER {
  public:
    NODE_ITER(const SCC_CONTAINER* cntr, SCC_NODE_TAB::ITERATOR tab_iter)
        : _cntr(cntr), _iter(tab_iter) {}
    ~NODE_ITER() {}

    SCC_NODE_PTR operator*() const;
    SCC_NODE_PTR operator->() const;
    NODE_ITER&   operator++();
    bool         operator==(const NODE_ITER& other) const;
    bool         operator!=(const NODE_ITER& other) const;

  private:
    // REQUIRED UNDEFINED UNWANTED methods
    NODE_ITER(void);
    NODE_ITER(const NODE_ITER&);
    NODE_ITER operator=(const NODE_ITER&);

    const SCC_CONTAINER*   _cntr;
    SCC_NODE_TAB::ITERATOR _iter;
  };

  //! @brief Iterator for traversing SCC edge table (_edge_tab).
  class EDGE_ITER {
  public:
    EDGE_ITER(const SCC_CONTAINER* cntr, SCC_EDGE_TAB::ITERATOR tab_iter)
        : _cntr(cntr), _iter(tab_iter) {}
    ~EDGE_ITER() {}

    SCC_EDGE_PTR operator*() const;
    SCC_EDGE_PTR operator->() const;
    EDGE_ITER&   operator++();
    bool         operator==(const EDGE_ITER& other) const;
    bool         operator!=(const EDGE_ITER& other) const;

  private:
    // REQUIRED UNDEFINED UNWANTED methods
    EDGE_ITER(void);
    EDGE_ITER(const EDGE_ITER&);
    EDGE_ITER& operator=(const EDGE_ITER&);

    const SCC_CONTAINER*   _cntr;
    SCC_EDGE_TAB::ITERATOR _iter;
  };

  static constexpr uint32_t NODE_TAB_KIND = 0x50001;
  static constexpr uint32_t EDGE_TAB_KIND = 0x50002;

  template <typename GRAPH>
  friend class SCC_BUILDER;
  friend class SCC_NODE;

  SCC_CONTAINER(void) {
    // create node & edge tables
    air::util::CXX_MEM_ALLOCATOR<SCC_NODE_TAB, SCC_MEMPOOL> node_a(&_mem_pool);
    _node_tab = node_a.Allocate(&_mem_pool, NODE_TAB_KIND, "node_tab", true);
    air::util::CXX_MEM_ALLOCATOR<SCC_EDGE_TAB, SCC_MEMPOOL> edge_a(&_mem_pool);
    _edge_tab = edge_a.Allocate(&_mem_pool, EDGE_TAB_KIND, "edge_tab", true);
  }
  ~SCC_CONTAINER() {}

  SCC_NODE_PTR Node(SCC_NODE_ID node) const;
  SCC_EDGE_PTR Edge(SCC_EDGE_ID edge) const;
  uint32_t     Node_cnt(void) const { return Node_tab()->Size(); }
  uint32_t     Edge_cnt(void) const { return Edge_tab()->Size(); }

  template <typename ELEM_ID>
  SCC_NODE_ID Scc_node(ELEM_ID elem) const {
    NODE_MAP::const_iterator iter = _elem2scc.find(elem.Value());
    if (iter == _elem2scc.end()) return SCC_NODE_ID(base::Null_id);
    return iter->second;
  }
  template <typename ELEM_ID>
  void Set_scc_node(SCC_NODE_ID scc_node, ELEM_ID elem) {
    _elem2scc[elem.Value()] = scc_node;
  }

  template <typename ELEM_CONTAINER>
  void Print_dot(const ELEM_CONTAINER* cntr, const char* file_name) const {
    std::ofstream file(file_name, std::ios_base::out);
    file << "digraph {" << std::endl;
    for (SCC_NODE_TAB::ITERATOR iter = Node_tab()->Begin();
         iter != Node_tab()->End(); ++iter) {
      SCC_NODE_PTR scc_node = Node(SCC_NODE_ID(*iter));
      scc_node->Print_dot(cntr, file, 4);
    }
    file << "}" << std::endl;
  }

  NODE_ITER Begin_node() const { return NODE_ITER(this, _node_tab->Begin()); }
  NODE_ITER End_node() const { return NODE_ITER(this, _node_tab->End()); }
  EDGE_ITER Begin_edge() const { return EDGE_ITER(this, _edge_tab->Begin()); }
  EDGE_ITER End_edge() const { return EDGE_ITER(this, _edge_tab->End()); }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  SCC_CONTAINER(const SCC_CONTAINER&);
  SCC_CONTAINER operator=(const SCC_CONTAINER&);

  SCC_NODE_PTR New_scc();
  SCC_EDGE_PTR New_edge(SCC_NODE_ID dst);

  SCC_NODE_TAB* Node_tab() const { return _node_tab; }
  SCC_EDGE_TAB* Edge_tab() const { return _edge_tab; }

  SCC_NODE_TAB* _node_tab;
  SCC_EDGE_TAB* _edge_tab;
  SCC_MEMPOOL   _mem_pool;
  NODE_MAP      _elem2scc;
};

}  // namespace opt
}  // namespace air
#endif