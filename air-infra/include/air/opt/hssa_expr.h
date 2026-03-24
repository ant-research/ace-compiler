//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_HSSA_EXPR_H
#define AIR_OPT_HSSA_EXPR_H

#include "air/base/node.h"
#include "air/base/opcode.h"
#include "air/base/ptr_wrapper.h"
#include "air/base/st.h"
#include "air/base/st_attr.h"
#include "air/core/opcode.h"
#include "air/opt/hssa_decl.h"
#include "air/opt/hssa_mu_chi.h"
#include "air/opt/ssa_decl.h"

//! @brief Define for HSSA

namespace air {
namespace opt {

//! @brief HSSA Expression Kind
enum EXPR_KIND {
  EK_INVALID = 0x00,  //!< Invalid EXPR
  EK_LDA     = 0x01,  //!< Load address
  EK_CONST   = 0x02,  //!< Compile time constant
  EK_RCONST  = 0x04,  //!< symbolic constant or constant in ST
  EK_VAR     = 0x08,  //!< Variable
  EK_IVAR    = 0x10,  //!< Indirect load
  EK_OP      = 0x20,  //!< Operator
  EK_DELETED = 0x40,  //!< Code node is deleted
};

//! @brief HSSA Expression Flags
enum EXPR_FLAG {
  EF_EMPTY = 0x00,  //!< no flag set
};

//! @brief VAR Kind
enum VAR_KIND {
  VK_INVALID,     //!< Invalid kind to capture error
  VK_PREG,        //!< For a PREG or PREG struct field
  VK_ADDR_DATUM,  //!< For a ADDR_DADUM or ADDR_DATUM field or element
};

//! @brief VAR DEF INFO
enum VAR_DEF_BY {
  DEF_BY_NONE,  //!< defined by none
  DEF_BY_CHI,   //!< defined by a chi
  DEF_BY_PHI,   //!< defined by a phi
  DEF_BY_STMT,  //!< defined by a statment
};

//! @brief Constant kind
enum CST_KIND {
  CK_INVALID,  //!< invalid constant kind
  CK_INT,      //!< integer constant
  CK_FLOAT,    //!< float constant
  CK_ID,       //!< symbolic constant specified by ID (string/array/struct)
};

//! @brief HEXPR_DATA holds the common data for HSSA Expression
class HEXPR_DATA {
public:
  friend class HEXPR;
  HEXPR_DATA(air::base::NODE_PTR node, EXPR_KIND k);
  HEXPR_DATA(air::base::OPCODE opcode, EXPR_KIND k, air::base::TYPE_ID rtype,
             air::base::TYPE_ID dsctype, air::base::ATTR_ID attr,
             const air::base::SPOS& spos)
      : _opc(opcode),
        _kind(k),
        _rtype(rtype),
        _dsctype(dsctype),
        _flags(EF_EMPTY),
        _attr(attr),
        _spos(spos),
        _next(HEXPR_ID()) {}

  HEXPR_DATA(EXPR_KIND k)
      : _opc(air::base::OPCODE::INVALID),
        _kind(k),
        _rtype(air::base::TYPE_ID()),
        _dsctype(air::base::TYPE_ID()),
        _flags(EF_EMPTY),
        _attr(air::base::ATTR_ID()),
        _spos(),
        _next(HEXPR_ID()) {}

  HEXPR_DATA(const HEXPR_DATA& other)
      : _opc(other.Opcode()),
        _kind(other.Kind()),
        _rtype(other.Rtype()),
        _dsctype(other.Dsctype()),
        _flags(other.Flags()),
        _attr(other.Attr()),
        _spos(other.Spos()),
        _next(HEXPR_ID()) {}

