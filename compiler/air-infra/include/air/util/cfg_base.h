//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_CFG_BASE_H
#define AIR_UTIL_CFG_BASE_H

#include <stdint.h>

#include <iostream>
#include <sstream>

#include "air/base/arena.h"
#include "air/base/id_wrapper.h"
#include "air/base/ptr_wrapper.h"
#include "air/util/cfg_data.h"
#include "air/util/debug.h"

namespace air {

namespace util {

//! @brief A generic control flow graph implementation
//!  NODE_DATA and EDGE_DATA contains user specific information for nodes and
//!  edges
template <typename NODE_DATA, typename EDGE_DATA, typename CONTAINER>
class CFG {
  friend CONTAINER;
  template <typename T>
  friend class DOM_BUILDER;

public:
  //! @brief Edge kind
  enum EDGE_KIND {
    PRED,  //!< edge is for a node's predecessors
    SUCC   //!< edge is for a node's successors
  };

  //! @brief invalid position(index) of a predecessor or successor
  static constexpr uint32_t INVALID_POS = UINT32_MAX;

  class NODE;
  class EDGE;
  template <EDGE_KIND>
  class NODE_ITER;

  using NODE_ID       = air::base::ID<NODE_DATA>;
  using EDGE_ID       = air::base::ID<EDGE_DATA>;
  using NODE_PTR      = air::base::PTR<NODE>;
  using EDGE_PTR      = air::base::PTR<EDGE>;
  using NODE_DATA_PTR = air::base::PTR_FROM_DATA<NODE_DATA>;
  using EDGE_DATA_PTR = air::base::PTR_FROM_DATA<EDGE_DATA>;

  //! @brief NODE
  class NODE {
    friend CONTAINER;
    friend class CFG;
    friend class air::base::PTR_TO_CONST<NODE>;
    friend class air::base::PTR<NODE>;
    template <typename T>
    friend class DOM_BUILDER;

  public:
    NODE_DATA_PTR Data() const { return _data; }
    NODE_ID       Id() const { return _data.Id(); }

    //! @brief Find
    //!  Find a node in predecessors(KIND == PRED) or successors(KIND == SUCC)
    //!  Return the corresponding edge
    template <EDGE_KIND KIND>
    EDGE_PTR Find(NODE_ID node) {
      EDGE_PTR edge = _cont->Cfg().Edge(KIND == PRED ? Pred_id() : Succ_id());
      while (edge != air::base::Null_ptr) {
        AIR_ASSERT((KIND == PRED ? edge->Succ_id() : edge->Pred_id()) == Id());
        NODE_ID id = (KIND == PRED) ? edge->Pred_id() : edge->Succ_id();
        if (id == node) {
          break;
        }
        edge = (KIND == PRED) ? edge->Succ_pred_next() : edge->Pred_succ_next();
      }
      return edge;
    }

    //! @brief Pos
    //!  Find the position (index) of a node in predecessors(KIND == PRED) or
    //!  successors(KIND == SUCC)
    template <EDGE_KIND KIND>
    uint32_t Pos(NODE_ID node) {
      EDGE_PTR edge = _cont->Cfg().Edge(KIND == PRED ? Pred_id() : Succ_id());
      uint32_t pos  = 0;
      while (edge != air::base::Null_ptr) {
        AIR_ASSERT((KIND == PRED ? edge->Succ_id() : edge->Pred_id()) == Id());
        NODE_ID id = (KIND == PRED) ? edge->Pred_id() : edge->Succ_id();
        if (id == node) {
          return pos;
        }
        ++pos;
        edge = (KIND == PRED) ? edge->Succ_pred_next() : edge->Pred_succ_next();
      }
      return INVALID_POS;
    }

    uint32_t Pred_pos(NODE_ID node) { return Pos<PRED>(node); }
    uint32_t Succ_pos(NODE_ID node) { return Pos<SUCC>(node); }

