//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_SIHE2CKKS_H
#define FHE_CKKS_SIHE2CKKS_H

#include <cstddef>
#include <string>
#include <vector>

#include "air/base/container.h"
#include "air/base/st.h"
#include "air/base/transform_ctx.h"
#include "air/base/visitor.h"
#include "air/core/handler.h"
#include "air/util/debug.h"
#include "fhe/ckks/ckks_gen.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/ckks/ckks_py_airgen.h"
#include "fhe/ckks/invalid_handler.h"
#include "fhe/ckks/sihe2ckks_ctx.h"
#include "fhe/sihe/core_lower_impl.h"
#include "fhe/sihe/sihe_gen.h"
#include "fhe/sihe/sihe_handler.h"
#include "fhe/sihe/sihe_opcode.h"
#include "fhe/sihe/skip_lowering.h"
#include "nn/core/attr.h"

namespace fhe {

namespace ckks {

using namespace air::base;
class SIHE2CKKS_IMPL : public INVALID_HANDLER {
public:
  using CORE_HANDLER = air::core::HANDLER<core::CORE_LOWER>;
  using SIHE_HANDLER = sihe::HANDLER<SIHE2CKKS_IMPL>;
  using LOWER_VISITOR =
      air::base::VISITOR<air::base::TRANSFORM_CTX, CORE_HANDLER, SIHE_HANDLER>;

  SIHE2CKKS_IMPL() {}

  template <typename RETV, typename VISITOR>
  RETV Handle_add(VISITOR* visitor, NODE_PTR node);
  template <typename RETV, typename VISITOR>
  RETV Handle_mul(VISITOR* visitor, NODE_PTR node);
  template <typename RETV, typename VISITOR>
  RETV Handle_sub(VISITOR* visitor, NODE_PTR node);
  template <typename RETV, typename VISITOR>
  RETV Handle_rotate(VISITOR* visitor, NODE_PTR node);
  template <typename RETV, typename VISITOR>
  RETV Handle_encode(VISITOR* visitor, NODE_PTR node);
  template <typename RETV, typename VISITOR>
  RETV Handle_bootstrap(VISITOR* visitor, NODE_PTR node);

  template <typename RETV, typename VISITOR>
  RETV Handle_add_msg(VISITOR* visitor, NODE_PTR node);
  template <typename RETV, typename VISITOR>
  RETV Handle_mul_msg(VISITOR* visitor, NODE_PTR node);
  template <typename RETV, typename VISITOR>
  RETV Handle_rotate_msg(VISITOR* visitor, NODE_PTR node);
  template <typename RETV, typename VISITOR>
  RETV Handle_relu_msg(VISITOR* visitor, NODE_PTR node);
  template <typename RETV, typename VISITOR>
  RETV Handle_bootstrap_msg(VISITOR* visitor, NODE_PTR node);

private:
  template <typename RETV, typename VISITOR>
  RETV Handle_bin_arith_node(VISITOR* visitor, NODE_PTR node, OPCODE new_opcode,
                             TYPE_PTR rtype);
  template <typename RETV, typename VISITOR>
  RETV Handle_encode_in_bin_arith_node(VISITOR* visitor, NODE_PTR node,
                                       NODE_PTR sibling_node);

