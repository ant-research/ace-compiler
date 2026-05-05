//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "h2lpoly.h"

namespace fhe {
namespace poly {
void H2LPOLY::Expand_op_to_rns(POLY_LOWER_CTX& ctx, air::base::NODE_PTR node,
                               CONST_VAR v_res, air::base::NODE_PTR opnd1,
                               air::base::NODE_PTR opnd2, bool is_ext,
                               air::base::STMT_LIST& sl) {
  POLY_IR_GEN&          pgen      = ctx.Poly_gen();
  air::base::CONTAINER* cntr      = pgen.Container();
  CONST_VAR&            v_rns_idx = pgen.Get_var(VAR_RNS_IDX, node->Spos());
  air::base::NODE_PTR   n_res     = pgen.New_var_load(v_res, node->Spos());
  NODE_PAIR             n_blks    = pgen.New_rns_loop(n_res, false);

  air::base::STMT_PTR s_op = Gen_rns_binary_op(
      node->Opcode(), ctx, v_res, v_rns_idx, opnd1, opnd2, sl, node->Spos());
  pgen.Append_rns_stmt(s_op, n_blks.second);
  sl.Append(air::base::STMT_LIST(n_blks.first));

  // Generate p loop
  if (is_ext) {
    NODE_PAIR ext_blks = pgen.New_rns_loop(cntr->Clone_node_tree(n_res), true);

    air::base::STMT_PTR s_pidx = pgen.New_adjust_p_idx(node->Spos());
    pgen.Append_rns_stmt(s_pidx, ext_blks.second);

    CONST_VAR&          v_p_idx  = pgen.Get_var(VAR_P_IDX, node->Spos());
    air::base::STMT_PTR s_ext_op = Gen_rns_binary_op(
        node->Opcode(), ctx, v_res, v_p_idx, cntr->Clone_node_tree(opnd1),
        cntr->Clone_node_tree(opnd2), sl, node->Spos());
    pgen.Append_rns_stmt(s_ext_op, ext_blks.second);
    sl.Append(air::base::STMT_LIST(ext_blks.first));
  }
}

void H2LPOLY::Call_rns_func(POLY_LOWER_CTX& ctx, air::base::NODE_PTR node,
                            air::base::NODE_PTR n_opnd1,
                            air::base::NODE_PTR n_opnd2, bool is_ext) {
  POLY_IR_GEN&           pgen      = ctx.Poly_gen();
  air::base::GLOB_SCOPE* glob      = pgen.Glob_scope();
  air::base::SPOS        spos      = node->Spos();
  core::LOWER_CTX*       lower_ctx = ctx.Lower_ctx();

  core::FHE_FUNC fhe_func_id;
  switch (node->Opcode()) {
    case OPC_ADD:
      fhe_func_id = is_ext ? core::RNS_ADD_EXT : core::RNS_ADD;
      break;
    case OPC_MUL:
      fhe_func_id = is_ext ? core::RNS_MUL_EXT : core::RNS_MUL;
      break;
    case OPC_ROTATE:
      fhe_func_id = is_ext ? core::RNS_ROTATE_EXT : core::RNS_ROTATE;
      break;
    default:
      AIR_ASSERT_MSG(false, "unexpected");
  }

  core::FHE_FUNC_INFO&   func_info = lower_ctx->Get_func_info(fhe_func_id);
  air::base::FUNC_SCOPE* fs        = func_info.Get_func_scope(glob);
  // create function if cache does not exists
  if (fs == nullptr) {
    air::base::FUNC_SCOPE* orig_fs = pgen.Func_scope();
    fs                             = pgen.New_func(fhe_func_id, spos);
    lower_ctx->Set_func_info(fhe_func_id, fs->Owning_func()->Id());

    pgen.Enter_func(fs);
    air::base::CONTAINER* new_cntr = pgen.Container();
    new_cntr->New_func_entry(node->Spos());
    air::base::STMT_LIST sl = new_cntr->Stmt_list();

    // create formal and expand operation in the new function
    VAR                 formal_0(fs, fs->Formal(0));
    VAR                 formal_1(fs, fs->Formal(1));
    VAR                 formal_2(fs, fs->Formal(2));
    air::base::NODE_PTR n_formal_0 = pgen.New_var_load(formal_0, spos);
    air::base::NODE_PTR n_formal_1 = pgen.New_var_load(formal_1, spos);
    air::base::NODE_PTR n_formal_2 = pgen.New_var_load(formal_2, spos);
    Expand_op_to_rns(ctx, node, formal_0, n_formal_1, n_formal_2, is_ext, sl);
    // switch back to orignal function scope
    ctx.Poly_gen().Enter_func(orig_fs);
  }

  // Gen call statment
  air::base::CONTAINER* cntr   = pgen.Container();
  air::base::TYPE_PTR   t_poly = pgen.Lower_ctx()->Get_rns_poly_type(glob);
  air::base::PREG_PTR   retv   = ctx.Poly_gen().Func_scope()->New_preg(t_poly);
  air::base::STMT_PTR   s_call =
      cntr->New_call(fs->Owning_func()->Entry_point(), retv, 3, spos);
  CONST_VAR&          v_res = pgen.Node_var(node);
  air::base::NODE_PTR n_res = pgen.New_var_load(v_res, node->Spos());
  cntr->New_arg(s_call, 0, n_res);  // put res as first parameter
  cntr->New_arg(s_call, 1, n_opnd1);
  cntr->New_arg(s_call, 2, n_opnd2);
  ctx.Prepend(s_call);
}

air::base::STMT_PTR H2LPOLY::Gen_rns_binary_op(
    air::base::OPCODE op, POLY_LOWER_CTX& ctx, CONST_VAR v_res,
    CONST_VAR v_rns_idx, air::base::NODE_PTR opnd1, air::base::NODE_PTR opnd2,
    air::base::STMT_LIST& sl_blk, const air::base::SPOS& spos) {
  POLY_IR_GEN&        pgen      = ctx.Poly_gen();
  CONST_VAR&          v_modulus = pgen.Get_var(VAR_MODULUS, spos);
  air::base::NODE_PTR n_modulus = pgen.New_var_load(v_modulus, spos);
  air::base::NODE_PTR n1_at_l   = pgen.New_poly_load_at_level(opnd1, v_rns_idx);
  air::base::NODE_PTR n_op      = air::base::Null_ptr;
  AIR_ASSERT(op.Domain() == fhe::poly::POLYNOMIAL_DID);

  switch (op) {
    case OPC_ADD: {
      air::base::NODE_PTR n2_at_l =
          pgen.New_poly_load_at_level(opnd2, v_rns_idx);
      n_op = pgen.New_hw_modadd(n1_at_l, n2_at_l, n_modulus, spos);
    } break;
    case OPC_MUL: {
      air::base::NODE_PTR n2_at_l =
          pgen.New_poly_load_at_level(opnd2, v_rns_idx);
      n_op = pgen.New_hw_modmul(n1_at_l, n2_at_l, n_modulus, spos);
    } break;
    case OPC_ROTATE: {
      // generate automorphism orders
      CONST_VAR& v_order = ctx.Poly_gen().Get_var(VAR_AUTO_ORDER, spos);
      air::base::NODE_PTR n_order = ctx.Poly_gen().New_auto_order(opnd2, spos);
      air::base::STMT_PTR s_order =
          ctx.Poly_gen().New_var_store(n_order, v_order, spos);
      sl_blk.Append(s_order);
      air::base::NODE_PTR n_ld_order =
          ctx.Poly_gen().New_var_load(v_order, spos);
      n_op = pgen.New_hw_rotate(n1_at_l, n_ld_order, n_modulus, spos);
    } break;
    default:
      AIR_ASSERT(false);
  }
  air::base::STMT_PTR s_op = pgen.New_poly_store_at_level(
      pgen.New_var_load(v_res, spos), n_op, v_rns_idx);
  return s_op;
}

bool H2LPOLY::Has_ext_attr(POLY_LOWER_CTX& ctx, air::base::NODE_PTR node) {
  bool            is_ext = false;
  const uint32_t* is_ext_ptr =
      node->Attr<uint32_t>(fhe::core::FHE_ATTR_KIND::EXTENDED);
  if (is_ext_ptr != nullptr && *is_ext_ptr != 0) {
    is_ext = true;
  }
  return is_ext;
}

}  // namespace poly
}  // namespace fhe