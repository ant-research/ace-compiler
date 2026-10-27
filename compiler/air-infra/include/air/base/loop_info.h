//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_LOOP_INFO_H
#define AIR_BASE_LOOP_INFO_H

#include <sys/types.h>

#include <cstdint>
#include <set>
#include <unordered_map>

#include "air/base/bb_enum.h"
#include "air/base/container.h"
#include "air/base/id_wrapper.h"
#include "air/base/ptr_wrapper.h"
#include "air/util/arena.h"
#include "air/util/debug.h"
#include "air/util/mem_allocator.h"
#include "air/util/mem_pool.h"

namespace air {
namespace base {

//! @brief LOOP_INFO_DATA holds the data of a loop's information
//! -> preheader -> header -> ... -> back -> tail
//!                ^       <LOOP_BODY>      |
//!                |------------------------|
template <typename BB_ID>
class LOOP_INFO_DATA {
public:
  LOOP_INFO_DATA(uint32_t back_num, uint32_t tail_num)
      : _parent(UINT32_MAX),
        _preheader(BB_ID()),
        _header(BB_ID()),
        _back_num(back_num),
        _tail_num(tail_num) {
    for (uint32_t id = 0; id < back_num; ++id) {
      Set_back(BB_ID(), id);
    }
    for (uint32_t id = 0; id < tail_num; ++id) {
      Set_tail(BB_ID(), id);
    }
  }
  ~LOOP_INFO_DATA() {}

  void Init(BB_ID preheader, BB_ID header, BB_ID back, BB_ID tail,
            uint32_t parent) {
    Set_preheader(preheader);
    Set_header(header);
    Set_parent(parent);
    Set_back(back);
    Set_tail(tail);
  }

  void     Set_parent(uint32_t p) { _parent = p; }
  uint32_t Parent(void) const { return _parent; }
  void     Set_preheader(BB_ID bb) { _preheader = bb; }
  BB_ID    Preheader(void) const { return _preheader; }
  void     Set_header(BB_ID bb) { _header = bb; }
  BB_ID    Header(void) const { return _header; }
  void     Set_back(BB_ID bb, uint32_t back_id = 0) {
    AIR_ASSERT(back_id < _back_num);
    _back_tail[back_id] = bb;
  }
  BB_ID Back(uint32_t back_id = 0) const {
    AIR_ASSERT(back_id < _back_num);
    return _back_tail[back_id];
  }
  void Set_tail(BB_ID bb, uint32_t tail_id = 0) {
    AIR_ASSERT(tail_id < _tail_num);
    _back_tail[_back_num + tail_id] = bb;
  }
  BB_ID Tail(uint32_t tail_id = 0) const {
    AIR_ASSERT(tail_id < _tail_num);
    return _back_tail[_back_num + tail_id];
  }

private:
  uint32_t _parent;       //!< Information about the parent loop
  BB_ID    _preheader;    //!< Preheader basic block
  BB_ID    _header;       //!< Header basic block
  uint32_t _back_num;     //!< Number of back basic block
  uint32_t _tail_num;     //!< Number of tail basic block
  BB_ID    _back_tail[];  //!< Back and tail basic blocks
};

//! @brief LOOP_INFO the wrapper for LOOP_INFO_DATA
template <typename LOOP_MGR, typename BB_ID, typename BB_PTR>
class LOOP_INFO {
public:
  using DATA     = LOOP_INFO_DATA<BB_ID>;
  using ID       = base::ID<DATA>;
  using PTR      = base::PTR<LOOP_INFO>;
  using DATA_PTR = base::PTR_FROM_DATA<DATA>;
  using BBID_SET = std::set<BB_ID>;

  LOOP_INFO(void) : _mgr(nullptr), _data(DATA_PTR()) {}
  LOOP_INFO(const LOOP_MGR* mgr, DATA_PTR data) : _mgr(mgr), _data(data) {}

  LOOP_MGR* Container() const { return _mgr->Container(); }
  DATA_PTR  Data() const { return _data; }
  ID        Id(void) const { return _data.Id(); }
  bool      Is_null(void) const { return _data.Is_null(); }
  void      Set_parent(ID p) { _data.Set_parent(p.Value()); }
  ID        Parent_id(void) const { return ID(_data->Parent()); }
  PTR       Parent(void) const { return _mgr->Loop(Parent_id()); }

