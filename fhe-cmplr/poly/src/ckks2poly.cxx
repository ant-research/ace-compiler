//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "ckks2poly.h"

namespace fhe {
namespace poly {

bool CKKS2POLY_RETV::Is_null() const {
  switch (Kind()) {
    case RK_DEFAULT:
    case RK_PLAIN_POLY:
    case RK_PLAIN_RNS_POLY:
    case RK_BLOCK:
      return Node1() == air::base::Null_ptr;
    case RK_CIPH_POLY:
    case RK_CIPH_RNS_POLY:
      return (Node1() == air::base::Null_ptr || Node2() == air::base::Null_ptr);
    case RK_CIPH3_POLY:
    case RK_CIPH3_RNS_POLY:
      return (Node1() == air::base::Null_ptr ||
              Node2() == air::base::Null_ptr || Node3() == air::base::Null_ptr);

    default:
      CMPLR_ASSERT(false, "unsupported CKKS2POLY_RETV kind");
  }
  return true;
}

CKKS2POLY_RETV CKKS2POLY_CTX::Handle_lower_retv(NODE_PTR n_node,
                                                NODE_PTR n_parent) {
  SPOS spos = n_node->Spos();
  VAR  v_parent;
  if (n_parent != air::base::Null_ptr && n_parent->Is_st()) {
    if (n_parent->Has_sym()) {
      v_parent.Set_var(Func_scope(), n_parent->Addr_datum());
    } else if (n_parent->Has_preg()) {
      v_parent.Set_var(Func_scope(), n_parent->Preg());
    }
  }
  CONST_VAR& v_node = Poly_gen().Node_var(n_node);

  if (!v_parent.Is_null() && !n_node->Is_ld()) {
    // for v_parent == v_node, already processed in callsite
    CMPLR_ASSERT(v_parent == v_node, "v_parent not equal to v_node");
    return CKKS2POLY_RETV(RETV_KIND::RK_BLOCK, air::base::Null_ptr);
  }

  RETV_KIND retv_kind = Retv_kind(n_parent, v_node.Type_id());
  switch (retv_kind) {
    case RK_CIPH:
    case RK_CIPH3:
    case RK_PLAIN:
      return CKKS2POLY_RETV(retv_kind, Poly_gen().New_var_load(v_node, spos));
    case RK_CIPH_POLY: {
      NODE_PAIR n_pair = Poly_gen().New_ciph_poly_load(v_node, false, spos);
      return CKKS2POLY_RETV(retv_kind, n_pair.first, n_pair.second);
    }
    case RK_CIPH_RNS_POLY: {
      NODE_PAIR n_pair = Poly_gen().New_ciph_poly_load(v_node, true, spos);
      return CKKS2POLY_RETV(retv_kind, n_pair.first, n_pair.second);
    }
    case RK_CIPH3_POLY: {
      NODE_TRIPLE n_tuple = Poly_gen().New_ciph3_poly_load(v_node, false, spos);
      return CKKS2POLY_RETV(retv_kind, std::get<0>(n_tuple),
                            std::get<1>(n_tuple), std::get<2>(n_tuple));
    }
    case RK_CIPH3_RNS_POLY: {
      NODE_TRIPLE n_tuple = Poly_gen().New_ciph3_poly_load(v_node, true, spos);
      return CKKS2POLY_RETV(retv_kind, std::get<0>(n_tuple),
                            std::get<1>(n_tuple), std::get<2>(n_tuple));
    }
    case RK_PLAIN_POLY: {
      NODE_PTR n_plain = Poly_gen().New_plain_poly_load(v_node, false, spos);
      return CKKS2POLY_RETV(retv_kind, n_plain);
    }
    case RK_PLAIN_RNS_POLY: {
      NODE_PTR n_plain = Poly_gen().New_plain_poly_load(v_node, true, spos);
      return CKKS2POLY_RETV(retv_kind, n_plain);
    }
    default:
      CMPLR_ASSERT(false, "unsupported retv_kind");
  }
  return CKKS2POLY_RETV(RETV_KIND::RK_BLOCK, air::base::Null_ptr);
}

CKKS2POLY_RETV CKKS2POLY::Handle_add_ciph(CKKS2POLY_CTX& ctx, NODE_PTR node,
                                          CKKS2POLY_RETV opnd0_pair,
                                          CKKS2POLY_RETV opnd1_pair) {
  CONTAINER* cntr      = ctx.Poly_gen().Container();
  CONST_VAR& v_modulus = ctx.Poly_gen().Get_var(VAR_MODULUS, node->Spos());
  NODE_PTR   new_opnd2 = ctx.Poly_gen().New_var_load(v_modulus, node->Spos());
  CMPLR_ASSERT((!opnd0_pair.Is_null() && !opnd1_pair.Is_null()), "null node");

  NODE_PTR add_0 = ctx.Poly_gen().New_hw_modadd(
      opnd0_pair.Node1(), opnd1_pair.Node1(), new_opnd2, node->Spos());
  NODE_PTR add_1 = ctx.Poly_gen().New_hw_modadd(
      opnd0_pair.Node2(), opnd1_pair.Node2(), new_opnd2, node->Spos());

  return CKKS2POLY_RETV(opnd0_pair.Kind(), add_0, add_1);
}

CKKS2POLY_RETV CKKS2POLY::Handle_add_plain(CKKS2POLY_CTX& ctx, NODE_PTR node,
                                           CKKS2POLY_RETV opnd0_pair,
                                           CKKS2POLY_RETV opnd1_pair) {
  CONTAINER* cntr      = ctx.Poly_gen().Container();
  CONST_VAR& v_modulus = ctx.Poly_gen().Get_var(VAR_MODULUS, node->Spos());
  NODE_PTR   new_opnd2 = ctx.Poly_gen().New_var_load(v_modulus, node->Spos());

  CMPLR_ASSERT((!opnd0_pair.Is_null() && !opnd1_pair.Is_null()), "null node");

  NODE_PTR add_0 = ctx.Poly_gen().New_hw_modadd(
      opnd0_pair.Node1(), opnd1_pair.Node1(), new_opnd2, node->Spos());

  NODE_PTR add_1 = opnd0_pair.Node2();

  return CKKS2POLY_RETV(opnd0_pair.Kind(), add_0, add_1);
}

CKKS2POLY_RETV CKKS2POLY::Handle_add_float(CKKS2POLY_CTX& ctx, NODE_PTR node,
                                           CKKS2POLY_RETV opnd0_pair,
                                           CKKS2POLY_RETV opnd1_pair) {
  CMPLR_ASSERT((!opnd0_pair.Is_null() && !opnd1_pair.Is_null() &&
                opnd0_pair.Kind() == RETV_KIND::RK_CIPH_RNS_POLY &&
                opnd1_pair.Node()->Rtype()->Is_float()),
               "invalid node");

  CONTAINER* cntr = ctx.Poly_gen().Container();
  SPOS       spos = node->Spos();

  NODE_PTR   n_child0  = node->Child(0);
  CONST_VAR& v_child0  = ctx.Poly_gen().Node_var(n_child0);
  CONST_VAR& v_modulus = ctx.Poly_gen().Get_var(VAR_MODULUS, spos);
  NODE_PTR   n_modulus = ctx.Poly_gen().New_var_load(v_modulus, spos);

  // 1. encode float data
  NODE_PTR n_encode =
      Gen_encode_float_from_ciph(ctx, v_child0, opnd1_pair.Node1(), false);
  air::base::ADDR_DATUM_PTR sym = ctx.Poly_gen().New_plain_var(spos);
  CONST_VAR& v_encode = ctx.Poly_gen().Add_node_var(node->Child(1), sym);
  STMT_PTR   s_encode = ctx.Poly_gen().New_var_store(n_encode, v_encode, spos);
  // append to current block, before rns loop
  ctx.Prepend(s_encode);

  // 2. add ciph with encoded float
  NODE_PTR n_plain_at_level =
      ctx.Poly_gen().New_plain_poly_load(v_encode, true, spos);

  NODE_PTR add_0 = ctx.Poly_gen().New_hw_modadd(
      opnd0_pair.Node1(), n_plain_at_level, n_modulus, node->Spos());
  NODE_PTR add_1 = opnd0_pair.Node2();
  return CKKS2POLY_RETV(opnd0_pair.Kind(), add_0, add_1);
}

CKKS2POLY_RETV CKKS2POLY::Handle_mul_ciph(CKKS2POLY_CTX& ctx, NODE_PTR node,
                                          CKKS2POLY_RETV opnd0_pair,
                                          CKKS2POLY_RETV opnd1_pair) {
  CMPLR_ASSERT((!opnd0_pair.Is_null() && !opnd1_pair.Is_null() &&
                opnd0_pair.Kind() == RETV_KIND::RK_CIPH_RNS_POLY &&
                opnd1_pair.Kind() == RETV_KIND::RK_CIPH_RNS_POLY &&
                ctx.Lower_ctx()->Is_cipher3_type(node->Rtype_id())),
               "invalid mul_ciph");

  CONTAINER*  cntr = ctx.Poly_gen().Container();
  GLOB_SCOPE* glob = cntr->Glob_scope();
  SPOS        spos = node->Spos();

  CONST_VAR& v_modulus = ctx.Poly_gen().Get_var(VAR_MODULUS, node->Spos());

  // 1. v_mul_0 = opnd0_c0 * opnd1_c0
  NODE_PTR n_opnd2 = ctx.Poly_gen().New_var_load(v_modulus, node->Spos());
  NODE_PTR n_mul_0 = ctx.Poly_gen().New_hw_modmul(
      opnd0_pair.Node1(), opnd1_pair.Node1(), n_opnd2, node->Spos());

  // 2. v_mul_1 = v_mul_1_0 + v_mul_1_1
  // Use temp variable v_mul_1_0 v_mul_1_1 to store intermediate result
  // v_mul_1_0 = opnd0_c1 * opnd1_c0
  // v_mul_1_1 = opnd0_c0 * opnd1_c0
  CONST_VAR v_mul_1_0(ctx.Func_scope(), ctx.Poly_gen().New_poly_var(spos));
  CONST_VAR v_mul_1_1(ctx.Func_scope(), ctx.Poly_gen().New_poly_var(spos));

  // alocate memory for v_mul_1_0 and v_mul_1_1
  // only need 1 prime to store the temp coeffcients
  NODE_PTR n_alloc_mul_1_0 = ctx.Poly_gen().New_alloc_poly(1, spos);
  NODE_PTR n_alloc_mul_1_1 = ctx.Poly_gen().New_alloc_poly(1, spos);
  STMT_PTR s_alloc_mul_1_0 =
      ctx.Poly_gen().New_var_store(n_alloc_mul_1_0, v_mul_1_0, spos);
  STMT_PTR s_alloc_mul_1_1 =
      ctx.Poly_gen().New_var_store(n_alloc_mul_1_1, v_mul_1_1, spos);
  ctx.Poly_gen().Append_rns_stmt(s_alloc_mul_1_0, ctx.Rns_outer_blk());
  ctx.Poly_gen().Append_rns_stmt(s_alloc_mul_1_1, ctx.Rns_outer_blk());

  NODE_PTR n_zero = cntr->New_intconst(
      glob->Prim_type(air::base::PRIMITIVE_TYPE::INT_U32), 0, spos);
  NODE_PTR n_hw_mul_1_0 = ctx.Poly_gen().New_hw_modmul(
      opnd0_pair.Node1(), opnd1_pair.Node2(), n_opnd2, node->Spos());
  NODE_PTR n_hw_mul_1_1 = ctx.Poly_gen().New_hw_modmul(
      opnd0_pair.Node2(), opnd1_pair.Node1(), n_opnd2, node->Spos());
  NODE_PTR n_mul_1_0 = ctx.Poly_gen().New_var_load(v_mul_1_0, spos);
  NODE_PTR n_mul_1_1 = ctx.Poly_gen().New_var_load(v_mul_1_1, spos);
  STMT_PTR s_mul_1_0 =
      ctx.Poly_gen().New_poly_store_at_level(n_mul_1_0, n_hw_mul_1_0, n_zero);
  ctx.Poly_gen().Append_rns_stmt(s_mul_1_0, ctx.Rns_body_blk());
  STMT_PTR s_mul_1_1 =
      ctx.Poly_gen().New_poly_store_at_level(n_mul_1_1, n_hw_mul_1_1, n_zero);
  ctx.Poly_gen().Append_rns_stmt(s_mul_1_1, ctx.Rns_body_blk());

  STMT_PTR s_free_v_mul_1_0 = ctx.Poly_gen().New_free_poly(v_mul_1_0, spos);
  ctx.Poly_gen().Append_stmt(s_free_v_mul_1_0, ctx.Rns_outer_blk());
  STMT_PTR s_free_v_mul_1_1 = ctx.Poly_gen().New_free_poly(v_mul_1_1, spos);
  ctx.Poly_gen().Append_stmt(s_free_v_mul_1_1, ctx.Rns_outer_blk());

  NODE_PTR n_mul_1_0_at_level =
      ctx.Poly_gen().New_poly_load_at_level(n_mul_1_0, n_zero);
  NODE_PTR n_mul_1_1_at_level =
      ctx.Poly_gen().New_poly_load_at_level(n_mul_1_1, n_zero);
  NODE_PTR n_mul_1 = ctx.Poly_gen().New_hw_modadd(
      n_mul_1_0_at_level, n_mul_1_1_at_level, n_opnd2, node->Spos());

  // 3. v_mul_2 = opnd0_c1 * opnd1_c1
  NODE_PTR n_mul_2 = ctx.Poly_gen().New_hw_modmul(
      opnd0_pair.Node2(), opnd1_pair.Node2(), n_opnd2, node->Spos());
  return CKKS2POLY_RETV(RETV_KIND::RK_CIPH3_RNS_POLY, n_mul_0, n_mul_1,
                        n_mul_2);
}

CKKS2POLY_RETV CKKS2POLY::Handle_mul_plain(CKKS2POLY_CTX& ctx, NODE_PTR node,
                                           CKKS2POLY_RETV opnd0_pair,
                                           CKKS2POLY_RETV opnd1_pair) {
  CMPLR_ASSERT((!opnd0_pair.Is_null() && !opnd1_pair.Is_null() &&
                opnd0_pair.Kind() == RETV_KIND::RK_CIPH_RNS_POLY &&
                opnd1_pair.Kind() == RETV_KIND::RK_PLAIN_RNS_POLY),
               "null node");

  CONTAINER* cntr      = ctx.Poly_gen().Container();
  CONST_VAR& v_modulus = ctx.Poly_gen().Get_var(VAR_MODULUS, node->Spos());
  NODE_PTR   new_opnd2 = ctx.Poly_gen().New_var_load(v_modulus, node->Spos());

  // TODO: shall we share the nodes in different op?
  NODE_PTR mul_0 = ctx.Poly_gen().New_hw_modmul(
      opnd0_pair.Node1(), opnd1_pair.Node1(), new_opnd2, node->Spos());
  NODE_PTR mul_1 = ctx.Poly_gen().New_hw_modmul(
      opnd0_pair.Node2(), opnd1_pair.Node1(), new_opnd2, node->Spos());

  return CKKS2POLY_RETV(RETV_KIND::RK_CIPH_RNS_POLY, mul_0, mul_1);
}

CKKS2POLY_RETV CKKS2POLY::Handle_mul_float(CKKS2POLY_CTX& ctx, NODE_PTR node,
                                           CKKS2POLY_RETV opnd0_pair,
                                           CKKS2POLY_RETV opnd1_pair) {
  CMPLR_ASSERT((!opnd0_pair.Is_null() && !opnd1_pair.Is_null() &&
                opnd0_pair.Kind() == RETV_KIND::RK_CIPH_RNS_POLY &&
                opnd1_pair.Node()->Rtype()->Is_float()),
               "invalid node");

  CONTAINER* cntr = ctx.Poly_gen().Container();
  SPOS       spos = node->Spos();

  NODE_PTR   n_child0  = node->Child(0);
  CONST_VAR& v_child0  = ctx.Poly_gen().Node_var(n_child0);
  CONST_VAR& v_modulus = ctx.Poly_gen().Get_var(VAR_MODULUS, spos);
  NODE_PTR   n_modulus = ctx.Poly_gen().New_var_load(v_modulus, spos);

  NODE_PTR n_encode =
      Gen_encode_float_from_ciph(ctx, v_child0, opnd1_pair.Node1(), true);
  air::base::ADDR_DATUM_PTR sym = ctx.Poly_gen().New_plain_var(spos);
  CONST_VAR& v_encode = ctx.Poly_gen().Add_node_var(node->Child(1), sym);
  STMT_PTR   s_encode = ctx.Poly_gen().New_var_store(n_encode, v_encode, spos);
  // append to current block, before rns loop
  ctx.Prepend(s_encode);

  // 2. mul ciph with encoded float
  NODE_PTR n_plain_at_level =
      ctx.Poly_gen().New_plain_poly_load(v_encode, true, spos);
  NODE_PTR mul_0 = ctx.Poly_gen().New_hw_modmul(
      opnd0_pair.Node1(), n_plain_at_level, n_modulus, spos);
  NODE_PTR mul_1 = ctx.Poly_gen().New_hw_modmul(
      opnd0_pair.Node2(), n_plain_at_level, n_modulus, spos);

  return CKKS2POLY_RETV(RETV_KIND::RK_CIPH_RNS_POLY, mul_0, mul_1);
}

NODE_PTR CKKS2POLY::Expand_rotate(CKKS2POLY_CTX& ctx, NODE_PTR node,
                                  NODE_PTR n_c0, NODE_PTR n_c1,
                                  NODE_PTR n_rot_idx, const SPOS& spos) {
  // 1. generate a new block to put all statements together
  CONTAINER*  cntr        = ctx.Poly_gen().Container();
  GLOB_SCOPE* glob        = ctx.Poly_gen().Glob_scope();
  NODE_PTR    n_outer_blk = cntr->New_stmt_block(spos);
  STMT_LIST   sl_outer    = STMT_LIST::Enclosing_list(n_outer_blk->End_stmt());
  CONST_VAR&  v_rot_res =
      ctx.Config().Inline_rotate()
           ? ctx.Poly_gen().Node_var(node)
           : ctx.Poly_gen().Get_var(POLY_PREDEF_VAR::VAR_ROT_RES, spos);

  // gen init for rotate result
  if (ctx.Config().Inline_rotate()) {
    STMT_PTR s_init = ctx.Poly_gen().New_init_ciph(v_rot_res, node);
    ctx.Prepend(s_init);
  } else {
    // init from formals
    NODE_PTR n_res   = ctx.Poly_gen().New_var_load(v_rot_res, spos);
    NODE_PTR n_opnd0 = cntr->New_ld(n_c0->Addr_datum(), spos);
    TYPE_PTR t_opnd1 = glob->New_ptr_type(
        glob->Prim_type(air::base::PRIMITIVE_TYPE::INT_U64)->Id(),
        air::base::POINTER_KIND::FLAT64);
    NODE_PTR n_opnd1 = cntr->New_zero(t_opnd1, spos);
    STMT_PTR s_init =
        ctx.Poly_gen().New_init_ciph_same_scale(n_res, n_opnd0, n_opnd1, spos);
    cntr->Stmt_list().Append(s_init);
  }

  // 2. alloc POLYS
  Handle_kswitch_alloc(ctx, sl_outer, n_c0, n_c1, spos);

  // 3. get switch key with given index
  NODE_PTR   n_swk = ctx.Poly_gen().New_swk(true, spos, n_rot_idx);
  CONST_VAR& v_swk = ctx.Poly_gen().Get_var(VAR_SWK, spos);
  STMT_PTR   s_swk = ctx.Poly_gen().New_var_store(n_swk, v_swk, spos);
  sl_outer.Append(s_swk);

  // 4. generate decompose loop to iterate all q parts and perform keyswitch
  Handle_kswitch(ctx, sl_outer, v_swk, n_c1, spos);

  // 5. mod_down
  Handle_mod_down(ctx, sl_outer, spos);

  // 6. post keyswitch for rotate: add(c0, mod_down_c0)
  Handle_rotate_post_keyswitch(ctx, sl_outer, n_c0, spos);

  // 7. automorphism polys
  Handle_automorphism(ctx, sl_outer, v_rot_res, n_rot_idx, spos);

  // 8. free memory
  Handle_kswitch_free(ctx, sl_outer, spos);

  if (!ctx.Config().Inline_rotate()) {
    NODE_PTR n_retv = ctx.Poly_gen().New_var_load(v_rot_res, spos);
    STMT_PTR s_retv = cntr->New_retv(n_retv, spos);
    ctx.Poly_gen().Append_stmt(s_retv, n_outer_blk);
  }
  return n_outer_blk;
}

void CKKS2POLY::Gen_rotate_func(CKKS2POLY_CTX& ctx, NODE_PTR node,
                                const SPOS& spos) {
  GLOB_SCOPE* glob   = ctx.Poly_gen().Glob_scope();
  TYPE_PTR    t_ciph = ctx.Poly_gen().Get_type(VAR_TYPE_KIND::CIPH, spos);

  if (ctx.Poly_gen().Rotate_entry() == air::base::Null_ptr) {
    FUNC_SCOPE* orig_fs = ctx.Poly_gen().Func_scope();
    FUNC_SCOPE* fs      = ctx.Poly_gen().New_rotate_func();
    ctx.Poly_gen().Enter_func(fs);
    CONTAINER* cntr = ctx.Poly_gen().Container();
    cntr->New_func_entry(spos);

    // create formals and expand rotate operations to the new rotate function
    CONST_VAR f_rot_opnd0(fs, fs->Formal(0));
    CONST_VAR f_rot_opnd1(fs, fs->Formal(1));
    NODE_PAIR opnd0_pair =
        ctx.Poly_gen().New_ciph_poly_load(f_rot_opnd0, false, spos);
    NODE_PTR n_opnd1    = ctx.Poly_gen().New_var_load(f_rot_opnd1, spos);
    NODE_PTR parent_blk = cntr->Stmt_list().Block_node();
    NODE_PTR n_exp_blk  = Expand_rotate(ctx, node, opnd0_pair.first,
                                        opnd0_pair.second, n_opnd1, spos);
    ctx.Poly_gen().Append_stmt(n_exp_blk->Stmt(), parent_blk);

    // switch back to orignal function scope
    ctx.Poly_gen().Enter_func(orig_fs);
  }
}

void CKKS2POLY::Call_rotate(CKKS2POLY_CTX& ctx, NODE_PTR node, NODE_PTR n_arg0,
                            NODE_PTR n_arg1) {
  CONTAINER* cntr = ctx.Poly_gen().Container();
  SPOS       spos = node->Spos();

  // Generate rotate function
  Gen_rotate_func(ctx, node, spos);

  // Call the rotate function and process return value
  TYPE_PTR t_ciph = ctx.Poly_gen().Get_type(VAR_TYPE_KIND::CIPH, spos);
  PREG_PTR retv   = air::base::Null_ptr;
  VAR      v_rot_res;
  if (ctx.Poly_gen().Has_node_var(node)) {
    v_rot_res = ctx.Poly_gen().Node_var(node);
    // reuse v_rot_res as call return if it is a preg
    retv = v_rot_res.Is_preg() && ctx.Config().Reuse_preg_as_retv()
               ? v_rot_res.Preg_var()
               : ctx.Poly_gen().Func_scope()->New_preg(t_ciph);
  } else {
    // use retv preg as the node result symbol
    retv = ctx.Poly_gen().Func_scope()->New_preg(t_ciph);
    ctx.Poly_gen().Add_node_var(node, retv);
  }
  STMT_PTR s_call =
      cntr->New_call(ctx.Poly_gen().Rotate_entry(), retv, 2, spos);
  cntr->New_arg(s_call, 0, n_arg0);
  cntr->New_arg(s_call, 1, n_arg1);
  ctx.Prepend(s_call);

  CONST_VAR v_retv(ctx.Func_scope(), retv);
  if (!v_rot_res.Is_null() && v_retv != v_rot_res) {
    // copy the data from preg to store symbol
    STMT_PTR s_res = ctx.Poly_gen().New_var_store(cntr->New_ldp(retv, spos),
                                                  v_rot_res, spos);
    ctx.Prepend(s_res);
  }
}

void CKKS2POLY::Handle_kswitch_alloc(CKKS2POLY_CTX& ctx, STMT_LIST& sl,
                                     NODE_PTR n_c0, NODE_PTR n_c1,
                                     const SPOS& spos) {
  CONTAINER* cntr           = ctx.Poly_gen().Container();
  NODE_PTR   n_alloc_swk_c0 = ctx.Poly_gen().New_alloc_poly(n_c1, true, spos);
  NODE_PTR   n_alloc_swk_c1 = ctx.Poly_gen().New_alloc_poly(n_c1, true, spos);
  NODE_PTR   n_alloc_ext    = ctx.Poly_gen().New_alloc_poly(n_c1, true, spos);
  NODE_PTR   n_alloc_mod_down_c0 =
      ctx.Poly_gen().New_alloc_poly(n_c0, false, spos);
  NODE_PTR n_alloc_mod_down_c1 =
      ctx.Poly_gen().New_alloc_poly(n_c1, false, spos);
  NODE_PTR n_alloc_decomp = ctx.Poly_gen().New_alloc_poly(n_c0, false, spos);
  NODE_PTR n_alloc_tmp    = ctx.Poly_gen().New_alloc_poly(1, spos);

  CONST_VAR& v_swk_c0      = ctx.Poly_gen().Get_var(VAR_SWK_C0, spos);
  CONST_VAR& v_swk_c1      = ctx.Poly_gen().Get_var(VAR_SWK_C1, spos);
  CONST_VAR& v_c1_ext      = ctx.Poly_gen().Get_var(VAR_EXT, spos);
  CONST_VAR& v_tmp         = ctx.Poly_gen().Get_var(VAR_TMP_POLY, spos);
  CONST_VAR& v_mod_down_c0 = ctx.Poly_gen().Get_var(VAR_MOD_DOWN_C0, spos);
  CONST_VAR& v_mod_down_c1 = ctx.Poly_gen().Get_var(VAR_MOD_DOWN_C1, spos);
  STMT_PTR   s_alloc_swk_c0 =
      ctx.Poly_gen().New_var_store(n_alloc_swk_c0, v_swk_c0, spos);
  STMT_PTR s_alloc_swk_c1 =
      ctx.Poly_gen().New_var_store(n_alloc_swk_c1, v_swk_c1, spos);
  STMT_PTR s_c1_ext = ctx.Poly_gen().New_var_store(n_alloc_ext, v_c1_ext, spos);
  STMT_PTR s_alloc_tmp = ctx.Poly_gen().New_var_store(n_alloc_tmp, v_tmp, spos);
  STMT_PTR s_alloc_mod_down_c0 =
      ctx.Poly_gen().New_var_store(n_alloc_mod_down_c0, v_mod_down_c0, spos);
  STMT_PTR s_alloc_mod_down_c1 =
      ctx.Poly_gen().New_var_store(n_alloc_mod_down_c1, v_mod_down_c1, spos);
  sl.Append(s_alloc_swk_c0);
  sl.Append(s_alloc_swk_c1);
  sl.Append(s_c1_ext);
  sl.Append(s_alloc_tmp);
  sl.Append(s_alloc_mod_down_c0);
  sl.Append(s_alloc_mod_down_c1);
  if (!ctx.Config().Fuse_decomp_modup()) {
    CONST_VAR& v_decomp = ctx.Poly_gen().Get_var(VAR_DECOMP, spos);
    STMT_PTR   s_decomp =
        ctx.Poly_gen().New_var_store(n_alloc_decomp, v_decomp, spos);
    sl.Append(s_decomp);
  }
}

void CKKS2POLY::Handle_kswitch_free(CKKS2POLY_CTX& ctx, STMT_LIST& sl,
                                    const SPOS& spos) {
  CONST_VAR& v_swk_c0      = ctx.Poly_gen().Get_var(VAR_SWK_C0, spos);
  CONST_VAR& v_swk_c1      = ctx.Poly_gen().Get_var(VAR_SWK_C1, spos);
  CONST_VAR& v_c1_ext      = ctx.Poly_gen().Get_var(VAR_EXT, spos);
  CONST_VAR& v_tmp         = ctx.Poly_gen().Get_var(VAR_TMP_POLY, spos);
  CONST_VAR& v_mod_down_c0 = ctx.Poly_gen().Get_var(VAR_MOD_DOWN_C0, spos);
  CONST_VAR& v_mod_down_c1 = ctx.Poly_gen().Get_var(VAR_MOD_DOWN_C1, spos);
  STMT_PTR   s_free_swk_c0 = ctx.Poly_gen().New_free_poly(v_swk_c0, spos);
  STMT_PTR   s_free_swk_c1 = ctx.Poly_gen().New_free_poly(v_swk_c1, spos);
  STMT_PTR   s_free_ext    = ctx.Poly_gen().New_free_poly(v_c1_ext, spos);
  STMT_PTR   s_free_tmp    = ctx.Poly_gen().New_free_poly(v_tmp, spos);
  STMT_PTR   s_free_mod_down_c0 =
      ctx.Poly_gen().New_free_poly(v_mod_down_c0, spos);
  STMT_PTR s_free_mod_down_c1 =
      ctx.Poly_gen().New_free_poly(v_mod_down_c1, spos);
  sl.Append(s_free_swk_c0);
  sl.Append(s_free_swk_c1);
  sl.Append(s_free_ext);
  sl.Append(s_free_tmp);
  sl.Append(s_free_mod_down_c0);
  sl.Append(s_free_mod_down_c1);

  if (!ctx.Config().Fuse_decomp_modup()) {
    CONST_VAR& v_decomp = ctx.Poly_gen().Get_var(VAR_DECOMP, spos);
    STMT_PTR   s_decomp = ctx.Poly_gen().New_free_poly(v_decomp, spos);
    sl.Append(s_decomp);
  }
}

void CKKS2POLY::Handle_kswitch(CKKS2POLY_CTX& ctx, STMT_LIST& sl,
                               CONST_VAR v_key, NODE_PTR n_c1,
                               const SPOS& spos) {
  CONTAINER*            cntr = ctx.Poly_gen().Container();
  std::vector<STMT_PTR> body_stmts;
  CONST_VAR&            v_c1_ext   = ctx.Poly_gen().Get_var(VAR_EXT, spos);
  CONST_VAR&            v_part_idx = ctx.Poly_gen().Get_var(VAR_PART_IDX, spos);
  CONST_VAR&            v_swk_c0   = ctx.Poly_gen().Get_var(VAR_SWK_C0, spos);
  CONST_VAR&            v_swk_c1   = ctx.Poly_gen().Get_var(VAR_SWK_C1, spos);
  CONST_VAR&            v_key0     = ctx.Poly_gen().Get_var(VAR_PUB_KEY0, spos);
  CONST_VAR&            v_key1     = ctx.Poly_gen().Get_var(VAR_PUB_KEY1, spos);

  if (ctx.Config().Fuse_decomp_modup()) {
    NODE_PTR n_decomp_modup =
        ctx.Poly_gen().New_decomp_modup(n_c1, v_part_idx, spos);
    STMT_PTR s_modup =
        ctx.Poly_gen().New_var_store(n_decomp_modup, v_c1_ext, spos);
    body_stmts.push_back(s_modup);
  } else {
    // decomp
    CONST_VAR& v_decomp = ctx.Poly_gen().Get_var(VAR_DECOMP, spos);
    NODE_PTR   n_decomp = ctx.Poly_gen().New_decomp(n_c1, v_part_idx, spos);
    STMT_PTR s_decomp = ctx.Poly_gen().New_var_store(n_decomp, v_decomp, spos);
    body_stmts.push_back(s_decomp);

    // modup
    n_decomp         = ctx.Poly_gen().New_var_load(v_decomp, spos);
    NODE_PTR n_modup = ctx.Poly_gen().New_mod_up(n_decomp, v_part_idx, spos);
    STMT_PTR s_modup = ctx.Poly_gen().New_var_store(n_modup, v_c1_ext, spos);
    body_stmts.push_back(s_modup);
  }

  // keyswitch
  NODE_PTR n_key0 = ctx.Poly_gen().New_pk0_at(v_key, v_part_idx, spos);
  NODE_PTR n_key1 = ctx.Poly_gen().New_pk1_at(v_key, v_part_idx, spos);
  STMT_PTR s_key0 = ctx.Poly_gen().New_var_store(n_key0, v_key0, spos);
  STMT_PTR s_key1 = ctx.Poly_gen().New_var_store(n_key1, v_key1, spos);
  body_stmts.push_back(s_key0);
  body_stmts.push_back(s_key1);

  NODE_PTR n_ksw_blk1 = ctx.Poly_gen().New_key_switch(
      v_swk_c0, v_swk_c1, v_c1_ext, v_key0, v_key1, spos, false);
  NODE_PTR n_ksw_blk2 = ctx.Poly_gen().New_key_switch(
      v_swk_c0, v_swk_c1, v_c1_ext, v_key0, v_key1, spos, true);
  body_stmts.push_back(n_ksw_blk1->Stmt());
  body_stmts.push_back(n_ksw_blk2->Stmt());

  // gen decomp loop
  NODE_PTR n_loop1 = ctx.Poly_gen().New_decomp_loop(n_c1, body_stmts, spos);
  sl.Append(n_loop1->Stmt());
}

void CKKS2POLY::Handle_mod_down(CKKS2POLY_CTX& ctx, STMT_LIST& sl,
                                const SPOS& spos) {
  CONTAINER* cntr          = ctx.Poly_gen().Container();
  CONST_VAR& v_swk_c0      = ctx.Poly_gen().Get_var(VAR_SWK_C0, spos);
  CONST_VAR& v_swk_c1      = ctx.Poly_gen().Get_var(VAR_SWK_C1, spos);
  CONST_VAR& v_mod_down_c0 = ctx.Poly_gen().Get_var(VAR_MOD_DOWN_C0, spos);
  CONST_VAR& v_mod_down_c1 = ctx.Poly_gen().Get_var(VAR_MOD_DOWN_C1, spos);

  // v_mod_down_c0 = mod_down(v_swk_c0)
  NODE_PTR n_swk_c0      = ctx.Poly_gen().New_var_load(v_swk_c0, spos);
  NODE_PTR n_mod_down_c0 = ctx.Poly_gen().New_mod_down(n_swk_c0, spos);
  STMT_PTR s_mod_down_c0 =
      ctx.Poly_gen().New_var_store(n_mod_down_c0, v_mod_down_c0, spos);

  // v_mod_down_c1 = mod_down(v_swk_c1)
  NODE_PTR n_swk_c1      = ctx.Poly_gen().New_var_load(v_swk_c1, spos);
  NODE_PTR n_mod_down_c1 = ctx.Poly_gen().New_mod_down(n_swk_c1, spos);
  STMT_PTR s_mod_down_c1 =
      ctx.Poly_gen().New_var_store(n_mod_down_c1, v_mod_down_c1, spos);

  sl.Append(s_mod_down_c0);
  sl.Append(s_mod_down_c1);
}

void CKKS2POLY::Handle_rotate_post_keyswitch(CKKS2POLY_CTX& ctx, STMT_LIST& sl,
                                             NODE_PTR n_c0, const SPOS& spos) {
  CONTAINER* cntr = ctx.Poly_gen().Container();

  CONST_VAR& v_mod_down_c0 = ctx.Poly_gen().Get_var(VAR_MOD_DOWN_C0, spos);
  CONST_VAR& v_mod_down_c1 = ctx.Poly_gen().Get_var(VAR_MOD_DOWN_C1, spos);
  CONST_VAR& v_rns_idx     = ctx.Poly_gen().Get_var(VAR_RNS_IDX, spos);
  CONST_VAR& v_modulus     = ctx.Poly_gen().Get_var(VAR_MODULUS, spos);

  // v_mod_down_c0 = v_mod_down_c0 + v_c0
  NODE_PTR n_mod_down_c0 = ctx.Poly_gen().New_var_load(v_mod_down_c0, spos);
  NODE_PTR n_mod_down_c0_at_level =
      ctx.Poly_gen().New_poly_load_at_level(n_mod_down_c0, v_rns_idx);
  NODE_PTR n_c0_at_level =
      ctx.Poly_gen().New_poly_load_at_level(n_c0, v_rns_idx);
  NODE_PTR n_modulus = ctx.Poly_gen().New_var_load(v_modulus, spos);
  NODE_PTR n_mod_add = ctx.Poly_gen().New_hw_modadd(
      n_mod_down_c0_at_level, n_c0_at_level, n_modulus, spos);
  // reuse n_mod_down_c0 node or create a new load
  STMT_PTR s_mod_down_c0 = ctx.Poly_gen().New_poly_store_at_level(
      n_mod_down_c0, n_mod_add, v_rns_idx);

  NODE_PAIR n_blks = ctx.Poly_gen().New_rns_loop(n_mod_down_c0, false);
  ctx.Poly_gen().Append_rns_stmt(s_mod_down_c0, n_blks.second);
  sl.Append(n_blks.first->Stmt());
}

void CKKS2POLY::Handle_automorphism(CKKS2POLY_CTX& ctx, STMT_LIST& sl,
                                    CONST_VAR v_rot_res, NODE_PTR n_rot_idx,
                                    const SPOS& spos) {
  CONTAINER* cntr = ctx.Poly_gen().Container();

  CONST_VAR& v_order       = ctx.Poly_gen().Get_var(VAR_AUTO_ORDER, spos);
  CONST_VAR& v_mod_down_c0 = ctx.Poly_gen().Get_var(VAR_MOD_DOWN_C0, spos);
  CONST_VAR& v_mod_down_c1 = ctx.Poly_gen().Get_var(VAR_MOD_DOWN_C1, spos);
  CONST_VAR& v_rns_idx     = ctx.Poly_gen().Get_var(VAR_RNS_IDX, spos);
  CONST_VAR& v_modulus     = ctx.Poly_gen().Get_var(VAR_MODULUS, spos);

  // generate automorphism orders
  NODE_PTR n_order = ctx.Poly_gen().New_auto_order(n_rot_idx, spos);
  STMT_PTR s_order = ctx.Poly_gen().New_var_store(n_order, v_order, spos);
  sl.Append(s_order);

  // generate automorphism loops
  NODE_PTR  n_mod_down_c0 = ctx.Poly_gen().New_var_load(v_mod_down_c0, spos);
  NODE_PAIR n_blks        = ctx.Poly_gen().New_rns_loop(n_mod_down_c0, false);

  // generate loop body statment
  NODE_PTR n_mod_down_c0_at_level =
      ctx.Poly_gen().New_poly_load_at_level(n_mod_down_c0, v_rns_idx);
  NODE_PTR n_mod_down_c1 = ctx.Poly_gen().New_var_load(v_mod_down_c1, spos);
  NODE_PTR n_mod_down_c1_at_level =
      ctx.Poly_gen().New_poly_load_at_level(n_mod_down_c1, v_rns_idx);
  NODE_PTR n_modulus  = ctx.Poly_gen().New_var_load(v_modulus, spos);
  NODE_PTR n_ld_order = ctx.Poly_gen().New_var_load(v_order, spos);

  NODE_PTR n_automorph_c0 = ctx.Poly_gen().New_hw_rotate(
      n_mod_down_c0_at_level, n_ld_order, n_modulus, spos);
  NODE_PTR n_automorph_c1 = ctx.Poly_gen().New_hw_rotate(
      n_mod_down_c1_at_level, n_ld_order, n_modulus, spos);

  STMT_PAIR s_rot_pair = ctx.Poly_gen().New_ciph_poly_store(
      v_rot_res, n_automorph_c0, n_automorph_c1, true, spos);

  ctx.Poly_gen().Append_rns_stmt(s_rot_pair.first, n_blks.second);
  ctx.Poly_gen().Append_rns_stmt(s_rot_pair.second, n_blks.second);
  sl.Append(n_blks.first->Stmt());
}

bool CKKS2POLY::Is_gen_rns_loop(NODE_PTR parent, NODE_PTR node) {
  if (node->Domain() != fhe::ckks::CKKS_DOMAIN::ID) return false;

  // for add/sub/mul, if parent node is store/rotate/rescale/relin,
  // generate a new rns loop to perform coefficient operations
  switch (node->Operator()) {
    case fhe::ckks::CKKS_OPERATOR::ADD:
    case fhe::ckks::CKKS_OPERATOR::SUB:
    case fhe::ckks::CKKS_OPERATOR::MUL:
      if (parent == air::base::Null_ptr) {
        return true;
      }
      air::base::OPCODE p_opcode = parent->Opcode();
      if (p_opcode == air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                        fhe::ckks::CKKS_OPERATOR::ROTATE) ||
          p_opcode == air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                        fhe::ckks::CKKS_OPERATOR::RESCALE) ||
          p_opcode == air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                        fhe::ckks::CKKS_OPERATOR::RELIN) ||
          parent->Is_st()) {
        return true;
      }
      break;
  }
  return false;
}

