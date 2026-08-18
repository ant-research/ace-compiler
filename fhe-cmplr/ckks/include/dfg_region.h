//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_DFG_REGION_H
#define FHE_CKKS_DFG_REGION_H

#include "air/opt/dfg_container.h"
#include "air/opt/dfg_node.h"
#include "air/opt/scc_container.h"
#include "air/opt/scc_node.h"
#include "air/opt/ssa_container.h"

namespace fhe {
namespace ckks {

class REGION_ELEM_DATA;
using REGION_ELEM_ID       = air::base::ID<REGION_ELEM_DATA>;
using REGION_ELEM_DATA_PTR = air::base::PTR_FROM_DATA<REGION_ELEM_DATA>;
class REGION_ELEM;
using REGION_ELEM_PTR       = air::base::PTR<REGION_ELEM>;
using CONST_REGION_ELEM_PTR = const REGION_ELEM_PTR&;

class REGION_EDGE_DATA;
using REGION_EDGE_ID       = air::base::ID<REGION_EDGE_DATA>;
using REGION_EDGE_DATA_PTR = air::base::PTR_FROM_DATA<REGION_EDGE_DATA>;
class REGION_EDGE;
using REGION_EDGE_PTR       = air::base::PTR<REGION_EDGE>;
using CONST_REGION_EDGE_PTR = const REGION_EDGE_PTR&;

class REGION_DATA;
using REGION_ID       = air::base::ID<REGION_DATA>;
using REGION_DATA_PTR = air::base::PTR_FROM_DATA<REGION_DATA>;
class REGION;
using REGION_PTR       = air::base::PTR<REGION>;
using CONST_REGION_PTR = const REGION_PTR&;

using MEMPOOL      = air::util::MEM_POOL<4096>;
using REGION_ALLOC = air::util::CXX_MEM_ALLOCATOR<REGION_ID, MEMPOOL>;
using ELEM_ALLOC   = air::util::CXX_MEM_ALLOCATOR<REGION_ELEM_ID, MEMPOOL>;
using ELEM_LIST    = std::list<REGION_ELEM_ID, ELEM_ALLOC>;

enum ELEM_ATTR : uint32_t {
  ELEM_ATTR_RESCALE   = 0,
  ELEM_ATTR_BOOTSTRAP = 1,
  ELEM_ATTR_LAST      = 2,
};

//! @brief REGION_EDGE_DATA contains the ID of dst node and the ID of the next
//! edge. The next edge links the src node and its next use-site.
class REGION_EDGE_DATA {
public:
  REGION_EDGE_DATA(REGION_ELEM_ID dst) : _dst(dst), _next() {}
  ~REGION_EDGE_DATA() {}

  REGION_ELEM_ID Dst(void) const { return _dst; }
  REGION_EDGE_ID Next(void) const { return _next; }
  void           Set_next(REGION_EDGE_ID next) { _next = next; }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  REGION_EDGE_DATA(void);
  REGION_EDGE_DATA(const REGION_EDGE_DATA&);
  REGION_EDGE_DATA& operator=(const REGION_EDGE_DATA&);

  REGION_ELEM_ID _dst;
  REGION_EDGE_ID _next;
};

class REGION_CONTAINER;
class REGION_EDGE {
public:
  REGION_EDGE(const REGION_CONTAINER* cntr, REGION_EDGE_DATA_PTR data)
      : _cntr(cntr), _data(data) {}
  ~REGION_EDGE() {}

  REGION_EDGE_ID  Id(void) const { return _data.Id(); }
  REGION_ELEM_ID  Dst_id(void) const { return _data->Dst(); }
  REGION_ELEM_PTR Dst(void) const;
  REGION_EDGE_ID  Next_id(void) const { return _data->Next(); }
  REGION_EDGE_PTR Next(void) const;
  void            Set_next(REGION_EDGE_ID next) { _data->Set_next(next); }

private:
  const REGION_CONTAINER* Cntr(void) const { return _cntr; }

