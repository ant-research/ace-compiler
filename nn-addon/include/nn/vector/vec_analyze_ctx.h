//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_VEC_ANALYZE_CTX_H
#define NN_VECTOR_VEC_ANALYZE_CTX_H

#include "air/base/transform_ctx.h"
#include "air/core/opcode.h"
#include "nn/vector/config.h"
#include "nn/vector/vector_ctx.h"
#include "nn/vector/vector_enum.h"
#include "nn/vector/vector_utils.h"

namespace nn {

namespace vector {

class VEC_ANALYZE_CTX : public air::base::ANALYZE_CTX {
public:
  VEC_ANALYZE_CTX(air::base::CONTAINER* cntr, VECTOR_CTX& ctx,
                  const air::driver::DRIVER_CTX* driver_ctx,
                  const VECTOR_CONFIG&           cfg)
      : _cntr(cntr), _driver_ctx(driver_ctx), _ctx(ctx), _config(cfg) {}

  air::base::CONTAINER* Container() const { return _cntr; }

  // declare access API for VECTOR_CTX
  DECLARE_VECTOR_CTX_ACCESS_API(_ctx)

  // declare access API for VECTOR_CONFIG
  DECLARE_VECTOR_CONFIG_ACCESS_API(_config)

  // declare trace API for detail tracing
  DECLARE_TRACE_DETAIL_API(_config, _driver_ctx)

private:
  air::base::CONTAINER*          _cntr;
  const air::driver::DRIVER_CTX* _driver_ctx;
  VECTOR_CTX&                    _ctx;
  const VECTOR_CONFIG&           _config;
};

//! @brief vec analyze handler
class VEC_ANALYZE_HANDLER : public nn::core::DEFAULT_HANDLER {
public:
  template <typename RETV, typename VISITOR>
  RETV Handle_conv(VISITOR* visitor, air::base::NODE_PTR node) {
    VEC_ANALYZE_CTX& ctx = visitor->Context();

    NODE_PTR input = node->Child(0);
    int64_t  batch = 0, channel_in = 0, input_height = 0, input_width = 0;
    Get_array_nchw(input->Rtype(), batch, channel_in, input_height,
                   input_width);
    AIR_ASSERT_MSG(batch == 1, "Conv only supports batch=1");

    NODE_PTR weight_node = node->Child(1);
    int64_t  channel_out = 0, channel_in_kernel = 0, kernel_height = 0,
            kernel_width = 0;
    Get_array_nchw(weight_node->Rtype(), channel_out, channel_in_kernel,
                   kernel_height, kernel_width);
    AIR_ASSERT_MSG(kernel_height == kernel_width,
                   "Conv only supports kernel_height = kernel_width");

    int64_t dup_num = 1;
    if (ctx.Conv_fast() && channel_out >= channel_in) {
      dup_num = channel_out / channel_in;
      if (!Is_power_of_two(channel_out)) {
        // duplicate once more
        dup_num += 1;
      }
    } else {
      dup_num = static_cast<int64_t>(ceil(1.0 * channel_out / channel_in)) + 1;
    }

    int64_t input_size  = channel_in * input_height * input_width;
    int64_t output_size = channel_out * input_height * input_width;
    int64_t slot_num    = dup_num * input_size;

    // adjust slot_num with max slot allowed value
    if (input_size == MAX_SLOT_ALLOWED) {
      slot_num = input_size;
    } else if (output_size == MAX_SLOT_ALLOWED) {
      slot_num = output_size;
    } else if (slot_num > MAX_SLOT_ALLOWED) {
      slot_num = MAX_SLOT_ALLOWED;
    }

    ctx.Update_slot(slot_num);
    return RETV();
  }
};

}  // namespace vector

}  // namespace nn

#endif  // NN_VECTOR_VEC_ANALYZE_CTX_H