  NODE_PTR Gen_tmp_for_complex_node(TRANSFORM_CTX& ctx, NODE_PTR node) {
    SPOS           spos = node->Spos();
    std::string    name("_ckks_gen_tmp_" + std::to_string(node->Id().Value()));
    ADDR_DATUM_PTR tmp_var =
        ctx.Func_scope()->New_var(node->Rtype(), name.c_str(), spos);
    STMT_PTR st = ctx.Container()->New_st(node, tmp_var, spos);
    ctx.Prepend(st);
    NODE_PTR ld_tmp = ctx.Container()->New_ld(tmp_var, spos);
    return ld_tmp;
  }
};

template <typename RETV, typename VISITOR>
RETV SIHE2CKKS_IMPL::Handle_encode_in_bin_arith_node(VISITOR* visitor,
                                                     NODE_PTR node,
                                                     NODE_PTR sibling_node) {
  CONTAINER*       cntr      = visitor->Context().Container();
  core::LOWER_CTX& lower_ctx = visitor->Context().Lower_ctx();
  CMPLR_ASSERT(node->Opcode() == sihe::OPC_ENCODE, "must be encode node");
  CMPLR_ASSERT(sibling_node->Rtype_id() == lower_ctx.Get_cipher_type_id(),
               "sibling node must be ciphertext");

  // 1. gen CKKS child0
  NODE_PTR child0 = node->Child(0);
  // CMPLR_ASSERT(child0->Is_const_ld(),
  //              "only support encode constant array that load with ldc");
  NODE_PTR new_child0 = cntr->Clone_node_tree(child0);

  SPOS spos = node->Spos();

  // 2. gen msg len
  NODE_PTR child1 = node->Child(1);
  AIR_ASSERT(child1->Rtype()->Is_int());
  NODE_PTR len_node = visitor->template Visit<RETV>(child1).Node();

  // 3. gen scale node
  OPCODE   get_scale_op(CKKS_DOMAIN::ID, CKKS_OPERATOR::SCALE);
  TYPE_PTR scale_type = lower_ctx.Get_scale_type(cntr->Glob_scope());
  NODE_PTR scale_node = cntr->New_cust_node(get_scale_op, scale_type, spos);
  scale_node->Set_child(0, cntr->Clone_node_tree(sibling_node));

  // 4. gen level node
  OPCODE   get_level_op(CKKS_DOMAIN::ID, CKKS_OPERATOR::LEVEL);
  TYPE_PTR level_type = lower_ctx.Get_level_type(cntr->Glob_scope());
  NODE_PTR level_node = cntr->New_cust_node(get_level_op, level_type, spos);
  level_node->Set_child(0, cntr->Clone_node_tree(sibling_node));

  // 5. gen CKKS encode node
  OPCODE   ckks_encode_op(CKKS_DOMAIN::ID, CKKS_OPERATOR::ENCODE);
  TYPE_PTR plain_type  = lower_ctx.Get_plain_type(cntr->Glob_scope());
  NODE_PTR ckks_encode = cntr->New_cust_node(ckks_encode_op, plain_type, spos);
  ckks_encode->Set_child(0, new_child0);
  ckks_encode->Set_child(1, len_node);
  ckks_encode->Set_child(2, scale_node);
  ckks_encode->Set_child(3, level_node);

  // copy ATTR
  const double* mask_len = node->Attr<double>(nn::core::ATTR::MASK);
  if (mask_len != nullptr) {
    ckks_encode->Set_attr(nn::core::ATTR::MASK, mask_len, 1);
  }
  return RETV(ckks_encode);
}

template <typename RETV, typename VISITOR>
RETV SIHE2CKKS_IMPL::Handle_bin_arith_node(VISITOR* visitor, NODE_PTR node,
                                           OPCODE new_opcode, TYPE_PTR rtype) {
  CONTAINER*       cntr      = visitor->Context().Container();
  core::LOWER_CTX& lower_ctx = visitor->Context().Lower_ctx();
  CMPLR_ASSERT(node->Opcode() == sihe::OPC_ADD ||
                   node->Opcode() == sihe::OPC_SUB ||
                   node->Opcode() == sihe::OPC_MUL,
               "only support add/sub/mul");

  // 1. handle child0
  NODE_PTR child0 = node->Child(0);
  CMPLR_ASSERT(child0->Rtype_id() == lower_ctx.Get_cipher_type_id(),
               "child0 must be cipher type");
  NODE_PTR new_child0 = visitor->template Visit<RETV>(child0).Node();

  // 2. handle child1
  NODE_PTR child1 = node->Child(1);
  CMPLR_ASSERT(lower_ctx.Is_cipher_type(child1->Rtype_id()) ||
                   lower_ctx.Is_plain_type(child1->Rtype_id()) ||
                   true /*TODO: rep true with child1->Rtype()->Is_scalar() */,
               "child1 must be either cipher type, plain type, or scalar");
  air::base::OPCODE sihe_encode(sihe::SIHE_DOMAIN::ID,
                                sihe::SIHE_OPERATOR::ENCODE);
  NODE_PTR          new_child1;
  if (child1->Opcode() == sihe_encode) {
    // if new_child0 is not leaf node, store it in a tmp to simplify
    // CKKS.encode opnds.
    if (!new_child0->Is_leaf()) {
      new_child0 = Gen_tmp_for_complex_node(visitor->Context(), new_child0);
    }
    new_child1 = Handle_encode_in_bin_arith_node<RETV, VISITOR>(visitor, child1,
                                                                new_child0)
                     .Node();
  } else
    new_child1 = visitor->template Visit<RETV>(child1).Node();

  // 3. gen ckks binary arithmetic node
  NODE_PTR bin_arith_node = cntr->New_bin_arith(new_opcode, rtype, new_child0,
                                                new_child1, node->Spos());
  return RETV(bin_arith_node);
}

template <typename RETV, typename VISITOR>
RETV SIHE2CKKS_IMPL::Handle_add(VISITOR* visitor, NODE_PTR node) {
  SIHE2CKKS_CTX& ctx = visitor->Context();
  if (ctx.Python_dsl()) {
    NODE_PTR     new_ld0 = visitor->template Visit<RETV>(node->Child(0)).Node();
    NODE_PTR     new_ld1 = visitor->template Visit<RETV>(node->Child(1)).Node();
    CKKS_PY_IMPL py_gen(ctx);
    return py_gen.New_py_add(node, new_ld0, new_ld1, node->Spos());
  }
  TYPE_PTR rtype = node->Rtype();
  return Handle_bin_arith_node<RETV, VISITOR>(visitor, node, ckks::OPC_ADD,
                                              rtype);
}

template <typename RETV, typename VISITOR>
RETV SIHE2CKKS_IMPL::Handle_mul(VISITOR* visitor, NODE_PTR node) {
  SIHE2CKKS_CTX*   ctx        = &visitor->Context();
  CONTAINER*       cntr       = ctx->Container();
  core::LOWER_CTX& lower_ctx  = ctx->Lower_ctx();
  GLOB_SCOPE*      glob_scope = ctx->Glob_scope();

  TYPE_ID child0_type = node->Child(0)->Rtype_id();
  AIR_ASSERT_MSG(lower_ctx.Is_cipher_type(child0_type),
                 "encounter non-normalized SIHE.mul");
  TYPE_ID child1_type = node->Child(1)->Rtype_id();
  bool    is_mulcc    = lower_ctx.Is_cipher_type(child1_type);

  TYPE_PTR rtype = is_mulcc ? lower_ctx.Get_cipher3_type(glob_scope)
                            : lower_ctx.Get_cipher_type(glob_scope);

  NODE_PTR ckks_mul_node =
      Handle_bin_arith_node<RETV, VISITOR>(visitor, node, ckks::OPC_MUL, rtype)
          .Node();
  if (!is_mulcc) {
    // TODO: check type of child1 is plain or scalar
    return RETV(ckks_mul_node);
  }

  OPCODE   relin_op(CKKS_DOMAIN::ID, CKKS_OPERATOR::RELIN);
  NODE_PTR ckks_relin = cntr->New_una_arith(
      relin_op, lower_ctx.Get_cipher_type(cntr->Glob_scope()), ckks_mul_node,
      ckks_mul_node->Spos());
  return RETV(ckks_relin);
}

template <typename RETV, typename VISITOR>
RETV SIHE2CKKS_IMPL::Handle_sub(VISITOR* visitor, NODE_PTR node) {
  TYPE_PTR rtype = node->Rtype();
  return Handle_bin_arith_node<RETV, VISITOR>(visitor, node, ckks::OPC_SUB,
                                              rtype);
}

template <typename RETV, typename VISITOR>
RETV SIHE2CKKS_IMPL::Handle_rotate(VISITOR* visitor, NODE_PTR node) {
  CONTAINER*       cntr      = visitor->Context().Container();
  core::LOWER_CTX& lower_ctx = visitor->Context().Lower_ctx();
  // 1. handle child0
  NODE_PTR child0 = node->Child(0);
  CMPLR_ASSERT(child0->Rtype_id() == lower_ctx.Get_cipher_type_id(),
               "child0 must be cipher type");
  NODE_PTR new_child0 = visitor->template Visit<RETV>(child0).Node();

  // 2. handle child1
  NODE_PTR child1 = node->Child(1);
  CMPLR_ASSERT(child1->Rtype()->Is_signed_int(),
               "rotate index must be integer");
  NODE_PTR new_child1 = visitor->template Visit<RETV>(child1).Node();

  // 3. gen CKKS rotate node
  OPCODE   ckks_rotate_op(CKKS_DOMAIN::ID, CKKS_OPERATOR::ROTATE);
  NODE_PTR ckks_rotate =
      cntr->New_cust_node(ckks_rotate_op, new_child0->Rtype(), node->Spos());
  ckks_rotate->Set_child(0, new_child0);
  ckks_rotate->Set_child(1, new_child1);

  const char* rot_idx_key   = nn::core::ATTR::RNUM;
  uint32_t    rot_idx_count = 0;
  const int*  rot_idx       = node->Attr<int>(rot_idx_key, &rot_idx_count);
  AIR_ASSERT(rot_idx != nullptr && rot_idx_count > 0);
  ckks_rotate->Set_attr(rot_idx_key, rot_idx, rot_idx_count);
  return RETV(ckks_rotate);
}

template <typename RETV, typename VISITOR>
RETV SIHE2CKKS_IMPL::Handle_encode(VISITOR* visitor, NODE_PTR node) {
  CONTAINER*       cntr      = visitor->Context().Container();
  core::LOWER_CTX& lower_ctx = visitor->Context().Lower_ctx();
  PRIM_TYPE_PTR    u32_type =
      cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_U32);
  SPOS     spos = node->Spos();
  OPCODE   ckks_encode_op(CKKS_DOMAIN::ID, CKKS_OPERATOR::ENCODE);
  TYPE_PTR plain_type  = lower_ctx.Get_plain_type(cntr->Glob_scope());
  NODE_PTR ckks_encode = cntr->New_cust_node(ckks_encode_op, plain_type, spos);
  NODE_PTR new_child0  = cntr->Clone_node_tree(node->Child(0));
  ckks_encode->Set_child(0, new_child0);
  NODE_PTR new_child1 = cntr->Clone_node_tree(node->Child(1));
  ckks_encode->Set_child(1, new_child1);
  ckks_encode->Set_child(2, cntr->New_intconst(u32_type, 1, spos));
  ckks_encode->Set_child(3, cntr->New_intconst(u32_type, 0, spos));