  EXPR_KIND          Kind(void) const { return _kind; }
  HEXPR_ID           Next(void) const { return _next; }
  air::base::OPCODE  Opcode(void) const { return _opc; }
  air::base::TYPE_ID Dsctype(void) const { return _dsctype; }
  air::base::TYPE_ID Rtype(void) const { return _rtype; }
  air::base::ATTR_ID Attr(void) const { return _attr; }
  air::base::SPOS    Spos(void) const { return _spos; }
  EXPR_FLAG          Flags(void) const { return _flags; }
  bool               Is_set_flag(EXPR_FLAG f) const { return _flags & f; }

  void Set_flag(EXPR_FLAG flag) { _flags = (EXPR_FLAG)(_flags | flag); }
  void Set_rtype(air::base::TYPE_ID rtype) { _rtype = rtype; }
  void Set_opcode(air::base::OPCODE opcode) { _opc = opcode; }
  void Set_next(HEXPR_ID next) { _next = next; }

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print(void) const;
  std::string To_str(void) const;

private:
  EXPR_KIND          _kind;     //!< EXPR kind
  EXPR_FLAG          _flags;    //!< EXPR flags
  air::base::OPCODE  _opc;      //!< EXPR opcode
  air::base::TYPE_ID _rtype;    //!< EXPR return type
  air::base::TYPE_ID _dsctype;  //!< EXPR descriptor type
  air::base::ATTR_ID _attr;     //!< EXPR attribute inherited from AIR NODE
  air::base::SPOS    _spos;     //!< EXPR source position
  HEXPR_ID           _next;     //!< Next Expression
};

//! @brief VAR_DATA holds the information of a EXPR_VAR kind expression
// TODO: should be decouple with SSA info
class VAR_DATA : public HEXPR_DATA {
public:
  VAR_DATA(void)
      : HEXPR_DATA(EK_VAR),
        _var_kind(VK_INVALID),
        _chi(air::base::Null_id),
        _var_id(0),
        _sub_idx(SSA_SYM::NO_INDEX),
        _ver(air::base::Null_prim_id),
        _def_by(DEF_BY_NONE) {}

  VAR_DATA(SSA_SYM_PTR sym) : HEXPR_DATA(EK_VAR) {
    if (sym->Kind() == SSA_SYM_KIND::PREG)
      _var_kind = VAR_KIND::VK_PREG;
    else if (sym->Kind() == SSA_SYM_KIND::ADDR_DATUM)
      _var_kind = VAR_KIND::VK_ADDR_DATUM;
    else
      _var_kind = VAR_KIND::VK_INVALID;
    _var_id  = sym->Var_id();
    _sub_idx = sym->Index();
    _ver     = air::base::Null_prim_id;
    _def_by  = DEF_BY_NONE;
    _chi     = air::base::Null_id;
    Set_rtype(sym->Type_id());
  }

  VAR_DATA(VAR_DATA_PTR var_expr) : HEXPR_DATA(EK_VAR) {
    _var_kind = var_expr->_var_kind;
    _var_id   = var_expr->_var_id;
    _sub_idx  = var_expr->_sub_idx;
    _ver      = air::base::Null_prim_id;
    _def_by   = DEF_BY_NONE;
    _chi      = air::base::Null_id;
    Set_rtype(var_expr->Rtype());
  }

  VAR_DATA(air::base::ADDR_DATUM_PTR datum,
           uint32_t                  sub_idx = SSA_SYM::NO_INDEX)
      : HEXPR_DATA(EK_VAR) {
    _var_kind = VAR_KIND::VK_ADDR_DATUM;
    _var_id   = datum->Id().Value();
    _sub_idx  = sub_idx;
    _ver      = air::base::Null_prim_id;
    _def_by   = DEF_BY_NONE;
    _chi      = air::base::Null_id;
    Set_rtype(datum->Type_id());
  }

  VAR_DATA(air::base::PREG_PTR preg) : HEXPR_DATA(EK_VAR) {
    _var_kind = VAR_KIND::VK_PREG;
    _var_id   = preg->Id().Value();
    _sub_idx  = SSA_SYM::NO_INDEX;
    _ver      = air::base::Null_prim_id;
    _def_by   = DEF_BY_NONE;
    _chi      = air::base::Null_id;
    Set_rtype(preg->Type_id());
  }

