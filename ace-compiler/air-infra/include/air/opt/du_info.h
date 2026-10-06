//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_DU_INFO_H
#define AIR_OPT_DU_INFO_H
#include "air/opt/hssa_container.h"

namespace air {
namespace opt {

enum VAR_USE_BY {
  USE_BY_NONE,
  USE_BY_CHI,
  USE_BY_PHI,
  USE_BY_STMT,
};

class USE_INFO {
public:
  USE_INFO() : _use_by(USE_BY_NONE) {}
  USE_INFO(HSTMT_ID id) {
    _use_by = USE_BY_STMT;
    _stmt   = id;
  }
  USE_INFO(HCHI_ID id) {
    _use_by = USE_BY_CHI;
    _chi    = id;
  }
  USE_INFO(HPHI_ID id) {
    _use_by = USE_BY_PHI;
    _phi    = id;
  }

  bool Is_stmt(void) const { return _use_by == USE_BY_STMT; }
  bool Is_phi(void) const { return _use_by == USE_BY_PHI; }
  bool Is_chi(void) const { return _use_by == USE_BY_CHI; }

  HSTMT_PTR Stmt(HCONTAINER* cont) {
    AIR_ASSERT(Is_stmt());
    return cont->Stmt_ptr(_stmt);
  }
  HCHI_PTR Chi(HCONTAINER* cont) {
    AIR_ASSERT(Is_chi());
    return cont->Chi_ptr(_chi);
  }
  HPHI_PTR Phi(HCONTAINER* cont) {
    AIR_ASSERT(Is_phi());
    return cont->Phi_ptr(_phi);
  }

private:
  VAR_USE_BY _use_by;
  union {
    HCHI_ID  _chi;
    HPHI_ID  _phi;
    HSTMT_ID _stmt;
  };
};

typedef std::vector<USE_INFO> USE_LIST;

class DU_INFO {
public:
  void Add_use(HEXPR_PTR expr, HSTMT_PTR stmt) {
    CMPLR_ASSERT(false, "not yet implemented");
  }
  void Add_use(HEXPR_PTR expr, HCHI_PTR chi) {
    CMPLR_ASSERT(false, "not yet implemented");
  }
  void Add_use(HEXPR_PTR expr, HPHI_PTR phi) {
    CMPLR_ASSERT(false, "not yet implemented");
  }
  void Remove_use(HEXPR_PTR expr, HSTMT_PTR stmt) {
    CMPLR_ASSERT(false, "not yet implemented");
  }
  void Remove_use(HEXPR_PTR expr, HPHI_PTR phi) {
    CMPLR_ASSERT(false, "not yet implemented");
  }
  USE_LIST* Uses(HEXPR_PTR expr) {
    CMPLR_ASSERT(false, "not yet implemented");
    return nullptr;
  }

private:
  std::map<uint32_t, USE_LIST> _use_map;
};

}  // namespace opt
}  // namespace air
#endif
