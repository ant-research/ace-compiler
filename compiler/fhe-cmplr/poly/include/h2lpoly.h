//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================
#ifndef FHE_POLY_H2LPOLY_H
#define FHE_POLY_H2LPOLY_H

#include "air/base/transform_ctx.h"
#include "air/base/visitor.h"
#include "air/core/default_handler.h"
#include "air/core/handler.h"
#include "fhe/poly/default_handler.h"
#include "fhe/poly/handler.h"
#include "poly_ir_gen.h"
#include "poly_lower_ctx.h"

namespace fhe {
namespace poly {
class CORE2LPOLY;
class H2LPOLY;
using H2LPOLY_VISITOR =
    air::base::VISITOR<POLY_LOWER_CTX, air::core::HANDLER<CORE2LPOLY>,
                       fhe::poly::HANDLER<H2LPOLY>>;

class H2LPOLY : public fhe::poly::DEFAULT_HANDLER {
public:
  //! @brief Construct a new H2LPOLY object
  H2LPOLY() {}

  //! @brief Handle HPOLY_OPERATOR::ADD
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_add(VISITOR* visitor, air::base::NODE_PTR node);

  //! @brief Handle HPOLY_OPERATOR::SUB
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_sub(VISITOR* visitor, air::base::NODE_PTR node);

  //! @brief Handle HPOLY_OPERATOR::MUL
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_mul(VISITOR* visitor, air::base::NODE_PTR node);

  //! @brief Handle HPOLY_OPERATOR::ROTATE
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_rotate(VISITOR* visitor, air::base::NODE_PTR node);

  //! @brief Handle HPOLY_OPERATOR::EXTEND
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_extend(VISITOR* visitor, air::base::NODE_PTR node);

private:
  // Genernal function that process binary operations
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_binary_op(VISITOR* visitor, air::base::NODE_PTR node);

  // Generate binary op for rns expanded polynomial
  air::base::STMT_PTR Gen_rns_binary_op(
      air::base::OPCODE op, POLY_LOWER_CTX& ctx, CONST_VAR v_res,
      CONST_VAR v_rns_idx, air::base::NODE_PTR opnd1, air::base::NODE_PTR opnd2,
      air::base::STMT_LIST& sl_blk, const air::base::SPOS& spos);

  // Expand hpoly operations to rns loops
  void Expand_op_to_rns(POLY_LOWER_CTX& ctx, air::base::NODE_PTR node,
                        CONST_VAR v_res, air::base::NODE_PTR opnd1,
                        air::base::NODE_PTR opnd2, bool is_ext,
                        air::base::STMT_LIST& sl);

  // Generate call to RNS-expanded functions and create a new function
  // if it does not already exist
  void Call_rns_func(POLY_LOWER_CTX& ctx, air::base::NODE_PTR node,
                     air::base::NODE_PTR n_opnd1, air::base::NODE_PTR n_opnd2,
                     bool is_ext);

