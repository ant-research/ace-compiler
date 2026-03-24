//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef CG_CGIR_SSA_H
#define CG_CGIR_SSA_H

#include <unordered_map>
#include <unordered_set>

#include "air/cg/analyze_ctx.h"
#include "air/cg/cgir_container.h"
#include "air/cg/cgir_util.h"
#include "air/cg/isa_core.h"

namespace air {

namespace cg {

//! @brief Context to build SSA on top of CGIR
//! Context help collect def/use info for pesudo/dedicated registers, then
//! performing phi insertion by creating PHI_INST and inserting to dominance
//! frontier of the def BB. Renaming phase goes after phi insertion and context
//! manages the renaming stack and creates new OPND for each def.
class SSA_BUILD_CTX : public ANALYZE_CTX {
private:
  // unify register key (rkey) for both pseudo and dedicated register so that 1
  // set/map can be used for both registers
  // pesudo register: 0 - 0x7fffffff, uses PREG_ID value directly
  // dedicated register: 0x80000000 - 0xffffffff, uses DREG_FLAG + (reg_class <<
  // 16) + reg_number
  static constexpr uint32_t DREG_FLAG = 0x80000000;

  // convert PREG_ID to rkey
  static uint32_t To_rkey(uint32_t preg) {
    AIR_ASSERT(preg < DREG_FLAG);
    return preg;
  }

  // convert register class and number to rkey. ISA?
  static uint32_t To_rkey(uint8_t rc, uint8_t rnum) {
    return DREG_FLAG + ((uint32_t)rc << 16) + rnum;
  }

  // convert OPND to rkey
  // NOTE: opnd doesn't have ISA info. what if rc is different between ISAs?
  static uint32_t To_rkey(OPND_PTR opnd) {
    AIR_ASSERT(opnd->Is_register());
    return opnd->Is_preg() ? To_rkey(opnd->Preg().Value())
                           : To_rkey(opnd->Reg_class(), opnd->Reg_num());
  }

private:
  // memory pool
  typedef air::util::MEM_POOL<512> MEM_POOL;

  // U32_SET
  typedef air::util::CXX_MEM_ALLOCATOR<uint32_t, MEM_POOL> U32_ALLOCATOR;
  typedef std::unordered_set<uint32_t, std::hash<uint32_t>,
                             std::equal_to<uint32_t>, U32_ALLOCATOR>
      U32_SET;

  // vector of U32_SET, to track registers defined in each BB
  typedef air::util::CXX_MEM_ALLOCATOR<U32_SET*, MEM_POOL> U32_SET_ALLOCATOR;
  typedef std::vector<U32_SET*, U32_SET_ALLOCATOR>         U32_SET_VEC;

  // map from BB_ID to U32_SET to track registers which needs PHI INST in the
  // beginning of the BB
  typedef std::pair<uint32_t, U32_SET*> U32_SET_PAIR;
  typedef air::util::CXX_MEM_ALLOCATOR<U32_SET_PAIR, MEM_POOL>
      U32_SET_PAIR_ALLOCATOR;
  typedef std::unordered_map<uint32_t, U32_SET*, std::hash<uint32_t>,
                             std::equal_to<uint32_t>, U32_SET_PAIR_ALLOCATOR>
      U32_SET_MAP;

  // rename stack for each rkey
  struct RENAME_STACK {
    // special constant for mark in renaming stack
    static constexpr uint32_t MARK_FLAG = 0x80000000;

    uint32_t              _rkey;   // rkey for pseudo/dedicated register
    uint32_t              _zero;   // OPND_ID for zero version
    uint32_t              _cver;   // current version
    std::vector<uint32_t> _stack;  // OPND_ID stack for renaming

    // convert id to special mark value
    static uint32_t To_mark(uint32_t id) {
      AIR_ASSERT(id < MARK_FLAG);
      return id + MARK_FLAG + 1;
    }

    // check if the id is special mark
    static bool Is_mark(uint32_t id) { return id > MARK_FLAG; }

