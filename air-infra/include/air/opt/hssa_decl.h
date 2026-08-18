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

//! @brief SSA Data Structure Declarations

class HSSA_CONTAINER;

class CR;
class HCR_DATA;
class OP_HCR_DATA;
class VAR_HCR_DATA;
class LDA_HCR_DATA;
class CST_HCR_DATA;
class HCR;
class BLK_DATA;
class ASSIGN_HSTMT_DATA;
class OP_HSTMT_DATA;
class DO_LOOP_HSTMT_DATA;
class IF_HSTMT_DATA;
class CALL_HSTMT_DATA;
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

typedef air::base::ID<OP_HCR_DATA> OP_HCR_ID;
typedef air::base::ID<HCR_DATA>    HCR_ID;
typedef air::base::ID<HSTMT_DATA>  HSTMT_ID;
typedef air::base::ID<HMU_DATA>    HMU_ID;
typedef air::base::ID<HCHI_DATA>   HCHI_ID;
typedef air::base::ID<HPHI_DATA>   HPHI_ID;

typedef air::base::PTR_FROM_DATA<HCR_DATA>           HCR_DATA_PTR;
typedef air::base::PTR_FROM_DATA<VAR_HCR_DATA>       VAR_HCR_DATA_PTR;
typedef air::base::PTR_FROM_DATA<LDA_HCR_DATA>       LDA_HCR_DATA_PTR;
typedef air::base::PTR_FROM_DATA<CST_HCR_DATA>       CST_HCR_DATA_PTR;
typedef air::base::PTR_FROM_DATA<OP_HCR_DATA>        OP_HCR_DATA_PTR;
typedef air::base::PTR_FROM_DATA<HSTMT_DATA>         HSTMT_DATA_PTR;
typedef air::base::PTR_FROM_DATA<BLK_DATA>           BLK_BEGIN_DATA_PTR;
typedef air::base::PTR_FROM_DATA<ASSIGN_HSTMT_DATA>  ASSIGN_HSTMT_DATA_PTR;
typedef air::base::PTR_FROM_DATA<OP_HSTMT_DATA>      OP_HSTMT_DATA_PTR;
typedef air::base::PTR_FROM_DATA<DO_LOOP_HSTMT_DATA> DO_LOOP_HSTMT_DATA_PTR;
typedef air::base::PTR_FROM_DATA<IF_HSTMT_DATA>      IF_HSTMT_DATA_PTR;
typedef air::base::PTR_FROM_DATA<CALL_HSTMT_DATA>    CALL_HSTMT_DATA_PTR;
typedef air::base::PTR_FROM_DATA<HMU_DATA>           HMU_DATA_PTR;
typedef air::base::PTR_FROM_DATA<HCHI_DATA>          HCHI_DATA_PTR;
typedef air::base::PTR_FROM_DATA<HPHI_DATA>          HPHI_DATA_PTR;

typedef air::base::PTR<HCR>   HCR_PTR;
typedef air::base::PTR<HSTMT> HSTMT_PTR;
typedef air::base::PTR<HMU>   HMU_PTR;
typedef air::base::PTR<HCHI>  HCHI_PTR;
typedef air::base::PTR<HPHI>  HPHI_PTR;

typedef air::base::PTR_TO_CONST<HCR>  CONST_HCR_PTR;
typedef air::base::PTR_TO_CONST<HMU>  CONST_HMU_PTR;
typedef air::base::PTR_TO_CONST<HCHI> CONST_HCHI_PTR;
typedef air::base::PTR_TO_CONST<HPHI> CONST_HPHI_PTR;

typedef NODE_LIST<HSTMT_ID, HSTMT_PTR, HSSA_CONTAINER> HSTMT_LIST;
typedef NODE_LIST<HCR_ID, HCR_PTR, HSSA_CONTAINER>     HCR_LIST;
typedef NODE_LIST<HPHI_ID, HPHI_PTR, HSSA_CONTAINER>   HPHI_LIST;
typedef NODE_LIST<HMU_ID, HMU_PTR, HSSA_CONTAINER>     HMU_LIST;
typedef NODE_LIST<HCHI_ID, HCHI_PTR, HSSA_CONTAINER>   HCHI_LIST;

typedef std::pair<HCR_PTR, HSTMT_PTR> NODE_INFO;
}  // namespace opt

}  // namespace air

#endif
