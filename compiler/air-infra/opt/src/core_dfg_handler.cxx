//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/opt/core_dfg_handler.h"

#include "air/opt/ssa_decl.h"
#include "air/opt/ssa_node_list.h"
#include "air/util/debug.h"

namespace air {
namespace opt {

void CORE_DATA_FLOW_HANDLER::Handle_phi_list(DFG_BUILD_CTX& ctx,
                                             NODE_PTR       node) {
  auto phi_handler = [](PHI_NODE_PTR phi, DFG_BUILD_CTX& ctx) {
    // 1. create DFG node of phi result
    SSA_VER_PTR           res_ver  = phi->Result();
    air::opt::SSA_SYM_PTR ssa_sym  = phi->Sym();
    TYPE_ID               sym_type = ssa_sym->Type_id();
    uint32_t              freq     = ctx.Top_freq();
    DFG_NODE_PTR          res_node = ctx.Get_dfg_node(res_ver, sym_type, freq);

    // 2. connect DFG nodes of opnds with that of phi result
    for (uint32_t id = 0; id < phi->Size(); ++id) {
      SSA_VER_PTR opnd = phi->Opnd(id);
      AIR_ASSERT(opnd->Sym_id() == res_ver->Sym_id());
      DFG_NODE_PTR opnd_dfg_node = ctx.Get_dfg_node(opnd, sym_type);
      res_node->Set_opnd(id, opnd_dfg_node);
    }
  };

  const SSA_CONTAINER* ssa_cntr = ctx.Ssa_cntr();
  AIR_ASSERT(ssa_cntr->Has_phi(node));
  PHI_NODE_ID phi_id = ssa_cntr->Node_phi(node->Id());
  PHI_LIST    phi_list(ssa_cntr, phi_id);
  phi_list.For_each(phi_handler, ctx);
}

}  // namespace opt
}  // namespace air