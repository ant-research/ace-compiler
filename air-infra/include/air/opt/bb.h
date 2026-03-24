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

//! @brief BB kind
enum BB_KIND {
  BB_UNKNOWN,     //!< Unknown block kind
  BB_ENTRY,       //!< Entry block
  BB_PHI,         //!< Phi block
  BB_LOOP_START,  //!< Loop start block
  BB_LOOP_END,    //!< Loop exit block
  BB_COND,        //!< Condition for loop and if
  BB_GOTO,        //!< Single target basic block
  BB_EXIT,        //!< Exit block
};

//! @brief BB_DATA holds the contents of a basic block
class BB_DATA {
public:
  BB_DATA(CFG_MPOOL& pool, BB_KIND kind, const air::base::SPOS& spos)
      : _kind(kind),
        _pred(BBID_ALLOC(&pool)),
        _succ(BBID_ALLOC(&pool)),
        _begin_stmt(HSTMT_ID()),
        _begin_phi(HPHI_ID()),
        _loop_info(LOOP_INFO_ID()),
        _spos(spos) {}

  BB_KIND         Kind(void) const { return _kind; }
  air::base::SPOS Spos(void) const { return _spos; }
  BB_ID           Next_id(void) const { return _next; }
  HSTMT_ID        Begin_stmt_id() const { return _begin_stmt; }
  HPHI_ID         Begin_phi_id() const { return _begin_phi; }
  LOOP_INFO_ID    Loop_info_id() const { return _loop_info; }
  BBID_VEC&       Succ() const { return const_cast<BBID_VEC&>(_succ); }
  BBID_VEC&       Pred() const { return const_cast<BBID_VEC&>(_pred); }
  BBID_VEC&       Succ() { return _succ; }
  BBID_VEC&       Pred() { return _pred; }
  BB_ID           Succ(uint32_t idx) const {
    AIR_ASSERT(idx < Succ().size());
    return _succ[idx];
  }
  BB_ID Pred(uint32_t idx) const {
    AIR_ASSERT(idx < Pred().size());
    return _pred[idx];
  }

  void Set_next_id(BB_ID next) { _next = next; }
  void Set_begin_stmt_id(HSTMT_ID id) { _begin_stmt = id; }
  void Set_begin_phi_id(HPHI_ID id) { _begin_phi = id; }
  void Set_loop_info_id(LOOP_INFO_ID id) { _loop_info = id; }

private:
  BB_KIND         _kind;        //!< Basic block kind
  BBID_VEC        _pred;        //!< Predecessor list
  BBID_VEC        _succ;        //!< Successor list
  HSTMT_ID        _begin_stmt;  //!< Begin stmt ID
  HPHI_ID         _begin_phi;   //!< Begin phi ID
  LOOP_INFO_ID    _loop_info;   //!< Loop info ID
  BB_ID           _next;        //!< Next basic block ID
  air::base::SPOS _spos;        //!< Source position
};

//! @brief BB provides accessing methods for BB_DATA
class BB {
public:
  BB() : _cfg(nullptr), _data(BB_DATA_PTR()) {}
  BB(CFG* cfg, BB_DATA_PTR data);

  CFG*            Cfg() const { return _cfg; }
  BB_DATA_PTR     Data() const { return _data; }
  BB_KIND         Kind(void) const { return _data->Kind(); }
  air::base::SPOS Spos(void) const { return _data->Spos(); }
  BB_ID           Id(void) const { return _data.Id(); }
  BB_PTR          Next(void) const;
  BB_ID           Next_id(void) const { return _data->Next_id(); }
  BBID_VEC&       Succ(void) const { return _data->Succ(); }
  BB_ID           Succ_id(uint32_t idx) const { return _data->Succ(idx); }
  BB_PTR          Succ(uint32_t idx) const;
  BBID_VEC&       Pred(void) const { return _data->Pred(); }
  BB_ID           Pred_id(uint32_t idx) const { return _data->Pred(idx); }
  BB_PTR          Pred(uint32_t idx) const;
  HSTMT_ID        Begin_stmt_id() const { return _data->Begin_stmt_id(); }
  HPHI_ID         Begin_phi_id() const { return _data->Begin_phi_id(); }
  HSTMT_PTR       Begin_stmt() const;
  HPHI_PTR        Begin_phi() const;
  LOOP_INFO_ID    Loop_info_id() const { return _data->Loop_info_id(); }
  LOOP_INFO_PTR   Loop_info() const;

  bool Is_null(void) const { return _data.Is_null(); }
  bool Is_phi(void) const { return Kind() == BB_PHI; }

  void Set_next(BB_ID next_id) { _data->Set_next_id(next_id); }
  void Set_next(BB_PTR next) { _data->Set_next_id(next->Id()); }
  void Set_begin_stmt_id(HSTMT_ID id) { _data->Set_begin_stmt_id(id); }
  void Set_begin_phi_id(HPHI_ID id) { _data->Set_begin_phi_id(id); }
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

  //! @brief Check if current bb dominates bb
  //! @return true if dominate, otherwise return false
  bool Dominates(BB_PTR bb);

  //! @brief Emit BB, returns an AIR NODE_PTR
  air::base::NODE_PTR Emit(air::base::NODE_PTR blk, BBID_SET& visited);

  //! @brief Emit loop body, returns an AIR NODE_PTR
  air::base::NODE_PTR Emit_loop_body(air::base::NODE_PTR& cur_blk,
                                     BBID_SET&            visited);

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

private:
  //! @brief Returns BB_KIND name string
  const char* Kind_name(void) const;

private:
  CFG*        _cfg;   //!< Owning control flow graph
  BB_DATA_PTR _data;  //!< BB data PTR
};

}  // namespace opt
}  // namespace air
#endif