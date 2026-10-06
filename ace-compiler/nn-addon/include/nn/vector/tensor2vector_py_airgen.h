//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_TENSOR2VECTOR_PY_AIRGEN_H
#define NN_VECTOR_TENSOR2VECTOR_PY_AIRGEN_H

#include <vector>

#include "air/base/container.h"
#include "air/base/st.h"
#include "nn/vector/tensor2vector_ctx.h"
#include "nn/vector/vector_gen.h"

namespace nn {
namespace vector {

class TENSOR2VECTOR_PY_IMPL : public VECTOR_GEN {
public:
  TENSOR2VECTOR_PY_IMPL(TENSOR2VECTOR_CTX& ctx)
      : VECTOR_GEN(ctx.Container()), _ctx(ctx) {}

  NODE_PTR New_py_gemm(NODE_PTR node, NODE_PTR op0, NODE_PTR op1, NODE_PTR op2,
                       const SPOS& spos);
  NODE_PTR New_py_conv(NODE_PTR node, NODE_PTR input, NODE_PTR weight,
                       NODE_PTR bias, const SPOS& spos, std::vector<int> ra,
                       int channel_in, int channel_out, int output_height,
                       int output_width, int kernel_hw, int stride);
  NODE_PTR New_py_add(NODE_PTR node, NODE_PTR op0, NODE_PTR op1,
                      const SPOS& spos);

protected:
  TENSOR2VECTOR_CTX& _ctx;
};

}  // namespace vector
}  // namespace nn

#endif  // NN_VECTOR_TENSOR2VECTOR_PY_AIRGEN_H
