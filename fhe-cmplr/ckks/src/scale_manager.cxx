//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "scale_manager.h"

#include <string>
#include <unordered_map>
#include <unordered_set>

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
SCALE_INFO PARS::Handle(NODE_PTR node) {
  Rescale_ana(node);
  Level_match(node);
  Scale_match(node);
  SCALE_INFO info = Downscale_analysis(node);
  Context()->Set_node_scale_info(node, info);
  return info;
}

void PARS::Rescale_ana(NODE_PTR node) {
  SCALE_MNG_CTX* ctx = Context();
  for (uint32_t id = 0; id < node->Num_child(); ++id) {
    NODE_PTR       child = node->Child(id);
    SCALE_MAP_ITER iter  = ctx->Node_scale_info_iter(child->Id());
    if (iter == ctx->Node_scale_info().end()) continue;

    // rescale operands of which scale degree is larger than 2.
    SCALE_INFO si        = iter->second;
    uint32_t   scale_deg = iter->second.Scale_deg();
    if (scale_deg > 2) {
      uint32_t rs_cnt = scale_deg - 2;
      uint32_t rs_lev = iter->second.Rescale_level() + rs_cnt;

      si = SCALE_INFO(2, rs_lev);
      EXPR_RESCALE_INFO rs_info(node, child, rs_cnt, si);
      ctx->Add_expr_rescale_info(rs_info);
      ctx->Trace(TD_CKKS_SCALE_MGT, std::string(ctx->Indent(), ' '),
                 "Rescale opnd1 of node: ");
      ctx->Trace_obj(TD_CKKS_SCALE_MGT, node);
    }
    ctx->Set_scale_info(child->Id(), si);
  }
}

void PARS::Level_match(NODE_PTR node) {
// In current implementation, waterline is equal to scale factor.
// Only Modswitch is needed to match level of operands of binary operations.
#if 0
  if (node->Opcode() != OPC_ADD && node->Opcode() != OPC_MUL) return;

  NODE_ID                   child0    = node->Child_id(0);
  NODE_ID                   child1    = node->Child_id(1);
  SCALE_MNG_CTX::SCALE_MAP& scale_map = Context()->Node_scale_info();
  SCALE_MAP_ITER            iter0     = scale_map.find(child0.Value());
  if (iter0 == scale_map.end()) return;
  SCALE_MAP_ITER iter1 = scale_map.find(child1.Value());
  if (iter1 == scale_map.end()) return;

  SCALE_INFO& info0   = iter0->second;
  SCALE_INFO& info1   = iter1->second;
  uint32_t    rs_lev0 = info0.Rescale_level();
  uint32_t    rs_lev1 = info1.Rescale_level();
  if (rs_lev0 < rs_lev1) {
    if (info0.Scale_deg() > 1) {
      uint32_t rs_cnt = info0.Scale_deg() - 1;
      rs_lev0 += rs_cnt;
      info0.Set_scale_deg(1);
      info0.Set_rescale_level(rs_lev0);
      EXPR_RESCALE_INFO rs_info(node, node->Child(0), rs_cnt, info0);
      Context()->Add_expr_rescale_info(rs_info);
    } else {
      info0.Set_rescale_level(info0.Rescale_level() + 1);
      info0.Set_scale_deg(info0.Scale_deg() - 1);
    }
  } else if (rs_lev0 > rs_lev1) {
    if (info1.Scale_deg() > 1) {
      uint32_t rs_cnt = info1.Scale_deg() - 1;
      rs_lev1 += rs_cnt;
      info1.Set_scale_deg(1);
      info1.Set_rescale_level(rs_lev1);
      EXPR_RESCALE_INFO rs_info(node, node->Child(1), rs_cnt, info1);
      Context()->Add_expr_rescale_info(rs_info);
    } else {
      info1.Set_rescale_level(info1.Rescale_level() + 1);
      info1.Set_scale_deg(info1.Scale_deg() - 1);
    }
  }
#endif
}

