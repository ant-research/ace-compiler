//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_SSA_NODE_H
#define AIR_OPT_SSA_NODE_H

#include "air/opt/ssa_st.h"

//! @brief Define SSA MU_NODE, CHI_NODE and PHI_NODE

namespace air {

namespace opt {

//! @brief SSA MU_NODE
//! SSA MU_NODE for May-Use

//! @brief MU_NODE Attribute
class MU_NODE_ATTR {
public:
  uint32_t _dead : 1;  //!< MU_NODE is dead
};

//! @brief MU_NODE_DATA
class MU_NODE_DATA {
  friend class SSA_CONTAINER;

public:
  MU_NODE_ID Next(void) const { return _next; }
  SSA_SYM_ID Sym(void) const { return _sym; }
  SSA_VER_ID Opnd(void) const { return _opnd; }

  bool Is_dead(void) const { return _attr._dead; }

  void Set_next(MU_NODE_ID next) { _next = next; }
  void Set_opnd(SSA_VER_ID opnd) { _opnd = opnd; }

private:
  MU_NODE_ATTR _attr;  //!< MU_NODE attributes
  MU_NODE_ID   _next;  //!< Next MU_NODE on same MU_LIST
  SSA_SYM_ID   _sym;   //!< SSA symbol
  SSA_VER_ID   _opnd;  //!< MU_NODE operand
};

//! @brief MU_NODE
class MU_NODE {
  friend class SSA_CONTAINER;
  friend class air::base::PTR_TO_CONST<MU_NODE>;

public:
  MU_NODE_ID  Id(void) const { return _data.Id(); }
  MU_NODE_ID  Next_id(void) const { return _data->Next(); }
  MU_NODE_PTR Next(void) const;
  SSA_SYM_ID  Sym_id(void) const { return _data->Sym(); }
  SSA_SYM_PTR Sym(void) const;
  SSA_VER_ID  Opnd_id(void) const { return _data->Opnd(); }
  SSA_VER_PTR Opnd(void) const;

  bool Is_null(void) const { return _data.Is_null(); }
  bool Is_dead(void) const { return _data->Is_dead(); }

  void Set_next(MU_NODE_ID next) { _data->Set_next(next); }
  void Set_opnd(SSA_VER_ID res) { _data->Set_opnd(res); }

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

private:
  MU_NODE() : _cont(nullptr), _data() {}

  MU_NODE(const SSA_CONTAINER* cont, MU_NODE_DATA_PTR data)
      : _cont(const_cast<SSA_CONTAINER*>(cont)), _data(data) {}

  SSA_CONTAINER* Ssa_cont() const { return _cont; }

  SSA_CONTAINER*   _cont;  //!< SSA container
  MU_NODE_DATA_PTR _data;  //!< MU_NODE_DATA
};

//! @brief SSA CHI_NODE
//! SSA CHI_NODE for May-Def

//! @brief CHI_NODE Attribute
class CHI_NODE_ATTR {
public:
  uint32_t _dead : 1;  //!< CHI_NODE is dead
};

//! @brief CHI_NODE_DATA
class CHI_NODE_DATA {
  friend class SSA_CONTAINER;

public:
  CHI_NODE_ID   Next(void) const { return _next; }
  SSA_SYM_ID    Sym(void) const { return _sym; }
  SSA_VER_ID    Result(void) const { return _res; }
  SSA_VER_ID    Opnd(void) const { return _opnd; }
  base::STMT_ID Def_stmt(void) const {
    AIR_ASSERT_MSG(_def_stmt != base::STMT_ID(), "Use before set define stmt");
    return _def_stmt;
  }

  bool Is_dead(void) const { return _attr._dead; }

  void Set_next(CHI_NODE_ID next) { _next = next; }
  void Set_result(SSA_VER_ID res) { _res = res; }
  void Set_opnd(SSA_VER_ID opnd) { _opnd = opnd; }
  void Set_def_stmt(base::STMT_ID def_stmt) {
    AIR_ASSERT_MSG(def_stmt != base::STMT_ID(), "Set def_stmt with invalid ID");
    _def_stmt = def_stmt;
  }

private:
  CHI_NODE_ATTR _attr;      //!< CHI_NODE attributes
  CHI_NODE_ID   _next;      //!< Next CHI_NODE on same CHI_LIST
  SSA_SYM_ID    _sym;       //!< SSA symbol
  SSA_VER_ID    _res;       //!< CHI_NODE result
  SSA_VER_ID    _opnd;      //!< CHI_NODE operand
  base::STMT_ID _def_stmt;  //!< Define stmt
};

//! @brief CHI_NODE
class CHI_NODE {
  friend class SSA_CONTAINER;
  friend class air::base::PTR_TO_CONST<CHI_NODE>;

public:
  CHI_NODE_ID   Id(void) const { return _data.Id(); }
  CHI_NODE_ID   Next_id(void) const { return _data->Next(); }
  CHI_NODE_PTR  Next(void) const;
  SSA_SYM_ID    Sym_id(void) const { return _data->Sym(); }
  SSA_SYM_PTR   Sym(void) const;
  SSA_VER_ID    Result_id(void) const { return _data->Result(); }
  SSA_VER_PTR   Result(void) const;
  SSA_VER_ID    Opnd_id(void) const { return _data->Opnd(); }
  SSA_VER_PTR   Opnd(void) const;
  base::STMT_ID Def_stmt_id(void) const {
    base::STMT_ID def_stmt = _data->Def_stmt();
    AIR_ASSERT_MSG(def_stmt != base::STMT_ID(), "Use before set define stmt");
    return def_stmt;
  }
  base::STMT_PTR Def_stmt(void) const;