    //! @brief
    //!  Number of all predecessors(KIND == PRED) or successors(KIND == SUCC)
    template <EDGE_KIND KIND>
    uint32_t Count() {
      EDGE_PTR edge = _cont->Cfg().Edge(KIND == PRED ? Pred_id() : Succ_id());
      uint32_t cnt  = 0;
      while (edge != air::base::Null_ptr) {
        AIR_ASSERT((KIND == PRED ? edge->Succ_id() : edge->Pred_id()) == Id());
        ++cnt;
        edge = (KIND == PRED) ? edge->Succ_pred_next() : edge->Pred_succ_next();
      }
      return cnt;
    }

    uint32_t Pred_cnt() { return Count<PRED>(); }
    uint32_t Succ_cnt() { return Count<SUCC>(); }

    //! @brief
    //!  Get edge at given index in predecessors(KIND == PRED) or successors
    //   (KIND == SUCC)
    template <EDGE_KIND KIND>
    EDGE_PTR Get(uint32_t idx) {
      EDGE_PTR edge = _cont->Cfg().Edge(KIND == PRED ? Pred_id() : Succ_id());
      while (edge != air::base::Null_ptr && idx > 0) {
        AIR_ASSERT((KIND == PRED ? edge->Succ_id() : edge->Pred_id()) == Id());
        --idx;
        edge = (KIND == PRED) ? edge->Succ_pred_next() : edge->Pred_succ_next();
      }
      return edge;
    }

    NODE_ID Pred_id(uint32_t idx) {
      EDGE_PTR edge = Get<PRED>(idx);
      return edge->Pred_id();
    }

    NODE_ID Succ_id(uint32_t idx) {
      EDGE_PTR edge = Get<SUCC>(idx);
      return edge->Succ_id();
    }

    NODE_ITER<PRED> Begin_pred() const {
      return NODE_ITER<PRED>(_cont->Node(Id()));
    }
    NODE_ITER<PRED> End_pred() const {
      return NODE_ITER<PRED>(air::base::Null_ptr);
    }
    NODE_ITER<SUCC> Begin_succ() const {
      return NODE_ITER<SUCC>(_cont->Node(Id()));
    }
    NODE_ITER<SUCC> End_succ() const {
      return NODE_ITER<SUCC>(air::base::Null_ptr);
    }

  private:
    EDGE_ID  Pred_id() const { return EDGE_ID(Info()->_preds); }
    EDGE_ID  Succ_id() const { return EDGE_ID(Info()->_succs); }
    EDGE_PTR Pred() const { return _cont->Cfg().Edge(Pred_id()); }
    EDGE_PTR Succ() const { return _cont->Cfg().Edge(Succ_id()); }
    bool     Is_null() const { return _data.Is_null(); }

    template <EDGE_KIND KIND, typename F, typename... ARGS>
    void For_each_node(F&& f, ARGS&&... args) const {
      EDGE_PTR edge = _cont->Cfg().Edge(KIND == PRED ? Pred_id() : Succ_id());
      while (edge != air::base::Null_ptr) {
        AIR_ASSERT((KIND == PRED ? edge->Succ_id() : edge->Pred_id()) == Id());
        NODE_PTR node = (KIND == PRED) ? edge->Pred() : edge->Succ();
        f(node, args...);
        edge = (KIND == PRED) ? edge->Succ_pred_next() : edge->Pred_succ_next();
      }
    }

    template <EDGE_KIND KIND, typename F, typename... ARGS>
    void For_each_edge(F&& f, ARGS&&... args) const {
      EDGE_PTR edge = _cont->Cfg().Edge(KIND == PRED ? Pred_id() : Succ_id());
      while (edge != air::base::Null_ptr) {
        AIR_ASSERT((KIND == PRED ? edge->Succ_id() : edge->Pred_id()) == Id());
        f(edge, args...);
        edge = (KIND == PRED) ? edge->Succ_pred_next() : edge->Pred_succ_next();
      }
    }

    template <EDGE_KIND KIND>
    void Add_edge(EDGE_PTR edge) {
      CFG_NODE_INFO* ninfo = Info();
      CFG_EDGE_INFO* einfo = edge->Info();
      AIR_ASSERT((KIND == PRED ? edge->Succ_id() : edge->Pred_id()) == Id());
      uint32_t* next = (KIND == PRED) ? &ninfo->_preds : &ninfo->_succs;
      uint32_t* new_next =
          (KIND == PRED) ? &einfo->_succ_pred_next : &einfo->_pred_succ_next;
      *new_next = *next;
      *next     = edge->Id().Value();
    }

