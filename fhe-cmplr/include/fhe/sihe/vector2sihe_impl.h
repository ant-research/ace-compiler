//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef SIFE_CORE_SIFE_GEN_H
#define SIFE_CORE_SIFE_GEN_H

#include <cstdint>
#include <string>
#include <vector>

#include "air/base/container.h"
#include "air/base/container_decl.h"
#include "air/base/st.h"
#include "air/base/transform_util.h"
#include "air/core/opcode.h"
#include "air/util/debug.h"
#include "fhe/core/lower_ctx.h"
#include "fhe/sihe/sihe_gen.h"
#include "nn/vector/null_handler.h"
#include "nn/vector/vector_gen.h"

namespace fhe {
namespace sihe {
using namespace air::base;
using OPCODE = air::base::OPCODE;

class VECTOR2SIHE_IMPL : public nn::vector::NULL_HANDLER {
public:
  template <typename RETV, typename VISITOR>
  RETV Handle_add(VISITOR* visitor, NODE_PTR node);

  template <typename RETV, typename VISITOR>
  RETV Handle_mul(VISITOR* visitor, NODE_PTR node);

  template <typename RETV, typename VISITOR>
  RETV Handle_sub(VISITOR* visitor, NODE_PTR node);

  template <typename RETV, typename VISITOR>
  RETV Handle_roll(VISITOR* visitor, NODE_PTR node);

  template <typename RETV, typename VISITOR>
  RETV Handle_slice(VISITOR* visitor, NODE_PTR node);

  template <typename RETV, typename VISITOR>
  RETV Handle_reshape(VISITOR* visitor, NODE_PTR node);