  bool Is_null(void) const { return _data.Is_null(); }
  bool Is_dead(void) const { return _data->Is_dead(); }

  void Set_next(CHI_NODE_ID next) { _data->Set_next(next); }
  void Set_result(SSA_VER_ID res) { _data->Set_result(res); }
  void Set_opnd(SSA_VER_ID opnd) { _data->Set_opnd(opnd); }
  void Set_def_stmt(base::STMT_ID def_stmt) { _data->Set_def_stmt(def_stmt); }

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

private:
  CHI_NODE() : _cont(nullptr), _data() {}

  CHI_NODE(const SSA_CONTAINER* cont, CHI_NODE_DATA_PTR data)
      : _cont(const_cast<SSA_CONTAINER*>(cont)), _data(data) {}

  SSA_CONTAINER* Ssa_cont() const { return _cont; }

  SSA_CONTAINER*    _cont;  //!< SSA container
  CHI_NODE_DATA_PTR _data;  //!< CHI_NODE_DATA
};

//! @brief SSA PHI_NODE
//! SSA PHI_NODE

//! @brief PHI_NODE Attribute
class PHI_NODE_ATTR {
public:
  uint32_t _dead : 1;  //!< PHI is dead
};

//! @brief PHI_NODE_DATA
class PHI_NODE_DATA {
  friend class SSA_CONTAINER;

public:
  PHI_NODE_ID Next(void) const { return _next; }
  uint32_t    Size(void) const { return _size; }
  SSA_SYM_ID  Sym(void) const { return _sym; }
  SSA_VER_ID  Result(void) const { return _res; }
  SSA_VER_ID  Opnd(uint32_t idx) {
    AIR_ASSERT(idx < _size);
    return _opnd[idx];
  }
  base::STMT_ID Def_stmt(void) const {
    AIR_ASSERT_MSG(_def_stmt != base::STMT_ID(), "Use before set define stmt");
    return _def_stmt;
  }

  bool Is_dead(void) const { return _attr._dead; }

  void Set_next(PHI_NODE_ID next) { _next = next; }
  void Set_result(SSA_VER_ID res) { _res = res; }
  void Set_opnd(uint32_t idx, SSA_VER_ID opnd) {
    AIR_ASSERT(idx < _size);
    _opnd[idx] = opnd;
  }
  void Set_def_stmt(base::STMT_ID def_stmt) {
    AIR_ASSERT_MSG(def_stmt != base::STMT_ID(), "Set def_stmt with invalid ID");
    _def_stmt = def_stmt;
  }

private:
  PHI_NODE_ATTR _attr;      //!< PHI_NODE attributes
  PHI_NODE_ID   _next;      //!< Next PHI_NODE on same PHI_LIST
  uint32_t      _size;      //!< Number of phi operands
  SSA_SYM_ID    _sym;       //!< SSA symbol
  base::STMT_ID _def_stmt;  //!< Define stmt
  SSA_VER_ID    _res;       //!< phi result
  SSA_VER_ID    _opnd[];    //!< phi operands
};

//! @brief PHI_NODE
class PHI_NODE {
  friend class SSA_CONTAINER;
  friend class air::base::PTR_TO_CONST<PHI_NODE>;

public:
  PHI_NODE_ID   Id(void) const { return _data.Id(); }
  PHI_NODE_ID   Next_id(void) const { return _data->Next(); }
  PHI_NODE_PTR  Next(void) const;
  uint32_t      Size(void) const { return _data->Size(); }
  SSA_SYM_ID    Sym_id(void) const { return _data->Sym(); }
  SSA_SYM_PTR   Sym(void) const;
  SSA_VER_ID    Result_id(void) const { return _data->Result(); }
  SSA_VER_PTR   Result(void) const;
  SSA_VER_ID    Opnd_id(uint32_t idx) const { return _data->Opnd(idx); }
  SSA_VER_PTR   Opnd(uint32_t idx) const;
  base::STMT_ID Def_stmt_id(void) const {
    base::STMT_ID def_stmt = _data->Def_stmt();
    AIR_ASSERT_MSG(def_stmt != base::STMT_ID(), "Use before set define stmt");
    return def_stmt;
  }
  base::STMT_PTR Def_stmt(void) const;

  bool Is_null(void) const { return _data.Is_null(); }
  bool Is_dead(void) const { return _data->Is_dead(); }

  void Set_next(PHI_NODE_ID next) { _data->Set_next(next); }
  void Set_result(SSA_VER_ID res) { _data->Set_result(res); }
  void Set_opnd(uint32_t idx, SSA_VER_ID opnd) { _data->Set_opnd(idx, opnd); }
  void Set_def_stmt(base::STMT_ID def_stmt) { _data->Set_def_stmt(def_stmt); }
  void Print(std::ostream& os, uint32_t indent = 0) const;
  void Print() const;
  std::string To_str() const;

private:
  PHI_NODE() : _cont(nullptr), _data() {}

  PHI_NODE(const SSA_CONTAINER* cont, PHI_NODE_DATA_PTR data)
      : _cont(const_cast<SSA_CONTAINER*>(cont)), _data(data) {}

  SSA_CONTAINER* Ssa_cont() const { return _cont; }

  SSA_CONTAINER*    _cont;  //!< SSA container
  PHI_NODE_DATA_PTR _data;  //!< PHI_NODE_DATA
};

}  // namespace opt

}  // namespace air

#endif  // AIR_OPT_SSA_NODE_H
