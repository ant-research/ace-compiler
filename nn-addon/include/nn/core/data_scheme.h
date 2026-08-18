//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_CORE_DATA_SCHEME_H
#define NN_CORE_DATA_SCHEME_H

#include <stdint.h>

#include "air/base/node.h"

namespace nn {

namespace core {

//! @brief Data chunk kind
//!  Kind of data chunk to describe how the chunk is partitioned
enum class DATA_CHUNK_KIND {
  BLOCK,  // a block chunk
};

//! @brief Data chunk
//! Data chunk to describe a small portion of data inside a big tensor
class DATA_CHUNK {
public:
  DATA_CHUNK_KIND Kind() const { return (DATA_CHUNK_KIND)_val[0]; }
  uint32_t        Id() const { return _val[1]; }
  uint32_t        Count() const { return _val[2]; }
  uint32_t        Start() const { return _val[3]; }
  uint32_t        End() const { return _val[4]; }
  uint32_t        Stride() const { return _val[5]; }

  const uint32_t*    Data() const { return _val; }
  constexpr uint32_t Size() const { return sizeof(_val) / sizeof(_val[0]); }

  void Init(DATA_CHUNK_KIND kind, uint32_t id, uint32_t cnt, uint32_t start,
            uint32_t end, uint32_t stride) {
    _val[0] = (uint32_t)kind;
    _val[1] = id;
    _val[2] = cnt;
    _val[3] = start;
    _val[4] = end;
    _val[5] = stride;
  }

  DATA_CHUNK() {}

private:
  uint32_t _val[6];
};

//! @brief Set attribute for input data scheme
//!  Input must be a 4D tensor (batch, channel, height, width) for this API
//! @param input: input tensor, which must be an IDNAME node
//! @param n_n:   number of pieces partitioned from batch size. 1 means 1 chunk
//! for 1 batch
//! @param n_c:   number of pieces partitioned from channel size.
//! @param n_h:   number of chunks partitioned from height
//! @param n_w:   number of chunks partitioned from width
void Set_input_scheme_attr(air::base::NODE_PTR input, uint32_t n_n = 1,
                           uint32_t n_c = 1, uint32_t n_h = 1,
                           uint32_t n_w = 1);

//! @brief Set attribute for output data scheme
//!  Output must be a 2D matrix (height, width) for this API
//! @param input: input tensor, which must be an RETV node
//! @param n_h:   number of chunks partitioned from height
//! @param n_w:   number of chunks partitioned from width
void Set_output_scheme_attr(air::base::NODE_PTR output, uint32_t n_h = 1,
                            uint32_t n_w = 1);

}  // namespace core

}  // namespace nn

#endif  // NN_CORE_DATA_SCHEME_H
