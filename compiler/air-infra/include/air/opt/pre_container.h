//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================
#ifndef AIR_OPT_PRE_CONTAINER_H
#define AIR_OPT_PRE_CONTAINER_H

#include <unordered_map>
#include <vector>

#include "air/opt/cfg.h"
#include "air/opt/hssa_decl.h"
#include "air/opt/occ.h"
#include "air/util/mem_allocator.h"
namespace air {

namespace opt {

enum {
  PRE_HTBLE_SIZE = 227,
};

typedef air::util::MEM_POOL<4096>                           MEM_POOL;
typedef air::util::CXX_MEM_ALLOCATOR<PRE_CAND_ID, MEM_POOL> WORKLIST_ALLOC;
typedef std::vector<PRE_CAND_ID, WORKLIST_ALLOC>            WORKLIST;

class PRE_CONTAINER {
  static constexpr uint32_t PRE_CAND_TAB_KIND = 0x30001;
  static constexpr uint32_t OCC_TAB_KIND      = 0x30002;
  typedef air::base::ARENA<sizeof(PRE_CAND_DATA), 4, false> PRE_CAND_TAB;
  typedef air::base::ARENA<sizeof(OCC_DATA), 4, false>      EXP_OCC_TAB;

  typedef air::util::CXX_MEM_ALLOCATOR<PRE_CAND_TAB, MEM_POOL>
      PRE_CAND_TAB_ALLOC;
  typedef air::util::CXX_MEM_ALLOCATOR<EXP_OCC_TAB, MEM_POOL> EXP_OCC_TAB_ALLOC;

public:
  PRE_CONTAINER(CFG& cfg, uint32_t htable_size = PRE_HTBLE_SIZE)
      : _cfg(cfg),
        _cand_hsize(htable_size),
        _worklist(WORKLIST_ALLOC(&_mpool)) {
    PRE_CAND_ID* htable_ptr =
        (PRE_CAND_ID*)_mpool.Allocate(sizeof(PRE_CAND_ID) * htable_size);
    _cand_htable = new (htable_ptr) PRE_CAND_ID[htable_size]();

    PRE_CAND_TAB_ALLOC cand_alloc(&_mpool);
    EXP_OCC_TAB_ALLOC  occ_alloc(&_mpool);
    _cand_tab =
        cand_alloc.Allocate(&_mpool, PRE_CAND_TAB_KIND, "cand_tab", true);
    _occ_tab = occ_alloc.Allocate(&_mpool, OCC_TAB_KIND, "occ_tab", true);
  }

  CFG&         Cfg() const { return _cfg; }
  HCONTAINER&  Hssa_cont() const { return *_cfg.Hssa_cont(); }
  const WORKLIST* Worklist() const { return &_worklist; }

  PRE_CAND_PTR Node(PRE_CAND_ID id) const { return Pre_cand_ptr(id); }
  OCC_PTR      Node(OCC_ID id) const { return Occ_ptr(id); }

  PRE_CAND_PTR Pre_cand_ptr(PRE_CAND_ID id) const {
    return PRE_CAND_PTR(PRE_CAND(this, _cand_tab->Find(id)));
  }

  OCC_PTR Occ_ptr(OCC_ID id) const {
    return OCC_PTR(OCC(this, _occ_tab->Find(id)));
  }

  OCC_PTR Append_real_occ(HEXPR_PTR expr, HSTMT_PTR stmt);
  void    Append_phi_occ(HEXPR_PTR expr, BB_PTR bb, uint32_t num_opnd);
  void    Append_phi_opnd_occ(OCC_PTR phi_occ, uint32_t phi_idx);
  void    Insert_new_operand(OCC_PTR phi_occ, OCC_PTR phi_opnd_occ);
  OCC_PTR New_real_occ(HEXPR_PTR expr, HSTMT_PTR stmt);

  void Print_worklist(std::ostream& os, uint32_t indent = 0) const;
  void Print_worklist() const;

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

private:
  uint32_t Htable_size(void) const { return _cand_hsize; }
  uint32_t Hash_exp(HEXPR_PTR expr);

  PRE_CAND_PTR New_pre_cand(HEXPR_PTR expr);

  PRE_CAND_PTR Find_or_new_pre_cand(HEXPR_PTR expr);
  void         Add_to_htable(uint32_t hash_idx, PRE_CAND_PTR cand);
  void         Add_to_worklist(PRE_CAND_PTR cand);

  CFG&     _cfg;
  MEM_POOL _mpool;

  EXP_OCC_TAB*  _occ_tab;      // expression occ data table
  PRE_CAND_TAB* _cand_tab;     // PRE candidate data table
  WORKLIST      _worklist;     // PRE candidate worklist
  uint32_t      _cand_hsize;   // PRE candidates hash table size
  PRE_CAND_ID*  _cand_htable;  // PRE candidates in hash table organize by ID
};

}  // namespace opt
}  // namespace air

#endif
