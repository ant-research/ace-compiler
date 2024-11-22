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
enum TRACE_FLAG : int {
  TF_MISC,   //!< Trace misc contents
  TF_LOWER,  //!< Trace lower pass
};

}  // namespace vector
}  // namespace nn

#endif  // NN_VECTOR_ENUM_H
