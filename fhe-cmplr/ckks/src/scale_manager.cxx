//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "scale_manager.h"

#include <string>

#include "air/base/container.h"
#include "air/base/container_decl.h"
#include "air/base/opcode.h"
#include "air/base/st_decl.h"
#include "air/opt/ssa_build.h"
#include "air/util/debug.h"
#include "fhe/ckks/ckks_gen.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/ckks/config.h"

namespace fhe {
namespace ckks {
bool SCALE_MNG_CTX::Rescale_phi_res(air::opt::PHI_NODE_PTR phi,
                                    uint32_t opnd_id, uint32_t scale_deg) {
  if (!Config()->Rescale_phi_res()) return false;

  if (scale_deg <= 1) return false;
  AIR_ASSERT(scale_deg == 2);

  STMT_PTR def_stmt = phi->Def_stmt();
  // 1. Only rescale phi nodes defined at do_loops.
  if (def_stmt->Opcode() != air::core::OPC_DO_LOOP) return false;

  // 2. because the value of phi result equal to the phi-opnd from backedge,
  //    we check if phi nodes need rescale in handling the backedge phi-opnd.
  if (opnd_id != air::opt::BACK_EDGE_PHI_OPND_ID) return false;

  // 3. skip phi node with zero-version phi opnd for preheader,
  //    because these phi nodes are dead after do_loop.
  if (phi->Opnd(air::opt::PREHEADER_PHI_OPND_ID)->Version() ==
      air::opt::SSA_VER::NO_VER)
    return false;

  // 4. skip phi nodes of nestted do_loops.
  NODE_PTR outer_block = def_stmt->Parent_node();
  NODE_PTR func_body   = Ssa_cntr()->Container()->Entry_node()->Body_blk();
  return (outer_block == func_body);
}

bool SCALE_MNG_CTX::Rescale_node(NODE_PTR node, STMT_PTR occ_stmt,
                                 uint32_t scale_deg) {
  AIR_ASSERT(occ_stmt != STMT_PTR());
  AIR_ASSERT(node != NODE_PTR());

  if (scale_deg <= 1) return false;
  AIR_ASSERT(scale_deg == 2);

  // 1. rescale nodes in the out-most block
  air::opt::SSA_CONTAINER* ssa_cntr = Ssa_cntr();
  NODE_PTR func_body = ssa_cntr->Container()->Entry_node()->Last_child();
  if (occ_stmt->Parent_node() == func_body) return true;

  air::opt::SSA_VER_ID ver_id = ssa_cntr->Node_ver_id(node->Id());
  if (ver_id == Null_id) return false;
  air::opt::SSA_VER_PTR ver = ssa_cntr->Ver(ver_id);
  STMT_PTR              def_stmt;
  switch (ver->Kind()) {
    case air::opt::VER_DEF_KIND::STMT: {
      def_stmt = ssa_cntr->Container()->Stmt(ver->Def_stmt_id());
      break;
    }
    case air::opt::VER_DEF_KIND::CHI: {
      def_stmt = ssa_cntr->Chi_node(ver->Def_chi_id())->Def_stmt();
      break;
    }
    case air::opt::VER_DEF_KIND::PHI: {
      def_stmt = ssa_cntr->Phi_node(ver->Def_phi_id())->Def_stmt();
      break;
    }
    default: {
      AIR_ASSERT_MSG(false, "not supported define kind");
    }
  }
  // 2. rescale nodes defined in out-most block
  return (def_stmt->Parent_node() == func_body);
}

void CKKS_SCALE_MANAGER::Handle_encode_in_bin_arith_node(
    SCALE_MNG_CTX* ana_ctx, NODE_PTR bin_node, uint32_t child0_scale) {
  OPCODE bin_arith_op = bin_node->Opcode();
  // only support CKKS.mul/add/sub
  AIR_ASSERT(bin_arith_op == OPC_MUL || bin_arith_op == OPC_ADD ||
             bin_arith_op == OPC_SUB);

  NODE_PTR encode_node = bin_node->Child(1);
  AIR_ASSERT(encode_node->Opcode() == OPC_ENCODE);

  SPOS           spos           = bin_node->Spos();
  const uint32_t scale_child_id = 2;
  NODE_PTR       scale          = encode_node->Child(scale_child_id);
  CONTAINER*     cntr           = bin_node->Container();
  if (ana_ctx->Const_encode_scale()) {
    TYPE_PTR u32_type  = cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_U32);
    NODE_PTR new_scale = cntr->New_intconst(u32_type, child0_scale, spos);
    if (scale->Opcode() == air::core::OPC_INTCONST &&
        scale->Intconst() != child0_scale) {
      ana_ctx->Trace(TRACE_CKKS_TRAN_RES, "Update target scale of encode from ",
                     scale->Intconst(), " to ", child0_scale);
    }
    encode_node->Set_child(scale_child_id, new_scale);
    return;
  }

