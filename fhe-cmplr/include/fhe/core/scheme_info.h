//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CORE_SCHEME_INFO_H
#define FHE_CORE_SCHEME_INFO_H

#include <cstdint>
#include <iostream>
#include <set>

namespace fhe {
namespace core {

class CTX_PARAM {
public:
  CTX_PARAM() = default;
  ~CTX_PARAM() {}

  class PRIME_INFO {
  public:
    PRIME_INFO(uint32_t q0_bit_num, uint32_t sf_bit_num)
        : _q0_bit_num(q0_bit_num), _sf_bit_num(sf_bit_num) {}
    ~PRIME_INFO() {}
    uint32_t First_prime_bit_num() const { return _q0_bit_num; }
    uint32_t Scale_factor_bit_num() const { return _sf_bit_num; }

  private:
    // REQUIRED UNDEFINED UNWANTED methods
    PRIME_INFO(void);
    PRIME_INFO(const PRIME_INFO&);
    PRIME_INFO& operator=(const PRIME_INFO&);

    uint32_t _q0_bit_num;
    uint32_t _sf_bit_num;
  };

  // to achieve better precision, update _q_part_num and bit number of q0/sf
  // with mul_level and poly_deg.
  void Update_prime_info();
  void Set_poly_degree(uint32_t deg) {
    if (_poly_degree >= deg) return;
    _poly_degree = deg;
    Update_prime_info();
  }

  uint32_t Get_poly_degree() const { return _poly_degree; }

  /**
   * @brief calculate min poly degree for a message.
   * poly degree is double of ceil power2 of msg_len.
   *
   * @param msg_len length of message
   * @return uint32_t value of poly degree
   */
  inline static uint32_t Get_poly_degree_of_msg(uint32_t msg_len) {
    uint32_t res = 1;
    // cal slot size for msg_len
    while (res < msg_len) {
      res <<= 1;
    }
    // poly_deg = 2 * slot_size
    res <<= 1;
    return res;
  }

  void Set_security_level(uint32_t sec_level) { _security_level = sec_level; }
  uint32_t Get_security_level() const { return _security_level; }
  void     Set_mul_level(uint32_t mul_level, bool update_prime_info) {
    if (_mul_level >= mul_level) return;
    _mul_level = mul_level;
    if (update_prime_info) Update_prime_info();
  }
  uint32_t Get_mul_level() const { return _mul_level; }
  void     Set_first_prime_bit_num(uint32_t bit_num) {
    _first_prime_bit_num = bit_num;
  }
  uint32_t Get_first_prime_bit_num() const { return _first_prime_bit_num; }
  void     Set_scaling_factor_bit_num(uint32_t bit_num) {
    _scaling_factor_bit_num = bit_num;
  }
  uint32_t Get_scaling_factor_bit_num() const {
    return _scaling_factor_bit_num;
  }
  uint32_t Get_p_prime_num() const;
  void     Set_q_part_num(uint32_t num) { _q_part_num = num; }
  uint32_t Get_q_part_num() const { return _q_part_num; }
  void     Set_hamming_weight(uint32_t hw) { _hamming_weight = hw; }
  uint32_t Get_hamming_weight() const { return _hamming_weight; }
  void     Add_rotate_index(int32_t index) { _rotate_index.insert(index); }
  void     Add_rotate_index(const std::set<int32_t>& index) {
    _rotate_index.insert(index.begin(), index.end());
  }

  const std::set<int32_t>& Get_rotate_index() const { return _rotate_index; }
  void                     Print(std::ostream& out = std::cout);
  uint32_t                 Mul_depth_of_bootstrap();
  uint32_t                 Get_modulus_bit_num() const;

private:
  // REQUIRED UNDEFINED UNWANTED methods
  CTX_PARAM(const CTX_PARAM&);
  CTX_PARAM& operator=(const CTX_PARAM&);

  uint32_t          _poly_degree            = 4;
  uint32_t          _security_level         = 0;
  uint32_t          _mul_level              = 0;
  uint32_t          _first_prime_bit_num    = 33;
  uint32_t          _scaling_factor_bit_num = 30;
  uint32_t          _q_part_num             = 0;
  uint32_t          _hamming_weight         = 0;
  std::set<int32_t> _rotate_index;
};

}  // namespace core
}  // namespace fhe
#endif