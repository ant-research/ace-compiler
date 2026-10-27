//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================
#ifndef FHE_POLY_HPOLY_P2_H
#define FHE_POLY_HPOLY_P2_H

#include <sstream>

#include "air/base/transform_ctx.h"
#include "air/base/visitor.h"
#include "air/core/default_handler.h"
#include "air/core/handler.h"
#include "fhe/core/rt_context.h"
#include "fhe/poly/default_handler.h"
#include "fhe/poly/handler.h"
#include "poly_ir_gen.h"
#include "poly_lower_ctx.h"
namespace fhe {
namespace poly {

class HP1TOHP2;
class CORE2HP2;
using HP1TOHP2_VISITOR =
    air::base::VISITOR<POLY_LOWER_CTX, air::core::HANDLER<CORE2HP2>,
                       fhe::poly::HANDLER<HP1TOHP2>>;

//! @brief Lower Phase I HPOLY operators such as: OPC_PRECOMP,
//! OPC_DOT_PROD, OPC_MOD_DOWN
class HP1TOHP2 : public fhe::poly::DEFAULT_HANDLER {
public:
  //! @brief Construct a new HP1TOHP2 object
  HP1TOHP2() {}

  //! @brief Handle HPOLY_OPERATOR::PRECOMP
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_precomp(VISITOR* visitor, air::base::NODE_PTR node);

  //! @brief Handle HPOLY_OPERATOR::DOT_PROD
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_dot_prod(VISITOR* visitor, air::base::NODE_PTR node);

  //! @brief Handle HPOLY_OPERATOR::MOD_DOWN
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_mod_down(VISITOR* visitor, air::base::NODE_PTR node);

  //! @brief Handle HPOLY_OPERATOR::RESCALE
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_rescale(VISITOR* visitor, air::base::NODE_PTR node);

  //! @brief Handle HPOLY_OPERATOR::RAISE_MOD
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_raise_mod(VISITOR* visitor, air::base::NODE_PTR node);
};

class CORE2HP2 : public air::core::DEFAULT_HANDLER {
public:
  //! @brief Construct a new CORE2HP2 object
  CORE2HP2() {}

  //! @brief Handle CORE::STORE
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_st(VISITOR* visitor, air::base::NODE_PTR node);

  //! @brief Handle CORE::STP
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_stp(VISITOR* visitor, air::base::NODE_PTR node);

  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_stf(VISITOR* visitor, air::base::NODE_PTR node);

  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_stpf(VISITOR* visitor, air::base::NODE_PTR node);

