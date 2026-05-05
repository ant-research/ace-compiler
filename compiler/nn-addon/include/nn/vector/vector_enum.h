//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_ENUM_H
#define NN_VECTOR_ENUM_H

namespace nn {
namespace vector {

//! @brief trace flags for VECTOR phase
enum TRACE_FLAG : uint64_t {
  TF_MISC     = 0x1,  //!< Trace misc contents
  TF_LOWER    = 0x2,  //!< Trace lower pass
  TF_SHARDING = 0x4,  //!< Trace sharding
};

}  // namespace vector
}  // namespace nn

#endif  // NN_VECTOR_ENUM_H