    // construction a RENAME_STACK
    RENAME_STACK(uint32_t rkey, OPND_ID zero)
        : _rkey(rkey), _zero(zero.Value()), _cver(0) {
      _stack.push_back(zero.Value());
    }

    // return the next version number
    uint32_t Next_ver() { return ++_cver; }

    // return the default ZERO OPND_ID
    OPND_ID Zero_opnd() { return OPND_ID(_zero); }

    // check if stack is empty
    bool Empty() const { return _stack.size() == 1; }

    // push a real version to stack
    void Push(OPND_ID id) {
      AIR_ASSERT(id.Value() < MARK_FLAG);
      _stack.push_back(id.Value());
    }

    // return top version from stack. if top version is mark, ignore the mark
    // and return the first real version under the mark
    OPND_ID Top() const {
      AIR_ASSERT(!_stack.empty());
      std::vector<uint32_t>::const_reverse_iterator it = _stack.rbegin();
      // skip marks
      while (it != _stack.rend() && Is_mark(*it)) {
        ++it;
      }
      AIR_ASSERT(it != _stack.rend());
      return OPND_ID(*it);
    }

    // push mark to stack
    void Push_mark(uint32_t id) {
      AIR_ASSERT(id < MARK_FLAG);
      _stack.push_back(To_mark(id));
    }

    // pop mark from stack
    void Pop_mark(uint32_t id) {
      AIR_ASSERT(id < MARK_FLAG);
      while (!_stack.empty() && !Is_mark(_stack.back())) {
        _stack.pop_back();
      }
      AIR_ASSERT(!_stack.empty() && _stack.back() == To_mark(id));
      _stack.pop_back();
    }

    // update bottom OPND_ID from original ZERO to CHI result
    void Update_bottom(OPND_ID res) {
      AIR_ASSERT(_stack.size() > 0 && _stack[0] == _zero);
      _stack[0] = res.Value();
    }
  };

  // map from rkey to RENAME_STACK as whole renaming stack
  typedef std::pair<uint32_t, RENAME_STACK*> U32_STK_PAIR;
  typedef air::util::CXX_MEM_ALLOCATOR<U32_STK_PAIR, MEM_POOL>
      U32_STK_PAIR_ALLOCATOR;
  typedef std::unordered_map<uint32_t, RENAME_STACK*, std::hash<uint32_t>,
                             std::equal_to<uint32_t>, U32_SET_PAIR_ALLOCATOR>
      U32_STK_MAP;

private:
  // find renaming stack from _stk_map
  RENAME_STACK* Opnd_stack(OPND_PTR opnd) {
    U32_STK_MAP::iterator it = _stk_map->find(To_rkey(opnd));
    AIR_ASSERT(it != _stk_map->end());
    return it->second;
  }

  // find ZERO OPND_ID from _stk_map
  OPND_ID Zero_opnd(uint32_t rkey) {
    U32_STK_MAP::iterator it = _stk_map->find(rkey);
    AIR_ASSERT(it != _stk_map->end());
    return it->second->Zero_opnd();
  }

  // create a U32_SET object
  U32_SET* New_u32_set() {
    air::util::CXX_MEM_ALLOCATOR<U32_SET, MEM_POOL> u32_a(&_mpool);
    return u32_a.Allocate(13, std::hash<uint32_t>(), std::equal_to<uint32_t>(),
                          U32_ALLOCATOR(&_mpool));
  }

  // create PHI instruction for given reg
  INST_PTR New_phi_inst(air::base::SPOS spos, OPND_ID res, uint32_t preds) {
    return _cntr->New_phi(spos, res, preds);
  }

  // create CHI instruction for given reg
  INST_PTR New_chi_inst(OPND_ID res, OPND_ID opnd) {
    BB_PTR   entry = _cntr->Entry();
    INST_PTR chi   = _cntr->New_chi(entry->Spos(), res, opnd);
    entry->Append(chi);
    return chi;
  }

