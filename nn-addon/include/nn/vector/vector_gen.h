//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_GEN_H
#define NN_VECTOR_GEN_H

#include <vector>

#include "air/base/container.h"
#include "air/base/st.h"
#include "air/driver/driver_ctx.h"
#include "nn/vector/config.h"
#include "nn/vector/vector_ctx.h"
#include "nn/vector/vector_utils.h"

namespace nn {
namespace vector {

class VECTOR_GEN {
public:
  VECTOR_GEN(air::base::CONTAINER* cntr) : _cntr(cntr) {}

  //! @brief New ADD node
  air::base::NODE_PTR New_add(air::base::NODE_PTR op0, air::base::NODE_PTR op1,
                              const air::base::SPOS& spos);

  //! @brief New STRIDED_SLICE node for pad/stride
  air::base::NODE_PTR New_strided_slice(air::base::NODE_PTR    op0,
                                        std::vector<int64_t>   start_indices,
                                        std::vector<int64_t>   slice_size,
                                        std::vector<int64_t>   stride_size,
                                        int                    channel,
                                        const air::base::SPOS& spos);

  //! @brief New MUL node
  air::base::NODE_PTR New_mul(air::base::NODE_PTR op0, air::base::NODE_PTR op1,
                              const air::base::SPOS& spos);

  //! @brief New a slice node which extracts a sub-array from the input array
  //! @param op0 is input ND array
  //! @param op1 is start_indices array
  //! @param op2 is slice size const
  air::base::NODE_PTR New_slice(air::base::NODE_PTR    op0,
                                air::base::NODE_PTR    start_indices,
                                air::base::NODE_PTR    slice_size,
                                const air::base::SPOS& spos);

  //! @brief New RESHAPE node
  air::base::NODE_PTR New_reshape(air::base::NODE_PTR    op0,
                                  std::vector<int64_t>   shape,
                                  const air::base::SPOS& spos);

  //! @brief New ROLL node
  air::base::NODE_PTR New_roll(air::base::NODE_PTR op0, air::base::NODE_PTR op1,
                               const air::base::SPOS& spos);

  //! @brief New ROLL node
  air::base::NODE_PTR New_roll(air::base::NODE_PTR op0, air::base::NODE_PTR op1,
                               std::vector<int>       attr,
                               const air::base::SPOS& spos);

protected:
  air::base::CONTAINER* _cntr;
};

air::base::GLOB_SCOPE* Vector_driver(air::base::GLOB_SCOPE*         glob,
                                     VECTOR_CTX&                    ctx,
                                     const air::driver::DRIVER_CTX* driver_ctx,
                                     const VECTOR_CONFIG&           cfg);

}  // namespace vector
}  // namespace nn

#endif  // NN_VECTOR_GEN_H
