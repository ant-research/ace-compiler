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
#include "air/util/cfg_base.h"

namespace air {

namespace cg {

// forward declaration for CGIR data structures
class CGIR_CONTAINER;  // CGIR container
class OPND;            // instruction operand
class INST;            // instruction
class BB;              // basic block (node on control flow graph)
class EDGE;            // edge on control flow graph
class OPND_DATA;       // operand data
class INST_DATA;       // instruction data
class BB_DATA;         // basic block data
class EDGE_DATA;       // edge data

// memory pool
typedef air::util::MEM_POOL<4096> MEM_POOL;

// CFG types
typedef air::util::CFG<BB_DATA, EDGE_DATA, CGIR_CONTAINER> CFG;
typedef CFG::NODE_ID                                       BB_ID;
typedef CFG::EDGE_ID                                       EDGE_ID;
typedef air::base::PTR<BB>                                 BB_PTR;
typedef air::base::PTR<EDGE>                               EDGE_PTR;

// Instruction & operand types
typedef air::base::PTR<OPND>                OPND_PTR;
typedef air::base::PTR<INST>                INST_PTR;
typedef air::base::ID<OPND_DATA>            OPND_ID;
typedef air::base::ID<INST_DATA>            INST_ID;
typedef air::base::PTR_FROM_DATA<OPND_DATA> OPND_DATA_PTR;
typedef air::base::PTR_FROM_DATA<INST_DATA> INST_DATA_PTR;

// NULL_BB_ID to represent an invalid BB_ID
static constexpr BB_ID NULL_BB_ID(air::base::Null_prim_id);

}  // namespace cg

}  // namespace air

#endif  // AIR_CG_CGIR_DECL_H
