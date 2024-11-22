//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_INCLUDE_KEY_GEN_H
#define RTLIB_INCLUDE_KEY_GEN_H

#include "poly/poly_eval.h"
#include "util/ckks_key_generator.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef SWITCH_KEY* SW_KEY;
typedef PUBLIC_KEY* PUB_KEY;

/**
 * @brief Get precomputed automorphism index for given rotation index
 *
 * @param rot_idx rotation index
 * @return precomputed automorphism index
 */
static inline uint32_t Auto_idx(int32_t rot_idx) {
  uint32_t auto_idx =
      Get_precomp_auto_idx((CKKS_KEY_GENERATOR*)Get_key_gen(Context), rot_idx);
  FMT_ASSERT(auto_idx, "cannot get precompute automorphism index");
  return auto_idx;
}

/**
 * @brief Get precomputed automorphism order for given rotation index
 *
 * @param rot_idx rotation index
 * @return precomputed order for automorphism
 */
static inline int64_t* Auto_order(int32_t rot_idx) {
  uint32_t    auto_idx = Auto_idx(rot_idx);
  VALUE_LIST* precomp  = Get_precomp_auto_order(
      (CKKS_KEY_GENERATOR*)Get_key_gen(Context), auto_idx);
  return Get_i64_values(precomp);
}

/**
 * @brief Get switch key for rotation key or relin key
 */
static inline SW_KEY Swk(bool is_rot, int32_t rot_idx) {
  CKKS_KEY_GENERATOR* generator = (CKKS_KEY_GENERATOR*)Get_key_gen(Context);
  if (is_rot) {
    uint32_t auto_idx = Auto_idx(rot_idx);
    SW_KEY   rot_key  = Get_auto_key(generator, auto_idx);
    FMT_ASSERT(rot_key, "cannot find auto key");
    return rot_key;
  } else {
    return Get_relin_key(generator);
  }
}

/**
 * @brief Get p0(POLY) of public key from given switch key & index
 */
static inline POLY Pk0_at(SW_KEY swk, uint32_t idx) {
  return Get_pk0(Get_swk_at(swk, idx));
}

/**
 * @brief Get p1(POLY) of public key from given switch key & index
 */
static inline POLY Pk1_at(SW_KEY swk, uint32_t idx) {
  return Get_pk1(Get_swk_at(swk, idx));
}

/**
 * @brief Get level from given public key
 *
 * @param pk public key
 * @return size_t
 */
static inline size_t Get_level_from_pk(PUB_KEY pk) {
  return Get_pubkey_prime_cnt(pk);
}

/**
 * @brief Set level for public key
 *
 * @param pk public key
 * @param level given level
 */
static inline void Set_level_for_pk(PUB_KEY pk, size_t level) {
  Set_poly_level(Get_pk0(pk), level);
  Set_poly_level(Get_pk1(pk), level);
}

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_INCLUDE_KEY_GEN_H