void CKKS2POLY::Handle_relin_post_keyswitch(CKKS2POLY_CTX& ctx, STMT_LIST& sl,
                                            CONST_VAR v_relin_res,
                                            NODE_PTR n_c0, NODE_PTR n_c1,
                                            const SPOS& spos) {
  CONTAINER* cntr = ctx.Poly_gen().Container();

  CONST_VAR& v_mod_down_c0 = ctx.Poly_gen().Get_var(VAR_MOD_DOWN_C0, spos);
  CONST_VAR& v_mod_down_c1 = ctx.Poly_gen().Get_var(VAR_MOD_DOWN_C1, spos);
  CONST_VAR& v_rns_idx     = ctx.Poly_gen().Get_var(VAR_RNS_IDX, spos);
  CONST_VAR& v_modulus     = ctx.Poly_gen().Get_var(VAR_MODULUS, spos);

  // v_relin_c0 = v_mod_down_c0 + v_c0
  NODE_PTR n_mod_down_c0 = ctx.Poly_gen().New_var_load(v_mod_down_c0, spos);
  NODE_PTR n_mod_down_c0_at_level =
      ctx.Poly_gen().New_poly_load_at_level(n_mod_down_c0, v_rns_idx);
  NODE_PTR n_c0_at_level =
      ctx.Poly_gen().New_poly_load_at_level(n_c0, v_rns_idx);
  NODE_PTR n_modulus    = ctx.Poly_gen().New_var_load(v_modulus, spos);
  NODE_PTR n_mod_add_c0 = ctx.Poly_gen().New_hw_modadd(
      n_mod_down_c0_at_level, n_c0_at_level, n_modulus, spos);

  // v_relin_c1 = v_mod_down_c1 + v_c1
  NODE_PTR n_mod_down_c1 = ctx.Poly_gen().New_var_load(v_mod_down_c1, spos);
  NODE_PTR n_mod_down_c1_at_level =
      ctx.Poly_gen().New_poly_load_at_level(n_mod_down_c1, v_rns_idx);
  NODE_PTR n_c1_at_level =
      ctx.Poly_gen().New_poly_load_at_level(n_c1, v_rns_idx);
  NODE_PTR n_mod_add_c1 = ctx.Poly_gen().New_hw_modadd(
      n_mod_down_c1_at_level, n_c1_at_level, n_modulus, spos);

  STMT_PAIR s_relin = ctx.Poly_gen().New_ciph_poly_store(
      v_relin_res, n_mod_add_c0, n_mod_add_c1, true, spos);

  NODE_PAIR n_blks = ctx.Poly_gen().New_rns_loop(n_c0, false);
  ctx.Poly_gen().Append_rns_stmt(s_relin.first, n_blks.second);
  ctx.Poly_gen().Append_rns_stmt(s_relin.second, n_blks.second);
  sl.Append(n_blks.first->Stmt());
}

