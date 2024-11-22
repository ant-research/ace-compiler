//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_INCLUDE_CKKS_KEY_GENERATOR_H
#define RTLIB_INCLUDE_CKKS_KEY_GENERATOR_H

#include <stdint.h>
#include <string.h>

#include "uthash.h"
#include "util/ckks_parameters.h"
#include "util/fhe_types.h"
#include "util/public_key.h"
#include "util/secret_key.h"
#include "util/switch_key.h"

// A module to generate public and private keys for the CKKS scheme.

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief A map for precompute automorphism index
 * hashmap<_rot_idx, _precomp_auto_idx>
 */
typedef struct {
  int32_t        _rot_idx;
  uint32_t       _precomp_auto_idx;
  UT_hash_handle HH;
} PRECOMP_AUTO_IDX_MAP;

/**
 * @brief A map for precomputed automorphism order
 * hashmap<_precomp_auto_idx, _precomp_auto_order>
 */
typedef struct {
  uint32_t       _precomp_auto_idx;
  VALUE_LIST*    _precomp_auto_order;
  UT_hash_handle HH;
} PRECOMP_AUTO_ORDER_MAP;

/**
 * @brief A map for automorphism key,
 * switch key for rotate and conjugate
 * hashmap<_precomp_auto_idx, _auto_key>
 */
typedef struct {
  uint32_t       _precomp_auto_idx;
  SWITCH_KEY*    _auto_key;
  UT_hash_handle HH;
} AUTO_KEY_MAP;

/**
 * @brief An instance to generate a public/secret key pair and relinearization
 * keys. The secret key s is generated randomly, and the public key is the pair
 * (-as + e, a). The relinearization keys are generated, as specified in the
 * CKKS paper.
 * TODO: need NTT precompute automorphism order for no-ntt
 */
typedef struct {
  CKKS_PARAMETER*         _params;
  SECRET_KEY*             _secret_key;
  PUBLIC_KEY*             _public_key;
  SWITCH_KEY*             _relin_key;
  PRECOMP_AUTO_IDX_MAP*   _precomp_auto_idx_map;
  PRECOMP_AUTO_ORDER_MAP* _precomp_auto_order_map;
  AUTO_KEY_MAP*           _auto_key_map;
} CKKS_KEY_GENERATOR;

//! @brief Get secret key from CKKS_KEY_GENERATOR
static inline SECRET_KEY* Get_sk(CKKS_KEY_GENERATOR* generator) {
  return generator->_secret_key;
}

//! @brief Get public key from CKKS_KEY_GENERATOR
static inline PUBLIC_KEY* Get_pk(CKKS_KEY_GENERATOR* generator) {
  return generator->_public_key;
}

/**
 * @brief Generates secret key & public key & relin key for CKKS scheme.
 *
 * @param params Parameters including degree & scaling factor
 * @return CKKS_KEY_GENERATOR*
 */
CKKS_KEY_GENERATOR* Alloc_ckks_key_generator(CKKS_PARAMETER* params,
                                             int32_t*        rot_idx,
                                             size_t          num_rot_idx);

/**
 * @brief cleanup CKKS_KEY_GENERATOR
 *
 * @param generator
 */
void Free_ckks_key_generator(CKKS_KEY_GENERATOR* generator);

/**
 * @brief Generates a secret key for CKKS scheme.
 *
 * @param generator
 */
void Generate_secret_key(CKKS_KEY_GENERATOR* generator);

/**
 * @brief Generates a public key for CKKS scheme.
 *
 * @param generator generator include ckks parameters
 */
void Generate_public_key(CKKS_KEY_GENERATOR* generator);

/**
 * @brief Generates a switching key as described in KSGen in the CKKS paper.
 * switch key can be multi-key, roation-key, conjugation-key
 * The basic formula for switch key is:
 * switch key = (-a * old_key + P * new_key + e, a)
 * [1] multi-key: old_key = sk, new_key = sk^2
 * [2] rotation-key: two rotation key are supported:
 *     normal rotate: old_key = sk, new_key = rot(sk)
 *     fast rotate: old_key = rot(sk), new_key = sk
 *     In fast rotate, the new/old key is switched, so that in rotation
 *     evaluation, we can reverse the run steps by first run key-switch,
 *     and then run polynomial permutations, precompute(bit-decompostion
 *     and Inverse NTT) can be done ahead.
 * [3] conjugation-key: old_key = sk, new_key = conj(sk)
 *
 * @param res A switching key
 * @param generator Generator include ckks parameters
 * @param new_key New key to generate switching key
 * @param old_key Old key
 * @param is_fast Fast key switch or not
 */
void Generate_switching_key(SWITCH_KEY* res, CKKS_KEY_GENERATOR* generator,
                            POLYNOMIAL* new_key, POLYNOMIAL* old_key,
                            bool is_fast);

/**
 * @brief Generates a relinearization key for CKKS scheme.
 *
 * @param generator Generator include ckks parameters
 */
void Generate_relin_key(CKKS_KEY_GENERATOR* generator);