void PARS::Scale_match(NODE_PTR node) {
  if (node->Opcode() != OPC_ADD) return;
  SCALE_MNG_CTX*            ctx       = Context();
  NODE_ID                   child0    = node->Child_id(0);
  NODE_ID                   child1    = node->Child_id(1);
  SCALE_MNG_CTX::SCALE_MAP& scale_map = ctx->Node_scale_info();
  SCALE_MAP_ITER            iter0     = scale_map.find(child0.Value());
  if (iter0 == scale_map.end()) return;
  SCALE_MAP_ITER iter1 = scale_map.find(child1.Value());
  if (iter1 == scale_map.end()) return;
  SCALE_INFO& info0 = iter0->second;
  SCALE_INFO& info1 = iter1->second;
  if (ctx->Is_unfix_scale(info0.Scale_deg())) {
    AIR_ASSERT(!ctx->Is_unfix_scale(info1.Scale_deg()));
    info0 = info1;
  }
  if (info0.Scale_deg() < info1.Scale_deg()) {
    uint32_t rs_cnt   = info1.Scale_deg() - info0.Scale_deg();
    uint32_t rs_level = info1.Rescale_level() + rs_cnt;

    EXPR_RESCALE_INFO rs_info(node, node->Child(1), rs_cnt,
                              SCALE_INFO(info0.Scale_deg(), rs_level));
    ctx->Add_expr_rescale_info(rs_info);
    ctx->Trace(TD_CKKS_SCALE_MGT, std::string(ctx->Indent(), ' '),
               "Rescale opnd1 of add: ");
    ctx->Trace_obj(TD_CKKS_SCALE_MGT, node);
  }
}

SCALE_INFO PARS::Downscale_analysis(NODE_PTR node) {
  SCALE_MNG_CTX*            ctx       = Context();
  NODE_ID                   child0    = node->Child_id(0);
  SCALE_MNG_CTX::SCALE_MAP& scale_map = ctx->Node_scale_info();
  SCALE_MAP_ITER            iter0     = scale_map.find(child0.Value());
  AIR_ASSERT(iter0 != scale_map.end());
  if (node->Opcode() != OPC_MUL) return iter0->second;

  NODE_PTR   child1    = node->Child(1);
  TYPE_ID    rtype     = child1->Rtype_id();
  LOWER_CTX* lower_ctx = ctx->Lower_ctx();
  // at runtime scale of plaintext operand of CKKS.mul is set as scale_factor
  if (!lower_ctx->Is_cipher_type(rtype) && !lower_ctx->Is_cipher3_type(rtype)) {
    return SCALE_INFO(iter0->second.Scale_deg() + 1,
                      iter0->second.Rescale_level());
  }

  SCALE_MAP_ITER iter1 = scale_map.find(child1->Id().Value());
  AIR_ASSERT(iter1 != scale_map.end());

  SCALE_INFO& info0     = iter0->second;
  SCALE_INFO& info1     = iter1->second;
  uint32_t    scale_deg = info0.Scale_deg() + info1.Scale_deg();
  uint32_t    rs_level = std::max(info0.Rescale_level(), info1.Rescale_level());
  if (scale_deg <= 3) return SCALE_INFO(scale_deg, rs_level);

  // rescale operand 0
  info0.Set_scale_deg(info0.Scale_deg() - 1);
  info0.Set_rescale_level(info0.Rescale_level() + 1);
  EXPR_RESCALE_INFO rs_info0(node, node->Child(0), 1, info0);
  ctx->Add_expr_rescale_info(rs_info0);
  // rescale operand 1
  info1.Set_scale_deg(info1.Scale_deg() - 1);
  info1.Set_rescale_level(info1.Rescale_level() + 1);
  EXPR_RESCALE_INFO rs_info1(node, node->Child(1), 1, info1);
  ctx->Add_expr_rescale_info(rs_info1);

  ctx->Trace(TD_CKKS_SCALE_MGT, std::string(ctx->Indent(), ' '),
             "Rescale opnd0 and opnd1 of mul: ");
  ctx->Trace_obj(TD_CKKS_SCALE_MGT, node);
  return SCALE_INFO(scale_deg - 2, rs_level + 1);
}

SCALE_INFO ACE_SM::Handle_mul(NODE_PTR node, SCALE_INFO si0, SCALE_INFO si1) {
  SCALE_MNG_CTX* ctx = Context();
  if (si0.Scale_deg() >= 2) {
    si0 = Rescale_res(node->Child(0), si0);
  }
  if (si1.Scale_deg() >= 2) {
    si1 = Rescale_res(node->Child(1), si1);
  }
  uint32_t   scale_deg = si0.Scale_deg() + si1.Scale_deg();
  uint32_t   rs_level  = std::max(si0.Rescale_level(), si1.Rescale_level());
  SCALE_INFO si(scale_deg, rs_level);
  if (!ctx->Rescale_node(node, ctx->Parent_stmt(), scale_deg)) return si;

  si                       = SCALE_INFO(1, rs_level + 1);
  NODE_PTR          parent = ctx->Parent(1);
  EXPR_RESCALE_INFO rs_info(parent, node, 1, si);
  ctx->Add_expr_rescale_info(rs_info);
  return si;
}

