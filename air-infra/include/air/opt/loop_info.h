//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_LOOP_INFO_H
#define AIR_OPT_LOOP_INFO_H

#include "air/opt/bb.h"

namespace air {
namespace opt {

//! @brief LOOP_INFO_DATA holds the data of a loop's information
class LOOP_INFO_DATA {
public:
  LOOP_INFO_DATA()
      : _init(BB_ID()),
        _phi(BB_ID()),
        _body(BB_ID()),
        _cond(BB_ID()),
        _exit(BB_ID()),
        _incr_stmt(HSTMT_ID()),
        _init_stmt(HSTMT_ID()),
        _cond_expr(HEXPR_ID()),
        _iv(HEXPR_ID()),
        _parent(LOOP_INFO_ID()) {}

  void Init(BB_ID init, BB_ID phi, BB_ID body, BB_ID cond, BB_ID exit,
            HSTMT_ID init_stmt, HSTMT_ID incr_stmt, HEXPR_ID cond_expr,
            HEXPR_ID iv, LOOP_INFO_ID parent) {
    _init      = init;
    _phi       = phi;
    _body      = body;
    _cond      = cond;
    _exit      = exit;
    _incr_stmt = incr_stmt;
    _init_stmt = init_stmt;
    _cond_expr = cond_expr;
    _iv        = iv;
    _parent    = parent;
  }

  BB_ID        Init(void) const { return _init; }
  BB_ID        Phi(void) const { return _phi; }
  BB_ID        Body(void) const { return _body; }
  BB_ID        Cond(void) const { return _cond; }
  HSTMT_ID     Incr_stmt(void) const { return _incr_stmt; }
  BB_ID        Exit(void) const { return _exit; }
  HSTMT_ID     Init_stmt(void) const { return _init_stmt; }
  HEXPR_ID     Cond_expr(void) const { return _cond_expr; }
  HEXPR_ID     Ind_expr(void) const { return _iv; }
  LOOP_INFO_ID Parent(void) const { return _parent; }

private:
  BB_ID        _init;       //!< Basic block that initializes the loop
  BB_ID        _phi;        //!< Merge basic block associated with the loop
  BB_ID        _body;       //!< The first basic block containing the loop body
  BB_ID        _cond;       //!< Basic block for the loop condition
  BB_ID        _exit;       //!< Basic block for exiting the loop
  HSTMT_ID     _incr_stmt;  //!< Statement that increments the loop variable
  HSTMT_ID     _init_stmt;  //!< Statement for initializing loop variables
  HEXPR_ID     _iv;         //!< Induction variable expression for the loop
  HEXPR_ID     _cond_expr;  //!< Expression for the loop condition
  LOOP_INFO_ID _parent;     //!< Information about the parent loop
};

//! @brief LOOP_INFO the wrapper for LOOP_INFO_DATA
class LOOP_INFO {
public:
  LOOP_INFO() : _cfg(nullptr), _data(LOOP_INFO_DATA_PTR()) {}
  LOOP_INFO(CFG* cfg, LOOP_INFO_DATA_PTR data) : _cfg(cfg), _data(data) {}
  void Init(BB_PTR init, BB_PTR phi, BB_PTR body, BB_PTR cond, BB_PTR exit,
            HSTMT_PTR incr, HSTMT_PTR init_stmt, HEXPR_PTR cond_expr,
            HEXPR_PTR iv, LOOP_INFO_PTR parent);

  CFG*               Cfg() const { return _cfg; }
  const HCONTAINER*  Hssa_cont() const;
  LOOP_INFO_DATA_PTR Data() const { return _data; }
  LOOP_INFO_ID       Id(void) const { return _data.Id(); }
  bool               Is_null(void) const { return _data.Is_null(); }
  BB_PTR             Init() const;
  HSTMT_PTR          Init_stmt() const;
  HEXPR_PTR          Cond_expr() const;
  HSTMT_PTR          Incr_stmt() const;
  BB_PTR             Loop_body() const;
  BB_PTR             Exit() const;
  BB_PTR             Cond() const;
  HEXPR_PTR          Ind_expr() const;
  LOOP_INFO_ID       Parent_id(void) const { return _data->Parent(); }
  LOOP_INFO_PTR      Parent(void) const;

  void Emit(air::base::NODE_PTR blk, BBID_SET& visited);

private:
  CFG*               _cfg;   //!< Owning control flow graph
  LOOP_INFO_DATA_PTR _data;  //!< Loop information data PTR
};

}  // namespace opt
}  // namespace air
#endif