NODE_PTR CKKS2POLY::Expand_relin(CKKS2POLY_CTX& ctx, NODE_PTR node,
                                 NODE_PTR n_c0, NODE_PTR n_c1, NODE_PTR n_c2) {
  // 1. generate a new block to put all statements together
  CONTAINER*  cntr        = ctx.Poly_gen().Container();
  GLOB_SCOPE* glob        = ctx.Poly_gen().Glob_scope();
  SPOS        spos        = node->Spos();
  NODE_PTR    n_outer_blk = cntr->New_stmt_block(spos);
  STMT_LIST   sl_outer    = STMT_LIST::Enclosing_list(n_outer_blk->End_stmt());
  CONST_VAR&  v_relin_res =
      ctx.Config().Inline_relin()
           ? ctx.Poly_gen().Node_var(node)
           : ctx.Poly_gen().Get_var(POLY_PREDEF_VAR::VAR_RELIN_RES, spos);

  // gen init for relinearize result
  if (ctx.Config().Inline_relin()) {
    STMT_PTR s_init = ctx.Poly_gen().New_init_ciph(v_relin_res, node);
    ctx.Prepend(s_init);
  } else {
    // init from formals
    NODE_PTR n_res   = ctx.Poly_gen().New_var_load(v_relin_res, spos);
    NODE_PTR n_opnd0 = cntr->New_ld(n_c0->Addr_datum(), spos);
    TYPE_PTR t_opnd1 = glob->New_ptr_type(
        glob->Prim_type(air::base::PRIMITIVE_TYPE::INT_U64)->Id(),
        air::base::POINTER_KIND::FLAT64);
    NODE_PTR n_opnd1 = cntr->New_zero(t_opnd1, spos);
    STMT_PTR s_init =
        ctx.Poly_gen().New_init_ciph_same_scale(n_res, n_opnd0, n_opnd1, spos);
    cntr->Stmt_list().Append(s_init);
  }

  // 2. alloc POLYS
  Handle_kswitch_alloc(ctx, sl_outer, n_c0, n_c2, spos);

  // 3. get switch key with given index
  NODE_PTR   n_swk = ctx.Poly_gen().New_swk(false, spos);
  CONST_VAR& v_swk = ctx.Poly_gen().Get_var(VAR_SWK, spos);
  STMT_PTR   s_swk = ctx.Poly_gen().New_var_store(n_swk, v_swk, spos);
  sl_outer.Append(s_swk);

  // 4. generate decompose loop to iterate all q parts and perform keyswitch
  Handle_kswitch(ctx, sl_outer, v_swk, n_c2, spos);

  // 5. mod_down
  Handle_mod_down(ctx, sl_outer, spos);

  // 6. post keyswitch for relin
  // relin_c0 = mod_down_c0 + mul_c0
  // relin_c1 = mod_down_c1 + mul_c1
  Handle_relin_post_keyswitch(ctx, sl_outer, v_relin_res, n_c0, n_c1, spos);

  // 7. free memory
  Handle_kswitch_free(ctx, sl_outer, spos);

  if (!ctx.Config().Inline_relin()) {
    NODE_PTR n_retv = ctx.Poly_gen().New_var_load(v_relin_res, spos);
    STMT_PTR s_retv = cntr->New_retv(n_retv, spos);
    ctx.Poly_gen().Append_stmt(s_retv, n_outer_blk);
  }
  return n_outer_blk;
}