//! @brief Generates a conjugation key for CKKS scheme.
//! conjugate equivalence to roate with idx (2N - 1)
void Generate_conj_key(SWITCH_KEY* res, CKKS_KEY_GENERATOR* generator);

//! @brief Generates a rotation key for CKKS scheme.
//! @param auto_idx Precompute automorphism index
//! @param is_fast Fast key switch or not
void Generate_rot_key(SWITCH_KEY* res, CKKS_KEY_GENERATOR* generator,
                      uint32_t auto_idx, bool is_fast);

//! @brief Generate precompute automorphism index & order and rotation key,
//! insert them into rot maps
void Insert_rot_map(CKKS_KEY_GENERATOR* generator, int32_t rotation);

/**
 * @brief Generate the maps used for rotation operation
 * auto_idx_map <rotation_idx, auto_idx>
 * precomp_order_map <auto_idx, precomp_order_list>
 * auto_key_map <auto_idx, auto_key>
 *
 * @param generator Generator include ckks parameters
 * @param num_rot_idx Number of rotation index
 * @param rot_idxs Array of rotation index
 */
void Generate_rot_maps(CKKS_KEY_GENERATOR* generator, size_t num_rot_idx,
                       int32_t* rot_idxs);

/**
 * @brief Insert precompute automorphism index into PRECOMP_AUTO_IDX_MAP
 *
 * @param generator Generator include ckks parameters
 * @param rot_idx Rotation index
 * @param auto_idx Precompute automorphism index
 */
static inline void Insert_precomp_auto_idx(CKKS_KEY_GENERATOR* generator,
                                           int32_t rot_idx, uint32_t auto_idx) {
  PRECOMP_AUTO_IDX_MAP* map;
  HASH_FIND_INT(generator->_precomp_auto_idx_map, &rot_idx, map);
  if (map == NULL) {
    map           = (PRECOMP_AUTO_IDX_MAP*)malloc(sizeof(PRECOMP_AUTO_IDX_MAP));
    map->_rot_idx = rot_idx;
    HASH_ADD_INT(generator->_precomp_auto_idx_map, _rot_idx, map);
  }
  map->_precomp_auto_idx = auto_idx;
}

/**
 * @brief Get precompute automorphism index from PRECOMP_AUTO_IDX_MAP
 *
 * @param generator Generator include ckks parameters
 * @param rot_idx Rotation index
 * @return uint32_t Precompute automorphism index
 */
static inline uint32_t Get_precomp_auto_idx(CKKS_KEY_GENERATOR* generator,
                                            int32_t             rot_idx) {
  PRECOMP_AUTO_IDX_MAP* result;
  HASH_FIND_INT(generator->_precomp_auto_idx_map, &rot_idx, result);
  if (result) {
    return result->_precomp_auto_idx;
  } else {
    return 0;
  }
}

/**
 * @brief Clean up PRECOMP_AUTO_IDX_MAP
 *
 * @param auto_idx_map PRECOMP_AUTO_IDX_MAP*
 */
static inline void Free_precomp_auto_idx_map(
    PRECOMP_AUTO_IDX_MAP* auto_idx_map) {
  PRECOMP_AUTO_IDX_MAP* current;
  PRECOMP_AUTO_IDX_MAP* tmp;
  HASH_ITER(HH, auto_idx_map, current, tmp) {
    HASH_DEL(auto_idx_map, current);
    free(current);
  }
}

//! @brief Print PRECOMP_AUTO_IDX_MAP
static inline void Print_precomp_auto_idx_map(
    FILE* fp, PRECOMP_AUTO_IDX_MAP* auto_idx_map) {
  PRECOMP_AUTO_IDX_MAP* current;
  PRECOMP_AUTO_IDX_MAP* tmp;
  HASH_ITER(hh, auto_idx_map, current, tmp) {
    fprintf(fp, "rot_idx = %d auto_idx = %d\n", current->_rot_idx,
            current->_precomp_auto_idx);
  }
}

/**
 * @brief Insert precomputed automorphism order into PRECOMP_AUTO_ORDER_MAP
 *
 * @param generator Generator include ckks parameters
 * @param auto_idx Precompute automorphism index
 * @param auto_order Precomputed automorphism order
 */
static inline void Insert_precomp_auto_order(CKKS_KEY_GENERATOR* generator,
                                             uint32_t            auto_idx,
                                             VALUE_LIST*         auto_order) {
  PRECOMP_AUTO_ORDER_MAP* map;
  HASH_FIND_INT(generator->_precomp_auto_order_map, &auto_idx, map);
  if (map == NULL) {
    map = (PRECOMP_AUTO_ORDER_MAP*)malloc(sizeof(PRECOMP_AUTO_ORDER_MAP));
    map->_precomp_auto_idx = auto_idx;
    HASH_ADD_INT(generator->_precomp_auto_order_map, _precomp_auto_idx, map);
  }
  map->_precomp_auto_order = auto_order;
}

/**
 * @brief Get precomputed automorphism order from PRECOMP_AUTO_ORDER_MAP
 *
 * @param generator Generator include ckks parameters
 * @param auto_idx Precompute automorphism index
 * @return VALUE_LIST* Precomputed automorphism order
 */
