//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_INCLUDE_KEY_GEN_H
#define RTLIB_INCLUDE_KEY_GEN_H

#include "ckks/key_gen.h"
#include "poly/rns_poly.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef SWITCH_KEY* SW_KEY;
typedef PUBLIC_KEY* PUB_KEY;

//! @brief Get precomputed automorphism index for given rotation index
//! @param rot_idx rotation index
//! @return precomputed automorphism index
static inline uint32_t Auto_idx(int32_t rot_idx) {
  uint32_t auto_idx =
      Get_precomp_auto_idx((CKKS_KEY_GENERATOR*)Keygen(), rot_idx);
  FMT_ASSERT(auto_idx, "cannot get precompute automorphism index");
  return auto_idx;
}

//! @brief Get precomputed automorphism order for given rotation index
//! @param rot_idx rotation index
//! @return precomputed order for automorphism
static inline int64_t* Auto_order(int32_t rot_idx) {
  uint32_t    auto_idx = Auto_idx(rot_idx);
  VALUE_LIST* precomp =
      Get_precomp_auto_order((CKKS_KEY_GENERATOR*)Keygen(), auto_idx);
  IS_TRUE(precomp, "cannot find precomputed automorphism order");
  return Get_i64_values(precomp);
}

//! @brief Get switch key for rotation key or relin key
//! @param is_rot If true, get a rotation key, otherwise, get relin key
//! @param rot_idx rotation idx, only used if is_rot is true, if is_rot is
//! false, this parameter could be any value
static inline SW_KEY Swk(bool is_rot, int32_t rot_idx) {
  CKKS_KEY_GENERATOR* generator = (CKKS_KEY_GENERATOR*)Keygen();
  if (is_rot) {
    uint32_t conj_idx = 2 * Degree() - 1;
    uint32_t auto_idx =
        (rot_idx == (int32_t)conj_idx) ? conj_idx : Auto_idx(rot_idx);
    SW_KEY rot_key = Get_auto_key(generator, auto_idx);
    FMT_ASSERT(rot_key, "cannot find auto key");
    return rot_key;
  } else {
    if (rot_idx != 0) {
      DEV_WARN("WARN: rot_idx %d is ignored when is_rot is false.\n", rot_idx);
    }
    return Get_relin_key(generator);
  }
}

//! @brief Get p0(POLY) of public key from given switch key & index
//! Outdated - TOBE deleted
static inline POLY Pk0_at(SW_KEY swk, uint32_t idx) {
  return Get_pk0(Get_swk_at(swk, idx));
}

//! @brief Get p1(POLY) of public key from given switch key & index
//! Outdated - TOBE deleted
static inline POLY Pk1_at(SW_KEY swk, uint32_t idx) {
  return Get_pk1(Get_swk_at(swk, idx));
}

//! @brief Set pk by switch key c0 at given decompose index
static inline void Set_pk0(POLY pk, SW_KEY swk, uint32_t idx) {
  POLY pk_c0 = Get_pk0(Get_swk_at(swk, idx));
  *pk        = *pk_c0;
}

//! @brief Set pk by switch key c1 at given decompose index
static inline void Set_pk1(POLY pk, SW_KEY swk, uint32_t idx) {
  POLY pk_c1 = Get_pk1(Get_swk_at(swk, idx));
  *pk        = *pk_c1;
}

//! @brief Returns POLYNOMIAL array of switch key c0
//! @param res POLYNOMIAL array head address that holds the switch key c0
//! @param size res array size
//! @param is_rot identify a rotation key or multiply key
//! @param rot_idx for rotation key, speicify the rotation index
//! @return POLYNOMIAL array
static inline POLY Swk_c0(POLY res, size_t size, bool is_rot, int32_t rot_idx) {
  SW_KEY swk = Swk(is_rot, rot_idx);
  FMT_ASSERT(size <= Get_swk_size(swk), "extra size provided");
  for (size_t idx = 0; idx < size; idx++) {
    // shallow copy
    res[idx] = *(Get_pk0(Get_swk_at(swk, idx)));
  }
  return res;
}

//! @brief Returns POLYNOMIAL array of switch key c1
//! @param res POLYNOMIAL array that holds the switch key c1
//! @param size res array size
//! @param is_rot identify a rotation key or multiply key
//! @param rot_idx for rotation key, speicify the rotation index
//! @return POLYNOMIAL array
static inline POLY Swk_c1(POLY res, size_t size, bool is_rot, int32_t rot_idx) {
  SW_KEY swk = Swk(is_rot, rot_idx);
  FMT_ASSERT(size <= Get_swk_size(swk), "extra size provided");
  for (size_t idx = 0; idx < size; idx++) {
    // shallow copy
    res[idx] = *(Get_pk1(Get_swk_at(swk, idx)));
  }
  return res;
}

//! @brief Get level from given public key
//! @param pk public key
//! @return size_t
static inline size_t Get_level_from_pk(PUB_KEY pk) {
  return Get_pubkey_prime_cnt(pk);
}

//! @brief Set level for public key
//! @param pk public key
//! @param level given level
static inline void Set_level_for_pk(PUB_KEY pk, size_t level) {
  Set_poly_level(Get_pk0(pk), level);
  Set_poly_level(Get_pk1(pk), level);
}

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_INCLUDE_KEY_GEN_H
