//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_DFG_REGION_CONTAINER_H
#define FHE_CKKS_DFG_REGION_CONTAINER_H

#include <algorithm>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "air/base/arena.h"
#include "air/base/arena_core.h"
#include "air/base/id_wrapper.h"
#include "air/base/st.h"
#include "air/base/st_decl.h"
#include "air/opt/dfg_container.h"
#include "air/opt/dfg_data.h"
#include "air/opt/dfg_node.h"
#include "air/opt/scc_container.h"
#include "air/opt/scc_node.h"
#include "air/opt/ssa_container.h"
#include "air/util/debug.h"
#include "air/util/mem_allocator.h"
#include "dfg_region.h"
#include "fhe/core/lower_ctx.h"

namespace fhe {
namespace ckks {

class REGION_CONTAINER {
public:
  using GRAPH         = air::util::GRAPH<REGION_CONTAINER>;
  using FUNC_ID       = air::base::FUNC_ID;
  using FUNC_SCOPE    = air::base::FUNC_SCOPE;
  using GLOB_SCOPE    = air::base::GLOB_SCOPE;
  using SSA_CONTAINER = air::opt::SSA_CONTAINER;
  using DFG_CONTAINER = air::opt::DFG_CONTAINER;
  using SCC_CONTAINER = air::opt::SCC_CONTAINER;
  using DFG           = DFG_CONTAINER::DFG;
  using SSA_CNTR_MAP  = std::map<FUNC_ID, std::unique_ptr<SSA_CONTAINER> >;
  using DFG_CNTR_MAP  = std::map<FUNC_ID, std::unique_ptr<DFG_CONTAINER> >;
  using SCC_CNTR_MAP  = std::map<FUNC_ID, std::unique_ptr<SCC_CONTAINER> >;
  using ELEM_REGION   = std::map<REGION_ELEM, REGION_ID>;
  using ELEM_ID_MAP   = std::map<REGION_ELEM_DATA, REGION_ELEM_ID>;
  using ELEM_ID_VEC   = std::vector<REGION_ELEM_ID>;
  using REGION_TAB =
      air::base::ARENA<sizeof(REGION_DATA), sizeof(REGION_ID), false>;
  using ELEM_TAB =
      air::base::ARENA<sizeof(REGION_ELEM), sizeof(REGION_ELEM_ID), false>;
  using EDGE_TAB =
      air::base::ARENA<sizeof(REGION_EDGE), sizeof(REGION_EDGE_ID), false>;

  using NODE_ID  = REGION_ELEM_ID;
  using NODE_PTR = REGION_ELEM_PTR;
  using EDGE_ID  = REGION_EDGE_ID;
  using EDGE_PTR = REGION_EDGE_PTR;

  class NODE_ITER {
  public:
    NODE_ITER(const REGION_CONTAINER* cntr, ELEM_TAB::ITERATOR iter)
        : _cntr(cntr), _iter(iter) {}
    REGION_ELEM_PTR operator*() const {
      return _cntr->Region_elem(REGION_ELEM_ID(*_iter));
    }
    REGION_ELEM_PTR operator->() const { return *(*this); }
    NODE_ITER&      operator++() {
      ++_iter;
      return *this;
    }
    bool operator==(const NODE_ITER& other) const {
      return _cntr == other._cntr && _iter == other._iter;
    }
    bool operator!=(const NODE_ITER& other) const { return !(*this == other); }

  private:
    const REGION_CONTAINER* _cntr;
    ELEM_TAB::ITERATOR      _iter;
  };
  class EDGE_ITER {
  public:
    EDGE_ITER(const REGION_CONTAINER* cntr, EDGE_TAB::ITERATOR iter)
        : _cntr(cntr), _iter(iter) {}
    REGION_EDGE_PTR operator*() const {
      return _cntr->Elem_edge(REGION_EDGE_ID(*_iter));
    }
    REGION_EDGE_PTR operator->() const { return *(*this); }
    EDGE_ITER&      operator++() {
      ++_iter;
      return *this;
    }
    bool operator==(const EDGE_ITER& other) const {
      return (_cntr == other._cntr && _iter == other._iter);
    }
    bool operator!=(const EDGE_ITER& other) const { return !(*this == other); }

