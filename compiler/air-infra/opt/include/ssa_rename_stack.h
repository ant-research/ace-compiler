//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_SSA_RENAME_STACK_H
#define AIR_OPT_SSA_RENAME_STACK_H

#include <unordered_map>
#include <unordered_set>

#include "air/base/analyze_ctx.h"
#include "air/core/default_handler.h"
#include "air/opt/ssa_container.h"
#include "air/opt/ssa_node_list.h"

namespace air {

namespace opt {

//! @brief Version Stack for SSA RENAME/VERIFY
class VERSION_STACK {
private:
  class SYM_VER_INFO;

  typedef air::util::MEM_POOL<512>                              MEM_POOL;
  typedef air::util::CXX_MEM_ALLOCATOR<uint32_t, MEM_POOL>      U32_ALLOCATOR;
  typedef air::util::CXX_MEM_ALLOCATOR<SYM_VER_INFO, MEM_POOL>  VER_ALLOCATOR;
  typedef air::util::CXX_MEM_ALLOCATOR<SYM_VER_INFO*, MEM_POOL> PTR_ALLOCATOR;
  typedef std::vector<uint32_t, U32_ALLOCATOR>                  U32_VECTOR;
  typedef std::vector<SYM_VER_INFO*, PTR_ALLOCATOR>             PTR_VECTOR;

  class SYM_VER_INFO {
  public:
    uint32_t   _last_ver;
    U32_VECTOR _stack;

    SYM_VER_INFO(MEM_POOL* mp) : _last_ver(0), _stack(U32_ALLOCATOR(mp)) {}

    static bool     Is_mark(uint32_t ver) { return ver > (uint32_t)INT32_MAX; }
    static bool     Is_ver(uint32_t ver) { return ver < (uint32_t)INT32_MAX; }
    static uint32_t To_mark(uint32_t id) {
      AIR_ASSERT(id < INT32_MAX);
      return id + INT32_MAX + 1;
    }

    bool     Empty() const { return _stack.size() == 0; }
    uint32_t Size() const { return _stack.size(); }

    uint32_t Next_version() { return ++_last_ver; }

    void Push_ver(SSA_VER_ID ver) {
      AIR_ASSERT(Is_ver(ver.Value()));
      _stack.push_back(ver.Value());
    }

    void Pop_ver(SSA_VER_ID ver) {
      AIR_ASSERT(!Empty());
      AIR_ASSERT(Is_ver(ver.Value()));
      AIR_ASSERT(_stack.back() == ver.Value());
      _stack.pop_back();
    }

    SSA_VER_ID Top_ver() const {
      AIR_ASSERT(!Empty());
      U32_VECTOR::const_reverse_iterator it = _stack.rbegin();
      while (it != _stack.rend() && Is_mark(*it)) {
        // skip all marks
        it++;
      }
      AIR_ASSERT(it != _stack.rend());
      return SSA_VER_ID(*it);
    }

    void Push_mark(air::base::NODE_ID node) {
      AIR_ASSERT(Is_ver(node.Value()));
      // make mark's value to be negative
      _stack.push_back(To_mark(node.Value()));
    }

    void Pop_mark(air::base::NODE_ID node) {
      AIR_ASSERT(!_stack.empty());
      AIR_ASSERT(Is_ver(node.Value()));
      while (Is_ver(_stack.back())) {
        _stack.pop_back();
      }
      AIR_ASSERT(To_mark(node.Value()) == _stack.back());
      _stack.pop_back();
    }
  };

public:
  VERSION_STACK(const SSA_CONTAINER* cont) : _ssa_cont(cont) {
    air::util::CXX_MEM_ALLOCATOR<PTR_VECTOR, MEM_POOL> ptr_vec_allocator(
        &_mpool);
    _stack = ptr_vec_allocator.Allocate(PTR_ALLOCATOR(&_mpool));
  }

public:
  //! @brief Initialize renaming stack
  void Initialize(air::base::NODE_ID node) {
    VER_ALLOCATOR allocator(&_mpool);
    uint32_t      num_sym = _ssa_cont->Num_sym();
    for (uint32_t i = 0; i < num_sym; ++i) {
      SYM_VER_INFO* info = allocator.Allocate(&_mpool);
      _stack->push_back(info);
      // push zero version to stack
      info->Push_ver(_ssa_cont->Sym(SSA_SYM_ID(i))->Zero_ver_id());
      info->Push_mark(node);
    }
  }

