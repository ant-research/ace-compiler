//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_HSSA_HMU_HCHI_H
#define AIR_OPT_HSSA_HMU_HCHI_H

#include "air/opt/cfg_decl.h"
#include "air/opt/hssa_decl.h"
#include "air/opt/ssa_st.h"
//! @brief Define SSA HMU, HCHI and HPHI

namespace air {

namespace opt {

class CFG;
//! @brief HSSA HMU
//! HSSA HMU for May-Use

//! @brief HSSA_HMU Attribute
class HMU_ATTR {
public:
  uint32_t _dead : 1;  //!< HMU is dead
};

//! @brief HMU_DATA
class HMU_DATA {
  friend class HSSA_CONTAINER;

public:
  void Init() {
    _attr._dead = false;
    _next       = HMU_ID();
    _opnd       = HCR_ID();
  }

  HMU_ID Next() const { return _next; }
  HCR_ID Opnd() const { return _opnd; }

  bool Is_dead() const { return _attr._dead; }

  void Set_next(HMU_ID next) { _next = next; }
  void Set_opnd(HCR_ID opnd) { _opnd = opnd; }

private:
  HMU_ATTR _attr;  //!< HMU attributes
  HMU_ID   _next;  //!< Next HMU on same HMU_LIST
  HCR_ID   _opnd;  //!< OPND CR ID
};

//! @brief HMU
class HMU {
  friend class HSSA_CONTAINER;
  friend class air::base::PTR_TO_CONST<HMU>;

public:
  HMU_ID Id() const { return _data.Id(); }
  HMU_ID Next_id() const { return _data->Next(); }
  HCR_ID Opnd_id() const { return _data->Opnd(); }

  bool Is_null() const { return _data.Is_null(); }
  bool Is_dead() const { return _data->Is_dead(); }

  void Set_next(HMU_ID next) { _data->Set_next(next); }
  void Set_opnd(HCR_ID opnd) { _data->Set_opnd(opnd); }

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

private:
  HMU() : _cont(nullptr), _data() {}

  HMU(const HSSA_CONTAINER* cont, HMU_DATA_PTR data)
      : _cont(const_cast<HSSA_CONTAINER*>(cont)), _data(data) {}

  HSSA_CONTAINER* _cont;  //!< HSSA container
  HMU_DATA_PTR    _data;  //!< HMU_DATA
};

//! @brief SSA HCHI
//! SSA HCHI for May-Def

//! @brief HCHI Attribute
class HCHI_ATTR {
public:
  uint32_t _dead : 1;  //!< HCHI is dead
};

//! @brief HCHI_DATA
class HCHI_DATA {
  friend class SSA_CONTAINER;

public:
  HCHI_DATA(HSTMT_ID stmt)
      : _next(HCHI_ID()), _res(HCR_ID()), _opnd(HCR_ID()), _stmt(stmt) {
    _attr._dead = false;
  }

  HCHI_ID  Next() const { return _next; }
  HCR_ID   Result() const { return _res; }
  HCR_ID   Opnd() const { return _opnd; }
  HSTMT_ID Stmt() const { return _stmt; }

  bool Is_dead() const { return _attr._dead; }

  void Set_next(HCHI_ID next) { _next = next; }
  void Set_result(HCR_ID res) { _res = res; }
  void Set_opnd(HCR_ID opnd) { _opnd = opnd; }
  void Set_stmt(HSTMT_ID stmt) { _stmt = stmt; }

private:
  HCHI_ATTR _attr;  //!< HCHI attributes
  HCHI_ID   _next;  //!< Next HCHI on same HCHI_LIST
  HCR_ID    _res;   //!< HCHI result
  HCR_ID    _opnd;  //!< HCHI opnd
  HSTMT_ID  _stmt;  // !< Owner of the chi data
};

//! @brief HCHI
class HCHI {
  friend class HSSA_CONTAINER;
  friend class air::base::PTR_TO_CONST<HCHI>;

public:
  HCHI(HSSA_CONTAINER* cont, HCHI_DATA_PTR data) : _cont(cont), _data(data) {}
  HCHI_ID  Id() const { return _data.Id(); }
  HCHI_ID  Next_id() const { return _data->Next(); }
  HCR_ID   Result_id() const { return _data->Result(); }
  HCR_ID   Opnd_id() const { return _data->Opnd(); }
  HSTMT_ID Stmt() const { return _data->Stmt(); }