  public:
    template <EDGE_KIND KIND>
    void Print_node_list(std::ostream& os) const {
      bool add_space = false;
      For_each_node<KIND>(
          [](NODE_PTR node, std::ostream& os, bool& space) {
            if (space == true) {
              os << " ";
            } else {
              space = true;
            }
            os << node->Id().Value();
          },
          os, add_space);
    }

    void Print(std::ostream& os, uint32_t indent) const {
      os << std::string(indent * 2, ' ') << "META: id=" << Id().Value()
         << " preds=[";
      Print_node_list<PRED>(os);
      os << "] succs=[";
      Print_node_list<SUCC>(os);
      os << "]" << std::endl;
    }

  protected:
    NODE() : _cont(nullptr), _data() {}
    NODE(const CONTAINER* cont, NODE_DATA_PTR data)
        : _cont(const_cast<CONTAINER*>(cont)), _data(data) {}
    CFG_NODE_INFO*       Info() { return _data->Cfg_node_info(); }
    const CFG_NODE_INFO* Info() const { return _data->Cfg_node_info(); }

    CONTAINER*    _cont;
    NODE_DATA_PTR _data;

  };  // NODE

  //! @brief EDGE
  class EDGE {
    friend class CFG;
    friend class air::base::PTR_TO_CONST<EDGE>;
    friend class air::base::PTR<EDGE>;

  public:
    EDGE_DATA_PTR Data() const { return _data; }
    EDGE_ID       Id() const { return _data.Id(); }
    NODE_ID       Pred_id() const { return NODE_ID(Info()->_pred); }
    NODE_ID       Succ_id() const { return NODE_ID(Info()->_succ); }
    NODE_PTR      Pred() const { return _cont->Cfg().Node(Pred_id()); }
    NODE_PTR      Succ() const { return _cont->Cfg().Node(Succ_id()); }

    void Print(std::ostream& os, uint32_t indent) const {
      os << std::string(indent * 2, ' ') << "META: id=" << Id().Value()
         << " pred=" << Pred_id().Value() << " succ=" << Succ_id().Value()
         << " pred_succ_next=";
      if (Info()->_pred_succ_next != air::base::Null_prim_id) {
        os << Info()->_pred_succ_next;
      } else {
        os << "(null)";
      }
      os << " succ_pred_next=";
      if (Info()->_succ_pred_next != air::base::Null_prim_id) {
        os << Info()->_succ_pred_next;
      } else {
        os << "(null)";
      }
      os << std::endl;
    }

  private:
    EDGE_PTR Pred_succ_next() const {
      return _cont->Cfg().Edge(EDGE_ID(Info()->_pred_succ_next));
    }
    EDGE_PTR Succ_pred_next() const {
      return _cont->Cfg().Edge(EDGE_ID(Info()->_succ_pred_next));
    }
    bool Is_null() const { return _data.Is_null(); }

  protected:
    EDGE() : _cont(nullptr), _data() {}
    EDGE(const CONTAINER* cont, EDGE_DATA_PTR data)
        : _cont(const_cast<CONTAINER*>(cont)), _data(data) {}
    const CFG_EDGE_INFO* Info() const { return _data->Cfg_edge_info(); }
    CFG_EDGE_INFO*       Info() { return _data->Cfg_edge_info(); }
    CONTAINER*           Cntr() const { return _cont; }

    CONTAINER*    _cont;
    EDGE_DATA_PTR _data;

  };  // EDGE

  template <EDGE_KIND KIND>
  class NODE_ITER {
  public:
    NODE_ITER(NODE_PTR node) {
      if (node == air::base::Null_ptr) {
        _cur_edge = air::base::Null_ptr;
      } else {
        _cur_edge = KIND == PRED ? node->Pred() : node->Succ();
      }
    }

    CONTAINER* Cntr() const { return _cur_edge->Cntr(); }
    EDGE_PTR   Curr() const { return _cur_edge; }
    NODE_ID    Node_id() const {
      return (KIND == PRED) ? _cur_edge->Pred_id() : _cur_edge->Succ_id();
    }

