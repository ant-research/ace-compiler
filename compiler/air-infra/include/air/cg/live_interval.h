//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_CG_LIVE_INTERVAL_H
#define AIR_CG_LIVE_INTERVAL_H

#include <climits>
#include <cstdint>
#include <iostream>
#include <list>
#include <ostream>
#include <random>
#include <set>
#include <unordered_map>
#include <utility>
#include <vector>

#include "air/base/id_wrapper.h"
#include "air/base/st_decl.h"
#include "air/base/st_enum.h"
#include "air/cg/cgir_container.h"
#include "air/cg/cgir_decl.h"
#include "air/cg/cgir_enum.h"
#include "air/cg/targ_info.h"
#include "air/driver/driver_ctx.h"
#include "air/util/debug.h"
#include "air/util/messg.h"

namespace air {
namespace cg {

//! @brief Occur kind of a variable.
enum OCC_KIND : uint8_t {
  INVALID_OCC   = 0,
  OCC_INST_RES  = 1,
  OCC_INST_OPND = 2,
  OCC_PHI_RES   = 3,
  OCC_PHI_OPND  = 4,
};

//! @brief Location to store the value of a PREG.
enum class LOC_KIND {
  REGISTER,  // physical register
  STACK,     // stack
  SPM,       // scratchpad memory
};

//! @brief Slot kind.
enum SLOT_KIND : uint8_t {
  INVALID_SLOT = 0,
  // slot for basic block entry which is used by live ranges defined
  // by PHI nodes
  BLOCK_SLOT = 1,
  // slot for early clobber which interferes with normal live ranges
  // for normal def/use.
  EARLY_CLOBBER_SLOT = 2,
  // slot for pseudo-registers defined by non-PHI instruction.
  REGISTER_SLOT = 3,
  // pseudo-register defined in current slot is dead.
  DEAD_SLOT = 4,
};

//! @brief SLOT implementation: in the current implementation, a SLOT can point
//! to either a real instruction or a basic block(BB) entry. The SLOT index
//! increases by SLOT::DIST with each BB entry and instruction except for PHI.
//! All PHIs share the same index as their basic block entry.
class SLOT {
public:
  SLOT(void)
      : _inst(INST_ID()),
        _bb(BB_ID()),
        _idx(INVALID_MIN_ID),
        _kind(INVALID_SLOT) {}
  SLOT(INST_ID inst, BB_ID bb, uint32_t index, SLOT_KIND kind)
      : _inst(inst), _bb(bb), _idx(index), _kind(kind) {}
  SLOT(BB_ID bb, uint32_t index) : SLOT(INST_ID(), bb, index, BLOCK_SLOT) {}
  SLOT(const SLOT& o) : SLOT(o._inst, o._bb, o._idx, o._kind){};
  SLOT& operator=(const SLOT& o) {
    _inst = o._inst;
    _bb   = o._bb;
    _idx  = o._idx;
    _kind = o._kind;
    return *this;
  }
  ~SLOT(void) {}

  INST_ID  Inst(void) const { return _inst; }
  BB_ID    Bb(void) const { return _bb; }
  uint32_t Idx(void) const { return _idx; }
  uint32_t Kind(void) const { return _kind; }
  void     Set_kind(SLOT_KIND kind) { _kind = kind; }
  bool     Valid(void) const {
    return _idx != INVALID_MIN_ID &&
           (_inst != INST_ID() || _kind == BLOCK_SLOT) && _bb != BB_ID();
  }
  //! @brief Return the next slot of a DEAD_SLOT, which is also a DEAD_SLOT.
  SLOT Next(void) const {
    AIR_ASSERT(_kind == DEAD_SLOT);
    return SLOT(_inst, _bb, _idx + DIST, _kind);
  }
  bool operator<(const SLOT& o) const { return _idx < o._idx; }
  bool operator<=(const SLOT& o) const { return _idx <= o._idx; }
  bool operator>(const SLOT& o) const { return _idx > o._idx; }
  bool operator==(const SLOT& o) const { return _idx == o._idx; }
  bool operator!=(const SLOT& o) const { return !(*this == o); }

