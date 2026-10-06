//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_OPCODE_H
#define AIR_BASE_OPCODE_H

#include <cstdint>

#include "air/util/debug.h"

namespace air {

namespace base {

/**
 * @brief OPCODE, made up by Domain id and operator id
 * Total 16 bits. High 6 bits for domain id and low 10 bits for operator id
 */
class OPCODE {
public:
  /**
   * @brief Construct a new OPCODE object with default value
   *
   */
  constexpr OPCODE() : _opcode(0) {}

  /**
   * @brief Construct a new OPCODE object with raw opcode which already
   * contains domain id and operator is
   *
   * @param opc Raw OPCODE value
   */
  constexpr OPCODE(uint32_t opc) : _opcode(opc) {}

  /**
   * @brief Construct a new OPCODE object with domain id and operator id
   *
   * @param domain
   * @param opr
   */
  constexpr OPCODE(uint32_t domain, uint32_t opr)
      : _opcode((domain << 10) | opr) {
    AIR_ASSERT(domain < (2 << DOMAIN_BITS));
    AIR_ASSERT(opr < (2 << OPERATOR_BITS));
  }

  /**
   * @brief Get domain id from OPCODE
   *
   * @return uint32_t Domain id
   */
  constexpr uint32_t Domain() const { return _opcode >> OPERATOR_BITS; }

  /**
   * @brief Get operator id from OPCODE
   *
   * @return uint32_t Operator id
   */
  constexpr uint32_t Operator() const {
    return (_opcode) & ((1 << OPERATOR_BITS) - 1);
  }

  /**
   * @brief Get raw OPCODE value
   *
   * @return uint32_t
   */
  constexpr operator uint32_t() const { return _opcode; }

  /**
   * @brief Default invalid OPCODE value
   *
   */
  enum { INVALID = 0 };

private:
  // bits for domain id and operator id
  enum { DOMAIN_BITS = 6, OPERATOR_BITS = 10 };
  // raw OPCODE value
  uint32_t _opcode;
};  // OPCODE

/**
 * @brief INTRN type definition
 *
 */
enum class INTRN { INVALID = 0x00, LAST = 0xff };

}  // namespace base
}  // namespace air

#endif
