//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_T2MV_HANDLER_H
#define NN_VECTOR_T2MV_HANDLER_H

#include "air/base/transform_util.h"
#include "nn/core/attr.h"
#include "nn/core/default_handler.h"
#include "nn/vector/tensor2vector_ctx.h"
#include "nn/vector/vector_gen.h"
#include "nn/vector/vector_opcode.h"
#include "nn/vector/vector_utils.h"

namespace nn {
namespace vector {

class T2MV_HANDLER : public nn::core::DEFAULT_HANDLER {
public:
  T2MV_HANDLER() {}

  template <typename RETV, typename VISITOR>
  RETV Handle_max_pool(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_average_pool<RETV, VISITOR>(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_average_pool(VISITOR* visitor, air::base::NODE_PTR node) {
    // decompose NN.average_pool to VECTOR.roll_sum and VECTOR.multiply
    // when has PROLL attribute, attach PROLL attribute to roll_sum op.
    TENSOR2VECTOR_CTX& ctx  = visitor->Context();
    CONTAINER*         cntr = ctx.Container();
    VECTOR_GEN         vgen(cntr);
    GLOB_SCOPE*        gscope = cntr->Glob_scope();
    SPOS               spos   = node->Spos();

    NODE_PTR new_node = visitor->template Handle_node<RETV>(node).Node();
    TYPE_PTR elem_type =
        new_node->Child(0)->Rtype()->Cast_to_arr()->Elem_type();

    NODE_PTR         new_input = new_node->Child(0);
    std::vector<int> kernel_shape =
        Get_attr_data<int>(node, core::ATTR::KSHAPE);

    // generate roll_sum
    NODE_PTR roll_sum_node = vgen.New_roll_sum(new_input, spos);
    roll_sum_node->Set_attr(core::ATTR::KSHAPE, kernel_shape.data(),
                            kernel_shape.size());

    int vdn_in_ks = kernel_shape[0] * kernel_shape[1];
    if (ctx.Selective_ss()) {
      if (Has_attr<int>(node, core::ATTR::PROLL)) {
        std::vector<int> roll_num = Get_attr_data<int>(node, core::ATTR::PROLL);
        std::vector<int> strides = Get_attr_data<int>(node, core::ATTR::STRIDE);

        int64_t stride = strides[0];
        int64_t ks     = kernel_shape[0];
        AIR_ASSERT_MSG(stride == 4,
                       "enable new selective_ss option, pre_roll attr has "
                       "value, stride should equal 4");
        AIR_ASSERT_MSG(ks == 4,
                       "enable new selective_ss option, pre_roll attr has "
                       "value, ks should equal 4");
        roll_sum_node->Set_attr(core::ATTR::PROLL, roll_num.data(),
                                roll_num.size());
      }
      if (Has_attr<int>(node, core::ATTR::VDN_IN_KS)) {
        std::vector<int> real_vdn_in_ks =
            Get_attr_data<int>(node, core::ATTR::VDN_IN_KS);
        AIR_ASSERT_MSG(real_vdn_in_ks.size() == 1,
                       "vdn_in_ks only contains one value");
        vdn_in_ks = real_vdn_in_ks[0];
      }
    }

    // generate average const
    CONSTANT_PTR average_const = gscope->New_const(
        CONSTANT_KIND::FLOAT, elem_type, (long double)1.0 / vdn_in_ks);
    NODE_PTR average_node = cntr->New_ldc(average_const, spos);

    NODE_PTR vmulc_node = vgen.New_mul(roll_sum_node, average_node, spos);
    return RETV(vmulc_node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_global_average_pool(VISITOR* visitor, air::base::NODE_PTR node) {
    // decompose NN.global_avgerage_pool to VECTOR.roll_sum and VECTOR.multiply
    TENSOR2VECTOR_CTX& ctx    = visitor->Context();
    CONTAINER*         cntr   = ctx.Container();
    GLOB_SCOPE*        gscope = cntr->Glob_scope();
    SPOS               spos   = node->Spos();

    NODE_PTR new_node  = visitor->template Handle_node<RETV>(node).Node();
    NODE_PTR new_input = new_node->Child(0);
    TYPE_PTR elem_type = new_input->Rtype()->Cast_to_arr()->Elem_type();
    std::vector<int64_t> input_dim = new_input->Rtype()->Cast_to_arr()->Shape();
    AIR_ASSERT(input_dim.size() == 4);
    int64_t h = input_dim[2];
    int64_t w = input_dim[3];
    // assume the kernel shape of global average pool is h,w
    std::vector<int> kernel_shape = {(int)h, (int)w};
    int64_t          vdn_in_ks    = h * w;

    if (ctx.Selective_ss()) {
      if (Has_attr<int>(node, core::ATTR::VDN_IN_KS)) {
        std::vector<int> real_vdn_in_ks =
            Get_attr_data<int>(node, core::ATTR::VDN_IN_KS);
        AIR_ASSERT_MSG(real_vdn_in_ks.size() == 1,
                       "vdn_in_ks only contains one value");
        vdn_in_ks = real_vdn_in_ks[0];
      }
    }

    // generate roll_sum
    VECTOR_GEN vgen(cntr);
    NODE_PTR   roll_sum_node = vgen.New_roll_sum(new_input, spos);
    roll_sum_node->Set_attr(core::ATTR::KSHAPE, kernel_shape.data(),
                            kernel_shape.size());

    // generate average const
    CONSTANT_PTR average_const = gscope->New_const(
        CONSTANT_KIND::FLOAT, elem_type, (long double)1.0 / vdn_in_ks);
    NODE_PTR average_node = cntr->New_ldc(average_const, spos);

    NODE_PTR vmulc_node = vgen.New_mul(roll_sum_node, average_node, spos);
    return RETV(vmulc_node);
  }
};

}  // namespace vector
}  // namespace nn

#endif  // NN_VECTOR_T2MV_HANDLER_H
