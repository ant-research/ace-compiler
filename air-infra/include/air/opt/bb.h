//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_BB_H
#define AIR_OPT_BB_H

#include <vector>

#include "air/base/node.h"
#include "air/opt/cfg_decl.h"
#include "air/opt/hssa_decl.h"

namespace air {

namespace opt {

class CFG;
class HSSA_FUNC;
enum BB_KIND {
  BB_DEF,
  BB_ENTRY,
  BB_LOOP_PHI,
  BB_LOOP_INIT,
  BB_COND,
  BB_LOOP_BODY,
  BB_LOOP_EXIT,
  BB_TRUE,
  BB_FALSE,
  BB_IF_PHI,
  BB_EXIT,
};

class BB_DATA {
public:
  BB_DATA(CFG_MEM_POOL& pool, BB_KIND kind, const air::base::SPOS& spos)
      : _kind(kind),
        _pred(BBID_ALLOC(&pool)),
        _succ(BBID_ALLOC(&pool)),
        _begin_stmt(HSTMT_ID()),
        _begin_phi(HPHI_ID()),
        _loop_info(LOOP_INFO_ID()),
        _spos(spos) {}

  BB_KIND         Kind(void) const { return _kind; }
  air::base::SPOS Spos(void) const { return _spos; }

  BBID_VEC& Succ() const { return const_cast<BBID_VEC&>(_succ); }
  BBID_VEC& Pred() const { return const_cast<BBID_VEC&>(_pred); }
  BBID_VEC& Succ() { return _succ; }
  BBID_VEC& Pred() { return _pred; }
  BB_ID     Succ(uint32_t idx) const {
    AIR_ASSERT(idx < Succ().size());
    return _succ[idx];
  }
  BB_ID Pred(uint32_t idx) const {
    AIR_ASSERT(idx < Pred().size());
    return _pred[idx];
  }

  BB_ID Next_id(void) const { return _next; }
  void  Set_next_id(BB_ID next) { _next = next; }

  HSTMT_ID Begin_stmt_id() { return _begin_stmt; }
  HPHI_ID  Begin_phi_id() { return _begin_phi; }
  void     Set_begin_stmt_id(HSTMT_ID id) { _begin_stmt = id; }
  void     Set_begin_phi_id(HPHI_ID id) { _begin_phi = id; }

  LOOP_INFO_ID Loop_info_id() const { return _loop_info; }
  void         Set_loop_info_id(LOOP_INFO_ID id) { _loop_info = id; }

private:
  BB_KIND         _kind;
  BBID_VEC        _pred;
  BBID_VEC        _succ;
  HSTMT_ID        _begin_stmt;
  HPHI_ID         _begin_phi;
  LOOP_INFO_ID    _loop_info;
  BB_ID           _next;
  air::base::SPOS _spos;
};

class BB {
public:
  BB() : _cfg(nullptr), _data(BB_DATA_PTR()) {}
  BB(CFG* cfg, BB_DATA_PTR data);

  CFG*            Cfg() const { return _cfg; }
  BB_DATA_PTR     Data() const { return _data; }
  BB_KIND         Kind(void) const { return _data->Kind(); }
  air::base::SPOS Spos(void) const { return _data->Spos(); }

  bool Is_null(void) const { return _data.Is_null(); }
  bool Is_phi(void) { return (Kind() == BB_LOOP_PHI || Kind() == BB_IF_PHI); }

  BB_ID  Id(void) const { return _data.Id(); }
  BB_ID  Next_id(void) const { return _data->Next_id(); }
  void   Set_next(BB_ID next_id) const { _data->Set_next_id(next_id); }
  void   Set_next(BB_PTR next) const { _data->Set_next_id(next->Id()); }
  BB_PTR Next(void) const;

  BBID_VEC& Succ(void) const { return _data->Succ(); }
  BB_ID     Succ_id(uint32_t idx) const { return _data->Succ(idx); }
  BB_PTR    Succ(uint32_t idx) const;

  BBID_VEC& Pred(void) const { return _data->Pred(); }
  BB_ID     Pred_id(uint32_t idx) const { return _data->Pred(idx); }
  BB_PTR    Pred(uint32_t idx) const;

  HSTMT_ID  Begin_stmt_id() const { return _data->Begin_stmt_id(); }
  HPHI_ID   Begin_phi_id() const { return _data->Begin_phi_id(); }
  HSTMT_PTR Begin_stmt();
  HPHI_PTR  Begin_phi();
  void      Set_begin_stmt_id(HSTMT_ID id) { _data->Set_begin_stmt_id(id); }
  void      Set_begin_phi_id(HPHI_ID id) { _data->Set_begin_phi_id(id); }

  LOOP_INFO_ID  Loop_info_id() const { return _data->Loop_info_id(); }
  LOOP_INFO_PTR Loop_info() const;
  void Set_loop_info_id(LOOP_INFO_ID id) { _data->Set_loop_info_id(id); }
  void Set_loop_info(LOOP_INFO_PTR info);

  void Prepend_stmt(HSTMT_PTR stmt);
  void Append_stmt(HSTMT_PTR stmt);
  void Insert_stmt_before(HSTMT_PTR stmt, HSTMT_PTR pos);
  void Insert_stmt_after(HSTMT_PTR stmt, HSTMT_PTR pos);
  void Remove_stmt(HSTMT_PTR stmt);
  void Add_succ(BB_PTR succ_bb);
  void Add_pred(BB_PTR pred_bb);
  void Append_phi(HPHI_PTR phi);

  bool                Dominates(BB_PTR bb);
  const char*         Kind_name(void) const;
  air::base::NODE_PTR Emit(HSSA_FUNC* hssa_func, air::base::NODE_PTR blk,
                           BBID_SET& visited);
  air::base::NODE_PTR Emit_loop_body(HSSA_FUNC*           hssa_func,
                                     air::base::NODE_PTR& cur_blk,
                                     BBID_SET&            visited);
  void                Print(std::ostream& os, uint32_t indent = 0) const;
  void                Print() const;
  std::string         To_str() const;

private:
  CFG*        _cfg;
  BB_DATA_PTR _data;
};

}  // namespace opt
}  // namespace air
#endif