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

//! @brief HSSA statement Kind
enum HSTMT_KIND {
  SK_INVALID,  //!< invalid statement
  SK_ASSIGN,   //!< assignment statement
  SK_NARY,     //!< nary operation statement
  SK_CALL,     //!< call statement
};

//! @brief HSSA statement attribute
class HSTMT_ATTR {
public:
  HSTMT_ATTR(void) : _dead(0) {}
  uint32_t _dead : 1;  //!< statement is dead
};

//! @brief HSTMT_DATA holds the common data for HSSA statement
class HSTMT_DATA {
  friend class HSTMT;

public:
  HSTMT_DATA(air::base::NODE_PTR node, HSTMT_KIND k = SK_INVALID)
      : _kind(k),
        _attr(),
        _bb(BB_ID()),
        _opcode(node->Opcode()),
        _spos(node->Spos()),
        _next(HSTMT_ID()) {}

  HSTMT_DATA(air::base::OPCODE opcode, HSTMT_KIND k = SK_INVALID)
      : _kind(k),
        _attr(),
        _bb(BB_ID()),
        _opcode(opcode),
        _spos(),
        _next(HSTMT_ID()) {}

  HSTMT_KIND        Kind(void) const { return _kind; }
  HSTMT_ATTR        Attr(void) const { return _attr; }
  BB_ID             Bb_id(void) const { return _bb; }
  air::base::OPCODE Opcode(void) const { return _opcode; }
  air::base::SPOS   Spos(void) const { return _spos; }
  HSTMT_ID          Next(void) { return _next; }

  void Set_next(HSTMT_ID next) { _next = next; }
  void Set_bb_id(BB_ID id) { _bb = id; }
  void Set_dead(void) { _attr._dead = 1; }

  bool Is_dead(void) const { return _attr._dead == 1; }

  void Print(HCONTAINER* cont, std::ostream& os, uint32_t indent) const;

private:
  HSTMT_KIND        _kind;    //!< statement kind
  HSTMT_ATTR        _attr;    //!< statement attribute
  BB_ID             _bb;      //!< the block id it belongs to
  air::base::OPCODE _opcode;  //!< air opcode
  air::base::SPOS   _spos;    //!< source position information
  HSTMT_ID          _next;    //!< next statment id
};

//! @brief ASSIGN_DATA holds the data for assign statement
class ASSIGN_DATA : public HSTMT_DATA {
public:
  ASSIGN_DATA(air::base::NODE_PTR node)
      : HSTMT_DATA(node, SK_ASSIGN),
        _mu(HMU_ID()),
        _chi(HCHI_ID()),
        _lhs(HEXPR_ID()),
        _rhs(HEXPR_ID()) {}

  ASSIGN_DATA(air::base::OPCODE opcode, HEXPR_ID lhs, HEXPR_ID rhs)
      : HSTMT_DATA(opcode, SK_ASSIGN),
        _mu(HMU_ID()),
        _chi(HCHI_ID()),
        _lhs(lhs),
        _rhs(rhs) {}

  HEXPR_ID Lhs(void) const { return _lhs; }
  HEXPR_ID Rhs(void) const { return _rhs; }
  HMU_ID   Mu(void) const { return _mu; }
  HCHI_ID  Chi(void) const { return _chi; }

  void Set_lhs(HEXPR_ID lhs) { _lhs = lhs; }
  void Set_rhs(HEXPR_ID rhs) { _rhs = rhs; }
  void Set_chi(HCHI_ID chi) { _chi = chi; }

  void Print(HCONTAINER* cont, std::ostream& os, uint32_t indent) const;

private:
  HMU_ID   _mu;   //!< may use list head ID
  HCHI_ID  _chi;  //!< may def list head ID
  HEXPR_ID _lhs;  //!< left hand side expression ID (EK_VAR)
  HEXPR_ID _rhs;  //!< right hand side expression ID
};

//! @brief NARY_DATA holds the data for nary operation statement
class NARY_DATA : public HSTMT_DATA {
  friend class HSTMT;

public:
  NARY_DATA(air::base::NODE_PTR node)
      : HSTMT_DATA(node, SK_NARY),
        _mu(HMU_ID()),
        _chi(HCHI_ID()),
        _kid_cnt(node->Num_child()) {}

  //! @brief static method to get the size of NARY_DATA with given kid count
  static size_t Size(uint32_t kid_cnt) {
    return sizeof(NARY_DATA) + kid_cnt * sizeof(HEXPR_ID);
  }

  HMU_ID   Mu(void) const { return _mu; }
  HCHI_ID  Chi(void) const { return _chi; }
  uint32_t Kid_cnt(void) const { return _kid_cnt; }
  HEXPR_ID Kid(uint32_t idx) const {
    AIR_ASSERT(idx < Kid_cnt());
    return _kids[idx];
  }

  void Set_kid(uint32_t idx, HEXPR_ID kid) {
    AIR_ASSERT(idx < _kid_cnt);
    _kids[idx] = kid;
  }
  void Set_chi(HCHI_ID chi) { _chi = chi; }
  void Set_mu(HMU_ID mu) { _mu = mu; }

  void Print(HCONTAINER* cont, std::ostream& os, uint32_t indent) const;

private:
  HMU_ID   _mu;       //!< may use list head ID
  HCHI_ID  _chi;      //!< may def list head ID
  uint32_t _kid_cnt;  //!< number of kids
  HEXPR_ID _kids[0];  //!< kids array
};

