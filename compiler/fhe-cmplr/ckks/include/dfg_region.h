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
#include "air/opt/dfg_data.h"
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
  float          Weight(void) const { return _weight; }
  void           Set_weight(float val) { _weight = val; }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  REGION_EDGE_DATA(void);
  REGION_EDGE_DATA(const REGION_EDGE_DATA&);
  REGION_EDGE_DATA& operator=(const REGION_EDGE_DATA&);

  REGION_ELEM_ID _dst;
  REGION_EDGE_ID _next;
  float          _weight = 0.;
};

class REGION_CONTAINER;
//! @brief REGION_EDGE contains the pointer of REGION_CONTAINER and
//! REGION_EDGE_DATA_PTR. Provides APIs to access the dst node and the next
//! edge.
class REGION_EDGE {
public:
  REGION_EDGE(const REGION_CONTAINER* cntr, REGION_EDGE_DATA_PTR data)
      : _cntr(cntr), _data(data) {}
  REGION_EDGE(const REGION_EDGE& o) : REGION_EDGE(o.Cntr(), o.Data()) {}
  ~REGION_EDGE() {}

  REGION_EDGE_ID  Id(void) const { return _data.Id(); }
  REGION_ELEM_ID  Dst_id(void) const { return _data->Dst(); }
  REGION_ELEM_PTR Dst(void) const;
  REGION_EDGE_ID  Next_id(void) const { return _data->Next(); }
  REGION_EDGE_PTR Next(void) const;
  void            Set_next(REGION_EDGE_ID next) { _data->Set_next(next); }
  float           Weight(void) const { return _data->Weight(); }
  void            Set_weight(float val) { _data->Set_weight(val); }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  REGION_EDGE(void);
  REGION_EDGE& operator=(const REGION_EDGE&);

  REGION_EDGE_DATA_PTR    Data(void) const { return _data; }
  const REGION_CONTAINER* Cntr(void) const { return _cntr; }

  const REGION_CONTAINER* _cntr;
  REGION_EDGE_DATA_PTR    _data;
};

//! @brief CALLSITE_INFO contains the FUNC_ID of the caller and the callee,
//! and the DFG node ID of call_node. The current implementation supports
//! call paths with a depth of 2 or less.
class CALLSITE_INFO {
public:
  using FUNC_ID     = air::base::FUNC_ID;
  using DFG_NODE_ID = air::opt::DFG_NODE_ID;
  CALLSITE_INFO(FUNC_ID caller, FUNC_ID callee, DFG_NODE_ID node)
      : _caller(caller), _callee(callee), _call_node(node) {}
  //! @brief constructor of CALLSITE_INFO for entry function.
  explicit CALLSITE_INFO(air::base::FUNC_ID callee)
      : _caller(), _callee(callee), _call_node(air::base::Null_id) {
    AIR_ASSERT(!callee.Is_null());
  }
  CALLSITE_INFO(void) : _caller(), _callee(), _call_node(air::base::Null_id) {}
  CALLSITE_INFO(const CALLSITE_INFO& o)
      : CALLSITE_INFO(o.Caller(), o.Callee(), o.Node()) {}
  ~CALLSITE_INFO() {}

  FUNC_ID     Caller(void) const { return _caller; }
  FUNC_ID     Callee(void) const { return _callee; }
  DFG_NODE_ID Node(void) const { return _call_node; }
  //! @brief Returns true if the call node or the callee function is invalid;
  //! otherwise, returns false.
  bool Is_invalid(void) const {
    return _call_node == air::base::Null_id || _callee.Is_null();
  }
  //! @brief Comparator of CALLSITE_INFO.
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
  void Print(std::ostream& os) const {
    os << "FUNC_" << Caller().Value() << " call " << Callee().Value()
       << " at NODE_" << Node().Value() << std::endl;
  }
  void Print(void) const { Print(std::cout); }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  CALLSITE_INFO& operator=(const CALLSITE_INFO&);

  FUNC_ID     _caller;
  FUNC_ID     _callee;
  DFG_NODE_ID _call_node;  // DFG node of call node
};

//! @brief REGION_ELEM_DATA contains data related to REGION_ELEM and includes
//! the following information: DFG Node ID (_dfg_node): The identifier of the
//! DFG node. Call Site Information (_call_site): Details about the call site.
//! Predecessor IDs (_pred): Identifiers of the predecessors.
//! Successor Edge ID (_succ): Edge identifiers of the successors.
//! Region ID (_region): Identifier of the region to which the current element
//! belongs. Attribute (_attr): Indicates if the current element requires
//! rescaling or bootstrapping.
class REGION_ELEM_DATA {
public:
  using DFG_NODE_ID = air::opt::DFG_NODE_ID;
  using FUNC_ID     = air::base::FUNC_ID;
  REGION_ELEM_DATA(DFG_NODE_ID node, uint32_t pred_cnt,
                   const CALLSITE_INFO& call_site)
      : _dfg_node(node),
        _pred_cnt(pred_cnt),
        _call_site(call_site),
        _region(air::base::Null_id),
        _succ(air::base::Null_id),
        _trav_state(air::opt::TRAV_STATE_RAW) {
    _attr._val = 0;
    for (uint32_t id = 0; id < _pred_cnt; ++id) {
      _pred[id] = air::base::Null_id;
    }
  }