  template <typename VISITOR>
  POLY_LOWER_RETV Handle_st_var(VISITOR* visitor, air::base::NODE_PTR node,
                                CONST_VAR var);
};

//! @brief Lowering for HPOLY::PRECOMP
//! The algorithm is based CKKS hybrid keyswitch, and the C implementation can
//! be referenced in rtlib/ant/poly/src/rns_poly.c:
//! Switch_key_precompute_fusion
template <typename RETV, typename VISITOR>
POLY_LOWER_RETV HP1TOHP2::Handle_precomp(VISITOR*            visitor,
                                         air::base::NODE_PTR node) {
  POLY_LOWER_CTX&       ctx       = visitor->Context();
  fhe::core::LOWER_CTX* lower_ctx = ctx.Lower_ctx();
  air::base::CONTAINER* cntr      = ctx.Container();
  POLY_IR_GEN&          irgen     = ctx.Poly_gen();
  air::base::SPOS       spos      = node->Spos();
  air::base::NODE_PTR   n_opnd0 =
      visitor->template Visit<RETV>(node->Child(0)).Node();

  if (ctx.Config().Gen_decomp_loop()) {
    AIR_ASSERT_MSG(false, "Handle_precomp: loop not supported");
    return POLY_LOWER_RETV();
  } else {
    AIR_ASSERT_MSG(irgen.Has_node_var(node), "precomp has no store var");
    CONST_VAR& v_res = irgen.Node_var(node);
    AIR_ASSERT(!irgen.Has_mapping_vars(v_res));
    VAR_ARR& v_raised_arr = irgen.Mapping_vars(v_res);

    uint32_t num_decomp = irgen.Get_num_decomp(node->Child(0));
    uint32_t num_q      = irgen.Get_num_q(node->Child(0));
    uint32_t num_p      = lower_ctx->Get_ctx_param().Get_p_prime_num();
    uint32_t num_qp     = num_q + num_p;
    const fhe::core::CTX_PARAM& ctx_param  = lower_ctx->Get_ctx_param();
    uint32_t                    part_size  = ctx_param.Get_per_part_size();
    air::base::TYPE_PTR         t_rns_poly = irgen.Poly_type();

    for (uint32_t idx = 0; idx < num_decomp; idx++) {
      uint32_t s_p1_idx = 0;
      uint32_t num_p1   = part_size * idx;
      uint32_t e_p1_idx = idx == 0 ? 0 : s_p1_idx + num_p1 - 1;

      uint32_t s_p2_idx = num_p1;
      uint32_t num_p2   = ctx_param.Get_part2_size(num_q, idx);
      uint32_t e_p2_idx = s_p2_idx + num_p2 - 1;

      uint32_t num_p3 = num_qp - num_p1 - num_p2;
      // part3's end rns index should always be the max rns index
      uint32_t e_p3_idx = ctx_param.Get_tot_prime_num() - 1;
      uint32_t s_p3_idx = e_p3_idx - num_p3 + 1;

      // p2 = HPOLY.extract(input, s_p2, e_p2)
      CONST_VAR           v_p2 = irgen.New_poly_preg();
      air::base::NODE_PTR n_p2 =
          irgen.New_extract(n_opnd0, s_p2_idx, e_p2_idx, spos);
      air::base::STMT_PTR s_p2 = irgen.New_var_store(n_p2, v_p2, spos);
      ctx.Prepend(s_p2);

      // p2_intt = HPOLY.intt(p2)
      CONST_VAR           v_p2_intt = irgen.New_poly_preg();
      air::base::NODE_PTR n_p2_intt =
          irgen.New_intt(irgen.New_var_load(v_p2, spos), spos);
      air::base::STMT_PTR s_p2_intt =
          irgen.New_var_store(n_p2_intt, v_p2_intt, spos);
      ctx.Prepend(s_p2_intt);

      // p2_qlhatinv = HPOLY.mul(p2_intt, ql_hatinvmodq)
      CONST_VAR           v_p2_qlhinv = irgen.New_poly_preg();
      air::base::NODE_PTR n_qlhinv =
          irgen.New_qlhinvmodq(idx, num_p2 - 1, spos);
      air::base::NODE_PTR n_p2_qlhinv = irgen.New_poly_mul(
          irgen.New_var_load(v_p2_intt, spos), n_qlhinv, spos);
      air::base::STMT_PTR s_p2_qlhinv =
          irgen.New_var_store(n_p2_qlhinv, v_p2_qlhinv, spos);
      ctx.Prepend(s_p2_qlhinv);

      CONST_VAR           v_raised_1 = irgen.New_poly_preg();
      air::base::STMT_PTR s_raised1;
      if (num_p1 > 0) {
        // p1_intt = HPOLY.bconv(p2_qlhatinv, qlhatmodp, s_p1_idx, e_p1_idx)
        air::base::NODE_PTR n_qhmodp =
            irgen.New_qlhmodp(num_q - 1, idx, 0, num_p1, spos);
        CONST_VAR           v_p1_intt = irgen.New_poly_preg();
        air::base::NODE_PTR n_p1_intt =
            irgen.New_bconv(irgen.New_var_load(v_p2_qlhinv, spos), n_qhmodp,
                            s_p1_idx, e_p1_idx, spos);
        air::base::STMT_PTR s_p1_intt2 =
            irgen.New_var_store(n_p1_intt, v_p1_intt, spos);
        ctx.Prepend(s_p1_intt2);

        // raised_1 = HPOLY.ntt(p1_intt), result directly to raised for first
        // part
        air::base::NODE_PTR n_raised1 =
            irgen.New_ntt(irgen.New_var_load(v_p1_intt, spos), spos);
        s_raised1 = irgen.New_var_store(n_raised1, v_raised_1, spos);
        ctx.Prepend(s_raised1);
      } else {
        // raised_1 = 0 when part1 is empty
        air::base::NODE_PTR n_zero = cntr->New_zero(t_rns_poly, spos);
        s_raised1 = irgen.New_var_store(n_zero, v_raised_1, spos);
        irgen.Set_num_q(n_zero, 0);
        ctx.Prepend(s_raised1);
      }

      // raised2 = HPOLY.concat(raised_1, p2)
      // create new preg to concat data
      CONST_VAR           v_raised_2 = irgen.New_poly_preg();
      air::base::NODE_PTR n_raised   = irgen.New_var_load(v_raised_1, spos);
      air::base::NODE_PTR n_raised2 =
          irgen.New_concat(n_raised, irgen.New_var_load(v_p2, spos), spos);
      air::base::STMT_PTR s_raised2 =
          irgen.New_var_store(n_raised2, v_raised_2, spos);
      ctx.Prepend(s_raised2);

      // p3_intt = HPOLY.bconv(p2_qlhatinv, qlhatmodp, s_p3_idx, e_p3_idx)
      CONST_VAR           v_p3_intt = irgen.New_poly_preg();
      air::base::NODE_PTR n_qhmodp =
          irgen.New_qlhmodp(num_q - 1, idx, num_p1, num_p3, spos);
      air::base::NODE_PTR n_p3_intt =
          irgen.New_bconv(irgen.New_var_load(v_p2_qlhinv, spos), n_qhmodp,
                          s_p3_idx, e_p3_idx, spos);
      air::base::STMT_PTR s_p3_intt_2 =
          irgen.New_var_store(n_p3_intt, v_p3_intt, spos);
      ctx.Prepend(s_p3_intt_2);

      // p3 = HPOLY.ntt(p3_intt)
      CONST_VAR           v_p3 = irgen.New_poly_preg();
      air::base::NODE_PTR n_p3 =
          irgen.New_ntt(irgen.New_var_load(v_p3_intt, spos), spos);
      air::base::STMT_PTR s_p3 = irgen.New_var_store(n_p3, v_p3, spos);
      ctx.Prepend(s_p3);

      // raised_3 = HPOLY.concat(raised_2, p3)
      // create new preg to concat data
      CONST_VAR v_raised_3 = irgen.New_poly_preg();
      n_raised             = irgen.New_var_load(v_raised_2, spos);
      air::base::NODE_PTR n_raised3 =
          irgen.New_concat(n_raised, irgen.New_var_load(v_p3, spos), spos);
      air::base::STMT_PTR s_raised3 =
          irgen.New_var_store(n_raised3, v_raised_3, spos);
      ctx.Prepend(s_raised3);

      v_raised_arr.push_back(v_raised_3);
    }
    // the store logic is recorded by var mapping
    return POLY_LOWER_RETV();
  }
}

//! @brief Lowering for HPOLY::DOTPROD(ch0, ch1, ch2)
//! Expand to HPOLY.MAC(res, ch0[i], ch1[i])
template <typename RETV, typename VISITOR>
POLY_LOWER_RETV HP1TOHP2::Handle_dot_prod(VISITOR*            visitor,
                                          air::base::NODE_PTR node) {
  POLY_LOWER_CTX&       ctx       = visitor->Context();
  fhe::core::LOWER_CTX* lower_ctx = ctx.Lower_ctx();
  POLY_IR_GEN&          irgen     = ctx.Poly_gen();
  air::base::CONTAINER* cntr      = ctx.Container();
  air::base::SPOS       spos      = node->Spos();

  air::base::NODE_PTR n_ch0 = node->Child(0);
  air::base::NODE_PTR n_ch1 = node->Child(1);
  air::base::NODE_PTR n_ch2 = node->Child(2);
  AIR_ASSERT(n_ch0->Is_ld() && irgen.Is_type_of(n_ch0->Rtype_id(), POLY_PTR));
  AIR_ASSERT(n_ch1->Is_ld() && irgen.Is_poly_array(n_ch1->Rtype()));
  AIR_ASSERT(irgen.Is_type_of(n_ch2->Rtype_id(), UINT32));

  if (ctx.Config().Gen_decomp_loop()) {
    AIR_ASSERT_MSG(false, "Handle_dot_prod: loop not supported");
    return POLY_LOWER_RETV();
  } else {
    CONST_VAR& v_res = irgen.Node_var(node);
    CONST_VAR& v_ch0 = irgen.Node_var(n_ch0);
    CONST_VAR& v_ch1 = irgen.Node_var(n_ch1);
    AIR_ASSERT(irgen.Has_mapping_vars(v_ch0));
    VAR_ARR& v_ch0_arr = irgen.Mapping_vars(v_ch0);

    // switch key are RNS_POLY array, array size should be >= ch0 size
    uint32_t ch1_size = n_ch1->Rtype()->Cast_to_arr()->Elem_count();
    uint32_t ch1_dims = n_ch1->Rtype()->Cast_to_arr()->Dim();
    AIR_ASSERT(ch1_dims == 1 && ch1_size >= v_ch0_arr.size());

    // v_res = 0
    air::base::TYPE_PTR t_rns_poly = irgen.Poly_type();
    air::base::NODE_PTR n_zero     = cntr->New_zero(t_rns_poly, spos);
    air::base::STMT_PTR s_res      = irgen.New_var_store(n_zero, v_res, spos);
    irgen.Set_num_q(n_zero, irgen.Get_num_q(node));
    irgen.Set_num_p(n_zero, irgen.Get_num_p(node));
    ctx.Prepend(s_res);

    for (uint32_t decomp_idx = 0; decomp_idx < v_ch0_arr.size(); decomp_idx++) {
      // v_res = mac(v_res, v_ch0_elem, v_ch1_elem)
      air::base::NODE_PTR n_res      = irgen.New_var_load(v_res, spos);
      CONST_VAR&          v_ch0_elem = v_ch0_arr[decomp_idx];
      air::base::NODE_PTR n_ch0_elem = irgen.New_var_load(v_ch0_elem, spos);

      // child1 element is load from switch key array by decomp_idx
      air::base::NODE_PTR n_ch1_lda = cntr->New_lda(
          v_ch1.Addr_var(), air::base::POINTER_KIND::FLAT64, spos);
      air::base::NODE_PTR n_ch1_arr =
          cntr->New_array(n_ch1_lda, ch1_dims, spos);
      air::base::NODE_PTR n_ch1_idx =
          cntr->New_intconst(irgen.Get_type(UINT32), decomp_idx, spos);
      cntr->Set_array_idx(n_ch1_arr, 0, n_ch1_idx);
      air::base::NODE_PTR n_ch1_elem = cntr->New_ild(n_ch1_arr, spos);

      air::base::NODE_PTR n_mac = irgen.New_poly_mac(
          n_res, n_ch0_elem, n_ch1_elem, irgen.Get_num_p(node) != 0, spos);
      s_res = irgen.New_var_store(n_mac, v_res, spos);
      ctx.Prepend(s_res);
    }
    return POLY_LOWER_RETV();
  }
}

//! @brief Lowering for HPOLY::MOD_DOWN
//! The algorithm is based CKKS hybrid keyswitch, and the C implementation can
//! be referenced in rtlib/ant/poly/src/rns_poly.c: Mod_down
//! Lowered to:
//!   v_md0 = Extract(v_ch0, num_q, num_q + num_p - 1);
//!   v_md1 = INTT(v_md0);
//!   v_md2 = Mul_ext(v_md1, PHatInvmodp);
//!   v_md3 = Bconv(v_md2, PhatModq, 0, num_q - 1); // 0 ~ q modulus
//!   v_md4 = NTT(v_md3);
//!   v_md5 = Sub(v_ch0, v_md4);
//!   v_res = MUL(v_md5, PInvModQ);
template <typename RETV, typename VISITOR>
POLY_LOWER_RETV HP1TOHP2::Handle_mod_down(VISITOR*            visitor,
                                          air::base::NODE_PTR node) {
  POLY_LOWER_CTX&       ctx        = visitor->Context();
  fhe::core::LOWER_CTX* lower_ctx  = ctx.Lower_ctx();
  POLY_IR_GEN&          irgen      = ctx.Poly_gen();
  air::base::CONTAINER* cntr       = ctx.Container();
  air::base::SPOS       spos       = node->Spos();
  air::base::TYPE_PTR   t_rns_poly = irgen.Poly_type();

  AIR_ASSERT(irgen.Has_node_var(node) && irgen.Has_node_var(node->Child(0)));
  CONST_VAR v_res = irgen.Node_var(node);
  CONST_VAR v_ch0 = irgen.Node_var(node->Child(0));
  AIR_ASSERT(irgen.Is_type_of(v_res.Type_id(), POLY) &&
             irgen.Is_type_of(v_ch0.Type_id(), POLY));
  uint32_t num_q = irgen.Get_num_q(node->Child(0));
  uint32_t num_p = irgen.Get_num_p(node->Child(0));
  AIR_ASSERT(num_p == lower_ctx->Get_ctx_param().Get_p_prime_num());

  // lower child 0
  air::base::NODE_PTR n_ch0 =
      visitor->template Visit<RETV>(node->Child(0)).Node();

  // v_md0 = Extract(v_ch0, num_q, num_q + num_p - 1);
  uint32_t            s_idx = num_q;
  uint32_t            e_idx = num_q + num_p - 1;
  CONST_VAR           v_md0 = irgen.New_poly_preg();
  air::base::NODE_PTR n_md0 = irgen.New_extract(n_ch0, s_idx, e_idx, spos);
  air::base::STMT_PTR s_md0 = irgen.New_var_store(n_md0, v_md0, spos);
  ctx.Prepend(s_md0);

  // v_md1 = INTT(v_md0)
  CONST_VAR           v_md1 = irgen.New_poly_preg();
  air::base::NODE_PTR n_md1 =
      irgen.New_intt(irgen.New_var_load(v_md0, spos), spos);
  air::base::STMT_PTR s_md1 = irgen.New_var_store(n_md1, v_md1, spos);
  ctx.Prepend(s_md1);

  // v_md2 = Mul(v_md1, Phatinvmodp)
  CONST_VAR           v_md2         = irgen.New_poly_preg();
  air::base::NODE_PTR n_phatinvmodp = irgen.New_phinvmodp(spos);
  air::base::NODE_PTR n_md2         = irgen.New_poly_mul_ext(
      irgen.New_var_load(v_md1, spos), n_phatinvmodp, spos);
  air::base::STMT_PTR s_md2 = irgen.New_var_store(n_md2, v_md2, spos);
  ctx.Prepend(s_md2);

  // v_md3 = Bconv(v_md2, PhatModq, s_q_idx, e_q_idx)
  // convert v_md2's RNS basis from P to Q_l
  CONST_VAR           v_md3    = irgen.New_poly_preg();
  uint32_t            s_q_idx  = 0;
  uint32_t            e_q_idx  = num_q - 1;
  air::base::NODE_PTR n_phmodq = irgen.New_phmodq(spos);
  air::base::NODE_PTR n_md3 = irgen.New_bconv(irgen.New_var_load(v_md2, spos),
                                              n_phmodq, s_q_idx, e_q_idx, spos);
  air::base::STMT_PTR s_md3 = irgen.New_var_store(n_md3, v_md3, spos);
  ctx.Prepend(s_md3);

  // v_md4 = NTT(v_md3)
  CONST_VAR           v_md4 = irgen.New_poly_preg();
  air::base::NODE_PTR n_md4 =
      irgen.New_ntt(irgen.New_var_load(v_md3, spos), spos);
  air::base::STMT_PTR s_md4 = irgen.New_var_store(n_md4, v_md4, spos);
  ctx.Prepend(s_md4);

  // v_md5 = SUB(v_ch0, v_md4)
  CONST_VAR           v_md5     = irgen.New_poly_preg();
  air::base::NODE_PTR n_md5_ch0 = irgen.New_var_load(v_ch0, spos);
  air::base::NODE_PTR n_md5_ch1 = irgen.New_var_load(v_md4, spos);
  air::base::NODE_PTR n_md5 = irgen.New_poly_sub(n_md5_ch0, n_md5_ch1, spos);
  air::base::STMT_PTR s_md5 = irgen.New_var_store(n_md5, v_md5, spos);
  ctx.Prepend(s_md5);

  // v_res = MUL(v_md5, PinvModQ)
  air::base::NODE_PTR n_pinvmodq = irgen.New_pinvmodq(spos);
  air::base::NODE_PTR n_res_ch0  = irgen.New_var_load(v_md5, spos);
  air::base::NODE_PTR n_res = irgen.New_poly_mul(n_res_ch0, n_pinvmodq, spos);
  air::base::STMT_PTR s_res = irgen.New_var_store(n_res, v_res, spos);
  ctx.Prepend(s_res);

  return POLY_LOWER_RETV();
}

//! @brief Lowering for HPOLY::RESCALE
//! The algorithm is based on CKKS Rescale
//!  v_res = HPOLY.Rescale(v_ch0)
//! Lowered to:
//!   // extract the last basis
//!   v_rs0 = Extract(v_ch0, num_q-1, num_q - 1);
//!   v_rs1 = INTT(v_res0)
//!   // change from flooring to rounding
//!   v_rs2 = ADD(v_rs1, q_last/2)
//!   // switch the modulus from q_last to ranges between [s_idx, e_idx]
//!   v_rs3 = BSWITCH(v_rs2, qlhalfmodq, s_idx, e_idx)
//!   v_rs4 = NTT(v_rs3)
//!   v_rs5 = SUB(v_ch0, v_rs4)
//!   v_res = MUL(v_rs5, ql_inv_mod_qi)
template <typename RETV, typename VISITOR>
POLY_LOWER_RETV HP1TOHP2::Handle_rescale(VISITOR*            visitor,
                                         air::base::NODE_PTR node) {
  POLY_LOWER_CTX&       ctx        = visitor->Context();
  fhe::core::LOWER_CTX* lower_ctx  = ctx.Lower_ctx();
  POLY_IR_GEN&          irgen      = ctx.Poly_gen();
  air::base::CONTAINER* cntr       = ctx.Container();
  air::base::SPOS       spos       = node->Spos();
  air::base::TYPE_PTR   t_rns_poly = irgen.Poly_type();

  AIR_ASSERT(irgen.Has_node_var(node) && irgen.Has_node_var(node->Child(0)));
  CONST_VAR v_res = irgen.Node_var(node);
  CONST_VAR v_ch0 = irgen.Node_var(node->Child(0));
  AIR_ASSERT(irgen.Is_type_of(v_res.Type_id(), POLY) &&
             irgen.Is_type_of(v_ch0.Type_id(), POLY));
  uint32_t num_q = irgen.Get_num_q(node->Child(0));
  AIR_ASSERT(num_q > 1);

  // lower child 0
  air::base::NODE_PTR n_ch0 =
      visitor->template Visit<RETV>(node->Child(0)).Node();

  // extract the last basis
  // v_rs0 = Extract(v_ch0, num_q-1, num_q - 1)
  uint32_t            s_idx = num_q - 1;
  uint32_t            e_idx = s_idx;
  CONST_VAR           v_rs0 = irgen.New_poly_preg();
  air::base::NODE_PTR n_rs0 = irgen.New_extract(n_ch0, s_idx, e_idx, spos);
  air::base::STMT_PTR s_rs0 = irgen.New_var_store(n_rs0, v_rs0, spos);
  ctx.Prepend(s_rs0);

  // v_rs1 = INTT(v_rs0)
  CONST_VAR           v_rs1 = irgen.New_poly_preg();
  air::base::NODE_PTR n_rs1 =
      irgen.New_intt(irgen.New_var_load(v_rs0, spos), spos);
  air::base::STMT_PTR s_rs1 = irgen.New_var_store(n_rs1, v_rs1, spos);
  ctx.Prepend(s_rs1);

  // change from flooring to rounding
  // v_rs2 = Add(v_rs1, q_last/2)
  CONST_VAR v_rs2 = irgen.New_poly_preg();
  uint64_t  q_last_half =
      lower_ctx->Get_crt_cst().Get_prime(irgen.Glob_scope(), (num_q - 1)) / 2;

  air::base::NODE_PTR n_half =
      cntr->New_intconst(irgen.Get_type(UINT64), q_last_half, spos);
  air::base::NODE_PTR n_rs2 =
      irgen.New_poly_add(irgen.New_var_load(v_rs1, spos), n_half, spos);
  air::base::STMT_PTR s_rs2 = irgen.New_var_store(n_rs2, v_rs2, spos);
  ctx.Prepend(s_rs2);

  // switch the modulus from q_last to ranges between [s_idx, e_idx]
  // v_rs3 = BSWITCH(v_rs2, qlhalfmodq, s_idx, e_idx)
  CONST_VAR v_rs3                = irgen.New_poly_preg();
  s_idx                          = irgen.Get_sbase(node);
  e_idx                          = s_idx + num_q - 2;
  uint32_t            qlhalf_idx = num_q - 2;
  air::base::NODE_PTR n_rs3      = irgen.New_bswitch(
      irgen.New_var_load(v_rs2, spos), irgen.New_qlhalfmodq(qlhalf_idx, spos),
      s_idx, e_idx, spos);
  air::base::STMT_PTR s_rs3 = irgen.New_var_store(n_rs3, v_rs3, spos);
  ctx.Prepend(s_rs3);

  // v_rs4 = NTT(v_rs3)
  CONST_VAR           v_rs4 = irgen.New_poly_preg();
  air::base::NODE_PTR n_rs4 =
      irgen.New_ntt(irgen.New_var_load(v_rs3, spos), spos);
  air::base::STMT_PTR s_rs4 = irgen.New_var_store(n_rs4, v_rs4, spos);
  ctx.Prepend(s_rs4);

  // v_rs5 = SUB(v_ch0, v_rs4)
  CONST_VAR           v_rs5 = irgen.New_poly_preg();
  air::base::NODE_PTR n_rs5 = irgen.New_poly_sub(
      irgen.New_var_load(v_ch0, spos), irgen.New_var_load(v_rs4, spos), spos);
  air::base::STMT_PTR s_rs5 = irgen.New_var_store(n_rs5, v_rs5, spos);
  ctx.Prepend(s_rs5);

  // v_res = MUL(v_rs5, ql_inv_mod_qi)
  // the index of last prime in ql_inv_mod_qi is num_q - 2
  air::base::NODE_PTR n_qlinvmodq = irgen.New_qlinvmodq(num_q - 2, spos);
  air::base::NODE_PTR n_res =
      irgen.New_poly_mul(irgen.New_var_load(v_rs5, spos), n_qlinvmodq, spos);
  air::base::STMT_PTR s_res = irgen.New_var_store(n_res, v_res, spos);
  ctx.Prepend(s_res);
  return POLY_LOWER_RETV();
}

//! @brief Lowering for HPOLY::Raise_mod
//! Raise_mod is the first step of Bootstrap, which raise the modulus of input
//! level to the required target level
//! v_res = HPOLY.Raise_mod(v_input, raised_level)
//! Lowered to:
//! // unrolled
//! while(v_input.sf_degree > 1) {
//!   v_input = Rescale(v_input)
//! }
//! v_input   = INTT(v_input)
//! v_switch1 = ADD(v_input, q0/2)
//! v_switch2 = BSWITCH(v_input, q0halfmodq, 1, raised_level - 1)
//! v_switch3 = CONCAT(v_input, v_switch2)
//! v_res     = NTT(v_switch3)
template <typename RETV, typename VISITOR>
POLY_LOWER_RETV HP1TOHP2::Handle_raise_mod(VISITOR*            visitor,
                                           air::base::NODE_PTR node) {
  POLY_LOWER_CTX&       ctx       = visitor->Context();
  fhe::core::LOWER_CTX* lower_ctx = ctx.Lower_ctx();
  POLY_IR_GEN&          irgen     = ctx.Poly_gen();
  air::base::SPOS       spos      = node->Spos();
  air::base::CONTAINER* cntr      = ctx.Container();
  air::base::NODE_PTR   opnd0     = node->Child(0);
  air::base::NODE_PTR   opnd1     = node->Child(1);
  CONST_VAR&            v_res     = irgen.Node_var(node);
  CONST_VAR&            v_input   = irgen.Node_var(opnd0);
  uint32_t              num_q     = irgen.Get_num_q(opnd0);

  CMPLR_ASSERT(irgen.Is_type_of(opnd0->Rtype_id(), POLY),
               "raisemod opnd is not RNS_POLY type");
  AIR_ASSERT(opnd1->Opcode() == air::core::OPC_INTCONST);
  AIR_ASSERT_MSG(num_q == 1, "bootstrap raisemod input level should be 1");

  uint32_t raised_level = opnd1->Intconst();
  if (raised_level == 0) {
    raised_level = lower_ctx->Get_ctx_param().Get_mul_level();
  }
  air::base::NODE_PTR opnd0_retv = visitor->template Visit<RETV>(opnd0).Node();

  // Step 1: Rescale the ciphertext if scaling factor greater than 1
  uint32_t sf_degree = irgen.Get_sf_deg(opnd0);
  while (sf_degree > 1) {
    air::base::NODE_PTR n_rs = irgen.New_rescale(opnd0_retv, spos);
    air::base::STMT_PTR s_rs = irgen.New_var_store(n_rs, v_input, spos);
    ctx.Prepend(s_rs);
    sf_degree = sf_degree - 1;
  }

  // Step 2: Perform intt
  air::base::NODE_PTR n_input = irgen.New_var_load(v_input, spos);
  air::base::NODE_PTR n_intt  = irgen.New_intt(n_input, spos);
  air::base::STMT_PTR s_intt  = irgen.New_var_store(n_intt, v_input, spos);
  ctx.Prepend(s_intt);

  // Step3: Switch modulus from q0 to {q0...q_level}
  // change from flooring to rounding
  CONST_VAR v_bswitch1 = irgen.New_poly_preg();
  uint64_t  q0_half =
      lower_ctx->Get_crt_cst().Get_prime(irgen.Glob_scope(), 0) / 2;
  air::base::NODE_PTR n_half =
      cntr->New_intconst(irgen.Get_type(UINT64), q0_half, spos);
  n_input                        = irgen.New_var_load(v_input, spos);
  air::base::NODE_PTR n_bswitch1 = irgen.New_poly_add(n_input, n_half, spos);
  air::base::STMT_PTR s_bswitch1 =
      irgen.New_var_store(n_bswitch1, v_bswitch1, spos);
  ctx.Prepend(s_bswitch1);

  CONST_VAR v_bswitch2           = irgen.New_poly_preg();
  uint32_t  s_idx                = 1;  // start from q1
  uint32_t  e_idx                = raised_level - 1;
  n_bswitch1                     = irgen.New_var_load(v_bswitch1, spos);
  air::base::NODE_PTR n_bswitch2 = irgen.New_bswitch(
      n_bswitch1, irgen.New_q0halfmodq(spos), s_idx, e_idx, spos);
  air::base::STMT_PTR s_bswitch2 =
      irgen.New_var_store(n_bswitch2, v_bswitch2, spos);
  ctx.Prepend(s_bswitch2);

  // Step4: Concat q0 and {q1...q_level}
  CONST_VAR v_concat           = irgen.New_poly_preg();
  n_input                      = irgen.New_var_load(v_input, spos);
  n_bswitch2                   = irgen.New_var_load(v_bswitch2, spos);
  air::base::NODE_PTR n_concat = irgen.New_concat(n_input, n_bswitch2, spos);
  air::base::STMT_PTR s_concat = irgen.New_var_store(n_concat, v_concat, spos);
  ctx.Prepend(s_concat);

  // Step5: Perform ntt
  n_concat                  = irgen.New_var_load(v_concat, spos);
  air::base::NODE_PTR n_ntt = irgen.New_ntt(n_concat, spos);
  air::base::STMT_PTR s_res = irgen.New_var_store(n_ntt, v_res, spos);
  ctx.Prepend(s_res);

  return POLY_LOWER_RETV();
}

//! @brief Handle CORE::STORE
template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CORE2HP2::Handle_st(VISITOR*            visitor,
                                    air::base::NODE_PTR node) {
  POLY_LOWER_CTX& ctx = visitor->Context();

  if (!(node->Child(0)->Is_ld())) {
    ctx.Poly_gen().Add_node_var(node->Child(0), node->Addr_datum());
  }
  return Handle_st_var(visitor, node,
                       VAR(ctx.Func_scope(), ctx.Func_scope()->Addr_datum(
                                                 node->Addr_datum_id())));
}

//! @brief Handle CORE::STP
template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CORE2HP2::Handle_stp(VISITOR*            visitor,
                                     air::base::NODE_PTR node) {
  POLY_LOWER_CTX& ctx = visitor->Context();
  if (!(node->Child(0)->Is_ld())) {
    ctx.Poly_gen().Add_node_var(node->Child(0), node->Preg());
  }
  return Handle_st_var(visitor, node, VAR(ctx.Func_scope(), node->Preg()));
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CORE2HP2::Handle_stf(VISITOR*            visitor,
                                     air::base::NODE_PTR node) {
  POLY_LOWER_CTX& ctx = visitor->Context();
  if (!(node->Child(0)->Is_ld())) {
    ctx.Poly_gen().Add_node_var(node->Child(0), node->Addr_datum(),
                                node->Field());
  }
  return Handle_st_var(
      visitor, node, VAR(ctx.Func_scope(), node->Addr_datum(), node->Field()));
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CORE2HP2::Handle_stpf(VISITOR*            visitor,
                                      air::base::NODE_PTR node) {
  POLY_LOWER_CTX& ctx = visitor->Context();
  if (!(node->Child(0)->Is_ld())) {
    ctx.Poly_gen().Add_node_var(node->Child(0), node->Preg(), node->Field());
  }
  return Handle_st_var(visitor, node,
                       VAR(ctx.Func_scope(), node->Preg(), node->Field()));
}

template <typename VISITOR>
POLY_LOWER_RETV CORE2HP2::Handle_st_var(VISITOR*            visitor,
                                        air::base::NODE_PTR node,
                                        CONST_VAR           var) {
  POLY_LOWER_CTX& ctx = visitor->Context();
  if (ctx.Lower_to_hpoly2(node->Child(0))) {
    POLY_IR_GEN&        irgen = ctx.Poly_gen();
    air::base::NODE_PTR n0    = node->Child(0);
    uint32_t            num_q = irgen.Get_num_q(n0);
    uint32_t            num_p = irgen.Get_num_p(n0);
    // add a comment node for better IR readability
    air::base::STMT_PTR s_cmnt = irgen.New_comment(
        node->Spos(), "%s.%s[%d|%d|%d]",
        air::base::META_INFO::Domain_name(n0->Opcode().Domain()), n0->Name(),
        num_q, num_p, irgen.Get_sbase(n0));
    ctx.Prepend(s_cmnt);

    POLY_LOWER_RETV rhs = visitor->template Visit<POLY_LOWER_RETV>(n0);
    AIR_ASSERT_MSG(rhs.Is_null(),
                   "store should be handled in the child lowering");
    return POLY_LOWER_RETV();
  } else {
    // clone the tree for store var type is not ciph/plain related
    air::base::STMT_PTR s_new =
        ctx.Poly_gen().Container()->Clone_stmt_tree(node->Stmt());
    return POLY_LOWER_RETV(s_new->Node());
  }
}

}  // namespace poly
}  // namespace fhe
#endif