  //! @brief Invalid slot index
  static constexpr uint32_t INVALID_MIN_ID = 0;
  static constexpr uint32_t INVALID_MAX_ID = UINT32_MAX;
  //! @brief The distance between slots of successive instructions, with DIST
  //! serving as a mask. Ensure DIST is a power of 2.
  static constexpr uint32_t DIST = 2;

private:
  BB_ID     _bb;        //< CG basic block
  INST_ID   _inst;      //< CG instruction of slot
  uint32_t  _idx : 29;  //< slot index
  SLOT_KIND _kind : 3;  //< slot kind
};

//! @brief A Range denotes a left-closed, right-open SLOT interval
//! [_start, _end), consisting of a start SLOT and an end SLOT.
class RANGE {
public:
  RANGE(SLOT start, SLOT end) : _start(start), _end(end) {
    AIR_ASSERT_MSG((start.Idx() & (SLOT::DIST - 1U)) == 0,
                   "Step of start num is %u\n", SLOT::DIST);
    AIR_ASSERT_MSG(
        (end.Idx() > start.Idx()) || (end.Idx() == SLOT::INVALID_MIN_ID),
        "Not support range start and end at the same num, use _end = _start + "
        "1");
  }
  RANGE(void) : RANGE(SLOT(), SLOT()) {}
  RANGE(const RANGE& o) : RANGE(o.Start(), o.End()) {}
  RANGE& operator=(const RANGE& o) {
    _start = o._start;
    _end   = o._end;
    return *this;
  }
  ~RANGE() {}

  const SLOT& Start(void) const { return _start; }
  void        Set_start(const SLOT& slot) {
    AIR_ASSERT(slot.Valid());
    AIR_ASSERT(slot.Idx() < End().Idx());
    _start = slot;
  }
  const SLOT& End(void) const { return _end; }
  void        Set_end(const SLOT& slot) {
    AIR_ASSERT(slot.Valid());
    AIR_ASSERT(slot.Idx() >= Start().Idx());
    _end = slot;
  }
  //! @brief Return true if current range containing SLOT;
  //! otherwise, return false.
  bool Contain(const SLOT& slot) const {
    return Start().Idx() <= slot.Idx() && slot < End();
  }
  bool operator==(const RANGE& o) const {
    return Start() == o.Start() && End() == o.End();
  }
  bool operator!=(const RANGE& o) const { return !(*this == o); }
  bool operator<(const RANGE& o) const {
    uint64_t val   = ((uint64_t)(_start.Idx()) << 32U) + _end.Idx();
    uint64_t val_o = ((uint64_t)(o._start.Idx()) << 32U) + o._end.Idx();
    return val < val_o;
  }

private:
  SLOT _start;  //< start SLOT
  SLOT _end;    //< end SLOT
};

//! @brief
class OCCUR_INFO {
public:
  using LIST = std::list<OCCUR_INFO>;

  OCCUR_INFO(SLOT slot, uint16_t opnd_id, OCC_KIND kind)
      : _slot(slot), _opnd_idx(opnd_id), _kind(kind) {}
  OCCUR_INFO(const OCCUR_INFO& o)
      : OCCUR_INFO(o.Slot(), o.Opnd_idx(), o.Kind()) {}
  OCCUR_INFO& operator=(const OCCUR_INFO& o) {
    _slot     = o.Slot();
    _opnd_idx = o.Opnd_idx();
    _kind     = o.Kind();
    return *this;
  }
  ~OCCUR_INFO() {}

