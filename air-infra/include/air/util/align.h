//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_ALIGN_H
#define AIR_UTIL_ALIGN_H

#include <assert.h>
#include <stdint.h>

#include <type_traits>

#include "air/util/debug.h"

namespace air {

namespace util {

/**
 * @brief Align value of t with given align size
 *
 * @tparam T Type of input data to be aligned
 * @param t Input data to be aligned
 * @param align Size to align to
 * @return T Aligned value
 */
template <typename T>
static inline T Align(T t, int align) {
  // make sure align is 2^n
  AIR_ASSERT(((align - 1) & align) == 0);
  if constexpr (std::is_pointer<T>::value) {
    // for pointer, convert type to uintptr_t at first
    return (T)(((uintptr_t)t + align - 1) & (~(align - 1)));
  } else {
    return (t + align - 1) & (~(align - 1));
  }
}  // Align

/**
 * @brief Check if a value t is aligned to given align size
 *
 * @tparam T Type of input data to be checked
 * @param t Input data to be checked
 * @param align Size to align
 * @return true If t is already aligned to align
 * @return false If t is not aligned to align
 */
template <typename T>
static inline bool Is_aligned(T t, int align) {
  // make sure align is 2^n
  AIR_ASSERT(((align - 1) & align) == 0);
  if constexpr (std::is_pointer<T>::value) {
    // for pointer, convert type to uintptr_t at first
    return ((uintptr_t)t & (align - 1)) == 0;
  } else {
    return (t & (align - 1)) == 0;
  }
}  // Is_aligned

}  // namespace util

}  // namespace air

#endif  // AIR_UTIL_ALIGN_H