SCALE_INFO ACE_SM::Handle_add(NODE_PTR node, SCALE_INFO si0, SCALE_INFO si1) {
  // scale match
  SCALE_MNG_CTX* ctx = Context();
  if (Context()->Is_unfix_scale(si0.Scale_deg())) {
    // Currently, CKKS.add supports at most one operand with an unfixed scale.
    // This occurs when accumulating ciphertexts, with the sum ciphertext
    // initialized to ZERO. Scale of the sum ciphertext is equal to that of the
    // summed ciphtertexts.
    AIR_ASSERT(!Context()->Is_unfix_scale(si1.Scale_deg()));
    si0 = si1;
  } else if (si0.Scale_deg() > si1.Scale_deg()) {
    AIR_ASSERT(si0.Scale_deg() == (si1.Scale_deg() + 1));
    si0 = Rescale_res(node->Child(0), si0);
    ctx->Trace(TD_CKKS_SCALE_MGT, std::string(ctx->Indent(), ' '),
               "Rescale opnd0 of add: ");
    ctx->Trace_obj(TD_CKKS_SCALE_MGT, node);
  } else if (si0.Scale_deg() < si1.Scale_deg()) {
    AIR_ASSERT(si1.Scale_deg() == (si0.Scale_deg() + 1));
    si1 = Rescale_res(node->Child(1), si1);
    ctx->Trace(TD_CKKS_SCALE_MGT, std::string(ctx->Indent(), ' '),
               "Rescale opnd1 of add: ");
    ctx->Trace_obj(TD_CKKS_SCALE_MGT, node);
  }
  uint32_t rescale_level = std::max(si0.Rescale_level(), si1.Rescale_level());
  return SCALE_INFO(si0.Scale_deg(), rescale_level);
}

SCALE_INFO ACE_SM::Handle_relin(NODE_PTR node, SCALE_INFO si) { return si; }

SCALE_INFO ACE_SM::Handle_rotate(NODE_PTR node, SCALE_INFO si) {
  SCALE_MNG_CTX* ctx = Context();
  // rescale child0 if current rotation occurs out of loop
  if (!ctx->Rescale_node(node, ctx->Parent_stmt(), si.Scale_deg())) return si;

  AIR_ASSERT(si.Scale_deg() == 2);
  NODE_PTR child = node->Child(0);
  si             = Rescale_res(child, si);

  ctx->Trace(TD_CKKS_SCALE_MGT, std::string(ctx->Indent(), ' '),
             "Rescale opnd of rotate: ");
  ctx->Trace_obj(TD_CKKS_SCALE_MGT, node);
  return si;
}

SCALE_INFO ACE_SM::Rescale_res(NODE_PTR node, const SCALE_INFO& si) {
  AIR_ASSERT(si.Scale_deg() >= 1);
  if (si.Scale_deg() == 1) return si;

  uint32_t scale_deg = si.Scale_deg();
  AIR_ASSERT(scale_deg > 1);
  uint32_t          rs_cnt        = scale_deg - 1;
  uint32_t          rescale_level = si.Rescale_level() + rs_cnt;
  SCALE_INFO        res_si(1, rescale_level);
  SCALE_MNG_CTX*    ctx = Context();
  EXPR_RESCALE_INFO rs_info(ctx->Parent(0), node, rs_cnt, res_si);
  ctx->Add_expr_rescale_info(rs_info);

  ctx->Trace(TD_CKKS_SCALE_MGT, std::string(ctx->Indent(), ' '),
             "rescale: s=", res_si.Scale_deg(), " l=", res_si.Rescale_level(),
             "\n");
  return res_si;
}

