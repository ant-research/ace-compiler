//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================
#ifndef AIR_OPT_EXPR_OCC_H
#define AIR_OPT_EXPR_OCC_H

#include "air/opt/bb.h"
#include "air/opt/hssa_container.h"
#include "air/opt/hssa_decl.h"
#include "air/opt/pre_decl.h"

namespace air {

namespace opt {

enum OCC_KIND {
  OCC_UNKNOWN  = 0,
  OCC_PHI      = 1,  // DO NOT CHANGE THE ORDER of phi,
  OCC_REAL     = 2,  // real,
  OCC_COMP     = 3,  // comparison occurrence for LFTR
  OCC_PHI_OPND = 4,  // phi_pred
  OCC_EXIT     = 5,  // exit
};

enum PHI_OCC_FLAGS {
  POF_NONE          = 0x0,
  POF_IS_DOWN_SAFE  = 0x1,
  POF_IS_LIVE       = 0x2,   // expr is PANT at this phi
  POF_CANT_BE_AVAIL = 0x4,   // this phi result can't be made available
  POF_STOP          = 0x8,   // can't insert later than this phi
  POF_IDENTITY      = 0x10,  // this phi need not become a temp phi
  POF_IDENT_INJURED = 0x20,  // this phi's identity is injured
};

enum PHI_OPND_OCC_FLAGS {
  POOF_NONE         = 0x0,
  POOF_IS_SAVE      = 0x1,
  POOF_IS_INSERTED  = 0x2,
  POOF_HAS_REAL_USE = 0x4,
  POOF_IS_PROCESSED = 0x8,
};

enum REAL_OCC_FLAGS {
  ROF_NONE      = 0x0,
  ROF_IS_RELOAD = 0x1,  // the real occ is reload
  ROF_IS_SAVE   = 0x2,  // the real occ is saved
  ROF_IS_LHS    = 0x4,  // the real occ is in left hand side
  ROF_IS_FORMAL = 0x8   // the fake lhs occurrence at entry for formals
};

class OCC_DATA {
public:
  OCC_DATA(OCC_KIND k, HEXPR_PTR expr, HSTMT_PTR stmt)
      : _kind(k),
        _expr(expr->Id()),
        _e_ver(0),
        _bb(BB_ID()),
        _def(OCC_ID()),
        _next(OCC_ID()) {
    if (!stmt->Is_null()) _bb = stmt->Bb_id();
  }
  OCC_DATA(OCC_KIND k, HEXPR_PTR expr, BB_PTR bb)
      : _kind(k),
        _expr(expr->Id()),
        _e_ver(0),
        _bb(bb->Id()),
        _def(OCC_ID()),
        _next(OCC_ID()) {}

  OCC_ID   Next_id(void) const { return _next; }
  void     Set_next(OCC_ID next) { _next = next; }
  OCC_KIND Kind(void) const { return _kind; }
  HEXPR_ID Expr_id(void) const { return _expr; }
  void     Set_expr_id(HEXPR_ID id) { _expr = id; }
  uint32_t Ver(void) const { return _e_ver; }
  void     Set_ver(uint32_t ver) { _e_ver = ver; }
  BB_ID    Bb_id(void) const { return _bb; }
  void     Set_bb_id(BB_ID id) { _bb = id; }

  OCC_ID Def_id(void) const { return _def; }
  void   Set_def(OCC_PTR def);
  void   Set_def(OCC_ID def) { _def = def; }

  void Print(PRE_CONTAINER* pre_cont, std::ostream& os,
             uint32_t indent = 0) const;

  void Print(PRE_CONTAINER* pre_cont) const;

  std::string To_str(PRE_CONTAINER* pre_cont) const;

private:
  OCC_KIND _kind;   // occurrence kind
  HEXPR_ID _expr;   // expression
  uint32_t _e_ver;  // expression version
  BB_ID    _bb;     // enclosing bb
  OCC_ID   _def;    // define occur
  OCC_ID   _next;   // next occurrence
};

class PHI_OCC_DATA : public OCC_DATA {
public:
  PHI_OCC_DATA(HEXPR_PTR expr, BB_PTR bb, uint32_t num_opnds)
      : OCC_DATA(OCC_PHI, expr, bb),
        _flags(POF_IS_DOWN_SAFE),
        _num_opnds(num_opnds) {}

  static uint32_t Size(uint32_t num_opnds) {
    return sizeof(PHI_OCC_DATA) + num_opnds * sizeof(OCC_ID);
  }
  uint32_t Num_opnds(void) const { return _num_opnds; }