  //! @brief Return the OPND of current occur
  OPND_PTR Opnd(CGIR_CONTAINER* cont) const {
    INST_PTR inst = cont->Inst(_slot.Inst());
    switch (_kind) {
      case OCC_INST_RES:
      case OCC_PHI_RES:
        return inst->Res(_opnd_idx);
      case OCC_INST_OPND:
      case OCC_PHI_OPND:
        return inst->Opnd(_opnd_idx);
      default:
        AIR_ASSERT_MSG(false, "Not supported occur kind");
        return OPND_PTR();
    }
  }

  OCC_KIND    Kind(void) const { return _kind; }
  const SLOT& Slot(void) const { return _slot; }
  SLOT&       Slot(void) { return _slot; }
  INST_ID     Occ_inst(void) const { return _slot.Inst(); }
  BB_ID       Occ_bb(void) const { return _slot.Bb(); }
  uint32_t    Opnd_idx(void) const { return _opnd_idx; }

  bool operator<(const OCCUR_INFO& o) const {
    if (_slot != o._slot) return _slot < o._slot;
    if (_kind != o._kind) return _kind < o._kind;
    return _opnd_idx < o._opnd_idx;
  }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  OCCUR_INFO(void);

  SLOT     _slot;      //< SLOT of
  uint16_t _opnd_idx;  //< opnd index of occur in _inst
  OCC_KIND _kind;      //< occur kind: use/def.
};

//! @brief A pseudo register (PREG) live range records a slot RANGE and
//! occurrences in the current range (_occ). LIVE_RANGE offers APIs to check,
//! access, and modify the slot RANGE and occurrence information.
class LIVE_RANGE {
public:
  LIVE_RANGE(void) : _range() {}

  LIVE_RANGE(SLOT start, SLOT end, const OCCUR_INFO::LIST& occ_list)
      : _range(start, end), _occ(occ_list) {}

  LIVE_RANGE(const LIVE_RANGE& o) : LIVE_RANGE(o.Start(), o.End(), o._occ) {}

  LIVE_RANGE& operator=(const LIVE_RANGE& o) {
    _range.Set_start(o.Start());
    _range.Set_end(o.End());
    return *this;
  }
  ~LIVE_RANGE() {}

  void        Set_start(const SLOT& slot) { _range.Set_start(slot); }
  const SLOT& Start(void) const { return _range.Start(); }
  void        Set_end(const SLOT& slot) { _range.Set_end(slot); }
  const SLOT& End(void) const { return _range.End(); }
  //! @brief Return true if current range containing SLOT slot;
  //! otherwise, return false.
  bool Contain(const SLOT& slot) const { return _range.Contain(slot); }
  // bool Contain(uint32_t slot_id) const { return _range.Contain(slot_id); }
  //! @brief Return true if the current range intersects with live range o;
  //! otherwise, return false.
  bool Intersect(const LIVE_RANGE& o) const {
    return Contain(o.Start()) || o.Contain(Start());
  }
  //! @brief Return true Constraint: return true if current live range overlap
  //! with o or connect at one endpoint; otherwise, return false.
  bool Linkable(const LIVE_RANGE& o) const {
    return (Start() == o.End()) || (End() == o.Start()) || Intersect(o);
  }
  //! @brief Merge current live range with live range o.
  //! Constraint: the current live range must overlap with o or connect at one
  //! endpoint.
  void Merge(const LIVE_RANGE& o) {
    AIR_ASSERT_MSG(Linkable(o), "not mergeable live range");
    _range.Set_start(std::min(Start(), o.Start()));
    _range.Set_end(std::max(End(), o.End()));
    _occ.insert(_occ.end(), o._occ.begin(), o._occ.end());
    _occ.sort();
  }
  //! @brief Comparator of LIVE_RANGE.
  bool operator<(const LIVE_RANGE& o) const { return _range < o._range; }
  bool operator==(const LIVE_RANGE& o) const { return _range == o._range; }
  bool operator!=(const LIVE_RANGE& o) const { return !(*this == o); }

  //! @brief Return occurs within current live range.
  const OCCUR_INFO::LIST& Occur(void) const { return _occ; }
  OCCUR_INFO::LIST&       Occur(void) { return _occ; }