  template <typename RETV, typename VISITOR>
  RETV Handle_add_ref(VISITOR* visitor, NODE_PTR node) {
    return Lower_cipher_cipher<RETV>(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_average_pool_rtv(VISITOR* visitor, NODE_PTR node) {
    return Lower_cipher<RETV>(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_conv_rtv(VISITOR* visitor, NODE_PTR node) {
    return Lower_cipher<RETV>(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_conv_ref(VISITOR* visitor, NODE_PTR node) {
    return Lower_cipher<RETV>(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_flatten_ref(VISITOR* visitor, NODE_PTR node) {
    return Lower_cipher<RETV>(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_gemm_rtv(VISITOR* visitor, NODE_PTR node) {
    return Lower_cipher<RETV>(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_gemm_ref(VISITOR* visitor, NODE_PTR node) {
    return Lower_cipher<RETV>(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_global_average_pool_rtv(VISITOR* visitor, NODE_PTR node) {
    return Lower_cipher<RETV>(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_global_average_pool_ref(VISITOR* visitor, NODE_PTR node) {
    return Lower_cipher<RETV>(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_max_pool_rtv(VISITOR* visitor, NODE_PTR node) {
    return Lower_cipher<RETV>(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_max_pool_ref(VISITOR* visitor, NODE_PTR node) {
    return Lower_cipher<RETV>(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_relu_rtv(VISITOR* visitor, NODE_PTR node) {
    return Lower_cipher<RETV>(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_relu_ref(VISITOR* visitor, NODE_PTR node) {
    return Lower_cipher<RETV>(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_reshape_ref(VISITOR* visitor, NODE_PTR node) {
    return Lower_cipher<RETV>(visitor, node);
  }

private:
  template <typename RETV, typename VISITOR>
  RETV Lower_cipher_cipher(VISITOR* visitor, NODE_PTR node) {
    AIR_ASSERT(node->Num_child() == 2);
    air::base::CONTAINER* cntr   = visitor->Context().Container();
    NODE_PTR              n_node = cntr->Clone_node(node);
    n_node->Set_child(0, visitor->template Visit<RETV>(node->Child(0)).Node());
    AIR_ASSERT(n_node->Child(0) != Null_ptr);
    n_node->Set_child(1, visitor->template Visit<RETV>(node->Child(1)).Node());
    AIR_ASSERT(n_node->Child(1) != Null_ptr);
    return RETV(n_node);
  }

  template <typename RETV, typename VISITOR>
  RETV Lower_cipher(VISITOR* visitor, NODE_PTR node) {
    AIR_ASSERT(node->Num_child() >= 1 && node->Num_child() <= 3);
    air::base::CONTAINER* cntr   = visitor->Context().Container();
    NODE_PTR              n_node = cntr->Clone_node(node);
    // convert first child to cipher
    n_node->Set_child(0, visitor->template Visit<RETV>(node->Child(0)).Node());
    AIR_ASSERT(n_node->Child(0) != Null_ptr);
    // clone other children
    for (uint32_t i = 1; i < node->Num_child(); ++i) {
      n_node->Set_child(i, cntr->Clone_node_tree(node->Child(i)));
    }
    return RETV(n_node);
  }

  NODE_PTR Lower_bin_arith_node(VECTOR2SIHE_CTX& ctx, NODE_PTR node,
                                OPCODE sihe_op, NODE_PTR op0, NODE_PTR op1);

  NODE_PTR Lower_rotate_node(VECTOR2SIHE_CTX& ctx, NODE_PTR node, NODE_PTR op0,
                             NODE_PTR op1);
};

template <typename RETV, typename VISITOR>
RETV VECTOR2SIHE_IMPL::Handle_add(VISITOR* visitor, NODE_PTR node) {
  VALIDATE_UTIL<NUM_CHILD::TWO> util(visitor->Context().Container(), false);
  OPCODE                        v_op(SIHE_DOMAIN::ID, SIHE_OPERATOR::ADD_MSG);
  util.template Initialize<RETV>(visitor, node, v_op);
  OPCODE   add_op(SIHE_DOMAIN::ID, SIHE_OPERATOR::ADD);
  NODE_PTR ret = Lower_bin_arith_node(visitor->Context(), node, add_op,
                                      util.Child(0), util.Child(1));
  ret          = util.Finalize(visitor, ret, -7);
  return RETV(ret);
}

template <typename RETV, typename VISITOR>
RETV VECTOR2SIHE_IMPL::Handle_mul(VISITOR* visitor, NODE_PTR node) {
  VALIDATE_UTIL<NUM_CHILD::TWO> util(visitor->Context().Container(), false);
  OPCODE                        v_op(SIHE_DOMAIN::ID, SIHE_OPERATOR::MUL_MSG);
  util.template Initialize<RETV>(visitor, node, v_op);
  OPCODE   mul_op(SIHE_DOMAIN::ID, SIHE_OPERATOR::MUL);
  NODE_PTR ret = Lower_bin_arith_node(visitor->Context(), node, mul_op,
                                      util.Child(0), util.Child(1));
  ret          = util.Finalize(visitor, ret, -7);
  return RETV(ret);
}

template <typename RETV, typename VISITOR>
RETV VECTOR2SIHE_IMPL::Handle_roll(VISITOR* visitor, NODE_PTR node) {
  VALIDATE_UTIL<NUM_CHILD::TWO> util(visitor->Context().Container(), false);
  OPCODE v_op(SIHE_DOMAIN::ID, SIHE_OPERATOR::ROTATE_MSG);
  util.template Initialize<RETV>(visitor, node, v_op);
  NODE_PTR ret =
      Lower_rotate_node(visitor->Context(), node, util.Child(0), util.Child(1));
  ret = util.Finalize(visitor, ret, -7);
  return RETV(ret);
}

template <typename RETV, typename VISITOR>
RETV VECTOR2SIHE_IMPL::Handle_slice(VISITOR* visitor, NODE_PTR node) {
  VECTOR2SIHE_CTX& ctx  = visitor->Context();
  CONTAINER*       cont = ctx.Container();
  SIHE_GEN         sihe_gen(cont, &ctx.Lower_ctx());

  NODE_PTR sliced_cst_array = cont->Clone_node_tree(node->Child(0));
  AIR_ASSERT(sliced_cst_array->Opcode() == air::core::OPC_LDC);
  NODE_PTR start_idx  = visitor->template Visit<RETV>(node->Child(1)).Node();
  NODE_PTR slice_size = visitor->template Visit<RETV>(node->Child(2)).Node();
  NODE_PTR new_slice_node = nn::vector::VECTOR_GEN(cont).New_slice(
      sliced_cst_array, start_idx, slice_size, node->Spos());

  TYPE_PTR plain_type = ctx.Lower_ctx().Get_plain_type(cont->Glob_scope());
  NODE_PTR encode_node =
      sihe_gen.Gen_encode(new_slice_node, plain_type, node->Spos());
  return RETV(encode_node);
}

template <typename RETV, typename VISITOR>
RETV VECTOR2SIHE_IMPL::Handle_reshape(VISITOR* visitor, NODE_PTR node) {
  VECTOR2SIHE_CTX& ctx    = visitor->Context();
  NODE_PTR         child0 = node->Child(0);
  AIR_ASSERT(child0->Rtype()->Cast_to_arr()->Elem_count() ==
             node->Rtype()->Cast_to_arr()->Elem_count());
  NODE_PTR new_child0 = visitor->template Visit<RETV>(child0).Node();
  AIR_ASSERT(ctx.Lower_ctx().Is_cipher_type(new_child0->Rtype_id()) ||
             ctx.Lower_ctx().Is_plain_type(new_child0->Rtype_id()));
  return RETV(new_child0);
}

}  // namespace sihe
}  // namespace fhe
#endif