bool SCALE_MNG_CTX::Rescale_phi_res(air::opt::PHI_NODE_PTR phi,
                                    uint32_t opnd_id, uint32_t scale_deg) {
  if (!Ace_sm()) return false;
  if (!Config()->Rsc_phi_res()) return false;

  if (scale_deg <= 1) return false;

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

void SCALE_MNG_CTX::Process_chi_res(NODE_PTR node, uint32_t scale,
                                    uint32_t level) {
  auto visit = [](air::opt::CHI_NODE_PTR chi, SCALE_MNG_CTX& ctx,
                  uint32_t scale, uint32_t level) {
    air::opt::SSA_VER_ID ver = chi->Result_id();
    AIR_ASSERT(chi->Opnd_id() == chi->Sym()->Zero_ver_id() ||
               SCALE_INFO() == ctx.Get_scale_info(chi->Opnd_id()) ||
               SCALE_INFO(scale, level) == ctx.Get_scale_info(chi->Opnd_id()));

    ctx.Trace(TD_CKKS_SCALE_MGT, "  chi:", chi->To_str(), " s=", scale,
              " l=", level, "\n");

    ctx.Set_scale_info(ver, SCALE_INFO(scale, level));
  };

  air::opt::CHI_NODE_ID chi = Ssa_cntr()->Node_chi(node->Id());
  if (chi != air::base::Null_id) {
    air::opt::CHI_LIST list(Ssa_cntr(), chi);
    list.For_each(visit, *this, scale, level);
  }
}

SCALE_INFO CORE_SCALE_MANAGER::Rescale_prehead_opnd(SCALE_MNG_CTX&        ctx,
                                                    SCALE_INFO            si,
                                                    air::opt::SSA_SYM_PTR sym) {
  if (si.Scale_deg() <= 2) return si;
  AIR_ASSERT(ctx.Pars_rsc());
  STMT_PTR    stmt = ctx.Parent_stmt();
  SPOS        spos = stmt->Spos();
  STMT_LIST   sl(ctx.Parent_block());
  CONTAINER*  cntr   = ctx.Ssa_cntr()->Container();
  FUNC_SCOPE* fs     = ctx.Ssa_cntr()->Func_scope();
  uint32_t    rs_cnt = si.Scale_deg() - 2U;
  NODE_PTR    ld;
  STMT_PTR    st;
  if (sym->Is_preg()) {
    PREG_PTR preg = fs->Preg(PREG_ID(sym->Var_id()));
    NODE_PTR node = cntr->New_ldp(preg, spos);
    for (uint32_t id = 0; id < rs_cnt; ++id) {
      node = CKKS_GEN(cntr, ctx.Lower_ctx()).Gen_rescale(node);
    }
    st = cntr->New_stp(node, preg, spos);
    sl.Prepend(stmt, st);
  } else {
    AIR_ASSERT(sym->Is_addr_datum());
    ADDR_DATUM_PTR datum = fs->Addr_datum(ADDR_DATUM_ID(sym->Var_id()));
    NODE_PTR       node  = cntr->New_ld(datum, stmt->Spos());
    for (uint32_t id = 0; id < rs_cnt; ++id) {
      node = CKKS_GEN(cntr, ctx.Lower_ctx()).Gen_rescale(node);
    }
    st = cntr->New_st(node, datum, spos);
    sl.Prepend(stmt, st);
  }
  ctx.Trace(TD_CKKS_SCALE_MGT, std::string(ctx.Indent(), ' '),
            "Rescale phi opnd from preheader: ");
  ctx.Trace_obj(TD_CKKS_SCALE_MGT, sym);
  ctx.Trace_obj(TD_CKKS_SCALE_MGT, st);
  return SCALE_INFO(2, si.Rescale_level() + rs_cnt);
}

SCALE_INFO CKKS_SCALE_MANAGER::Rescale_res(SCALE_MNG_CTX* ctx, NODE_PTR node,
                                           const SCALE_INFO& si) {
  if (!ctx->Ace_sm() || si.Scale_deg() == 1) return si;

  uint32_t scale_deg = si.Scale_deg();
  AIR_ASSERT(scale_deg > 1);
  uint32_t rs_cnt        = scale_deg - 1;
  uint32_t rescale_level = si.Rescale_level() + rs_cnt;

  SCALE_INFO        res_si(1, rescale_level);
  EXPR_RESCALE_INFO rs_info(ctx->Parent(0), node, rs_cnt, res_si);
  ctx->Add_expr_rescale_info(rs_info);

  ctx->Trace(TD_CKKS_SCALE_MGT, std::string(ctx->Indent(), ' '),
             "rescale: s=", res_si.Scale_deg(), " l=", res_si.Rescale_level(),
             "\n");
  return res_si;
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
  if (ana_ctx->Enc_scl_cst()) {
    TYPE_PTR u32_type  = cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_U32);
    NODE_PTR new_scale = cntr->New_intconst(u32_type, child0_scale, spos);
    if (scale->Opcode() == air::core::OPC_INTCONST &&
        scale->Intconst() != child0_scale) {
      ana_ctx->Trace(TD_CKKS_SCALE_MGT, "Update target scale of encode from ",
                     scale->Intconst(), " to ", child0_scale);
    }
    encode_node->Set_child(scale_child_id, new_scale);
    encode_node->Set_attr(core::FHE_ATTR_KIND::SCALE, &child0_scale, 1);
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
}

void SCALE_MANAGER::Build_ssa() {
  air::opt::SSA_BUILDER ssa_builder(Func_scope(), Ssa_cntr(),
                                    Mng_ctx().Driver_ctx());
  // update SSA_CONFIG
  air::opt::SSA_CONFIG& ssa_config = ssa_builder.Ssa_config();
  ssa_config.Set_trace_ir_before_ssa(
      Mng_ctx().Config()->Is_trace(ckks::TRACE_DETAIL::TD_IR_BEFORE_SSA));
  ssa_config.Set_trace_ir_after_insert_phi(Mng_ctx().Config()->Is_trace(
      ckks::TRACE_DETAIL::TD_IR_AFTER_SSA_INSERT_PHI));
  ssa_config.Set_trace_ir_after_ssa(
      Mng_ctx().Config()->Is_trace(ckks::TRACE_DETAIL::TD_IR_AFTER_SSA));

  ssa_builder.Perform();
}

STMT_PTR SCALE_MANAGER::Gen_rescale(air::opt::SSA_SYM_PTR sym,
                                    const SCALE_INFO& si, SPOS spos) {
  CONTAINER*  cntr = Ssa_cntr()->Container();
  FUNC_SCOPE* fs   = cntr->Parent_func_scope();
  CKKS_GEN    ckks_gen(cntr, Lower_ctx());

  if (sym->Is_preg()) {
    // a preg, generate rescale with ldp/stp directly
    PREG_PTR preg = fs->Preg(PREG_ID(sym->Var_id()));
    NODE_PTR ldp  = cntr->New_ldp(preg, spos);
    Mng_ctx().Set_node_scale_info(ldp, si.Scale_deg() + 1,
                                  si.Rescale_level() - 1);
    NODE_PTR rescale = ckks_gen.Gen_rescale(ldp);
    Mng_ctx().Set_node_scale_info(rescale, si);
    return cntr->New_stp(rescale, preg, spos);
  }

  AIR_ASSERT(sym->Is_addr_datum());
  ADDR_DATUM_PTR datum = fs->Addr_datum(ADDR_DATUM_ID(sym->Var_id()));
  if (!datum->Type()->Is_array()) {
    // a ciphertext, generate rescale with ld/st
    NODE_PTR ld = cntr->New_ld(datum, spos);
    Mng_ctx().Set_node_scale_info(ld, si.Scale_deg() + 1,
                                  si.Rescale_level() - 1);
    NODE_PTR rescale = ckks_gen.Gen_rescale(ld);
    Mng_ctx().Set_node_scale_info(rescale, si);
    return cntr->New_st(rescale, datum, spos);
  }

  // sym is array of ciphertext, either single element or whole array
  AIR_ASSERT(datum->Type()->Cast_to_arr()->Dim() == 1);
  TYPE_PTR elem_type  = datum->Type()->Cast_to_arr()->Elem_type();
  uint64_t elem_count = datum->Type()->Cast_to_arr()->Elem_count();
  AIR_ASSERT(Lower_ctx()->Is_cipher_type(elem_type->Id()) ||
             Lower_ctx()->Is_cipher3_type(elem_type->Id()));
  NODE_PTR       lda   = cntr->New_lda(datum, POINTER_KIND::FLAT64, spos);
  NODE_PTR       array = cntr->New_array(lda, 1, spos);
  ADDR_DATUM_PTR iv;
  TYPE_PTR s32_type = cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_S32);
  if (sym->Index() != air::opt::SSA_SYM::NO_INDEX) {
    // single element
    AIR_ASSERT(sym->Index() < elem_count);
    cntr->Set_array_idx(array, 0,
                        cntr->New_intconst(s32_type, sym->Index(), spos));
  } else {
    // whole array
    iv = fs->New_var(s32_type, "rescale_iv", spos);
    cntr->Set_array_idx(array, 0, cntr->New_ld(iv, spos));
  }
  NODE_PTR ild = cntr->New_ild(array, spos);
  Mng_ctx().Set_node_scale_info(ild, si.Scale_deg() + 1,
                                si.Rescale_level() - 1);
  NODE_PTR rescale = ckks_gen.Gen_rescale(ild);
  Mng_ctx().Set_node_scale_info(rescale, si);
  STMT_PTR rescale_stmt =
      cntr->New_ist(cntr->Clone_node_tree(array), rescale, spos);
  if (sym->Index() == air::opt::SSA_SYM::NO_INDEX) {
    // need a loop to handle each element
    NODE_PTR init = cntr->New_intconst(s32_type, 0, spos);
    NODE_PTR comp = cntr->New_bin_arith(
        air::core::OPC_LT, s32_type, cntr->New_ld(iv, spos),
        cntr->New_intconst(s32_type, elem_count, spos), spos);
    NODE_PTR incr = cntr->New_bin_arith(
        air::core::OPC_ADD, s32_type, cntr->New_ld(iv, spos),
        cntr->New_intconst(s32_type, 1, spos), spos);
    NODE_PTR body = cntr->New_stmt_block(spos);
    STMT_LIST(body).Append(rescale_stmt);
    rescale_stmt = cntr->New_do_loop(iv, init, comp, incr, body, spos);
  }
  return rescale_stmt;
}

