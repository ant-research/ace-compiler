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

class LOOP_INFO_DATA {
public:
  LOOP_INFO_DATA()
      : _init_bb(BB_ID()),
        _phi_bb(BB_ID()),
        _body_bb(BB_ID()),
        _cond_bb(BB_ID()),
        _exit_bb(BB_ID()),
        _incr_stmt(HSTMT_ID()),
        _init_stmt(HSTMT_ID()),
        _cond_expr(HCR_ID()),
        _iv(HCR_ID()),
        _parent(LOOP_INFO_ID()) {}

  void Init(BB_ID init, BB_ID phi_bb, BB_ID body, BB_ID cond, BB_ID exit,
            HSTMT_ID init_stmt, HSTMT_ID incr_stmt, HCR_ID cond_expr, HCR_ID iv,
            LOOP_INFO_ID parent) {
    _init_bb   = init;
    _phi_bb    = phi_bb;
    _body_bb   = body;
    _cond_bb   = cond;
    _exit_bb   = exit;
    _incr_stmt = incr_stmt;
    _init_stmt = init_stmt;
    _cond_expr = cond_expr;
    _iv        = iv;
    _parent    = parent;
  }

  BB_ID        Init_bb(void) const { return _init_bb; }
  BB_ID        Phi_bb(void) const { return _phi_bb; }
  BB_ID        Body_bb(void) const { return _body_bb; }
  BB_ID        Cond_bb(void) const { return _cond_bb; }
  HSTMT_ID     Incr_stmt(void) const { return _incr_stmt; }
  BB_ID        Exit_bb(void) const { return _exit_bb; }
  HSTMT_ID     Init_stmt(void) const { return _init_stmt; }
  HCR_ID       Cond_expr(void) const { return _cond_expr; }
  HCR_ID       Ind_expr(void) const { return _iv; }
  LOOP_INFO_ID Parent(void) const { return _parent; }

private:
  BB_ID        _init_bb;
  BB_ID        _phi_bb;
  BB_ID        _body_bb;
  BB_ID        _cond_bb;
  BB_ID        _exit_bb;
  HSTMT_ID     _incr_stmt;
  HSTMT_ID     _init_stmt;
  HCR_ID       _iv;
  HCR_ID       _cond_expr;
  LOOP_INFO_ID _parent;
};

class LOOP_INFO {
public:
  LOOP_INFO() : _cfg(nullptr), _data(LOOP_INFO_DATA_PTR()) {}
  LOOP_INFO(CFG* cfg, LOOP_INFO_DATA_PTR data) : _cfg(cfg), _data(data) {}
  void Init(BB_PTR init, BB_PTR phi_bb, BB_PTR body, BB_PTR cond, BB_PTR exit,
            HSTMT_PTR incr, HSTMT_PTR init_stmt, HCR_PTR cond_expr, HCR_PTR iv,
            LOOP_INFO_PTR parent);

  CFG*               Cfg() const { return _cfg; }
  HSSA_CONTAINER&    Hssa_cont() const;
  LOOP_INFO_DATA_PTR Data() const { return _data; }
  LOOP_INFO_ID       Id(void) const { return _data.Id(); }
  bool               Is_null(void) const { return _data.Is_null(); }
  BB_PTR             Init_bb() const;
  HSTMT_PTR          Init_stmt() const;
  HCR_PTR            Cond_expr() const;
  HSTMT_PTR          Incr_stmt() const;
  BB_PTR             Loop_body() const;
  BB_PTR             Exit_bb() const;
  BB_PTR             Cond_bb() const;
  HCR_PTR            Ind_expr() const;

  LOOP_INFO_ID  Parent_id(void) const { return _data->Parent(); }
  LOOP_INFO_PTR Parent(void) const;

  void Emit(HSSA_FUNC* hssa_func, air::base::NODE_PTR blk, BBID_SET& visited);

private:
  CFG*               _cfg;
  LOOP_INFO_DATA_PTR _data;
};

}  // namespace opt
}  // namespace air
#endif