void CKKS2POLY::Gen_relin_func(CKKS2POLY_CTX& ctx, NODE_PTR node,
                               const SPOS& spos) {
  GLOB_SCOPE* glob    = ctx.Poly_gen().Glob_scope();
  TYPE_PTR    t_ciph  = ctx.Poly_gen().Get_type(VAR_TYPE_KIND::CIPH, spos);
  TYPE_PTR    t_ciph3 = ctx.Poly_gen().Get_type(VAR_TYPE_KIND::CIPH3, spos);

  if (ctx.Poly_gen().Relin_entry() == air::base::Null_ptr) {
    FUNC_SCOPE* orig_fs = ctx.Poly_gen().Func_scope();
    FUNC_SCOPE* fs      = ctx.Poly_gen().New_relin_func();
    fs->Container().New_func_entry(spos);
    ctx.Poly_gen().Enter_func(fs);

    // create formals and expand rotate operations to the new rotate function
    CONST_VAR   f_relin_opnd0(fs, fs->Formal(0));
    NODE_TRIPLE n_opnd0_triple =
        ctx.Poly_gen().New_ciph3_poly_load(f_relin_opnd0, false, spos);
    NODE_PTR parent_blk = ctx.Poly_gen().Container()->Stmt_list().Block_node();
    NODE_PTR n_exp_blk =
        Expand_relin(ctx, node, std::get<0>(n_opnd0_triple),
                     std::get<1>(n_opnd0_triple), std::get<2>(n_opnd0_triple));
    ctx.Poly_gen().Append_stmt(n_exp_blk->Stmt(), parent_blk);

    // switch back to orignal function scope
    ctx.Poly_gen().Enter_func(orig_fs);
  }
}