static inline VALUE_LIST* Get_precomp_auto_order(CKKS_KEY_GENERATOR* generator,
                                                 uint32_t            auto_idx) {
  PRECOMP_AUTO_ORDER_MAP* result;
  HASH_FIND_INT(generator->_precomp_auto_order_map, &auto_idx, result);
  if (result) {
    return result->_precomp_auto_order;
  } else {
    FMT_ASSERT(false, "cannot find precomputed automorphism order");
  }
}

/**
 * @brief Clean up PRECOMP_AUTO_ORDER_MAP
 *
 * @param auto_order_map PRECOMP_AUTO_ORDER_MAP*
 */
static inline void Free_precomp_auto_order_map(
    PRECOMP_AUTO_ORDER_MAP* auto_order_map) {
  PRECOMP_AUTO_ORDER_MAP* current;
  PRECOMP_AUTO_ORDER_MAP* tmp;
  HASH_ITER(HH, auto_order_map, current, tmp) {
    if (current->_precomp_auto_order) {
      Free_value_list(current->_precomp_auto_order);
      current->_precomp_auto_order = NULL;
    }
    HASH_DEL(auto_order_map, current);
    free(current);
  }
}

/**
 * @brief Insert auto key into AUTO_KEY_MAP
 *
 * @param generator Generator include ckks parameters
 * @param auto_idx Precompute automorphism index
 * @param auto_key Switch key
 */
static inline void Insert_auto_key(CKKS_KEY_GENERATOR* generator,
                                   uint32_t auto_idx, SWITCH_KEY* auto_key) {
  AUTO_KEY_MAP* map;
  HASH_FIND_INT(generator->_auto_key_map, &auto_idx, map);
  if (map == NULL) {
    map                    = (AUTO_KEY_MAP*)malloc(sizeof(AUTO_KEY_MAP));
    map->_precomp_auto_idx = auto_idx;
    HASH_ADD_INT(generator->_auto_key_map, _precomp_auto_idx, map);
  }
  map->_auto_key = auto_key;
}

/**
 * @brief Get automorphism key into AUTO_KEY_MAP
 *
 * @param generator Generator include ckks parameters
 * @param auto_idx Precompute automorphism index
 * @return SWITCH_KEY* auto key
 */
static inline SWITCH_KEY* Get_auto_key(CKKS_KEY_GENERATOR* generator,
                                       uint32_t            auto_idx) {
  AUTO_KEY_MAP* result;
  HASH_FIND_INT(generator->_auto_key_map, &auto_idx, result);
  if (result) {
    return result->_auto_key;
  } else {
    return NULL;
  }
}

/**
 * @brief Clean up AUTO_KEY_MAP
 *
 * @param auto_key_map AUTO_KEY_MAP*
 */
static inline void Free_auto_keys(AUTO_KEY_MAP* auto_key_map) {
  AUTO_KEY_MAP* current;
  AUTO_KEY_MAP* tmp;
  HASH_ITER(HH, auto_key_map, current, tmp) {
    if (current->_auto_key) {
      Free_switch_key(current->_auto_key);
      current->_auto_key = NULL;
    }
    HASH_DEL(auto_key_map, current);
    free(current);
  }
}

//! @brief Print AUTO_KEY_MAP
static inline void Print_auto_keys(FILE* fp, AUTO_KEY_MAP* auto_key_map,
                                   bool print_swk) {
  AUTO_KEY_MAP* current;
  AUTO_KEY_MAP* tmp;
  HASH_ITER(hh, auto_key_map, current, tmp) {
    fprintf(fp, "auto_idx = %d switch_key = %p\n", current->_precomp_auto_idx,
            current->_auto_key);
    if (print_swk) Print_switch_key(fp, current->_auto_key);
  }
}

/**
 * @brief Get relinearize key from generator
 */
static inline SWITCH_KEY* Get_relin_key(CKKS_KEY_GENERATOR* generator) {
  return generator->_relin_key;
}

//! @brief Get memory size of allocated number of rotation key
static inline size_t Get_rot_key_mem_size(CKKS_KEY_GENERATOR* generator,
                                          size_t*             num_rot_key) {
  AUTO_KEY_MAP* key_map = generator->_auto_key_map;
  if (key_map != NULL) {
    size_t swk_size = Get_swk_mem_size(key_map->_auto_key);
    *num_rot_key    = HASH_COUNT(key_map);
    return *num_rot_key * swk_size;
  }
  return 0;
}

//! @brief Get total key size from generator
static inline size_t Get_total_key_size(CKKS_KEY_GENERATOR* generator) {
  size_t sk_size        = Get_sk_mem_size(Get_sk(generator));
  size_t pk_size        = Get_pk_mem_size(Get_pk(generator));
  size_t relin_key_size = Get_swk_mem_size(Get_relin_key(generator));
  size_t num_rot_key    = 0;
  size_t rot_key_size   = Get_rot_key_mem_size(generator, &num_rot_key);
  return sk_size + pk_size + relin_key_size + rot_key_size;
}

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_INCLUDE_CKKS_KEY_GENERATOR_H
