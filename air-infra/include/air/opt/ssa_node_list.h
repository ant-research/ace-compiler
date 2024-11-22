//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_SSA_NODE_LIST_H
#define AIR_OPT_SSA_NODE_LIST_H

#include "air/opt/ssa_container.h"

namespace air {

namespace opt {

//! @brief SSA PHI_LIST
//! Iterate PHI_NODE on the same list
template <typename ID_TYPE, typename PTR_TYPE>
class SSA_NODE_LIST {
public:
  SSA_NODE_LIST(SSA_CONTAINER* cont) : _cont(cont) {}

  SSA_NODE_LIST(const SSA_CONTAINER* cont, ID_TYPE head)
      : _cont(const_cast<SSA_CONTAINER*>(cont)), _head(head) {}

  PTR_TYPE Find(SSA_SYM_ID sym) const {
    ID_TYPE id = _head;
    while (id != air::base::Null_id) {
      PTR_TYPE node = _cont->Node(id);
      AIR_ASSERT(node != air::base::Null_ptr);
      if (node->Sym_id() == sym) {
        return node;
      }
      id = node->Next_id();
    }
    return air::base::Null_ptr;
  }

  template <typename F, typename... Args>
  void For_each(F&& f, Args&&... args) const {
    ID_TYPE id = _head;
    while (id != air::base::Null_id) {
      PTR_TYPE node = _cont->Node(id);
      AIR_ASSERT(node != air::base::Null_ptr);
      f(node, args...);
      id = node->Next_id();
    }
  }

  void Prepend(ID_TYPE node) {
    if (_head != air::base::Null_id) {
      AIR_ASSERT(_tail != air::base::Null_id);
      _cont->Node(_head)->Set_next(node);
      _head = node;
    } else {
      AIR_ASSERT(_tail == air::base::Null_id);
      _head = _tail = node;
    }
  }

  void Append(ID_TYPE node) {
    if (_tail != air::base::Null_id) {
      AIR_ASSERT(_head != air::base::Null_id);
      _cont->Node(_tail)->Set_next(node);
      _tail = node;
    } else {
      AIR_ASSERT(_head == air::base::Null_id);
      _head = _tail = node;
    }
  }

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

private:
  SSA_CONTAINER* _cont;  //!< SSA container
  ID_TYPE        _head;  //!< Head of SSA NODE list
  ID_TYPE        _tail;  //!< Tail of SSA NODE list
};

typedef SSA_NODE_LIST<MU_NODE_ID, MU_NODE_PTR>   MU_LIST;
typedef SSA_NODE_LIST<CHI_NODE_ID, CHI_NODE_PTR> CHI_LIST;
typedef SSA_NODE_LIST<PHI_NODE_ID, PHI_NODE_PTR> PHI_LIST;

}  // namespace opt

}  // namespace air

#endif  // AIR_OPT_SSA_NODE_LIST_H
