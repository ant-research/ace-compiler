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

#include "air/base/container.h"
#include "air/base/container_decl.h"
#include "air/base/spos.h"
#include "air/base/transform_ctx.h"
#include "air/base/visitor.h"
#include "air/core/default_handler.h"
#include "air/core/handler.h"
#include "air/util/debug.h"
#include "fhe/ckks/ckks_handler.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/ckks/default_handler.h"
#include "fhe/core/lower_ctx.h"
#include "fhe/poly/config.h"
#include "fhe/poly/opcode.h"
#include "poly_ir_gen.h"
#include "poly_lower_ctx.h"

namespace fhe {
namespace poly {

class CKKS2HPOLY;
class CORE2HPOLY;
using CKKS2HPOLY_VISITOR =
    air::base::VISITOR<POLY_LOWER_CTX, air::core::HANDLER<CORE2HPOLY>,
                       fhe::ckks::HANDLER<CKKS2HPOLY>>;

//! @brief Lower CKKS IR to HPOLY IR
class CKKS2HPOLY : public fhe::ckks::DEFAULT_HANDLER {
public:
  //! @brief Construct a new CKKS2HPOLY object
  CKKS2HPOLY() {}

  //! @brief Handle CKKS_OPERATOR::ADD
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_add(VISITOR* visitor, air::base::NODE_PTR node);

  //! @brief Handle CKKS_OPERATOR::SUB
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_sub(VISITOR* visitor, air::base::NODE_PTR node);

  //! @brief Handle CKKS_OPERATOR::MUL
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_mul(VISITOR* visitor, air::base::NODE_PTR node);

  //! @brief Handle CKKS_OPERATOR::RELIN
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_relin(VISITOR* visitor, air::base::NODE_PTR node);

  //! @brief Handle CKKS_OPERATOR::ROTATE
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_rotate(VISITOR* visitor, air::base::NODE_PTR node);

  //! @brief Handle CKKS_OPERATOR::RESCALE
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_rescale(VISITOR* visitor, air::base::NODE_PTR node);

  //! @brief Handle CKKS_OPERATOR::MODSWITCH
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_modswitch(VISITOR* visitor, air::base::NODE_PTR node);

  //! @brief Handle CKKS_OPERATOR::RAISE_MOD
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_raise_mod(VISITOR* visitor, air::base::NODE_PTR node);

private:
  //! @brief Handle binary arithmetic operations for CKKS_OPERATOR::ADD/SUB/MUL:
  //! cc_op_handler: handle operations between ciphertexts.
  //! cp_op_handler: handle operations between ciphertext and plaintext.
  //! cf_op_handler: handle operations between ciphertext and float.
  template <typename RETV, typename VISITOR, typename OP_HANDLER>
  POLY_LOWER_RETV Handle_bin_arith(VISITOR* visitor, air::base::NODE_PTR node,
                                   OP_HANDLER cc_op_handler,
                                   OP_HANDLER cp_op_handler,
                                   OP_HANDLER cf_op_handler);
  //! @brief Handle binary arithmetic CKKS_OPERATOR::ADD/SUB/MUL between
  //! ciphertexts.
  POLY_LOWER_RETV Handle_bin_arith_cc(POLY_LOWER_CTX&     ctx,
                                      air::base::NODE_PTR node,
                                      POLY_LOWER_RETV     opnd0_pair,
                                      POLY_LOWER_RETV opnd1_pair, OPCODE opc);
  //! @brief Handle binary arithmetic CKKS_OPERATOR::ADD/SUB/MUL between
  //! ciphertext and plaintext.
  POLY_LOWER_RETV Handle_bin_arith_cp(POLY_LOWER_CTX&     ctx,
                                      air::base::NODE_PTR node,
                                      POLY_LOWER_RETV     opnd0_pair,
                                      POLY_LOWER_RETV opnd1_pair, OPCODE opc);
  //! @brief Handle binary arithmetic CKKS_OPERATOR::ADD/SUB/MUL between
  //! ciphertext and float.
  POLY_LOWER_RETV Handle_bin_arith_cf(POLY_LOWER_CTX&     ctx,
                                      air::base::NODE_PTR node,
                                      POLY_LOWER_RETV     opnd0_pair,
                                      POLY_LOWER_RETV opnd1_pair, OPCODE opc);
  POLY_LOWER_RETV Handle_add_ciph(POLY_LOWER_CTX& ctx, air::base::NODE_PTR node,
                                  POLY_LOWER_RETV opnd0_pair,
                                  POLY_LOWER_RETV opnd1_pair) {
    return Handle_bin_arith_cc(ctx, node, opnd0_pair, opnd1_pair, ADD);
  }

