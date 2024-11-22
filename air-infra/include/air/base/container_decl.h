//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_CONTAINER_DECL_H
#define AIR_BASE_CONTAINER_DECL_H

#include "air/base/ptr_wrapper.h"

namespace air {
namespace base {

#define OFFSETOF(s, m) (((size_t)(&(((s*)128L)->m))) - ((size_t)(s*)128L))

class CONTAINER;
class STMT_LIST;
class NODE_DATA;
class STMT_DATA;
class NODE;
class STMT;

typedef ID<NODE_DATA> NODE_ID;
typedef ID<STMT_DATA> STMT_ID;

typedef PTR_FROM_DATA<NODE_DATA> NODE_DATA_PTR;
typedef PTR_FROM_DATA<STMT_DATA> STMT_DATA_PTR;

typedef PTR<NODE>          NODE_PTR;
typedef PTR<STMT>          STMT_PTR;
typedef PTR_TO_CONST<NODE> CONST_NODE_PTR;
typedef PTR_TO_CONST<STMT> CONST_STMT_PTR;

}  // namespace base
}  // namespace air

#endif
