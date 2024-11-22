//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_SSA_ST_H
#define AIR_OPT_SSA_ST_H

#include "air/base/st.h"
#include "air/opt/ssa_decl.h"

//! @brief Define SSA symbol and version

namespace air {

namespace opt {

//! @brief SSA Symbol Table
//! SSA Symbol Table contains entities which can be renamed standalone:
//!   - Scalar ADDR_DATUM/PREG
//!   - Fields in Struct ADDR_DATUM/PREG
//!   - Elements in Array ADDR_DATUM

//! @brief SSA Symbol Kind
enum class SSA_SYM_KIND {
  UNKNOWN,     //!< Unknown kind to capture error
  PREG,        //!< For a PREG or PREG struct field
  ADDR_DATUM,  //!< For a ADDR_DADUM or ADDR_DATUM field or element
};

//! @brief SSA Symbol Attribute
class SSA_SYM_ATTR {
public:
  uint32_t _kind : 2;      //!< SSA symbol kind
  uint32_t _iv : 1;        //!< Symbol is DO_LOOP IV
  uint32_t _real_use : 1;  //!< Symbol is real used in IR
};

//! @brief SSA Symbol Data
class SSA_SYM_DATA {
  friend class SSA_CONTAINER;

public:
  bool Is_preg() const { return Kind() == SSA_SYM_KIND::PREG; }

  bool Is_addr_datum() const { return Kind() == SSA_SYM_KIND::ADDR_DATUM; }

  bool Is_iv() const { return _attr._iv; }
  bool Real_use() const { return _attr._real_use; }

  SSA_SYM_KIND Kind() const { return (SSA_SYM_KIND)_attr._kind; }
  SSA_SYM_ID   Parent_id() const { return _parent; }
  SSA_SYM_ID   Sibling_id() const { return _sibling; }
  SSA_SYM_ID   Child_id() const { return _child; }
  uint32_t     Var_id() const { return _var_id; }
  uint32_t     Index() const { return _sub_idx; }
  SSA_VER_ID   Zero_ver() const { return _zero_ver; }

  void Set_iv() { _attr._iv = true; }
  void Set_real_use() { _attr._real_use = true; }

  void Set_parent(SSA_SYM_ID id) { _parent = id; }
  void Set_sibling(SSA_SYM_ID id) { _sibling = id; }
  void Set_child(SSA_SYM_ID id) { _child = id; }

private:
  SSA_SYM_ATTR       _attr;      //!< Symbol attributes
  air::base::TYPE_ID _type;      //!< Symbol or field or elem type.
  SSA_SYM_ID         _parent;    //!< Parent symbol in struct or array
  SSA_SYM_ID         _sibling;   //!< Sibling symbol in same group
  SSA_SYM_ID         _child;     //!< first child symbol in sub group
  SSA_VER_ID         _zero_ver;  //!< Zero version
  uint32_t           _var_id;    //!< ADDR_DATUM_ID or PREG_ID
  uint32_t           _sub_idx;   //!< Field id or element index
};

//! @brief SSA Symbol
class SSA_SYM {
  friend class SSA_CONTAINER;

public:
  static constexpr uint32_t NO_INDEX = (uint32_t)-1;

  SSA_SYM_ID Id() const { return _data.Id(); }

  bool Is_iv() const { return _data->Is_iv(); }
  bool Real_use() const { return _data->Real_use(); }

  SSA_SYM_ID Parent_id() const { return _data->Parent_id(); }
  SSA_SYM_ID Sibling_id() const { return _data->Sibling_id(); }
  SSA_SYM_ID Child_id() const { return _data->Child_id(); }
  SSA_VER_ID Zero_ver_id() const { return _data->Zero_ver(); }

  void Set_iv() { _data->Set_iv(); }
  void Set_real_use() { _data->Set_real_use(); }

  void Set_parent(SSA_SYM_ID id) { _data->Set_parent(id); }
  void Set_sibling(SSA_SYM_ID id) { _data->Set_sibling(id); }
  void Set_child(SSA_SYM_ID id) { _data->Set_child(id); }

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

private:
  SSA_SYM(const SSA_CONTAINER* cont, SSA_SYM_DATA_PTR data)
      : _cont(const_cast<SSA_CONTAINER*>(cont)), _data(data) {}