  void Set_opnd(uint32_t idx, OCC_ID opnd) {
    AIR_ASSERT(idx < _num_opnds);
    _opnds[idx] = opnd;
  }

  OCC_ID Opnd(uint32_t idx) const {
    AIR_ASSERT(idx < _num_opnds);
    return _opnds[idx];
  }

  void    Set_saved_phi(HPHI_ID id) { _saved_phi = id; }
  HPHI_ID Saved_phi(void) const { return _saved_phi; }

  void Set_flag(PHI_OCC_FLAGS f) { _flags = (PHI_OCC_FLAGS)(_flags | f); }
  void Clear_flag(PHI_OCC_FLAGS f) { _flags = (PHI_OCC_FLAGS)(_flags & ~f); }
  bool Is_set_flag(PHI_OCC_FLAGS f) const { return (_flags & f); }

#if 0
  void Set_save(void) { Set_flag(POOF_IS_SAVE); }
  void Set_reload(void) { Set_flag(ROF_IS_RELOAD); }
  void Set_is_lhs(void) { Set_flag(ROF_IS_LHS); }

  void Reset_reload(void) { Clear_flag(ROF_IS_RELOAD); }
#endif
  void Set_is_down_safe(void) { Set_flag(POF_IS_DOWN_SAFE); }
  void Reset_is_down_safe(void) { Clear_flag(POF_IS_DOWN_SAFE); }
  bool Will_be_avail(void) const {
    // return !Is_set_flag(POF_CANT_BE_AVAIL) && Is_set_flag(POF_STOP);
    // Fast hack
    return Is_set_flag(POF_IS_DOWN_SAFE);
  }

  void Print_flag(std::ostream& os) const;
  void Print(std::ostream& os, uint32_t indent) const;

private:
  PHI_OCC_FLAGS _flags;
  HPHI_ID       _saved_phi;
  uint32_t      _num_opnds;
  OCC_ID        _opnds[];
};

class PHI_OPND_OCC_DATA : public OCC_DATA {
public:
  PHI_OPND_OCC_DATA(HEXPR_PTR expr, BB_PTR bb)
      : OCC_DATA(OCC_PHI_OPND, expr, bb), _flags(POOF_NONE) {}

  void Set_flag(PHI_OPND_OCC_FLAGS f) {
    _flags = (PHI_OPND_OCC_FLAGS)(_flags | f);
  }
  void Clear_flag(PHI_OPND_OCC_FLAGS f) {
    _flags = (PHI_OPND_OCC_FLAGS)(_flags & ~f);
  }
  bool Is_set_flag(PHI_OPND_OCC_FLAGS f) const { return (_flags & f); }

  void Set_inserted(void) { Set_flag(POOF_IS_INSERTED); }
  void Set_save(void) { Set_flag(POOF_IS_SAVE); }
  void Set_has_real_use(void) { Set_flag(POOF_HAS_REAL_USE); }
  void Set_is_processed(void) { Set_flag(POOF_IS_PROCESSED); }

  void Reset_has_real_use(void) { Clear_flag(POOF_HAS_REAL_USE); }

  bool Is_inserted(void) const { return Is_set_flag(POOF_IS_INSERTED); }
  bool Is_processed(void) const { return Is_set_flag(POOF_IS_PROCESSED); }
  bool Is_save(void) const { return Is_set_flag(POOF_IS_SAVE); }
  bool Has_real_use(void) const { return Is_set_flag(POOF_HAS_REAL_USE); }

  void   Set_owning_phi_occ(OCC_ID phi_occ) { _phi_occ = phi_occ; }
  OCC_ID Owning_phi_occ(void) const { return _phi_occ; }

  // HEXPR_ID Cur_ver_expr(void) const { return _cur_expr; }
  // void     Set_cur_ver_expr(HEXPR_ID expr) { _cur_expr = expr; }

  HEXPR_ID Saved_expr(void) const { return _saved_expr; }
  void     Set_saved_expr(HEXPR_ID expr) { _saved_expr = expr; }

  void Print_flag(std::ostream& os) const;
  void Print(std::ostream& os, uint32_t indent) const;

private:
  PHI_OPND_OCC_FLAGS _flags;
  OCC_ID             _phi_occ;  // owning phi occ
  BB_ID              _bb;
  HEXPR_ID           _saved_expr;  // the saved VAR expr
};

class REAL_OCC_DATA : public OCC_DATA {
public:
  REAL_OCC_DATA(HEXPR_PTR expr, HSTMT_PTR stmt)
      : OCC_DATA(OCC_REAL, expr, stmt),
        _flags(ROF_NONE),
        _saved_expr(HEXPR_ID()),
        _stmt(stmt->Id()) {}