  // Check if node contains "extended" attribute
  bool Has_ext_attr(POLY_LOWER_CTX& ctx, air::base::NODE_PTR node);
};

class CORE2LPOLY : public air::core::DEFAULT_HANDLER {
public:
  //! @brief Construct a new CKKS2HPOLY object
  CORE2LPOLY() {}

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

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV H2LPOLY::Handle_add(VISITOR*            visitor,
                                    air::base::NODE_PTR node) {
  return Handle_binary_op<RETV>(visitor, node);
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV H2LPOLY::Handle_sub(VISITOR*            visitor,
                                    air::base::NODE_PTR node) {
  return Handle_binary_op<RETV>(visitor, node);
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV H2LPOLY::Handle_mul(VISITOR*            visitor,
                                    air::base::NODE_PTR node) {
  return Handle_binary_op<RETV>(visitor, node);
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV H2LPOLY::Handle_rotate(VISITOR*            visitor,
                                       air::base::NODE_PTR node) {
  return Handle_binary_op<RETV>(visitor, node);
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV H2LPOLY::Handle_extend(VISITOR*            visitor,
                                       air::base::NODE_PTR node) {
  POLY_LOWER_CTX& ctx  = visitor->Context();
  POLY_IR_GEN&    pgen = ctx.Poly_gen();
  // add init node to allocate memory
  CONST_VAR&          v_res = pgen.Node_var(node);
  air::base::STMT_PTR s_init =
      pgen.New_init_poly_by_opnd(v_res, node, true, node->Spos());
  ctx.Prepend(s_init);
  return ctx.Poly_gen().Container()->Clone_node_tree(node);
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV H2LPOLY::Handle_binary_op(VISITOR*            visitor,
                                          air::base::NODE_PTR node) {
  POLY_LOWER_CTX&       ctx  = visitor->Context();
  POLY_IR_GEN&          pgen = ctx.Poly_gen();
  air::base::CONTAINER* cntr = pgen.Container();
  air::base::SPOS       spos = node->Spos();
  CMPLR_ASSERT(node->Num_child() == 2, "invalid binary op node");

  // visit two child node
  air::base::NODE_PTR n0      = node->Child(0);
  air::base::NODE_PTR n1      = node->Child(1);
  POLY_LOWER_RETV     n0_retv = visitor->template Visit<RETV>(n0);
  POLY_LOWER_RETV     n1_retv = visitor->template Visit<RETV>(n1);
  CMPLR_ASSERT((!n0_retv.Is_null() && !n1_retv.Is_null()), "null node");
  CONST_VAR& v_node = pgen.Node_var(node);

  // Add init poly
  bool                is_ext = Has_ext_attr(ctx, node);
  air::base::STMT_PTR s_init =
      pgen.New_init_poly_by_opnd(v_node, node, is_ext, node->Spos());
  ctx.Prepend(s_init);

  if (ctx.Config().Inline_rns()) {
    air::base::STMT_LIST sl = air::base::STMT_LIST::Enclosing_list(s_init);
    Expand_op_to_rns(ctx, node, v_node, n0_retv.Node(), n1_retv.Node(), is_ext,
                     sl);
  } else {
    Call_rns_func(ctx, node, n0_retv.Node(), n1_retv.Node(), is_ext);
  }
  return POLY_LOWER_RETV();
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CORE2LPOLY::Handle_st(VISITOR*            visitor,
                                      air::base::NODE_PTR node) {
  POLY_LOWER_CTX& ctx = visitor->Context();
  if (!(node->Child(0)->Is_ld())) {
    ctx.Poly_gen().Add_node_var(node->Child(0), node->Addr_datum());
  }
  return Handle_st_var(visitor, node,
                       VAR(ctx.Func_scope(), node->Addr_datum()));
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CORE2LPOLY::Handle_stp(VISITOR*            visitor,
                                       air::base::NODE_PTR node) {
  POLY_LOWER_CTX& ctx = visitor->Context();
  if (!(node->Child(0)->Is_ld())) {
    ctx.Poly_gen().Add_node_var(node->Child(0), node->Preg());
  }
  return Handle_st_var(visitor, node, VAR(ctx.Func_scope(), node->Preg()));
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CORE2LPOLY::Handle_stf(VISITOR*            visitor,
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
POLY_LOWER_RETV CORE2LPOLY::Handle_stpf(VISITOR*            visitor,
                                        air::base::NODE_PTR node) {
  POLY_LOWER_CTX& ctx = visitor->Context();
  if (!(node->Child(0)->Is_ld())) {
    ctx.Poly_gen().Add_node_var(node->Child(0), node->Preg(), node->Field());
  }
  return Handle_st_var(visitor, node,
                       VAR(ctx.Func_scope(), node->Preg(), node->Field()));
}

template <typename VISITOR>
POLY_LOWER_RETV CORE2LPOLY::Handle_st_var(VISITOR*            visitor,
                                          air::base::NODE_PTR node,
                                          CONST_VAR           var) {
  air::base::TYPE_ID    tid       = var.Type_id();
  POLY_LOWER_CTX&       ctx       = visitor->Context();
  fhe::core::LOWER_CTX* lower_ctx = ctx.Lower_ctx();
  if (!ctx.Lower_to_lpoly(node->Child(0))) {
    // clone the tree for
    // 1) store var type is not ciph/plain related
    // 2) load/ldp for ciph/plain, keep store(res, ciph)
    //    do not lower to store polys for now
    air::base::STMT_PTR s_new =
        ctx.Poly_gen().Container()->Clone_stmt_tree(node->Stmt());
    return POLY_LOWER_RETV(s_new->Node());
  } else {
    POLY_LOWER_RETV       retv;
    air::base::CONTAINER* cntr = ctx.Container();
    POLY_LOWER_RETV       rhs =
        visitor->template Visit<POLY_LOWER_RETV>(node->Child(0));
    if (!rhs.Is_null()) {
      air::base::STMT_PTR s_new =
          ctx.Poly_gen().Container()->Clone_stmt(node->Stmt());
      s_new->Node()->Set_child(0, rhs.Node());
      return POLY_LOWER_RETV(s_new->Node());
    } else {
      return POLY_LOWER_RETV();
    }
  }
}

}  // namespace poly
}  // namespace fhe
#endif