void SCALE_MANAGER::Rescale_phi_res() {
  CONTAINER*  cntr       = Ssa_cntr()->Container();
  FUNC_SCOPE* func_scope = cntr->Parent_func_scope();
  CKKS_GEN    ckks_gen(cntr, Lower_ctx());

  // both whole array and individual elements have phi, we always rescale
  // whole array. set up a map to track the sym already emitted
  typedef std::unordered_map<uint64_t, SCALE_INFO> SYM_SI_MAP;
  SYM_SI_MAP                                       sym_si;
  for (air::opt::PHI_NODE_ID phi_id : Mng_ctx().Phi_res_need_rescale()) {
    air::opt::PHI_NODE_PTR phi      = Ssa_cntr()->Phi_node(phi_id);
    STMT_PTR               def_stmt = phi->Def_stmt();
    air::opt::SSA_SYM_PTR  sym      = phi->Sym();
    SCALE_INFO scale_info = Mng_ctx().Get_scale_info(phi->Result_id());

    if (sym->Is_addr_datum()) {
      uint64_t key = ((uint64_t)def_stmt->Id().Value()) << 32 | sym->Var_id();
      SYM_SI_MAP::iterator it = sym_si.find(key);
      if (it != sym_si.end()) {
        // whole array already handled
        AIR_ASSERT(it->second == scale_info);
      } else {
        sym_si[key] = scale_info;
      }
      if (sym->Index() != air::opt::SSA_SYM::NO_INDEX) {
        continue;
      }
    }

    AIR_ASSERT(scale_info.Scale_deg() == 1);
    AIR_ASSERT(scale_info.Rescale_level() >= 1);

    STMT_PTR  rescale_stmt = Gen_rescale(sym, scale_info, def_stmt->Spos());
    STMT_LIST sl(def_stmt->Parent_node());
    sl.Append(phi->Def_stmt(), rescale_stmt);
    Mng_ctx().Trace(TD_CKKS_SCALE_MGT,
                    "\nSCALE_MANAGER::Rescale_phi_res rescale phi result ",
                    sym->To_str(), ":\n");
    Mng_ctx().Trace_obj(TD_CKKS_SCALE_MGT, rescale_stmt);
  }
}

void SCALE_MANAGER::Rescale_expr() {
  uint32_t cnt = 0;
  CKKS_GEN ckks_gen(&Func_scope()->Container(), Lower_ctx());
  for (const EXPR_RESCALE_INFO& rs_info : Mng_ctx().Rescale_expr()) {
    // 1. gen rescale node
    NODE_PTR rescale = rs_info.Node();
    for (uint32_t id = 0; id < rs_info.Rescale_cnt(); ++id) {
      rescale = ckks_gen.Gen_rescale(rescale);
    }
    Mng_ctx().Set_node_scale_info(rescale, rs_info.Res_scale());

    // 2. replace original node with rescale node
    NODE_PTR parent   = rs_info.Parent();
    uint32_t child_id = rs_info.Child_id();
    AIR_ASSERT(child_id < parent->Num_child());
    AIR_ASSERT(parent->Child(child_id) == rs_info.Node());
    parent->Set_child(child_id, rescale);

    Mng_ctx().Trace(TD_CKKS_SCALE_MGT, "\n", ++cnt, ": Rescale ", child_id,
                    "-th operand of:\n");
    Mng_ctx().Trace(TD_CKKS_SCALE_MGT, parent->To_str());
  }
}

}  // namespace ckks
}  // namespace fhe