//! @brief CALL_DATA holds the data for call statement
class CALL_DATA : public HSTMT_DATA {
public:
  CALL_DATA(air::base::NODE_PTR node) : HSTMT_DATA(node, SK_CALL) {
    _entry   = node->Entry_id();
    _retv    = HEXPR_ID();
    _mu      = HMU_ID();
    _chi     = HCHI_ID();
    _arg_cnt = node->Num_arg();
  }

  //! @brief static method to get the size of CALL_DATA with given arg count
  static size_t Size(uint32_t arg_cnt) {
    return sizeof(NARY_DATA) + arg_cnt * sizeof(HEXPR_ID);
  }

  air::base::ENTRY_ID Entry(void) const { return _entry; }
  HEXPR_ID            Retv(void) const { return _retv; }
  HMU_ID              Mu(void) const { return _mu; }
  HCHI_ID             Chi(void) const { return _chi; }
  uint32_t            Arg_cnt(void) const { return _arg_cnt; }
  HEXPR_ID            Arg(uint32_t idx) const {
    AIR_ASSERT(idx < Arg_cnt());
    return _args[idx];
  }

  void Set_mu(HMU_ID mu) { _mu = mu; }
  void Set_chi(HCHI_ID chi) { _chi = chi; }
  void Set_retv(HEXPR_ID retv) { _retv = retv; }
  void Set_arg(uint32_t idx, HEXPR_ID kid) {
    AIR_ASSERT(idx < _arg_cnt);
    _args[idx] = kid;
  }

  void Print(HCONTAINER* cont, std::ostream& os, uint32_t indent) const;

private:
  air::base::ENTRY_ID _entry;    //!< call entry ID
  HEXPR_ID            _retv;     //!< return expression ID
  HMU_ID              _mu;       //!< may use list head
  HCHI_ID             _chi;      //!< may def list head
  uint32_t            _arg_cnt;  //!< argument cnt
  HEXPR_ID            _args[0];  //!< argument expressions
};

//! @brief HSTMT: Wrapper for accessing all kinds of HSSA Statement data
class HSTMT {
public:
  HSTMT(void) : _cont(nullptr), _data() {}
  HSTMT(const HCONTAINER* cont, HSTMT_DATA_PTR data)
      : _cont(const_cast<HCONTAINER*>(cont)), _data(data) {}

  HCONTAINER*       Hssa_cont(void) const { return _cont; }
  HSTMT_KIND        Kind(void) const { return _data->Kind(); }
  HSTMT_ID          Id(void) const { return _data.Id(); }
  HSTMT_ID          Next_id(void) const { return _data->Next(); }
  BB_ID             Bb_id(void) const { return _data->Bb_id(); }
  BB_PTR            Bb(CFG* cfg) const;
  uint32_t          Domain(void) const { return Opcode().Domain(); }
  uint32_t          Operator(void) const { return Opcode().Operator(); }
  air::base::OPCODE Opcode(void) const { return _data->_opcode; }
  air::base::SPOS   Spos(void) const { return _data->Spos(); }
  HSTMT_DATA_PTR    Data(void) const { return _data; }
  HSTMT_ATTR        Attr(void) const { return _data->Attr(); }
  bool              Is_null(void) const { return _data.Is_null(); }
  bool              Is_dead(void) const { return _data->Is_dead(); }
  HEXPR_ID          Lhs_id(void) const;
  HEXPR_ID          Rhs_id(void) const;
  HEXPR_PTR         Lhs(void) const;
  HEXPR_PTR         Rhs(void) const;
  HCHI_ID           Chi(void) const;
  HMU_ID            Mu(void) const;

  ASSIGN_DATA_PTR Cast_to_assign(void) const {
    AIR_ASSERT(Kind() == SK_ASSIGN);
    return air::base::Static_cast<ASSIGN_DATA_PTR>(_data);
  }

  NARY_DATA_PTR Cast_to_nary(void) const {
    AIR_ASSERT(Kind() == SK_NARY);
    return air::base::Static_cast<NARY_DATA_PTR>(_data);
  }

  CALL_DATA_PTR Cast_to_call(void) const {
    AIR_ASSERT(Kind() == SK_CALL);
    return air::base::Static_cast<CALL_DATA_PTR>(_data);
  }

  void              Set_bb_id(BB_ID id) { _data->Set_bb_id(id); }
  void              Set_dead(void) { _data->Set_dead(); }
  void              Set_next(HSTMT_ID next) { _data->Set_next(next); }
  void              Set_lhs(HEXPR_ID lhs) { Cast_to_assign()->Set_lhs(lhs); }
  void              Set_rhs(HEXPR_ID rhs) { Cast_to_assign()->Set_rhs(rhs); }
  void              Set_chi(HCHI_ID chi);
  air::base::OPCODE Set_opcode(air::base::OPCODE opcode) {
    return _data->_opcode = opcode;
  }

  //! @brief replace expr with new_expr in current statement
  //! @return true if replaced, otherwise returns false
  bool Replace_expr(HEXPR_ID expr, HEXPR_ID new_expr);

  //! @brief check if current statement dominate stmt
  //! @return true if dominate, otherwise returns false
  bool Is_dominate(HSTMT_PTR stmt) const;

  //! @brief Emit HEXPR, returns an AIR STMT_PTR
  air::base::STMT_PTR Emit(air::base::CONTAINER* cont) const;

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print(void) const;
  std::string To_str(void) const;

private:
  HCONTAINER*    _cont;  //!< owning container
  HSTMT_DATA_PTR _data;  //!< statement data ptr
};

}  // namespace opt
}  // namespace air

#endif