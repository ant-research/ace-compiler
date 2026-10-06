//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_SCC_NODE_H
#define AIR_OPT_SCC_NODE_H
#include <algorithm>
#include <list>
#include <string>

#include "air/base/arena.h"
#include "air/base/id_wrapper.h"
#include "air/base/ptr_wrapper.h"
#include "air/opt/ssa_container.h"
#include "air/util/graph_base.h"
#include "dfg_container.h"
#include "dfg_data.h"

namespace air {
namespace opt {

class SCC_NODE_DATA;
using SCC_NODE_ID       = base::ID<SCC_NODE_DATA>;
using SCC_NODE_DATA_PTR = base::PTR_FROM_DATA<SCC_NODE_DATA>;

class SCC_NODE;
using SCC_NODE_PTR = base::PTR<SCC_NODE>;

class SCC_EDGE_DATA;
using SCC_EDGE_ID       = base::ID<SCC_EDGE_DATA>;
using SCC_EDGE_DATA_PTR = base::PTR_FROM_DATA<SCC_EDGE_DATA>;

class SCC_EDGE;
using SCC_EDGE_PTR = base::PTR<SCC_EDGE>;

using SCC_MEMPOOL     = air::util::MEM_POOL<4096>;
using SCC_NODE_ALLOC  = air::util::CXX_MEM_ALLOCATOR<SCC_NODE_ID, SCC_MEMPOOL>;
using SCC_NODE_LIST   = std::list<SCC_NODE_ID, SCC_NODE_ALLOC>;
using SCC_EDGE_ALLOC  = air::util::CXX_MEM_ALLOCATOR<SCC_EDGE_ID, SCC_MEMPOOL>;
using SCC_EDGE_LIST   = std::list<SCC_EDGE_ID, SCC_EDGE_ALLOC>;
using ELEM_NODE_ALLOC = air::util::CXX_MEM_ALLOCATOR<uint32_t, SCC_MEMPOOL>;
using ELEM_NODE_LIST  = std::list<uint32_t, ELEM_NODE_ALLOC>;

//! @brief SCC_EDGE_DATA contains the ID of dst node.
class SCC_EDGE_DATA {
private:
  friend class SCC_EDGE;
  friend class SCC_CONTAINER;
  SCC_EDGE_DATA(void) : _dst(base::Null_id) {}
  explicit SCC_EDGE_DATA(SCC_NODE_ID dst) : _dst(dst) {}
  ~SCC_EDGE_DATA() {}

  SCC_NODE_ID Dst(void) const { return _dst; }

  // REQUIRED UNDEFINED UNWANTED methods
  SCC_EDGE_DATA(const SCC_EDGE_DATA&);
  SCC_EDGE_DATA operator=(const SCC_EDGE_DATA&);

  SCC_NODE_ID _dst;
};

//! @brief SCC_NODE_DATA contains ID value of element nodes (_elem), ID of
//! predecessors (_pred), ID of successors (_succ), and traversing state of
//! current node (_trav_stat).
class SCC_NODE_DATA {
public:
  friend class SCC_NODE;
  friend class SCC_CONTAINER;
  SCC_NODE_DATA(SCC_NODE_ALLOC alloc)
      : _elem(alloc), _pred(alloc), _succ(alloc) {}
  ~SCC_NODE_DATA() {}

private:
  // REQUIRED UNDEFINED UNWANTED methods
  SCC_NODE_DATA(void);
  SCC_NODE_DATA(const SCC_NODE_DATA&);
  SCC_NODE_DATA operator=(const SCC_NODE_DATA&);

  ELEM_NODE_LIST::const_iterator Begin_elem(void) const {
    return _elem.begin();
  }
  ELEM_NODE_LIST::const_iterator End_elem(void) const { return _elem.end(); }

  SCC_NODE_LIST::const_iterator Begin_pred(void) const { return _pred.begin(); }
  SCC_NODE_LIST::const_iterator End_pred(void) const { return _pred.end(); }

  SCC_EDGE_LIST::const_iterator Begin_succ(void) const { return _succ.begin(); }
  SCC_EDGE_LIST::const_iterator End_succ(void) const { return _succ.end(); }

  uint32_t Succ_cnt() const { return _succ.size(); }
  uint32_t Pred_cnt() const { return _pred.size(); }

  bool Has_elem(uint32_t node) const {
    return std::find(Begin_elem(), End_elem(), node) != End_elem();
  }
  void     Add_elem(uint32_t node) { _elem.push_back(node); }
  uint32_t Elem_cnt(void) const { return _elem.size(); }
  bool     Has_pred(SCC_NODE_ID pred) const {
    return std::find(Begin_pred(), End_pred(), pred) != End_pred();
  }
  void Add_pred(SCC_NODE_ID pred) { _pred.push_back(pred); }
  void Add_succ(SCC_EDGE_ID succ) { _succ.push_back(succ); }