  //! @brief Return the cost increase from spilling the current live range.
  double Weight(void) const;
  void   Print(std::ostream& os, uint32_t indent) const;
  void   Print(void) const { Print(std::cout, 0); }

private:
  RANGE            _range;  //< SLOT range
  OCCUR_INFO::LIST _occ;    //< occurs within current live range
};

//! @brief DREG_INFO: physical registor or memory information of the allocated
//! PREG.
class DREG_INFO {
public:
  DREG_INFO(uint8_t rc, uint8_t reg_num)
      : _kind(LOC_KIND::REGISTER),
        _u({
            {rc, reg_num}
  }) {}
  ~DREG_INFO(void) {}

  void     Set_kind(LOC_KIND kind) { _kind = kind; }
  LOC_KIND Kind(void) const { return _kind; }
  uint32_t Reg_cls(void) const { return _u._reg[0]._reg_cls; }
  void     Clear_reg_num(void) {
    AIR_ASSERT(_kind == LOC_KIND::REGISTER);
    _u._reg[0]._reg_num = REG_UNKNOWN;
  }
  void Set_reg_num(uint8_t reg_num) {
    AIR_ASSERT(_u._reg[0]._reg_num == REG_UNKNOWN);
    _u._reg[0]._reg_num = reg_num;
    _kind               = LOC_KIND::REGISTER;
  }
  uint8_t Reg_num(void) const {
    AIR_ASSERT(_kind == LOC_KIND::REGISTER);
    return _u._reg[0]._reg_num;
  }
  void Set_stack_id(uint32_t stack) {
    _kind     = LOC_KIND::STACK;
    _u._stack = stack;
  }
  uint32_t Stack_id(void) const {
    AIR_ASSERT(_kind == LOC_KIND::STACK);
    return _u._stack;
  }
  void Set_spm(uint32_t spm) {
    _kind   = LOC_KIND::SPM;
    _u._spm = spm;
  }
  uint32_t Spm(void) const { return _u._spm; }
  void     Print(std::ostream& os, uint32_t indent) const;
  void     Print(void) const { Print(std::cout, 0); }

private:
  //! REQUIRED UNDEFINED UNWANTED methods
  DREG_INFO(void);
  DREG_INFO(const DREG_INFO&);
  DREG_INFO& operator=(const DREG_INFO&);

  LOC_KIND _kind;  //< Location kind
  union {
    struct {
      uint8_t _reg_cls;
      uint8_t _reg_num;
    } _reg[2];
    uint32_t _stack;  //< stack idx
    uint32_t _spm;    //< scratchpad memory idx.
  } _u;
};

//! @brief Information of a PREG includes the PREG_ID, the define instruction ID
//! for the current PREG value, and details of the allocated physical register,
//! stack, or SMP for the current value.
class PREG_INFO {
  using PREG_ID = base::PREG_ID;

public:
  //! @brief Hash used in mapping PREG_VALUE_INFO to its live range info.
  class HASH {
  public:
    uint64_t operator()(const PREG_INFO& v) const {
      uint64_t val = (((uint64_t)v._def_inst.Value()) << 32U) + v._preg.Index();
      return std::hash<uint64_t>{}(val);
    }
  };

  PREG_INFO(void) : PREG_INFO(PREG_ID(), INST_ID(), 0) {}
  PREG_INFO(PREG_ID preg, INST_ID def, uint32_t rc)
      : _reg_info(rc, REG_UNKNOWN), _preg(preg), _def_inst(def) {}

  PREG_INFO(const PREG_INFO& o)
      : PREG_INFO(o.Preg(), o.Def_inst(), o.Reg_cls()) {}
  PREG_INFO& operator=(const PREG_INFO& o) {
    _preg     = o.Preg();
    _def_inst = o.Def_inst();
    return *this;
  }
  ~PREG_INFO(void) {}

