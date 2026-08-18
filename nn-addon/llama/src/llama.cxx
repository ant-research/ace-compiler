//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "nn/llama/llama.h"

using namespace air::base;

namespace nn {
namespace llama {

air::base::GLOB_SCOPE* Llama_driver(GLOB_SCOPE*                    glob,
                                    const air::driver::DRIVER_CTX* driver_ctx,
                                    const nn::llama::LLAMA_CONFIG& cfg) {
  LLAMA lla(glob);
  lla.Create_entry_func();

  ADDR_DATUM_PTR rmsnorm_output = lla.Create_rmsnorm("rmsnorm_output_0");
  if (rmsnorm_output == Null_ptr) return nullptr;

  ADDR_DATUM_PTR xq =
      lla.Create_matmul(rmsnorm_output, "weight_input_0", "matmul_output_xq_0");
  ADDR_DATUM_PTR xk =
      lla.Create_matmul(rmsnorm_output, "weight_input_1", "matmul_output_xk");
  ADDR_DATUM_PTR xv =
      lla.Create_matmul(rmsnorm_output, "matmul_weight_2", "matmul_output_xv");
  std::pair<ADDR_DATUM_PTR, ADDR_DATUM_PTR> xqk =
      lla.Create_rope_rotary(xq, xk, "weight_input_3", "rope_output_xq_xk");
  std::pair<ADDR_DATUM_PTR, ADDR_DATUM_PTR> kv =
      lla.Create_kv_cache(xqk.first, xqk.second, xv, "start_pos", "kv_output");

  ADDR_DATUM_PTR keys   = lla.Create_repeat_kv(kv.first, 6, "keys_output_0");
  ADDR_DATUM_PTR values = lla.Create_repeat_kv(kv.second, 6, "values_output_0");
  keys                  = lla.Create_transpose(keys, 2, 3, "keys_output_2");
  ADDR_DATUM_PTR attn_weights =
      lla.Create_matmul(xq, keys, "attn_weights_output_0");
  ADDR_DATUM_PTR sqrt_output =
      lla.Create_sqrt("head_dim_weight", "sqrt_output_0");

  ADDR_DATUM_PTR scores =
      lla.Create_divide(attn_weights, sqrt_output, "divide_output_0");
  ADDR_DATUM_PTR softmax_output_0 =
      lla.Create_softmax(scores, "softmax_output_0");
  ADDR_DATUM_PTR attn_output =
      lla.Create_matmul(softmax_output_0, values, "matmul_output_2");
  ADDR_DATUM_PTR attn_output_1 =
      lla.Create_transpose(attn_output, 1, 2, "attn_output_1");

  lla.Create_return(attn_output_1);
  return glob;
}

}  // namespace llama
}  // namespace nn
