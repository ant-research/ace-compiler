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
  uint32_t        Pad() const { return _val[4]; }
  uint32_t        Stride() const { return _val[5]; }

  const uint32_t*    Data() const { return _val; }
  constexpr uint32_t Size() const { return sizeof(_val) / sizeof(_val[0]); }

  void Init(DATA_CHUNK_KIND kind, uint32_t id, uint32_t cnt, uint32_t start,
            uint32_t pad, uint32_t stride) {
    _val[0] = (uint32_t)kind;
    _val[1] = id;
    _val[2] = cnt;
    _val[3] = start;
    _val[4] = pad;
    _val[5] = stride;
  }

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

  DATA_CHUNK() {}

private:
  uint32_t _val[6];
};

//! @brief Set attribute for input/ouput data scheme
//! @param node: new input/output parameter, which must be an IDNAME/RETV node
//! with
//!               vector of vector type
//! @param type:  original tensor type, must be a 4D(NCHW) or 2D(HW) tensor
//! @param n_n:   number of pieces partitioned from batch size. 1 means 1 chunk
//! for 1 batch
//! @param n_c:   number of pieces partitioned from channel size.
//! @param n_h:   number of chunks partitioned from height
//! @param n_w:   number of chunks partitioned from width
void Set_scheme_attr(air::base::NODE_PTR node, air::base::TYPE_PTR type,
                     uint32_t n_n = 1, uint32_t n_c = 1, uint32_t n_h = 1,
                     uint32_t n_w = 1, uint32_t o_h = 0, uint32_t o_w = 0);

//! @brief Get number of data chunk from attr on input parameter or retv
//!        statement
//! @param node:      input or retv, which must be an IDNAME or RETV node
//! @return uint32_t: number of data chunk. 0 means no attr
uint32_t Num_data_chunk(air::base::NODE_PTR node);

//! @brief Get data scheme from attr on input parameter or retv statement
//! @param node:         input or retv, which must be an IDNAME or RETV node
//! @param count:        for output to indicate how many DATA_CHUNK available
//! @return DATA_CHUNK*: pointer to the first DATA_CHUNK. nullptr if the input
//!                      or retv is not partitioned
const DATA_CHUNK* Data_scheme_attr(air::base::NODE_PTR node, uint32_t* count);

//! @brief Get data shape from attr on input parameter or retv statement
//! @param node:      input or retv, which must be an IDNAME or RETV node
//! @param dim:       for output to indicate how many dimension in the shape
//! @return int64_t*: number of element for each dimension
const int64_t* Data_shape_attr(air::base::NODE_PTR node, uint32_t* dim);

}  // namespace core

}  // namespace nn

#endif  // NN_CORE_DATA_SCHEME_H