  INST_ID          Def_inst(void) const { return _def_inst; }
  PREG_ID          Preg(void) const { return _preg; }
  uint8_t          Reg_cls(void) const { return _reg_info.Reg_cls(); }
  uint8_t          Reg_num(void) const { return _reg_info.Reg_num(); }
  const DREG_INFO& Reg_info(void) const { return _reg_info; }
  DREG_INFO&       Reg_info(void) { return _reg_info; }

  //! @brief Comparator of PREG_VALUE_INFO.
  bool operator<(const PREG_INFO& o) const {
    if (_def_inst != o._def_inst) return _def_inst < o._def_inst;
    return _preg < o._preg;
  }
  bool operator==(const PREG_INFO& o) const {
    return Preg() == o.Preg() && Def_inst() == o.Def_inst();
  }
  bool operator!=(const PREG_INFO& o) const { return !(*this == o); }

  void Print(std::ostream& os, uint32_t indent) const;
  void Print(void) const { Print(std::cout, 0); }

private:
  DREG_INFO _reg_info;  //< Info of allocated physical register/stack/SMP.
  PREG_ID   _preg;      //< PREG ID
  INST_ID   _def_inst;  //< Define instruction ID
};

//! @brief Implementation of live interval of a PREG which consitsts the ranges
//! value of current PREG must be live and info of current PREG(_value).
class LIVE_INTERVAL {
  using PREG_ID  = base::PREG_ID;
  using PREG_PTR = base::PREG_PTR;

public:
  using LR_LIST       = std::list<LIVE_RANGE>;
  using LR_ITER       = LR_LIST::iterator;
  using LR_CONST_ITER = LR_LIST::const_iterator;
  using LIST          = std::list<LIVE_INTERVAL*>;

  LIVE_INTERVAL(CGIR_CONTAINER* cntr, PREG_INFO val)
      : _cont(cntr), _reg_info(val), _range() {}

  //! @brief Copy constructor
  LIVE_INTERVAL(const LIVE_INTERVAL& o)
      : _cont(o._cont), _reg_info(o._reg_info), _range() {
    AIR_ASSERT_MSG(o._range.empty(), "Only support copy empty live interval");
  }
  LIVE_INTERVAL& operator=(const LIVE_INTERVAL& o) {
    AIR_ASSERT_MSG(o._range.empty(), "Only support copy empty live interval");
    _cont     = o._cont;
    _reg_info = o._reg_info;
    return *this;
  }

  //! @brief Return the first live range of which end > slot_id.
  //! slot is large than any existing live range return nullptr.
  const LIVE_RANGE* Advance_to(const SLOT slot) const {
    for (const LIVE_RANGE& lr : _range) {
      if (lr.End() > slot) return &lr;
    }
    return nullptr;
  }
  LIVE_RANGE* Advance_to(const SLOT& slot) {
    return const_cast<LIVE_RANGE*>(
        ((const LIVE_INTERVAL*)this)->Advance_to(slot));
  }

  //! @brief Return true if live range containing slot; otherwise, return false.
  bool Contain(const SLOT& slot) const {
    for (const LIVE_RANGE& lr : _range) {
      if (lr.Start() > slot) return false;
      if (lr.Contain(slot)) return true;
    }
    return false;
  }
  //! @brief Return the first intersection slot idx between the current live
  //! interval and li. If no intersection is found, return a large value
  //! (UINT_MAX) that is unreachable in any function.
  uint32_t Intersect_pos(const LIVE_INTERVAL* li) const;
  //! @brief Return true if the current live interval overlaps with interval li;
  //! otherwise, return false.
  bool Intersect(const LIVE_INTERVAL* li) const {
    return Intersect_pos(li) != SLOT::INVALID_MAX_ID;
  }