  void   Set_preheader(BB_ID bb) { Data()->Set_preheader(bb); }
  BB_ID  Preheader_id(void) const { return Data()->Preheader(); }
  BB_PTR Preheader(void) const { return Container()->Bb(Preheader_id()); }
  void   Set_header(BB_ID bb) { Data()->Set_header(bb); }
  BB_ID  Header_id(void) const { return Data()->Header(); }
  BB_PTR Header(void) const { return Container()->Bb(Header_id()); }
  void   Set_back(BB_ID bb, uint32_t back_id = 0) {
    Data()->Set_back(bb, back_id);
  }
  BB_ID  Back_id(uint32_t back_id = 0) const { return Data()->Back(); }
  BB_PTR Back(uint32_t back_id = 0) const { return Container()->Bb(Back_id()); }
  void   Set_tail(BB_ID bb, uint32_t tail_id = 0) {
    Data()->Set_tail(bb, tail_id);
  }
  BB_ID  Tail_id(uint32_t tail_id = 0) const { return Data()->Tail(tail_id); }
  BB_PTR Tail(uint32_t tail_id = 0) const {
    return Container()->Bb(Tail_id(tail_id));
  }
  //! @brief Return true if current loop is nested in loop p;
  //! otherwise, return false.
  bool Is_inner_loop(ID p) const {
    if (Parent_id() == base::Null_id) return false;
    if (Parent_id() == p) {
      return true;
    }
    return Parent()->Is_inner_loop(p);
  }
  bool Is_inner_loop(PTR p) const { return Is_inner_loop(p->Id()); }

private:
  const LOOP_MGR* _mgr;   //!< IR Container
  DATA_PTR        _data;  //!< Loop information data PTR
};

//! @brief Implement LOOP_INFO manager with APIs to create and access loop
//! information for each basic block.
//! Constraints: Loop info is utilized in phases of low-level IR where loops
//! consist of a series of basic blocks. Currently, loop info is used in HSSA
//! and CodeGen.
template <typename CONTAINER, typename BB_ID, typename BB_PTR>
class LOOP_INFO_MGR {
  using LOOP          = LOOP_INFO<LOOP_INFO_MGR, BB_ID, BB_PTR>;
  using LOOP_DATA     = typename LOOP::DATA;
  using LOOP_DATA_PTR = typename LOOP::DATA_PTR;
  using MEM_POOL      = air::util::MEM_POOL<4096>;
  static constexpr uint32_t LOOP_TAB_KIND = 0x50001;

public:
  using LOOP_INFO_ID  = typename LOOP::ID;
  using LOOP_INFO_PTR = typename LOOP::PTR;
  using BB2LOOP_INFO  = std::unordered_map<uint32_t, LOOP_INFO_ID>;
  using LOOP_INFO_TAB =
      air::base::ARENA<sizeof(LOOP_DATA), sizeof(LOOP_INFO_ID), false>;

  LOOP_INFO_MGR(CONTAINER* cntr) : _cont(cntr) {
    air::util::CXX_MEM_ALLOCATOR<LOOP_INFO_TAB, MEM_POOL> loop_a(&_mpool);
    _loop_tab = loop_a.Allocate(&_mpool, LOOP_TAB_KIND, "loop_tab", true);
  }
  ~LOOP_INFO_MGR() {}

