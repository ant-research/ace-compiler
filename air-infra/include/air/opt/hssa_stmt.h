//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_HSSA_STMT_H
#define AIR_OPT_HSSA_STMT_H

#include "air/base/node.h"
#include "air/base/opcode.h"
#include "air/base/st.h"
#include "air/base/st_decl.h"
#include "air/opt/hssa_decl.h"
#include "air/opt/ssa_decl.h"
#include "hssa_expr.h"

namespace air {

namespace opt {

class HSSA_CONTAINER;
class CFG;

enum HSTMT_KIND {
  SK_INVALID,
  SK_BLK_ENTRY,
  SK_ASSIGN,
  SK_OP,
  SK_IF,
  SK_DO_LOOP,
  SK_CALL,
};

class HSTMT_ATTR {
public:
  HSTMT_ATTR() : _dead(0) {}
  uint32_t _dead : 1;  //!< statement is dead
};

class HSTMT_DATA {
  friend class HSTMT;

public:
  HSTMT_DATA(air::base::NODE_PTR node, HSTMT_KIND k = SK_INVALID)
      : _kind(k),
        _opcode(node->Opcode()),
        _spos(node->Spos()),
        _next(air::base::Null_id),
        _attr() {}

  HSTMT_DATA(air::base::OPCODE opcode, HSTMT_KIND k = SK_INVALID)
      : _kind(k), _opcode(opcode), _spos(), _next(air::base::Null_id) {}

  HSTMT_KIND        Kind() const { return _kind; }
  air::base::OPCODE Opcode() const { return _opcode; }
  HSTMT_ID          Next() { return _next; }
  void              Set_next(HSTMT_ID next) { _next = next; }
  void              Set_bb_id(BB_ID id) { _bb = id; }
  BB_ID             Bb_id(void) const { return _bb; }
  air::base::SPOS   Spos(void) const { return _spos; }
  HSTMT_ATTR        Attr(void) const { return _attr; }
  void              Set_dead(void) { _attr._dead = 1; }
  bool              Is_dead() const { return _attr._dead == 1; }

  void Print(HSSA_CONTAINER* cont, std::ostream& os, uint32_t indent) const;

private:
  HSTMT_KIND        _kind;
  air::base::OPCODE _opcode;
  air::base::SPOS   _spos;  // source position information
  BB_ID             _bb;    // the block id it belongs to
  HSTMT_ID          _next;
  HSTMT_ATTR        _attr;
};

class ASSIGN_HSTMT_DATA : public HSTMT_DATA {
public:
  ASSIGN_HSTMT_DATA(air::base::NODE_PTR node)
      : HSTMT_DATA(node, SK_ASSIGN),
        _mu(air::base::Null_id),
        _chi(air::base::Null_id),
        _lhs(air::base::Null_id),
        _rhs(air::base::Null_id) {}

  ASSIGN_HSTMT_DATA(air::base::OPCODE opcode, HCR_ID lhs, HCR_ID rhs)
      : HSTMT_DATA(opcode, SK_ASSIGN),
        _mu(air::base::Null_id),
        _chi(air::base::Null_id),
        _lhs(lhs),
        _rhs(rhs) {}

  HCR_ID  Lhs(void) const { return _lhs; }
  HCR_ID  Rhs(void) const { return _rhs; }
  HMU_ID  Mu(void) const { return _mu; }
  HCHI_ID Chi(void) const { return _chi; }
  void    Set_lhs(HCR_ID lhs) { _lhs = lhs; }
  void    Set_rhs(HCR_ID rhs) { _rhs = rhs; }
  void    Set_chi(HCHI_ID chi) { _chi = chi; }

  void Print(HSSA_CONTAINER* cont, std::ostream& os, uint32_t indent) const;

private:
  HMU_ID  _mu;
  HCHI_ID _chi;
  HCR_ID  _lhs;
  HCR_ID  _rhs;
};

class OP_HSTMT_DATA : public HSTMT_DATA {
  friend class HSTMT;

public:
  OP_HSTMT_DATA(air::base::NODE_PTR node)
      : HSTMT_DATA(node, SK_OP),
        _mu(air::base::Null_id),
        _chi(air::base::Null_id),
        _kid_cnt(node->Num_child()) {}

  static size_t Size(uint32_t kid_cnt) {
    return sizeof(OP_HSTMT_DATA) + kid_cnt * sizeof(HCR_ID);
  }

