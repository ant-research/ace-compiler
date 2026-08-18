//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_POLY_CKKS2HPOLY_H
#define FHE_POLY_CKKS2HPOLY_H

#include <stack>

#include "air/base/transform_ctx.h"
#include "air/base/visitor.h"
#include "air/core/default_handler.h"
#include "air/core/handler.h"
#include "fhe/ckks/ckks_handler.h"
#include "fhe/ckks/default_handler.h"
#include "fhe/core/lower_ctx.h"
#include "fhe/poly/config.h"
#include "poly_ir_gen.h"
#include "poly_lower_ctx.h"

namespace fhe {

namespace poly {

class CKKS2HPOLY;
class CORE2HPOLY;
class POLY_LOWER_CTX;
using CKKS2HPOLY_VISITOR =
    air::base::VISITOR<POLY_LOWER_CTX, air::core::HANDLER<CORE2HPOLY>,
                       fhe::ckks::HANDLER<CKKS2HPOLY>>;

//! @brief Lower CKKS IR to HPoly IR
class CKKS2HPOLY : public fhe::ckks::DEFAULT_HANDLER {
public:
  //! @brief Construct a new CKKS2HPOLY object
  CKKS2HPOLY() {}

  //! @brief Handle CKKS_OPERATOR::ADD
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_add(VISITOR* visitor, NODE_PTR node);

  //! @brief Handle CKKS_OPERATOR::MUL
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_mul(VISITOR* visitor, NODE_PTR node);

  //! @brief Handle CKKS_OPERATOR::RELIN
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_relin(VISITOR* visitor, NODE_PTR node);

  //! @brief Handle CKKS_OPERATOR::ROTATE
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_rotate(VISITOR* visitor, NODE_PTR node);

  //! @brief Handle CKKS_OPERATOR::RESCALE
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_rescale(VISITOR* visitor, NODE_PTR node);

  //! @brief Handle CKKS_OPERATOR::ENCODE
  // template <typename RETV, typename VISITOR>
  // POLY_LOWER_RETV Handle_encode(VISITOR* visitor, NODE_PTR node);

  //! @brief Handle CKKS_OPERATOR::MODSWITCH
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_modswitch(VISITOR* visitor, NODE_PTR node);

private:
  POLY_LOWER_RETV Handle_add_ciph(POLY_LOWER_CTX& ctx, NODE_PTR node,
                                  POLY_LOWER_RETV opnd0_pair,
                                  POLY_LOWER_RETV opnd1_pair);

  POLY_LOWER_RETV Handle_add_plain(POLY_LOWER_CTX& ctx, NODE_PTR node,
                                   POLY_LOWER_RETV opnd0_pair,
                                   POLY_LOWER_RETV opnd1_pair);

  POLY_LOWER_RETV Handle_add_float(POLY_LOWER_CTX& ctx, NODE_PTR node,
                                   POLY_LOWER_RETV opnd0_pair,
                                   POLY_LOWER_RETV opnd1_pair);

  POLY_LOWER_RETV Handle_mul_ciph(POLY_LOWER_CTX& ctx, NODE_PTR node,
                                  POLY_LOWER_RETV opnd0_pair,
                                  POLY_LOWER_RETV opnd1_pair);

  POLY_LOWER_RETV Handle_mul_plain(POLY_LOWER_CTX& ctx, NODE_PTR node,
                                   POLY_LOWER_RETV opnd0_pair,
                                   POLY_LOWER_RETV opnd1_pair);

  POLY_LOWER_RETV Handle_mul_float(POLY_LOWER_CTX& ctx, NODE_PTR node,
                                   POLY_LOWER_RETV opnd0_pair,
                                   POLY_LOWER_RETV opnd1_pair);

  // generate IR to allocate memory for keyswitch variables
  // TODO: all memory related op should be postponed to CG phase
  void Handle_kswitch_alloc(POLY_LOWER_CTX& ctx, STMT_LIST& sl, NODE_PTR n_c0,
                            NODE_PTR n_c1, const SPOS& spos);