void CKKS2POLY::Call_relin(CKKS2POLY_CTX& ctx, NODE_PTR node, NODE_PTR n_arg) {
  CONTAINER* cntr = ctx.Poly_gen().Container();
  SPOS       spos = node->Spos();

  // Generate relin function
  Gen_relin_func(ctx, node, spos);

  // Call the relin function and process return value
  TYPE_PTR t_ciph = ctx.Poly_gen().Get_type(VAR_TYPE_KIND::CIPH, spos);
  PREG_PTR retv   = air::base::Null_ptr;
  VAR      v_relin_res;
  if (ctx.Poly_gen().Has_node_var(node)) {
    v_relin_res = ctx.Poly_gen().Node_var(node);
    // reuse v_relin_res as call return if it is a preg
    retv = v_relin_res.Is_preg() && ctx.Config().Reuse_preg_as_retv()
               ? v_relin_res.Preg_var()
               : ctx.Poly_gen().Func_scope()->New_preg(t_ciph);

  } else {
    // use retv preg as the node result symbol
    retv = ctx.Poly_gen().Func_scope()->New_preg(t_ciph);
    ctx.Poly_gen().Add_node_var(node, retv);
  }

  STMT_PTR s_call = cntr->New_call(ctx.Poly_gen().Relin_entry(), retv, 1, spos);
  cntr->New_arg(s_call, 0, n_arg);
  ctx.Prepend(s_call);

  CONST_VAR v_retv(ctx.Func_scope(), retv);
  if (!v_relin_res.Is_null() && v_retv != v_relin_res) {
    // copy the data from preg to store symbol
    STMT_PTR s_res = ctx.Poly_gen().New_var_store(cntr->New_ldp(retv, spos),
                                                  v_relin_res, spos);
    ctx.Prepend(s_res);
  }
}