  VAR_KIND Var_kind(void) const { return _var_kind; }
  uint32_t Var_id(void) const { return _var_id; }
  uint32_t Sub_idx(void) const { return _sub_idx; }
  uint32_t Ver(void) const { return _ver; }
  bool     Def_by_none(void) const { return (_def_by == DEF_BY_NONE); }
  bool     Def_by_phi(void) const { return (_def_by == DEF_BY_PHI); }
  bool     Def_by_chi(void) const { return (_def_by == DEF_BY_CHI); }
  bool     Def_by_stmt(void) const { return (_def_by == DEF_BY_STMT); }
  HPHI_ID  Def_phi(void) const {
    AIR_ASSERT(_def_by == DEF_BY_PHI);
    return _phi;
  }
  HSTMT_ID Def_stmt(void) const {
    AIR_ASSERT(_def_by == DEF_BY_STMT);
    return _defstmt;
  }
  HCHI_ID Def_chi(void) const {
    AIR_ASSERT(_def_by == DEF_BY_CHI);
    return _chi;
  }

  void Set_sub_idx(uint32_t sub_idx) { _sub_idx = sub_idx; }
  void Set_ver(uint32_t ver) { _ver = ver; }
  void Set_def_phi(HPHI_ID id) {
    _def_by = DEF_BY_PHI;
    _phi    = id;
  }
  void Set_def_stmt(HSTMT_ID id) {
    _def_by  = DEF_BY_STMT;
    _defstmt = id;
  }
  void Set_def_chi(HCHI_ID id) {
    _def_by = DEF_BY_CHI;
    _chi    = id;
  }

  std::string Name(HCONTAINER* cont) const;

  //! @brief content match
  bool Match(VAR_DATA_PTR other) const;

  //! @brief lexical match
  bool Match_lex(VAR_DATA_PTR other) const;

  //! @brief Emit VAR_DATA as left hand side returns AIR STMT_PTR
  air::base::STMT_PTR Emit_lhs(air::base::CONTAINER*  cont,
                               air::base::NODE_PTR    rhs,
                               const air::base::SPOS& spos) const;

  //! @brief Emit VAR_DATA as right hand side, returns AIR NODE_PTR
  air::base::NODE_PTR Emit_rhs(air::base::CONTAINER*  cont,
                               const air::base::SPOS& spos) const;

  void Print(HCONTAINER* hssa_cont, std::ostream& os,
             uint32_t indent = 0) const;

private:
  VAR_KIND   _var_kind;    //!< PREG or ADDR_DATUM
  uint32_t   _var_id;      //!< ADDR_DATUM_ID or PREG_ID
  uint32_t   _sub_idx;     //!< Field id or element index
  uint32_t   _ver;         //!< SSA version number
  VAR_DEF_BY _def_by : 3;  //!< var def by
  union {
    HCHI_ID  _chi;      //!< the chi node that define this expr
    HPHI_ID  _phi;      //!< the phi node that define this expr
    HSTMT_ID _defstmt;  //!< statement that defines this var
  };
};

//! @brief CST_DATA holds the information of a const kind expression
class CST_DATA : public HEXPR_DATA {
public:
  CST_DATA(air::base::NODE_PTR node);

  CST_DATA(uint64_t val, air::base::TYPE_ID ty) : HEXPR_DATA(EK_CONST) {
    Set_opcode(air::base::OPCODE(air::core::LDC));
    Set_rtype(ty);
    _cst_kind  = CK_INT;
    _const_val = val;
  }

  CST_DATA(CST_DATA_PTR other) : HEXPR_DATA(*(other.Addr())) {
    _cst_kind  = other->Cst_kind();
    _const_val = other->_const_val;
  }

