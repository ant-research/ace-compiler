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
  MU_NODE_ID Next() const { return _next; }
  SSA_SYM_ID Sym() const { return _sym; }
  SSA_VER_ID Opnd() const { return _opnd; }

  bool Is_dead() const { return _attr._dead; }

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
  MU_NODE_ID Id() const { return _data.Id(); }
  MU_NODE_ID Next_id() const { return _data->Next(); }
  SSA_SYM_ID Sym_id() const { return _data->Sym(); }
  SSA_VER_ID Opnd_id() const { return _data->Opnd(); }

  bool Is_null() const { return _data.Is_null(); }
  bool Is_dead() const { return _data->Is_dead(); }

  void Set_next(MU_NODE_ID next) { _data->Set_next(next); }
  void Set_opnd(SSA_VER_ID res) { _data->Set_opnd(res); }

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

private:
  MU_NODE() : _cont(nullptr), _data() {}

  MU_NODE(const SSA_CONTAINER* cont, MU_NODE_DATA_PTR data)
      : _cont(const_cast<SSA_CONTAINER*>(cont)), _data(data) {}

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
  CHI_NODE_ID Next() const { return _next; }
  SSA_SYM_ID  Sym() const { return _sym; }
  SSA_VER_ID  Result() const { return _res; }
  SSA_VER_ID  Opnd() const { return _opnd; }

  bool Is_dead() const { return _attr._dead; }

  void Set_next(CHI_NODE_ID next) { _next = next; }
  void Set_result(SSA_VER_ID res) { _res = res; }
  void Set_opnd(SSA_VER_ID opnd) { _opnd = opnd; }

private:
  CHI_NODE_ATTR _attr;  //!< CHI_NODE attributes
  CHI_NODE_ID   _next;  //!< Next CHI_NODE on same CHI_LIST
  SSA_SYM_ID    _sym;   //!< SSA symbol
  SSA_VER_ID    _res;   //!< MU_NODE operand
  SSA_VER_ID    _opnd;  //!< MU_NODE operand
};

//! @brief CHI_NODE
class CHI_NODE {
  friend class SSA_CONTAINER;
  friend class air::base::PTR_TO_CONST<CHI_NODE>;

public:
  CHI_NODE_ID Id() const { return _data.Id(); }
  CHI_NODE_ID Next_id() const { return _data->Next(); }
  SSA_SYM_ID  Sym_id() const { return _data->Sym(); }
  SSA_VER_ID  Result_id() const { return _data->Result(); }
  SSA_VER_ID  Opnd_id() const { return _data->Opnd(); }

  bool Is_null() const { return _data.Is_null(); }
  bool Is_dead() const { return _data->Is_dead(); }

  void Set_next(CHI_NODE_ID next) { _data->Set_next(next); }
  void Set_result(SSA_VER_ID res) { _data->Set_result(res); }
  void Set_opnd(SSA_VER_ID res) { _data->Set_opnd(res); }

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

private:
  CHI_NODE() : _cont(nullptr), _data() {}

  CHI_NODE(const SSA_CONTAINER* cont, CHI_NODE_DATA_PTR data)
      : _cont(const_cast<SSA_CONTAINER*>(cont)), _data(data) {}

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
  PHI_NODE_ID Next() const { return _next; }
  uint32_t    Size() const { return _size; }
  SSA_SYM_ID  Sym() const { return _sym; }
  SSA_VER_ID  Result() const { return _res; }
  SSA_VER_ID  Opnd(uint32_t idx) {
    AIR_ASSERT(idx < _size);
    return _opnd[idx];
  }

  bool Is_dead() const { return _attr._dead; }

  void Set_next(PHI_NODE_ID next) { _next = next; }
  void Set_result(SSA_VER_ID res) { _res = res; }
  void Set_opnd(uint32_t idx, SSA_VER_ID opnd) {
    AIR_ASSERT(idx < _size);
    _opnd[idx] = opnd;
  }

private:
  PHI_NODE_ATTR _attr;    //!< PHI_NODE attributes
  PHI_NODE_ID   _next;    //!< Next PHI_NODE on same PHI_LIST
  uint32_t      _size;    //!< Number of phi operands
  SSA_SYM_ID    _sym;     //!< SSA symbol
  SSA_VER_ID    _res;     //!< phi result
  SSA_VER_ID    _opnd[];  //!< phi operands
};

//! @brief PHI_NODE
class PHI_NODE {
  friend class SSA_CONTAINER;
  friend class air::base::PTR_TO_CONST<PHI_NODE>;

public:
  PHI_NODE_ID Id() const { return _data.Id(); }
  PHI_NODE_ID Next_id() const { return _data->Next(); }
  uint32_t    Size() const { return _data->Size(); }
  SSA_SYM_ID  Sym_id() const { return _data->Sym(); }
  SSA_VER_ID  Result_id() const { return _data->Result(); }
  SSA_VER_ID  Opnd_id(uint32_t idx) const { return _data->Opnd(idx); }

  bool Is_null() const { return _data.Is_null(); }
  bool Is_dead() const { return _data->Is_dead(); }

  void Set_next(PHI_NODE_ID next) { _data->Set_next(next); }
  void Set_result(SSA_VER_ID res) { _data->Set_result(res); }
  void Set_opnd(uint32_t idx, SSA_VER_ID opnd) { _data->Set_opnd(idx, opnd); }

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

private:
  PHI_NODE() : _cont(nullptr), _data() {}

  PHI_NODE(const SSA_CONTAINER* cont, PHI_NODE_DATA_PTR data)
      : _cont(const_cast<SSA_CONTAINER*>(cont)), _data(data) {}

  SSA_CONTAINER*    _cont;  //!< SSA container
  PHI_NODE_DATA_PTR _data;  //!< PHI_NODE_DATA
};

}  // namespace opt

}  // namespace air

#endif  // AIR_OPT_SSA_NODE_H