  private:
    const REGION_CONTAINER* _cntr;
    EDGE_TAB::ITERATOR      _iter;
  };
  //! @brief Iterator for traversing regions in region table(_region_tab).
  class REGION_ITER {
  public:
    REGION_ITER(const REGION_CONTAINER* cntr, REGION_TAB::ITERATOR iter)
        : _cntr(cntr), _iter(iter) {}
    REGION_PTR operator*() const {
      REGION_ID id(*_iter);
      AIR_ASSERT_MSG(id != air::base::Null_id, "invalid region index");
      return _cntr->Region(id);
    }
    REGION_PTR operator->() const {
      REGION_ID id(*_iter);
      AIR_ASSERT_MSG(id != air::base::Null_id, "invalid region index");
      return _cntr->Region(id);
    }
    REGION_ITER& operator++() {
      ++_iter;
      return *this;
    }
    bool operator==(const REGION_ITER& other) const {
      return _cntr == other->Cntr() && _iter == other.Iter();
    }
    bool operator!=(const REGION_ITER& other) const {
      return !(*this == other);
    }

  private:
    // REQUIRED UNDEFINED UNWANTED methods
    REGION_ITER(void);
    REGION_ITER(const REGION_ITER&);
    REGION_ITER& operator=(const REGION_ITER&);

    const REGION_CONTAINER*     Cntr() const { return _cntr; }
    const REGION_TAB::ITERATOR& Iter() const { return _iter; }
    REGION_TAB::ITERATOR&       Iter() { return _iter; }

    const REGION_CONTAINER* _cntr;
    REGION_TAB::ITERATOR    _iter;
  };

  friend class REGION_ELEM;
  friend class REGION;
  friend class REGION_BUILDER;
  constexpr static uint32_t REGION_TAB_KIND = 0x50001;
  constexpr static uint32_t ELEM_TAB_KIND   = 0x50002;
  constexpr static uint32_t EDGE_TAB_KIND   = 0x50003;

  REGION_CONTAINER(air::base::GLOB_SCOPE* glob_scope,
                   core::LOWER_CTX*       lower_ctx)
      : _glob_scope(glob_scope), _lower_ctx(lower_ctx) {
    air::util::CXX_MEM_ALLOCATOR<REGION_TAB, MEMPOOL> alloc(&_mem_pool);
    _region_tab =
        alloc.Allocate(&_mem_pool, REGION_TAB_KIND, "region_tab", true);
    air::util::CXX_MEM_ALLOCATOR<ELEM_TAB, MEMPOOL> elem_alloc(&_mem_pool);
    _elem_tab =
        elem_alloc.Allocate(&_mem_pool, ELEM_TAB_KIND, "elem_tab", true);
    air::util::CXX_MEM_ALLOCATOR<EDGE_TAB, MEMPOOL> edge_alloc(&_mem_pool);
    _edge_tab =
        edge_alloc.Allocate(&_mem_pool, EDGE_TAB_KIND, "edge_tab", true);
  }
  ~REGION_CONTAINER() {}

  REGION_PTR Region(REGION_ID id) const {
    if (id == air::base::Null_id) {
      AIR_ASSERT_MSG(false, "invalid region index");
    }
    AIR_ASSERT_MSG(id.Value() < Region_cnt(), "region index out of range");
    REGION_DATA_PTR data = Region_tab()->Find(id);
    return REGION_PTR(REGION(const_cast<REGION_CONTAINER*>(this), data));
  }

  REGION_ELEM_PTR Region_elem(REGION_ELEM_ID elem_id) const {
    AIR_ASSERT_MSG(elem_id != air::base::Null_id, "invalid element index");
    AIR_ASSERT_MSG(elem_id.Value() < Elem_cnt(), "element index out of range");
    REGION_ELEM_DATA_PTR data = _elem_tab->Find(elem_id);
    return REGION_ELEM_PTR(REGION_ELEM(this, data));
  }