    NODE_PTR operator*() const {
      return (KIND == PRED) ? _cur_edge->Pred() : _cur_edge->Succ();
    }

    NODE_PTR operator->() const {
      return (KIND == PRED) ? _cur_edge->Pred() : _cur_edge->Succ();
    }

    NODE_ITER& operator++() {
      _cur_edge = (KIND == PRED) ? _cur_edge->Succ_pred_next()
                                 : _cur_edge->Pred_succ_next();
      return *this;
    }
    NODE_ITER operator++(int) {
      NODE_ITER tmp = *this;
      _cur_edge     = (KIND == PRED) ? _cur_edge->Succ_pred_next()
                                     : _cur_edge->Pred_succ_next();
      return tmp;
    }

    bool operator==(const NODE_ITER& other) const {
      return _cur_edge == other.Curr();
    }

    bool operator!=(const NODE_ITER& other) const { return !(*this == other); }

  private:
    EDGE_PTR _cur_edge;
  };

public:
  CFG(const CONTAINER* cont) : _cont(const_cast<CONTAINER*>(cont)) {}

  uint32_t Node_count() const { return _cont->Node_count(); }
  uint32_t Edge_count() const { return _cont->Edge_count(); }

  NODE_PTR Node(NODE_ID node) const {
    return NODE_PTR(NODE(_cont, _cont->Find(node)));
  }

  EDGE_PTR Edge(EDGE_ID edge) const {
    return EDGE_PTR(EDGE(_cont, _cont->Find(edge)));
  }

public:
  NODE_PTR New_node() {
    NODE_DATA_PTR data = _cont->New_node_data();
    new (data) NODE_DATA();
    return NODE_PTR(NODE(_cont, data));
  }

  EDGE_PTR Connect(NODE_PTR pred, NODE_PTR succ) {
    AIR_ASSERT(pred->template Find<SUCC>(succ->Id()) == air::base::Null_ptr);
    AIR_ASSERT(succ->template Find<PRED>(pred->Id()) == air::base::Null_ptr);
    EDGE_PTR edge = New_edge(pred->Id(), succ->Id());
    pred->template Add_edge<SUCC>(edge);
    succ->template Add_edge<PRED>(edge);
    return edge;
  }

private:
  template <typename F, typename... ARGS>
  void For_each_node(F&& f, ARGS&&... args) const {
    auto node_tab = _cont->Node_tab();
    for (auto id = node_tab->Begin(); id != node_tab->End(); ++id) {
      NODE_PTR node = Node(NODE_ID(*id));
      f(node, args...);
    }
  }

  template <typename F, typename... ARGS>
  void For_each_edge(F&& f, ARGS&&... args) const {
    auto edge_tab = _cont->Edge_tab();
    for (auto id = edge_tab->Begin(); id != edge_tab->End(); ++id) {
      EDGE_PTR edge = Edge(EDGE_ID(*id));
      f(edge, args...);
    }
  }

public:
  void Print(std::ostream& os, uint32_t indent = 0) const {
    os << "NODE:" << std::endl;
    For_each_node(
        +[](NODE_PTR node, std::ostream& os, uint32_t idt) {
          node->Print(os, idt);
          os << std::endl;
        },
        os, indent + 1);
    os << "EDGE:" << std::endl;
    For_each_edge(
        +[](EDGE_PTR edge, std::ostream& os, uint32_t idt) {
          edge->Print(os, idt);
          os << std::endl;
        },
        os, indent + 1);
  }

  void Print() const { Print(std::cout, 0); }

  std::string To_str() const {
    std::stringbuf buf;
    std::ostream   os(&buf);
    Print(os, 0);
    return buf.str();
  }

private:
  EDGE_PTR New_edge(NODE_ID pred, NODE_ID succ) {
    EDGE_DATA_PTR data = _cont->New_edge_data();
    new (data) EDGE_DATA();
    data->Cfg_edge_info()->_pred = pred.Value();
    data->Cfg_edge_info()->_succ = succ.Value();
    return EDGE_PTR(EDGE(_cont, data));
  }

  CONTAINER* _cont;  //!< Mempool for nodes and edges in the CFG

};  // class CFG_BASE

}  // namespace util

}  // namespace air

#endif  // AIR_UTIL_CFG_BASE_H
