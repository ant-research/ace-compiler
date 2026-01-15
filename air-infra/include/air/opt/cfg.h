//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_CFG_H
#define AIR_OPT_CFG_H

#include "air/opt/bb.h"
#include "air/opt/cfg_decl.h"
#include "air/opt/dom.h"
#include "air/opt/hssa_decl.h"
#include "air/opt/loop_info.h"

namespace air {
namespace opt {

//! @brief Control Flow Graph
class CFG {
public:
  CFG(HCONTAINER* cont = nullptr) : _cont(cont), _dom_info(Mem_pool()) {
    BB_TAB_ALLOC bb_alloc(&_mpool);
    _bb_tab = bb_alloc.Allocate(&_mpool, BB_TAB_KIND, "bb_tab", true);
    LOOP_INFO_TAB_ALLOC loop_alloc(&_mpool);
    _loop_info_tab =
        loop_alloc.Allocate(&_mpool, LOOP_INFO_TAB_KIND, "loop_info_tab", true);
    _entry_bb = BB_ID();
    _exit_bb  = BB_ID();
  }

  CFG_MPOOL&    Mem_pool() { return _mpool; }
  HCONTAINER*   Hssa_cont() const { return _cont; }
  BB_PTR        Entry_bb() { return Bb_ptr(_entry_bb); }
  BB_PTR        Exit_bb() { return Bb_ptr(_exit_bb); }
  BB_PTR        Node(BB_ID id) { return Bb_ptr(id); }
  BB_PTR        Bb_ptr(BB_ID id) { return BB_PTR(BB(this, _bb_tab->Find(id))); }
  uint32_t      Total_bb_cnt(void) const { return _bb_tab->Size(); }
  DOM_INFO&     Dom_info(void) { return _dom_info; }
  LOOP_INFO_PTR Loop_info_ptr(LOOP_INFO_ID id) {
    return LOOP_INFO_PTR(LOOP_INFO(this, _loop_info_tab->Find(id)));
  }

  // CFG build methods

  //! @brief create a new basic block with BB_KIND k
  BB_PTR New_bb(BB_KIND k, const air::base::SPOS& spos);

  //! @brief append bb to the tail of entry bb_list
  void Append_bb(BB_PTR bb);

  //! @brief insert bb before pos
  void Insert_bb_before(BB_PTR bb, BB_PTR pos);

  //! @brief insert bb after pos
  void Insert_bb_after(BB_PTR bb, BB_PTR pos);

  //! @brief connect bb pred with successor succ
  void Connect_with_succ(BB_PTR pred, BB_PTR succ);

  //! @brief connect bb succ with predecessor pred
  void Connect_with_pred(BB_PTR succ, BB_PTR pred);

  //! @brief build dominance info
  void Build_dom_info();

  //! @brief create a new LOOP_INFO
  LOOP_INFO_PTR New_loop_info();

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

private:
  static constexpr uint32_t BB_TAB_KIND        = 0x40001;
  static constexpr uint32_t LOOP_INFO_TAB_KIND = 0x40002;

  typedef air::base::ARENA<sizeof(BB_DATA), 4, false>     BB_TAB;
  typedef air::util::CXX_MEM_ALLOCATOR<BB_TAB, CFG_MPOOL> BB_TAB_ALLOC;

  typedef air::base::ARENA<sizeof(LOOP_INFO_DATA), 4, false> LOOP_INFO_TAB;
  typedef air::util::CXX_MEM_ALLOCATOR<LOOP_INFO_TAB, CFG_MPOOL>
      LOOP_INFO_TAB_ALLOC;

  CFG_MPOOL      _mpool;          //!< CFG Memory pool
  HCONTAINER*    _cont;           //!< HSSA container
  BB_ID          _entry_bb;       //!< entry bb ID
  BB_ID          _exit_bb;        //!< exit bb ID
  BB_TAB*        _bb_tab;         //!< bb table
  LOOP_INFO_TAB* _loop_info_tab;  //!< loop info table
  DOM_INFO       _dom_info;       //!< dominance info
};

}  // namespace opt
}  // namespace air
#endif