  REGION_EDGE_PTR Elem_edge(REGION_EDGE_ID edge_id) const {
    AIR_ASSERT_MSG(edge_id != air::base::Null_id, "invalid edge index");
    AIR_ASSERT_MSG(edge_id.Value() < Edge_cnt(), "edge index out of range");
    REGION_EDGE_DATA_PTR data = _edge_tab->Find(edge_id);
    return REGION_EDGE_PTR(REGION_EDGE(this, data));
  }

  REGION_ELEM_ID Elem_id(const REGION_ELEM_DATA& data) const {
    ELEM_ID_MAP::const_iterator iter = _elem2id.find(data);
    if (iter == _elem2id.end()) return REGION_ELEM_ID();
    return iter->second;
  }

  REGION_ID Region_id(const REGION_ELEM_DATA& data) const {
    REGION_ELEM_ID elem_id = Elem_id(data);
    if (elem_id == air::base::Null_id) return REGION_ID();
    return Region_elem(elem_id)->Region();
  }

  uint32_t Region_cnt(void) const { return _region_tab->Size(); }
  uint32_t Elem_cnt(void) const { return _elem_tab->Size(); }
  uint32_t Edge_cnt(void) const { return _edge_tab->Size(); }

  REGION_ELEM_ID Get_elem(air::opt::DFG_NODE_ID node, uint32_t pred_cnt,
                          air::base::FUNC_ID callee, air::base::FUNC_ID caller,
                          air::opt::DFG_NODE_ID call_site);
  REGION_ELEM_ID Get_elem(air::opt::DFG_NODE_ID node, uint32_t pred_cnt,
                          const CALLSITE_INFO& call_info);
  REGION_PTR     Get_region(REGION_ID id) {
    AIR_ASSERT_MSG(id != air::base::Null_id, "invalid region index");
    while (Region_cnt() <= id.Value()) {
      New_region();
    }
    REGION_DATA_PTR data = _region_tab->Find(id);
    return REGION_PTR(REGION(this, data));
  }

  SSA_CONTAINER* Get_ssa_cntr(FUNC_SCOPE* func_scope) {
    FUNC_ID                func = func_scope->Owning_func_id();
    SSA_CNTR_MAP::iterator iter = _ssa_cntr.find(func);
    if (iter == _ssa_cntr.end()) {
      iter = _ssa_cntr
                 .emplace(func, std::make_unique<SSA_CONTAINER>(
                                    &func_scope->Container()))
                 .first;
    }
    return iter->second.get();
  }

  const SSA_CONTAINER* Ssa_cntr(FUNC_ID func) const {
    SSA_CNTR_MAP::const_iterator iter = _ssa_cntr.find(func);
    AIR_ASSERT_MSG(iter != _ssa_cntr.end(), "not find SSA_CONTAINER");
    return iter->second.get();
  }

  DFG_CONTAINER* Get_dfg_cntr(FUNC_ID func) {
    DFG_CNTR_MAP::iterator iter = _dfg_cntr.find(func);
    if (iter == _dfg_cntr.end()) {
      iter = _dfg_cntr
                 .emplace(func, std::make_unique<DFG_CONTAINER>(Ssa_cntr(func)))
                 .first;
    }
    return iter->second.get();
  }

  const DFG_CONTAINER* Dfg_cntr(FUNC_ID func) const {
    DFG_CNTR_MAP::const_iterator iter = _dfg_cntr.find(func);
    if (iter == _dfg_cntr.end()) AIR_ASSERT_MSG(false, "");
    return iter->second.get();
  }

  const SCC_CONTAINER* Scc_cntr() const { return &_scc_cntr; }
  SCC_CONTAINER*       Scc_cntr() { return &_scc_cntr; }

  REGION_ITER Begin_region(void) const {
    return REGION_ITER(this, _region_tab->Begin());
  }
  REGION_ITER End_region(void) const {
    return REGION_ITER(this, _region_tab->End());
  }
  void Print_dot(const char* file_name) const;
  void Print_dot(const char* file_name, REGION_ID region) const;

  void                   Set_entry_func(FUNC_ID entry) { _entry_func = entry; }
  FUNC_ID                Entry_func(void) const { return _entry_func; }
  const GLOB_SCOPE*      Glob_scope(void) const { return _glob_scope; }
  const core::LOWER_CTX* Lower_ctx(void) const { return _lower_ctx; }