  SSA_CONTAINER*   _cont;  //!< SSA container
  SSA_SYM_DATA_PTR _data;  //!< SSA_SYM_DATA
};

//! @brief SSA Version Table
//! SSA Version Table contains versions definition info

//! @brief VER Definition Kind
enum class VER_DEF_KIND {
  UNKNOWN,  //!< Unkonown define
  STMT,     //!< Defined by STMT
  PHI,      //!< Defined by PHI
  CHI,      //!< Defined by CHI
};

//! @brief SSA Version Attribute
class SSA_VER_ATTR {
public:
  uint32_t _def_kind : 2;  //!< Define kind
};

//! @brief SSA Version Data
class SSA_VER_DATA {
  friend class SSA_CONTAINER;

public:
  VER_DEF_KIND       Kind() const { return (VER_DEF_KIND)_attr._def_kind; }
  SSA_SYM_ID         Sym_id() const { return _sym; }
  uint32_t           Version() const { return _ver; }
  air::base::STMT_ID Def_stmt() const {
    AIR_ASSERT(Kind() == VER_DEF_KIND::STMT);
    return _def_stmt;
  }
  PHI_NODE_ID Def_phi() const {
    AIR_ASSERT(Kind() == VER_DEF_KIND::PHI);
    return _def_phi;
  }
  CHI_NODE_ID Def_chi() const {
    AIR_ASSERT(Kind() == VER_DEF_KIND::CHI);
    return _def_chi;
  }

  void Set_version(uint32_t v) { _ver = v; }

  void Set_def_stmt(air::base::STMT_ID stmt) {
    AIR_ASSERT(Kind() == VER_DEF_KIND::STMT);
    _def_stmt = stmt;
  }

  void Set_def_phi(PHI_NODE_ID phi) {
    AIR_ASSERT(Kind() == VER_DEF_KIND::PHI);
    _def_phi = phi;
  }

  void Set_def_chi(CHI_NODE_ID chi) {
    AIR_ASSERT(Kind() == VER_DEF_KIND::CHI);
    _def_chi = chi;
  }

private:
  SSA_VER_ATTR _attr;  //!< Version attributes
  SSA_SYM_ID   _sym;   //!< SSA symbol id
  uint32_t     _ver;   //!< SSA version number
  union {
    air::base::STMT_ID _def_stmt;  //!< Def stmt id
    PHI_NODE_ID        _def_phi;   //!< Def phi id
    CHI_NODE_ID        _def_chi;   //!< Def chi id
  };
};

//! @brief SSA Symbol
class SSA_VER {
  friend class SSA_CONTAINER;

public:
  static constexpr uint32_t NO_VER = (uint32_t)-1;

  SSA_VER_ID         Id() const { return _data.Id(); }
  VER_DEF_KIND       Kind() const { return _data->Kind(); }
  SSA_SYM_ID         Sym_id() const { return _data->Sym_id(); }
  uint32_t           Version() const { return _data->Version(); }
  air::base::STMT_ID Def_stmt_id() const { return _data->Def_stmt(); }
  PHI_NODE_ID        Def_phi_id() const { return _data->Def_phi(); }
  CHI_NODE_ID        Def_chi_id() const { return _data->Def_chi(); }

  void Set_version(uint32_t v) { _data->Set_version(v); }
  void Set_def_stmt(air::base::STMT_ID stmt) { _data->Set_def_stmt(stmt); }
  void Set_def_phi(PHI_NODE_ID phi) { _data->Set_def_phi(phi); }
  void Set_def_chi(CHI_NODE_ID chi) { _data->Set_def_chi(chi); }

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

private:
  SSA_VER(const SSA_CONTAINER* cont, SSA_VER_DATA_PTR data)
      : _cont(const_cast<SSA_CONTAINER*>(cont)), _data(data) {}

  SSA_CONTAINER*   _cont;  //!< SSA container
  SSA_VER_DATA_PTR _data;  //!< SSA_VER_DATA
};

}  // namespace opt

}  // namespace air

#endif  // AIR_OPT_SSA_ST_H
