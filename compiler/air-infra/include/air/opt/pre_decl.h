//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================
#ifndef AIR_OPT_PRE_DECL_H
#define AIR_OPT_PRE_DECL_H

#include "air/base/id_wrapper.h"
#include "air/base/ptr_wrapper.h"
#include "air/opt/node_list.h"

namespace air {
namespace opt {

class PRE_CONTAINER;
class OCC;
class OCC_DATA;
class REAL_OCC_DATA;
class PHI_OCC_DATA;
class PHI_OPND_OCC_DATA;
class PRE_CAND;
class PRE_CAND_DATA;
template <typename ID_TYPE, typename PTR_TYPE, typename CONT_TYPE>
class NODE_LIST;

typedef air::base::ID<OCC_DATA>      OCC_ID;
typedef air::base::ID<PRE_CAND_DATA> PRE_CAND_ID;

typedef air::base::PTR_FROM_DATA<PRE_CAND_DATA>     PRE_CAND_DATA_PTR;
typedef air::base::PTR_FROM_DATA<OCC_DATA>          OCC_DATA_PTR;
typedef air::base::PTR_FROM_DATA<REAL_OCC_DATA>     REAL_OCC_DATA_PTR;
typedef air::base::PTR_FROM_DATA<PHI_OCC_DATA>      PHI_OCC_DATA_PTR;
typedef air::base::PTR_FROM_DATA<PHI_OPND_OCC_DATA> PHI_OPND_OCC_DATA_PTR;

typedef air::base::PTR<OCC>      OCC_PTR;
typedef air::base::PTR<PRE_CAND> PRE_CAND_PTR;

typedef NODE_LIST<OCC_ID, OCC_PTR, PRE_CONTAINER>           OCC_LIST;
typedef NODE_LIST<PRE_CAND_ID, PRE_CAND_PTR, PRE_CONTAINER> PRE_CAND_LIST;

}  // namespace opt
}  // namespace air

#endif