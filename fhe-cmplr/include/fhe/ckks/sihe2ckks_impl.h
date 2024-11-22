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
#include "fhe/ckks/ckks_gen.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/ckks/invalid_handler.h"
#include "fhe/sihe/core_lower_impl.h"
#include "fhe/sihe/sihe_gen.h"
#include "fhe/sihe/sihe_handler.h"
#include "fhe/sihe/sihe_opcode.h"

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
  RETV Handle_add(VISITOR* visitor, NODE_PTR add_node);
  template <typename RETV, typename VISITOR>
  RETV Handle_mul(VISITOR* visitor, NODE_PTR mul_node);
  template <typename RETV, typename VISITOR>
  RETV Handle_sub(VISITOR* visitor, NODE_PTR sub_node);
  template <typename RETV, typename VISITOR>
  RETV Handle_rotate(VISITOR* visitor, NODE_PTR rotate_node);
  template <typename RETV, typename VISITOR>
  RETV Handle_encode(VISITOR* visitor, NODE_PTR encode_node);
  template <typename RETV, typename VISITOR>
  RETV Handle_bootstrap(VISITOR* visitor, NODE_PTR bootstrap_node);

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
  RETV Handle_bin_arith_node(VISITOR* visitor, NODE_PTR bin_node,
                             OPCODE new_opcode);
  template <typename RETV, typename VISITOR>
  RETV Handle_encode_in_bin_arith_node(VISITOR* visitor, NODE_PTR encode_node,
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
                                                     NODE_PTR encode_node,
                                                     NODE_PTR sibling_node) {
  CONTAINER*       cntr      = visitor->Context().Container();
  core::LOWER_CTX& lower_ctx = visitor->Context().Lower_ctx();
  CMPLR_ASSERT(encode_node->Operator() == sihe::SIHE_OPERATOR::ENCODE &&
                   encode_node->Domain() == sihe::SIHE_DOMAIN::ID,
               "must be encode node");
  CMPLR_ASSERT(sibling_node->Rtype_id() == lower_ctx.Get_cipher_type_id(),
               "sibling node must be ciphertext");

  // 1. gen CKKS child0
  NODE_PTR child0 = encode_node->Child(0);
  // CMPLR_ASSERT(child0->Is_const_ld(),
  //              "only support encode constant array that load with ldc");
  NODE_PTR new_child0 = cntr->Clone_node_tree(child0);

  SPOS     spos     = encode_node->Spos();
  TYPE_PTR u64_type = cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_U64);

  // 2. gen msg len
  NODE_PTR child1 = encode_node->Child(1);
  AIR_ASSERT(child1->Rtype()->Is_int());
  NODE_PTR len_node = visitor->template Visit<RETV>(child1).Node();

  // 3. gen scale node
  OPCODE   get_scale_op(CKKS_DOMAIN::ID, CKKS_OPERATOR::SCALE);
  NODE_PTR scale_node = cntr->New_cust_node(get_scale_op, u64_type, spos);
  scale_node->Set_child(0, sibling_node);

  // 4. gen level node
  OPCODE   get_level_op(CKKS_DOMAIN::ID, CKKS_OPERATOR::LEVEL);
  NODE_PTR level_node = cntr->New_cust_node(get_level_op, u64_type, spos);
  level_node->Set_child(0, sibling_node);

  // 5. gen CKKS encode node
  OPCODE   ckks_encode_op(CKKS_DOMAIN::ID, CKKS_OPERATOR::ENCODE);
  TYPE_PTR plain_type  = lower_ctx.Get_plain_type(cntr->Glob_scope());
  NODE_PTR ckks_encode = cntr->New_cust_node(ckks_encode_op, plain_type, spos);
  ckks_encode->Set_child(0, new_child0);
  ckks_encode->Set_child(1, len_node);
  ckks_encode->Set_child(2, scale_node);
  ckks_encode->Set_child(3, level_node);
  return RETV(ckks_encode);
}

template <typename RETV, typename VISITOR>
RETV SIHE2CKKS_IMPL::Handle_bin_arith_node(VISITOR* visitor, NODE_PTR bin_node,
                                           OPCODE new_opcode) {
  CONTAINER*       cntr      = visitor->Context().Container();
  core::LOWER_CTX& lower_ctx = visitor->Context().Lower_ctx();
  CMPLR_ASSERT(bin_node->Operator() == sihe::SIHE_OPERATOR::ADD ||
                   bin_node->Operator() == sihe::SIHE_OPERATOR::SUB ||
                   bin_node->Operator() == sihe::SIHE_OPERATOR::MUL,
               "only support add/sub/mul");

  // 1. handle child0
  NODE_PTR child0 = bin_node->Child(0);
  CMPLR_ASSERT(child0->Rtype_id() == lower_ctx.Get_cipher_type_id(),
               "child0 must be cipher type");
  NODE_PTR new_child0 = visitor->template Visit<RETV>(child0).Node();

  // 2. handle child1
  NODE_PTR child1 = bin_node->Child(1);
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
    if (!new_child0->Is_ld()) {
      new_child0 = Gen_tmp_for_complex_node(visitor->Context(), new_child0);
    }
    new_child1 = Handle_encode_in_bin_arith_node<RETV, VISITOR>(visitor, child1,
                                                                new_child0)
                     .Node();
  } else
    new_child1 = visitor->template Visit<RETV>(child1).Node();

  // 3. gen ckks binary arithmetic node
  NODE_PTR bin_arith_node =
      cntr->New_bin_arith(new_opcode, new_child0, new_child1, bin_node->Spos());
  bin_arith_node->Set_rtype(new_child0->Rtype());
  return RETV(bin_arith_node);
}