  // generate IR to free memory for keyswitch variables
  // TODO: all memory related op should be postponed to CG phase
  void Handle_kswitch_free(POLY_LOWER_CTX& ctx, STMT_LIST& sl,
                           const SPOS& spos);

  // generate IR to perform key switch operation
  void Handle_kswitch(POLY_LOWER_CTX& ctx, STMT_LIST& sl, CONST_VAR v_key,
                      NODE_PTR n_c1, const SPOS& spos);

  // generate IR to perform ModDown operation
  // ModDown op will reduce the modulus of a polynomial from Q+P to Q
  void Handle_mod_down(POLY_LOWER_CTX& ctx, STMT_LIST& sl, const SPOS& spos);

  // generate IR to perfom rotate afterwards op of keyswitch
  // res_c0 = input_c0 + mod_down_c0, c1 remains unchanged
  void Handle_rotate_post_keyswitch(POLY_LOWER_CTX& ctx, STMT_LIST& sl,
                                    NODE_PTR n_c0, const SPOS& spos);

  // generate IR to perform polynomial automorphism
  void Handle_automorphism(POLY_LOWER_CTX& ctx, STMT_LIST& sl,
                           CONST_VAR v_rot_res, NODE_PTR n_rot_idx,
                           const SPOS& spos);

  // generate IR to perform relinearlize afterwards op for keyswitch
  // res_c0 = input_c0 + mod_down_c0
  // res_c1 = input_c1 + mod_down_c1
  void Handle_relin_post_keyswitch(POLY_LOWER_CTX& ctx, STMT_LIST& sl,
                                   CONST_VAR v_relin_res, NODE_PTR n_c0,
                                   NODE_PTR n_c1, const SPOS& spos);

  // expand rotate sub operations
  POLY_LOWER_RETV Expand_rotate(POLY_LOWER_CTX& ctx, NODE_PTR node,
                                NODE_PTR n_c0, NODE_PTR n_c1, NODE_PTR n_opnd1,
                                const SPOS& spos);

  // generate IR to call rotate function, create the function if not ready
  void Call_rotate(POLY_LOWER_CTX& ctx, NODE_PTR node, NODE_PTR n_arg0,
                   NODE_PTR n_arg1);

  // generate rotate function, when PGEN:inline_rotate set to false
  void Gen_rotate_func(POLY_LOWER_CTX& ctx, NODE_PTR node, const SPOS& spos);

  // expand relinearize sub options
  POLY_LOWER_RETV Expand_relin(POLY_LOWER_CTX& ctx, NODE_PTR node,
                               NODE_PTR n_c0, NODE_PTR n_c1, NODE_PTR n_c2);

  // generate relinearize function, when PGEN:inline_relin set to false
  void Gen_relin_func(POLY_LOWER_CTX& ctx, NODE_PTR node, const SPOS& spos);

  // generate IR to call relinearize function, create the function if not ready
  void Call_relin(POLY_LOWER_CTX& ctx, NODE_PTR node, NODE_PTR n_arg);

  // Check if RNS loop need to be generated at current node
  bool Is_gen_rns_loop(NODE_PTR parent, NODE_PTR node);

  // Pre handle for ckks operators, generate rns loop if needed
  template <typename VISITOR>
  bool Pre_handle_ckks_op(VISITOR* visitor, NODE_PTR node);

  // Post handle for ckks operators, generate init ciph,
  // add store to result symbol
  template <typename VISITOR>
  POLY_LOWER_RETV Post_handle_ckks_op(VISITOR* visitor, NODE_PTR node,
                                      POLY_LOWER_RETV rhs,
                                      bool            is_gen_rns_loop);

  NODE_PTR Gen_encode_float_from_ciph(POLY_LOWER_CTX& ctx, NODE_PTR node,
                                      CONST_VAR v_ciph, NODE_PTR n_cst,
                                      bool is_mul);
};

//! @brief Lower IR with Core Opcode to Poly
class CORE2HPOLY : public air::core::DEFAULT_HANDLER {
public:
  //! @brief Construct a new CKKS2HPOLY object
  CORE2HPOLY() {}

