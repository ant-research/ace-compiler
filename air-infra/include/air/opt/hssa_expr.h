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

typedef uint8_t  MTYPE;
typedef uint32_t IDTYPE;

enum CODEKIND {
  CK_INVALID = 0x0,   // Invalid CR
  CK_LDA     = 0x01,  // Load address
  CK_CONST   = 0x02,  // Compile time constant
  CK_RCONST  = 0x04,  // symbolic constant or constant in ST
  CK_VAR     = 0x08,  // Variable
  CK_IVAR    = 0x10,  // Indirect load
  CK_OP      = 0x20,  // Operator
  CK_DELETED = 0x40,  // Code node is deleted
  // do not add new values without adding more bits to HCR's kind field
};

enum CR_FLAG {
  CF_EMPTY         = 0x00,  // no flag set
  CF_C_P_PROCESSED = 0x01,  // For non-leaf only: has been processed by copy
                            // propagation
  CF_LDA_LABEL    = 0x01,   // this CK_LDA node is an LDA_LABEL
  CF_DONT_PROP    = 0x02,   // Do not copy propagate this CK_VAR/CK_IVAR
  CF_C_P_REHASHED = 0x02,   // For non-leaf only: has been rehashed due to copy
                            // propagation of the var/ivar nodes within it;
                            // kid[0] will point to replacing node
  CF_SPRE_REMOVED = 0x04,   // the store of this variable has been removed by
                            // SPRE, so any use of this node needs to be renamed
  CF_DEF_BY_PHI = 0x08,     // defined by phi
  CF_DEF_BY_CHI = 0x10,     // defined by chi
                            // if defined by normal coderep, no flag
                            // but w defstmt
  CF_OWNED_BY_TEMP = 0x20,  // for CK_OP and CK_IVAR only, in SSAPRE, coderep
                            // is to be changed to temporary
  CF_INCOMPLETE_USES = 0x40,   // CK_VAR only; indicates it is converted from
                               // a zero version by Find_def(), but not all its
                               // uses have been converted; i.e. there may be a
                               // zero-version use that should be this node
  CF_IS_ZERO_VERSION = 0x80,   // is a zero version
  CF_FOLDED_LDID     = 0x100,  // is folded from (ILOAD(LDA))
  CF_MADEUP_TYPE     = 0x200,  // the type is made up by SSA
  // do not add new values without checking HCR's flags field
};

enum ISOP_FLAG {
  ISOP_EMPTY = 0x00,  // no flag set
};

// return value of Propagatable to tell whether an expression can be
// propagated to the current point in the code
enum PROPAGATABILITY {
  NOT_PROPAGATABLE,   // cannot be propagated
  PROP_WITH_INVERSE,  // can be propagated only after non-current
                      // versions are transformed to current version
                      // using inverse function
  PROPAGATABLE,       // can be propagated
  // must be in increasing order of propagatability so that, when combining
  // two values, and just take the min
};

class HCR_DATA {
public:
  friend class HCR;
  HCR_DATA(air::base::NODE_PTR node, CODEKIND k);
  HCR_DATA(air::base::OPCODE opcode, CODEKIND k, air::base::TYPE_ID rtype,
           air::base::TYPE_ID dsctype, air::base::ATTR_ID attr,
           const air::base::SPOS& spos)
      : _opc(opcode),
        _kind(k),
        _rtype(rtype),
        _dsctype(dsctype),
        _usecnt(0),
        _flags(CF_EMPTY),
        _attr(attr),
        _spos(spos),
        _next(HCR_ID()) {}
  HCR_DATA(CODEKIND k)
      : _opc(air::base::OPCODE::INVALID),
        _kind(k),
        _rtype(air::base::TYPE_ID()),
        _dsctype(air::base::TYPE_ID()),
        _usecnt(0),
        _flags(CF_EMPTY),
        _attr(air::base::ATTR_ID()),
        _spos(),
        _next(HCR_ID()) {}

  CODEKIND Kind() const { return _kind; }
  void     Set_flag(CR_FLAG flag) { _flags = (CR_FLAG)(_flags | flag); }
  void     Set_rtype(air::base::TYPE_ID rtype) { _rtype = rtype; }