  // copy ATTR
  const double* mask_len = node->Attr<double>(nn::core::ATTR::MASK);
  if (mask_len != nullptr) {
    ckks_encode->Set_attr(nn::core::ATTR::MASK, mask_len, 1);
  }
  return RETV(ckks_encode);
}

template <typename RETV, typename VISITOR>
RETV SIHE2CKKS_IMPL::Handle_bootstrap(VISITOR* visitor, NODE_PTR node) {
  // Check if Python has a registered lowering for this op
  if (sihe::Should_skip_lowering("fhe::sihe", "bootstrap")) {
    // Preserve as CKKS.bootstrap (not SIHE) so scale manager can process it
    // The Python lowering pass will replace this node later
    CONTAINER* cntr      = visitor->Context().Container();
    NODE_PTR   child     = node->Child(0);
    NODE_PTR   new_child = visitor->template Visit<RETV>(child).Node();

    // Create CKKS bootstrap node (scale manager needs CKKS domain)
    OPCODE   ckks_bootstrap_op(CKKS_DOMAIN::ID, CKKS_OPERATOR::BOOTSTRAP);
    NODE_PTR new_bootstrap = cntr->New_una_arith(
        ckks_bootstrap_op, new_child->Rtype(), new_child, node->Spos());
    new_bootstrap->Set_rtype(new_child->Rtype());
    return RETV(new_bootstrap);
  }

  SIHE2CKKS_CTX& ctx      = visitor->Context();
  CKKS_GEN&      ckks_gen = ctx.Ckks_gen();
  // 1. handle child
  NODE_PTR    child     = node->Child(0);
  NODE_PTR    new_child = visitor->template Visit<RETV>(child).Node();
  const SPOS& spos      = node->Spos();

  if (ctx.Python_dsl()) {
    CKKS_PY_IMPL py_gen(ctx);
    FUNC_SCOPE*  bts_func = py_gen.New_py_bts(node, spos);
    // Generate call to bootstrap function
    CONTAINER*  cntr       = ctx.Container();
    FUNC_SCOPE* func_scope = cntr->Parent_func_scope();
    TYPE_PTR    cipher_ty = ctx.Lower_ctx().Get_cipher_type(cntr->Glob_scope());
    PREG_PTR    retv      = func_scope->New_preg(cipher_ty);
    ENTRY_PTR   entry     = bts_func->Owning_func()->Entry_point();
    STMT_PTR    s_call    = cntr->New_call(entry, retv, 1, spos);
    s_call->Node()->Set_child(0, new_child);
    // Set MUL_DEPTH attribute required by scale manager
    const uint32_t* level_ptr =
        node->Attr<uint32_t>(fhe::core::FHE_ATTR_KIND::LEVEL);
    uint32_t mul_depth = level_ptr ? *level_ptr : 0;
    s_call->Node()->Set_attr(fhe::core::FHE_ATTR_KIND::MUL_DEPTH, &mul_depth,
                             1);
    ctx.Prepend(s_call);
    NODE_PTR result = cntr->New_ldp(retv, spos);
    return RETV(result);
  }

  NODE_PTR ckks_bootstrap = ckks_gen.Gen_bootstrap(new_child, spos);

  const char*     slot_key   = nn::core::ATTR::SLOT;
  uint32_t        slot_count = 0;
  const uint32_t* slot_idx   = node->Attr<uint32_t>(slot_key, &slot_count);
  AIR_ASSERT(slot_key != nullptr && slot_count > 0);
  ckks_bootstrap->Set_attr(slot_key, slot_idx, slot_count);

  return RETV(ckks_bootstrap);
}

template <typename RETV, typename VISITOR>
RETV SIHE2CKKS_IMPL::Handle_add_msg(VISITOR* visitor, NODE_PTR node) {
  return visitor->Context().template Handle_node<RETV, VISITOR>(visitor, node);
}

template <typename RETV, typename VISITOR>
RETV SIHE2CKKS_IMPL::Handle_mul_msg(VISITOR* visitor, NODE_PTR node) {
  return visitor->Context().template Handle_node<RETV, VISITOR>(visitor, node);
}

template <typename RETV, typename VISITOR>
RETV SIHE2CKKS_IMPL::Handle_rotate_msg(VISITOR* visitor, NODE_PTR node) {
  return visitor->Context().template Handle_node<RETV, VISITOR>(visitor, node);
}

template <typename RETV, typename VISITOR>
RETV SIHE2CKKS_IMPL::Handle_relu_msg(VISITOR* visitor, NODE_PTR node) {
  return visitor->Context().template Handle_node<RETV, VISITOR>(visitor, node);
}

template <typename RETV, typename VISITOR>
RETV SIHE2CKKS_IMPL::Handle_bootstrap_msg(VISITOR* visitor, NODE_PTR node) {
  return visitor->Context().template Handle_node<RETV, VISITOR>(visitor, node);
}

}  // namespace ckks
}  // namespace fhe

#endif
