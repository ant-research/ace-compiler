//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_NODE_LIST_H
#define AIR_UTIL_NODE_LIST_H

#include <iostream>
#include <sstream>
#include <string>

#include "air/base/id_wrapper.h"
#include "air/base/ptr_wrapper.h"
#include "air/util/debug.h"
namespace air {

namespace util {

//! @brief NODE_LIST implements a general linked list that organizes nodes using
//! unique IDs typename ID_TYPE specifies the type of ID typename PTR_TYPE
//! specifies the coresponding PTR type of ID_TYPE typename CONT_TYPE specifies
//! the container type
template <typename ID_TYPE, typename PTR_TYPE, typename CONT_TYPE>
class NODE_LIST {
public:
  NODE_LIST(CONT_TYPE* cont) : _cont(cont), _cnt(0) {}

  NODE_LIST(const CONT_TYPE* cont, ID_TYPE head)
      : _cont(const_cast<CONT_TYPE*>(cont)), _head(head) {
    if (_head != air::base::Null_id) {
      PTR_TYPE node = _cont->Node(_head);
      _tail         = _head;
      _cnt++;
      while (node->Next_id() != air::base::Null_id) {
        _tail = node->Next_id();
        node  = _cont->Node(_tail);
        _cnt++;
      }
    } else {
      _tail = air::base::Null_id;
    }
  }
  PTR_TYPE Tail() { return _cont->Node(_tail); }
  uint32_t Size() { return _cnt; }

  PTR_TYPE Find(PTR_TYPE node) const {
    ID_TYPE id = _head;
    while (id != air::base::Null_id) {
      PTR_TYPE cur_node = _cont->Node(id);
      AIR_ASSERT(!cur_node->Is_null());
      if (node->Match(cur_node)) {
        return cur_node;
      }
      id = cur_node->Next_id();
    }
    return PTR_TYPE();
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
      _cont->Node(node)->Set_next(_head);
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

  ID_TYPE Insert_before(ID_TYPE node, ID_TYPE pos_node) {
    ID_TYPE  id       = _head;
    ID_TYPE  prev     = id;
    PTR_TYPE node_ptr = _cont->Node(node);
    while (id != air::base::Null_id) {
      PTR_TYPE cur_node = _cont->Node(id);
      if (id == pos_node) {
        node_ptr->Set_next(id);
        if (id == _head) {
          _head = node;
        } else {
          _cont->Node(prev)->Set_next(node);
        }
        break;
      }
      prev = id;
      id   = cur_node->Next_id();
    }

    if (id != pos_node) {
      AIR_ASSERT_MSG(false, "pos_node is not found in current chain");
    }
    return _head;
  }

  // Return new head
  ID_TYPE Remove(ID_TYPE node) {
    ID_TYPE  id       = _head;
    ID_TYPE  prev     = id;
    PTR_TYPE node_ptr = _cont->Node(node);
    while (id != air::base::Null_id) {
      PTR_TYPE cur_node = _cont->Node(id);
      if (id == node) {
        if (id == _head) {
          _head = cur_node->Next_id();
        } else {
          _cont->Node(prev)->Set_next(cur_node->Next_id());
        }
        cur_node->Set_next(ID_TYPE());
        break;
      }
      prev = id;
      id   = cur_node->Next_id();
    }
    if (id != node) {
      AIR_ASSERT_MSG(false, "node is not found in current chain");
    }
    return _head;
  }

  void Print(std::ostream& os, uint32_t indent = 0) const {
    auto print = [](PTR_TYPE ptr, std::ostream& os, uint32_t indent) {
      ptr->Print(os, indent);
      os << std::endl;
    };
    For_each(print, os, indent);
  }

  void Print_id(std::ostream& os, uint32_t indent = 0) const {
    auto print = [](PTR_TYPE ptr, std::ostream& os, uint32_t indent) {
      os << ptr->Id().Value() << ",";
    };
    For_each(print, os, indent);
  }

  void Print() const {
    Print(std::cout, 0);
    std::cout << std::endl;
  }

  std::string To_str() const {
    std::stringbuf buf;
    std::ostream   os(&buf);
    Print(os, 0);
    return buf.str();
  }

private:
  CONT_TYPE* _cont;  //!< Container
  uint32_t   _cnt;   //!< Element count
  ID_TYPE    _head;  //!< Head of NODE list
  ID_TYPE    _tail;  //!< Tail of NODE list
};

}  // namespace util

}  // namespace air

#endif  // AIR_UTIL_NODE_LIST_H