  CST_KIND Cst_kind(void) const { return _cst_kind; }
  uint64_t Cst_val(void) const {
    AIR_ASSERT(_cst_kind == CK_INT);
    return _const_val;
  }
  air::base::CONSTANT_ID Cst_id(void) const {
    AIR_ASSERT(_cst_kind == CK_ID);
    return _const_id;
  }

  void Set_cst_kind(CST_KIND k) { _cst_kind = k; }
  void Set_value(uint64_t val) { _const_val = val; }
  void Set_value(air::base::CONSTANT_ID id) { _const_id = id; }

  //! @brief returns hash table index
  uint32_t Hash_idx(void) const;

  //! @brief content match
  bool Match(CST_DATA_PTR other) const;

  //! @brief Emit CST_DATA, returns an AIR NODE_PTR
  air::base::NODE_PTR Emit(air::base::CONTAINER* cont) const;

  void Print(HCONTAINER* hssa_cont, std::ostream& os,
             uint32_t indent = 0) const;

private:
  CST_KIND _cst_kind;  //!< constant kind
  union {
    air::base::CONSTANT_ID _const_id;   //!< symbolic constant
    uint64_t               _const_val;  //!< constant value
  };
};

//! @brief OP_DATA holds the information of a EK_OP kind expression
class OP_DATA : public HEXPR_DATA {
public:
  OP_DATA(air::base::NODE_PTR node) : HEXPR_DATA(node, EK_OP) {
    _kid_cnt = node->Num_child();
  }

  OP_DATA(OP_DATA_PTR other) : HEXPR_DATA(*(other.Addr())) {
    _kid_cnt = other->_kid_cnt;
    for (uint32_t idx = 0; idx < Kid_cnt(); idx++) {
      _kids[idx] = other->Kid(idx);
    }
  }

  OP_DATA(air::base::OPCODE opcode, uint32_t kid_cnt, air::base::TYPE_ID rtype,
          air::base::TYPE_ID dsctype, air::base::SPOS spos)
      : HEXPR_DATA(opcode, EK_OP, rtype, dsctype, air::base::Null_id, spos) {
    _kid_cnt = kid_cnt;
  }

  //! @brief static method to allocate a temporay OP_DATA
  static OP_DATA* Alloc(uint32_t kid_cnt) {
    return (OP_DATA*)malloc(OP_DATA::Size(kid_cnt));
  }

  //! @brief static method to get the memory size of an OP_DATA
  static size_t Size(uint32_t kid_cnt) {
    return sizeof(OP_DATA) + kid_cnt * sizeof(HEXPR_ID);
  }

  uint32_t Kid_cnt(void) const { return _kid_cnt; }

  HEXPR_ID Kid(uint32_t idx) const {
    AIR_ASSERT(idx < Kid_cnt());
    return _kids[idx];
  }

  void Set_kid_cnt(uint32_t kid_cnt) { _kid_cnt = kid_cnt; }

  void Set_kid(uint32_t kid_idx, HEXPR_ID id) {
    AIR_ASSERT(kid_idx < _kid_cnt);
    _kids[kid_idx] = id;
  }

  //! @brief returns hash table index
  uint32_t Hash_idx(void) const;

  //! @brief content match
  bool Match(OP_DATA_PTR other) const;

  //! @brief Emit CST_DATA, returns an AIR NODE_PTR
  air::base::NODE_PTR Emit(air::base::CONTAINER* cont,
                           HCONTAINER*           hssa_cont) const;

  void Print(HCONTAINER* hssa_cont, std::ostream& os,
             uint32_t indent = 0) const;

private:
  uint32_t _kid_cnt;  //!< number of kids
  HEXPR_ID _kids[];   //!< operation kids
};

//! @brief HEXPR - Wrapper for accessing all kinds of EXPR data
class HEXPR {
  friend class HCONTAINER;

public:
  HEXPR(void) = delete;
  HEXPR(const HCONTAINER* cont, HEXPR_DATA_PTR data)
      : _cont(const_cast<HCONTAINER*>(cont)), _data(data) {}

