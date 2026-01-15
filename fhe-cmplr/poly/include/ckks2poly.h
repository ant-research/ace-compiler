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

#include "air/base/timing_util.h"
#include "air/base/transform_ctx.h"
#include "air/base/visitor.h"
#include "air/core/default_handler.h"
#include "air/core/handler.h"
#include "fhe/ckks/ckks_handler.h"
#include "fhe/ckks/default_handler.h"
#include "fhe/core/lower_ctx.h"
#include "fhe/core/rt_timing.h"
#include "fhe/poly/config.h"
#include "poly_ir_gen.h"
#include "poly_lower_ctx.h"

namespace fhe {

namespace poly {

class CKKS2POLY;
class CORE2POLY;
class CKKS2POLY_CTX;
using CKKS2POLY_VISITOR =
    air::base::VISITOR<CKKS2POLY_CTX, air::core::HANDLER<CORE2POLY>,
                       fhe::ckks::HANDLER<CKKS2POLY>>;

//! @brief CKKS2POLY handler context
class CKKS2POLY_CTX : public air::base::TRANSFORM_CTX {
public:
  //! @brief Construct a new ckks2poly ctx object
  CKKS2POLY_CTX(POLY_CONFIG& config, fhe::core::LOWER_CTX* fhe_ctx,
                air::base::CONTAINER* cntr)
      : air::base::TRANSFORM_CTX(cntr),
        _config(config),
        _fhe_ctx(fhe_ctx),
        _cntr(cntr),
        _pool(),
        _poly_gen(cntr, fhe_ctx, Mem_pool()),
        _rns_blk() {
    POLY_LOWER_RETV x[2];
    _pool.Push();
  }

  //! @brief Destroy the ckks2poly ctx object
  ~CKKS2POLY_CTX(void) { _pool.Pop(); }

  //! @brief Get lower context
  fhe::core::LOWER_CTX* Lower_ctx() { return _fhe_ctx; }

  //! Get poly config options
  POLY_CONFIG& Config() { return _config; }

  //! Check if RT_TIMING is enabled
  bool Rt_timing() const { return _config.Rt_timing(); }

  //! @brief Get current container
  air::base::CONTAINER* Container() { return _cntr; }

  air::base::FUNC_SCOPE* Func_scope() { return _poly_gen.Func_scope(); }

  void Push_rns_blk(NODE_PAIR blk_pair) { _rns_blk.push(blk_pair); }

  NODE_PAIR Pop_rns_blk() {
    CMPLR_ASSERT(!_rns_blk.empty(), "corrupt rns block stack");
    NODE_PAIR top = _rns_blk.top();
    _rns_blk.pop();
    return top;
  }

  air::base::NODE_PTR Rns_outer_blk() {
    CMPLR_ASSERT(!_rns_blk.empty(), "corrupt rns block stack");
    NODE_PAIR top = _rns_blk.top();
    return top.first;
  }

  air::base::NODE_PTR Rns_body_blk() {
    CMPLR_ASSERT(!_rns_blk.empty(), "corrupt rns block stack");
    NODE_PAIR top = _rns_blk.top();
    return top.second;
  }

  //! @brief Get Return value kind based on parent and child type
  //! @param parent Parent node
  //! @param tid_child Child type id
  //! @return return value kind: whole CIPH/CIPH3/PLAIN or POLY or RNS POLY
  RETV_KIND Retv_kind(air::base::NODE_PTR parent,
                      air::base::TYPE_ID  tid_child) {
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
        case fhe::ckks::CKKS_OPERATOR::MODSWITCH:
        case fhe::ckks::CKKS_OPERATOR::RELIN:
        case fhe::ckks::CKKS_OPERATOR::NEG:
        case fhe::ckks::CKKS_OPERATOR::UPSCALE:
          return retv_cand[1];  // XX_POLY
        case fhe::ckks::CKKS_OPERATOR::BOOTSTRAP:
        case fhe::ckks::CKKS_OPERATOR::RAISE_MOD:
        case fhe::ckks::CKKS_OPERATOR::CONJUGATE:
        case fhe::ckks::CKKS_OPERATOR::MUL_MONO:
          CMPLR_ASSERT(Lower_ctx()->Is_cipher_type(tid_child),
                       "child should be ciphertext");
          return retv_cand[0];  // CIPH
        case fhe::ckks::CKKS_OPERATOR::ENCODE:
          return retv_cand[0];  // PLAIN -> keep original kind
        case fhe::ckks::CKKS_OPERATOR::SCALE:
        case fhe::ckks::CKKS_OPERATOR::LEVEL:
        case fhe::ckks::CKKS_OPERATOR::BATCH_SIZE:
          return retv_cand[0];  // returns scalar, keep original kind
        case fhe::ckks::CKKS_OPERATOR::FREE:
          return retv_cand[0];  // no return value
        default:
          CMPLR_ASSERT(false, "unsupported ckks op");
      }
    }
    return retv_cand[0];  // orignal kind
  }