  TRAV_STATE Trav_state(void) const { return _trav_stat; }
  bool At_trav_state(TRAV_STATE state) const { return state == _trav_stat; }
  void Set_trav_state(TRAV_STATE state) { _trav_stat = state; }

  ELEM_NODE_LIST _elem;
  SCC_NODE_LIST  _pred;
  SCC_EDGE_LIST  _succ;
  TRAV_STATE     _trav_stat = TRAV_STATE_RAW;
};

class SCC_CONTAINER;
//! @brief SCC_EDGE contains pointer of SCC_CONTAINER (_cntr) and edge data
//! (_data), provides APIs for getting edge ID and accessing dst node.
class SCC_EDGE {
public:
  SCC_EDGE(SCC_CONTAINER* cntr, SCC_EDGE_DATA_PTR data)
      : _cntr(cntr), _data(data) {}
  SCC_EDGE(const SCC_EDGE& other) : SCC_EDGE(other.Cntr(), other.Data()){};
  SCC_EDGE(void) : SCC_EDGE(nullptr, SCC_EDGE_DATA_PTR()){};
  ~SCC_EDGE() {}

  SCC_EDGE_ID  Id() const { return _data.Id(); }
  SCC_NODE_ID  Dst_id() { return _data->Dst(); }
  SCC_NODE_PTR Dst();

private:
  // REQUIRED UNDEFINED UNWANTED methods
  SCC_EDGE operator=(const SCC_EDGE&);

  SCC_CONTAINER*    Cntr() const { return _cntr; }
  SCC_EDGE_DATA_PTR Data() const { return _data; }

  SCC_CONTAINER*    _cntr;
  SCC_EDGE_DATA_PTR _data;
};

//! @brief SCC_NODE contains pointer of SCC_CONTAINER and node data, provides
//! APIs for getting node ID and traversing predecessor, successor, and element.
class SCC_NODE {
public:
  //! @brief Iterator for traversing element nodes, dereferencing returns the ID
  //! value of the element node.
  using ELEM_ITER = ELEM_NODE_LIST::const_iterator;
  //! @brief Iterator for traversing predecessors of SCC_NODE.
  class SCC_NODE_ITER {
  public:
    SCC_NODE_ITER(const SCC_CONTAINER* cntr, SCC_NODE_LIST::const_iterator iter)
        : _cntr(cntr), _node(iter) {}

    SCC_NODE_PTR   operator*() const;
    SCC_NODE_PTR   operator->() const;
    SCC_NODE_ITER& operator++() {
      ++_node;
      return *this;
    }
    bool operator==(const SCC_NODE_ITER& other) const {
      return (_cntr == other._cntr && _node == other._node);
    }
    bool operator!=(const SCC_NODE_ITER& other) const {
      return !(*this == other);
    }

  private:
    // REQUIRED UNDEFINED UNWANTED methods
    SCC_NODE_ITER(void);
    SCC_NODE_ITER(const SCC_NODE_ITER&);
    SCC_NODE_ITER& operator=(const SCC_NODE_ITER&);

    const SCC_CONTAINER* Cntr() const { return _cntr; }
    const SCC_NODE_ID    Node() const { return *_node; }

    const SCC_CONTAINER*          _cntr;
    SCC_NODE_LIST::const_iterator _node;
  };

  //! @brief Iterator for traversing successor edges of SCC_NODE.
  class SCC_EDGE_ITER {
  public:
    SCC_EDGE_ITER(const SCC_CONTAINER* cntr, SCC_EDGE_LIST::const_iterator edge)
        : _cntr(cntr), _edge(edge) {}
    ~SCC_EDGE_ITER() {}

    SCC_EDGE_PTR   operator*() const;
    SCC_EDGE_PTR   operator->() const;
    SCC_EDGE_ITER& operator++() {
      ++_edge;
      return *this;
    }
    bool operator==(const SCC_EDGE_ITER& other) const {
      return (_cntr == other._cntr && _edge == other._edge);
    }
    bool operator!=(const SCC_EDGE_ITER& other) const {
      return !(*this == other);
    }

  private:
    // REQUIRED UNDEFINED UNWANTED methods
    SCC_EDGE_ITER(void);
    SCC_EDGE_ITER(const SCC_EDGE_ITER&);
    SCC_EDGE_ITER operator=(const SCC_EDGE_ITER&);

    const SCC_CONTAINER* Cntr() const { return _cntr; }
    SCC_EDGE_ID          Edge() const { return *_edge; }

