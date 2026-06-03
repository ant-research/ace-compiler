//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "ckks2hpoly.h"

namespace fhe {
namespace poly {

using namespace air::base;

POLY_LOWER_RETV CKKS2HPOLY::Handle_bin_arith_cc(POLY_LOWER_CTX&     ctx,
                                                air::base::NODE_PTR node,
                                                POLY_LOWER_RETV     opnd0_pair,
                                                POLY_LOWER_RETV     opnd1_pair,
                                                OPCODE              opc) {
  air::base::CONTAINER* cntr = ctx.Poly_gen().Container();
  CMPLR_ASSERT((!opnd0_pair.Is_null() && !opnd1_pair.Is_null()), "null node");

  air::base::NODE_PTR opnd0 = ctx.Poly_gen().New_bin_arith(
      opc, opnd0_pair.Node1(), opnd1_pair.Node1(), node->Spos());
  air::base::NODE_PTR opnd1 = ctx.Poly_gen().New_bin_arith(
      opc, opnd0_pair.Node2(), opnd1_pair.Node2(), node->Spos());

  return POLY_LOWER_RETV(opnd0_pair.Kind(), opnd0, opnd1);
}

POLY_LOWER_RETV CKKS2HPOLY::Handle_bin_arith_cp(POLY_LOWER_CTX&     ctx,
                                                air::base::NODE_PTR node,
                                                POLY_LOWER_RETV     opnd0_pair,
                                                POLY_LOWER_RETV     opnd1_pair,
                                                OPCODE              opc) {
  air::base::CONTAINER* cntr = ctx.Poly_gen().Container();
  CMPLR_ASSERT((!opnd0_pair.Is_null() && !opnd1_pair.Is_null()), "null node");

  air::base::NODE_PTR opnd0 = ctx.Poly_gen().New_bin_arith(
      opc, opnd0_pair.Node1(), opnd1_pair.Node1(), node->Spos());

  air::base::NODE_PTR opnd1 = opnd0_pair.Node2();

  return POLY_LOWER_RETV(opnd0_pair.Kind(), opnd0, opnd1);
}

POLY_LOWER_RETV CKKS2HPOLY::Handle_bin_arith_cf(POLY_LOWER_CTX&     ctx,
                                                air::base::NODE_PTR node,
                                                POLY_LOWER_RETV     opnd0_pair,
                                                POLY_LOWER_RETV     opnd1_pair,
                                                OPCODE              opc) {
  CMPLR_ASSERT((!opnd0_pair.Is_null() && !opnd1_pair.Is_null() &&
                opnd0_pair.Kind() == RETV_KIND::RK_CIPH_POLY &&
                opnd1_pair.Node()->Rtype()->Is_float()),
               "invalid node");

  air::base::CONTAINER* cntr = ctx.Poly_gen().Container();
  air::base::SPOS       spos = node->Spos();

  air::base::NODE_PTR n_child0 = node->Child(0);
  CONST_VAR&          v_child0 = ctx.Poly_gen().Node_var(n_child0);

  // 1. encode float data
  air::base::NODE_PTR n_encode = Gen_encode_float_from_ciph(
      ctx, node, v_child0, opnd1_pair.Node1(), false);
  air::base::ADDR_DATUM_PTR sym = ctx.Poly_gen().New_plain_var(spos);
  CONST_VAR& v_encode = ctx.Poly_gen().Add_node_var(node->Child(1), sym);
  air::base::STMT_PTR s_encode =
      ctx.Poly_gen().New_var_store(n_encode, v_encode, spos);
  ctx.Prepend(s_encode);

  // 2. add ciph with encoded float
  air::base::NODE_PTR n_plain =
      ctx.Poly_gen().New_plain_poly_load(v_encode, false, spos);

  air::base::NODE_PTR opnd0 = ctx.Poly_gen().New_bin_arith(
      opc, opnd0_pair.Node1(), n_plain, node->Spos());
  air::base::NODE_PTR opnd1 = cntr->Clone_node_tree(opnd0_pair.Node2());

  return POLY_LOWER_RETV(opnd0_pair.Kind(), opnd0, opnd1);
}

POLY_LOWER_RETV CKKS2HPOLY::Handle_mul_ciph(POLY_LOWER_CTX& ctx, NODE_PTR node,
                                            POLY_LOWER_RETV opnd0_pair,
                                            POLY_LOWER_RETV opnd1_pair) {
  CMPLR_ASSERT((!opnd0_pair.Is_null() && !opnd1_pair.Is_null() &&
                opnd0_pair.Kind() == RETV_KIND::RK_CIPH_POLY &&
                opnd1_pair.Kind() == RETV_KIND::RK_CIPH_POLY &&
                ctx.Lower_ctx()->Is_cipher3_type(node->Rtype_id())),
               "invalid mul_ciph");
  POLY_IR_GEN& pgen = ctx.Poly_gen();
  CONTAINER*   cntr = pgen.Container();
  GLOB_SCOPE*  glob = cntr->Glob_scope();
  SPOS         spos = node->Spos();

  // 1. v_mul_0 = opnd0_c0 * opnd1_c0
  NODE_PTR n_mul_0 =
      pgen.New_poly_mul(opnd0_pair.Node1(), opnd1_pair.Node1(), node->Spos());

  // 2. n_mul_1 = n_mul_1_0 + n_mul_1_1
  // n_mul_1_0 = opnd0_c1 * opnd1_c0
  // n_mul_1_1 = opnd0_c0 * opnd1_c0
  NODE_PTR n_mul_1_0 = pgen.New_poly_mul(
      opnd0_pair.Node2(), cntr->Clone_node_tree(opnd1_pair.Node1()),
      node->Spos());
  NODE_PTR n_mul_1_1 = pgen.New_poly_mul(
      cntr->Clone_node_tree(opnd0_pair.Node1()),
      cntr->Clone_node_tree(opnd1_pair.Node2()), node->Spos());
  NODE_PTR n_mul_1 = pgen.New_poly_add(n_mul_1_0, n_mul_1_1, node->Spos());

  // 3. n_mul_2 = opnd0_c1 * opnd1_c1
  NODE_PTR n_mul_2 = pgen.New_poly_mul(
      cntr->Clone_node_tree(opnd0_pair.Node2()),
      cntr->Clone_node_tree(opnd1_pair.Node2()), node->Spos());

  pgen.Set_mul_ciph(n_mul_0);
  pgen.Set_mul_ciph(n_mul_1_0);
  pgen.Set_mul_ciph(n_mul_1_1);
  pgen.Set_mul_ciph(n_mul_2);

  return POLY_LOWER_RETV(RETV_KIND::RK_CIPH3_POLY, n_mul_0, n_mul_1, n_mul_2);
}

POLY_LOWER_RETV CKKS2HPOLY::Handle_mul_plain(POLY_LOWER_CTX& ctx, NODE_PTR node,
                                             POLY_LOWER_RETV opnd0_pair,
                                             POLY_LOWER_RETV opnd1_pair) {
  CMPLR_ASSERT((!opnd0_pair.Is_null() && !opnd1_pair.Is_null() &&
                opnd0_pair.Kind() == RETV_KIND::RK_CIPH_POLY &&
                opnd1_pair.Kind() == RETV_KIND::RK_PLAIN_POLY),
               "null node");

  CONTAINER* cntr = ctx.Poly_gen().Container();

  // TODO: shall we share the nodes in different op?
  NODE_PTR mul_0 = ctx.Poly_gen().New_poly_mul(
      opnd0_pair.Node1(), opnd1_pair.Node1(), node->Spos());
  NODE_PTR mul_1 = ctx.Poly_gen().New_poly_mul(
      opnd0_pair.Node2(), cntr->Clone_node_tree(opnd1_pair.Node1()),
      node->Spos());

  return POLY_LOWER_RETV(RETV_KIND::RK_CIPH_POLY, mul_0, mul_1);
}

POLY_LOWER_RETV CKKS2HPOLY::Handle_mul_float(POLY_LOWER_CTX& ctx, NODE_PTR node,
                                             POLY_LOWER_RETV opnd0_pair,
                                             POLY_LOWER_RETV opnd1_pair) {
  CMPLR_ASSERT((!opnd0_pair.Is_null() && !opnd1_pair.Is_null() &&
                opnd0_pair.Kind() == RETV_KIND::RK_CIPH_POLY &&
                opnd1_pair.Node()->Rtype()->Is_float()),
               "invalid node");

  CONTAINER* cntr = ctx.Poly_gen().Container();
  SPOS       spos = node->Spos();

  // encode float data
  NODE_PTR   n_child0 = node->Child(0);
  CONST_VAR& v_child0 = ctx.Poly_gen().Node_var(n_child0);
  NODE_PTR   n_encode =
      Gen_encode_float_from_ciph(ctx, node, v_child0, opnd1_pair.Node1(), true);
  air::base::ADDR_DATUM_PTR sym = ctx.Poly_gen().New_plain_var(spos);
  CONST_VAR& v_encode = ctx.Poly_gen().Add_node_var(node->Child(1), sym);
  STMT_PTR   s_encode = ctx.Poly_gen().New_var_store(n_encode, v_encode, spos);
  ctx.Prepend(s_encode);

  NODE_PTR n_plain = ctx.Poly_gen().New_plain_poly_load(v_encode, false, spos);

  NODE_PTR mul_0 =
      ctx.Poly_gen().New_poly_mul(opnd0_pair.Node1(), n_plain, spos);
  NODE_PTR mul_1 = ctx.Poly_gen().New_poly_mul(
      opnd0_pair.Node2(), cntr->Clone_node_tree(n_plain), spos);
  return POLY_LOWER_RETV(RETV_KIND::RK_CIPH_POLY, mul_0, mul_1);
}

POLY_LOWER_RETV CKKS2HPOLY::Expand_relin(POLY_LOWER_CTX& ctx, NODE_PTR node,
                                         NODE_PTR n_c0, NODE_PTR n_c1,
                                         NODE_PTR n_c2) {
  CONTAINER*  cntr = ctx.Poly_gen().Container();
  GLOB_SCOPE* glob = ctx.Poly_gen().Glob_scope();
  SPOS        spos = node->Spos();

  // generate alloc for RNS_POLY array specific to the CPU target
  VAR v_precomp(ctx.Func_scope(), ctx.Poly_gen().New_polys_var(spos));
  if (ctx.Config().Lower_to_lpoly()) {
    NODE_PTR n_alloc = ctx.Poly_gen().New_alloc_for_precomp(n_c2, spos);
    STMT_PTR s_alloc = ctx.Poly_gen().New_var_store(n_alloc, v_precomp, spos);
    ctx.Prepend(s_alloc);
  }

  // generate precompute: v_precompute = KSW_PRECOMP(n_c2)
  NODE_PTR n_precomp = ctx.Poly_gen().New_precomp(n_c2, spos);
  STMT_PTR s_precomp = ctx.Poly_gen().New_var_store(n_precomp, v_precomp, spos);
  ctx.Prepend(s_precomp);

  // generate get relin key: v_swk = SWK()
  VAR      v_swk_c0(ctx.Func_scope(), ctx.Poly_gen().New_swk_var(spos));
  VAR      v_swk_c1(ctx.Func_scope(), ctx.Poly_gen().New_swk_var(spos));
  NODE_PTR n_swk_c0 = ctx.Poly_gen().New_swk_c0(false, spos);
  NODE_PTR n_swk_c1 = ctx.Poly_gen().New_swk_c1(false, spos);
  STMT_PTR s_swk_c0 = ctx.Poly_gen().New_var_store(n_swk_c0, v_swk_c0, spos);
  STMT_PTR s_swk_c1 = ctx.Poly_gen().New_var_store(n_swk_c1, v_swk_c1, spos);
  ctx.Prepend(s_swk_c0);
  ctx.Prepend(s_swk_c1);

  // dot prod size
  NODE_PTR                  n_num_part;
  air::base::CONST_TYPE_PTR ui32_type =
      ctx.Poly_gen().Get_type(VAR_TYPE_KIND::UINT32);
  if (ctx.Config().Lower_to_lpoly()) {
    // for targeting cpu, the value is calculate through rtlib call
    n_num_part = ctx.Poly_gen().New_poly_node(NUM_DECOMP, ui32_type, spos);
    n_num_part->Set_child(0, cntr->Clone_node_tree(n_c1));
  } else {
    // for targeting hpu, the value should be calculate through attribute
    // the logic canbe moved to constant folding
    n_num_part = cntr->New_intconst(ui32_type,
                                    ctx.Poly_gen().Get_num_decomp(node), spos);
  }

  // generate keyswitch dot product n_prod = DOT_PROD()
  NODE_PTR n_prod_c0 = ctx.Poly_gen().New_dot_prod(
      ctx.Poly_gen().New_var_load(v_precomp, spos),
      ctx.Poly_gen().New_var_load(v_swk_c0, spos), n_num_part, spos);
  NODE_PTR n_prod_c1 =
      ctx.Poly_gen().New_dot_prod(ctx.Poly_gen().New_var_load(v_precomp, spos),
                                  ctx.Poly_gen().New_var_load(v_swk_c1, spos),
                                  cntr->Clone_node_tree(n_num_part), spos);

  // generate moddown: MOD_DOWN(n_prod_c0)
  //                   MOD_DOWN(n_prod_c1)
  NODE_PTR n_md_c0 = ctx.Poly_gen().New_mod_down(n_prod_c0, spos);
  NODE_PTR n_md_c1 = ctx.Poly_gen().New_mod_down(n_prod_c1, spos);

  // post add
  NODE_PTR n_add_c0 = ctx.Poly_gen().New_poly_add(n_c0, n_md_c0, spos);
  NODE_PTR n_add_c1 = ctx.Poly_gen().New_poly_add(n_c1, n_md_c1, spos);
  return POLY_LOWER_RETV(RETV_KIND::RK_CIPH_POLY, n_add_c0, n_add_c1);
}

POLY_LOWER_RETV CKKS2HPOLY::Expand_rotate(POLY_LOWER_CTX& ctx, NODE_PTR node,
                                          NODE_PTR n_c0, NODE_PTR n_c1,
                                          NODE_PTR    n_rot_idx,
                                          const SPOS& spos) {
  CONTAINER*  cntr = ctx.Poly_gen().Container();
  GLOB_SCOPE* glob = ctx.Poly_gen().Glob_scope();

  // generate alloc for RNS_POLY array for cpu target
  VAR v_precomp(ctx.Func_scope(), ctx.Poly_gen().New_polys_var(spos));
  if (ctx.Config().Lower_to_lpoly()) {
    NODE_PTR n_alloc = ctx.Poly_gen().New_alloc_for_precomp(n_c1, spos);
    STMT_PTR s_alloc = ctx.Poly_gen().New_var_store(n_alloc, v_precomp, spos);
    ctx.Prepend(s_alloc);
  }

  // generate precompute: v_precompute = KSW_PRECOMP(n_c1)
  NODE_PTR n_precomp = ctx.Poly_gen().New_precomp(n_c1, spos);
  STMT_PTR s_precomp = ctx.Poly_gen().New_var_store(n_precomp, v_precomp, spos);
  ctx.Prepend(s_precomp);

  // get switch key with given index
  VAR      v_swk_c0(ctx.Func_scope(), ctx.Poly_gen().New_swk_var(spos));
  VAR      v_swk_c1(ctx.Func_scope(), ctx.Poly_gen().New_swk_var(spos));
  NODE_PTR n_swk_c0 = ctx.Poly_gen().New_swk_c0(true, spos, n_rot_idx);
  NODE_PTR n_swk_c1 =
      ctx.Poly_gen().New_swk_c1(true, spos, cntr->Clone_node_tree(n_rot_idx));
  STMT_PTR s_swk_c0 = ctx.Poly_gen().New_var_store(n_swk_c0, v_swk_c0, spos);
  STMT_PTR s_swk_c1 = ctx.Poly_gen().New_var_store(n_swk_c1, v_swk_c1, spos);
  ctx.Prepend(s_swk_c0);
  ctx.Prepend(s_swk_c1);

  // dot prod size
  NODE_PTR                  n_num_part;
  air::base::CONST_TYPE_PTR ui32_type =
      ctx.Poly_gen().Get_type(VAR_TYPE_KIND::UINT32);
  if (ctx.Config().Lower_to_lpoly()) {
    // for targeting cpu, the value is calculate through rtlib call
    n_num_part = ctx.Poly_gen().New_poly_node(NUM_DECOMP, ui32_type, spos);
    n_num_part->Set_child(0, cntr->Clone_node_tree(n_c1));
  } else {
    // for targeting hpu, the value should be calculate through attribute
    // the logic canbe moved to constant folding
    n_num_part = cntr->New_intconst(ui32_type,
                                    ctx.Poly_gen().Get_num_decomp(node), spos);
  }

  // generate keyswitch dot product v_prod = DOT_PROD(n_precomp, n_swk)
  NODE_PTR n_prod_c0 = ctx.Poly_gen().New_dot_prod(
      ctx.Poly_gen().New_var_load(v_precomp, spos),
      ctx.Poly_gen().New_var_load(v_swk_c0, spos), n_num_part, spos);
  NODE_PTR n_prod_c1 =
      ctx.Poly_gen().New_dot_prod(ctx.Poly_gen().New_var_load(v_precomp, spos),
                                  ctx.Poly_gen().New_var_load(v_swk_c1, spos),
                                  cntr->Clone_node_tree(n_num_part), spos);

  // generate moddown: MOD_DOWN(n_prod_c0)
  //                   MOD_DOWN(n_prod_c1)
  NODE_PTR n_md_c0 = ctx.Poly_gen().New_mod_down(n_prod_c0, spos);
  NODE_PTR n_md_c1 = ctx.Poly_gen().New_mod_down(n_prod_c1, spos);

  // add_c0
  NODE_PTR n_add_c0 = ctx.Poly_gen().New_poly_add(n_c0, n_md_c0, spos);

  // rotate poly
  NODE_PTR n_rot_c0 = ctx.Poly_gen().New_poly_rotate(
      n_add_c0, cntr->Clone_node_tree(n_rot_idx), spos);
  NODE_PTR n_rot_c1 = ctx.Poly_gen().New_poly_rotate(
      n_md_c1, cntr->Clone_node_tree(n_rot_idx), spos);
  n_rot_c0->Copy_attr(node);
  n_rot_c1->Copy_attr(node);

  return POLY_LOWER_RETV(RETV_KIND::RK_CIPH_POLY, n_rot_c0, n_rot_c1);
}

NODE_PTR CKKS2HPOLY::Gen_encode_float_from_ciph(POLY_LOWER_CTX& ctx,
                                                NODE_PTR node, CONST_VAR v_ciph,
                                                NODE_PTR n_cst, bool is_mul) {
  CONTAINER* cntr = ctx.Poly_gen().Container();
  SPOS       spos = n_cst->Spos();
  TYPE_PTR   t_ui32 =
      cntr->Glob_scope()->Prim_type(air::base::PRIMITIVE_TYPE::INT_U32);

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
  NODE_PTR n_len = cntr->New_intconst(t_ui32, 1, spos);

  // child 3: encode scale degree
  NODE_PTR n_scale;
  NODE_PTR n_level;
  // for ciph.mul_const, encode const to degree 1
  // for ciph.add_const, encode const to v_ciph's degree
  if (is_mul) {
    n_scale = cntr->New_intconst(t_ui32, 1, spos);
  } else {
    n_scale = cntr->New_cust_node(fhe::ckks::OPC_SCALE, t_ui32, spos);
    n_scale->Set_child(0, n_ciph);
  }
  // child 4: encode level get from v_ciph
  n_level = cntr->New_cust_node(fhe::ckks::OPC_LEVEL, t_ui32, spos);
  n_level->Set_child(0, n_ciph);

  NODE_PTR n_enc =
      ctx.Poly_gen().New_encode(n_cst, n_len, n_scale, n_level, spos);
  n_enc->Copy_attr(node);
  return n_enc;
}

}  // namespace poly

}  // namespace fhe
