//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_T2TSHARDING_ANALYSIS_HANDLER_H
#define NN_T2TSHARDING_ANALYSIS_HANDLER_H

#include "air/base/transform_util.h"
#include "air/core/default_handler.h"
#include "nn/core/default_handler.h"
#include "nn/core/null_handler.h"
#include "nn/vector/sharding.h"
#include "nn/vector/t2tsharding_analysis_ctx.h"
#include "nn/vector/tensor2vector_util.h"

namespace nn {
namespace vector {

class T2TSHARDING_ANALYSIS_HANDLER : public nn::core::NULL_HANDLER {
public:
  T2TSHARDING_ANALYSIS_HANDLER() {}

  template <typename RETV, typename VISITOR>
  RETV Handle_conv(VISITOR* visitor, air::base::NODE_PTR node) {
    T2TSHARDING_ANALYSIS_CTX& ctx  = visitor->Context();
    CONTAINER*                cntr = ctx.Container();

    // Verification
    visitor->template Visit<RETV>(node->Child(0));

    NODE_PTR orig_input = node->Child(0);
    int64_t  batch = 0, channel_in = 0, input_height = 0, input_width = 0;
    Get_array_nchw(orig_input->Rtype(), batch, channel_in, input_height,
                   input_width);
    AIR_ASSERT_MSG(batch == 1, "Conv only supports batch=1");

    NODE_PTR weight_node = node->Child(1);
    int64_t  channel_out = 0, channel_in_kernel = 0, kernel_height = 0,
            kernel_width = 0;
    Get_array_nchw(weight_node->Rtype(), channel_out, channel_in_kernel,
                   kernel_height, kernel_width);
    AIR_ASSERT_MSG(channel_in == channel_in_kernel,
                   "channel_in == channel_in_kernel");

    // Get sharding strategy: N-dimension partition
    // input  [channel_in/y, input_height/z, input_width]
    // weight [channel_out/(x=channel_out), channel_in/y, kernel_height,
    // kernel_width]
    // output [channel_out/(x=channel_out), input_height/z,
    // input_width]
    ARRAY_SHARDING*      shmap = ctx.Sharding();
    std::vector<int64_t> xyz =
        shmap->Analyze_conv(channel_out, channel_in, input_height, input_width);
    shmap->Set_node_map(node, xyz);

    // formal
    if (orig_input->Opcode() ==
        air::base::OPCODE(air::core::CORE, air::core::OPCODE::LD)) {
      ADDR_DATUM_PTR input_data = orig_input->Addr_datum();
      if (input_data->Is_formal()) {
        shmap->Set_data_map(input_data->Defining_func_scope()->Id(),
                            input_data->Name_id(), {1, xyz[1], xyz[2], 1});
      }
    }

    return RETV();
  }
};

}  // namespace vector
}  // namespace nn

#endif  // NN_VECTOR_HANDLER_H