  //! @brief Return the node's parent symbol if not exists return Null_ptr
  template <typename VISITOR>
  VAR Parent_sym(VISITOR* visitor, air::base::NODE_PTR node) {
    air::base::NODE_PTR n_parent = visitor->Parent(1);
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

  //! @brief Enter a new function scope, update GLOB_SCOPE,
  //! air::base::CONTAINER, air::base::FUNC_SCOPE
  void Enter_func(air::base::FUNC_SCOPE* fscope) {
    _cntr = &(fscope->Container());
    Poly_gen().Enter_func(fscope);
  }

  //! @brief A general method to handle lower return value.
  //  Return value is determined by both current node and parent node
  POLY_LOWER_RETV Handle_lower_retv(air::base::NODE_PTR n_node,
                                    air::base::NODE_PTR n_parent);

  //! @brief Returns a Memory pool
  POLY_MEM_POOL* Mem_pool() { return &_pool; }

private:
  POLY_MEM_POOL         _pool;
  POLY_CONFIG&          _config;
  fhe::core::LOWER_CTX* _fhe_ctx;
  air::base::CONTAINER* _cntr;
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
  POLY_LOWER_RETV Handle_encode(VISITOR* visitor, air::base::NODE_PTR node);

  //! @brief Handle CKKS_OPERATOR::ADD
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_add(VISITOR* visitor, air::base::NODE_PTR node);

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

private:
  POLY_LOWER_RETV Handle_add_ciph(CKKS2POLY_CTX& ctx, air::base::NODE_PTR node,
                                  POLY_LOWER_RETV opnd0_pair,
                                  POLY_LOWER_RETV opnd1_pair);

  POLY_LOWER_RETV Handle_add_plain(CKKS2POLY_CTX& ctx, air::base::NODE_PTR node,
                                   POLY_LOWER_RETV opnd0_pair,
                                   POLY_LOWER_RETV opnd1_pair);

  POLY_LOWER_RETV Handle_add_float(CKKS2POLY_CTX& ctx, air::base::NODE_PTR node,
                                   POLY_LOWER_RETV opnd0_pair,
                                   POLY_LOWER_RETV opnd1_pair);

  POLY_LOWER_RETV Handle_mul_ciph(CKKS2POLY_CTX& ctx, air::base::NODE_PTR node,
                                  POLY_LOWER_RETV opnd0_pair,
                                  POLY_LOWER_RETV opnd1_pair);

  POLY_LOWER_RETV Handle_mul_plain(CKKS2POLY_CTX& ctx, air::base::NODE_PTR node,
                                   POLY_LOWER_RETV opnd0_pair,
                                   POLY_LOWER_RETV opnd1_pair);

  POLY_LOWER_RETV Handle_mul_float(CKKS2POLY_CTX& ctx, air::base::NODE_PTR node,
                                   POLY_LOWER_RETV opnd0_pair,
                                   POLY_LOWER_RETV opnd1_pair);

  // generate IR to allocate memory for keyswitch variables
  // TODO: all memory related op should be postponed to CG phase
  void Handle_kswitch_alloc(CKKS2POLY_CTX& ctx, air::base::STMT_LIST& sl,
                            air::base::NODE_PTR n_c0, air::base::NODE_PTR n_c1,
                            const air::base::SPOS& spos);

  // generate IR to free memory for keyswitch variables
  // TODO: all memory related op should be postponed to CG phase
  void Handle_kswitch_free(CKKS2POLY_CTX& ctx, air::base::STMT_LIST& sl,
                           const air::base::SPOS& spos);

  // generate IR to perform key switch operation
  void Handle_kswitch(CKKS2POLY_CTX& ctx, air::base::STMT_LIST& sl,
                      CONST_VAR v_key, air::base::NODE_PTR n_c1,
                      const air::base::SPOS& spos);

  // generate IR to perform ModDown operation
  // ModDown op will reduce the modulus of a polynomial from Q+P to Q
  void Handle_mod_down(CKKS2POLY_CTX& ctx, air::base::STMT_LIST& sl,
                       const air::base::SPOS& spos);

  // generate IR to perfom rotate afterwards op of keyswitch
  // res_c0 = input_c0 + mod_down_c0, c1 remains unchanged
  void Handle_rotate_post_keyswitch(CKKS2POLY_CTX&         ctx,
                                    air::base::STMT_LIST&  sl,
                                    air::base::NODE_PTR    n_c0,
                                    const air::base::SPOS& spos);

  // generate IR to perform polynomial automorphism
  void Handle_automorphism(CKKS2POLY_CTX& ctx, air::base::STMT_LIST& sl,
                           CONST_VAR v_rot_res, air::base::NODE_PTR n_rot_idx,
                           const air::base::SPOS& spos);

  // generate IR to perform relinearlize afterwards op for keyswitch
  // res_c0 = input_c0 + mod_down_c0
  // res_c1 = input_c1 + mod_down_c1
  void Handle_relin_post_keyswitch(CKKS2POLY_CTX& ctx, air::base::STMT_LIST& sl,
                                   CONST_VAR              v_relin_res,
                                   air::base::NODE_PTR    n_c0,
                                   air::base::NODE_PTR    n_c1,
                                   const air::base::SPOS& spos);

  // expand rotate sub operations
  air::base::NODE_PTR Expand_rotate(CKKS2POLY_CTX&         ctx,
                                    air::base::NODE_PTR    node,
                                    air::base::NODE_PTR    n_c0,
                                    air::base::NODE_PTR    n_c1,
                                    air::base::NODE_PTR    n_opnd1,
                                    const air::base::SPOS& spos);

  // generate IR to call rotate function, create the function if not ready
  void Call_rotate(CKKS2POLY_CTX& ctx, air::base::NODE_PTR node,
                   air::base::NODE_PTR n_arg0, air::base::NODE_PTR n_arg1);

  // generate rotate function, when PGEN:inline_rotate set to false
  air::base::FUNC_SCOPE* Gen_rotate_func(CKKS2POLY_CTX&         ctx,
                                         air::base::NODE_PTR    node,
                                         const air::base::SPOS& spos);

  // expand relinearize sub options
  air::base::NODE_PTR Expand_relin(CKKS2POLY_CTX& ctx, air::base::NODE_PTR node,
                                   air::base::NODE_PTR n_c0,
                                   air::base::NODE_PTR n_c1,
                                   air::base::NODE_PTR n_c2);

  // generate relinearize function, when PGEN:inline_relin set to false
  air::base::FUNC_SCOPE* Gen_relin_func(CKKS2POLY_CTX&         ctx,
                                        air::base::NODE_PTR    node,
                                        const air::base::SPOS& spos);

  // generate IR to call relinearize function, create the function if not ready
  void Call_relin(CKKS2POLY_CTX& ctx, air::base::NODE_PTR node,
                  air::base::NODE_PTR n_arg);

  // Check if RNS loop need to be generated at current node
  bool Is_gen_rns_loop(CKKS2POLY_CTX& ctx, air::base::NODE_PTR parent,
                       air::base::NODE_PTR node);

  // generate encode for floating point data, the scale and degree are retrived
  // from ciphertext
  air::base::NODE_PTR Gen_encode_float_from_ciph(CKKS2POLY_CTX&      ctx,
                                                 CONST_VAR           v_ciph,
                                                 air::base::NODE_PTR n_cst,
                                                 bool                is_mul);

  // Pre handle for ckks operators, generate rns loop if needed
  template <typename VISITOR>
  bool Pre_handle_ckks_op(VISITOR* visitor, air::base::NODE_PTR node);

  // Post handle for ckks operators, generate init ciph,
  // add store to result symbol
  template <typename VISITOR>
  POLY_LOWER_RETV Post_handle_ckks_op(VISITOR*            visitor,
                                      air::base::NODE_PTR node,
                                      POLY_LOWER_RETV rhs, bool is_gen_rns_loop,
                                      uint32_t event);
};

//! @brief Lower IR with Core Opcode to Poly
class CORE2POLY : public air::core::DEFAULT_HANDLER {
public:
  //! @brief Construct a new CKKS2POLY object
  CORE2POLY() {}