  //! @brief Erase live range at iter and return iterator of the next live
  //! range.
  LR_ITER Erase(LR_ITER iter) { return _range.erase(iter); }
  //! @brief Reset the start SLOT of the live range containing slot to slot.
  void Update_start_slot(SLOT slot);
  //! @brief Merge lr and the later live ranges that intersect or link to lr.
  bool Merge_succ(LR_LIST::iterator lr_iter);
  //! @brief Add lr into _range and merge it with any mergeable live ranges
  //! within _range.
  void Add_range(const LIVE_RANGE& lr);

  //! @brief Merge live interval li into current live interval.
  void Merge(const LIVE_INTERVAL& li) {
    if (li.Empty()) return;
    for (const LIVE_RANGE& lr : li._range) {
      Add_range(lr);
    }
  }

  LR_ITER        Begin_range(void) { return _range.begin(); }
  LR_ITER        End_range(void) { return _range.end(); }
  LR_LIST&       Range(void) { return _range; }
  const LR_LIST& Range(void) const { return _range; }
  bool           Empty(void) const { return _range.empty(); }
  uint32_t       Size(void) const { return _range.size(); }
  const SLOT&    Start(void) const {
    AIR_ASSERT(!_range.empty());
    return _range.front().Start();
  }
  const SLOT& End(void) const {
    AIR_ASSERT(!_range.empty());
    return _range.back().End();
  }

  const PREG_INFO& Preg_info(void) const { return _reg_info; }
  PREG_INFO&       Preg_info(void) { return _reg_info; }
  PREG_ID          Preg_id(void) const { return _reg_info.Preg(); }
  PREG_PTR Preg(void) const { return _cont->Func_scope()->Preg(Preg_id()); }
  INST_ID  Def_inst_id(void) const { return _reg_info.Def_inst(); }
  INST_PTR Def_inst(void) const;

  //! @brief Return register class of physical register.
  uint8_t Reg_cls(void) const { return _reg_info.Reg_cls(); }
  //! @brief Return the allocated physical register for current live
  //! interval.
  uint8_t Reg_num(void) const { return _reg_info.Reg_num(); }
  //! @brief Set the allocated physical register for current live interval.
  void Set_reg_num(uint8_t reg_num) {
    _reg_info.Reg_info().Set_reg_num(reg_num);
  }
  //! @brief Return the SPM allocated for current live interval.
  uint32_t Spm(void) const { return _reg_info.Reg_info().Spm(); }
  //! @brief Set the SPM allocated for current live interval.
  void     Set_spm(uint32_t spm) { _reg_info.Reg_info().Set_spm(spm); }
  LOC_KIND Loc_kind(void) const { return _reg_info.Reg_info().Kind(); }

  //! @brief Comparator of LIVE_INTERVAL
  bool operator<(const LIVE_INTERVAL& o) const {
    if (_range != o._range) return _range < o._range;
    return _reg_info < o._reg_info;
  }

  void Print(std::ostream& os, uint32_t indent = 0) const;
  void Print(void) const { Print(std::cout, 0); }

  //! @brief Check if live ranges statisfy follow constraints:
  //! 1. Live ranges in _range is ordered according to start SLOT;
  //! 2. No mergeable live range exists;
  //! 3. No live range intersects with another one.
  bool Verify(void) const;

private:
  // REQUIRED UNDEFINED UNWANTED methods
  LIVE_INTERVAL(void);

  CGIR_CONTAINER* Container(void) const { return _cont; }

  CGIR_CONTAINER* _cont;      //< CG IR container
  LR_LIST         _range;     //< ranges in current live interval
  PREG_INFO       _reg_info;  //< the value current live interval belongs to
};

//! @brief LIVE_INTERVAL_INFO contains the LIVE_INTERVAL of each PREG defined
//! or used in the current function. Additionally, it records the SLOT of each
//! instruction and the RANGE of each basic block.
class LIVE_INTERVAL_INFO {
  friend class LIVE_INTERVAL_BUILDER;

public:
  using INST2SLOT = std::vector<SLOT>;
  using BB2LR     = std::vector<RANGE>;
  using VALUE2LI =
      std::unordered_map<PREG_INFO, LIVE_INTERVAL, PREG_INFO::HASH>;
  using LI_LIST       = LIVE_INTERVAL::LIST;
  using ITER          = LI_LIST::iterator;
  using LI_CONST_ITER = LI_LIST::const_iterator;