  //! @brief Create and return a new LOOP_INFO.
  LOOP_INFO_PTR New_loop(uint32_t back_num = 1, uint32_t tail_num = 1) {
    uint32_t data_size =
        sizeof(LOOP_DATA) + (back_num + tail_num) * sizeof(BB_ID);
    LOOP_DATA_PTR data = air::base::Static_cast<LOOP_DATA_PTR>(
        _loop_tab->Malloc(data_size >> 2U));
    new (data) LOOP_DATA(back_num, tail_num);
    return LOOP_INFO_PTR(LOOP(this, data));
  }
  //! @brief Return the LOOP_INFO with ID= id.
  LOOP_INFO_PTR Loop(LOOP_INFO_ID id) const {
    AIR_ASSERT(id != base::Null_id);
    return LOOP_INFO_PTR(LOOP(this, _loop_tab->Find(id)));
  }
  //! @brief Return the inner most loop containing basic block bb.
  LOOP_INFO_PTR Parent_loop(BB_ID bb) const {
    typename BB2LOOP_INFO::const_iterator iter = _parent_loop.find(bb.Value());
    AIR_ASSERT(iter != _parent_loop.end());
    return Loop(iter->second);
  }
  //! @brief Set the parent loop for bb. If bb is within nested loops, always
  //! set its parent loop as the innermost loop containing bb. Return true if
  //! the parent loop of bb is updated; otherwise, return false.
  bool Set_parent_loop(BB_PTR bb, LOOP_INFO_PTR loop) {
    std::pair<typename BB2LOOP_INFO::iterator, bool> res =
        _parent_loop.insert({bb->Id().Value(), loop->Id()});
    if (res.second) return true;
    if (res.first->second == base::Null_id) {
      res.first->second = loop->Id();
      return true;
    }
    if (loop->Is_inner_loop(res.first->second)) {
      res.first->second = loop->Id();
      return true;
    }
    return false;
  }
  bool Set_parent_loop(BB_ID bb_id, LOOP_INFO_ID loop_id) {
    return Set_parent_loop(_cont->Bb(bb_id), Loop(loop_id));
  }
  //! @brief Updated preheader of loop. Always set bb as the preheader for the
  //! innermost loop that takes it as a preheader. Return true if the loop
  //! information is updated; otherwise, return false.
  bool Set_preheader(LOOP_INFO_PTR loop, BB_PTR bb) {
    if (!Set_parent_loop(bb, loop)) {
      if (bb->Has_attr(BB_LOOP_PREHEADER)) return false;
    }
    bb->Set_attr(BB_LOOP_PREHEADER);
    loop->Set_preheader(bb->Id());
    return true;
  }
  //! @brief Updated header of loop. Always set bb as the header for the
  //! innermost loop that takes it as a header. Return true if the loop
  //! information is updated; otherwise, return false.
  bool Set_header(LOOP_INFO_PTR loop, BB_PTR bb) {
    if (!Set_parent_loop(bb, loop)) {
      if (bb->Has_attr(BB_LOOP_HEADER)) return false;
    }
    bb->Set_attr(BB_LOOP_HEADER);
    bb->Set_attr(BB_LOOP_BODY);
    loop->Set_header(bb->Id());
    return true;
  }
  //! @brief Updated back of loop. Always set bb as the back for the
  //! innermost loop that takes it as a back. Return true if the loop
  //! information is updated; otherwise, return false.
  bool Set_back(LOOP_INFO_PTR loop, BB_PTR bb) {
    if (!Set_parent_loop(bb, loop)) {
      if (bb->Has_attr(BB_LOOP_BACK)) return false;
    }
    bb->Set_attr(BB_LOOP_BACK);
    bb->Set_attr(BB_LOOP_BODY);
    loop->Set_back(bb->Id());
    return true;
  }
  //! @brief Updated tail of loop. Always set bb as the tail for the
  //! innermost loop that takes it as a tail. Return true if the loop
  //! information is updated; otherwise, return false.
  bool Set_tail(LOOP_INFO_PTR loop, BB_PTR bb) {
    if (!Set_parent_loop(bb, loop)) {
      if (bb->Has_attr(BB_LOOP_TAIL)) return false;
    }
    bb->Set_attr(BB_LOOP_TAIL);
    loop->Set_tail(bb->Id());
    return true;
  }
  //! @brief Return IR container.
  CONTAINER* Container(void) const { return _cont; }

private:
  MEM_POOL       _mpool;
  LOOP_INFO_TAB* _loop_tab;
  CONTAINER*     _cont;         //!< IR Container
  BB2LOOP_INFO   _parent_loop;  //!< inner-most loop containing the basic block
};
}  // namespace base
}  // namespace air
#endif