  bool Is_null() const { return _data.Is_null(); }
  bool Is_dead() const { return _data->Is_dead(); }

  void Set_next(HCHI_ID next) { _data->Set_next(next); }
  void Set_result(HCR_ID res) { _data->Set_result(res); }
  void Set_opnd(HCR_ID res) { _data->Set_opnd(res); }

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

private:
  HCHI() : _cont(nullptr), _data() {}

  HCHI(const HSSA_CONTAINER* cont, HCHI_DATA_PTR data)
      : _cont(const_cast<HSSA_CONTAINER*>(cont)), _data(data) {}

  HSSA_CONTAINER* _cont;  //!< SSA container
  HCHI_DATA_PTR   _data;  //!< HCHI_DATA
};

//! @brief SSA HPHI
//! SSA HPHI

//! @brief HPHI Attribute
class HPHI_ATTR {
public:
  uint32_t _dead : 1;  //!< PHI is dead
};

//! @brief HPHI_DATA
class HPHI_DATA {
  friend class SSA_CONTAINER;

public:
  HPHI_DATA(BB_ID bb, uint32_t num_opnd) {
    _attr._dead = false;
    _next       = HPHI_ID();
    _size       = num_opnd;
    _res        = HCR_ID();
    _bb         = bb;
    for (uint32_t i = 0; i < _size; ++i) {
      _opnd[i] = HCR_ID();
    }
  }

  BB_ID    Bb_id() const { return _bb; }
  HPHI_ID  Next() const { return _next; }
  uint32_t Size() const { return _size; }
  HCR_ID   Result() const { return _res; }
  HCR_ID   Opnd(uint32_t idx) {
    AIR_ASSERT(idx < _size);
    return _opnd[idx];
  }

  bool Is_dead() const { return _attr._dead; }

  void Set_next(HPHI_ID next) { _next = next; }
  void Set_result(HCR_ID res) { _res = res; }
  void Set_opnd(uint32_t idx, HCR_ID opnd) {
    AIR_ASSERT(idx < _size);
    _opnd[idx] = opnd;
  }

private:
  HPHI_ATTR _attr;  //!< HPHI attributes
  BB_ID     _bb;
  HPHI_ID   _next;    //!< Next HPHI on same HPHI_LIST
  uint32_t  _size;    //!< Number of phi operands
  HCR_ID    _res;     //!< phi result
  HCR_ID    _opnd[];  //!< phi operands
};

//! @brief HPHI
class HPHI {
  friend class HSSA_CONTAINER;
  friend class air::base::PTR_TO_CONST<HPHI>;

public:
  HPHI_ID  Id() const { return _data.Id(); }
  HPHI_ID  Next_id() const { return _data->Next(); }
  uint32_t Size() const { return _data->Size(); }
  HCR_ID   Result_id() const { return _data->Result(); }
  HCR_PTR  Result() const;
  BB_ID    Bb_id() const { return _data->Bb_id(); }
  BB_PTR   Bb(CFG* cfg) const;
  HCR_ID   Opnd_id(uint32_t idx) const { return _data->Opnd(idx); }
  HCR_PTR  Opnd(uint32_t idx) const;
  int32_t  Opnd_idx(HCR_PTR opnd) const;
  bool     Is_null() const { return _data.Is_null(); }
  bool     Is_dead() const { return _data->Is_dead(); }

  void Set_next(HPHI_ID next) { _data->Set_next(next); }
  void Set_result(HCR_ID res) { _data->Set_result(res); }
  void Set_opnd(uint32_t idx, HCR_ID opnd) { _data->Set_opnd(idx, opnd); }

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

private:
  HPHI() : _cont(nullptr), _data() {}
  HPHI(const HSSA_CONTAINER* cont, HPHI_DATA_PTR data)
      : _cont(const_cast<HSSA_CONTAINER*>(cont)), _data(data) {}

  // TODO: add cfg
  HSSA_CONTAINER* _cont;  //!< HSSA container
  HPHI_DATA_PTR   _data;  //!< HPHI_DATA
};

}  // namespace opt

}  // namespace air

#endif  // AIR_OPT_SSA_NODE_H
