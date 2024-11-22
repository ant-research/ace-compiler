//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_POLY_CKKS2POLY_H
#define FHE_POLY_CKKS2POLY_H

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

namespace fhe {

namespace poly {

class CKKS2POLY;
class CORE2POLY;
class CKKS2POLY_CTX;
using CKKS2POLY_VISITOR =
    air::base::VISITOR<CKKS2POLY_CTX, air::core::HANDLER<CORE2POLY>,
                       fhe::ckks::HANDLER<CKKS2POLY>>;

enum RETV_KIND : uint8_t {
  RK_DEFAULT,
  RK_CIPH,
  RK_CIPH_POLY,
  RK_CIPH_RNS_POLY,
  RK_CIPH3,
  RK_CIPH3_POLY,
  RK_CIPH3_RNS_POLY,
  RK_PLAIN,
  RK_PLAIN_POLY,
  RK_PLAIN_RNS_POLY,
  RK_BLOCK,
};

//! @brief CKKS2POLY handler return type
class CKKS2POLY_RETV {
public:
  CKKS2POLY_RETV() : _kind(RETV_KIND::RK_DEFAULT), _num_node(0) {}

  CKKS2POLY_RETV(NODE_PTR node)
      : _kind(RETV_KIND::RK_DEFAULT), _num_node(1), _node1(node) {}

  CKKS2POLY_RETV(RETV_KIND kind, NODE_PTR node)
      : _kind(kind), _num_node(1), _node1(node) {}

  CKKS2POLY_RETV(RETV_KIND kind, NODE_PTR node1, NODE_PTR node2)
      : _kind(kind), _num_node(2), _node1(node1), _node2(node2) {}

  CKKS2POLY_RETV(RETV_KIND kind, NODE_PTR node1, NODE_PTR node2, NODE_PTR node3)
      : _kind(kind),
        _num_node(3),
        _node1(node1),
        _node2(node2),
        _node3(node3) {}

  NODE_PTR  Node() const { return _node1; }
  NODE_PTR  Node1() const { return _node1; }
  NODE_PTR  Node2() const { return _node2; }
  NODE_PTR  Node3() const { return _node3; }
  RETV_KIND Kind() const { return _kind; }
  bool      Is_null() const;
  uint32_t  Num_node() const { return _num_node; }

private:
  RETV_KIND _kind;
  uint8_t   _num_node;
  NODE_PTR  _node1;
  NODE_PTR  _node2;
  NODE_PTR  _node3;
};

//! @brief CKKS2POLY handler context
class CKKS2POLY_CTX : public air::base::TRANSFORM_CTX {
public:
  //! @brief Construct a new ckks2poly ctx object
  CKKS2POLY_CTX(POLY_CONFIG& config, fhe::core::LOWER_CTX* fhe_ctx,
                CONTAINER* cntr)
      : air::base::TRANSFORM_CTX(cntr),
        _config(config),
        _fhe_ctx(fhe_ctx),
        _cntr(cntr),
        _pool(),
        _poly_gen(cntr, fhe_ctx, Mem_pool()),
        _rns_blk() {
    CKKS2POLY_RETV x[2];
    _pool.Push();
  }

  //! @brief Destroy the ckks2poly ctx object
  ~CKKS2POLY_CTX(void) { _pool.Pop(); }

  //! @brief Get lower context
  fhe::core::LOWER_CTX* Lower_ctx() { return _fhe_ctx; }

  //! Get poly config options
  POLY_CONFIG& Config() { return _config; }

  //! @brief Get current container
  CONTAINER* Container() { return _cntr; }

  air::base::FUNC_SCOPE* Func_scope() { return _poly_gen.Func_scope(); }

  void Push_rns_blk(NODE_PAIR blk_pair) { _rns_blk.push(blk_pair); }

  NODE_PAIR Pop_rns_blk() {
    CMPLR_ASSERT(!_rns_blk.empty(), "corrupt rns block stack");
    NODE_PAIR top = _rns_blk.top();
    _rns_blk.pop();
    return top;
  }

  NODE_PTR Rns_outer_blk() {
    CMPLR_ASSERT(!_rns_blk.empty(), "corrupt rns block stack");
    NODE_PAIR top = _rns_blk.top();
    return top.first;
  }

  NODE_PTR Rns_body_blk() {
    CMPLR_ASSERT(!_rns_blk.empty(), "corrupt rns block stack");
    NODE_PAIR top = _rns_blk.top();
    return top.second;
  }