  void               Set_opcode(air::base::OPCODE opcode) { _opc = opcode; }
  air::base::OPCODE  Opcode(void) const { return _opc; }
  air::base::TYPE_ID Dsctype(void) const { return _dsctype; }
  air::base::TYPE_ID Rtype(void) const { return _rtype; }
  HCR_ID             Next() { return _next; }
  void               Set_next(HCR_ID next) { _next = next; }
  const air::base::ATTR_ID& Attr(void) const { return _attr; }
  air::base::SPOS           Spos(void) const { return _spos; }

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

private:
  air::base::OPCODE  _opc;
  CODEKIND           _kind;
  air::base::TYPE_ID _rtype;
  air::base::TYPE_ID _dsctype;
  uint32_t           _usecnt;
  CR_FLAG            _flags;
  air::base::ATTR_ID _attr;
  air::base::SPOS    _spos;
  HCR_ID             _next;
};

enum VAR_DEF_BY {
  DEF_BY_NONE,
  DEF_BY_CHI,
  DEF_BY_PHI,
  DEF_BY_STMT,
};

enum VAR_KIND {
  UNKNOWN,     //!< Unknown kind to capture error
  PREG,        //!< For a PREG or PREG struct field
  ADDR_DATUM,  //!< For a ADDR_DADUM or ADDR_DATUM field or element
};

// TODO: VAR_HCR_DATA should be decouple with SSA info
class VAR_HCR_DATA : public HCR_DATA {
public:
  VAR_HCR_DATA(void)
      : HCR_DATA(CK_VAR),
        _ver_id(SSA_VER_ID()),
        _var_kind(VAR_KIND::UNKNOWN),
        _chi(air::base::Null_id),
        _var_id(0),
        _sub_idx(SSA_SYM::NO_INDEX),
        _def_by(DEF_BY_NONE) {}

  VAR_HCR_DATA(SSA_SYM_PTR sym) : HCR_DATA(CK_VAR) {
    if (sym->Kind() == SSA_SYM_KIND::PREG)
      _var_kind = VAR_KIND::PREG;
    else if (sym->Kind() == SSA_SYM_KIND::ADDR_DATUM)
      _var_kind = VAR_KIND::ADDR_DATUM;
    else
      _var_kind = UNKNOWN;
    _chi     = air::base::Null_id;
    _var_id  = sym->Var_id();
    _sub_idx = sym->Index();
    _def_by  = DEF_BY_NONE;
    Set_rtype(sym->Type_id());
  }

  VAR_HCR_DATA(VAR_HCR_DATA_PTR var_cr)
      : HCR_DATA(CK_VAR), _ver_id(SSA_VER_ID()) {
    _var_kind = var_cr->_var_kind;
    _chi      = air::base::Null_id;
    _var_id   = var_cr->_var_id;
    _sub_idx  = var_cr->_sub_idx;
    _def_by   = DEF_BY_NONE;
    _ver      = 0;
    Set_rtype(var_cr->Rtype());
  }

  VAR_HCR_DATA(air::base::PREG_PTR preg) : HCR_DATA(CK_VAR) {
    _var_kind = VAR_KIND::PREG;
    _chi      = air::base::Null_id;
    _var_id   = preg->Id().Value();
    _sub_idx  = SSA_SYM::NO_INDEX;
    _def_by   = DEF_BY_NONE;
    Set_rtype(preg->Type_id());
  }

  VAR_HCR_DATA(air::base::ADDR_DATUM_PTR datum,
               uint32_t                  sub_idx = SSA_SYM::NO_INDEX)
      : HCR_DATA(CK_VAR) {
    _var_kind = VAR_KIND::ADDR_DATUM;
    _chi      = air::base::Null_id;
    _var_id   = datum->Id().Value();
    _sub_idx  = sub_idx;
    _def_by   = DEF_BY_NONE;
    Set_rtype(datum->Type_id());
  }

