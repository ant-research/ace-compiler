//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_DFG_CONTAINER_H
#define AIR_OPT_DFG_CONTAINER_H

#include <cstddef>
#include <list>

#include "air/base/arena.h"
#include "air/base/container_decl.h"
#include "air/base/id_wrapper.h"
#include "air/base/ptr_wrapper.h"
#include "air/base/st_decl.h"
#include "air/opt/ssa_container.h"
#include "air/opt/ssa_decl.h"
#include "air/util/debug.h"
#include "air/util/graph_base.h"
#include "dfg_data.h"

namespace air {
namespace opt {

static constexpr uint32_t NODE_TAB_KIND = 0x50001;
static constexpr uint32_t EDGE_TAB_KIND = 0x50002;

//! @brief DFG_CONTAINER contains tables for both DFG node and DFG edge,
//! and provides APIs for accessing and creating DFG node and edge.
class DFG_CONTAINER {
public:
  using DFG_NODE_TAB =
      air::base::ARENA<sizeof(DFG_NODE_DATA), sizeof(DFG_NODE_ID), false>;
  using DFG_EDGE_TAB =
      air::base::ARENA<sizeof(DFG_EDGE_DATA), sizeof(DFG_EDGE_ID), false>;
  using CONTAINER         = air::base::CONTAINER;
  using SSA_CONTAINER     = air::opt::SSA_CONTAINER;
  using NODE_MAP          = std::map<uint32_t, uint32_t>;
  using DFG_NODE_VEC      = std::vector<DFG_NODE_ID>;
  using DFG_NODE_VEC_ITER = DFG_NODE_VEC::const_iterator;
  using MEM_POOL          = air::util::MEM_POOL<4096>;
  using DFG               = util::GRAPH<DFG_CONTAINER>;
  using NODE_ID           = DFG_NODE_ID;
  using NODE_PTR          = DFG_NODE_PTR;
  using EDGE_ID           = DFG_EDGE_ID;
  using EDGE_PTR          = DFG_EDGE_PTR;
  //! @brief Iterator for traversing DFG nodes in the node table (_node_tab).
  class NODE_ITER {
  public:
    NODE_ITER(const DFG_CONTAINER* cntr, DFG_NODE_TAB::ITERATOR iter)
        : _cntr(cntr), _iter(iter) {
      AIR_ASSERT(cntr != nullptr);
    }
    ~NODE_ITER() {}

    DFG_NODE_PTR operator*() const;
    DFG_NODE_PTR operator->() const;
    NODE_ITER&   operator++() {
      ++_iter;
      return *this;
    }
    bool operator==(const NODE_ITER& other) const {
      return _cntr == other.Cntr() && _iter == other.Iter();
    }
    bool operator!=(const NODE_ITER& other) const { return !(*this == other); }

  private:
    // REQUIRED UNDEFINED UNWANTED methods
    NODE_ITER(void);
    NODE_ITER(const NODE_ITER&);
    NODE_ITER& operator=(const NODE_ITER&);

    const DFG_CONTAINER*   Cntr() const { return _cntr; }
    DFG_NODE_TAB::ITERATOR Iter() const { return _iter; }
    const DFG_CONTAINER*   _cntr;
    DFG_NODE_TAB::ITERATOR _iter;
  };

  //! @brief Iterator for traversing DFG edges in the edge table (_edge_tab).
  class EDGE_ITER {
  public:
    EDGE_ITER(const DFG_CONTAINER* cntr, DFG_EDGE_TAB::ITERATOR iter)
        : _cntr(cntr), _iter(iter) {}
    ~EDGE_ITER() {}

    DFG_EDGE_PTR operator*() const;
    DFG_EDGE_PTR operator->() const;
    EDGE_ITER&   operator++() {
      ++_iter;
      return *this;
    }
    bool operator==(const EDGE_ITER& other) const {
      return _cntr == other.Cntr() && _iter == other.Iter();
    }
    bool operator!=(const EDGE_ITER& other) const { return !(*this == other); }