  // create RENAME_STACK for given rkey and opnd_id
  RENAME_STACK* New_rename_stack(uint32_t rkey, OPND_ID zero) {
    air::util::CXX_MEM_ALLOCATOR<RENAME_STACK, MEM_POOL> rvs_a(&_mpool);
    return rvs_a.Allocate(rkey, zero);
  }

  // check if all stack entries are empty
  bool Stack_empty() const {
    for (auto&& it : *_stk_map) {
      AIR_ASSERT(it.second != nullptr);
      AIR_ASSERT(it.first == it.second->_rkey);
      if (!it.second->Empty()) {
        return false;
      }
    }
    return true;
  }

  // push marks to all stack entries to start handling the BB
  void Push_stk_mark(uint32_t id) {
    for (auto&& it : *_stk_map) {
      it.second->Push_mark(id);
    }
  }

  // pop marks from all stack entries. called after the BB is handled
  void Pop_stk_mark(uint32_t id) {
    for (auto&& it : *_stk_map) {
      it.second->Pop_mark(id);
    }
  }

  // update def_inst and version, push a new version to stack
  OPND_ID Push_ver(OPND_PTR opnd, INST_ID def) {
    RENAME_STACK* stk = Opnd_stack(opnd);
    OPND_PTR      res = _cntr->Clone(opnd);
    res->Set_def(def, stk->Next_ver());
    stk->Push(res->Id());
    return res->Id();
  }

  // return top version in stack. if top entry in stack is mark, ignore
  // the mark and find real top version under mark
  OPND_ID Top_ver(OPND_PTR opnd) {
    RENAME_STACK* stk = Opnd_stack(opnd);
    OPND_ID       top = stk->Top();
    // check if top is a real def
    if (top != stk->Zero_opnd()) {
      return top;
    }
    // use before any def, create a CHI node in Entry BB, return CHI result
    OPND_PTR res = _cntr->Clone(opnd);
    INST_PTR chi = New_chi_inst(res->Id(), top);
    res->Set_def(chi->Id(), stk->Next_ver());
    // update renaming stack bottom with new CHI result to be reused later
    stk->Update_bottom(res->Id());
    return res->Id();
  }

  // rename PHI operand at pos in bb with top version from stack
  void Rename_succ_opnd(BB_PTR bb, uint32_t pos) {
    INST_PTR inst = bb->First();
    while (inst != air::base::Null_ptr && inst->Opcode() == CORE_PHI) {
      AIR_ASSERT(inst->Res_count() == 1 && inst->Opnd_count() > pos);
      OPND_PTR res = inst->Res(0);
      inst->Set_opnd(pos, Top_ver(res));
      inst = inst->Next();
    }
  }

public:
  //! @brief handle preg/dreg definition
  void Handle_def(OPND_PTR opnd, BB_ID def_bb) {
    AIR_ASSERT(opnd->Is_register());
    AIR_ASSERT(def_bb.Value() < _def_vec->size());
    uint32_t rkey = To_rkey(opnd);
    if (_trace_defuse) {
      printf("DEF: %d --> %d\n", rkey, opnd->Id().Value());
    }
    // add bb->rkey to _def_vec for later phi insertion phase
    U32_SET*& rset = (*_def_vec)[def_bb.Value()];
    if (rset == nullptr) {
      rset = New_u32_set();
    }
    rset->insert(rkey);
    // init rkey->stack to _stk_map for later renaming phase
    // AIR_ASSERT(_stk_map->find(rkey) == _stk_map->end() ||
    //           (*_stk_map)[rkey]->Zero_opnd() == opnd->Id());
    if (_stk_map->find(rkey) == _stk_map->end()) {
      (*_stk_map)[rkey] = New_rename_stack(rkey, opnd->Id());
    }
  }

