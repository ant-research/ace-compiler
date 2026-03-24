//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_HSSA_MU_CHI_H
#define AIR_OPT_HSSA_MU_CHI_H

#include "air/opt/cfg_decl.h"
#include "air/opt/hssa_decl.h"
#include "air/opt/ssa_st.h"

//! @brief Define HMU, HCHI and HPHI
namespace air {
namespace opt {

//! @brief HSSA HMU
//! HSSA HMU for May-Use

//! @brief HSSA_HMU Attribute
class HMU_ATTR {
public:
  uint32_t _dead : 1;  //!< HMU is dead
};

//! @brief HMU_DATA
class HMU_DATA {
  friend class HCONTAINER;

public:
  void Init(void) {
    _attr._dead = false;
    _next       = HMU_ID();
    _opnd       = HEXPR_ID();
  }

  HMU_ID   Next(void) const { return _next; }
  HEXPR_ID Opnd(void) const { return _opnd; }

  void Set_next(HMU_ID next) { _next = next; }
  void Set_opnd(HEXPR_ID opnd) { _opnd = opnd; }

  bool Is_dead(void) const { return _attr._dead; }

private:
  HMU_ATTR _attr;  //!< HMU attributes
  HEXPR_ID _opnd;  //!< OPND expression ID
  HMU_ID   _next;  //!< Next HMU on same HMU_LIST
};

//! @brief HMU
class HMU {
  friend class HCONTAINER;
  friend class air::base::PTR_TO_CONST<HMU>;

public:
  HMU_ID   Id(void) const { return _data.Id(); }
  HMU_ID   Next_id(void) const { return _data->Next(); }
  HEXPR_ID Opnd_id(void) const { return _data->Opnd(); }

  bool Is_null(void) const { return _data.Is_null(); }
  bool Is_dead(void) const { return _data->Is_dead(); }

  void Set_next(HMU_ID next) { _data->Set_next(next); }
  void Set_opnd(HEXPR_ID opnd) { _data->Set_opnd(opnd); }

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print(void) const;
  std::string To_str(void) const;

private:
  HMU(void) : _cont(nullptr), _data() {}

  HMU(const HCONTAINER* cont, HMU_DATA_PTR data)
      : _cont(const_cast<HCONTAINER*>(cont)), _data(data) {}

  HCONTAINER*  _cont;  //!< HSSA container
  HMU_DATA_PTR _data;  //!< HMU_DATA
};

//! @brief HSSA HCHI
//! HSSA HCHI for May-Def

//! @brief HCHI Attribute
class HCHI_ATTR {
public:
  uint32_t _dead : 1;  //!< HCHI is dead
};

//! @brief HCHI_DATA
class HCHI_DATA {
  friend class HCONTAINER;

public:
  HCHI_DATA(HSTMT_ID stmt)
      : _next(HCHI_ID()), _res(HEXPR_ID()), _opnd(HEXPR_ID()), _stmt(stmt) {
    _attr._dead = false;
  }

  HCHI_ID  Next(void) const { return _next; }
  HEXPR_ID Result(void) const { return _res; }
  HEXPR_ID Opnd(void) const { return _opnd; }
  HSTMT_ID Stmt(void) const { return _stmt; }

  bool Is_dead(void) const { return _attr._dead; }

  void Set_next(HCHI_ID next) { _next = next; }
  void Set_result(HEXPR_ID res) { _res = res; }
  void Set_opnd(HEXPR_ID opnd) { _opnd = opnd; }
  void Set_stmt(HSTMT_ID stmt) { _stmt = stmt; }

private:
  HCHI_ATTR _attr;  //!< HCHI attributes
  HEXPR_ID  _res;   //!< HCHI result
  HEXPR_ID  _opnd;  //!< HCHI opnd
  HSTMT_ID  _stmt;  //!< Owner of the chi data
  HCHI_ID   _next;  //!< Next HCHI on same HCHI_LIST
};

//! @brief HCHI
class HCHI {
  friend class HCONTAINER;
  friend class air::base::PTR_TO_CONST<HCHI>;

public:
  HCHI(HCONTAINER* cont, HCHI_DATA_PTR data) : _cont(cont), _data(data) {}
  HCHI_ID  Id(void) const { return _data.Id(); }
  HCHI_ID  Next_id(void) const { return _data->Next(); }
  HEXPR_ID Result_id(void) const { return _data->Result(); }
  HEXPR_ID Opnd_id(void) const { return _data->Opnd(); }
  HSTMT_ID Stmt(void) const { return _data->Stmt(); }

