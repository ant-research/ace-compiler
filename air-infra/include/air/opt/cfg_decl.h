//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_CFG_DECL_H
#define AIR_OPT_CFG_DECL_H

#include <set>
#include <vector>

#include "air/base/id_wrapper.h"
#include "air/base/ptr_wrapper.h"
#include "air/util/mem_allocator.h"
#include "air/util/mem_pool.h"

namespace air {

namespace opt {
class BB_DATA;
class BB;
class CFG;
class LOOP_INFO_DATA;
class LOOP_INFO;
template <typename ID_TYPE, typename PTR_TYPE, typename CONT_TYPE>
class NODE_LIST;

typedef air::base::ID<BB_DATA>            BB_ID;
typedef air::base::PTR_FROM_DATA<BB_DATA> BB_DATA_PTR;
typedef air::base::PTR<BB>                BB_PTR;
typedef NODE_LIST<BB_ID, BB_PTR, CFG>     BB_LIST;

typedef air::util::MEM_POOL<4096>                         CFG_MEM_POOL;
typedef air::util::CXX_MEM_ALLOCATOR<BB_ID, CFG_MEM_POOL> BBID_ALLOC;
typedef std::vector<BB_ID, BBID_ALLOC>                    BBID_VEC;
typedef std::set<BB_ID, std::less<BB_ID>, BBID_ALLOC>     BBID_CUS_SET;
typedef std::set<BB_ID>                                   BBID_SET;

typedef air::util::CXX_MEM_ALLOCATOR<BBID_CUS_SET, CFG_MEM_POOL>
                                                      BBID_CUS_SET_ALLOC;
typedef std::vector<BBID_CUS_SET, BBID_CUS_SET_ALLOC> BBID_SET_VEC;

typedef air::util::CXX_MEM_ALLOCATOR<BBID_VEC, CFG_MEM_POOL> BBID_VEC_ALLOC;
typedef std::vector<BBID_VEC, BBID_VEC_ALLOC>                BBID_VEC_VEC;

typedef air::util::CXX_MEM_ALLOCATOR<int32_t, CFG_MEM_POOL> I32_ALLOC;
typedef std::vector<int32_t, I32_ALLOC>                     I32_VEC;

typedef air::base::ID<LOOP_INFO_DATA>            LOOP_INFO_ID;
typedef air::base::PTR_FROM_DATA<LOOP_INFO_DATA> LOOP_INFO_DATA_PTR;
typedef air::base::PTR<LOOP_INFO>                LOOP_INFO_PTR;

}  // namespace opt
}  // namespace air

#endif