  REGION_ELEM_DATA(const REGION_ELEM_DATA& o)
      : REGION_ELEM_DATA(o.Dfg_node(), 0, o.Callsite_info()) {}
  ~REGION_ELEM_DATA() {}

  DFG_NODE_ID          Dfg_node(void) const { return _dfg_node; }
  const CALLSITE_INFO& Callsite_info(void) const { return _call_site; }
  FUNC_ID              Caller(void) const { return _call_site.Caller(); }
  FUNC_ID              Parent_func(void) const { return _call_site.Callee(); }
  DFG_NODE_ID          Call_node(void) const { return _call_site.Node(); }
  REGION_ID            Region(void) const { return _region; }
  REGION_EDGE_ID       Succ(void) const { return _succ; }
  void                 Set_succ(REGION_EDGE_ID succ) { _succ = succ; }
  void                 Set_region(REGION_ID region) { _region = region; }
  uint32_t             Pred_cnt(void) { return _pred_cnt; }
  void                 Set_pred(uint32_t id, REGION_ELEM_ID pred) {
    AIR_ASSERT_MSG(id < _pred_cnt, "pred index out of range");
    _pred[id] = pred;
  }
  REGION_ELEM_ID Pred(uint32_t id) const {
    AIR_ASSERT_MSG(id < _pred_cnt, "pred index out of range");
    return _pred[id];
  }
  air::opt::TRAV_STATE Trav_state(void) const { return _trav_state; }
  void Set_trav_state(air::opt::TRAV_STATE state) { _trav_state = state; }

  void Clear_attr(void) { _attr._val = 0; }
  void Set_need_rescale(bool val) { _attr._var._rescale = val; }
  void Set_need_bootstrap(bool val) { _attr._var._bootstrap = val; }
  bool Rescale_elem(void) const { return _attr._var._rescale; }
  bool Bootstrap_elem(void) const { return _attr._var._bootstrap; }

  //! @brief Comparator of REGION_ELEM. REGION_ELEM represents a DFG node for a
  //! callsite. A REGION_ELEM is uniquelly identified by CALLSITE_INFO
  //! and the DFG node.
  bool operator==(const REGION_ELEM_DATA& other) const {
    return _dfg_node == other.Dfg_node() && _call_site == other._call_site;
  }

  bool operator<(const REGION_ELEM_DATA& other) const {
    if (_call_site < other._call_site) return true;
    if (other._call_site < _call_site) return false;
    return _dfg_node.Value() < other._dfg_node.Value();
  }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  REGION_ELEM_DATA(void);
  REGION_ELEM_DATA& operator=(const REGION_ELEM_DATA&);

  CALLSITE_INFO  _call_site;
  DFG_NODE_ID    _dfg_node;
  REGION_ID      _region;
  REGION_EDGE_ID _succ;
  union {
    uint32_t _val;
    struct {
      bool _bootstrap;
      bool _rescale;
    } _var;
  } _attr;
  air::opt::TRAV_STATE _trav_state;
  uint32_t             _pred_cnt;
  REGION_ELEM_ID       _pred[0];
};

//! @brief REGION_ELEM represents a CIPHER type DFG node for a callsite.
//! A REGION_ELEM is uniquelly identified by CALLSITE_INFO and the DFG node.
//! REGION_ELEM contains the pointer of the REGION_CONTAINER (_cntr) and
//! REGION_ELEM_DATA (_data). Offers APIs and iterator to access and update
//! REGION_ELEM_DATA, as well as to traverse predecessor and successor nodes.
class REGION_ELEM {
public:
  //! @brief Iterator to traverse successor nodes.
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

  using FUNC_ID      = air::base::FUNC_ID;
  using DFG_NODE_ID  = air::opt::DFG_NODE_ID;
  using DFG_NODE_PTR = air::opt::DFG_NODE_PTR;
  using TYPE_ID      = air::base::TYPE_ID;
  using TYPE_PTR     = air::base::TYPE_PTR;
  using NODE_ID      = air::base::NODE_ID;
  using NODE_PTR     = air::base::NODE_PTR;

  REGION_ELEM(const REGION_CONTAINER* cntr, REGION_ELEM_DATA_PTR data)
      : _cntr(cntr), _data(data) {}
  REGION_ELEM(const REGION_ELEM& o) : REGION_ELEM(o.Cntr(), o.Data()) {}
  REGION_ELEM(void) : _cntr(nullptr), _data() {}
  ~REGION_ELEM() {}