  private:
    // REQUIRED UNDEFINED UNWANTED methods
    EDGE_ITER(void);
    EDGE_ITER(const EDGE_ITER&);
    EDGE_ITER& operator=(const EDGE_ITER&);

    const DFG_CONTAINER*   Cntr() const { return _cntr; }
    DFG_EDGE_TAB::ITERATOR Iter() const { return _iter; }

    const DFG_CONTAINER*   _cntr;
    DFG_EDGE_TAB::ITERATOR _iter;
  };

  friend class DFG_NODE;

  explicit DFG_CONTAINER(const SSA_CONTAINER* ssa_cntr) : _ssa_cntr(ssa_cntr) {
    // create opnd & inst tables
    air::util::CXX_MEM_ALLOCATOR<DFG_NODE_TAB, MEM_POOL> node_a(&_mem_pool);
    _node_tab =
        node_a.Allocate(&_mem_pool, NODE_TAB_KIND, "dfg_node_tab", true);
    air::util::CXX_MEM_ALLOCATOR<DFG_EDGE_TAB, MEM_POOL> edge_a(&_mem_pool);
    _edge_tab =
        edge_a.Allocate(&_mem_pool, EDGE_TAB_KIND, "dfg_edge_tab", true);
  }
  ~DFG_CONTAINER() {}

  DFG_NODE_PTR Node(DFG_NODE_ID id) const;
  DFG_EDGE_PTR Edge(DFG_EDGE_ID id) const;
  DFG_NODE_ID  Node_id(base::NODE_ID expr) const;
  DFG_NODE_ID  Node_id(SSA_VER_ID ver) const;

  //! @brief Return existing DFG node of node,
  //! or create and return a new one if it does not exist.
  DFG_NODE_PTR Get_node(base::NODE_ID node, base::TYPE_ID type,
                        uint32_t opnd_num);
  //! @brief Return existing DFG node of ver,
  //! or create and return a new one if it does not exist.
  DFG_NODE_PTR Get_node(SSA_VER_ID ver, base::TYPE_ID type, uint32_t opnd_num);
  //! @brief Return existing DFG edge linking src and dst,
  //! or create and return a new one if it does not exist.
  DFG_EDGE_PTR Get_edge(DFG_NODE_ID src, DFG_NODE_ID dst);

  DFG_NODE_ID Formal_id(uint32_t id) const {
    AIR_ASSERT_MSG(_formal.size() > id, "formal index out of range");
    AIR_ASSERT_MSG(_formal[id] != base::Null_id, "formal dfg node is invalid");
    return _formal[id];
  }
  DFG_NODE_PTR Formal(uint32_t id) const;
  void         Set_formal(uint32_t id, DFG_NODE_ID formal) {
    AIR_ASSERT_MSG(formal != air::base::Null_id, "Invalid node id");
    AIR_ASSERT_MSG(id <= _formal.size(), "Invalid formal id");
    if (id >= _formal.size()) _formal.resize(id + 1);
    _formal[id] = formal;
  }
  uint32_t Formal_cnt(void) const { return _formal.size(); }

  DFG_NODE_ID  Entry_id(uint32_t id) const { return Formal_id(id); }
  DFG_NODE_PTR Entry(uint32_t id) const;
  void     Set_entry(uint32_t id, DFG_NODE_ID node) { Set_formal(id, node); }
  uint32_t Entry_cnt(void) const { return Formal_cnt(); }

  DFG_NODE_VEC_ITER Begin_retv(void) const { return _retv.begin(); }
  DFG_NODE_VEC_ITER End_retv(void) const { return _retv.end(); }
  void              Add_retv(DFG_NODE_ID retv) {
    AIR_ASSERT_MSG(retv != air::base::Null_id, "Invalid node id");
    for (DFG_NODE_ID id : _retv) {
      AIR_ASSERT_MSG(id != retv, "redundant retv node");
    }
    _retv.push_back(retv);
  }
  uint32_t Retv_cnt(void) const { return _retv.size(); }

