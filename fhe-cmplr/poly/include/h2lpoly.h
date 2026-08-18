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

  //! @brief Handle POLY_OPERATOR::ADD
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_add(VISITOR* visitor, NODE_PTR node);

  //! @brief Handle POLY_OPERATOR::MUL
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_mul(VISITOR* visitor, NODE_PTR node);

  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_rotate(VISITOR* visitor, NODE_PTR node);
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_extend(VISITOR* visitor, NODE_PTR node);

  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_binary_op(VISITOR* visitor, NODE_PTR node);

  STMT_PTR Gen_rns_binary_op(air::base::OPCODE op, POLY_LOWER_CTX& ctx,
                             CONST_VAR v_res, CONST_VAR v_rns_idx,
                             NODE_PTR opnd1, NODE_PTR opnd2, NODE_PTR blk,
                             const SPOS& spos);

  void Expand_op_to_rns(POLY_LOWER_CTX& ctx, NODE_PTR node, CONST_VAR v_res,
                        NODE_PTR opnd0, NODE_PTR opnd1, bool is_ext,
                        NODE_PTR blk);
  void Call_rns_func(POLY_LOWER_CTX& ctx, NODE_PTR node, NODE_PTR n_opnd0,
                     NODE_PTR n_opnd1, bool is_ext);

  bool Has_ext_attr(POLY_LOWER_CTX& ctx, NODE_PTR node);
};

class CORE2LPOLY : public air::core::DEFAULT_HANDLER {
public:
  //! @brief Construct a new CKKS2HPOLY object
  CORE2LPOLY() {}
#if 0
  //! @brief Handle CORE::LOAD
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_ld(VISITOR* visitor, NODE_PTR node);

  //! @brief Handle CORE::LDP
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_ldp(VISITOR* visitor, NODE_PTR node);

  //! @brief Handle CORE::ILD
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_ild(VISITOR* visitor, NODE_PTR node);

#endif

  //! @brief Handle CORE::STORE
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_st(VISITOR* visitor, NODE_PTR node);