NODE_PTR CKKS2POLY::Gen_encode_float_from_ciph(CKKS2POLY_CTX& ctx,
                                               CONST_VAR v_ciph, NODE_PTR n_cst,
                                               bool is_mul) {
  CONTAINER* cntr = ctx.Poly_gen().Container();
  SPOS       spos = n_cst->Spos();
  TYPE_PTR   t_ui64 =
      cntr->Glob_scope()->Prim_type(air::base::PRIMITIVE_TYPE::INT_U64);

  AIR_ASSERT(ctx.Lower_ctx()->Is_cipher_type(v_ciph.Type_id()));
  AIR_ASSERT(n_cst->Opcode() ==
                 air::base::OPCODE(air::core::CORE, air::core::OPCODE::LD) ||
             n_cst->Opcode() ==
                 air::base::OPCODE(air::core::CORE, air::core::OPCODE::LDC));

  NODE_PTR n_ciph = ctx.Poly_gen().New_var_load(v_ciph, spos);

  // child 1: const data node
  if (n_cst->Opcode() ==
      air::base::OPCODE(air::core::CORE, air::core::OPCODE::LD)) {
    n_cst = cntr->New_lda(n_cst->Addr_datum(), air::base::POINTER_KIND::FLAT32,
                          spos);
  } else {
    n_cst =
        cntr->New_ldca(n_cst->Const(), air::base::POINTER_KIND::FLAT32, spos);
  }
  // child 2: data len = 1
  NODE_PTR n_len = cntr->New_intconst(t_ui64, 1, spos);

  // child 3: encode scale degree
  // for ciph.mul_const, encode const to degree 1
  // for ciph.add_const, encode const to v_ciph's degree
  NODE_PTR n_scale;
  if (is_mul) {
    n_scale = cntr->New_intconst(t_ui64, 1, spos);
  } else {
    n_scale =
        cntr->New_cust_node(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                              fhe::ckks::CKKS_OPERATOR::SCALE),
                            t_ui64, spos);
    n_scale->Set_child(0, n_ciph);
  }

  // child 4: encode level get from v_ciph
  NODE_PTR n_level =
      cntr->New_cust_node(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                            fhe::ckks::CKKS_OPERATOR::LEVEL),
                          t_ui64, spos);
  n_level->Set_child(0, n_ciph);

  return ctx.Poly_gen().New_encode(n_cst, n_len, n_scale, n_level, spos);
}

}  // namespace poly

}  // namespace fhe
