//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_MV2V_HANDLER_H
#define NN_VECTOR_MV2V_HANDLER_H

#include "air/base/transform_util.h"
#include "nn/core/attr.h"
#include "nn/vector/default_handler.h"
#include "nn/vector/tensor2vector_ctx.h"
#include "nn/vector/tensor2vector_util.h"
#include "nn/vector/vector_gen.h"
#include "nn/vector/vector_opcode.h"
#include "nn/vector/vector_utils.h"

namespace nn {
namespace vector {

class MV2V_HANDLER : public nn::vector::DEFAULT_HANDLER {
public:
  MV2V_HANDLER() {}

  template <typename RETV, typename VISITOR>
  RETV Handle_roll_sum(VISITOR* visitor, air::base::NODE_PTR node) {
    TENSOR2VECTOR_CTX& ctx    = visitor->Context();
    CONTAINER*         cntr   = ctx.Container();
    GLOB_SCOPE*        gscope = cntr->Glob_scope();
    TENSOR2VECTOR_UTIL vgen(ctx);
    ctx.Incr_num_vloop();

    NODE_PTR new_node = visitor->template Handle_node<RETV>(node);
    SPOS     spos     = new_node->Spos();

    AIR_ASSERT_MSG(new_node->Num_child() == 1,
                   "roll sum operator only have 1 child");

    NODE_PTR input = visitor->template Handle_node<RETV>(new_node->Child(0));
    // avoid modifying input directly
    NODE_PTR new_input = ctx.Store_and_load_new_preg(input);
    AIR_ASSERT_MSG(new_input->Rtype()->Is_array(), "not array type");
    std::vector<int64_t> input_dim = new_input->Rtype()->Cast_to_arr()->Shape();
    AIR_ASSERT(input_dim.size() == 4);

    NODE_PTR extra_roll_node = new_input;
    if (ctx.Selective_ss()) {
      if (Has_attr<int>(node, core::ATTR::PROLL)) {
        CONST_TYPE_PTR   s32_type = gscope->Prim_type(PRIMITIVE_TYPE::INT_S32);
        std::vector<int> roll_num = Get_attr_data<int>(node, core::ATTR::PROLL);
        extra_roll_node           = vgen.New_roll(
            new_input, cntr->New_intconst(s32_type, roll_num[0], spos),
            roll_num, spos);
      }
    }

    int64_t h = input_dim[2];
    int64_t w = input_dim[3];

    std::vector<int> kernel_shape =
        Get_attr_data<int>(node, core::ATTR::KSHAPE);
    AIR_ASSERT(kernel_shape[0] == kernel_shape[1]);

    if ((h == kernel_shape[0]) && (w == kernel_shape[1])) {
      // global average pool
      return vgen.Gen_roll_sum_body(extra_roll_node, kernel_shape);
    } else {
      // average pool
      return vgen.Gen_roll_sum_no_loop_body(extra_roll_node, kernel_shape);
    }
  }
};

}  // namespace vector
}  // namespace nn

#endif  // NN_VECTOR_MV2V_HANDLER_H