  NODE_PTR  Node(REGION_ELEM_ID id) const { return Region_elem(id); }
  NODE_ITER Begin_node(void) const {
    return NODE_ITER(this, _elem_tab->Begin());
  }
  NODE_ITER End_node(void) const { return NODE_ITER(this, _elem_tab->End()); }
  EDGE_ITER Begin_edge(void) const {
    return EDGE_ITER(this, _edge_tab->Begin());
  }
  EDGE_ITER End_edge(void) const { return EDGE_ITER(this, _edge_tab->End()); }

  uint32_t        Entry_cnt(void) const { return _entry.size(); }
  REGION_ELEM_PTR Entry(uint32_t id) const {
    AIR_ASSERT_MSG(id < _entry.size(), "entry id out of range");
    AIR_ASSERT(_entry[id] != air::base::Null_id);
    return Region_elem(_entry[id]);
  }
  void            Add_entry(REGION_ELEM_ID elem) { _entry.push_back(elem); }
  uint32_t        Exit_cnt(void) const { return _exit.size(); }
  REGION_ELEM_PTR Exit(uint32_t id) const {
    AIR_ASSERT_MSG(id < _exit.size(), "exit id out of range");
    AIR_ASSERT(_exit[id] != air::base::Null_id);
    return Region_elem(_exit[id]);
  }
  void Add_exit(REGION_ELEM_ID elem) { _exit.push_back(elem); }

  void Set_scc_region(air::opt::SCC_NODE_ID scc_node_id, REGION_ID region) {
    AIR_ASSERT(scc_node_id != air::base::Null_id);
    if (_scc2region.empty()) {
      _scc2region.resize(_scc_cntr.Node_cnt(), air::base::Null_id);
    }
    _scc2region[scc_node_id.Value()] = region;
  }

  REGION_ID Scc_region(air::opt::SCC_NODE_ID scc_node_id) const {
    AIR_ASSERT(scc_node_id != air::base::Null_id);
    AIR_ASSERT_MSG(scc_node_id.Value() < _scc2region.size(),
                   "SCC node ID out of range");
    return _scc2region[scc_node_id.Value()];
  }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  REGION_CONTAINER(void);
  REGION_CONTAINER(const REGION_CONTAINER&);
  REGION_CONTAINER operator=(const REGION_CONTAINER&);

  REGION_PTR      New_region(void);
  MEMPOOL&        Mem_pool(void) { return _mem_pool; }
  const MEMPOOL&  Mem_pool(void) const { return _mem_pool; }
  REGION_DATA_PTR New_region_data(void);
  REGION_ELEM_PTR New_elem(air::opt::DFG_NODE_ID node, uint32_t pred_cnt,
                           air::base::FUNC_ID callee, air::base::FUNC_ID caller,
                           air::opt::DFG_NODE_ID call_site);
  REGION_EDGE_PTR New_edge(REGION_ELEM_ID dst) const;
  REGION_TAB*     Region_tab(void) const { return _region_tab; }
  ELEM_TAB*       Elem_tab(void) const { return _elem_tab; }
  EDGE_TAB*       Edge_tab(void) const { return _edge_tab; }
  ELEM_ID_MAP&    Elem_id(void) { return _elem2id; }

  const GLOB_SCOPE*      _glob_scope;
  const core::LOWER_CTX* _lower_ctx;
  REGION_TAB*            _region_tab;
  ELEM_TAB*              _elem_tab;
  EDGE_TAB*              _edge_tab;
  ELEM_REGION            _elem_region;
  ELEM_ID_MAP            _elem2id;
  ELEM_ID_VEC            _entry;
  ELEM_ID_VEC            _exit;
  SSA_CNTR_MAP           _ssa_cntr;
  DFG_CNTR_MAP           _dfg_cntr;
  SCC_CONTAINER          _scc_cntr;
  std::vector<REGION_ID> _scc2region;
  FUNC_ID                _entry_func;
  MEMPOOL                _mem_pool;
};

}  // namespace ckks
}  // namespace fhe

#endif  // FHE_CKKS_DFG_REGION_CONTAINER_H