  // 2. handle scale get from child0
  AIR_ASSERT(scale->Opcode() == OPC_SCALE);
  NODE_PTR child0 = bin_node->Child(0);
  AIR_ASSERT(ana_ctx->Lower_ctx()->Is_cipher_type(child0->Rtype_id()));
  if (!child0->Is_ld() || !child0->Has_sym()) {
    PREG_PTR preg        = cntr->Parent_func_scope()->New_preg(child0->Rtype());
    STMT_PTR stp_child0  = cntr->New_stp(child0, preg, spos);
    STMT_PTR parent_stmt = ana_ctx->Parent_stmt();
    STMT_LIST(ana_ctx->Parent_block()).Prepend(parent_stmt, stp_child0);
    child0 = cntr->New_ldp(preg, spos);
    bin_node->Set_child(0, child0);
  } else {
    NODE_PTR cipher_var = scale->Child(0);
    // scale and mul_level get from current child0, not need update
    if (cipher_var->Is_ld() && cipher_var->Has_sym() &&
        cipher_var->Addr_datum() == child0->Addr_datum()) {
      return;
    }
  }
  // reset scale and level nodes of encode
  child0 = bin_node->Child(0);
  CKKS_GEN       ckks_gen(cntr, ana_ctx->Lower_ctx());
  const uint32_t level_child_id = 3;
  if (child0->Opcode() == air::core::OPC_LD) {
    ADDR_DATUM_PTR cipher_var = child0->Addr_datum();
    scale->Set_child(0, cntr->New_ld(cipher_var, spos));
    NODE_PTR new_level = ckks_gen.Gen_get_level(cntr->New_ld(cipher_var, spos));
    encode_node->Set_child(level_child_id, new_level);
  } else if (child0->Opcode() == air::core::OPC_LDP) {
    PREG_PTR cipher_var = child0->Preg();
    scale->Set_child(0, cntr->New_ldp(cipher_var, spos));
    NODE_PTR new_level =
        ckks_gen.Gen_get_level(cntr->New_ldp(cipher_var, spos));
    encode_node->Set_child(level_child_id, new_level);
  }

  // set the is_ext child
  const uint32_t is_ext_child_id = 4;
  TYPE_PTR       t_is_ext = cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::BOOL);
  NODE_PTR       n_is_ext = cntr->New_zero(t_is_ext, spos);
  encode_node->Set_child(is_ext_child_id, n_is_ext);
  return;
}

void SCALE_MANAGER::Build_ssa() {
  air::opt::SSA_BUILDER ssa_builder(Func_scope(), Ssa_cntr(),
                                    Mng_ctx().Driver_ctx());
  // update SSA_CONFIG
  air::opt::SSA_CONFIG& ssa_config = ssa_builder.Ssa_config();
  ssa_config.Set_trace_ir_before_ssa(
      Mng_ctx().Config()->Is_trace(ckks::TRACE_DETAIL::TRACE_IR_BEFORE_SSA));
  ssa_config.Set_trace_ir_after_insert_phi(Mng_ctx().Config()->Is_trace(
      ckks::TRACE_DETAIL::TRACE_IR_AFTER_SSA_INSERT_PHI));
  ssa_config.Set_trace_ir_after_ssa(
      Mng_ctx().Config()->Is_trace(ckks::TRACE_DETAIL::TRACE_IR_AFTER_SSA));

  ssa_builder.Perform();
}

void SCALE_MANAGER::Rescale_phi_res() {
  CONTAINER*  cntr       = Ssa_cntr()->Container();
  FUNC_SCOPE* func_scope = cntr->Parent_func_scope();
  CKKS_GEN    ckks_gen(cntr, Lower_ctx());

  for (air::opt::PHI_NODE_ID phi_id : Mng_ctx().Phi_res_need_rescale()) {
    air::opt::PHI_NODE_PTR phi      = Ssa_cntr()->Phi_node(phi_id);
    STMT_PTR               def_stmt = phi->Def_stmt();
    const SPOS&            spos     = def_stmt->Spos();
    air::opt::SSA_SYM_PTR  ssa_sym  = phi->Sym();
    STMT_PTR               rescale_stmt;
    SCALE_INFO scale_info = Mng_ctx().Get_scale_info(phi->Result_id());
    AIR_ASSERT(scale_info.Scale_deg() == 1);
    AIR_ASSERT(scale_info.Rescale_level() >= 1);
    if (ssa_sym->Is_preg()) {
      PREG_ID  preg_id(ssa_sym->Var_id());
      PREG_PTR preg = func_scope->Preg(preg_id);
      NODE_PTR ldp  = cntr->New_ldp(preg, spos);
      Mng_ctx().Set_node_scale_info(ldp, scale_info.Scale_deg() + 1,
                                    scale_info.Rescale_level() - 1);

      NODE_PTR rescale_preg = ckks_gen.Gen_rescale(ldp);
      Mng_ctx().Set_node_scale_info(rescale_preg, scale_info);

      rescale_stmt = cntr->New_stp(rescale_preg, preg, spos);
    } else if (ssa_sym->Is_addr_datum()) {
      ADDR_DATUM_ID  datum_id(ssa_sym->Var_id());
      ADDR_DATUM_PTR addr_datum = func_scope->Addr_datum(datum_id);
      NODE_PTR       ld         = cntr->New_ld(addr_datum, spos);
      Mng_ctx().Set_node_scale_info(ld, scale_info.Scale_deg() + 1,
                                    scale_info.Rescale_level() - 1);

      NODE_PTR rescale_datum = ckks_gen.Gen_rescale(ld);
      Mng_ctx().Set_node_scale_info(rescale_datum, scale_info);

      rescale_stmt = cntr->New_st(rescale_datum, addr_datum, spos);
    }

    STMT_LIST sl(def_stmt->Parent_node());
    sl.Append(phi->Def_stmt(), rescale_stmt);
    Mng_ctx().Trace(TRACE_CKKS_TRAN_RES,
                    "\nSCALE_MANAGER::Rescale_phi_res rescale phi result ",
                    ssa_sym->To_str(), ":\n");
    Mng_ctx().Trace_obj(TRACE_CKKS_TRAN_RES, rescale_stmt);
  }
}

}  // namespace ckks
}  // namespace fhe