  POLY_LOWER_RETV Handle_add_plain(POLY_LOWER_CTX&     ctx,
                                   air::base::NODE_PTR node,
                                   POLY_LOWER_RETV     opnd0_pair,
                                   POLY_LOWER_RETV     opnd1_pair) {
    return Handle_bin_arith_cp(ctx, node, opnd0_pair, opnd1_pair, ADD);
  }

  POLY_LOWER_RETV Handle_add_float(POLY_LOWER_CTX&     ctx,
                                   air::base::NODE_PTR node,
                                   POLY_LOWER_RETV     opnd0_pair,
                                   POLY_LOWER_RETV     opnd1_pair) {
    return Handle_bin_arith_cf(ctx, node, opnd0_pair, opnd1_pair, ADD);
  }

  POLY_LOWER_RETV Handle_sub_ciph(POLY_LOWER_CTX& ctx, air::base::NODE_PTR node,
                                  POLY_LOWER_RETV opnd0_pair,
                                  POLY_LOWER_RETV opnd1_pair) {
    return Handle_bin_arith_cc(ctx, node, opnd0_pair, opnd1_pair, SUB);
  }

  POLY_LOWER_RETV Handle_sub_plain(POLY_LOWER_CTX&     ctx,
                                   air::base::NODE_PTR node,
                                   POLY_LOWER_RETV     opnd0_pair,
                                   POLY_LOWER_RETV     opnd1_pair) {
    return Handle_bin_arith_cp(ctx, node, opnd0_pair, opnd1_pair, SUB);
  }

  POLY_LOWER_RETV Handle_sub_float(POLY_LOWER_CTX&     ctx,
                                   air::base::NODE_PTR node,
                                   POLY_LOWER_RETV     opnd0_pair,
                                   POLY_LOWER_RETV     opnd1_pair) {
    return Handle_bin_arith_cf(ctx, node, opnd0_pair, opnd1_pair, SUB);
  }

  POLY_LOWER_RETV Handle_mul_ciph(POLY_LOWER_CTX& ctx, air::base::NODE_PTR node,
                                  POLY_LOWER_RETV opnd0_pair,
                                  POLY_LOWER_RETV opnd1_pair);

  POLY_LOWER_RETV Handle_mul_plain(POLY_LOWER_CTX&     ctx,
                                   air::base::NODE_PTR node,
                                   POLY_LOWER_RETV     opnd0_pair,
                                   POLY_LOWER_RETV     opnd1_pair);

  POLY_LOWER_RETV Handle_mul_float(POLY_LOWER_CTX&     ctx,
                                   air::base::NODE_PTR node,
                                   POLY_LOWER_RETV     opnd0_pair,
                                   POLY_LOWER_RETV     opnd1_pair);

  // expand rotate sub operations
  POLY_LOWER_RETV Expand_rotate(POLY_LOWER_CTX& ctx, air::base::NODE_PTR node,
                                air::base::NODE_PTR    n_c0,
                                air::base::NODE_PTR    n_c1,
                                air::base::NODE_PTR    n_opnd1,
                                const air::base::SPOS& spos);

  // expand relinearize sub options
  POLY_LOWER_RETV Expand_relin(POLY_LOWER_CTX& ctx, air::base::NODE_PTR node,
                               air::base::NODE_PTR n_c0,
                               air::base::NODE_PTR n_c1,
                               air::base::NODE_PTR n_c2);

  // generate IR for encoding single float value
  air::base::NODE_PTR Gen_encode_float_from_ciph(POLY_LOWER_CTX&     ctx,
                                                 air::base::NODE_PTR node,
                                                 CONST_VAR           v_ciph,
                                                 air::base::NODE_PTR n_cst,
                                                 bool                is_mul);
};

//! @brief Lower IR with Core Opcode to HPoly
class CORE2HPOLY : public air::core::DEFAULT_HANDLER {
public:
  //! @brief Construct a new CKKS2HPOLY object
  CORE2HPOLY() {}

  //! @brief Handle CORE::LOAD
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_ld(VISITOR* visitor, air::base::NODE_PTR node);

  //! @brief Handle CORE::LDP
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_ldp(VISITOR* visitor, air::base::NODE_PTR node);

  //! @brief Handle CORE::ILD
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_ild(VISITOR* visitor, air::base::NODE_PTR node);

  //! @brief Handle CORE::ST
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_st(VISITOR* visitor, air::base::NODE_PTR node);

  //! @brief Handle CORE::STP
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_stp(VISITOR* visitor, air::base::NODE_PTR node);

  //! @brief Handle CORE::IST
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_ist(VISITOR* visitor, air::base::NODE_PTR node);