  //! @brief Finalize renaming stack to make sure all versions are pop'ed
  void Finalize(air::base::NODE_ID node) {
    for (auto it = _stack->begin(); it != _stack->end(); ++it) {
      (*it)->Pop_mark(node);
      // zero version should still be in stack
      AIR_ASSERT((*it)->Size() == 1);
      SSA_VER_ID top = (*it)->Top_ver();
      AIR_ASSERT(_ssa_cont->Ver_sym(top)->Zero_ver_id() == top);
    }
  }

  //! @brief Push a new version to renaming stack
  void Push_ver(SSA_VER_PTR ver) {
    AIR_ASSERT(ver->Sym_id().Value() < _stack->size());
    (*_stack)[ver->Sym_id().Value()]->Push_ver(ver->Id());
  }

  //! @brief Pop a version from renaming stack
  void Pop_ver(SSA_VER_PTR ver) {
    AIR_ASSERT(ver->Sym_id().Value() < _stack->size());
    (*_stack)[ver->Sym_id().Value()]->Pop_ver(ver->Id());
  }

  //! @brief Get current top version
  SSA_VER_ID Top_ver_id(SSA_SYM_ID sym) const {
    AIR_ASSERT(sym.Value() < _stack->size());
    return (*_stack)[sym.Value()]->Top_ver();
  }

  //! @brief Get current top version
  SSA_VER_PTR Top_ver(SSA_SYM_ID sym) const {
    return _ssa_cont->Ver(Top_ver_id(sym));
  }

  //! @brief Get next version number for symbol
  uint32_t Next_version(SSA_SYM_ID sym) {
    AIR_ASSERT(sym.Value() < _stack->size());
    return (*_stack)[sym.Value()]->Next_version();
  }

  //! @brief Push a block mark to renaming stack
  void Push_mark(air::base::NODE_PTR node) {
    AIR_ASSERT(SSA_CONTAINER::Has_phi(node));

    auto push_mark = [](PHI_NODE_PTR phi, PTR_VECTOR& stk,
                        air::base::NODE_ID node) {
      SSA_SYM_ID sym = phi->Sym_id();
      AIR_ASSERT(sym.Value() < stk.size());
      stk[sym.Value()]->Push_mark(node);
    };

    PHI_NODE_ID id = _ssa_cont->Node_phi(node->Id());
    PHI_LIST    list(_ssa_cont, id);
    list.For_each(push_mark, *_stack, node->Id());
  }

  //! @brief Pop block mark from renaming stack
  void Pop_mark(air::base::NODE_PTR node) {
    AIR_ASSERT(SSA_CONTAINER::Has_phi(node));

    auto pop_mark = [](PHI_NODE_PTR phi, PTR_VECTOR& stk,
                       air::base::NODE_ID node) {
      SSA_SYM_ID sym = phi->Sym_id();
      AIR_ASSERT(sym.Value() < stk.size());
      stk[sym.Value()]->Pop_mark(node);
    };

    PHI_NODE_ID id = _ssa_cont->Node_phi(node->Id());
    PHI_LIST    list(_ssa_cont, id);
    list.For_each(pop_mark, *_stack, node->Id());
  }

  //! @brief Get stack size
  uint32_t Size() const { return _stack->size(); }

private:
  const SSA_CONTAINER* _ssa_cont;
  MEM_POOL             _mpool;
  PTR_VECTOR*          _stack;  // SSA rename stack
};

}  // namespace opt

}  // namespace air

#endif  // AIR_OPT_SSA_RENAME_STACK_H