  DFG_NODE_VEC_ITER Begin_exit(void) const { return Begin_retv(); }
  DFG_NODE_VEC_ITER End_exit(void) const { return End_retv(); }
  void              Add_exit(DFG_NODE_ID node) { Add_retv(node); }
  uint32_t          Exit_cnt(void) const { return Retv_cnt(); }

  void Set_ssa_cntr(const SSA_CONTAINER* ssa_cntr) { _ssa_cntr = ssa_cntr; }
  const SSA_CONTAINER* Ssa_cntr(void) const { return _ssa_cntr; }
  const CONTAINER*     Cntr(void) const {
    AIR_ASSERT_MSG(_ssa_cntr != nullptr, "SSA container is null");
    return _ssa_cntr->Container();
  }

  NODE_ITER Begin_node(void) const {
    return NODE_ITER(this, _node_tab->Begin());
  }
  NODE_ITER End_node(void) const { return NODE_ITER(this, _node_tab->End()); }
  EDGE_ITER Begin_edge(void) const {
    return EDGE_ITER(this, _edge_tab->Begin());
  }
  EDGE_ITER End_edge(void) const { return EDGE_ITER(this, _edge_tab->End()); }

  size_t Node_cnt(void) const { return _node_tab->Size(); }
  size_t Edge_cnt(void) const { return _edge_tab->Size(); }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  DFG_CONTAINER(void);
  DFG_CONTAINER(const DFG_CONTAINER&);
  DFG_CONTAINER operator=(const DFG_CONTAINER&);

  DFG_NODE_PTR New_node(base::NODE_PTR node, base::TYPE_PTR type,
                        uint32_t opnd_num);
  DFG_NODE_PTR New_node(base::NODE_ID node, base::TYPE_ID type,
                        uint32_t opnd_num);

  DFG_NODE_PTR New_node(SSA_VER_PTR ver, base::TYPE_PTR type,
                        uint32_t opnd_num);
  DFG_NODE_PTR New_node(SSA_VER_ID ver, base::TYPE_ID type, uint32_t opnd_num);
  DFG_NODE_DATA_PTR New_node_data(base::NODE_ID node, base::TYPE_ID type,
                                  uint32_t opnd_num);
  DFG_NODE_DATA_PTR New_node_data(SSA_VER_ID ver, base::TYPE_ID type,
                                  uint32_t opnd_num);
  DFG_EDGE_PTR      New_edge(DFG_NODE_ID dst);

  DFG_NODE_TAB* Node_tab(void) const { return _node_tab; }
  DFG_EDGE_TAB* Edge_tab(void) const { return _edge_tab; }
  void          Record_node(air::base::NODE_ID ir_node, DFG_NODE_ID dfg_node) {
    auto res = _air_node_dfg_node.insert({ir_node.Value(), dfg_node.Value()});
    AIR_ASSERT_MSG(res.second, "Overwriting valid DFG node");
  }
  void Record_node(air::opt::SSA_VER_ID ver, DFG_NODE_ID dfg_node) {
    auto res = _ssa_ver_dfg_node.insert({ver.Value(), dfg_node.Value()});
    AIR_ASSERT_MSG(res.second, "Overwriting valid DFG node");
  }

  MEM_POOL             _mem_pool;  // memory pool for all tables
  const SSA_CONTAINER* _ssa_cntr;
  DFG_NODE_TAB*        _node_tab;
  DFG_EDGE_TAB*        _edge_tab;
  DFG_NODE_VEC         _formal;
  DFG_NODE_VEC         _retv;
  NODE_MAP             _ssa_ver_dfg_node;
  NODE_MAP             _air_node_dfg_node;
};
}  // namespace opt
}  // namespace air

#endif  // AIR_OPT_DFG_CONTAINER_H