  //! @brief Handle CORE::LOAD
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_ld(VISITOR* visitor, air::base::NODE_PTR node);

  //! @brief Handle CORE::LDP
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_ldp(VISITOR* visitor, air::base::NODE_PTR node);

  //! @brief Handle CORE::ILD
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_ild(VISITOR* visitor, air::base::NODE_PTR node);

  //! @brief Handle CORE::STORE
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_st(VISITOR* visitor, air::base::NODE_PTR node);

  //! @brief Handle CORE::STP
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_stp(VISITOR* visitor, air::base::NODE_PTR node);

  //! @breif Handle CORE::RETV
  template <typename RETV, typename VISITOR>
  POLY_LOWER_RETV Handle_retv(VISITOR* visitor, air::base::NODE_PTR node);

private:
  template <typename VISITOR>
  POLY_LOWER_RETV Handle_ld_var(VISITOR* visitor, air::base::NODE_PTR node);

  template <typename VISITOR>
  POLY_LOWER_RETV Handle_st_var(VISITOR* visitor, air::base::NODE_PTR node,
                                CONST_VAR var);
};

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CKKS2POLY::Handle_encode(VISITOR*            visitor,
                                         air::base::NODE_PTR node) {
  air::base::SPOS     spos     = node->Spos();
  CKKS2POLY_CTX&      ctx      = visitor->Context();
  air::base::NODE_PTR n_clone  = ctx.Container()->Clone_node_tree(node);
  CONST_VAR&          v_encode = ctx.Poly_gen().Node_var(node);

  air::base::STMT_PTR s_encode =
      ctx.Poly_gen().New_var_store(n_clone, v_encode, spos);
  ctx.Prepend(s_encode);
  return ctx.Handle_lower_retv(node, visitor->Parent(1));
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CKKS2POLY::Handle_add(VISITOR*            visitor,
                                      air::base::NODE_PTR node) {
  POLY_LOWER_RETV       retv;
  CKKS2POLY_CTX&        ctx             = visitor->Context();
  fhe::core::LOWER_CTX* lower_ctx       = ctx.Lower_ctx();
  bool                  is_gen_rns_loop = Pre_handle_ckks_op(visitor, node);

  // visit add's two child node
  air::base::NODE_PTR opnd0      = node->Child(0);
  air::base::NODE_PTR opnd1      = node->Child(1);
  POLY_LOWER_RETV     opnd0_pair = visitor->template Visit<RETV>(opnd0);
  POLY_LOWER_RETV     opnd1_pair = visitor->template Visit<RETV>(opnd1);

  uint32_t event = air::base::RTM_NONE;
  if (lower_ctx->Is_cipher_type(opnd1->Rtype_id()) ||
      lower_ctx->Is_cipher3_type(opnd1->Rtype_id())) {
    retv  = Handle_add_ciph(ctx, node, opnd0_pair, opnd1_pair);
    event = core::RTM_FHE_ADDCC;
  } else if (lower_ctx->Is_plain_type(opnd1->Rtype_id())) {
    retv  = Handle_add_plain(ctx, node, opnd0_pair, opnd1_pair);
    event = core::RTM_FHE_ADDCP;
  } else if (opnd1->Rtype()->Is_float() || opnd1->Rtype()->Is_int()) {
    retv  = Handle_add_float(ctx, node, opnd0_pair, opnd1_pair);
    event = core::RTM_FHE_ADDCP;
  } else if (opnd1->Rtype()->Is_prim()) {
    // Handle other primitive types (e.g., from Python DSL Int32)
    retv  = Handle_add_float(ctx, node, opnd0_pair, opnd1_pair);
    event = core::RTM_FHE_ADDCP;
  } else {
    CMPLR_ASSERT(false, "invalid add opnd_1 type");
    retv = POLY_LOWER_RETV();
  }

  retv = Post_handle_ckks_op(visitor, node, retv, is_gen_rns_loop, event);
  return retv;
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CKKS2POLY::Handle_mul(VISITOR*            visitor,
                                      air::base::NODE_PTR node) {
  POLY_LOWER_RETV       retv;
  CKKS2POLY_CTX&        ctx             = visitor->Context();
  fhe::core::LOWER_CTX* lower_ctx       = ctx.Lower_ctx();
  bool                  is_gen_rns_loop = Pre_handle_ckks_op(visitor, node);
  air::base::NODE_PTR   opnd0           = node->Child(0);
  air::base::NODE_PTR   opnd1           = node->Child(1);

  CMPLR_ASSERT(lower_ctx->Is_cipher_type(opnd0->Rtype_id()) ||
                   lower_ctx->Is_cipher3_type(opnd0->Rtype_id()),
               "invalid mul opnd0 (must be CIPHERTEXT or CIPHERTEXT3)");

  POLY_LOWER_RETV opnd0_pair = visitor->template Visit<RETV>(opnd0);
  POLY_LOWER_RETV opnd1_pair = visitor->template Visit<RETV>(opnd1);

  uint32_t event = air::base::RTM_NONE;
  if (lower_ctx->Is_cipher_type(opnd1->Rtype_id())) {
    retv  = Handle_mul_ciph(ctx, node, opnd0_pair, opnd1_pair);
    event = core::RTM_FHE_MULCC;
  } else if (lower_ctx->Is_plain_type(opnd1->Rtype_id())) {
    retv  = Handle_mul_plain(ctx, node, opnd0_pair, opnd1_pair);
    event = core::RTM_FHE_MULCP;
  } else if (opnd1->Rtype()->Is_float() || opnd1->Rtype()->Is_prim()) {
    // Handle float and other primitive types (e.g., int from Python DSL)
    retv  = Handle_mul_float(ctx, node, opnd0_pair, opnd1_pair);
    event = core::RTM_FHE_MULCP;
  } else {
    CMPLR_ASSERT(false, "invalid mul opnd_1 type");
    retv = POLY_LOWER_RETV();
  }

  retv = Post_handle_ckks_op(visitor, node, retv, is_gen_rns_loop, event);
  return retv;
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CKKS2POLY::Handle_relin(VISITOR*            visitor,
                                        air::base::NODE_PTR node) {
  CKKS2POLY_CTX&        ctx  = visitor->Context();
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
    air::base::NODE_PTR blk = Expand_relin(
        ctx, node, opnd0_retv.Node1(), opnd0_retv.Node2(), opnd0_retv.Node3());
    ctx.Prepend(air::base::STMT_LIST(blk));
  } else {
    // call relin function
    CONST_VAR&          v_opnd0 = ctx.Poly_gen().Node_var(n_opnd0);
    air::base::NODE_PTR n_arg   = ctx.Poly_gen().New_var_load(v_opnd0, spos);
    Call_relin(ctx, node, n_arg);
  }

  return ctx.Handle_lower_retv(node, visitor->Parent(1));
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CKKS2POLY::Handle_rotate(VISITOR*            visitor,
                                         air::base::NODE_PTR node) {
  CKKS2POLY_CTX&        ctx  = visitor->Context();
  air::base::CONTAINER* cntr = ctx.Container();
  air::base::SPOS       spos = node->Spos();

  air::base::NODE_PTR n_opnd0 = node->Child(0);
  air::base::NODE_PTR n_opnd1 = node->Child(1);

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

  if (ctx.Config().Inline_rotate()) {
    air::base::NODE_PTR blk =
        Expand_rotate(ctx, node, opnd0_retv.Node1(), opnd0_retv.Node2(),
                      opnd1_retv.Node(), spos);
    ctx.Prepend(air::base::STMT_LIST(blk));
  } else {
    CONST_VAR&          v_opnd0 = ctx.Poly_gen().Node_var(n_opnd0);
    air::base::NODE_PTR n_arg0  = ctx.Poly_gen().New_var_load(v_opnd0, spos);
    air::base::NODE_PTR n_arg1  = opnd1_retv.Node();
    Call_rotate(ctx, node, n_arg0, n_arg1);
  }

  return ctx.Handle_lower_retv(node, visitor->Parent(1));
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CKKS2POLY::Handle_rescale(VISITOR*            visitor,
                                          air::base::NODE_PTR node) {
  CKKS2POLY_CTX&        ctx             = visitor->Context();
  fhe::core::LOWER_CTX* lower_ctx       = ctx.Lower_ctx();
  bool                  is_gen_rns_loop = Pre_handle_ckks_op(visitor, node);

  air::base::NODE_PTR opnd0 = node->Child(0);
  CMPLR_ASSERT(ctx.Lower_ctx()->Is_cipher_type(opnd0->Rtype_id()) ||
                   ctx.Lower_ctx()->Is_cipher3_type(opnd0->Rtype_id()),
               "rescale opnd is not ciphertext/ciphertext3");

  POLY_LOWER_RETV opnd0_pair = visitor->template Visit<RETV>(opnd0);
  CMPLR_ASSERT(opnd0_pair.Kind() == RETV_KIND::RK_CIPH_POLY ||
                   opnd0_pair.Kind() == RETV_KIND::RK_CIPH3_POLY,
               "invalid RETV kind");

  air::base::NODE_PTR n_rescale_c0 =
      ctx.Poly_gen().New_rescale(opnd0_pair.Node1(), node->Spos());
  air::base::NODE_PTR n_rescale_c1 =
      ctx.Poly_gen().New_rescale(opnd0_pair.Node2(), node->Spos());
  if (opnd0_pair.Kind() == RK_CIPH_POLY) {
    POLY_LOWER_RETV retv = Post_handle_ckks_op(
        visitor, node,
        POLY_LOWER_RETV(RETV_KIND::RK_CIPH_POLY, n_rescale_c0, n_rescale_c1),
        is_gen_rns_loop, core::RTM_FHE_RESCALE);
    return retv;
  }

  AIR_ASSERT(opnd0_pair.Kind() == RK_CIPH3_POLY);
  air::base::NODE_PTR n_rescale_c2 =
      ctx.Poly_gen().New_rescale(opnd0_pair.Node3(), node->Spos());
  POLY_LOWER_RETV retv = Post_handle_ckks_op(
      visitor, node,
      POLY_LOWER_RETV(RETV_KIND::RK_CIPH3_POLY, n_rescale_c0, n_rescale_c1,
                      n_rescale_c2),
      is_gen_rns_loop, core::RTM_FHE_RESCALE);
  return retv;
}

//! @brief Handle CKKS_OPERATOR::MODSWITCH
template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CKKS2POLY::Handle_modswitch(VISITOR*            visitor,
                                            air::base::NODE_PTR node) {
  CKKS2POLY_CTX&        ctx             = visitor->Context();
  fhe::core::LOWER_CTX* lower_ctx       = ctx.Lower_ctx();
  bool                  is_gen_rns_loop = Pre_handle_ckks_op(visitor, node);
  air::base::NODE_PTR   opnd0           = node->Child(0);
  CMPLR_ASSERT(lower_ctx->Is_cipher_type(opnd0->Rtype_id()) ||
                   lower_ctx->Is_cipher3_type(opnd0->Rtype_id()),
               "modswitch opnd is not ciphertext/ciphertext3");

  POLY_LOWER_RETV     opnd0_pair = visitor->template Visit<RETV>(opnd0);
  air::base::NODE_PTR n_modswitch_c0 =
      ctx.Poly_gen().New_modswitch(opnd0_pair.Node1(), node->Spos());
  air::base::NODE_PTR n_modswitch_c1 =
      ctx.Poly_gen().New_modswitch(opnd0_pair.Node2(), node->Spos());
  if (opnd0_pair.Kind() == RETV_KIND::RK_CIPH_POLY) {
    POLY_LOWER_RETV retv =
        Post_handle_ckks_op(visitor, node,
                            POLY_LOWER_RETV(RETV_KIND::RK_CIPH_POLY,
                                            n_modswitch_c0, n_modswitch_c1),
                            is_gen_rns_loop, core::RTM_FHE_MODSWITCH);
    return retv;
  }

  AIR_ASSERT(opnd0_pair.Kind() == RETV_KIND::RK_CIPH3_POLY);
  air::base::NODE_PTR n_modswitch_c2 =
      ctx.Poly_gen().New_modswitch(opnd0_pair.Node3(), node->Spos());
  POLY_LOWER_RETV retv = Post_handle_ckks_op(
      visitor, node,
      POLY_LOWER_RETV(RETV_KIND::RK_CIPH3_POLY, n_modswitch_c0, n_modswitch_c1,
                      n_modswitch_c2),
      is_gen_rns_loop, core::RTM_FHE_MODSWITCH);
  return retv;
}

template <typename VISITOR>
bool CKKS2POLY::Pre_handle_ckks_op(VISITOR* visitor, air::base::NODE_PTR node) {
  CKKS2POLY_CTX& ctx   = visitor->Context();
  bool is_gen_rns_loop = Is_gen_rns_loop(ctx, visitor->Parent(1), node);
  if (is_gen_rns_loop) {
    CONST_VAR&          v_res = ctx.Poly_gen().Node_var(node);
    air::base::NODE_PTR n_res_c0 =
        ctx.Poly_gen().New_poly_load(v_res, 0, node->Spos());
    NODE_PAIR block_pair = ctx.Poly_gen().New_rns_loop(n_res_c0, false);
    ctx.Push_rns_blk(block_pair);
  }
  return is_gen_rns_loop;
}

template <typename VISITOR>
POLY_LOWER_RETV CKKS2POLY::Post_handle_ckks_op(VISITOR*            visitor,
                                               air::base::NODE_PTR node,
                                               POLY_LOWER_RETV     rhs,
                                               bool     is_gen_rns_loop,
                                               uint32_t event) {
  POLY_LOWER_RETV     retv;
  CKKS2POLY_CTX&      ctx      = visitor->Context();
  CONST_VAR&          v_node   = ctx.Poly_gen().Node_var(node);
  VAR                 v_parent = ctx.Parent_sym(visitor, node);
  air::base::NODE_PTR n_parent = visitor->Parent(1);

  // insert timing start. timing end prepended automatically by destructor
  air::base::TIMING_UTIL tm(ctx, event, 0, node->Spos(), true);

  // Add init statement
  air::base::STMT_PTR s_init = ctx.Poly_gen().New_init_ciph(v_parent, node);
  if (s_init != air::base::STMT_PTR()) {
    ctx.Prepend(s_init);
  }

  switch (rhs.Kind()) {
    case RETV_KIND::RK_CIPH_RNS_POLY: {
      air::base::NODE_PTR body_blk = ctx.Rns_body_blk();
      STMT_PAIR           s_pair   = ctx.Poly_gen().New_ciph_poly_store(
          v_node, rhs.Node1(), rhs.Node2(), true, node->Spos());
      ctx.Poly_gen().Append_rns_stmt(s_pair.first, body_blk);
      ctx.Poly_gen().Append_rns_stmt(s_pair.second, body_blk);

      retv = ctx.Handle_lower_retv(node, n_parent);
      break;
    }
    case RETV_KIND::RK_CIPH3_RNS_POLY: {
      air::base::NODE_PTR body_blk = ctx.Rns_body_blk();
      STMT_TRIPLE         s_tuple  = ctx.Poly_gen().New_ciph3_poly_store(
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
    case RETV_KIND::RK_CIPH3_POLY: {
      STMT_TRIPLE s_triple = ctx.Poly_gen().New_ciph3_poly_store(
          v_node, rhs.Node1(), rhs.Node2(), rhs.Node3(), false, node->Spos());
      ctx.Prepend(std::get<0>(s_triple));
      ctx.Prepend(std::get<1>(s_triple));
      ctx.Prepend(std::get<2>(s_triple));
      retv = ctx.Handle_lower_retv(node, n_parent);
      break;
    }
    default:
      CMPLR_ASSERT(false, "invalid retv kind");
  }
  // reach the end of rns loop, the rns expand is terminated, pop rns blocks
  if (is_gen_rns_loop) {
    ctx.Prepend(air::base::STMT_LIST(ctx.Rns_outer_blk()));
    ctx.Pop_rns_blk();
  }

  return retv;
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CORE2POLY::Handle_ld(VISITOR*            visitor,
                                     air::base::NODE_PTR node) {
  return Handle_ld_var(visitor, node);
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CORE2POLY::Handle_ldp(VISITOR*            visitor,
                                      air::base::NODE_PTR node) {
  return Handle_ld_var(visitor, node);
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CORE2POLY::Handle_ild(VISITOR*            visitor,
                                      air::base::NODE_PTR node) {
  CKKS2POLY_CTX&  ctx  = visitor->Context();
  POLY_LOWER_RETV retv = Handle_ld_var(visitor, node);
  if (retv.Kind() != RETV_KIND::RK_DEFAULT &&
      retv.Kind() != RETV_KIND::RK_BLOCK) {
    // save ild to temp
    CONST_VAR&          v_ild   = ctx.Poly_gen().Node_var(node);
    air::base::NODE_PTR n_clone = ctx.Container()->Clone_node_tree(node);
    air::base::STMT_PTR s_ild =
        ctx.Poly_gen().New_var_store(n_clone, v_ild, node->Spos());
    ctx.Prepend(s_ild);
  }
  return retv;
}

template <typename VISITOR>
POLY_LOWER_RETV CORE2POLY::Handle_ld_var(VISITOR*            visitor,
                                         air::base::NODE_PTR node) {
  air::base::TYPE_ID    tid       = node->Rtype_id();
  CKKS2POLY_CTX&        ctx       = visitor->Context();
  fhe::core::LOWER_CTX* lower_ctx = ctx.Lower_ctx();
  if (lower_ctx->Is_cipher_type(tid) || lower_ctx->Is_cipher3_type(tid) ||
      lower_ctx->Is_plain_type(tid)) {
    return ctx.Handle_lower_retv(node, visitor->Parent(1));
  } else {
    // for load node, not cipher/plain/cipher3 type, clone the whole tree
    air::base::NODE_PTR n_clone = ctx.Container()->Clone_node_tree(node);
    return POLY_LOWER_RETV(n_clone);
  }
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CORE2POLY::Handle_st(VISITOR*            visitor,
                                     air::base::NODE_PTR node) {
  CKKS2POLY_CTX& ctx = visitor->Context();
  if (!(node->Child(0)->Is_ld())) {
    ctx.Poly_gen().Add_node_var(node->Child(0), node->Addr_datum());
  }
  VAR node_var(ctx.Func_scope(), node->Addr_datum());
  return Handle_st_var(visitor, node, node_var);
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CORE2POLY::Handle_stp(VISITOR*            visitor,
                                      air::base::NODE_PTR node) {
  CKKS2POLY_CTX& ctx = visitor->Context();
  if (!(node->Child(0)->Is_ld())) {
    ctx.Poly_gen().Add_node_var(node->Child(0), node->Preg());
  }
  VAR node_preg(ctx.Func_scope(), node->Preg());
  return Handle_st_var(visitor, node, node_preg);
}

template <typename VISITOR>
POLY_LOWER_RETV CORE2POLY::Handle_st_var(VISITOR*            visitor,
                                         air::base::NODE_PTR node,
                                         CONST_VAR           var) {
  air::base::TYPE_ID    tid       = var.Type_id();
  CKKS2POLY_CTX&        ctx       = visitor->Context();
  fhe::core::LOWER_CTX* lower_ctx = ctx.Lower_ctx();
  if (!(lower_ctx->Is_cipher_type(tid) || lower_ctx->Is_cipher3_type(tid) ||
        lower_ctx->Is_plain_type(tid)) ||
      node->Child(0)->Is_ld()) {
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
    switch (rhs.Kind()) {
      case RETV_KIND::RK_BLOCK:
        // already processed in block
        retv = rhs;
        break;
      case RETV_KIND::RK_DEFAULT: {
        CMPLR_ASSERT(rhs.Node() != air::base::Null_ptr, "null child node");
        air::base::STMT_PTR new_stmt = cntr->Clone_stmt(node->Stmt());
        new_stmt->Node()->Set_child(0, rhs.Node());
        retv = POLY_LOWER_RETV(new_stmt->Node());
        break;
      }
      default:
        CMPLR_ASSERT(false, "invalid RETV_KIND");
        retv = POLY_LOWER_RETV();
        break;
    }
    return retv;
  }
}

template <typename RETV, typename VISITOR>
POLY_LOWER_RETV CORE2POLY::Handle_retv(VISITOR*            visitor,
                                       air::base::NODE_PTR node) {
  CKKS2POLY_CTX&      ctx     = visitor->Context();
  air::base::NODE_PTR n_opnd0 = node->Child(0);
  AIR_ASSERT_MSG(n_opnd0->Domain() != fhe::ckks::CKKS_DOMAIN::ID,
                 "CKKS domain opnd0 not supported");
  // do not lower retv statement
  air::base::STMT_PTR s_new = ctx.Container()->Clone_stmt_tree(node->Stmt());
  return POLY_LOWER_RETV(s_new->Node());
}

}  // namespace poly
}  // namespace fhe

#endif  // FHE_POLY_CKKS2POLY_H