  const REGION_CONTAINER* _cntr;
  REGION_EDGE_DATA_PTR    _data;
};

class CALLSITE_INFO {
public:
  using FUNC_ID     = air::base::FUNC_ID;
  using DFG_NODE_ID = air::opt::DFG_NODE_ID;
  CALLSITE_INFO(FUNC_ID caller, FUNC_ID callee, DFG_NODE_ID node)
      : _caller(caller), _callee(callee), _call_node(node) {}
  explicit CALLSITE_INFO(air::base::FUNC_ID callee)
      : _caller(), _callee(callee), _call_node(air::base::Null_id) {
    AIR_ASSERT(!callee.Is_null());
  }
  CALLSITE_INFO(void) : _caller(), _callee(), _call_node(air::base::Null_id) {}
  CALLSITE_INFO(const CALLSITE_INFO& other)
      : CALLSITE_INFO(other.Caller(), other.Callee(), other.Node()) {}
  ~CALLSITE_INFO() {}

  FUNC_ID     Caller(void) const { return _caller; }
  FUNC_ID     Callee(void) const { return _callee; }
  DFG_NODE_ID Node(void) const { return _call_node; }
  bool        Is_invalid(void) const {
    return _call_node == air::base::Null_id || _callee.Is_null();
  }
  bool operator<(const CALLSITE_INFO& o) const {
    if (_caller < o.Caller()) return true;
    if (_caller > o.Caller()) return false;
    if (_callee < o.Callee()) return true;
    if (_callee > o.Callee()) return false;
    return _call_node < o.Node();
  }
  bool operator==(const CALLSITE_INFO& other) const {
    return _caller == other.Caller() && _callee == other.Callee() &&
           _call_node == other.Node();
  }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  CALLSITE_INFO& operator=(const CALLSITE_INFO&);

  FUNC_ID     _caller;
  FUNC_ID     _callee;
  DFG_NODE_ID _call_node;  // DFG node of call node
};

class REGION_ELEM_DATA {
public:
  REGION_ELEM_DATA(air::opt::DFG_NODE_ID node, uint32_t pred_cnt,
                   air::base::FUNC_ID callee, air::base::FUNC_ID caller,
                   air::opt::DFG_NODE_ID call_site)
      : _dfg_node(node),
        _pred_cnt(pred_cnt),
        _call_site_info(caller, callee, call_site),
        _region(air::base::Null_id) {
    _attr._val = 0;
    for (uint32_t id = 0; id < _pred_cnt; ++id) {
      _pred[id] = air::base::Null_id;
    }
  }
  REGION_ELEM_DATA(air::opt::DFG_NODE_ID node, uint32_t pred_cnt,
                   air::base::FUNC_ID callee, air::base::FUNC_ID caller)
      : REGION_ELEM_DATA(node, pred_cnt, callee, caller,
                         air::opt::DFG_NODE_ID()) {}
  ~REGION_ELEM_DATA() {}

  air::opt::DFG_NODE_ID Dfg_node(void) const { return _dfg_node; }
  const CALLSITE_INFO&  Callsite_info(void) const { return _call_site_info; }
  air::base::FUNC_ID    Caller(void) const { return _call_site_info.Caller(); }
  air::base::FUNC_ID    Callee(void) const { return _call_site_info.Callee(); }
  air::opt::DFG_NODE_ID Call_node(void) const { return _call_site_info.Node(); }
  REGION_ID             Region(void) const { return _region; }
  REGION_EDGE_ID        Succ(void) const { return _succ; }
  void                  Set_succ(REGION_EDGE_ID succ) { _succ = succ; }
  void                  Set_region(REGION_ID region) { _region = region; }
  uint32_t              Pred_cnt(void) { return _pred_cnt; }
  void                  Set_pred(uint32_t id, REGION_ELEM_ID pred) {
    AIR_ASSERT_MSG(id < _pred_cnt, "pred index out of range");
    _pred[id] = pred;
  }
  REGION_ELEM_ID Pred(uint32_t id) const {
    AIR_ASSERT_MSG(id < _pred_cnt, "pred index out of range");
    return _pred[id];
  }
  air::opt::TRAV_STATE Trav_state(void) const { return _trav_stat; }
  void Set_trav_state(air::opt::TRAV_STATE state) { _trav_stat = state; }

  void Set_need_rescale(bool val) { _attr._var._rescale = val; }
  void Set_need_bootstrap(bool val) { _attr._var._bootstrap = val; }
  bool Need_rescale(void) const { return _attr._var._rescale; }
  bool Need_bootstrap(void) const { return _attr._var._bootstrap; }

  bool operator==(const REGION_ELEM_DATA& other) const {
    return _dfg_node == other.Dfg_node() &&
           _call_site_info == other._call_site_info;
  }