  //! @brief Handle CORE::LOAD
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_ld(VISITOR* visitor, NODE_PTR node);

  //! @brief Handle CORE::LDP
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_ldp(VISITOR* visitor, NODE_PTR node);

  //! @brief Handle CORE::ILD
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_ild(VISITOR* visitor, NODE_PTR node);

  //! @brief Handle CORE::STORE
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_st(VISITOR* visitor, NODE_PTR node);

  //! @brief Handle CORE::STP
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_stp(VISITOR* visitor, NODE_PTR node);

  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_retv(VISITOR* visitor, NODE_PTR node);

private:
  template <typename VISITOR>
  POLY_LOWER_RETV Handle_ld_var(VISITOR* visitor, NODE_PTR node);

  template <typename VISITOR>
  POLY_LOWER_RETV Handle_st_var(VISITOR* visitor, NODE_PTR node, CONST_VAR var);
};

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CKKS2HPOLY::Handle_add(VISITOR* visitor, NODE_PTR node) {
  POLY_LOWER_RETV       retv;
  POLY_LOWER_CTX&       ctx       = visitor->Context();
  fhe::core::LOWER_CTX* lower_ctx = ctx.Lower_ctx();

  // visit add's two child node
  NODE_PTR        opnd0      = node->Child(0);
  NODE_PTR        opnd1      = node->Child(1);
  POLY_LOWER_RETV opnd0_pair = visitor->template Visit<RETV>(opnd0);
  POLY_LOWER_RETV opnd1_pair = visitor->template Visit<RETV>(opnd1);

  if (lower_ctx->Is_cipher_type(opnd1->Rtype_id())) {
    retv = Handle_add_ciph(ctx, node, opnd0_pair, opnd1_pair);
  } else if (lower_ctx->Is_plain_type(opnd1->Rtype_id())) {
    retv = Handle_add_plain(ctx, node, opnd0_pair, opnd1_pair);
  } else if (opnd1->Rtype()->Is_float()) {
    retv = Handle_add_float(ctx, node, opnd0_pair, opnd1_pair);
  } else {
    CMPLR_ASSERT(false, "invalid add opnd_1 type");
    retv = POLY_LOWER_RETV();
  }
  STMT_PTR init_stmt =
      ctx.Poly_gen().New_init_ciph(ctx.Poly_gen().Node_var(node), node);
  if (init_stmt != air::base::Null_ptr) {
    ctx.Prepend(init_stmt);
  }
  return retv;
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CKKS2HPOLY::Handle_mul(VISITOR* visitor, NODE_PTR node) {
  POLY_LOWER_RETV       retv;
  POLY_LOWER_CTX&       ctx       = visitor->Context();
  fhe::core::LOWER_CTX* lower_ctx = ctx.Lower_ctx();
  NODE_PTR              opnd0     = node->Child(0);
  NODE_PTR              opnd1     = node->Child(1);

  CMPLR_ASSERT(lower_ctx->Is_cipher_type(opnd0->Rtype_id()),
               "invalid mul opnd0");
  POLY_LOWER_RETV opnd0_pair = visitor->template Visit<RETV>(opnd0);
  POLY_LOWER_RETV opnd1_pair = visitor->template Visit<RETV>(opnd1);

  if (lower_ctx->Is_cipher_type(opnd1->Rtype_id())) {
    retv = Handle_mul_ciph(ctx, node, opnd0_pair, opnd1_pair);
  } else if (lower_ctx->Is_plain_type(opnd1->Rtype_id())) {
    retv = Handle_mul_plain(ctx, node, opnd0_pair, opnd1_pair);
  } else if (opnd1->Rtype()->Is_float()) {
    retv = Handle_mul_float(ctx, node, opnd0_pair, opnd1_pair);
  } else {
    CMPLR_ASSERT(false, "invalid mul opnd_1 type");
    retv = POLY_LOWER_RETV();
  }
  STMT_PTR init_stmt =
      ctx.Poly_gen().New_init_ciph(ctx.Poly_gen().Node_var(node), node);
  if (init_stmt != air::base::Null_ptr) {
    ctx.Prepend(init_stmt);
  }
  return retv;
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CKKS2HPOLY::Handle_relin(VISITOR* visitor, NODE_PTR node) {
  POLY_LOWER_CTX& ctx  = visitor->Context();
  CONTAINER*      cntr = ctx.Container();
  SPOS            spos = node->Spos();

  NODE_PTR n_opnd0 = node->Child(0);
  CMPLR_ASSERT(ctx.Lower_ctx()->Is_cipher3_type(n_opnd0->Rtype_id()),
               "invalid relin opnd");

  POLY_LOWER_RETV opnd0_retv = visitor->template Visit<RETV>(n_opnd0);
  CMPLR_ASSERT(
      opnd0_retv.Kind() == RETV_KIND::RK_CIPH3_POLY && !opnd0_retv.Is_null(),
      "invalid relin opnd0");

  STMT_PTR init_stmt =
      ctx.Poly_gen().New_init_ciph(ctx.Poly_gen().Node_var(node), node);
  if (init_stmt != air::base::Null_ptr) {
    ctx.Prepend(init_stmt);
  }
  NODE_PTR n_ret = air::base::Null_ptr;
  if (ctx.Config().Inline_relin()) {
    return Expand_relin(ctx, node, opnd0_retv.Node1(), opnd0_retv.Node2(),
                        opnd0_retv.Node3());
  } else {
    CMPLR_ASSERT(false, "not supported yet");
  }
  return POLY_LOWER_RETV(RETV_KIND::RK_BLOCK, air::base::Null_ptr);
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CKKS2HPOLY::Handle_rotate(VISITOR* visitor, NODE_PTR node) {
  POLY_LOWER_CTX& ctx  = visitor->Context();
  CONTAINER*      cntr = ctx.Container();
  SPOS            spos = node->Spos();

  NODE_PTR n_opnd0 = node->Child(0);
  NODE_PTR n_opnd1 = node->Child(1);

  // TO FIX: opnd0_retv already returned pair of polys which will not be used
  // if gen rotate function
  POLY_LOWER_RETV opnd0_retv = visitor->template Visit<RETV>(n_opnd0);
  POLY_LOWER_RETV opnd1_retv = visitor->template Visit<RETV>(n_opnd1);
  CMPLR_ASSERT(
      opnd0_retv.Kind() == RETV_KIND::RK_CIPH_POLY && !opnd0_retv.Is_null(),
      "invalid rotate opnd0");
  CMPLR_ASSERT(
      opnd1_retv.Kind() == RETV_KIND::RK_DEFAULT && !opnd0_retv.Is_null(),
      "invalid rotate opnd1");

  STMT_PTR init_stmt =
      ctx.Poly_gen().New_init_ciph(ctx.Poly_gen().Node_var(node), node);
  if (init_stmt != air::base::Null_ptr) ctx.Prepend(init_stmt);
  if (ctx.Config().Inline_rotate()) {
    return Expand_rotate(ctx, node, opnd0_retv.Node1(), opnd0_retv.Node2(),
                         opnd1_retv.Node(), spos);
  } else {
    CMPLR_ASSERT(false, "not supported yet");
  }
  return POLY_LOWER_RETV(RETV_KIND::RK_BLOCK, air::base::Null_ptr);
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CKKS2HPOLY::Handle_rescale(VISITOR* visitor, NODE_PTR node) {
  POLY_LOWER_CTX&       ctx       = visitor->Context();
  fhe::core::LOWER_CTX* lower_ctx = ctx.Lower_ctx();
  NODE_PTR              opnd0     = node->Child(0);
  POLY_LOWER_RETV       retv;

  CMPLR_ASSERT(lower_ctx->Is_cipher_type(opnd0->Rtype_id()) ||
                   lower_ctx->Is_cipher3_type(opnd0->Rtype_id()),
               "rescale opnd is not ciphertext/ciphertext3");

  POLY_LOWER_RETV opnd0_retv = visitor->template Visit<RETV>(opnd0);

  if (opnd0_retv.Kind() == RK_CIPH_POLY) {
    NODE_PTR n_c0 =
        ctx.Poly_gen().New_rescale(opnd0_retv.Node1(), node->Spos());
    NODE_PTR n_c1 =
        ctx.Poly_gen().New_rescale(opnd0_retv.Node2(), node->Spos());
    retv = POLY_LOWER_RETV(RETV_KIND::RK_CIPH_POLY, n_c0, n_c1);
  } else if (opnd0_retv.Kind() == RK_CIPH3_POLY) {
    NODE_PTR n_c0 =
        ctx.Poly_gen().New_rescale(opnd0_retv.Node1(), node->Spos());
    NODE_PTR n_c1 =
        ctx.Poly_gen().New_rescale(opnd0_retv.Node2(), node->Spos());
    NODE_PTR n_c2 =
        ctx.Poly_gen().New_rescale(opnd0_retv.Node3(), node->Spos());
    retv = POLY_LOWER_RETV(RETV_KIND::RK_CIPH3_POLY, n_c0, n_c1, n_c2);
  } else {
    AIR_ASSERT_MSG(false, "unexpected retv");
  }

  STMT_PTR init_stmt =
      ctx.Poly_gen().New_init_ciph(ctx.Poly_gen().Node_var(node), node);
  if (init_stmt != air::base::Null_ptr) {
    ctx.Prepend(init_stmt);
  }
  return retv;
}

//! @brief Handle CKKS_OPERATOR::MODSWITCH
template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CKKS2HPOLY::Handle_modswitch(VISITOR* visitor, NODE_PTR node) {
  POLY_LOWER_CTX&       ctx       = visitor->Context();
  fhe::core::LOWER_CTX* lower_ctx = ctx.Lower_ctx();
  POLY_IR_GEN&          pgen      = ctx.Poly_gen();
  NODE_PTR              opnd0     = node->Child(0);
  POLY_LOWER_RETV       retv;

  CMPLR_ASSERT(lower_ctx->Is_cipher_type(opnd0->Rtype_id()) ||
                   lower_ctx->Is_cipher3_type(opnd0->Rtype_id()),
               "modswitch opnd is not ciphertext/ciphertext3");

  POLY_LOWER_RETV opnd0_retv = visitor->template Visit<RETV>(opnd0);

  if (opnd0_retv.Kind() == RK_CIPH_POLY) {
    NODE_PTR n_c0 = pgen.New_modswitch(opnd0_retv.Node1(), node->Spos());
    NODE_PTR n_c1 = pgen.New_modswitch(opnd0_retv.Node2(), node->Spos());
    retv          = POLY_LOWER_RETV(opnd0_retv.Kind(), n_c0, n_c1);
  } else if (opnd0_retv.Kind() == RK_CIPH3_POLY) {
    NODE_PTR n_c0 = pgen.New_modswitch(opnd0_retv.Node1(), node->Spos());
    NODE_PTR n_c1 = pgen.New_modswitch(opnd0_retv.Node2(), node->Spos());
    NODE_PTR n_c2 = pgen.New_modswitch(opnd0_retv.Node3(), node->Spos());
    retv          = POLY_LOWER_RETV(opnd0_retv.Kind(), n_c0, n_c1, n_c2);
  } else {
    AIR_ASSERT_MSG(false, "unexpected retv");
  }
  STMT_PTR init_stmt = pgen.New_init_ciph(pgen.Node_var(node), node);
  if (init_stmt != air::base::Null_ptr) {
    ctx.Prepend(init_stmt);
  }
  return retv;
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CORE2HPOLY::Handle_ld(VISITOR* visitor, NODE_PTR node) {
  return Handle_ld_var(visitor, node);
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CORE2HPOLY::Handle_ldp(VISITOR* visitor, NODE_PTR node) {
  return Handle_ld_var(visitor, node);
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CORE2HPOLY::Handle_ild(VISITOR* visitor, NODE_PTR node) {
  POLY_LOWER_CTX& ctx  = visitor->Context();
  POLY_LOWER_RETV retv = Handle_ld_var(visitor, node);
  if (retv.Kind() != RETV_KIND::RK_DEFAULT) {
    // save ild to temp
    CONST_VAR& v_ild   = ctx.Poly_gen().Node_var(node);
    NODE_PTR   n_clone = ctx.Container()->Clone_node_tree(node);
    STMT_PTR s_ild = ctx.Poly_gen().New_var_store(n_clone, v_ild, node->Spos());
    ctx.Prepend(s_ild);
  }
  return retv;
}

template <typename VISITOR>
POLY_LOWER_RETV CORE2HPOLY::Handle_ld_var(VISITOR* visitor, NODE_PTR node) {
  TYPE_ID               tid       = node->Rtype_id();
  POLY_LOWER_CTX&       ctx       = visitor->Context();
  fhe::core::LOWER_CTX* lower_ctx = ctx.Lower_ctx();
  air::base::SPOS       spos      = node->Spos();
  POLY_IR_GEN&          pgen      = ctx.Poly_gen();
  if (ctx.Lower_to_poly(node, visitor->Parent(1))) {
    if (lower_ctx->Is_cipher_type(tid)) {
      CONST_VAR& v_node = pgen.Node_var(node);
      NODE_PAIR  n_pair = pgen.New_ciph_poly_load(v_node, false, spos);
      return POLY_LOWER_RETV(RETV_KIND::RK_CIPH_POLY, n_pair.first,
                             n_pair.second);
    } else if (lower_ctx->Is_cipher3_type(tid)) {
      CONST_VAR&  v_node  = pgen.Node_var(node);
      NODE_TRIPLE n_tuple = pgen.New_ciph3_poly_load(v_node, false, spos);
      return POLY_LOWER_RETV(RETV_KIND::RK_CIPH3_POLY, std::get<0>(n_tuple),
                             std::get<1>(n_tuple), std::get<2>(n_tuple));
    } else if (lower_ctx->Is_plain_type(tid)) {
      CONST_VAR& v_node  = pgen.Node_var(node);
      NODE_PTR   n_plain = pgen.New_plain_poly_load(v_node, false, spos);
      return POLY_LOWER_RETV(RETV_KIND::RK_PLAIN_POLY, n_plain);
    } else {
      AIR_ASSERT_MSG(false, "unexpected type");
      return POLY_LOWER_RETV();
    }
  } else {
    // for load node do not lower to poly domain, clone the whole tree
    NODE_PTR n_clone = ctx.Container()->Clone_node_tree(node);
    return POLY_LOWER_RETV(n_clone);
  }
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CORE2HPOLY::Handle_st(VISITOR* visitor, NODE_PTR node) {
  POLY_LOWER_CTX& ctx = visitor->Context();

  if (!(node->Child(0)->Is_ld())) {
    ctx.Poly_gen().Add_node_var(node->Child(0), node->Addr_datum());
  }
  return Handle_st_var(visitor, node,
                       VAR(ctx.Func_scope(), ctx.Func_scope()->Addr_datum(
                                                 node->Addr_datum_id())));
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CORE2HPOLY::Handle_stp(VISITOR* visitor, NODE_PTR node) {
  POLY_LOWER_CTX& ctx = visitor->Context();

  if (!(node->Child(0)->Is_ld())) {
    ctx.Poly_gen().Add_node_var(node->Child(0), node->Preg());
  }
  return Handle_st_var(visitor, node, VAR(ctx.Func_scope(), node->Preg()));
}

template <typename VISITOR>
POLY_LOWER_RETV CORE2HPOLY::Handle_st_var(VISITOR* visitor, NODE_PTR node,
                                          CONST_VAR var) {
  TYPE_ID               tid       = var.Type_id();
  POLY_LOWER_CTX&       ctx       = visitor->Context();
  fhe::core::LOWER_CTX* lower_ctx = ctx.Lower_ctx();
  if (!(lower_ctx->Is_cipher_type(tid) || lower_ctx->Is_cipher3_type(tid) ||
        lower_ctx->Is_plain_type(tid)) ||
      node->Child(0)->Is_ld()) {
    // clone the tree for
    // 1) store var type is not ciph/plain related
    // 2) load/ldp for ciph/plain, keep store(res, ciph)
    //    do not lower to store polys for now
    STMT_PTR s_new = ctx.Poly_gen().Container()->Clone_stmt_tree(node->Stmt());
    return POLY_LOWER_RETV(s_new->Node());
  }
  POLY_LOWER_RETV rhs =
      visitor->template Visit<POLY_LOWER_RETV>(node->Child(0));
  if (rhs.Kind() == RETV_KIND::RK_CIPH_POLY) {
    CMPLR_ASSERT(lower_ctx->Is_cipher_type(tid), "store symbol is not ciph");
    STMT_PAIR s_pair = ctx.Poly_gen().New_ciph_poly_store(
        var, rhs.Node1(), rhs.Node2(), false, node->Spos());
    ctx.Prepend(s_pair.first);
    ctx.Prepend(s_pair.second);
    return POLY_LOWER_RETV(RETV_KIND::RK_BLOCK, air::base::Null_ptr);
  } else if (rhs.Kind() == RETV_KIND::RK_CIPH3_POLY) {
    CMPLR_ASSERT(lower_ctx->Is_cipher3_type(tid), "store symbol is not ciph3");
    STMT_TRIPLE s_tuple = ctx.Poly_gen().New_ciph3_poly_store(
        var, rhs.Node1(), rhs.Node2(), rhs.Node3(), false, node->Spos());
    ctx.Prepend(std::get<0>(s_tuple));
    ctx.Prepend(std::get<1>(s_tuple));
    ctx.Prepend(std::get<2>(s_tuple));
    return POLY_LOWER_RETV(RETV_KIND::RK_BLOCK, air::base::Null_ptr);
  } else if (rhs.Kind() == RETV_KIND::RK_PLAIN_POLY) {
    CMPLR_ASSERT(lower_ctx->Is_plain_type(tid), "store symbol is not plain");
    STMT_PTR s_plain = ctx.Poly_gen().New_plain_poly_store(var, rhs.Node1(),
                                                           false, node->Spos());
    ctx.Prepend(s_plain);
    return POLY_LOWER_RETV(RETV_KIND::RK_BLOCK, air::base::Null_ptr);
  } else {
    // clone the tree for
    // 1) store var type is not ciph/plain related
    // 2) load/ldp for ciph/plain, keep store(res, ciph)
    //    do not lower to store polys for now
    STMT_PTR s_new = ctx.Poly_gen().Container()->Clone_stmt_tree(node->Stmt());
    return POLY_LOWER_RETV(s_new->Node());
  }
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CORE2HPOLY::Handle_retv(VISITOR* visitor, NODE_PTR node) {
  POLY_LOWER_CTX& ctx     = visitor->Context();
  NODE_PTR        n_opnd0 = node->Child(0);
  AIR_ASSERT_MSG(n_opnd0->Domain() != fhe::ckks::CKKS_DOMAIN::ID,
                 "CKKS domain opnd0 not supported");
  // do not lower retv statement
  STMT_PTR s_new = ctx.Container()->Clone_stmt_tree(node->Stmt());
  return POLY_LOWER_RETV(s_new->Node());
}

}  // namespace poly
}  // namespace fhe

#endif  // FHE_POLY_CKKS2HPOLY_H
