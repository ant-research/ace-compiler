//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_UTIL_APP_RELU_INFO_H
#define FHE_UTIL_APP_RELU_INFO_H

#include <cstdint>
#include <map>
#include <memory>
#include <ostream>
#include <vector>

#include "air/util/debug.h"

namespace fhe {
namespace util {

enum class POLY_BASIS_TYPE {
  INVALID   = 0,
  POWER     = 1,
  CHEBYSHEV = 2,
  LAST      = 3,
};

//! polynomial data compose of polynomial basis type,
//! degree, and coefficients.
class POLY_DATA {
public:
  using POLY_COEFF            = std::vector<double>;
  using POLY_COEFF_ITER       = POLY_COEFF::iterator;
  using POLY_COEFF_CONST_ITER = POLY_COEFF::const_iterator;

  POLY_DATA(POLY_BASIS_TYPE basis_type, uint32_t deg, POLY_COEFF&& poly_coeff)
      : _basis_type(basis_type), _deg(deg), _coeff(poly_coeff) {}

  uint32_t Get_deg() const { return _deg; }

  void Set_deg(uint32_t deg) { _deg = deg; }

  const POLY_COEFF& Get_coeff() const { return _coeff; }

  POLY_COEFF& Get_coeff() { return _coeff; }

  POLY_BASIS_TYPE Get_basis_type() const { return _basis_type; }
  bool Is_power_basis() const { return _basis_type == POLY_BASIS_TYPE::POWER; }
  bool Is_chebyshev_basis() const {
    return _basis_type == POLY_BASIS_TYPE::CHEBYSHEV;
  }

  inline uint32_t Deg_stride() const { return 1; }

  void Print(std::ostream& out, uint32_t indent) const;

  //! minimum degree of polynomial
  inline uint32_t Get_min_deg() const { return 1; }

  //! return degree above which polynomial item need fully decompose
  //! to minimize mul_depth in BSGS.
  uint32_t Get_full_decompose_deg() const;

  //! return degree below which polynomial item need be precomputed
  //! to minimize multiplications in BSGS
  uint32_t Get_precompute_deg() const;

private:
  //! basis type of polynomial
  POLY_BASIS_TYPE _basis_type;

  //! degree of polynomial
  uint32_t _deg;

  //! coefficients of polynomial
  POLY_COEFF _coeff;
};

//! tree node of decomposed polynomial.
//! power basis polynomial is decomposed as following:
//! poly = _poly_q * x^(2^_pow) + _poly_r.
//! Chebyshev basis polynomial is decomposed as following:
//! poly = _poly_q * T_(2^_pow) + _poly_r.
class POLY_TREE_NODE {
public:
  explicit POLY_TREE_NODE(const POLY_DATA& poly_data, uint32_t actual_deg)
      : _poly_data(poly_data), _actual_deg(actual_deg) {}

  explicit POLY_TREE_NODE(POLY_DATA&& poly_data, uint32_t actual_deg)
      : _poly_data(poly_data), _actual_deg(actual_deg) {}

  const POLY_DATA& Get_poly_data() const { return _poly_data; }

  POLY_DATA& Get_poly_data() { return _poly_data; }

  void Set_actual_deg(uint32_t deg) { _actual_deg = deg; }

  uint32_t Get_actual_deg() const { return _actual_deg; }

  const POLY_TREE_NODE* Get_poly_q() const { return _poly_q.get(); }

  POLY_TREE_NODE* Get_poly_q() { return _poly_q.get(); }

  void Set_poly_q(POLY_TREE_NODE* node) { _poly_q.reset(node); }

  const POLY_TREE_NODE* Get_poly_r() const { return _poly_r.get(); }

  POLY_TREE_NODE* Get_poly_r() { return _poly_r.get(); }

  void Set_poly_r(POLY_TREE_NODE* node) { _poly_r.reset(node); }

  uint32_t Get_divisor_pow() const { return _pow; }

  void Set_divisor_pow(uint32_t val) { _pow = val; }

  //! @brief decompose polynomial of _poly_data
  //! @param full_decompose_deg above which item need fully decompose
  //! @param precompute_deg below which poly item need precompute
  void Decompose_pow_basis_poly(uint32_t full_decompose_deg,
                                uint32_t precompute_deg);

  void Decompose_chebyshev_basis_poly(uint32_t full_decompose_deg,
                                      uint32_t precompute_deg);

private:
  //! polynomial data of current polynomial.
  POLY_DATA _poly_data;

  //! actual degree of current sub poly in original poly
  uint32_t _actual_deg = 0;

  //! for polynomial with power basis, 2^_pow is power of divisor.
  //! for polynomial with Chebyshev base, 2^_pow is id of Chebyshev polynomial.
  uint32_t _pow = 0;

  //! polynomial of quotient
  std::unique_ptr<POLY_TREE_NODE> _poly_q;

  //! polynomial of remainder
  std::unique_ptr<POLY_TREE_NODE> _poly_r;
};

class POLY_INFO {
public:
  POLY_INFO(POLY_BASIS_TYPE basis_type, uint32_t depth)
      : _basis_type(basis_type), _mul_depth(depth) {}

  bool operator<(const POLY_INFO& other) const {
     return ((static_cast<uint64_t>(_basis_type) << 32U) + _mul_depth) <
            ((static_cast<uint64_t>(other._basis_type) << 32U) +
             other._mul_depth);
  }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  POLY_INFO(void);
  POLY_INFO& operator=(const POLY_INFO&);

  POLY_BASIS_TYPE _basis_type;
  uint32_t        _mul_depth;
};

//! approximate composite polynomial of sign function
class APP_SIGN_COMP_POLY {
public:
  using POLY_ARRAY         = std::vector<POLY_DATA>;
  using COMPOSITE_POLY_MAP = std::map<POLY_INFO, const POLY_ARRAY*>;

  APP_SIGN_COMP_POLY() { Register_app_sign_comp_poly(); }

  void Register_app_sign_comp_poly();

  const std::vector<POLY_TREE_NODE> Get_app_sign_comp_poly(
      POLY_INFO poly_info) {
    COMPOSITE_POLY_MAP::const_iterator iter =
        _app_sign_comp_poly.find(poly_info);
    AIR_ASSERT(iter != _app_sign_comp_poly.end());
    AIR_ASSERT(iter->second != nullptr);

    std::vector<POLY_TREE_NODE> root_node;
    for (const POLY_DATA& poly : *iter->second) {
      POLY_TREE_NODE tree_node(poly, poly.Get_deg());
      if (poly.Is_power_basis()) {
        tree_node.Decompose_pow_basis_poly(poly.Get_full_decompose_deg(),
                                           poly.Get_precompute_deg());
      } else {
        AIR_ASSERT(poly.Is_chebyshev_basis());
        tree_node.Decompose_chebyshev_basis_poly(poly.Get_full_decompose_deg(),
                                                 poly.Get_precompute_deg());
      }

      root_node.emplace_back(std::move(tree_node));
    }
    return root_node;
  }

  void Print(std::ostream& out = std::cout, uint32_t indent = 0);

private:
  //! key: mul_depth of composite polynomial.
  COMPOSITE_POLY_MAP _app_sign_comp_poly;
};

}  // namespace util
}  // namespace fhe
#endif
