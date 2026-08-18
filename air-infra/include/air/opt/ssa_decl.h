//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_SSA_DECL_H
#define AIR_OPT_SSA_DECL_H

#include "air/base/id_wrapper.h"
#include "air/base/ptr_wrapper.h"

namespace air {

namespace opt {

//! @brief SSA Data Structure Declarations

class SSA_CONTAINER;

class SSA_SYM_DATA;
class SSA_VER_DATA;
class MU_NODE_DATA;
class CHI_NODE_DATA;
class PHI_NODE_DATA;

typedef air::base::ID<SSA_SYM_DATA>  SSA_SYM_ID;
typedef air::base::ID<SSA_VER_DATA>  SSA_VER_ID;
typedef air::base::ID<MU_NODE_DATA>  MU_NODE_ID;
typedef air::base::ID<CHI_NODE_DATA> CHI_NODE_ID;
typedef air::base::ID<PHI_NODE_DATA> PHI_NODE_ID;

typedef air::base::PTR_FROM_DATA<SSA_SYM_DATA>  SSA_SYM_DATA_PTR;
typedef air::base::PTR_FROM_DATA<SSA_VER_DATA>  SSA_VER_DATA_PTR;
typedef air::base::PTR_FROM_DATA<MU_NODE_DATA>  MU_NODE_DATA_PTR;
typedef air::base::PTR_FROM_DATA<CHI_NODE_DATA> CHI_NODE_DATA_PTR;
typedef air::base::PTR_FROM_DATA<PHI_NODE_DATA> PHI_NODE_DATA_PTR;

class SSA_SYM;
class SSA_VER;
class MU_NODE;
class CHI_NODE;
class PHI_NODE;

typedef air::base::PTR<SSA_SYM>  SSA_SYM_PTR;
typedef air::base::PTR<SSA_VER>  SSA_VER_PTR;
typedef air::base::PTR<MU_NODE>  MU_NODE_PTR;
typedef air::base::PTR<CHI_NODE> CHI_NODE_PTR;
typedef air::base::PTR<PHI_NODE> PHI_NODE_PTR;

typedef air::base::PTR_TO_CONST<SSA_SYM>  CONST_SSA_SYM_PTR;
typedef air::base::PTR_TO_CONST<SSA_VER>  CONST_SSA_VER_PTR;
typedef air::base::PTR_TO_CONST<MU_NODE>  CONST_MU_NODE_PTR;
typedef air::base::PTR_TO_CONST<CHI_NODE> CONST_CHI_NODE_PTR;
typedef air::base::PTR_TO_CONST<PHI_NODE> CONST_PHI_NODE_PTR;

}  // namespace opt

}  // namespace air

#endif  // AIR_OPT_SSA_DECL_H