  //! @brief Handle CORE::STP
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_stp(VISITOR* visitor, NODE_PTR node);

  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_stf(VISITOR* visitor, NODE_PTR node);

  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_stpf(VISITOR* visitor, NODE_PTR node);
  template <typename VISITOR>
  POLY_LOWER_RETV Handle_st_var(VISITOR* visitor, NODE_PTR node, CONST_VAR var);
};

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV H2LPOLY::Handle_add(VISITOR* visitor, NODE_PTR node) {
  return Handle_binary_op<RETV>(visitor, node);
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV H2LPOLY::Handle_mul(VISITOR* visitor, NODE_PTR node) {
  return Handle_binary_op<RETV>(visitor, node);
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV H2LPOLY::Handle_rotate(VISITOR* visitor, NODE_PTR node) {
  return Handle_binary_op<RETV>(visitor, node);
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV H2LPOLY::Handle_extend(VISITOR* visitor, NODE_PTR node) {
  POLY_LOWER_CTX& ctx  = visitor->Context();
  POLY_IR_GEN&    pgen = ctx.Poly_gen();
  // add init node to allocate memory
  CONST_VAR& v_res  = pgen.Node_var(node);
  STMT_PTR   s_init = pgen.New_init_poly(v_res, node, true);
  if (s_init != STMT_PTR()) {
    ctx.Prepend(s_init);
  }
  return ctx.Poly_gen().Container()->Clone_node_tree(node);
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CORE2LPOLY::Handle_st(VISITOR* visitor, NODE_PTR node) {
  POLY_LOWER_CTX& ctx = visitor->Context();
  if (!(node->Child(0)->Is_ld())) {
    ctx.Poly_gen().Add_node_var(node->Child(0), node->Addr_datum());
  }
  return Handle_st_var(visitor, node,
                       VAR(ctx.Func_scope(), node->Addr_datum()));
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CORE2LPOLY::Handle_stp(VISITOR* visitor, NODE_PTR node) {
  POLY_LOWER_CTX& ctx = visitor->Context();
  if (!(node->Child(0)->Is_ld())) {
    ctx.Poly_gen().Add_node_var(node->Child(0), node->Preg());
  }
  return Handle_st_var(visitor, node, VAR(ctx.Func_scope(), node->Preg()));
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CORE2LPOLY::Handle_stf(VISITOR* visitor, NODE_PTR node) {
  POLY_LOWER_CTX& ctx = visitor->Context();
  if (!(node->Child(0)->Is_ld())) {
    ctx.Poly_gen().Add_node_var(node->Child(0), node->Addr_datum(),
                                node->Field());
  }
  return Handle_st_var(
      visitor, node, VAR(ctx.Func_scope(), node->Addr_datum(), node->Field()));
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CORE2LPOLY::Handle_stpf(VISITOR* visitor, NODE_PTR node) {
  POLY_LOWER_CTX& ctx = visitor->Context();
  if (!(node->Child(0)->Is_ld())) {
    ctx.Poly_gen().Add_node_var(node->Child(0), node->Preg(), node->Field());
  }
  return Handle_st_var(visitor, node,
                       VAR(ctx.Func_scope(), node->Preg(), node->Field()));
}

template <typename VISITOR>
POLY_LOWER_RETV CORE2LPOLY::Handle_st_var(VISITOR* visitor, NODE_PTR node,
                                          CONST_VAR var) {
  TYPE_ID               tid       = var.Type_id();
  POLY_LOWER_CTX&       ctx       = visitor->Context();
  fhe::core::LOWER_CTX* lower_ctx = ctx.Lower_ctx();
  if (!ctx.Lower_to_lpoly(node->Child(0))) {
    // clone the tree for
    // 1) store var type is not ciph/plain related
    // 2) load/ldp for ciph/plain, keep store(res, ciph)
    //    do not lower to store polys for now
    STMT_PTR s_new = ctx.Poly_gen().Container()->Clone_stmt_tree(node->Stmt());
    return POLY_LOWER_RETV(s_new->Node());
  } else {
    POLY_LOWER_RETV retv;
    CONTAINER*      cntr = ctx.Container();
    POLY_LOWER_RETV rhs =
        visitor->template Visit<POLY_LOWER_RETV>(node->Child(0));
    if (!rhs.Is_null()) {
      STMT_PTR s_new = ctx.Poly_gen().Container()->Clone_stmt(node->Stmt());
      s_new->Node()->Set_child(0, rhs.Node());
      return POLY_LOWER_RETV(s_new->Node());
    } else {
      return POLY_LOWER_RETV();
    }
  }
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV H2LPOLY::Handle_binary_op(VISITOR* visitor, NODE_PTR node) {
  POLY_LOWER_CTX& ctx  = visitor->Context();
  POLY_IR_GEN&    pgen = ctx.Poly_gen();
  CONTAINER*      cntr = pgen.Container();
  SPOS            spos = node->Spos();
  CMPLR_ASSERT(node->Num_child() == 2, "invalid binary op node");

  // visit add's two child node
  NODE_PTR        n0      = node->Child(0);
  NODE_PTR        n1      = node->Child(1);
  POLY_LOWER_RETV n0_retv = visitor->template Visit<RETV>(n0);
  POLY_LOWER_RETV n1_retv = visitor->template Visit<RETV>(n1);
  CMPLR_ASSERT((!n0_retv.Is_null() && !n1_retv.Is_null()), "null node");

  bool is_ext = Has_ext_attr(ctx, node);
  // Add init poly, consider move to function body
  STMT_PTR s_init = pgen.New_init_poly(pgen.Node_var(node), node, is_ext);
  ctx.Prepend(s_init);

  if (ctx.Config().Inline_rns_op()) {
    Expand_op_to_rns(ctx, node, ctx.Poly_gen().Node_var(node), n0_retv.Node(),
                     n1_retv.Node(), is_ext, ctx.Parent_block());
  } else {
    Call_rns_func(ctx, node, n0_retv.Node(), n1_retv.Node(), is_ext);
  }
  return POLY_LOWER_RETV();
}

void H2LPOLY::Expand_op_to_rns(POLY_LOWER_CTX& ctx, NODE_PTR node,
                               CONST_VAR v_res, NODE_PTR opnd0, NODE_PTR opnd1,
                               bool is_ext, NODE_PTR blk) {
  POLY_IR_GEN& pgen      = ctx.Poly_gen();
  CONTAINER*   cntr      = pgen.Container();
  CONST_VAR&   v_rns_idx = pgen.Get_var(VAR_RNS_IDX, node->Spos());
  NODE_PTR     n_res     = pgen.New_var_load(v_res, node->Spos());
  NODE_PAIR    n_blks    = pgen.New_rns_loop(n_res, false);

  STMT_PTR s_op = Gen_rns_binary_op(node->Opcode(), ctx, v_res, v_rns_idx,
                                    opnd0, opnd1, blk, node->Spos());
  pgen.Append_rns_stmt(s_op, n_blks.second);

  pgen.Append_stmt(n_blks.first->Stmt(), blk);
  // Generate p loop
  if (is_ext) {
    NODE_PAIR ext_blks = pgen.New_rns_loop(cntr->Clone_node_tree(n_res), true);

    STMT_PTR s_pidx = pgen.New_adjust_p_idx(node->Spos());
    pgen.Append_rns_stmt(s_pidx, ext_blks.second);

    CONST_VAR& v_p_idx  = pgen.Get_var(VAR_P_IDX, node->Spos());
    STMT_PTR   s_ext_op = Gen_rns_binary_op(
        node->Opcode(), ctx, v_res, v_p_idx, cntr->Clone_node_tree(opnd0),
        cntr->Clone_node_tree(opnd1), blk, node->Spos());
    pgen.Append_rns_stmt(s_ext_op, ext_blks.second);

    pgen.Append_stmt(ext_blks.first->Stmt(), blk);
  }
}

void H2LPOLY::Call_rns_func(POLY_LOWER_CTX& ctx, NODE_PTR node,
                            NODE_PTR n_opnd0, NODE_PTR n_opnd1, bool is_ext) {
  POLY_IR_GEN&     pgen      = ctx.Poly_gen();
  GLOB_SCOPE*      glob      = pgen.Glob_scope();
  SPOS             spos      = node->Spos();
  core::LOWER_CTX* lower_ctx = ctx.Lower_ctx();

  core::FHE_FUNC fhe_func_id;
  switch (node->Opcode().Operator()) {
    case ADD:
      fhe_func_id = is_ext ? core::RNS_ADD_EXT : core::RNS_ADD;
      break;
    case MUL:
      fhe_func_id = is_ext ? core::RNS_MUL_EXT : core::RNS_MUL;
      break;
    case ROTATE:
      fhe_func_id = is_ext ? core::RNS_ROTATE_EXT : core::RNS_ROTATE;
      break;
    default:
      AIR_ASSERT_MSG(false, "unexpected");
  }

  core::FHE_FUNC_INFO& func_info = lower_ctx->Get_func_info(fhe_func_id);
  FUNC_SCOPE*          fs        = func_info.Get_func_scope(glob);
  if (fs == nullptr) {
    FUNC_SCOPE* orig_fs = pgen.Func_scope();
    fs                  = pgen.New_func(fhe_func_id, spos);
    lower_ctx->Set_func_info(fhe_func_id, fs->Owning_func()->Id());

    pgen.Enter_func(fs);
    CONTAINER* new_cntr = pgen.Container();
    new_cntr->New_func_entry(node->Spos());
    NODE_PTR blk = new_cntr->Stmt_list().Block_node();

    // create formal and expand operation in the new function
    VAR      formal_0(fs, fs->Formal(0));
    VAR      formal_1(fs, fs->Formal(1));
    VAR      formal_2(fs, fs->Formal(2));
    NODE_PTR n_formal_0 = pgen.New_var_load(formal_0, spos);
    NODE_PTR n_formal_1 = pgen.New_var_load(formal_1, spos);
    NODE_PTR n_formal_2 = pgen.New_var_load(formal_2, spos);
    Expand_op_to_rns(ctx, node, formal_0, n_formal_1, n_formal_2, is_ext, blk);
    // switch back to orignal function scope
    ctx.Poly_gen().Enter_func(orig_fs);
  }

  // Gen call statment
  CONTAINER* cntr   = pgen.Container();
  TYPE_PTR   t_poly = pgen.Lower_ctx()->Get_poly_type(glob);
  PREG_PTR   retv   = pgen.Func_scope()->New_preg(t_poly);
  STMT_PTR   s_call =
      cntr->New_call(fs->Owning_func()->Entry_point(), retv, 3, spos);
  CONST_VAR& v_res = pgen.Node_var(node);
  NODE_PTR   n_res = pgen.New_var_load(v_res, node->Spos());
  cntr->New_arg(s_call, 0, n_res);  // put res as first parameter
  cntr->New_arg(s_call, 1, n_opnd0);
  cntr->New_arg(s_call, 2, n_opnd1);
  ctx.Prepend(s_call);
}

STMT_PTR H2LPOLY::Gen_rns_binary_op(air::base::OPCODE op, POLY_LOWER_CTX& ctx,
                                    CONST_VAR v_res, CONST_VAR v_rns_idx,
                                    NODE_PTR opnd1, NODE_PTR opnd2,
                                    NODE_PTR blk, const SPOS& spos) {
  POLY_IR_GEN& pgen      = ctx.Poly_gen();
  CONST_VAR&   v_modulus = pgen.Get_var(VAR_MODULUS, spos);
  NODE_PTR     n_modulus = pgen.New_var_load(v_modulus, spos);
  NODE_PTR     n0_at_l   = pgen.New_poly_load_at_level(opnd1, v_rns_idx);
  NODE_PTR     n_op      = air::base::Null_ptr;
  AIR_ASSERT(op.Domain() == fhe::poly::POLYNOMIAL_DID);

  switch (op.Operator()) {
    case ADD: {
      NODE_PTR n1_at_l = pgen.New_poly_load_at_level(opnd2, v_rns_idx);
      n_op             = pgen.New_hw_modadd(n0_at_l, n1_at_l, n_modulus, spos);
    } break;
    case MUL: {
      NODE_PTR n1_at_l = pgen.New_poly_load_at_level(opnd2, v_rns_idx);
      n_op             = pgen.New_hw_modmul(n0_at_l, n1_at_l, n_modulus, spos);
    } break;
    case ROTATE: {
      // generate automorphism orders
      CONST_VAR& v_order = ctx.Poly_gen().Get_var(VAR_AUTO_ORDER, spos);
      NODE_PTR   n_order = ctx.Poly_gen().New_auto_order(opnd2, spos);
      STMT_PTR   s_order = ctx.Poly_gen().New_var_store(n_order, v_order, spos);
      pgen.Append_stmt(s_order, blk);
      NODE_PTR n_ld_order = ctx.Poly_gen().New_var_load(v_order, spos);
      n_op = pgen.New_hw_rotate(n0_at_l, n_ld_order, n_modulus, spos);
    } break;
    default:
      AIR_ASSERT(false);
  }
  STMT_PTR s_op = pgen.New_poly_store_at_level(pgen.New_var_load(v_res, spos),
                                               n_op, v_rns_idx);
  return s_op;
}

bool H2LPOLY::Has_ext_attr(POLY_LOWER_CTX& ctx, NODE_PTR node) {
  bool            is_ext     = false;
  const uint32_t* is_ext_ptr = node->Attr<uint32_t>(
      ctx.Lower_ctx()->Attr_name(fhe::core::FHE_ATTR_KIND::EXTENDED));
  if (is_ext_ptr != nullptr && *is_ext_ptr != 0) {
    is_ext = true;
  }
  return is_ext;
}

}  // namespace poly
}  // namespace fhe
#endif