  //! @brief Get Return value kind based on parent and child type
  //! @param parent Parent node
  //! @param tid_child Child type id
  //! @return return value kind: whole CIPH/PLAIN/PLAIN or POLY or RNS POLY
  RETV_KIND Retv_kind(NODE_PTR parent, TYPE_ID tid_child) {
    RETV_KIND retv_list[3][3] = {
        {RETV_KIND::RK_CIPH,  RETV_KIND::RK_CIPH_POLY,
         RETV_KIND::RK_CIPH_RNS_POLY },
        {RETV_KIND::RK_CIPH3, RETV_KIND::RK_CIPH3_POLY,
         RETV_KIND::RK_CIPH3_RNS_POLY},
        {RETV_KIND::RK_PLAIN, RETV_KIND::RK_PLAIN_POLY,
         RETV_KIND::RK_PLAIN_RNS_POLY},
    };
    RETV_KIND* retv_cand;
    if (Lower_ctx()->Is_cipher_type(tid_child)) {
      retv_cand = (RETV_KIND*)&(retv_list[0]);
    } else if (Lower_ctx()->Is_cipher3_type(tid_child)) {
      retv_cand = (RETV_KIND*)&(retv_list[1]);
    } else if (Lower_ctx()->Is_plain_type(tid_child)) {
      retv_cand = (RETV_KIND*)&(retv_list[2]);
    } else {
      CMPLR_ASSERT(false, "unsupported child type");
    }

    if (parent == air::base::Null_ptr) {
      // in unit test, single node have no parent, return POLY kind
      return retv_cand[1];
    }

    if (parent->Domain() == fhe::ckks::CKKS_DOMAIN::ID) {
      switch (parent->Operator()) {
        case fhe::ckks::CKKS_OPERATOR::ADD:
        case fhe::ckks::CKKS_OPERATOR::SUB:
        case fhe::ckks::CKKS_OPERATOR::MUL:
          return retv_cand[2];  // XX_RNS_POLY
        case fhe::ckks::CKKS_OPERATOR::ROTATE:
        case fhe::ckks::CKKS_OPERATOR::RESCALE:
        case fhe::ckks::CKKS_OPERATOR::RELIN:
          return retv_cand[1];  // XX_POLY
        case fhe::ckks::CKKS_OPERATOR::BOOTSTRAP:
          CMPLR_ASSERT(Lower_ctx()->Is_cipher_type(tid_child),
                       "bootstrap child should be ciphertext");
          return retv_cand[0];  // CIPH
        default:
          CMPLR_ASSERT(false, "unsupported ckks op");
      }
    }
    return retv_cand[0];  // orignal kind
  }

  //! @brief Return the node's parent symbol if not exists return Null_ptr
  template <typename VISITOR>
  VAR Parent_sym(VISITOR* visitor, NODE_PTR node) {
    NODE_PTR n_parent = visitor->Parent(1);
    if (n_parent != air::base::Null_ptr && n_parent->Is_st()) {
      if (n_parent->Has_sym()) {
        return VAR(Func_scope(), n_parent->Addr_datum());
      } else if (n_parent->Has_preg()) {
        return VAR(Func_scope(), n_parent->Preg());
      }
    }
    return VAR();
  }

  //! @brief Get POLY_IR_GEN instance
  POLY_IR_GEN& Poly_gen() { return _poly_gen; }

  //! @brief Enter a new function scope, update GLOB_SCOPE, CONTAINER,
  //! FUNC_SCOPE
  void Enter_func(FUNC_SCOPE* fscope) {
    _cntr = &(fscope->Container());
    Poly_gen().Enter_func(fscope);
  }

  //! @brief A general method to handle lower return value.
  //  Return value is determined by both current node and parent node
  CKKS2POLY_RETV Handle_lower_retv(NODE_PTR n_node, NODE_PTR n_parent);

  //! @brief Returns a Memory pool
  POLY_MEM_POOL* Mem_pool() { return &_pool; }

private:
  POLY_MEM_POOL         _pool;
  POLY_CONFIG&          _config;
  fhe::core::LOWER_CTX* _fhe_ctx;
  CONTAINER*            _cntr;
  POLY_IR_GEN           _poly_gen;
  std::stack<NODE_PAIR> _rns_blk;  // stack maintains the RNS loop block
                                   // pair <outer_block, body_block>
};

//! @brief Lower CKKS IR to Poly IR
class CKKS2POLY : public fhe::ckks::DEFAULT_HANDLER {
public:
  //! @brief Construct a new CKKS2POLY object
  CKKS2POLY() {}

