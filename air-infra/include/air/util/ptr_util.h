//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_PTR_UTIL_H
#define AIR_UTIL_PTR_UTIL_H

#include <stdint.h>

#include "air/util/align.h"

namespace air {

namespace util {

/**
 * @brief A tagged pointer have the same size of a pointer.
 * In compiler, we make sure all internal objects allocated are aligned at least
 * pointer size. In this case, the lower 2-bit (32-bit system) or 3-bit (64-bit
 * system) are always 0. So we can use lower 2 or 3 bits to save extra "tag".
 *
 */
class TAGGED_POINTER {
public:
  /**
   * @brief Construct a new tagged pointer object with default value 0
   *
   */
  TAGGED_POINTER() : _tagged_pointer(0) {}

  /**
   * @brief Construct a new tagged pointer object with given pointer and tag
   *
   * @tparam OBJ Type of the pointee
   * @tparam TAG Type of the tag to be saved
   * @param ptr Raw pointer to be packed
   * @param tag Tag to be packed
   */
  template <typename OBJ, typename TAG>
  TAGGED_POINTER(OBJ* ptr, TAG tag)
      : _tagged_pointer((uintptr_t)ptr | (uintptr_t)tag) {
    AIR_STATIC_ASSERT(sizeof(OBJ) >= sizeof(uintptr_t));
    AIR_ASSERT(((uintptr_t)ptr & TAG_MASK) == 0);
    AIR_ASSERT(((uintptr_t)tag & PTR_MASK) == 0);
  }

  /**
   * @brief Get the tag packed in the tagged pointer
   *
   * @tparam TAG Type of the tag
   * @return TAG Tag value packed in the tagged pointer
   */
  template <typename TAG>
  TAG Tag() const {
    return (TAG)(_tagged_pointer & TAG_MASK);
  }

  /**
   * @brief Get the raw pointer packed in the tagged pointer
   *
   * @tparam OBJ Type of the raw pointer
   * @return OBJ* Pointer packed in the tagged pointer
   */
  template <typename OBJ>
  OBJ* Ptr() const {
    return (OBJ*)(_tagged_pointer & PTR_MASK);
  }

  /**
   * @brief Type conversion operator to raw pointer type
   *
   * @tparam OBJ Type of the raw pointer
   * @return OBJ Pointer packed in the tagged pointer
   */
  template <typename OBJ>
  operator OBJ() const {
    AIR_STATIC_ASSERT(std::is_pointer<OBJ>::value);
    return (OBJ)(_tagged_pointer & PTR_MASK);
  }

  /**
   * @brief Type conversion operator to uintptr_t type
   *
   * @return uintptr_t Value contains both raw pointer and tag
   */
  operator uintptr_t() const { return _tagged_pointer; }

  /**
   * @brief Type conversion operator to bool type
   *
   * @return true If contains a valid raw pointer
   * @return false If not contain a valid raw pointer
   */
  operator bool() const { return (_tagged_pointer & PTR_MASK) != 0; }

  TAGGED_POINTER* operator&() const = delete;

private:
  // Mask for tag, which is the lowest 2 or 3 bits
  static constexpr uintptr_t TAG_MASK = sizeof(void*) - 1;
  // Mask for raw pointer, which is the highest 30 or 61 bits
  static constexpr uintptr_t PTR_MASK = ~TAG_MASK;

  // Value contains both pointer and tag
  uintptr_t _tagged_pointer;
};  // class TAGGED_POINTER

}  // namespace util

}  // namespace air

#endif  // AIR_UTIL_PTR_UTIL_H