  //! @brief handle preg/drep use
  void Handle_use(OPND_PTR opnd) {
    AIR_ASSERT(opnd->Is_register());
    uint32_t rkey = To_rkey(opnd);
    if (_trace_defuse) {
      printf("USE: %d --> %d\n", rkey, opnd->Id().Value());
    }
    if (_stk_map->find(rkey) == _stk_map->end()) {
      (*_stk_map)[rkey] = New_rename_stack(rkey, opnd->Id());
    }
  }

  //! @brief phi insertion
  void Insert_phi();

public:
  //! @brief initialize rename phase
  void Init_rename() { AIR_ASSERT(Stack_empty()); }

  //! @brief finalize rename phase
  void Fini_rename() { AIR_ASSERT(Stack_empty()); }

  //! @brief start handle a bb on DOM tree before inst list and children
  //! are handled:
  //! 1. push mark
  //! 2. handle inst list
  //!    2.1 rename rhs with top version from stack for normal inst except PHI
  //!    2.2 push lhs of all inst to stack
  //! 3. rename succ phi opnds at given position with top version from stack
  void Handle_bb(uint32_t id);

  // finish a bb after all children on DOM tree is handled:
  // 1. pop mark
  void Finish_bb(uint32_t id) { Pop_stk_mark(id); }

public:
  //! @brief constructor, initialize internal data structures
  SSA_BUILD_CTX(CGIR_CONTAINER* cntr, const air::util::DOM_INFO* dom_info);

private:
  MEM_POOL                   _mpool;         // memory pool
  CGIR_CONTAINER*            _cntr;          // CGIR container
  const air::util::DOM_INFO* _dom;           // DOM_INFO
  U32_SET_VEC*               _def_vec;       // rkey defined in each bb
  U32_STK_MAP*               _stk_map;       // rkey -> renaming stack map
  U32_SET_MAP*               _phi_map;       // bb -> rkey set for phi insertion
  bool                       _trace_defuse;  // trace rkey collection
  bool                       _trace_insert;  // trace phi insertion
  bool                       _trace_rename;  // trace rename
};

//! @brief PHI_INSERT_HANDLER
//! Check all operands and results of an inst and collect all pseudo/dedicated
//! registers used and defined by the inst for later phi insertion phase
class PHI_INSERT_HANDLER {
public:
  //! @brief handle the instruction
  template <typename VISITOR>
  void Handle(VISITOR* visitor, INST_PTR inst) {
    uint32_t res_cnt  = inst->Res_count();
    uint32_t opnd_cnt = inst->Opnd_count();
    if (res_cnt + opnd_cnt == 0) {
      return;
    }
    SSA_BUILD_CTX& ctx = visitor->Context();
    for (uint32_t i = 0; i < inst->Opnd_count(); ++i) {
      OPND_PTR opnd = inst->Opnd(i);
      if (opnd->Is_register()) {
        ctx.Handle_use(inst->Opnd(i));
      }
    }
    BB_ID bb = ctx.Top()->Id();
    for (uint32_t i = 0; i < inst->Res_count(); ++i) {
      AIR_ASSERT(inst->Res(i)->Is_register());
      ctx.Handle_def(inst->Res(i), bb);
    }
  }
};

//! @brief RENAME_HANDLER
//! Called when enter and exit from a node on DOM tree and perform SSA renaming
//! algorithm in callbacks
class RENAME_HANDLER {
public:
  //! @brief Push_mark
  //! called when enter a BB on DOM tree before inst list in the bb and
  //! children on DOM tree are handled. Once returned, DOM_VISITOR will visit
  //! all children BBs
  template <typename VISITOR>
  void Push_mark(VISITOR* visitor, uint32_t id) {
    visitor->Context().Handle_bb(id);
  }

  //! @brief Pop_mark
  //! called when exit a BB on DOM tree after all children on DOM tree are
  //! handled.
  template <typename VISITOR>
  void Pop_mark(VISITOR* visitor, uint32_t id) {
    visitor->Context().Finish_bb(id);
  }
};

}  // namespace cg

}  // namespace air

#endif  // CG_CGIR_SSA_H