  //! @brief Handle CKKS_OPERATOR::ENCODE
  template <typename RETV, typename VISITOR>
  CKKS2POLY_RETV Handle_encode(VISITOR* visitor, NODE_PTR node);

  //! @brief Handle CKKS_OPERATOR::ADD
  template <typename RETV, typename VISITOR>
  CKKS2POLY_RETV Handle_add(VISITOR* visitor, NODE_PTR node);

  //! @brief Handle CKKS_OPERATOR::MUL
  template <typename RETV, typename VISITOR>
  CKKS2POLY_RETV Handle_mul(VISITOR* visitor, NODE_PTR node);

  //! @brief Handle CKKS_OPERATOR::RELIN
  template <typename RETV, typename VISITOR>
  CKKS2POLY_RETV Handle_relin(VISITOR* visitor, NODE_PTR node);

  //! @brief Handle CKKS_OPERATOR::ROTATE
  template <typename RETV, typename VISITOR>
  CKKS2POLY_RETV Handle_rotate(VISITOR* visitor, NODE_PTR node);

  //! @brief Handle CKKS_OPERATOR::RESCALE
  template <typename RETV, typename VISITOR>
  CKKS2POLY_RETV Handle_rescale(VISITOR* visitor, NODE_PTR node);

private:
  CKKS2POLY_RETV Handle_add_ciph(CKKS2POLY_CTX& ctx, NODE_PTR node,
                                 CKKS2POLY_RETV opnd0_pair,
                                 CKKS2POLY_RETV opnd1_pair);

  CKKS2POLY_RETV Handle_add_plain(CKKS2POLY_CTX& ctx, NODE_PTR node,
                                  CKKS2POLY_RETV opnd0_pair,
                                  CKKS2POLY_RETV opnd1_pair);

  CKKS2POLY_RETV Handle_add_float(CKKS2POLY_CTX& ctx, NODE_PTR node,
                                  CKKS2POLY_RETV opnd0_pair,
                                  CKKS2POLY_RETV opnd1_pair);

  CKKS2POLY_RETV Handle_mul_ciph(CKKS2POLY_CTX& ctx, NODE_PTR node,
                                 CKKS2POLY_RETV opnd0_pair,
                                 CKKS2POLY_RETV opnd1_pair);

  CKKS2POLY_RETV Handle_mul_plain(CKKS2POLY_CTX& ctx, NODE_PTR node,
                                  CKKS2POLY_RETV opnd0_pair,
                                  CKKS2POLY_RETV opnd1_pair);

  CKKS2POLY_RETV Handle_mul_float(CKKS2POLY_CTX& ctx, NODE_PTR node,
                                  CKKS2POLY_RETV opnd0_pair,
                                  CKKS2POLY_RETV opnd1_pair);

  // generate IR to allocate memory for keyswitch variables
  // TODO: all memory related op should be postponed to CG phase
  void Handle_kswitch_alloc(CKKS2POLY_CTX& ctx, STMT_LIST& sl, NODE_PTR n_c0,
                            NODE_PTR n_c1, const SPOS& spos);

  // generate IR to free memory for keyswitch variables
  // TODO: all memory related op should be postponed to CG phase
  void Handle_kswitch_free(CKKS2POLY_CTX& ctx, STMT_LIST& sl, const SPOS& spos);

  // generate IR to perform key switch operation
  void Handle_kswitch(CKKS2POLY_CTX& ctx, STMT_LIST& sl, CONST_VAR v_key,
                      NODE_PTR n_c1, const SPOS& spos);

  // generate IR to perform ModDown operation
  // ModDown op will reduce the modulus of a polynomial from Q+P to Q
  void Handle_mod_down(CKKS2POLY_CTX& ctx, STMT_LIST& sl, const SPOS& spos);

  // generate IR to perfom rotate afterwards op of keyswitch
  // res_c0 = input_c0 + mod_down_c0, c1 remains unchanged
  void Handle_rotate_post_keyswitch(CKKS2POLY_CTX& ctx, STMT_LIST& sl,
                                    NODE_PTR n_c0, const SPOS& spos);

  // generate IR to perform polynomial automorphism
  void Handle_automorphism(CKKS2POLY_CTX& ctx, STMT_LIST& sl,
                           CONST_VAR v_rot_res, NODE_PTR n_rot_idx,
                           const SPOS& spos);