  VAR_KIND Var_kind(void) const { return _var_kind; }
  uint32_t Var_id(void) const { return _var_id; }
  uint32_t Sub_idx(void) const { return _sub_idx; }
  void     Set_sub_idx(uint32_t sub_idx) { _sub_idx = sub_idx; }
  uint32_t Ver(void) const { return _ver; }
  void     Set_ver(uint32_t ver) { _ver = ver; }
  bool     Def_by_none(void) { return (_def_by == DEF_BY_NONE); }
  bool     Def_by_phi(void) { return (_def_by == DEF_BY_PHI); }
  bool     Def_by_chi(void) { return (_def_by == DEF_BY_CHI); }
  bool     Def_by_stmt(void) { return (_def_by == DEF_BY_STMT); }
  HPHI_ID  Def_phi(void) {
    AIR_ASSERT(_def_by == DEF_BY_PHI);
    return _phi;
  }
  void Set_def_phi(HPHI_ID id) {
    _def_by = DEF_BY_PHI;
    _phi    = id;
  }

  HSTMT_ID Def_stmt(void) {
    AIR_ASSERT(_def_by == DEF_BY_STMT);
    return _defstmt;
  }
  void Set_def_stmt(HSTMT_ID id) {
    _def_by  = DEF_BY_STMT;
    _defstmt = id;
  }

  HCHI_ID Def_chi(void) {
    AIR_ASSERT(_def_by == DEF_BY_CHI);
    return _chi;
  }

  void Set_def_chi(HCHI_ID id) {
    _def_by = DEF_BY_CHI;
    _chi    = id;
  }

  std::string Name(HSSA_CONTAINER* cont) const;

  bool Match(VAR_HCR_DATA_PTR other) const;

  bool Match_lex(VAR_HCR_DATA_PTR other) const;

  air::base::STMT_PTR Emit_lhs(air::base::CONTAINER*  cont,
                               air::base::NODE_PTR    rhs,
                               const air::base::SPOS& spos);

  air::base::NODE_PTR Emit_rhs(air::base::CONTAINER*  cont,
                               const air::base::SPOS& spos);
  void                Print(HSSA_CONTAINER* hssa_cont, std::ostream& os,
                            uint32_t indent = 0) const;

private:
  VAR_KIND _var_kind;  // PREG or ADDR_DATUM
  uint32_t _var_id;    //!< ADDR_DATUM_ID or PREG_ID
  uint32_t _sub_idx;   //!< Field id or element index

  uint32_t   _ver;  //!< SSA version number
  SSA_VER_ID _ver_id;
  union {
    // A CR defined by HSTMT has mu_list, but no chi_node or phi_node.
    // A CR defined by either chi or phi has no mu_list
    HCHI_ID  _chi;      // the chi node that define this cr
    HPHI_ID  _phi;      // the phi node that define this cr
    HSTMT_ID _defstmt;  // statement that defines this var
  };
  VAR_DEF_BY _def_by : 3;  // var def by
};

class LDA_HCR_DATA : public HCR_DATA {
private:
  SSA_SYM_ID                _aux_id;    // the entry number in aux symbol table
  air::base::ADDR_DATUM_PTR _base_st;   // the base
  air::base::TYPE_ID        _ty;        // type pointer for LDA
  uint16_t                  _afieldid;  // field id of the LDA
};

enum CST_KIND { CSTKIND_INT, CSTKIND_ID, CSTKIND_ADDR };
class CST_HCR_DATA : public HCR_DATA {
public:
  CST_HCR_DATA(air::base::NODE_PTR node);

  CST_HCR_DATA(uint64_t val, air::base::TYPE_ID ty) : HCR_DATA(CK_CONST) {
    Set_opcode(air::base::OPCODE(air::core::LDC));
    Set_rtype(ty);
    _cst_kind  = CSTKIND_INT;
    _const_val = val;
  }

  CST_HCR_DATA(CST_HCR_DATA_PTR other)
      : HCR_DATA(other->Opcode(), CK_CONST, other->Rtype(), other->Dsctype(),
                 other->Attr(), other->Spos()) {
    _cst_kind  = other->Cst_kind();
    _const_val = other->_const_val;
  }

  uint32_t Hash_idx(void);
  CST_KIND Cst_kind(void) const { return _cst_kind; }

  bool Match(CST_HCR_DATA_PTR other) const;

  void Set_cst_kind(CST_KIND k) { _cst_kind = k; }
  void Set_value(uint64_t val) { _const_val = val; }
  void Set_value(air::base::CONSTANT_ID id) { _const_id = id; }

