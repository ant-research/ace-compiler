//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_CG_CGIR_DECL_H
#define AIR_CG_CGIR_DECL_H

#include "air/base/arena.h"
#include "air/base/id_wrapper.h"
#include "air/base/ptr_wrapper.h"

namespace air {

namespace cg {

// forward declaration for CGIR data structures
class CGIR_CONTAINER;  // CGIR container
class OPND;            // instruction operand
class INST;            // instruction
class NODE;            // node on control flow graph (basic block)
class EDGE;            // edge on control flow graph
class OPND_DATA;       // operand data
class INST_DATA;       // instruction data
class NODE_DATA;       // operand data
class EDGE_DATA;       // instruction data

// CGIR types
typedef air::util::MEM_POOL<4096>           MEM_POOL;
typedef air::base::PTR<OPND>                OPND_PTR;
typedef air::base::PTR<INST>                INST_PTR;
typedef air::base::PTR<NODE>                NODE_PTR;
typedef air::base::PTR<EDGE>                EDGE_PTR;
typedef air::base::ID<OPND_DATA>            OPND_ID;
typedef air::base::ID<INST_DATA>            INST_ID;
typedef air::base::PTR_FROM_DATA<OPND_DATA> OPND_DATA_PTR;
typedef air::base::PTR_FROM_DATA<INST_DATA> INST_DATA_PTR;

}  // namespace cg

}  // namespace air

#endif  // AIR_CG_CGIR_DECL_H