  void Set_next(HCHI_ID next) { _data->Set_next(next); }
  void Set_result(HEXPR_ID res) { _data->Set_result(res); }
  void Set_opnd(HEXPR_ID res) { _data->Set_opnd(res); }

  bool Is_null(void) const { return _data.Is_null(); }
  bool Is_dead(void) const { return _data->Is_dead(); }

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print(void) const;
  std::string To_str(void) const;

private:
  HCHI(void) : _cont(nullptr), _data() {}

  HCHI(const HCONTAINER* cont, HCHI_DATA_PTR data)
      : _cont(const_cast<HCONTAINER*>(cont)), _data(data) {}

  HCONTAINER*   _cont;  //!< HSSA container
  HCHI_DATA_PTR _data;  //!< HCHI_DATA
};

//! @brief HSSA HPHI
//! HSSA HPHI

//! @brief HPHI Attribute
class HPHI_ATTR {
public:
  uint32_t _dead : 1;  //!< PHI is dead
};

//! @brief HPHI_DATA
class HPHI_DATA {
  friend class HCONTAINER;

public:
  HPHI_DATA(BB_ID bb, uint32_t num_opnd) {
    _attr._dead = false;
    _next       = HPHI_ID();
    _size       = num_opnd;
    _res        = HEXPR_ID();
    _bb         = bb;
    for (uint32_t i = 0; i < _size; ++i) {
      _opnd[i] = HEXPR_ID();
    }
  }

  BB_ID    Bb_id(void) const { return _bb; }
  HPHI_ID  Next(void) const { return _next; }
  uint32_t Size(void) const { return _size; }
  HEXPR_ID Result(void) const { return _res; }
  HEXPR_ID Opnd(uint32_t idx) const {
    AIR_ASSERT(idx < _size);
    return _opnd[idx];
  }

  void Set_next(HPHI_ID next) { _next = next; }
  void Set_result(HEXPR_ID res) { _res = res; }
  void Set_opnd(uint32_t idx, HEXPR_ID opnd) {
    AIR_ASSERT(idx < _size);
    _opnd[idx] = opnd;
  }

  bool Is_dead(void) const { return _attr._dead; }

private:
  uint32_t  _size;    //!< Number of phi operands
  HPHI_ATTR _attr;    //!< HPHI attributes
  BB_ID     _bb;      //!< Owning basic block ID
  HPHI_ID   _next;    //!< Next HPHI on same HPHI_LIST
  HEXPR_ID  _res;     //!< phi result
  HEXPR_ID  _opnd[];  //!< phi operands
};

//! @brief HPHI
class HPHI {
  friend class HCONTAINER;
  friend class air::base::PTR_TO_CONST<HPHI>;

public:
  HPHI_ID   Id(void) const { return _data.Id(); }
  HPHI_ID   Next_id(void) const { return _data->Next(); }
  uint32_t  Size(void) const { return _data->Size(); }
  HEXPR_ID  Result_id(void) const { return _data->Result(); }
  HEXPR_PTR Result(void) const;
  BB_ID     Bb_id(void) const { return _data->Bb_id(); }
  BB_PTR    Bb(CFG* cfg) const;
  HEXPR_ID  Opnd_id(uint32_t idx) const { return _data->Opnd(idx); }
  HEXPR_PTR Opnd(uint32_t idx) const;
  int32_t   Opnd_idx(HEXPR_PTR opnd) const;
  bool      Is_null(void) const { return _data.Is_null(); }
  bool      Is_dead(void) const { return _data->Is_dead(); }

  void Set_next(HPHI_ID next) { _data->Set_next(next); }
  void Set_result(HEXPR_ID res) { _data->Set_result(res); }
  void Set_opnd(uint32_t idx, HEXPR_ID opnd) { _data->Set_opnd(idx, opnd); }

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print(void) const;
  std::string To_str(void) const;

private:
  HPHI(void) : _cont(nullptr), _data() {}
  HPHI(const HCONTAINER* cont, HPHI_DATA_PTR data)
      : _cont(const_cast<HCONTAINER*>(cont)), _data(data) {}

  HCONTAINER*   _cont;  //!< HSSA container
  HPHI_DATA_PTR _data;  //!< HPHI_DATA
};

}  // namespace opt

}  // namespace air

#endif  // AIR_OPT_SSA_NODE_H
