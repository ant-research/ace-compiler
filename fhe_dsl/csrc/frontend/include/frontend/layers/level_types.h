//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_LEVEL_TYPES_H
#define AIR_LEVEL_TYPES_H

#include <string>

namespace ace {
namespace frontend {

//! @brief Level types supported by the frontend
//!
//! Shared by both Path 1 (Custom Ops / direct mode) and
//! Path 2 (Interpreter / default mode).
enum class LEVEL_TYPE {
    TENSOR,  // Plain tensor operations (nn::core)
    VECTOR,  // Vector operations (nn::vector)
    CKKS,    // CKKS homomorphic operations (fhe::ckks)
    SIHE,    // SIHE homomorphic operations (fhe::sihe)
    POLY     // Polynomial operations (fhe::poly)
};

//! @brief Convert LEVEL_TYPE to string
inline std::string Level_type_to_string(LEVEL_TYPE type) {
    switch (type) {
        case LEVEL_TYPE::TENSOR:  return "tensor";
        case LEVEL_TYPE::VECTOR:  return "vector";
        case LEVEL_TYPE::CKKS:    return "ckks";
        case LEVEL_TYPE::SIHE:    return "sihe";
        case LEVEL_TYPE::POLY:    return "poly";
        default: return "unknown";
    }
}

//! @brief Parse LEVEL_TYPE from string
inline LEVEL_TYPE String_to_level_type(const std::string& str) {
    if (str == "tensor")  return LEVEL_TYPE::TENSOR;
    if (str == "vector")  return LEVEL_TYPE::VECTOR;
    if (str == "ckks")    return LEVEL_TYPE::CKKS;
    if (str == "sihe")    return LEVEL_TYPE::SIHE;
    if (str == "poly")    return LEVEL_TYPE::POLY;
    return LEVEL_TYPE::TENSOR;  // default
}

}  // namespace frontend
}  // namespace ace

#endif  // AIR_LEVEL_TYPES_H