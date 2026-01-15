//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_CORE_ATTR_H
#define NN_CORE_ATTR_H

#include <string>

namespace nn {

namespace core {

//! @brief Attribute keys for NN module
class ATTR {
public:
  // standard
  static constexpr const char* CHANNEL = "channel";
  static constexpr const char* STRIDE  = "strides";
  static constexpr const char* KSHAPE  = "kernel_shape";
  static constexpr const char* PAD     = "pads";
  static constexpr const char* GROUP   = "group";
  static constexpr const char* AXIS    = "axis";
  static constexpr const char* SHAPE   = "shape";

  // data scheme for input/output
  static constexpr const char* NUM_CHUNK   = "num_chunk";
  static constexpr const char* DATA_SCHEME = "data_scheme";

  // self defined
  static constexpr const char* PROLL = "pre_roll";  // a pre roll
  static constexpr const char* RNUM  = "nums";      // roll nummber
  static constexpr const char* KEEP_SHAPE_PAD =
      "keep_shape_pad";  // pad to keep same input and output shape
  static constexpr const char* ORIG_STRIDE = "orig_strides";  // orginal stride
  // valid data number in kernel shape, used for pool now
  static constexpr const char* VDN_IN_KS = "vdn_in_ks";
  // mask whose value is valid data len, 0 means no need masking
  static constexpr const char* MASK = "mask";
  // slot whose value is valid data len
  static constexpr const char* SLOT = "slot";

  // sharding
  static constexpr const char* SHARDING   = "sharding";
  static constexpr const char* ORIG_GROUP = "orig_group";
};

}  // namespace core

}  // namespace nn

#endif  // NN_CORE_ATTR_H