  uint64_t Cst_val() const {
    AIR_ASSERT(_cst_kind == CSTKIND_INT);
    return _const_val;
  }
  air::base::CONSTANT_ID Cst_id() const {
    AIR_ASSERT(_cst_kind == CSTKIND_ID);
    return _const_id;
  }

  air::base::NODE_PTR Emit(air::base::CONTAINER* cont);
  void                Print(HSSA_CONTAINER* hssa_cont, std::ostream& os,
                            uint32_t indent = 0) const;

private:
  CST_KIND _cst_kind;
  union {
    air::base::CONSTANT_ID _const_id;   // symbolic constant or constant
    uint64_t               _const_val;  // constant value
  };
};

class OP_HCR_DATA : public HCR_DATA {
public:
  OP_HCR_DATA(air::base::NODE_PTR node) : HCR_DATA(node, CK_OP) {
    _isop_flags      = ISOP_EMPTY;
    _propagatability = PROPAGATABLE;
    _kid_cnt         = node->Num_child();
    _max_depth       = 0;
  }

  OP_HCR_DATA(OP_HCR_DATA_PTR other)
      : HCR_DATA(other->Opcode(), CK_OP, other->Rtype(), other->Dsctype(),
                 other->Attr(), other->Spos()) {
    _isop_flags      = other->_isop_flags;
    _propagatability = other->_propagatability;
    _kid_cnt         = other->_kid_cnt;
    _max_depth       = other->_max_depth;
    for (uint32_t idx = 0; idx < Kid_cnt(); idx++) {
      _kids[idx] = other->Kid(idx);
    }
  }

  OP_HCR_DATA(air::base::OPCODE opcode, uint32_t kid_cnt,
              air::base::TYPE_ID rtype, air::base::TYPE_ID dsctype,
              air::base::SPOS spos)
      : HCR_DATA(opcode, CK_OP, rtype, dsctype, air::base::ATTR_ID(), spos) {
    _isop_flags      = ISOP_EMPTY;
    _propagatability = PROPAGATABLE;
    _kid_cnt         = kid_cnt;
    _max_depth       = 0;
  }

  static OP_HCR_DATA* Alloc(uint32_t kid_cnt) {
    return (OP_HCR_DATA*)malloc(OP_HCR_DATA::Size(kid_cnt));
  }

  static size_t Size(uint32_t kid_cnt) {
    return sizeof(OP_HCR_DATA) + kid_cnt * sizeof(HCR_ID);
  }
  void     Set_kid_cnt(uint32_t kid_cnt) { _kid_cnt = kid_cnt; }
  uint32_t Hash_idx(void);

  bool Match(OP_HCR_DATA_PTR other) const;

  bool Match_lex(OP_HCR_DATA_PTR other) const;

  static const OP_HCR_DATA& Cast_to_me(const HCR_DATA& data) {
    AIR_ASSERT(data.Kind() == CK_OP);
    return static_cast<const OP_HCR_DATA&>(data);
  }

  uint32_t Kid_cnt(void) const { return _kid_cnt; }

  HCR_ID Kid(uint32_t idx) const {
    AIR_ASSERT(idx < Kid_cnt());
    return _kids[idx];
  }

  void Set_kid(uint32_t kid_idx, HCR_ID id) {
    AIR_ASSERT(kid_idx < _kid_cnt);
    _kids[kid_idx] = id;
  }

  air::base::NODE_PTR Emit(air::base::CONTAINER* cont,
                           HSSA_CONTAINER*       hssa_cont);
  void                Print(HSSA_CONTAINER* hssa_cont, std::ostream& os,
                            uint32_t indent = 0) const;

private:
  ISOP_FLAG       _isop_flags : 22;
  PROPAGATABILITY _propagatability : 2;  // used during copy propagation
  int32_t         _kid_cnt : 14;         // number of kids
  uint8_t         _max_depth;  // used in estimating rehash cost (SSAPRE)
  HCR_ID          _kids[];     // op kids
};

class IVAR_HCR_DATA : public HCR_DATA {
private:
  int32_t  _num_of_min_max : 6;  // number of minmax, collectively
  int32_t  _unused : 10;         // unused
  int16_t  _ifieldid;            // field id
  MU_NODE* _mu_node;             // MU-list for this memory ref
  HSTMT_ID _defstmt;             // defining stmt for ILOD
  HCR*     _base[2];             // the base address expr, base[0]
  // IDTYPE  occ;                 // dynamically created, access base[1]
  // TY     *ilod_ty;             // dynamically created, access base[2]
  // HCR *base[3];            // dynamically created, give STORE's
  //  base address
  // TY     *ilod_base_ty;        // base type of load_addr_type, base[4]
  //  for MSTORE/MLOAD number of bytes (size)
};