  // generate IR to perform relinearlize afterwards op for keyswitch
  // res_c0 = input_c0 + mod_down_c0
  // res_c1 = input_c1 + mod_down_c1
  void Handle_relin_post_keyswitch(CKKS2POLY_CTX& ctx, STMT_LIST& sl,
                                   CONST_VAR v_relin_res, NODE_PTR n_c0,
                                   NODE_PTR n_c1, const SPOS& spos);

  // expand rotate sub operations
  NODE_PTR Expand_rotate(CKKS2POLY_CTX& ctx, NODE_PTR node, NODE_PTR n_c0,
                         NODE_PTR n_c1, NODE_PTR n_opnd1, const SPOS& spos);

  // generate IR to call rotate function, create the function if not ready
  void Call_rotate(CKKS2POLY_CTX& ctx, NODE_PTR node, NODE_PTR n_arg0,
                   NODE_PTR n_arg1);

  // generate rotate function, when PGEN:inline_rotate set to false
  void Gen_rotate_func(CKKS2POLY_CTX& ctx, NODE_PTR node, const SPOS& spos);

  // expand relinearize sub options
  NODE_PTR Expand_relin(CKKS2POLY_CTX& ctx, NODE_PTR node, NODE_PTR n_c0,
                        NODE_PTR n_c1, NODE_PTR n_c2);

  // generate relinearize function, when PGEN:inline_relin set to false
  void Gen_relin_func(CKKS2POLY_CTX& ctx, NODE_PTR node, const SPOS& spos);

  // generate IR to call relinearize function, create the function if not ready
  void Call_relin(CKKS2POLY_CTX& ctx, NODE_PTR node, NODE_PTR n_arg);

  // Check if RNS loop need to be generated at current node
  bool Is_gen_rns_loop(NODE_PTR parent, NODE_PTR node);

  // generate encode for floating point data, the scale and degree are retrived
  // from ciphertext
  NODE_PTR Gen_encode_float_from_ciph(CKKS2POLY_CTX& ctx, CONST_VAR v_ciph,
                                      NODE_PTR n_cst, bool is_mul);

  // Pre handle for ckks operators, generate rns loop if needed
  template <typename VISITOR>
  bool Pre_handle_ckks_op(VISITOR* visitor, NODE_PTR node);

  // Post handle for ckks operators, generate init ciph,
  // add store to result symbol
  template <typename VISITOR>
  CKKS2POLY_RETV Post_handle_ckks_op(VISITOR* visitor, NODE_PTR node,
                                     CKKS2POLY_RETV rhs, bool is_gen_rns_loop);
};

//! @brief Lower IR with Core Opcode to Poly
class CORE2POLY : public air::core::DEFAULT_HANDLER {
public:
  //! @brief Construct a new CKKS2POLY object
  CORE2POLY() {}

  //! @brief Handle CORE::LOAD
  template <typename RETV, typename VISITOR>
  CKKS2POLY_RETV Handle_ld(VISITOR* visitor, NODE_PTR node);

  //! @brief Handle CORE::LDP
  template <typename RETV, typename VISITOR>
  CKKS2POLY_RETV Handle_ldp(VISITOR* visitor, NODE_PTR node);

  //! @brief Handle CORE::ILD
  template <typename RETV, typename VISITOR>
  CKKS2POLY_RETV Handle_ild(VISITOR* visitor, NODE_PTR node);

  //! @brief Handle CORE::STORE
  template <typename RETV, typename VISITOR>
  CKKS2POLY_RETV Handle_st(VISITOR* visitor, NODE_PTR node);

  //! @brief Handle CORE::STP
  template <typename RETV, typename VISITOR>
  CKKS2POLY_RETV Handle_stp(VISITOR* visitor, NODE_PTR node);

  //! @breif Handle CORE::RETV
  template <typename RETV, typename VISITOR>
  CKKS2POLY_RETV Handle_retv(VISITOR* visitor, NODE_PTR node);

private:
  template <typename VISITOR>
  CKKS2POLY_RETV Handle_ld_var(VISITOR* visitor, NODE_PTR node);