  explicit LIVE_INTERVAL_INFO(CGIR_CONTAINER* cont) : _cont(cont) {
    _bb_range.resize(_cont->Bb_count(), RANGE());
    _inst_slot.resize(_cont->Inst_count(), SLOT());
  }
  ~LIVE_INTERVAL_INFO() {
    _bb_range.clear();
    _inst_slot.clear();
  }

  //! @brief Return all the live intervals of current function
  const LI_LIST& Live_interval(void) const { return _li; }
  LI_LIST&       Live_interval(void) { return _li; }

  //! @brief Return an existing live interval of the PREG.
  LIVE_INTERVAL& Live_interval(const PREG_INFO& val) {
    VALUE2LI::iterator iter = _value_li.find(val);
    AIR_ASSERT(iter != _value_li.end());
    return iter->second;
  }
  //! @brief Return the existing live interval of PREG.
  //! If non-exist, create a new one and return it.
  LIVE_INTERVAL& Get_live_interval(PREG_INFO val) {
    LIVE_INTERVAL                       li(_cont, val);
    std::pair<VALUE2LI::iterator, bool> res = _value_li.emplace(val, li);
    return res.first->second;
  }

  LI_CONST_ITER Begin_li(void) const { return _li.begin(); }
  ITER          Begin_li(void) { return _li.begin(); }
  LI_CONST_ITER End_li(void) const { return _li.end(); }
  ITER          End_li(void) { return _li.end(); }

  //! @brief Return the range of basic block bb.
  const RANGE& Bb_range(BB_ID bb) const {
    AIR_ASSERT(bb.Value() < _bb_range.size());
    return _bb_range[bb.Value()];
  }
  //! @brief Setup the range for basic block bb.
  void Set_bb_range(BB_ID bb, const RANGE& rg) {
    AIR_ASSERT(bb.Value() < _bb_range.size());
    _bb_range[bb.Value()] = rg;
  }
  //! @brief Return the SLOT of instruction inst.
  const SLOT& Inst_slot(INST_ID inst) const {
    AIR_ASSERT(inst.Value() < _inst_slot.size());
    return _inst_slot[inst.Value()];
  }
  //! @brief Setup the the SLOT for instruction inst.
  void Set_inst_slot(INST_ID inst, const SLOT& slot) {
    if (inst.Value() >= _inst_slot.size()) {
      _inst_slot.resize(inst.Value() + 1, SLOT());
    }
    _inst_slot[inst.Value()] = slot;
  }

  void Print(std::ostream& os, uint32_t indent = 0) const;
  void Print(void) const { Print(std::cout, 0); }

  //! @brief Verify that live intervals in _li are ordered by their start
  //! positions.
  bool Verify(void) const;

private:
  // REQUIRED UNDEFINED UNWANTED methods
  LIVE_INTERVAL_INFO(void);
  LIVE_INTERVAL_INFO(const LIVE_INTERVAL_INFO&);
  LIVE_INTERVAL_INFO& operator=(const LIVE_INTERVAL_INFO&);

  //! @brief Update _li to include all the live intervals contained in _value_li
  void Update(void);

  CGIR_CONTAINER* _cont;       //< CG IR container of current function.
  LI_LIST         _li;         //< live intervals of current function.
  VALUE2LI        _value_li;   //< live interval of each value.
  INST2SLOT       _inst_slot;  //< slot of each cg instruction.
  BB2LR           _bb_range;   //< live range of each cg basic block.
};

}  // namespace cg
}  // namespace air
#endif  // AIR_CG_LIVE_INTERVAL_H