  uint32_t Kid_cnt() const { return _kid_cnt; }
  void     Set_kid(uint32_t idx, HCR_ID kid) {
    AIR_ASSERT(idx < _kid_cnt);
    _kids[idx] = kid;
  }
  HCR_ID Kid(uint32_t idx) const {
    AIR_ASSERT(idx < Kid_cnt());
    return _kids[idx];
  }

  HCHI_ID Chi(void) const { return _chi; }
  void    Set_chi(HCHI_ID chi) { _chi = chi; }

  void Print(HSSA_CONTAINER* cont, std::ostream& os, uint32_t indent) const;

private:
  HMU_ID   _mu;
  HCHI_ID  _chi;
  uint32_t _kid_cnt;
  HCR_ID   _kids[0];
};

class DO_LOOP_HSTMT_DATA : public HSTMT_DATA {
public:
  DO_LOOP_HSTMT_DATA(air::base::NODE_PTR node)
      : HSTMT_DATA(node, SK_DO_LOOP),
        _entry(air::base::Null_id),
        _exit(air::base::Null_id),
        _cond(air::base::Null_id),
        _body(air::base::Null_id),
        _incr(air::base::Null_id) {}

  DO_LOOP_HSTMT_DATA(air::base::NODE_PTR node, HSTMT_ID init, HCR_ID cond,
                     HSTMT_ID body, HCR_ID incr)
      : HSTMT_DATA(node, SK_DO_LOOP) {
    _entry = init;
    _exit  = air::base::Null_id;
    _cond  = cond;
    _body  = body;
    _incr  = incr;
  }

  DO_LOOP_HSTMT_DATA(air::base::NODE_PTR node, HSTMT_PTR init, HCR_PTR cond,
                     HSTMT_PTR body, HCR_PTR incr, HPHI_PTR hphi);

  HSTMT_ID Entry() const { return _entry; }
  HSTMT_ID Exit() const { return _exit; }
  HSTMT_ID Body() const { return _body; }
  HCR_ID   Cond() const { return _cond; }
  HCR_ID   Incr() const { return _incr; }

  void Print(HSSA_CONTAINER* cont, std::ostream& os, uint32_t indent) const;

private:
  HSTMT_ID _entry;  // entry statments
  HSTMT_ID _exit;   // exit statments
  HCR_ID   _cond;
  HSTMT_ID _body;  // body statments
  HCR_ID   _incr;
};

class IF_HSTMT_DATA : public HSTMT_DATA {
public:
  IF_HSTMT_DATA(air::base::NODE_PTR node)
      : HSTMT_DATA(node, SK_IF), _cond(air::base::Null_id) {}

  IF_HSTMT_DATA(air::base::NODE_PTR node, HCR_PTR cond)
      : HSTMT_DATA(node, SK_IF), _cond(cond->Id()) {}

  IF_HSTMT_DATA(air::base::OPCODE opcode, HCR_PTR cond)
      : HSTMT_DATA(opcode, SK_IF), _cond(cond->Id()) {}

  HCR_ID Cond(void) const { return _cond; }

  void Print(HSSA_CONTAINER* cont, std::ostream& os, uint32_t indent) const;

private:
  HCR_ID _cond;
};

class CALL_HSTMT_DATA : public HSTMT_DATA {
public:
  CALL_HSTMT_DATA(air::base::NODE_PTR node) : HSTMT_DATA(node, SK_CALL) {
    _entry   = node->Entry_id();
    _retv    = HCR_ID();
    _kid_cnt = node->Num_arg();
    _chi     = HCHI_ID();
  }

  static size_t Size(uint32_t kid_cnt) {
    return sizeof(OP_HSTMT_DATA) + kid_cnt * sizeof(HCR_ID);
  }

  air::base::ENTRY_ID Entry(void) const { return _entry; }

  HCR_ID Retv(void) const { return _retv; }

  void Set_retv(HCR_ID retv) { _retv = retv; }

  uint32_t Kid_cnt() const { return _kid_cnt; }

  void Set_kid(uint32_t idx, HCR_ID kid) {
    AIR_ASSERT(idx < _kid_cnt);
    _kids[idx] = kid;
  }

  HCR_ID Kid(uint32_t idx) const {
    AIR_ASSERT(idx < Kid_cnt());
    return _kids[idx];
  }

  HCHI_ID Chi(void) const { return _chi; }