  HCONTAINER*            Hssa_cont(void) const { return _cont; }
  air::base::FUNC_SCOPE* Func_scope(void) const;
  HEXPR_DATA_PTR         Data(void) const { return _data; }
  EXPR_KIND              Kind(void) const { return _data->Kind(); }
  bool                Is_null(void) const { return _data.Id() == HEXPR_ID(); }
  HEXPR_ID            Id(void) const { return _data.Id(); }
  HEXPR_ID            Next_id(void) const { return _data->Next(); }
  uint32_t            Domain(void) const { return Opcode().Domain(); }
  uint32_t            Operator(void) const { return Opcode().Operator(); }
  air::base::OPCODE   Opcode(void) const { return _data->Opcode(); }
  air::base::TYPE_ID  Rtype_id(void) const { return _data->Rtype(); }
  air::base::TYPE_PTR Rtype(void) const;
  air::base::TYPE_ID  Dsctype_id(void) const { return _data->Dsctype(); }
  uint32_t            Kid_cnt(void) const;
  int32_t             Kid_idx(HEXPR_PTR kid) const;
  HEXPR_PTR           Kid(uint32_t idx) const;
  air::base::SPOS     Spos(void) const { return _data->Spos(); }
  HPHI_PTR            Def_phi(void) const;
  HSTMT_PTR           Def_stmt(void) const;
  BB_ID               Def_bb(void) const;
  const air::base::ATTR_ID Attr(void) const { return _data->Attr(); }
  DECLATR_ATTR_ACCESS_API(Attr(), (air::base::SCOPE_BASE*)Func_scope())

  void Set_next(HEXPR_ID next) { _data->Set_next(next); }
  void Set_rtype(air::base::TYPE_ID rtype) { _data->Set_rtype(rtype); }
  void Set_flag(EXPR_FLAG flag) { _data->Set_flag(flag); }
  void Set_defphi(HPHI_PTR hphi);
  void Set_kid(uint32_t idx, HEXPR_ID id);
  void Set_kid(uint32_t idx, HEXPR_PTR expr);

  VAR_DATA_PTR Cast_to_var_expr(void) const {
    AIR_ASSERT(Kind() == EK_VAR);
    return air::base::Static_cast<VAR_DATA_PTR>(_data);
  }

  OP_DATA_PTR Cast_to_op_expr(void) const {
    AIR_ASSERT(Kind() == EK_OP);
    return air::base::Static_cast<OP_DATA_PTR>(_data);
  }

  CST_DATA_PTR Cast_to_cst_expr(void) const {
    AIR_ASSERT(Kind() == EK_CONST);
    return air::base::Static_cast<CST_DATA_PTR>(_data);
  }

  //! @brief returns hash table index
  uint32_t Hash_idx(void) const;

  //! @brief content match
  bool Match(HEXPR_PTR other, HCONTAINER* cont = NULL) const;

  //! @brief lexical match
  bool Match_lex(HEXPR_PTR other) const;

  //! @brief replace expr with new_expr in current expression tree
  //! @return true if replaced, otherwise returns false
  bool Replace_expr(HEXPR_ID expr, HEXPR_ID new_expr);

  //! @brief check if current expr have the same version with expr
  //!        recursive checking the kid
  //! @return true if all versions are the same, otherwise returns false
  bool Is_same_e_ver(HEXPR_PTR expr) const;

  //! @brief Emit HEXPR, returns an AIR NODE_PTR
  air::base::NODE_PTR Emit(air::base::CONTAINER* cont) const;

  static void Print_opcode(air::base::OPCODE opcode, std::ostream& os,
                           uint32_t indent = 0);

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print(void) const;
  std::string To_str(void) const;

private:
  HCONTAINER*    _cont;  //!< owning container
  HEXPR_DATA_PTR _data;  //!< expression data ptr
};

}  // namespace opt
}  // namespace air
#endif