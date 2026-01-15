//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_HSSA_DECL_H
#define AIR_OPT_HSSA_DECL_H

#include "air/base/id_wrapper.h"
#include "air/base/ptr_wrapper.h"

namespace air {

namespace opt {

//! @brief HSSA Data Structure Declarations

class HCONTAINER;

class OP_DATA;
class VAR_DATA;
class CST_DATA;
class HEXPR_DATA;
class HEXPR;

class ASSIGN_DATA;
class NARY_DATA;
class CALL_DATA;
class HSTMT_DATA;
class HSTMT;

class HMU;
class HMU_DATA;
class HCHI;
class HCHI_DATA;
class HPHI;
class HPHI_DATA;

class CFG;
template <typename ID_TYPE, typename PTR_TYPE, typename CONT_TYPE>
class NODE_LIST;

typedef air::base::ID<HEXPR_DATA> HEXPR_ID;
typedef air::base::ID<HSTMT_DATA> HSTMT_ID;
typedef air::base::ID<HMU_DATA>   HMU_ID;
typedef air::base::ID<HCHI_DATA>  HCHI_ID;
typedef air::base::ID<HPHI_DATA>  HPHI_ID;

typedef air::base::PTR_FROM_DATA<HEXPR_DATA> HEXPR_DATA_PTR;
typedef air::base::PTR_FROM_DATA<VAR_DATA>   VAR_DATA_PTR;
typedef air::base::PTR_FROM_DATA<CST_DATA>   CST_DATA_PTR;
typedef air::base::PTR_FROM_DATA<OP_DATA>    OP_DATA_PTR;

typedef air::base::PTR_FROM_DATA<HSTMT_DATA>  HSTMT_DATA_PTR;
typedef air::base::PTR_FROM_DATA<ASSIGN_DATA> ASSIGN_DATA_PTR;
typedef air::base::PTR_FROM_DATA<NARY_DATA>   NARY_DATA_PTR;
typedef air::base::PTR_FROM_DATA<CALL_DATA>   CALL_DATA_PTR;

typedef air::base::PTR_FROM_DATA<HMU_DATA>  HMU_DATA_PTR;
typedef air::base::PTR_FROM_DATA<HCHI_DATA> HCHI_DATA_PTR;
typedef air::base::PTR_FROM_DATA<HPHI_DATA> HPHI_DATA_PTR;

typedef air::base::PTR<HEXPR> HEXPR_PTR;
typedef air::base::PTR<HSTMT> HSTMT_PTR;
typedef air::base::PTR<HMU>   HMU_PTR;
typedef air::base::PTR<HCHI>  HCHI_PTR;
typedef air::base::PTR<HPHI>  HPHI_PTR;

typedef air::base::PTR_TO_CONST<HEXPR> CONST_HEXPR_PTR;
typedef air::base::PTR_TO_CONST<HMU>   CONST_HMU_PTR;
typedef air::base::PTR_TO_CONST<HCHI>  CONST_HCHI_PTR;
typedef air::base::PTR_TO_CONST<HPHI>  CONST_HPHI_PTR;

typedef NODE_LIST<HSTMT_ID, HSTMT_PTR, HCONTAINER> HSTMT_LIST;
typedef NODE_LIST<HEXPR_ID, HEXPR_PTR, HCONTAINER> HEXPR_LIST;
typedef NODE_LIST<HPHI_ID, HPHI_PTR, HCONTAINER>   HPHI_LIST;
typedef NODE_LIST<HMU_ID, HMU_PTR, HCONTAINER>     HMU_LIST;
typedef NODE_LIST<HCHI_ID, HCHI_PTR, HCONTAINER>   HCHI_LIST;

typedef std::pair<HEXPR_PTR, HSTMT_PTR> NODE_INFO;
}  // namespace opt

}  // namespace air

#endif
