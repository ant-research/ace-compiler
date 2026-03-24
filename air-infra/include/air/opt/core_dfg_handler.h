//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_CORE_DFG_HANDLER_H
#define AIR_OPT_CORE_DFG_HANDLER_H

#include "air/base/visitor.h"
#include "air/core/invalid_handler.h"
#include "air/core/opcode.h"
#include "air/opt/dfg_build_ctx.h"
#include "air/util/debug.h"

namespace air {
namespace opt {

//! @brief impl of data flow node handler for CORE operations.
class CORE_DATA_FLOW_HANDLER : public air::core::INVALID_HANDLER {
public:
  using NODE_ID        = air::base::NODE_ID;
  using NODE_PTR       = air::base::NODE_PTR;
  using STMT_PTR       = air::base::STMT_PTR;
  using TYPE_ID        = air::base::TYPE_ID;
  using TYPE_PTR       = air::base::TYPE_PTR;
  using ADDR_DATUM_PTR = air::base::ADDR_DATUM_PTR;

  template <typename RETV, typename VISITOR>
  RETV Handle_func_entry(VISITOR* visitor, NODE_PTR func_entry);
  template <typename RETV, typename VISITOR>
  RETV Handle_idname(VISITOR* visitor, NODE_PTR idname);
  template <typename RETV, typename VISITOR>
  RETV Handle_do_loop(VISITOR* visitor, NODE_PTR do_loop);
  template <typename RETV, typename VISITOR>
  RETV Handle_block(VISITOR* visitor, NODE_PTR block);