class HCR {
  friend class HSSA_CONTAINER;

public:
  HCR(void) {}
  HCR(const HSSA_CONTAINER* cont, HCR_DATA_PTR data)
      : _cont(const_cast<HSSA_CONTAINER*>(cont)), _data(data) {}

  HSSA_CONTAINER*        Hssa_cont() const { return _cont; }
  air::base::FUNC_SCOPE* Func_scope() const;

  HCR_DATA_PTR Data() const { return _data; }
  CODEKIND     Kind() const { return _data->Kind(); }
  bool         Is_null() const { return _data.Id() == HCR_ID(); }
  HCR_ID       Id() const { return _data.Id(); }
  HCR_ID       Next_id() const { return _data->Next(); }
  void         Set_next(HCR_ID next) { _data->Set_next(next); }

  uint32_t Domain() const { return Opcode().Domain(); }

  uint32_t Operator() const { return Opcode().Operator(); }

  air::base::OPCODE Opcode() const { return _data->Opcode(); }

  void Set_rtype(air::base::TYPE_ID rtype) { _data->Set_rtype(rtype); }
  air::base::TYPE_ID        Rtype_id(void) const { return _data->Rtype(); }
  air::base::TYPE_PTR       Rtype(void) const;
  air::base::TYPE_ID        Dsctype_id(void) const { return _data->Dsctype(); }
  uint32_t                  Kid_cnt(void);
  int32_t                   Kid_idx(HCR_PTR kid);
  HCR_PTR                   Kid(uint32_t idx);
  air::base::SPOS           Spos() const { return _data->Spos(); }
  const air::base::ATTR_ID& Attr(void) const { return _data->Attr(); }
  DECLATR_ATTR_ACCESS_API(Attr(), (air::base::SCOPE_BASE*)Func_scope())

  void Set_flag(CR_FLAG flag) { _data->Set_flag(flag); }
  void Set_defphi(HPHI_PTR hphi);
  void Set_kid(uint32_t idx, HCR_ID id);
  void Set_kid(uint32_t idx, HCR_PTR id);

  HPHI_PTR Def_phi(void) const;

  void Init_const(air::base::NODE_PTR node);
  void Init_sym(air::base::NODE_PTR node);

  VAR_HCR_DATA_PTR Cast_to_var_cr(void) const {
    AIR_ASSERT(Kind() == CK_VAR);
    return air::base::Static_cast<VAR_HCR_DATA_PTR>(_data);
  }

  OP_HCR_DATA_PTR Cast_to_op_cr(void) const {
    AIR_ASSERT(Kind() == CK_OP);
    return air::base::Static_cast<OP_HCR_DATA_PTR>(_data);
  }

  CST_HCR_DATA_PTR Cast_to_cst_cr(void) const {
    AIR_ASSERT(Kind() == CK_CONST);
    return air::base::Static_cast<CST_HCR_DATA_PTR>(_data);
  }

  uint32_t Hash_idx() const;

  bool Match(HCR_PTR other, HSSA_CONTAINER* cont = NULL) const;

  //! Returns TRUE if 'this' is lexically identical to other
  bool Match_lex(HCR_PTR other) const;

  bool Replace_cr(HCR_ID cr, HCR_ID new_cr);

  bool Is_dominate(HSTMT_PTR sr);
  bool Is_same_e_ver(HCR_PTR cr);

  HSTMT_PTR Def_stmt(void);
  BB_ID     Def_bb(void);

  air::base::NODE_PTR Emit(air::base::CONTAINER* cont);
  static void         Print_opcode(air::base::OPCODE opcode, std::ostream& os,
                                   uint32_t indent = 0);
  void                Print(std::ostream& os, uint32_t indent = 0) const;
  void                Print() const;
  std::string         To_str() const;

private:
  HSSA_CONTAINER* _cont;
  HCR_DATA_PTR    _data;
};

}  // namespace opt
}  // namespace air
#endif