  REGION_ELEM_ID Id(void) const { return _data.Id(); }
  //! @brief Return information of callsite which current element belongs to.
  const CALLSITE_INFO& Callsite_info(void) const {
    return _data->Callsite_info();
  }
  //! @brief Return the parent function ID of the DFG node.
  FUNC_ID      Parent_func(void) const { return _data->Parent_func(); }
  DFG_NODE_ID  Dfg_node_id(void) const { return _data->Dfg_node(); }
  DFG_NODE_PTR Dfg_node(void) const;
  TYPE_ID      Type_id(void) const { return Dfg_node()->Type(); }
  TYPE_PTR     Type(void) const;
  NODE_PTR     Call_node(void) const;
  NODE_ID      Call_node_id(void) const {
    if (Data()->Call_node() == air::base::Null_id) return air::base::NODE_ID();
    return Call_node()->Id();
  }
  REGION_ID        Region_id(void) const { return _data->Region(); }
  CONST_REGION_PTR Region(void) const;
  REGION_EDGE_ID   Succ_id(void) const { return _data->Succ(); }
  REGION_EDGE_PTR  Succ(void) const;
  REGION_ELEM_ID   Pred_id(uint32_t id) const { return _data->Pred(id); }
  REGION_ELEM_PTR  Pred(uint32_t id) const;
  uint32_t         Pred_cnt(void) const { return _data->Pred_cnt(); }
  void             Set_pred(uint32_t id, CONST_REGION_ELEM_PTR pred);
  bool             Has_valid_pred() const {
    if (Pred_cnt() == 0) return false;
    for (uint32_t id = 0; id < Pred_cnt(); ++id) {
      if (Pred_id(id) != air::base::Null_id) return true;
    }
    return false;
  }
  bool      Has_succ(REGION_ELEM_ID succ) const;
  EDGE_ITER Begin_succ(void) const { return EDGE_ITER(Cntr(), Succ_id()); }
  EDGE_ITER End_succ(void) const { return EDGE_ITER(Cntr(), REGION_EDGE_ID()); }
  uint32_t  Succ_cnt(void) const {
    uint32_t cnt = 0;
    for (EDGE_ITER iter = Begin_succ(); iter != End_succ(); ++iter) ++cnt;
    return cnt;
  }
  void Add_succ(REGION_ELEM_ID succ);
  void Set_region(REGION_ID region) { _data->Set_region(region); }

  air::opt::TRAV_STATE Trav_state(void) const { return _data->Trav_state(); }
  void                 Set_trav_state(air::opt::TRAV_STATE state) {
    _data->Set_trav_state(state);
  }
  //! @brief Return mul_level consumed by operations in current element.
  uint32_t Mul_depth(void) const;
  bool     Need_rescale(void) const { return _data->Rescale_elem(); }
  bool     Need_bootstrap(void) const { return _data->Bootstrap_elem(); }
  void     Set_need_rescale(bool val) { _data->Set_need_rescale(val); }
  void     Set_need_bootstrap(bool val) { _data->Set_need_bootstrap(val); }
  void     Clear_attr(void) const { _data->Clear_attr(); }

  std::string To_str(void) const;
  void        Print_dot(std::ostream& os, uint32_t indent) const;
  void        Print(void) const { Print(std::cout, 0); }
  void        Print(std::ostream& os, uint32_t indent = 0) const {
    os << std::string(indent, ' ') << To_str() << std::endl;
  }

  bool operator==(const REGION_ELEM_PTR& other) const {
    return _cntr == other->Cntr() && _data == other->Data();
  }
  const REGION_CONTAINER* Cntr() const { return _cntr; }
  REGION_ELEM_DATA_PTR    Data(void) const { return _data; }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  REGION_ELEM& operator=(const REGION_ELEM&);

  void Set_succ(REGION_EDGE_ID succ) { _data->Set_succ(succ); }
  const REGION_CONTAINER* _cntr;
  REGION_ELEM_DATA_PTR    _data;
};

//! @brief REGION_DATA contains IDs of REGION_ELEMs belonging to the current
//! region. Provides APIs to add and traverse REGION_ELEMs.
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

//! @brief REGION is a subgraph of the data flow graph, which is an analyzing
//! unit of RESBM. REGION contains elements that can be computed at the same
//! mul_depth. REGION contains pointer of REGION_CONTAINER (_cntr) and
//! REGION_DATA (_data), provides APIs and iterator to traverse region elements
//! and access/update REGION_DATA.
class REGION {
public:
  //! @brief Iterator to traverse elements.
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

  REGION_ID Id() const { return _data.Id(); }
  uint32_t  Elem_cnt(void) const { return _data->Elem_cnt(); }
  void      Add_elem(REGION_ELEM_ID elem) { _data->Add_elem(elem); }

  ELEM_ITER Begin_elem(void) const {
    return ELEM_ITER(Cntr(), _data->Begin_elem());
  }
  ELEM_ITER End_elem(void) const {
    return ELEM_ITER(Cntr(), _data->End_elem());
  }

  std::string To_str() const {
    return "REGION_" + std::to_string(Id().Value());
  }
  void Print(std::ostream& os, uint32_t indent) const {
    os << std::string(indent, ' ') << To_str() << std::endl;
  }
  void Print() const { Print(std::cout, 0); }
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