  bool operator<(const REGION_ELEM_DATA& other) const {
    if (_call_site_info < other._call_site_info) return true;
    if (other._call_site_info < _call_site_info) return false;
    return _dfg_node.Value() < other._dfg_node.Value();
  }

private:
  air::opt::DFG_NODE_ID _dfg_node;
  CALLSITE_INFO         _call_site_info;
  union {
    uint32_t _val;
    struct {
      bool _bootstrap;
      bool _rescale;
    } _var;
  } _attr;
  air::opt::TRAV_STATE _trav_stat;
  REGION_ID            _region;
  REGION_EDGE_ID       _succ;  // interprocedural edge.
  uint32_t             _pred_cnt;
  REGION_ELEM_ID       _pred[0];
};

class REGION_ELEM {
public:
  class EDGE_ITER {
  public:
    EDGE_ITER(const REGION_CONTAINER* cntr, REGION_EDGE_ID edge)
        : _cntr(cntr), _edge(edge) {}
    REGION_EDGE_PTR operator*() const;
    REGION_EDGE_PTR operator->() const;
    EDGE_ITER&      operator++();
    bool            operator==(const EDGE_ITER& other) const {
      return (_cntr == other._cntr && _edge == other._edge);
    }
    bool operator!=(const EDGE_ITER& other) const { return !(*this == other); }

  private:
    const REGION_CONTAINER* Cntr(void) const { return _cntr; }
    REGION_EDGE_ID          Edge(void) const { return _edge; }

    const REGION_CONTAINER* _cntr;
    REGION_EDGE_ID          _edge;
  };

  REGION_ELEM(const REGION_CONTAINER* cntr, REGION_ELEM_DATA_PTR data)
      : _cntr(cntr), _data(data) {}
  ~REGION_ELEM() {}

  REGION_ELEM_ID       Id(void) const { return _data.Id(); }
  const CALLSITE_INFO& Callsite_info(void) const {
    return _data->Callsite_info();
  }
  air::base::FUNC_ID     Caller(void) const { return _data->Caller(); }
  air::base::FUNC_ID     Callee(void) const { return _data->Callee(); }
  air::opt::DFG_NODE_ID  Dfg_node_id(void) const { return _data->Dfg_node(); }
  air::opt::DFG_NODE_PTR Dfg_node(void) const;
  air::base::TYPE_ID     Type_id(void) const { return Dfg_node()->Type(); }
  air::base::TYPE_PTR    Type(void) const;
  air::base::NODE_PTR    Call_node(void) const;
  air::base::NODE_ID     Call_node_id(void) const {
    if (Data()->Call_node() == air::base::Null_id) return air::base::NODE_ID();
    return Call_node()->Id();
  }
  REGION_ID       Region(void) const { return _data->Region(); }
  REGION_EDGE_ID  Succ_id(void) const { return _data->Succ(); }
  REGION_EDGE_PTR Succ(void) const;
  REGION_ELEM_ID  Pred_id(uint32_t id) const { return _data->Pred(id); }
  REGION_ELEM_PTR Pred(uint32_t id) const;
  uint32_t        Pred_cnt(void) const { return _data->Pred_cnt(); }
  void            Set_pred(uint32_t id, REGION_ELEM_PTR pred);
  bool            Has_valid_pred() const {
    if (Pred_cnt() == 0) return false;
    for (uint32_t id = 0; id < Pred_cnt(); ++id) {
      if (Pred_id(id) != air::base::Null_id) return true;
    }
    return false;
  }
  bool      Has_succ(REGION_ELEM_ID succ) const;
  EDGE_ITER Begin_succ(void) const { return EDGE_ITER(Cntr(), Succ_id()); }
  EDGE_ITER End_succ(void) const { return EDGE_ITER(Cntr(), REGION_EDGE_ID()); }

  void Add_succ(REGION_ELEM_ID succ);
  void Set_region(REGION_ID region) { _data->Set_region(region); }

  air::opt::TRAV_STATE Trav_state(void) const { return _data->Trav_state(); }
  void                 Set_trav_state(air::opt::TRAV_STATE state) {
    _data->Set_trav_state(state);
  }

  uint32_t Mul_depth(void) const;
  bool     Need_rescale(void) const { return _data->Need_rescale(); }
  bool     Need_bootstrap(void) const { return _data->Need_bootstrap(); }
  void     Set_need_rescale(bool val) { _data->Set_need_rescale(val); }
  void     Set_need_bootstrap(bool val) { _data->Set_need_bootstrap(val); }

  std::string To_str() const;
  void        Print_dot(std::ostream& os, uint32_t indent) const;

