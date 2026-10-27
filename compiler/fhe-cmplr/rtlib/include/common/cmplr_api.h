//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_COMMON_CMPLR_API_H
#define RTLIB_COMMON_CMPLR_API_H

//! @brief cmplr_api.h
//! Define API prototypes for compiler-generated functions

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif
//! @brief Set up the environment for the compiler to initialize CKKS parameters
//! and CRT values.
//! @param degree degree of polynomial ring
//! @param sec_level security level
//! @param depth the depth of computation graph
//! @param input_lev level of input ciphertext
//! @param first_mod_size bit size of first mod
//! @param scaling_mod_size bit size of scaling factor
//! @param num_q_parts number of q part(dnum)
//! @param hamming_weight hamming weight value
void Prepare_context_for_cmplr(uint32_t degree, uint32_t sec_level,
                               uint32_t depth, uint32_t input_lev,
                               uint32_t first_mod_size,
                               uint32_t scaling_mod_size, uint32_t num_q_parts,
                               uint32_t hamming_weight);

//! @brief Finalize the context for compiler, release the allocated context
//! memory
void Finalize_context_for_cmplr();

//! @brief Retrieve the count of elements in Q primes
//! @return size_t
size_t Get_q_cnt();

//! @brief Retrieve the count of elements in P primes
//! @return size_t
size_t Get_p_cnt();

//! @brief Retrieve the count of elements in the third dimension of the $q_lhat
//! \mod p$.
//! @param q_idx Index for the first dimension, indicating the q index
//! @param part_idx Index for the second dimension, indicating the part index
//! @return size_t
size_t Get_qlhmodp_dim3_cnt(uint32_t q_idx, uint32_t part_idx);

//! @brief Retrieve the count of elements in the first dimension of the $phat
//! \mod q_i$
//! @return size_t
size_t Get_phmodq_dim1_cnt();

//! @brief Retrieve the q prime values
//! @param primes memory to store the fetched values
//! @return size of q primes
size_t Fetch_q_primes(uint64_t* primes);

//! @brief Retrieve the p prime values
//! @param primes memory to store the fetched values
//! @return size of p primes
size_t Fetch_p_primes(uint64_t* primes);

//! @brief Retrieve the $q_lhat^{-1} \mod q_i$ values based on the specified
//! indices. $q_lhat^{-1} \mod q_i$ values are 3D vector.
//  This function retrieves data from the innermost using specified indices.
//! @param vals Pointer to a location where the address of the retrieved values
//! be stored
//! @param cnt Pointer to a size_t variable where the count of values retrieved
//! be stored
//! @param part_idx Index for the first dimension, indicating the part index
//! @param part_size_idx Index for the second dimension, indicating the part
//! size index
void Fetch_qlhinvmodq_at(const uint64_t** vals, size_t* cnt, uint32_t part_idx,
                         uint32_t part_size_idx);

//! @brief Retrieve the  $q_lhat^{-1} \mod q_i$  values based on specified
//! indices. $q_lhat^{-1} \mod q_i$ values are 4D vector. This function
//! retrieves data from the innermost using specified indices.
//! @param vals Pointer to a location where the address of the retrieved values
//! be stored
//! @param cnt Pointer to a size_t variable where the count of values retrieved
//! be stored
//! @param q_idx Index for the first dimension, indicating the q index
//! @param part_idx Index for the second dimension, indicating the part index
//! @param part_size_idx Index for the third dimension, indicating the part size
//! index
void Fetch_qlhmodp_at(const uint64_t** vals, size_t* cnt, uint32_t q_idx,
                      uint32_t part_idx, uint32_t part_size_idx);

//! @brief Retrieve the  $q_l^{-1} \mod q_i$  values based on specified indices.
//! $q_l^{-1} \mod q_i$ values are 2D vector.
//! This function retrieves data from the innermost using specified indices.
//! @param vals Pointer to a location where the address of the retrieved values
//! be stored
//! @param cnt Pointer to a size_t variable where the count of values retrieved
//! be stored
//! @param idx Index for the first dimension, indicating the q index
void Fetch_qlinvmodq_at(const uint64_t** vals, size_t* cnt, uint32_t idx);

//! @brief Retrieve the  $phat \mod q_i$  values based on specified indices.
//! $phat \mod q_i$  values are 2D vector.
//! This function retrieves data from the innermost using specified indices.
//! @param vals Pointer to a location where the address of the retrieved values
//! be stored
//! @param cnt Pointer to a size_t variable where the count of values retrieved
//! be stored
//! @param idx Index for the first dimension, indicating the q index
void Fetch_phmodq_at(const uint64_t** vals, size_t* cnt, uint32_t idx);

//! @brief Retrieve the  $ql/2 \mod q_i$  values based on specified indices.
//! $ql/2 \mod q_i$ values are 2D vector.
//! This function retrieves data from the innermost using specified indices.
//! @param vals Pointer to a location where the address of the retrieved values
//! be stored
//! @param cnt Pointer to a size_t variable where the count of values retrieved
//! be stored
//! @param idx Index for the first dimension, indicating the q index
void Fetch_qlhalfmodq_at(const uint64_t** vals, size_t* cnt, uint32_t idx);

//! @brief Retrieve the $phat^{-1} \mod p_i$ values.
//! $phat^{-1} \mod p_i$ values are 1D vector.
//! @param vals Pointer to a location where the address of the retrieved values
//! be stored
//! @param cnt Pointer to a size_t variable where the count of values retrieved
//! be stored
void Fetch_phinvmodp(const uint64_t** vals, size_t* cnt);

//! @brief Retrieve the $p^{-1} \mod q_i$ values based on specified indices.
//! $p^{-1} \mod q_i$ values are 1D vector.
//! @param vals Pointer to a location where the address of the retrieved values
//! be stored
//! @param cnt Pointer to a size_t variable where the count of values retrieved
//! be stored
void Fetch_pinvmodq(const uint64_t** vals, size_t* cnt);
#ifdef __cplusplus
}
#endif

#endif