  void Set_flag(REAL_OCC_FLAGS f) { _flags = (REAL_OCC_FLAGS)(_flags | f); }
  void Clear_flag(REAL_OCC_FLAGS f) { _flags = (REAL_OCC_FLAGS)(_flags & ~f); }
  bool Is_set_flag(REAL_OCC_FLAGS f) const { return (_flags & f); }

  void Set_save(void) { Set_flag(ROF_IS_SAVE); }
  void Set_reload(void) { Set_flag(ROF_IS_RELOAD); }
  void Set_is_lhs(void) { Set_flag(ROF_IS_LHS); }

  void Reset_reload(void) { Clear_flag(ROF_IS_RELOAD); }

  bool Is_save(void) const { return Is_set_flag(ROF_IS_SAVE); }
  bool Is_reload(void) const { return Is_set_flag(ROF_IS_RELOAD); }
  bool Is_lhs(void) const { return Is_set_flag(ROF_IS_LHS); }

  HEXPR_ID Saved_expr(void) { return _saved_expr; }
  void     Set_saved_expr(HEXPR_ID saved_expr) { _saved_expr = saved_expr; }
  void     Gen_save(PRE_CAND_PTR pre_cand, PRE_CONTAINER& pre_cont);
  void     Gen_reload(PRE_CAND_PTR pre_cand, PRE_CONTAINER& pre_cont);

  void     Set_stmt_id(HSTMT_ID stmt) { _stmt = stmt; }
  HSTMT_ID Stmt_id(void) const { return _stmt; }

  void Print_flag(std::ostream& os) const;
  void Print(std::ostream& os, uint32_t indent) const;

private:
  REAL_OCC_FLAGS _flags;
  HEXPR_ID       _saved_expr;
  HSTMT_ID       _stmt;  // enclosing statement
};

class OCC {
public:
  OCC() : _cont(nullptr), _data() {}
  OCC(const PRE_CONTAINER* cont, OCC_DATA_PTR data)
      : _cont(const_cast<PRE_CONTAINER*>(cont)), _data(data) {}

  bool           Is_null(void) const { return _data.Is_null(); }
  CFG&           Cfg(void) const;
  PRE_CONTAINER* Cont(void) const { return _cont; }
  OCC_ID         Id(void) const { return _data.Id(); }
  OCC_ID         Next_id(void) const { return _data->Next_id(); }
  OCC_PTR        Next(void) const;
  bool           Has_next(void) const;
  void           Set_next(OCC_ID next) { _data->Set_next(next); }
  OCC_KIND       Kind(void) const { return _data->Kind(); }
  HEXPR_ID       Expr_id(void) const { return _data->Expr_id(); }
  HEXPR_PTR      Expr(void) const;
  void           Set_expr(HEXPR_PTR expr) { _data->Set_expr_id(expr->Id()); }
  HSTMT_PTR      Stmt(void) const;
  BB_ID          Bb_id(void) const { return _data->Bb_id(); }
  BB_PTR         Bb(void) const;
  void           Set_bb(BB_PTR bb) { _data->Set_bb_id(bb->Id()); }
  uint32_t       Ver(void) const { return _data->Ver(); }
  void           Set_ver(uint32_t v) { _data->Set_ver(v); }
  OCC_PTR        Def(void) const;
  void           Set_def(OCC_PTR def) { _data->Set_def(def); }
  bool           Dominates(OCC_PTR other) {
    BB_PTR bb       = Bb();
    BB_PTR other_bb = other->Bb();
    return bb->Dominates(other_bb);
  }
  void Set_save(void);
  void Set_saved_expr(HEXPR_PTR expr);