template <typename RETV, typename VISITOR>
RETV SIHE2CKKS_IMPL::Handle_add(VISITOR* visitor, NODE_PTR add_node) {
  OPCODE add_op(CKKS_DOMAIN::ID, CKKS_OPERATOR::ADD);
  return Handle_bin_arith_node<RETV, VISITOR>(visitor, add_node, add_op);
}

template <typename RETV, typename VISITOR>
RETV SIHE2CKKS_IMPL::Handle_mul(VISITOR* visitor, NODE_PTR mul_node) {
  CONTAINER*       cntr      = visitor->Context().Container();
  core::LOWER_CTX& lower_ctx = visitor->Context().Lower_ctx();
  OPCODE           mul_op(CKKS_DOMAIN::ID, CKKS_OPERATOR::MUL);
  NODE_PTR         ckks_mul_node =
      Handle_bin_arith_node<RETV, VISITOR>(visitor, mul_node, mul_op).Node();

  NODE_PTR child1       = ckks_mul_node->Child(1);
  TYPE_ID  child1_rtype = child1->Rtype_id();
  if (!lower_ctx.Is_cipher_type(child1_rtype)) {
    // TODO: check type of child1 is plain or scalar
    return RETV(ckks_mul_node);
  }

  // result of (CIPHER * CIPHER) is a ciphertext contains 3 polynomials
  ckks_mul_node->Set_rtype(lower_ctx.Get_cipher3_type_id());

  OPCODE   relin_op(CKKS_DOMAIN::ID, CKKS_OPERATOR::RELIN);
  NODE_PTR ckks_relin =
      cntr->New_una_arith(relin_op, ckks_mul_node, ckks_mul_node->Spos());
  ckks_relin->Set_rtype(lower_ctx.Get_cipher_type_id());
  return RETV(ckks_relin);
}

template <typename RETV, typename VISITOR>
RETV SIHE2CKKS_IMPL::Handle_sub(VISITOR* visitor, NODE_PTR sub_node) {
  OPCODE sub_op(CKKS_DOMAIN::ID, CKKS_OPERATOR::SUB);
  return Handle_bin_arith_node<RETV, VISITOR>(visitor, sub_node, sub_op);
}

template <typename RETV, typename VISITOR>
RETV SIHE2CKKS_IMPL::Handle_rotate(VISITOR* visitor, NODE_PTR rotate_node) {
  CONTAINER*       cntr      = visitor->Context().Container();
  core::LOWER_CTX& lower_ctx = visitor->Context().Lower_ctx();
  // 1. handle child0
  NODE_PTR child0 = rotate_node->Child(0);
  CMPLR_ASSERT(child0->Rtype_id() == lower_ctx.Get_cipher_type_id(),
               "child0 must be cipher type");
  NODE_PTR new_child0 = visitor->template Visit<RETV>(child0).Node();

  // 2. handle child1
  NODE_PTR child1 = rotate_node->Child(1);
  CMPLR_ASSERT(child1->Rtype()->Is_signed_int(),
               "rotate index must be integer");
  NODE_PTR new_child1 = visitor->template Visit<RETV>(child1).Node();

  // 3. gen CKKS rotate node
  OPCODE   ckks_rotate_op(CKKS_DOMAIN::ID, CKKS_OPERATOR::ROTATE);
  NODE_PTR ckks_rotate = cntr->New_cust_node(
      ckks_rotate_op, new_child0->Rtype(), rotate_node->Spos());
  ckks_rotate->Set_child(0, new_child0);
  ckks_rotate->Set_child(1, new_child1);

  const char* rot_idx_key   = "nums";
  uint32_t    rot_idx_count = 0;
  const int*  rot_idx = rotate_node->Attr<int>(rot_idx_key, &rot_idx_count);
  AIR_ASSERT(rot_idx != nullptr && rot_idx_count > 0);
  ckks_rotate->Set_attr(rot_idx_key, rot_idx, rot_idx_count);
  return RETV(ckks_rotate);
}

template <typename RETV, typename VISITOR>
RETV SIHE2CKKS_IMPL::Handle_encode(VISITOR* visitor, NODE_PTR encode) {
  CONTAINER*       cntr      = visitor->Context().Container();
  core::LOWER_CTX& lower_ctx = visitor->Context().Lower_ctx();
  PRIM_TYPE_PTR    u32_type =
      cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_U32);
  SPOS     spos = encode->Spos();
  OPCODE   ckks_encode_op(CKKS_DOMAIN::ID, CKKS_OPERATOR::ENCODE);
  TYPE_PTR plain_type  = lower_ctx.Get_plain_type(cntr->Glob_scope());
  NODE_PTR ckks_encode = cntr->New_cust_node(ckks_encode_op, plain_type, spos);
  NODE_PTR new_child0  = cntr->Clone_node_tree(encode->Child(0));
  ckks_encode->Set_child(0, new_child0);
  NODE_PTR new_child1 = cntr->Clone_node_tree(encode->Child(1));
  ckks_encode->Set_child(1, new_child1);
  ckks_encode->Set_child(2, cntr->New_intconst(u32_type, 1, spos));
  ckks_encode->Set_child(3, cntr->New_intconst(u32_type, 0, spos));
  return RETV(ckks_encode);
}

template <typename RETV, typename VISITOR>
RETV SIHE2CKKS_IMPL::Handle_bootstrap(VISITOR* visitor,
                                      NODE_PTR bootstrap_node) {
  CKKS_GEN& ckks_gen = visitor->Context().Ckks_gen();
  // 1. handle child
  NODE_PTR    child          = bootstrap_node->Child(0);
  NODE_PTR    new_child      = visitor->template Visit<RETV>(child).Node();
  const SPOS& spos           = bootstrap_node->Spos();
  NODE_PTR    ckks_bootstrap = ckks_gen.Gen_bootstrap(new_child, spos);
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