  bool operator==(const REGION_ELEM_PTR& other) const {
    return _data == other->_data;
  }
  const REGION_CONTAINER* Cntr() const { return _cntr; }
  REGION_ELEM_DATA_PTR    Data(void) const { return _data; }

private:
  void Set_succ(REGION_EDGE_ID succ) { _data->Set_succ(succ); }
  const REGION_CONTAINER* _cntr;
  REGION_ELEM_DATA_PTR    _data;
};

class REGION_DATA {
public:
  explicit REGION_DATA(MEMPOOL* mem_pool) : _elem(ELEM_ALLOC(mem_pool)) {}
  REGION_DATA(const REGION_DATA& other) : _elem(other._elem) {}
  ~REGION_DATA() {}

  void Add_elem(REGION_ELEM_ID elem) { _elem.push_back(elem); }

  bool Has_elem(const REGION_ELEM_ID& elem) {
    return std::find(_elem.begin(), _elem.end(), elem) != _elem.end();
  }

  ELEM_LIST::const_iterator Begin_elem(void) const { return _elem.begin(); }
  ELEM_LIST::const_iterator End_elem(void) const { return _elem.end(); }
  uint32_t                  Elem_cnt(void) const { return _elem.size(); }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  REGION_DATA(void);
  REGION_DATA operator=(const REGION_DATA&);

  ELEM_LIST _elem;
};

class REGION {
public:
  //! @brief
  class ELEM_ITER {
  public:
    ELEM_ITER(const REGION_CONTAINER* cntr, ELEM_LIST::const_iterator iter)
        : _cntr(cntr), _iter(iter) {}
    ~ELEM_ITER() {}

    REGION_ELEM_PTR operator*() const;
    REGION_ELEM_PTR operator->() const;
    ELEM_ITER&      operator++() {
      ++_iter;
      return *this;
    }
    bool operator==(const ELEM_ITER& other) const {
      return _cntr == other._cntr && _iter == other._iter;
    }
    bool operator!=(const ELEM_ITER& other) const { return !(*this == other); }

  private:
    // REQUIRED UNDEFINED UNWANTED methods
    ELEM_ITER(void);
    ELEM_ITER(const ELEM_ITER&);
    ELEM_ITER operator=(const ELEM_ITER&);

    const REGION_CONTAINER*   Cntr() const { return _cntr; }
    ELEM_LIST::const_iterator Iter() const { return _iter; }
    const REGION_CONTAINER*   _cntr;
    ELEM_LIST::const_iterator _iter;
  };
  friend class REGION_CONTAINER;

  REGION(REGION_CONTAINER* cntr, REGION_DATA_PTR data)
      : _cntr(cntr), _data(data) {}
  REGION(const REGION& other) : _cntr(other.Cntr()), _data(other.Data()) {}
  ~REGION() {}

  REGION_ID      Id() const { return _data.Id(); }
  uint32_t       Elem_cnt(void) const { return _data->Elem_cnt(); }
  void           Add_elem(REGION_ELEM_ID elem) { _data->Add_elem(elem); }
  REGION_ELEM_ID Add_elem(air::opt::DFG_NODE_ID node, uint32_t pred_cnt,
                          air::base::FUNC_ID callee, air::base::FUNC_ID caller,
                          air::opt::DFG_NODE_ID call_site);
  ELEM_ITER      Begin_elem(void) const {
    return ELEM_ITER(Cntr(), _data->Begin_elem());
  }
  ELEM_ITER End_elem(void) const {
    return ELEM_ITER(Cntr(), _data->End_elem());
  }

  air::opt::DFG_NODE_PTR Dfg_node(const REGION_ELEM& elem) const;
  air::base::NODE_PTR    Call_node(const REGION_ELEM& elem) const;
  std::string            To_str() const {
    return "REGION_" + std::to_string(Id().Value());
  }
  void Print() const { std::cout << To_str() << std::endl; }
  void Print_dot(std::ostream& os, uint32_t indent) const;

private:
  // REQUIRED UNDEFINED UNWANTED methods
  REGION(void);
  REGION operator=(const REGION&);

  REGION_CONTAINER* Cntr() const { return _cntr; }
  REGION_DATA_PTR   Data() const { return _data; }

  REGION_CONTAINER* _cntr;
  REGION_DATA_PTR   _data;
};

}  // namespace ckks
}  // namespace fhe
#endif  // FHE_CKKS_DFG_REGION_H