  void Set_chi(HCHI_ID chi) { _chi = chi; }

  void Print(HSSA_CONTAINER* cont, std::ostream& os, uint32_t indent) const;

private:
  air::base::ENTRY_ID _entry;
  HCR_ID              _retv;
  HCHI_ID             _chi;
  uint32_t            _kid_cnt;
  HCR_ID              _kids[0];
};

class HSTMT {
public:
  HSTMT() : _cont(nullptr), _data() {}
  HSTMT(const HSSA_CONTAINER* cont, HSTMT_DATA_PTR data)
      : _cont(const_cast<HSSA_CONTAINER*>(cont)), _data(data) {}

  HSSA_CONTAINER* Hssa_cont(void) const { return _cont; }
  HSTMT_KIND      Kind() const { return _data->Kind(); }
  bool            Is_null() const { return _data.Is_null(); }
  HSTMT_ID        Id() const { return _data.Id(); }
  HSTMT_ID        Next_id() const { return _data->Next(); }

  void   Set_bb_id(BB_ID id) { _data->Set_bb_id(id); }
  BB_ID  Bb_id(void) const { return _data->Bb_id(); }
  BB_PTR Bb(CFG* cfg);

  void Set_next(HSTMT_ID next) { _data->Set_next(next); }

  uint32_t Domain() const { return Opcode().Domain(); }
  uint32_t Operator() const { return Opcode().Operator(); }

  air::base::OPCODE Set_opcode(air::base::OPCODE opcode) const {
    return _data->_opcode = opcode;
  }
  air::base::OPCODE Opcode() const { return _data->_opcode; }
  air::base::SPOS   Spos() const { return _data->Spos(); }
  HSTMT_DATA_PTR    Data() const { return _data; }
  HSTMT_ATTR        Attr() const { return _data->Attr(); }
  void              Set_dead() { _data->Set_dead(); }
  bool              Is_dead() const { return _data->Is_dead(); }

  ASSIGN_HSTMT_DATA_PTR Cast_to_assign_sr(void) const {
    AIR_ASSERT(Kind() == SK_ASSIGN);
    return air::base::Static_cast<ASSIGN_HSTMT_DATA_PTR>(_data);
  }

  OP_HSTMT_DATA_PTR Cast_to_op_sr(void) const {
    AIR_ASSERT(Kind() == SK_OP);
    return air::base::Static_cast<OP_HSTMT_DATA_PTR>(_data);
  }

  DO_LOOP_HSTMT_DATA_PTR Cast_to_do_loop_sr(void) const {
    AIR_ASSERT(Kind() == SK_DO_LOOP);
    return air::base::Static_cast<DO_LOOP_HSTMT_DATA_PTR>(_data);
  }

  IF_HSTMT_DATA_PTR Cast_to_if_sr(void) const {
    AIR_ASSERT(Kind() == SK_IF);
    return air::base::Static_cast<IF_HSTMT_DATA_PTR>(_data);
  }

  CALL_HSTMT_DATA_PTR Cast_to_call_sr(void) const {
    AIR_ASSERT(Kind() == SK_CALL);
    return air::base::Static_cast<CALL_HSTMT_DATA_PTR>(_data);
  }

  HCR_ID  Lhs_id() const;
  HCR_ID  Rhs_id() const;
  HCR_PTR Lhs() const;
  HCR_PTR Rhs() const;

  void Set_lhs(HCR_ID lhs) { Cast_to_assign_sr()->Set_lhs(lhs); }

  void Set_rhs(HCR_ID rhs) { Cast_to_assign_sr()->Set_rhs(rhs); }

  HMU_ID Mu() const {
    CMPLR_ASSERT(false, "TO IMPL mu");
    return HMU_ID();
  }

  void    Set_chi(HCHI_ID chi);
  HCHI_ID Chi() const;

  bool Replace_cr(HCR_ID cr, HCR_ID new_cr);
  bool Is_scf(void) const { return (Kind() == SK_DO_LOOP || Kind() == SK_IF); }
  bool Is_dominate(HSTMT_PTR stmt);

  air::base::STMT_PTR Emit(air::base::CONTAINER* cont);
  void                Print(std::ostream& os, uint32_t indent = 0) const;
  void                Print() const;
  std::string         To_str() const;

private:
  HSSA_CONTAINER* _cont;
  HSTMT_DATA_PTR  _data;
};

}  // namespace opt
}  // namespace air

#endif