  //! @brief Handle CORE::zero
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_zero(VISITOR* visitor, air::base::NODE_PTR node);

private:
  template <typename VISITOR>
  POLY_LOWER_RETV Handle_ld_var(VISITOR* visitor, air::base::NODE_PTR node);

  template <typename VISITOR>
  POLY_LOWER_RETV Handle_st_var(VISITOR* visitor, air::base::NODE_PTR node,
                                CONST_VAR var);

  template <typename VISITOR>
  POLY_LOWER_RETV Handle_ild_ist_addr(VISITOR*            visitor,
                                      air::base::NODE_PTR addr);
};

//! @brief Handle binary arithmetic operations for CKKS_OPERATOR::ADD/SUB/MUL:
//! cc_op_handler: handle operations between ciphertexts.
//! cp_op_handler: handle operations between ciphertext and plaintext.
//! cf_op_handler: handle operations between ciphertext and float.
template <typename RETV, typename VISITOR, typename OP_HANDLER>
POLY_LOWER_RETV CKKS2HPOLY::Handle_bin_arith(VISITOR*            visitor,
                                             air::base::NODE_PTR node,
                                             OP_HANDLER          cc_op_handler,
                                             OP_HANDLER          cp_op_handler,
                                             OP_HANDLER cf_op_handler) {
  air::base::OPCODE opc = node->Opcode();
  AIR_ASSERT(opc == ckks::OPC_ADD || opc == ckks::OPC_SUB ||
             opc == ckks::OPC_MUL);
  POLY_LOWER_RETV       retv;
  POLY_LOWER_CTX&       ctx       = visitor->Context();
  fhe::core::LOWER_CTX* lower_ctx = ctx.Lower_ctx();

  // visit 2 child nodes
  air::base::NODE_PTR opnd0      = node->Child(0);
  air::base::NODE_PTR opnd1      = node->Child(1);
  POLY_LOWER_RETV     opnd0_pair = visitor->template Visit<RETV>(opnd0);
  POLY_LOWER_RETV     opnd1_pair = visitor->template Visit<RETV>(opnd1);

  if (lower_ctx->Is_cipher_type(opnd1->Rtype_id())) {
    retv = (this->*cc_op_handler)(ctx, node, opnd0_pair, opnd1_pair);
  } else if (lower_ctx->Is_plain_type(opnd1->Rtype_id())) {
    retv = (this->*cp_op_handler)(ctx, node, opnd0_pair, opnd1_pair);
  } else if (opnd1->Rtype()->Is_float()) {
    retv = (this->*cf_op_handler)(ctx, node, opnd0_pair, opnd1_pair);
  } else {
    AIR_ASSERT_MSG(false, "invalid opnd_1 type of ", node->Name());
    retv = POLY_LOWER_RETV();
  }
  return retv;
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CKKS2HPOLY::Handle_add(VISITOR*            visitor,
                                       air::base::NODE_PTR node) {
  return Handle_bin_arith<RETV>(visitor, node, &CKKS2HPOLY::Handle_add_ciph,
                                &CKKS2HPOLY::Handle_add_plain,
                                &CKKS2HPOLY::Handle_add_float);
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CKKS2HPOLY::Handle_sub(VISITOR*            visitor,
                                       air::base::NODE_PTR node) {
  return Handle_bin_arith<RETV>(visitor, node, &CKKS2HPOLY::Handle_sub_ciph,
                                &CKKS2HPOLY::Handle_sub_plain,
                                &CKKS2HPOLY::Handle_sub_float);
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CKKS2HPOLY::Handle_mul(VISITOR*            visitor,
                                       air::base::NODE_PTR node) {
  POLY_LOWER_CTX&       ctx       = visitor->Context();
  fhe::core::LOWER_CTX* lower_ctx = ctx.Lower_ctx();
  air::base::NODE_PTR   opnd0     = node->Child(0);

  CMPLR_ASSERT(lower_ctx->Is_cipher_type(opnd0->Rtype_id()),
               "invalid mul opnd0");
  return Handle_bin_arith<RETV>(visitor, node, &CKKS2HPOLY::Handle_mul_ciph,
                                &CKKS2HPOLY::Handle_mul_plain,
                                &CKKS2HPOLY::Handle_mul_float);
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CKKS2HPOLY::Handle_relin(VISITOR*            visitor,
                                         air::base::NODE_PTR node) {
  POLY_LOWER_CTX&       ctx  = visitor->Context();
  air::base::CONTAINER* cntr = ctx.Container();
  air::base::SPOS       spos = node->Spos();

  air::base::NODE_PTR n_opnd0 = node->Child(0);
  CMPLR_ASSERT(ctx.Lower_ctx()->Is_cipher3_type(n_opnd0->Rtype_id()),
               "invalid relin opnd");

  POLY_LOWER_RETV opnd0_retv = visitor->template Visit<RETV>(n_opnd0);
  CMPLR_ASSERT(
      opnd0_retv.Kind() == RETV_KIND::RK_CIPH3_POLY && !opnd0_retv.Is_null(),
      "invalid relin opnd0");
  air::base::NODE_PTR n_ret = air::base::Null_ptr;
  if (ctx.Config().Inline_relin()) {
    return Expand_relin(ctx, node, opnd0_retv.Node1(), opnd0_retv.Node2(),
                        opnd0_retv.Node3());
  } else {
    AIR_ASSERT_MSG(false, "not supported yet");
  }
  return POLY_LOWER_RETV(RETV_KIND::RK_BLOCK, air::base::Null_ptr);
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CKKS2HPOLY::Handle_rotate(VISITOR*            visitor,
                                          air::base::NODE_PTR node) {
  POLY_LOWER_CTX&       ctx  = visitor->Context();
  air::base::CONTAINER* cntr = ctx.Container();
  air::base::SPOS       spos = node->Spos();

  air::base::NODE_PTR n_opnd0 = node->Child(0);
  air::base::NODE_PTR n_opnd1 = node->Child(1);

  POLY_LOWER_RETV opnd0_retv = visitor->template Visit<RETV>(n_opnd0);
  POLY_LOWER_RETV opnd1_retv = visitor->template Visit<RETV>(n_opnd1);
  CMPLR_ASSERT(
      opnd0_retv.Kind() == RETV_KIND::RK_CIPH_POLY && !opnd0_retv.Is_null(),
      "invalid rotate opnd0");
  CMPLR_ASSERT(
      opnd1_retv.Kind() == RETV_KIND::RK_DEFAULT && !opnd1_retv.Is_null(),
      "invalid rotate opnd1");
  if (ctx.Config().Inline_rotate()) {
    return Expand_rotate(ctx, node, opnd0_retv.Node1(), opnd0_retv.Node2(),
                         opnd1_retv.Node(), spos);
  } else {
    CMPLR_ASSERT(false, "not supported yet");
  }
  return POLY_LOWER_RETV(RETV_KIND::RK_BLOCK, air::base::Null_ptr);
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CKKS2HPOLY::Handle_rescale(VISITOR*            visitor,
                                           air::base::NODE_PTR node) {
  POLY_LOWER_CTX&       ctx       = visitor->Context();
  fhe::core::LOWER_CTX* lower_ctx = ctx.Lower_ctx();
  air::base::NODE_PTR   opnd0     = node->Child(0);
  POLY_LOWER_RETV       retv;

  CMPLR_ASSERT(lower_ctx->Is_cipher_type(opnd0->Rtype_id()) ||
                   lower_ctx->Is_cipher3_type(opnd0->Rtype_id()),
               "rescale opnd is not ciphertext/ciphertext3");

  POLY_LOWER_RETV opnd0_retv = visitor->template Visit<RETV>(opnd0);

  if (opnd0_retv.Kind() == RK_CIPH_POLY) {
    air::base::NODE_PTR n_c0 =
        ctx.Poly_gen().New_rescale(opnd0_retv.Node1(), node->Spos());
    air::base::NODE_PTR n_c1 =
        ctx.Poly_gen().New_rescale(opnd0_retv.Node2(), node->Spos());
    retv = POLY_LOWER_RETV(RETV_KIND::RK_CIPH_POLY, n_c0, n_c1);
  } else if (opnd0_retv.Kind() == RK_CIPH3_POLY) {
    air::base::NODE_PTR n_c0 =
        ctx.Poly_gen().New_rescale(opnd0_retv.Node1(), node->Spos());
    air::base::NODE_PTR n_c1 =
        ctx.Poly_gen().New_rescale(opnd0_retv.Node2(), node->Spos());
    air::base::NODE_PTR n_c2 =
        ctx.Poly_gen().New_rescale(opnd0_retv.Node3(), node->Spos());
    retv = POLY_LOWER_RETV(RETV_KIND::RK_CIPH3_POLY, n_c0, n_c1, n_c2);
  } else {
    AIR_ASSERT_MSG(false, "unexpected retv");
  }
  return retv;
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CKKS2HPOLY::Handle_modswitch(VISITOR*            visitor,
                                             air::base::NODE_PTR node) {
  POLY_LOWER_CTX&       ctx       = visitor->Context();
  fhe::core::LOWER_CTX* lower_ctx = ctx.Lower_ctx();
  POLY_IR_GEN&          pgen      = ctx.Poly_gen();
  air::base::NODE_PTR   opnd0     = node->Child(0);
  POLY_LOWER_RETV       retv;

  CMPLR_ASSERT(lower_ctx->Is_cipher_type(opnd0->Rtype_id()) ||
                   lower_ctx->Is_cipher3_type(opnd0->Rtype_id()),
               "modswitch opnd is not ciphertext/ciphertext3");

  POLY_LOWER_RETV opnd0_retv = visitor->template Visit<RETV>(opnd0);

  if (opnd0_retv.Kind() == RK_CIPH_POLY) {
    air::base::NODE_PTR n_c0 =
        pgen.New_modswitch(opnd0_retv.Node1(), node->Spos());
    air::base::NODE_PTR n_c1 =
        pgen.New_modswitch(opnd0_retv.Node2(), node->Spos());
    retv = POLY_LOWER_RETV(opnd0_retv.Kind(), n_c0, n_c1);
  } else if (opnd0_retv.Kind() == RK_CIPH3_POLY) {
    air::base::NODE_PTR n_c0 =
        pgen.New_modswitch(opnd0_retv.Node1(), node->Spos());
    air::base::NODE_PTR n_c1 =
        pgen.New_modswitch(opnd0_retv.Node2(), node->Spos());
    air::base::NODE_PTR n_c2 =
        pgen.New_modswitch(opnd0_retv.Node3(), node->Spos());
    retv = POLY_LOWER_RETV(opnd0_retv.Kind(), n_c0, n_c1, n_c2);
  } else {
    AIR_ASSERT_MSG(false, "unexpected retv");
  }
  return retv;
}

//! @brief Lowering for CKKS::Raise_mod
//! Raise_mod is the first step of Bootstrap, which raise the modulus of input
//! level to the required target level
//! v_res = CKKS.Raise_mod(v_input, raised_level)
//! Lowered to:
//! v_res.c0 = HPOLY.Raise_mod(v_input.c0, raised_level)
//! v_res.c1 = HPOLY.Raise_mod(v_input.c1, raised_level)
template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CKKS2HPOLY::Handle_raise_mod(VISITOR*            visitor,
                                             air::base::NODE_PTR node) {
  POLY_LOWER_CTX&       ctx       = visitor->Context();
  fhe::core::LOWER_CTX* lower_ctx = ctx.Lower_ctx();
  air::base::NODE_PTR   opnd0     = node->Child(0);
  air::base::NODE_PTR   opnd1     = node->Child(1);
  POLY_LOWER_RETV       retv;

  AIR_ASSERT_MSG(lower_ctx->Is_cipher_type(opnd0->Rtype_id()),
                 "raise_mod opnd is not ciphertext");
  AIR_ASSERT_MSG(opnd1->Opcode() == air::core::OPC_INTCONST,
                 "raise_mod opnd 1 is not a const");

  POLY_LOWER_RETV opnd0_retv = visitor->template Visit<RETV>(opnd0);
  POLY_LOWER_RETV opnd1_retv = visitor->template Visit<RETV>(opnd1);

  if (opnd0_retv.Kind() == RK_CIPH_POLY) {
    air::base::NODE_PTR n_c0 = ctx.Poly_gen().New_raise_mod(
        opnd0_retv.Node1(), opnd1_retv.Node1(), node->Spos());
    air::base::NODE_PTR n_c1 = ctx.Poly_gen().New_raise_mod(
        opnd0_retv.Node2(),
        ctx.Container()->Clone_node_tree(opnd1_retv.Node1()), node->Spos());
    retv = POLY_LOWER_RETV(RETV_KIND::RK_CIPH_POLY, n_c0, n_c1);
  } else {
    AIR_ASSERT_MSG(false, "unexpected retv");
  }
  return retv;
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CORE2HPOLY::Handle_ld(VISITOR*            visitor,
                                      air::base::NODE_PTR node) {
  return Handle_ld_var(visitor, node);
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CORE2HPOLY::Handle_ldp(VISITOR*            visitor,
                                       air::base::NODE_PTR node) {
  return Handle_ld_var(visitor, node);
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CORE2HPOLY::Handle_ild(VISITOR*            visitor,
                                       air::base::NODE_PTR node) {
  POLY_LOWER_CTX& ctx = visitor->Context();
  if (ctx.Lower_ctx()->Is_cipher_type(node->Rtype_id())) {
    POLY_LOWER_RETV addr = Handle_ild_ist_addr(visitor, node->Child(0));
    AIR_ASSERT(addr.Num_node() == 2);
    air::base::NODE_PTR n_ild_c0 =
        ctx.Container()->New_ild(addr.Node1(), node->Spos());
    air::base::NODE_PTR n_ild_c1 =
        ctx.Container()->New_ild(addr.Node2(), node->Spos());
    return POLY_LOWER_RETV(RETV_KIND::RK_CIPH_POLY, n_ild_c0, n_ild_c1);
  } else {
    air::base::NODE_PTR n_clone = ctx.Container()->Clone_node_tree(node);
    return POLY_LOWER_RETV(n_clone);
  }
}

template <typename VISITOR>
POLY_LOWER_RETV CORE2HPOLY::Handle_ld_var(VISITOR*            visitor,
                                          air::base::NODE_PTR node) {
  air::base::TYPE_ID    tid       = node->Rtype_id();
  POLY_LOWER_CTX&       ctx       = visitor->Context();
  fhe::core::LOWER_CTX* lower_ctx = ctx.Lower_ctx();
  air::base::SPOS       spos      = node->Spos();
  POLY_IR_GEN&          pgen      = ctx.Poly_gen();
  if (ctx.Lower_to_hpoly(node, visitor->Parent(1))) {
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
      CONST_VAR&          v_node = pgen.Node_var(node);
      air::base::NODE_PTR n_plain =
          pgen.New_plain_poly_load(v_node, false, spos);
      return POLY_LOWER_RETV(RETV_KIND::RK_PLAIN_POLY, n_plain);
    } else {
      AIR_ASSERT_MSG(false, "unexpected type");
      return POLY_LOWER_RETV();
    }
  } else {
    // for load node do not lower to poly domain, clone the whole
    // tree
    air::base::NODE_PTR n_clone = ctx.Container()->Clone_node_tree(node);
    return POLY_LOWER_RETV(n_clone);
  }
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CORE2HPOLY::Handle_st(VISITOR*            visitor,
                                      air::base::NODE_PTR node) {
  POLY_LOWER_CTX& ctx = visitor->Context();

  if (!(node->Child(0)->Is_ld())) {
    ctx.Poly_gen().Add_node_var(node->Child(0), node->Addr_datum());
  }
  return Handle_st_var(visitor, node,
                       VAR(ctx.Func_scope(), ctx.Func_scope()->Addr_datum(
                                                 node->Addr_datum_id())));
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CORE2HPOLY::Handle_stp(VISITOR*            visitor,
                                       air::base::NODE_PTR node) {
  POLY_LOWER_CTX& ctx = visitor->Context();

  if (!(node->Child(0)->Is_ld())) {
    ctx.Poly_gen().Add_node_var(node->Child(0), node->Preg());
  }
  return Handle_st_var(visitor, node, VAR(ctx.Func_scope(), node->Preg()));
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CORE2HPOLY::Handle_ist(VISITOR*            visitor,
                                       air::base::NODE_PTR node) {
  POLY_LOWER_CTX&       ctx       = visitor->Context();
  fhe::core::LOWER_CTX* lower_ctx = ctx.Lower_ctx();
  air::base::TYPE_ID    tid       = node->Access_type_id();
  POLY_IR_GEN&          irgen     = ctx.Poly_gen();
  air::base::CONTAINER* cntr      = irgen.Container();
  if (!(lower_ctx->Is_cipher_type(tid) || lower_ctx->Is_cipher3_type(tid) ||
        lower_ctx->Is_plain_type(tid))) {
    // clone the tree for ist type is not ciph/plain related
    air::base::STMT_PTR s_new = cntr->Clone_stmt_tree(node->Stmt());
    return POLY_LOWER_RETV(s_new->Node());
  }
  POLY_LOWER_RETV rhs =
      visitor->template Visit<POLY_LOWER_RETV>(node->Child(1));
  if (rhs.Kind() == RETV_KIND::RK_CIPH_POLY) {
    // gen init stmt for cpu target
    if (ctx.Config().Lower_to_lpoly()) {
      air::base::NODE_PTR n_res =
          cntr->New_ild(cntr->Clone_node_tree(node->Child(0)), node->Spos());
      air::base::NODE_PTR n_opnd0 =
          irgen.New_var_load(irgen.Node_var(node->Child(1)), node->Spos());
      air::base::TYPE_PTR t_opnd1 = ctx.Glob_scope()->New_ptr_type(
          ctx.Glob_scope()->Prim_type(air::base::PRIMITIVE_TYPE::INT_U64)->Id(),
          air::base::POINTER_KIND::FLAT64);
      air::base::NODE_PTR n_opnd1 = cntr->New_zero(t_opnd1, node->Spos());
      air::base::STMT_PTR s_init =
          irgen.New_init_ciph_same_scale(n_res, n_opnd0, n_opnd1, node->Spos());
      ctx.Prepend(s_init);
    }

    // generate ist poly
    POLY_LOWER_RETV     ist_addr = Handle_ild_ist_addr(visitor, node->Child(0));
    air::base::STMT_PTR s_c0 =
        cntr->New_ist(ist_addr.Node1(), rhs.Node1(), node->Spos());
    air::base::STMT_PTR s_c1 =
        cntr->New_ist(ist_addr.Node2(), rhs.Node2(), node->Spos());
    ctx.Prepend(s_c0);
    ctx.Prepend(s_c1);
    return POLY_LOWER_RETV(RETV_KIND::RK_BLOCK, air::base::Null_ptr);
  } else {
    AIR_ASSERT_MSG(false, "unsupported ist rhs");
  }
  return POLY_LOWER_RETV();
}

template <typename VISITOR>
POLY_LOWER_RETV CORE2HPOLY::Handle_ild_ist_addr(VISITOR*            visitor,
                                                air::base::NODE_PTR addr) {
  POLY_LOWER_CTX&       ctx       = visitor->Context();
  fhe::core::LOWER_CTX* lower_ctx = ctx.Lower_ctx();
  POLY_IR_GEN&          irgen     = ctx.Poly_gen();
  air::base::CONTAINER* cntr      = irgen.Container();

  AIR_ASSERT(addr->Opcode() == air::core::OPC_ARRAY);
  AIR_ASSERT(lower_ctx->Is_cipher_type(
      addr->Rtype()->Cast_to_ptr()->Domain_type_id()));

  air::base::NODE_PTR n_addr_c0      = cntr->Clone_node_tree(addr);
  air::base::NODE_PTR n_addr_c1      = cntr->Clone_node_tree(addr);
  air::base::TYPE_PTR t_rns_poly_ptr = cntr->Glob_scope()->New_ptr_type(
      lower_ctx->Get_rns_poly_type_id(), air::base::POINTER_KIND::FLAT64);
  uint32_t            c0_ofst   = 0;
  uint32_t            c1_ofst   = 1;
  air::base::NODE_PTR n_c0_ofst = cntr->New_intconst(
      air::base::PRIMITIVE_TYPE::INT_U32, c0_ofst, addr->Spos());
  air::base::NODE_PTR n_c1_ofst = cntr->New_intconst(
      air::base::PRIMITIVE_TYPE::INT_U32, c1_ofst, addr->Spos());
  n_addr_c0 = cntr->New_bin_arith(air::core::ADD, t_rns_poly_ptr, n_addr_c0,
                                  n_c0_ofst, addr->Spos());
  n_addr_c1 = cntr->New_bin_arith(air::core::ADD, t_rns_poly_ptr, n_addr_c1,
                                  n_c1_ofst, addr->Spos());

  return POLY_LOWER_RETV(RETV_KIND::RK_CIPH_POLY, n_addr_c0, n_addr_c1);
}

template <typename VISITOR>
POLY_LOWER_RETV CORE2HPOLY::Handle_st_var(VISITOR*            visitor,
                                          air::base::NODE_PTR node,
                                          CONST_VAR           var) {
  air::base::TYPE_ID    tid       = var.Type_id();
  POLY_LOWER_CTX&       ctx       = visitor->Context();
  fhe::core::LOWER_CTX* lower_ctx = ctx.Lower_ctx();
  if (!(lower_ctx->Is_cipher_type(tid) || lower_ctx->Is_cipher3_type(tid) ||
        lower_ctx->Is_plain_type(tid))) {
    // clone the tree for store var type is not ciph/plain related
    air::base::STMT_PTR s_new =
        ctx.Poly_gen().Container()->Clone_stmt_tree(node->Stmt());
    return POLY_LOWER_RETV(s_new->Node());
  }
  POLY_LOWER_RETV rhs =
      visitor->template Visit<POLY_LOWER_RETV>(node->Child(0));
  if (rhs.Kind() == RETV_KIND::RK_CIPH_POLY) {
    CMPLR_ASSERT(lower_ctx->Is_cipher_type(tid), "store symbol is not ciph");
    // gen init for cpu target
    if (ctx.Config().Lower_to_lpoly()) {
      air::base::STMT_PTR init_stmt =
          ctx.Poly_gen().New_init_ciph(var, node->Child(0));
      if (init_stmt != air::base::Null_ptr) {
        ctx.Prepend(init_stmt);
      }
    }
    STMT_PAIR s_pair = ctx.Poly_gen().New_ciph_poly_store(
        var, rhs.Node1(), rhs.Node2(), false, node->Spos());
    ctx.Prepend(s_pair.first);
    ctx.Prepend(s_pair.second);

    return POLY_LOWER_RETV(RETV_KIND::RK_BLOCK, air::base::Null_ptr);
  } else if (rhs.Kind() == RETV_KIND::RK_CIPH3_POLY) {
    CMPLR_ASSERT(lower_ctx->Is_cipher3_type(tid), "store symbol is not ciph3");

    // gen init for cpu target
    if (ctx.Config().Lower_to_lpoly()) {
      air::base::STMT_PTR init_stmt =
          ctx.Poly_gen().New_init_ciph(var, node->Child(0));
      if (init_stmt != air::base::Null_ptr) {
        ctx.Prepend(init_stmt);
      }
    }
    STMT_TRIPLE s_tuple = ctx.Poly_gen().New_ciph3_poly_store(
        var, rhs.Node1(), rhs.Node2(), rhs.Node3(), false, node->Spos());
    ctx.Prepend(std::get<0>(s_tuple));
    ctx.Prepend(std::get<1>(s_tuple));
    ctx.Prepend(std::get<2>(s_tuple));
    return POLY_LOWER_RETV(RETV_KIND::RK_BLOCK, air::base::Null_ptr);
  } else if (rhs.Kind() == RETV_KIND::RK_PLAIN_POLY) {
    CMPLR_ASSERT(lower_ctx->Is_plain_type(tid), "store symbol is not plain");

    // gen init for cpu target
    if (ctx.Config().Lower_to_lpoly()) {
      air::base::STMT_PTR init_stmt =
          ctx.Poly_gen().New_init_ciph(var, node->Child(0));
      if (init_stmt != air::base::Null_ptr) {
        ctx.Prepend(init_stmt);
      }
    }
    air::base::STMT_PTR s_plain = ctx.Poly_gen().New_plain_poly_store(
        var, rhs.Node1(), false, node->Spos());
    ctx.Prepend(s_plain);
    return POLY_LOWER_RETV(RETV_KIND::RK_BLOCK, air::base::Null_ptr);
  } else {
    // clone the tree for ciph/plain/ciph3 that not lowered to
    // polys such as CKKS.encode
    air::base::STMT_PTR s_new =
        ctx.Poly_gen().New_var_store(rhs.Node(), var, node->Spos());
    return POLY_LOWER_RETV(s_new->Node());
  }
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CORE2HPOLY::Handle_zero(VISITOR*            visitor,
                                        air::base::NODE_PTR node) {
  air::base::NODE_PTR   parent    = visitor->Parent(1);
  air::base::TYPE_ID    tid       = parent->Access_type_id();
  POLY_LOWER_CTX&       ctx       = visitor->Context();
  POLY_IR_GEN&          irgen     = ctx.Poly_gen();
  air::base::CONTAINER* cntr      = ctx.Container();
  fhe::core::LOWER_CTX* lower_ctx = ctx.Lower_ctx();
  air::base::TYPE_PTR   t_rns_poly =
      lower_ctx->Get_rns_poly_type(cntr->Glob_scope());
  uint32_t num_q = irgen.Get_num_q(parent);
  // lower zero to multiple rns poly type zero based on parent
  // type
  if (lower_ctx->Is_cipher_type(tid)) {
    air::base::NODE_PTR n_zero_c0 = cntr->New_zero(t_rns_poly, node->Spos());
    air::base::NODE_PTR n_zero_c1 = cntr->New_zero(t_rns_poly, node->Spos());
    irgen.Set_num_q(n_zero_c0, num_q);
    irgen.Set_num_q(n_zero_c1, num_q);
    return POLY_LOWER_RETV(RETV_KIND::RK_CIPH_POLY, n_zero_c0, n_zero_c1);
  } else if (lower_ctx->Is_cipher3_type(tid)) {
    air::base::NODE_PTR n_zero_c0 = cntr->New_zero(t_rns_poly, node->Spos());
    air::base::NODE_PTR n_zero_c1 = cntr->New_zero(t_rns_poly, node->Spos());
    air::base::NODE_PTR n_zero_c2 = cntr->New_zero(t_rns_poly, node->Spos());
    irgen.Set_num_q(n_zero_c0, num_q);
    irgen.Set_num_q(n_zero_c1, num_q);
    irgen.Set_num_q(n_zero_c2, num_q);
    return POLY_LOWER_RETV(RETV_KIND::RK_CIPH3_POLY, n_zero_c0, n_zero_c1,
                           n_zero_c2);
  } else if (lower_ctx->Is_plain_type(tid)) {
    air::base::NODE_PTR n_zero_plain = cntr->New_zero(t_rns_poly, node->Spos());
    irgen.Set_num_q(n_zero_plain, num_q);
    return POLY_LOWER_RETV(RETV_KIND::RK_PLAIN_POLY, n_zero_plain);
  } else {
    air::base::NODE_PTR n_clone = ctx.Container()->Clone_node_tree(node);
    return POLY_LOWER_RETV(n_clone);
  }
}

}  // namespace poly
}  // namespace fhe

#endif  // FHE_POLY_CKKS2HPOLY_H