    const SCC_CONTAINER*          _cntr;
    SCC_EDGE_LIST::const_iterator _edge;
  };

  SCC_NODE(SCC_CONTAINER* cntr, SCC_NODE_DATA_PTR data)
      : _cntr(cntr), _data(data) {}
  SCC_NODE(const SCC_NODE& other) : SCC_NODE(other.Cntr(), other.Data()) {}
  SCC_NODE(void) : SCC_NODE(nullptr, SCC_NODE_DATA_PTR()) {}
  ~SCC_NODE() {}

  SCC_NODE_ID Id() const { return _data.Id(); }

  template <typename ELEM_ID>
  bool Has_elem(ELEM_ID elem) {
    return _data->Has_elem(elem.Value());
  }
  template <typename ELEM_ID>
  void Add_elem(ELEM_ID elem) {
    _data->Add_elem(elem.Value());
  }
  ELEM_ITER Begin_elem(void) const { return _data->Begin_elem(); }
  ELEM_ITER End_elem(void) const { return _data->End_elem(); }

  bool Has_pred(SCC_NODE_ID pred) const { return _data->Has_pred(pred); }
  void Add_pred(SCC_NODE_ID pred) const { _data->Add_pred(pred); }
  SCC_NODE_ITER Begin_pred(void) const {
    return SCC_NODE_ITER(Cntr(), _data->Begin_pred());
  }
  SCC_NODE_ITER End_pred(void) const {
    return SCC_NODE_ITER(Cntr(), _data->End_pred());
  }

  bool          Has_succ(SCC_NODE_ID succ);
  void          Add_succ(SCC_NODE_ID succ);
  SCC_EDGE_ITER Begin_succ(void) const {
    return SCC_EDGE_ITER(Cntr(), _data->Begin_succ());
  }
  SCC_EDGE_ITER End_succ(void) const {
    return SCC_EDGE_ITER(Cntr(), _data->End_succ());
  }

  uint32_t Elem_cnt(void) const { return _data->Elem_cnt(); }
  uint32_t Succ_cnt(void) const { return _data->Succ_cnt(); }
  uint32_t Pred_cnt(void) const { return _data->Pred_cnt(); }

  TRAV_STATE Trav_state(void) const { return _data->Trav_state(); }
  bool       At_trav_state(TRAV_STATE state) const {
    return _data->At_trav_state(state);
  }
  void Set_trav_state(TRAV_STATE state) { _data->Set_trav_state(state); }

  std::string To_str(void) {
    return std::string("SCC_") + std::to_string(Id().Value());
  }
  void Print_dot(std::ostream& os, uint32_t indent);
  template <typename ELEM_CONTAINER>
  void Print_dot(const ELEM_CONTAINER* elem_cntr, std::ostream& os,
                 uint32_t indent) {
    std::string indent_str0(indent, ' ');
    os << indent_str0 << "subgraph cluster_" << To_str() << "{" << std::endl;
    std::string indent_str1(indent + 4, ' ');
    os << indent_str1 << "label= \"" << To_str() << "\"" << std::endl;
    os << indent_str1 << "style=filled;" << std::endl;
    os << indent_str1 << "color=lightgray;" << std::endl;
    os << indent_str1 << "node [style=filled,color=white];" << std::endl;
    for (ELEM_ITER iter = Begin_elem(); iter != End_elem(); ++iter) {
      typename ELEM_CONTAINER::NODE_ID  id(*iter);
      typename ELEM_CONTAINER::NODE_PTR node = elem_cntr->Node(id);
      os << indent_str1 << "\"" << node->To_str() << "\";" << std::endl;
    }
    os << indent_str0 << "}" << std::endl;
    for (ELEM_ITER iter = Begin_elem(); iter != End_elem(); ++iter) {
      typename ELEM_CONTAINER::NODE_ID  id(*iter);
      typename ELEM_CONTAINER::NODE_PTR node = elem_cntr->Node(id);
      node->Print_dot(os, indent);
    }
  }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  SCC_NODE operator=(const SCC_NODE&);

  bool Has_pred(SCC_NODE_ID pred) { return _data->Has_pred(pred); }
  //! @brief return true if add pred, return false if failed
  bool Add_pred(SCC_NODE_ID pred) {
    if (Has_pred(pred)) return false;
    _data->Add_pred(pred);
    return true;
  }

  SCC_CONTAINER*    Cntr() const { return _cntr; }
  SCC_NODE_DATA_PTR Data() const { return _data; }

  SCC_CONTAINER*    _cntr;
  SCC_NODE_DATA_PTR _data;
};

}  // namespace opt
}  // namespace air
#endif  // AIR_OPT_SCC_NODE_H