  //! @brief Handler of st node creates a DFG node for the defined SSA_VER
  //! and connects the DFG node of the rhs to it.
  //! In current impl, the defined SSA_VER in CHI_LIST is ignored.
  template <typename RETV, typename VISITOR>
  RETV Handle_st(VISITOR* visitor, NODE_PTR st) {
    return Handle_st_var<RETV>(visitor, st);
  }
  //! @brief Handler of stp node creates a DFG node for the defined SSA_VER
  //! and connects the DFG node of the rhs to it.
  template <typename RETV, typename VISITOR>
  RETV Handle_stp(VISITOR* visitor, NODE_PTR stp) {
    return Handle_st_var<RETV>(visitor, stp);
  }
  //! @brief Handler of stf node creates a DFG node for the defined SSA_VER
  //! and connects the DFG node of the rhs to it.
  //! In current impl, the nested sub-fields and the nesting objects defined
  //! in CHI_LIST is ignored.
  template <typename RETV, typename VISITOR>
  RETV Handle_stf(VISITOR* visitor, NODE_PTR stf) {
    return Handle_st_var<RETV>(visitor, stf);
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_st_var(VISITOR* visitor, NODE_PTR st);

  template <typename RETV, typename VISITOR>
  RETV Handle_ist(VISITOR* visitor, NODE_PTR ist);
  template <typename RETV, typename VISITOR>
  RETV Handle_call(VISITOR* visitor, NODE_PTR call);
  template <typename RETV, typename VISITOR>
  RETV Handle_retv(VISITOR* visitor, NODE_PTR retv);

  //! @brief Get and return the DFG node of the accessed SSA_VER
  template <typename RETV, typename VISITOR>
  RETV Handle_ld(VISITOR* visitor, NODE_PTR ld) {
    return Handle_ld_var<RETV>(visitor, ld);
  }
  //! @brief Get and return the DFG node of the accessed SSA_VER
  template <typename RETV, typename VISITOR>
  RETV Handle_ldp(VISITOR* visitor, NODE_PTR ldp) {
    return Handle_ld_var<RETV>(visitor, ldp);
  }
  //! @brief Get and return the DFG node of the accessed SSA_VER
  template <typename RETV, typename VISITOR>
  RETV Handle_ldf(VISITOR* visitor, NODE_PTR ldf) {
    return Handle_ld_var<RETV>(visitor, ldf);
  }
  //! @brief Get and return the DFG node of the accessed SSA_VER
  template <typename RETV, typename VISITOR>
  RETV Handle_ld_var(VISITOR* visitor, NODE_PTR ld);

  template <typename RETV, typename VISITOR>
  RETV Handle_ild(VISITOR* visitor, NODE_PTR ild);

  template <typename RETV, typename VISITOR>
  RETV Handle_add(VISITOR* visitor, NODE_PTR add) {
    return Handle_arith_node<RETV>(visitor, add);
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_sub(VISITOR* visitor, NODE_PTR sub) {
    return Handle_arith_node<RETV>(visitor, sub);
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_mul(VISITOR* visitor, NODE_PTR mul) {
    return Handle_arith_node<RETV>(visitor, mul);
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_shl(VISITOR* visitor, NODE_PTR shl) {
    return Handle_arith_node<RETV>(visitor, shl);
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_lshr(VISITOR* visitor, NODE_PTR lshr) {
    return Handle_arith_node<RETV>(visitor, lshr);
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_ashr(VISITOR* visitor, NODE_PTR ashr) {
    return Handle_arith_node<RETV>(visitor, ashr);
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_eq(VISITOR* visitor, NODE_PTR eq) {
    return Handle_arith_node<RETV>(visitor, eq);
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_ne(VISITOR* visitor, NODE_PTR ne) {
    return Handle_arith_node<RETV>(visitor, ne);
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_lt(VISITOR* visitor, NODE_PTR lt) {
    return Handle_arith_node<RETV>(visitor, lt);
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_gt(VISITOR* visitor, NODE_PTR gt) {
    return Handle_arith_node<RETV>(visitor, gt);
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_le(VISITOR* visitor, NODE_PTR le) {
    return Handle_arith_node<RETV>(visitor, le);
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_ge(VISITOR* visitor, NODE_PTR ge) {
    return Handle_arith_node<RETV>(visitor, ge);
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_array(VISITOR* visitor, NODE_PTR array) {
    return Handle_arith_node<RETV>(visitor, array);
  }
  //! @brief Create DFG node for current node, and connects the DFG nodes of its
  //! operands to it.
  template <typename RETV, typename VISITOR>
  RETV Handle_arith_node(VISITOR* visitor, NODE_PTR node);

  template <typename RETV, typename VISITOR>
  RETV Handle_ldc(VISITOR* visitor, NODE_PTR ldc) {
    return Handle_const<RETV>(visitor, ldc);
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_zero(VISITOR* visitor, NODE_PTR zero) {
    return Handle_const<RETV>(visitor, zero);
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_one(VISITOR* visitor, NODE_PTR one) {
    return Handle_const<RETV>(visitor, one);
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_intconst(VISITOR* visitor, NODE_PTR intconst) {
    return Handle_const<RETV>(visitor, intconst);
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_lda(VISITOR* visitor, NODE_PTR ldc) {
    return Handle_const<RETV>(visitor, ldc);
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_ldca(VISITOR* visitor, NODE_PTR ldca) {
    return Handle_const<RETV>(visitor, ldca);
  }
  //! @brief DFG node of constant is ignored in current impl.
  //! Handler of constant nodes return null DFG node.
  template <typename RETV, typename VISITOR>
  RETV Handle_const(VISITOR* visitor, NODE_PTR node);

  template <typename RETV, typename VISITOR>
  RETV Handle_validate(VISITOR* visitor, NODE_PTR node) {
    return RETV{DFG_NODE_PTR()};
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_tm_start(VISITOR* visitor, NODE_PTR node) {
    return RETV{DFG_NODE_PTR()};
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_tm_taken(VISITOR* visitor, NODE_PTR node) {
    return RETV{DFG_NODE_PTR()};
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_dump_var(VISITOR* visitor, NODE_PTR node) {
    return RETV{DFG_NODE_PTR()};
  }
  //! @brief Create DFG node for phi result and connect DFG node of each operand
  //! to it.
  void Handle_phi_list(DFG_BUILD_CTX& ctx, NODE_PTR node);
};

template <typename RETV, typename VISITOR>
RETV CORE_DATA_FLOW_HANDLER::Handle_func_entry(VISITOR* visitor,
                                               NODE_PTR func_entry) {
  DFG_BUILD_CTX& ctx = visitor->Context();
  ctx.Push_freq(1);
  DFG_BUILD_CTX::DFG& dfg = ctx.Dfg();
  for (uint32_t id = 0; id < func_entry->Num_child(); ++id) {
    NODE_PTR child = func_entry->Child(id);
    RETV     res   = visitor->template Visit<RETV>(child);
    if (child->Opcode() == air::core::OPC_IDNAME) {
      DFG_NODE_ID dfg_node = res.Node()->Id();
      AIR_ASSERT(dfg.Container().Entry_cnt() == id);
      dfg.Container().Set_entry(id, dfg_node);
    }
  }
  return RETV();
}

template <typename RETV, typename VISITOR>
RETV CORE_DATA_FLOW_HANDLER::Handle_idname(VISITOR* visitor, NODE_PTR idname) {
  DFG_BUILD_CTX&       ctx      = visitor->Context();
  const SSA_CONTAINER* ssa_cntr = ctx.Ssa_cntr();
  SSA_VER_ID           ver_id   = ssa_cntr->Node_ver_id(idname->Id());
  SSA_VER_PTR          ssa_ver  = ssa_cntr->Ver(ver_id);
  DFG_NODE_PTR         dfg_node =
      ctx.Get_dfg_node(ssa_ver, idname->Addr_datum()->Type_id(), 1);
  return RETV{dfg_node};
}

template <typename RETV, typename VISITOR>
RETV CORE_DATA_FLOW_HANDLER::Handle_do_loop(VISITOR* visitor,
                                            NODE_PTR do_loop) {
  DFG_BUILD_CTX& ctx = visitor->Context();
  // 1. handle phi list
  Handle_phi_list(ctx, do_loop);

  // 2. cal and record iteration count of do_loop
  NODE_PTR init_node = do_loop->Child(0);
  AIR_ASSERT(init_node->Opcode() == air::core::OPC_INTCONST);
  int64_t init_val = init_node->Intconst();

  NODE_PTR condition_node = do_loop->Child(1);
  AIR_ASSERT(condition_node->Opcode() == air::core::OPC_LT);
  NODE_PTR upper_node = condition_node->Child(1);
  AIR_ASSERT(upper_node->Opcode() == air::core::OPC_INTCONST);
  int64_t upper_bound = upper_node->Intconst();

  NODE_PTR incr_node = do_loop->Child(2);
  AIR_ASSERT(incr_node->Opcode() == air::core::OPC_ADD &&
             incr_node->Child(1)->Opcode() == air::core::OPC_INTCONST);
  int64_t incr_val = incr_node->Child(1)->Intconst();
  AIR_ASSERT_MSG(incr_val != 0, "loop incr must not be zero");

  int64_t cur_freq = (upper_bound - init_val) / incr_val;
  AIR_ASSERT(cur_freq > 0);
  ctx.Push_freq(cur_freq);

  // 3. handle loop body
  NODE_PTR loop_body = do_loop->Last_child();
  (void)visitor->template Visit<RETV>(loop_body);
  AIR_ASSERT(!ctx.Freq_empty());
  AIR_ASSERT(ctx.Top_freq() == cur_freq);
  ctx.Pop_freq();
  return RETV();
}

template <typename RETV, typename VISITOR>
RETV CORE_DATA_FLOW_HANDLER::Handle_block(VISITOR* visitor, NODE_PTR block) {
  for (air::base::STMT_PTR stmt        = block->Begin_stmt();
       stmt != block->End_stmt(); stmt = stmt->Next()) {
    (void)visitor->template Visit<RETV>(stmt->Node());
  }
  return RETV();
}

template <typename RETV, typename VISITOR>
RETV CORE_DATA_FLOW_HANDLER::Handle_st_var(VISITOR* visitor, NODE_PTR st) {
  // 1. handle rhs node
  NODE_PTR     rhs      = st->Child(0);
  DFG_NODE_PTR rhs_node = visitor->template Visit<RETV>(rhs).Node();

  // 2. Generate data flow node for lhs of st, and link rhs node to lhs node.
  DFG_BUILD_CTX&       ctx      = visitor->Context();
  uint32_t             freq     = (rhs_node == DFG_NODE_PTR() ? 0 : ctx.Freq());
  const SSA_CONTAINER* ssa_cntr = ctx.Ssa_cntr();
  SSA_VER_ID           ver_id   = ssa_cntr->Node_ver_id(st->Id());
  SSA_VER_PTR          ssa_ver  = ssa_cntr->Ver(ver_id);
  TYPE_ID              type     = ssa_cntr->Sym(ssa_ver->Sym_id())->Type_id();
  DFG_NODE_PTR         dfg_node = ctx.Get_dfg_node(ssa_ver, type, freq);

  if (rhs_node != DFG_NODE_PTR()) dfg_node->Set_opnd(0, rhs_node);
  return RETV{dfg_node};
}

template <typename RETV, typename VISITOR>
RETV CORE_DATA_FLOW_HANDLER::Handle_ist(VISITOR* visitor, NODE_PTR ist) {
  DFG_BUILD_CTX& ctx = visitor->Context();
  NODE_PTR       rhs = ist->Child(1);

  // check supported format of istore
  NODE_PTR addr_child = ist->Child(0);
  AIR_ASSERT(addr_child->Opcode() == air::core::OPC_ARRAY);
  NODE_PTR lda_child = addr_child->Array_base();
  AIR_ASSERT(lda_child->Opcode() == air::core::OPC_LDA);
  ADDR_DATUM_PTR array_sym  = lda_child->Addr_datum();
  TYPE_PTR       array_type = array_sym->Type();
  AIR_ASSERT(array_type->Is_array());

  // 1. generate data flow node for rhs node
  DFG_NODE_PTR rhs_node = visitor->template Visit<RETV>(rhs).Node();
  uint32_t     freq     = (rhs_node == DFG_NODE_PTR() ? 0 : ctx.Freq());

  // 2. Generate data flow node for lhs of ist, and link rhs node to lhs node.
  const SSA_CONTAINER* ssa_cntr = ctx.Ssa_cntr();
  SSA_VER_PTR          ssa_ver  = ssa_cntr->Node_ver(ist->Id());
  DFG_NODE_PTR dfg_node = ctx.Get_dfg_node(ssa_ver, array_type->Id(), freq);

  if (rhs_node != DFG_NODE_PTR()) dfg_node->Set_opnd(0, rhs_node);
  return RETV{dfg_node};
}

template <typename RETV, typename VISITOR>
RETV CORE_DATA_FLOW_HANDLER::Handle_call(VISITOR* visitor, NODE_PTR call) {
  DFG_BUILD_CTX&       ctx           = visitor->Context();
  const SSA_CONTAINER* ssa_cntr      = ctx.Ssa_cntr();
  SSA_VER_PTR          ret_ver       = ssa_cntr->Node_ver(call->Id());
  TYPE_ID              type          = call->Ret_preg()->Type_id();
  uint32_t             freq          = ctx.Freq();
  DFG_NODE_PTR         ret_dfg_node  = ctx.Get_dfg_node(ret_ver, type, freq);
  DFG_NODE_PTR         call_dfg_node = ctx.Get_dfg_node(call, type, freq);
  ret_dfg_node->Set_opnd(0, call_dfg_node->Id());
  for (uint32_t id = 0; id < call->Num_child(); ++id) {
    NODE_PTR     child      = call->Child(id);
    DFG_NODE_PTR child_node = visitor->template Visit<RETV>(child).Node();
    call_dfg_node->Set_opnd(id, child_node->Id());
  }
  return RETV{ret_dfg_node};
}

template <typename RETV, typename VISITOR>
RETV CORE_DATA_FLOW_HANDLER::Handle_retv(VISITOR* visitor, NODE_PTR retv) {
  DFG_BUILD_CTX& ctx   = visitor->Context();
  TYPE_ID        rtype = retv->Child(0)->Rtype_id();

  AIR_ASSERT_MSG(ctx.Top_freq() == 1, "freq of retv must be 1");
  DFG_NODE_PTR dfg_node = ctx.Get_dfg_node(retv, rtype, 1);
  RETV         opnd_res = visitor->template Visit<RETV>(retv->Child(0));
  dfg_node->Set_opnd(0, opnd_res.Node());
  ctx.Dfg().Container().Add_exit(dfg_node->Id());
  return RETV{dfg_node};
}

template <typename RETV, typename VISITOR>
RETV CORE_DATA_FLOW_HANDLER::Handle_ld_var(VISITOR* visitor, NODE_PTR ld) {
  DFG_BUILD_CTX& ctx   = visitor->Context();
  TYPE_ID        rtype = ld->Rtype_id();

  // get and return the data-flow-graph node of loaded SSA_VER.
  const SSA_CONTAINER* ssa_cntr = ctx.Ssa_cntr();
  SSA_VER_PTR          ssa_ver  = ssa_cntr->Node_ver(ld->Id());
  DFG_NODE_PTR         dfg_node = ctx.Get_dfg_node(ssa_ver, rtype);
  // update the frequency of the phi result. If it is used only within its
  // defining loop, set its frequency to the loop count. Otherwise, set it to
  // the frequency of the loop stmt.
  if (ssa_ver->Kind() == VER_DEF_KIND::PHI) {
    PHI_NODE_PTR phi      = ssa_ver->Def_phi();
    STMT_PTR     def_stmt = phi->Def_stmt();
    NODE_PTR     def_node = def_stmt->Node();
    if (def_node->Is_if()) {
      dfg_node->Set_freq(1);
    } else if (def_node->Is_do_loop()) {
      uint32_t freq         = ctx.Freq();
      NODE_PTR def_block    = def_stmt->Parent_node();
      NODE_PTR parent_block = ctx.Parent_block();
      if (parent_block == def_node->Last_child() || parent_block == def_block) {
        dfg_node->Set_freq(freq);
      } else if (def_block == ssa_cntr->Container()->Entry_node()->Last_child()) {
        dfg_node->Set_freq(1);
      } else {
        AIR_ASSERT_MSG(false, "not supported case");
      }
    } else {
      AIR_ASSERT_MSG(false, "not supported case");
    }
  }
  return RETV{dfg_node};
}

template <typename RETV, typename VISITOR>
RETV CORE_DATA_FLOW_HANDLER::Handle_ild(VISITOR* visitor, NODE_PTR ild) {
  // 1. check supported format of iload
  NODE_PTR addr_child = ild->Child(0);
  AIR_ASSERT(addr_child->Opcode() == air::core::OPC_ARRAY);
  NODE_PTR lda_child = addr_child->Array_base();
  if (lda_child->Opcode() != air::core::OPC_LDA) {
    AIR_ASSERT(lda_child->Opcode() == core::OPC_LDCA);
    return RETV{DFG_NODE_PTR()};
  }
  AIR_ASSERT(lda_child->Opcode() == air::core::OPC_LDA);
  ADDR_DATUM_PTR array_sym  = lda_child->Addr_datum();
  TYPE_PTR       array_type = array_sym->Type();
  AIR_ASSERT(array_type->Is_array());

  // 2. get and return data-flow-graph node iloaded SSA_VER
  DFG_BUILD_CTX&       ctx      = visitor->Context();
  const SSA_CONTAINER* ssa_cntr = ctx.Ssa_cntr();
  SSA_VER_PTR          ssa_ver  = ssa_cntr->Node_ver(ild->Id());

  DFG_NODE_PTR dfg_node = ctx.Get_dfg_node(ssa_ver, array_type->Id());
  return RETV{dfg_node};
}

template <typename RETV, typename VISITOR>
RETV CORE_DATA_FLOW_HANDLER::Handle_const(VISITOR* visitor, NODE_PTR node) {
  return RETV{DFG_NODE_PTR()};
  DFG_BUILD_CTX& ctx      = visitor->Context();
  uint32_t       freq     = ctx.Freq();
  DFG_NODE_PTR   dfg_node = ctx.Get_dfg_node(node, node->Rtype_id(), freq);
  return RETV{dfg_node};
}

template <typename RETV, typename VISITOR>
RETV CORE_DATA_FLOW_HANDLER::Handle_arith_node(VISITOR* visitor,
                                               NODE_PTR node) {
  DFG_BUILD_CTX& ctx  = visitor->Context();
  uint32_t       freq = ctx.Freq();

  DFG_NODE_PTR dfg_node = ctx.Get_dfg_node(node, node->Rtype_id(), freq);
  for (uint32_t id = 0; id < node->Num_child(); ++id) {
    NODE_PTR     child      = node->Child(id);
    DFG_NODE_PTR child_node = visitor->template Visit<RETV>(child).Node();
    dfg_node->Set_opnd(id, child_node);
  }
  return RETV{dfg_node};
}

}  // namespace opt
}  // namespace air
#endif  // AIR_OPT_CORE_DFG_HANDLER_H