  template <typename VISITOR>
  CKKS2POLY_RETV Handle_st_var(VISITOR* visitor, NODE_PTR node, CONST_VAR var);
};

template <typename RETV, typename VISITOR>
CKKS2POLY_RETV CKKS2POLY::Handle_encode(VISITOR* visitor, NODE_PTR node) {
  SPOS           spos     = node->Spos();
  CKKS2POLY_CTX& ctx      = visitor->Context();
  NODE_PTR       n_clone  = ctx.Container()->Clone_node_tree(node);
  CONST_VAR&     v_encode = ctx.Poly_gen().Node_var(node);

  STMT_PTR s_encode = ctx.Poly_gen().New_var_store(n_clone, v_encode, spos);
  ctx.Prepend(s_encode);
  return ctx.Handle_lower_retv(node, visitor->Parent(1));
}

template <typename RETV, typename VISITOR>
CKKS2POLY_RETV CKKS2POLY::Handle_add(VISITOR* visitor, NODE_PTR node) {
  CKKS2POLY_RETV        retv;
  CKKS2POLY_CTX&        ctx             = visitor->Context();
  fhe::core::LOWER_CTX* lower_ctx       = ctx.Lower_ctx();
  bool                  is_gen_rns_loop = Pre_handle_ckks_op(visitor, node);

  // visit add's two child node
  NODE_PTR       opnd0      = node->Child(0);
  NODE_PTR       opnd1      = node->Child(1);
  CKKS2POLY_RETV opnd0_pair = visitor->template Visit<RETV>(opnd0);
  CKKS2POLY_RETV opnd1_pair = visitor->template Visit<RETV>(opnd1);

  if (lower_ctx->Is_cipher_type(opnd1->Rtype_id())) {
    retv = Handle_add_ciph(ctx, node, opnd0_pair, opnd1_pair);
  } else if (lower_ctx->Is_plain_type(opnd1->Rtype_id())) {
    retv = Handle_add_plain(ctx, node, opnd0_pair, opnd1_pair);
  } else if (opnd1->Rtype()->Is_float()) {
    retv = Handle_add_float(ctx, node, opnd0_pair, opnd1_pair);
  } else {
    CMPLR_ASSERT(false, "invalid add opnd_1 type");
    retv = CKKS2POLY_RETV();
  }

  retv = Post_handle_ckks_op(visitor, node, retv, is_gen_rns_loop);
  return retv;
}

template <typename RETV, typename VISITOR>
CKKS2POLY_RETV CKKS2POLY::Handle_mul(VISITOR* visitor, NODE_PTR node) {
  CKKS2POLY_RETV        retv;
  CKKS2POLY_CTX&        ctx             = visitor->Context();
  fhe::core::LOWER_CTX* lower_ctx       = ctx.Lower_ctx();
  bool                  is_gen_rns_loop = Pre_handle_ckks_op(visitor, node);
  NODE_PTR              opnd0           = node->Child(0);
  NODE_PTR              opnd1           = node->Child(1);

  CMPLR_ASSERT(lower_ctx->Is_cipher_type(opnd0->Rtype_id()),
               "invalid mul opnd0");
  CKKS2POLY_RETV opnd0_pair = visitor->template Visit<RETV>(opnd0);
  CKKS2POLY_RETV opnd1_pair = visitor->template Visit<RETV>(opnd1);

  if (lower_ctx->Is_cipher_type(opnd1->Rtype_id())) {
    retv = Handle_mul_ciph(ctx, node, opnd0_pair, opnd1_pair);
  } else if (lower_ctx->Is_plain_type(opnd1->Rtype_id())) {
    retv = Handle_mul_plain(ctx, node, opnd0_pair, opnd1_pair);
  } else if (opnd1->Rtype()->Is_float()) {
    retv = Handle_mul_float(ctx, node, opnd0_pair, opnd1_pair);
  } else {
    CMPLR_ASSERT(false, "invalid mul opnd_1 type");
    retv = CKKS2POLY_RETV();
  }

  retv = Post_handle_ckks_op(visitor, node, retv, is_gen_rns_loop);
  return retv;
}

template <typename RETV, typename VISITOR>
CKKS2POLY_RETV CKKS2POLY::Handle_relin(VISITOR* visitor, NODE_PTR node) {
  CKKS2POLY_CTX& ctx  = visitor->Context();
  CONTAINER*     cntr = ctx.Container();
  SPOS           spos = node->Spos();

  NODE_PTR n_opnd0 = node->Child(0);
  CMPLR_ASSERT(ctx.Lower_ctx()->Is_cipher3_type(n_opnd0->Rtype_id()),
               "invalid relin opnd");

  CKKS2POLY_RETV opnd0_retv = visitor->template Visit<RETV>(n_opnd0);
  CMPLR_ASSERT(
      opnd0_retv.Kind() == RETV_KIND::RK_CIPH3_POLY && !opnd0_retv.Is_null(),
      "invalid relin opnd0");

  NODE_PTR n_ret = air::base::Null_ptr;
  if (ctx.Config().Inline_relin()) {
    NODE_PTR blk = Expand_relin(ctx, node, opnd0_retv.Node1(),
                                opnd0_retv.Node2(), opnd0_retv.Node3());
    ctx.Prepend(blk->Stmt());
  } else {
    // call relin function
    CONST_VAR& v_opnd0 = ctx.Poly_gen().Node_var(n_opnd0);
    NODE_PTR   n_arg   = ctx.Poly_gen().New_var_load(v_opnd0, spos);
    Call_relin(ctx, node, n_arg);
  }

  return ctx.Handle_lower_retv(node, visitor->Parent(1));
}

template <typename RETV, typename VISITOR>
CKKS2POLY_RETV CKKS2POLY::Handle_rotate(VISITOR* visitor, NODE_PTR node) {
  CKKS2POLY_CTX& ctx  = visitor->Context();
  CONTAINER*     cntr = ctx.Container();
  SPOS           spos = node->Spos();

  NODE_PTR n_opnd0 = node->Child(0);
  NODE_PTR n_opnd1 = node->Child(1);

  // TO FIX: opnd0_retv already returned pair of polys which will not be used
  // if gen rotate function
  CKKS2POLY_RETV opnd0_retv = visitor->template Visit<RETV>(n_opnd0);
  CKKS2POLY_RETV opnd1_retv = visitor->template Visit<RETV>(n_opnd1);
  CMPLR_ASSERT(
      opnd0_retv.Kind() == RETV_KIND::RK_CIPH_POLY && !opnd0_retv.Is_null(),
      "invalid rotate opnd0");
  CMPLR_ASSERT(
      opnd1_retv.Kind() == RETV_KIND::RK_DEFAULT && !opnd0_retv.Is_null(),
      "invalid rotate opnd1");

  if (ctx.Config().Inline_rotate()) {
    NODE_PTR blk = Expand_rotate(ctx, node, opnd0_retv.Node1(),
                                 opnd0_retv.Node2(), opnd1_retv.Node(), spos);
    ctx.Prepend(blk->Stmt());
  } else {
    CONST_VAR& v_opnd0 = ctx.Poly_gen().Node_var(n_opnd0);
    NODE_PTR   n_arg0  = ctx.Poly_gen().New_var_load(v_opnd0, spos);
    NODE_PTR   n_arg1  = opnd1_retv.Node();
    Call_rotate(ctx, node, n_arg0, n_arg1);
  }

  return ctx.Handle_lower_retv(node, visitor->Parent(1));
}

template <typename RETV, typename VISITOR>
CKKS2POLY_RETV CKKS2POLY::Handle_rescale(VISITOR* visitor, NODE_PTR node) {
  CKKS2POLY_CTX&        ctx             = visitor->Context();
  fhe::core::LOWER_CTX* lower_ctx       = ctx.Lower_ctx();
  bool                  is_gen_rns_loop = Pre_handle_ckks_op(visitor, node);

  NODE_PTR opnd0 = node->Child(0);
  CMPLR_ASSERT(ctx.Lower_ctx()->Is_cipher_type(opnd0->Rtype_id()),
               "rescale opnd is not ciphertext");

  CKKS2POLY_RETV opnd0_pair = visitor->template Visit<RETV>(opnd0);
  CMPLR_ASSERT(opnd0_pair.Kind() == RETV_KIND::RK_CIPH_POLY,
               "invalid RETV kind");

  NODE_PTR n_rescale_c0 =
      ctx.Poly_gen().New_rescale(opnd0_pair.Node1(), node->Spos());
  NODE_PTR n_rescale_c1 =
      ctx.Poly_gen().New_rescale(opnd0_pair.Node2(), node->Spos());

  CKKS2POLY_RETV retv = Post_handle_ckks_op(
      visitor, node,
      CKKS2POLY_RETV(RETV_KIND::RK_CIPH_POLY, n_rescale_c0, n_rescale_c1),
      is_gen_rns_loop);
  return retv;
}

template <typename VISITOR>
bool CKKS2POLY::Pre_handle_ckks_op(VISITOR* visitor, NODE_PTR node) {
  CKKS2POLY_CTX& ctx             = visitor->Context();
  bool           is_gen_rns_loop = Is_gen_rns_loop(visitor->Parent(1), node);
  if (is_gen_rns_loop) {
    CONST_VAR& v_res    = ctx.Poly_gen().Node_var(node);
    NODE_PTR   n_res_c0 = ctx.Poly_gen().New_poly_load(v_res, 0, node->Spos());
    NODE_PAIR  block_pair = ctx.Poly_gen().New_rns_loop(n_res_c0, false);
    ctx.Push_rns_blk(block_pair);
  }
  return is_gen_rns_loop;
}

template <typename VISITOR>
CKKS2POLY_RETV CKKS2POLY::Post_handle_ckks_op(VISITOR* visitor, NODE_PTR node,
                                              CKKS2POLY_RETV rhs,
                                              bool           is_gen_rns_loop) {
  CKKS2POLY_RETV retv;
  CKKS2POLY_CTX& ctx      = visitor->Context();
  CONST_VAR&     v_node   = ctx.Poly_gen().Node_var(node);
  VAR            v_parent = ctx.Parent_sym(visitor, node);
  NODE_PTR       n_parent = visitor->Parent(1);

  // Add init statement
  STMT_PTR s_init = ctx.Poly_gen().New_init_ciph(v_parent, node);
  ctx.Prepend(s_init);

  switch (rhs.Kind()) {
    case RETV_KIND::RK_CIPH_RNS_POLY: {
      NODE_PTR  body_blk = ctx.Rns_body_blk();
      STMT_PAIR s_pair   = ctx.Poly_gen().New_ciph_poly_store(
          v_node, rhs.Node1(), rhs.Node2(), true, node->Spos());
      ctx.Poly_gen().Append_rns_stmt(s_pair.first, body_blk);
      ctx.Poly_gen().Append_rns_stmt(s_pair.second, body_blk);

      retv = ctx.Handle_lower_retv(node, n_parent);
      break;
    }
    case RETV_KIND::RK_CIPH3_RNS_POLY: {
      NODE_PTR    body_blk = ctx.Rns_body_blk();
      STMT_TRIPLE s_tuple  = ctx.Poly_gen().New_ciph3_poly_store(
          v_node, rhs.Node1(), rhs.Node2(), rhs.Node3(), true, node->Spos());
      ctx.Poly_gen().Append_rns_stmt(std::get<0>(s_tuple), body_blk);
      ctx.Poly_gen().Append_rns_stmt(std::get<1>(s_tuple), body_blk);
      ctx.Poly_gen().Append_rns_stmt(std::get<2>(s_tuple), body_blk);

      retv = ctx.Handle_lower_retv(node, n_parent);
      break;
    }
    case RETV_KIND::RK_CIPH_POLY: {
      STMT_PAIR s_pair = ctx.Poly_gen().New_ciph_poly_store(
          v_node, rhs.Node1(), rhs.Node2(), false, node->Spos());
      ctx.Prepend(s_pair.first);
      ctx.Prepend(s_pair.second);
      retv = ctx.Handle_lower_retv(node, n_parent);
      break;
    }
    default:
      CMPLR_ASSERT(false, "invalid retv kind");
  }
  // reach the end of rns loop, the rns expand is terminated, pop rns blocks
  if (is_gen_rns_loop) {
    ctx.Prepend(ctx.Rns_outer_blk()->Stmt());
    ctx.Pop_rns_blk();
  }
  return retv;
}

template <typename RETV, typename VISITOR>
CKKS2POLY_RETV CORE2POLY::Handle_ld(VISITOR* visitor, NODE_PTR node) {
  return Handle_ld_var(visitor, node);
}

template <typename RETV, typename VISITOR>
CKKS2POLY_RETV CORE2POLY::Handle_ldp(VISITOR* visitor, NODE_PTR node) {
  return Handle_ld_var(visitor, node);
}

template <typename RETV, typename VISITOR>
CKKS2POLY_RETV CORE2POLY::Handle_ild(VISITOR* visitor, NODE_PTR node) {
  CKKS2POLY_CTX& ctx  = visitor->Context();
  CKKS2POLY_RETV retv = Handle_ld_var(visitor, node);
  if (retv.Kind() != RETV_KIND::RK_DEFAULT &&
      retv.Kind() != RETV_KIND::RK_BLOCK) {
    // save ild to temp
    CONST_VAR& v_ild   = ctx.Poly_gen().Node_var(node);
    NODE_PTR   n_clone = ctx.Container()->Clone_node_tree(node);
    STMT_PTR s_ild = ctx.Poly_gen().New_var_store(n_clone, v_ild, node->Spos());
    ctx.Prepend(s_ild);
  }
  return retv;
}

template <typename VISITOR>
CKKS2POLY_RETV CORE2POLY::Handle_ld_var(VISITOR* visitor, NODE_PTR node) {
  TYPE_ID               tid       = node->Rtype_id();
  CKKS2POLY_CTX&        ctx       = visitor->Context();
  fhe::core::LOWER_CTX* lower_ctx = ctx.Lower_ctx();
  if (lower_ctx->Is_cipher_type(tid) || lower_ctx->Is_cipher3_type(tid) ||
      lower_ctx->Is_plain_type(tid)) {
    return ctx.Handle_lower_retv(node, visitor->Parent(1));
  } else {
    // for load node, not cipher/plain/cipher3 type, clone the whole tree
    NODE_PTR n_clone = ctx.Container()->Clone_node_tree(node);
    return CKKS2POLY_RETV(n_clone);
  }
}

template <typename RETV, typename VISITOR>
CKKS2POLY_RETV CORE2POLY::Handle_st(VISITOR* visitor, NODE_PTR node) {
  CKKS2POLY_CTX& ctx = visitor->Context();
  if (!(node->Child(0)->Is_ld())) {
    ctx.Poly_gen().Add_node_var(node->Child(0), node->Addr_datum());
  }
  VAR node_var(ctx.Func_scope(), node->Addr_datum());
  return Handle_st_var(visitor, node, node_var);
}

template <typename RETV, typename VISITOR>
CKKS2POLY_RETV CORE2POLY::Handle_stp(VISITOR* visitor, NODE_PTR node) {
  CKKS2POLY_CTX& ctx = visitor->Context();
  if (!(node->Child(0)->Is_ld())) {
    ctx.Poly_gen().Add_node_var(node->Child(0), node->Preg());
  }
  VAR node_preg(ctx.Func_scope(), node->Preg());
  return Handle_st_var(visitor, node, node_preg);
}

template <typename VISITOR>
CKKS2POLY_RETV CORE2POLY::Handle_st_var(VISITOR* visitor, NODE_PTR node,
                                        CONST_VAR var) {
  CKKS2POLY_CTX&        ctx       = visitor->Context();
  TYPE_ID               tid       = var.Type_id();
  fhe::core::LOWER_CTX* lower_ctx = ctx.Lower_ctx();
  if (!(lower_ctx->Is_cipher_type(tid) || lower_ctx->Is_cipher3_type(tid) ||
        lower_ctx->Is_plain_type(tid)) ||
      node->Child(0)->Is_ld()) {
    // clone the tree for
    // 1) store var type is not ciph/plain related
    // 2) load/ldp for ciph/plain, keep store(res, ciph)
    //    do not lower to store polys for now
    STMT_PTR s_new = ctx.Poly_gen().Container()->Clone_stmt_tree(node->Stmt());
    return CKKS2POLY_RETV(s_new->Node());
  } else {
    CKKS2POLY_RETV retv;
    CONTAINER*     cntr = ctx.Container();
    CKKS2POLY_RETV rhs =
        visitor->template Visit<CKKS2POLY_RETV>(node->Child(0));
    switch (rhs.Kind()) {
      case RETV_KIND::RK_BLOCK:
        // already processed in block
        retv = rhs;
        break;
      case RETV_KIND::RK_DEFAULT: {
        CMPLR_ASSERT(rhs.Node() != air::base::Null_ptr, "null child node");
        STMT_PTR new_stmt = cntr->Clone_stmt(node->Stmt());
        new_stmt->Node()->Set_child(0, rhs.Node());
        retv = CKKS2POLY_RETV(new_stmt->Node());
        break;
      }
      default:
        CMPLR_ASSERT(false, "invalid RETV_KIND");
        retv = CKKS2POLY_RETV();
        break;
    }
    return retv;
  }
}

template <typename RETV, typename VISITOR>
CKKS2POLY_RETV CORE2POLY::Handle_retv(VISITOR* visitor, NODE_PTR node) {
  CKKS2POLY_CTX& ctx     = visitor->Context();
  NODE_PTR       n_opnd0 = node->Child(0);
  AIR_ASSERT_MSG(n_opnd0->Domain() != fhe::ckks::CKKS_DOMAIN::ID,
                 "CKKS domain opnd0 not supported");
  // do not lower retv statement
  STMT_PTR s_new = ctx.Container()->Clone_stmt_tree(node->Stmt());
  return CKKS2POLY_RETV(s_new->Node());
}

}  // namespace poly
}  // namespace fhe

#endif  // FHE_POLY_CKKS2POLY_H