  REAL_OCC_DATA_PTR Cast_to_real_occ(void) {
    AIR_ASSERT(Kind() == OCC_REAL);
    return air::base::Static_cast<REAL_OCC_DATA_PTR>(_data);
  }
  REAL_OCC_DATA_PTR Cast_to_real_occ(void) const {
    AIR_ASSERT(Kind() == OCC_REAL);
    return air::base::Static_cast<REAL_OCC_DATA_PTR>(_data);
  }
  PHI_OCC_DATA_PTR Cast_to_phi_occ(void) {
    AIR_ASSERT(Kind() == OCC_PHI);
    return air::base::Static_cast<PHI_OCC_DATA_PTR>(_data);
  }
  PHI_OCC_DATA_PTR Cast_to_phi_occ(void) const {
    AIR_ASSERT(Kind() == OCC_PHI);
    return air::base::Static_cast<PHI_OCC_DATA_PTR>(_data);
  }
  PHI_OPND_OCC_DATA_PTR Cast_to_phi_opnd_occ(void) {
    AIR_ASSERT(Kind() == OCC_PHI_OPND);
    return air::base::Static_cast<PHI_OPND_OCC_DATA_PTR>(_data);
  }
  PHI_OPND_OCC_DATA_PTR Cast_to_phi_opnd_occ(void) const {
    AIR_ASSERT(Kind() == OCC_PHI_OPND);
    return air::base::Static_cast<PHI_OPND_OCC_DATA_PTR>(_data);
  }

  bool Is_dpo_less_than(OCC_PTR other);

  struct LESS {
    bool operator()(const OCC_PTR& a, const OCC_PTR& b) const {
      return a->Id().Value() < b->Id().Value();
    }
  };

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

private:
  PRE_CONTAINER* _cont;
  OCC_DATA_PTR   _data;
};

class PRE_CAND_DATA {
public:
  PRE_CAND_DATA(HEXPR_PTR expr) : _expr(expr->Id()) {
    _real_occ_head     = OCC_ID();
    _phi_occ_head      = OCC_ID();
    _phi_opnd_occ_head = OCC_ID();
    _next              = PRE_CAND_ID();
  }

  HEXPR_ID Expr_id(void) const { return _expr; }
  OCC_ID   Real_occs_id(void) const { return _real_occ_head; }
  OCC_ID   Phi_occs_id(void) const { return _phi_occ_head; }
  OCC_ID   Phi_opnd_occs_id(void) const { return _phi_opnd_occ_head; }

  PRE_CAND_ID Next_id() const { return _next; }
  void        Set_next(PRE_CAND_ID next) { _next = next; }

  bool Check_dpo_order(OCC_LIST& list, OCC_PTR occ) {
    // CMPLR_ASSERT(false, "TO IMPL");
    return true;
  }

  void Append_occ(PRE_CONTAINER* pre_cont, OCC_PTR occ);

  bool Match(PRE_CONTAINER* pre_cont, PRE_CAND_PTR other) const;

  void Print(PRE_CONTAINER* pre_cont, std::ostream& os,
             uint32_t indent = 0) const;

private:
  HEXPR_ID    _expr;
  OCC_ID      _real_occ_head;
  OCC_ID      _phi_occ_head;
  OCC_ID      _phi_opnd_occ_head;
  PRE_CAND_ID _next;
};

class PRE_CAND {
public:
  PRE_CAND(void) : _pre_cont(nullptr), _data() {}
  PRE_CAND(const PRE_CONTAINER* cont, PRE_CAND_DATA_PTR data)
      : _pre_cont(const_cast<PRE_CONTAINER*>(cont)), _data(data) {}

  PRE_CAND_DATA_PTR Data(void) const { return _data; }

  OCC_ID  Real_occs_id(void) { return _data->Real_occs_id(); }
  OCC_ID  Phi_occs_id(void) { return _data->Phi_occs_id(); }
  OCC_ID  Phi_opnd_occs_id(void) { return _data->Phi_opnd_occs_id(); }
  OCC_PTR Real_occs(void);
  OCC_PTR Phi_occs(void);
  OCC_PTR Phi_opnd_occs(void);

  PRE_CONTAINER* Cont(void) const { return _pre_cont; }
  CFG&           Cfg(void) const;

  PRE_CAND_ID Id() const { return _data.Id(); }

  PRE_CAND_ID Next_id() const { return _data->Next_id(); }

  void Set_next(PRE_CAND_ID next) { _data->Set_next(next); }

  bool Is_null(void) const { return _data.Is_null(); }

  HEXPR_ID Expr_id(void) const { return _data->Expr_id(); }

  HEXPR_PTR Expr(void) const;

  void Append_occ(OCC_PTR occ) { _data->Append_occ(_pre_cont, occ); }

  bool Match(PRE_CAND_PTR other) const;

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

private:
  PRE_CONTAINER*    _pre_cont;
  PRE_CAND_DATA_PTR _data;
};

}  // namespace opt

}  // namespace air
#endif
