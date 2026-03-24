//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_ANT_INCLUDE_UTIL_CRT_H
#define RTLIB_ANT_INCLUDE_UTIL_CRT_H

#include <stdint.h>
#include <stdlib.h>

#include "util/modular.h"
#include "util/ntt.h"
#include "util/std_param.h"

// A module to split a large number into its prime factors using
// the Chinese Remainder Theorem (CRT).

#ifdef __cplusplus
extern "C" {
#endif

typedef VALUE_LIST VL_I64;             // 1D value list of <int64_t>
typedef VALUE_LIST VL_VL_I64;          // 2D value list of <int64_t>
typedef VALUE_LIST VL_VL_VL_I64;       // 3D value list of <int64_t>
typedef VALUE_LIST VL_VL_VL_VL_I64;    // 4D value list of <int64_t>
typedef VALUE_LIST VL_CRTPRIME;        // 1D value list of <CRT_PRIME *>
typedef VALUE_LIST VL_VL_CRTPRIME;     // 2D value list of <CRT_PRIME *>
typedef VALUE_LIST VL_VL_VL_CRTPRIME;  // 3D value list of <CRT_PRIME *>
typedef VALUE_LIST VL_BIGINT;          // 1D value list of <BIGINT>

typedef struct CRT_PRIME   CRT_PRIME;
typedef struct CRT_PRIMES  CRT_PRIMES;
typedef struct CRT_CONTEXT CRT_CONTEXT;

//! @brief Get the modulus from CRT_PRIME
MODULUS* Get_modulus(CRT_PRIME* prime);

//! @brief Get modulus value from crt primes
int64_t Get_modulus_val(CRT_PRIME* prime);

//! @brief Get the NTT from CRT_PRIME
NTT_CONTEXT* Get_ntt(CRT_PRIME* prime);

//! @brief CRT_PRIME memory are continus
//! adjust the pointers to get next modulus
MODULUS* Get_next_modulus(MODULUS* modulus);

//! @brief If crt_primes is Q primes
bool Is_q(CRT_PRIMES* crt_primes);

//! @brief Get value list of CRT_PRIME from CRT_PRIMES
VALUE_LIST* Get_primes(CRT_PRIMES* crt_primes);

//! @brief Get CRT_PRIME from CRT_PRIMES
//! @param idx index of CRT_PRIME in CRT_PRIMES
//! @return CRT_PRIME*
CRT_PRIME* Get_prime_at(CRT_PRIMES* crt_primes, size_t idx);

//! @brief Get head address of CRT_PRIME from CRT_PRIMES
//! @return CRT_PRIME*
CRT_PRIME* Get_prime_head(CRT_PRIMES* crt_primes);

//! @brief Get address of CRT_PRIME at given idx from CRT_PRIMES
//! RT_PRIME are put into continus memory
//! @param idx index of CRT_PRIME
//! @return CRT_PRIME*
CRT_PRIME* Get_nth_prime(CRT_PRIME* prime_head, size_t idx);

//! @brief Get next address of CRT_PRIME
//! CRT_PRIME are put into continus memory
//! @return CRT_PRIME*
CRT_PRIME* Get_next_prime(CRT_PRIME* prime_head);

//! @brief Get CRT_PRIME from VL_CRTPRIME at given idx
//! @return CRT_PRIME*
CRT_PRIME* Get_vlprime_at(VL_CRTPRIME* vl_primes, size_t idx);

//! @brief Get head address of modulus from VL_CRTPRIME
//! @return MODULUS*
MODULUS* Get_modulus_head(VL_CRTPRIME* primes);

//! @brief Get VL_CRTPRIME from CRT_PRIMES at given part_idx
//! @return VL_CRTPRIME*
VL_CRTPRIME* Get_qpart_prime_at_l1(CRT_PRIMES* crt_primes, size_t part_idx);

//! @brief Get qpart_compl VL_CRTPRIME* from CRT_PRIMES
//! @return VL_CRTPRIME*
VL_CRTPRIME* Get_qpart_compl_at(CRT_PRIMES* crt_primes, size_t idx1,
                                size_t idx2);

//! @brief Get length of primes
//! @return size_t
size_t Get_primes_cnt(CRT_PRIMES* crt_primes);

//! @brief Get number of parts from crt primes
//! @return size_t
size_t Get_num_parts(CRT_PRIMES* crt_primes);

// Access APIs for Q precomputed
//! @brief Get {((Q/q_i)^-1)%q_i} at given idx from crt primes
//! @return VL_I64*
VL_I64* Get_qhatinvmodq_at(CRT_PRIMES* crt_primes, size_t idx);

//! @brief Get precomputed values for {((Q/q_i)^-1)%q_i} at given level index
VL_I64* Get_qhatinvmodq_prec_at(CRT_PRIMES* crt_primes, size_t idx);

// Get API for P precomputed
//! @brief Get {P % q_j} from p crt primes
//! @return VL_I64*
VL_I64* Get_pmodq(CRT_PRIMES* crt_primes);

//! @brief Get {P^-1 % q_j} from p crt primes
VL_I64* Get_pinvmodq(CRT_PRIMES* crt_primes);

//! @brief Get precomputed values for {P^-1 % q_j}
VL_I64* Get_pinvmodq_prec(CRT_PRIMES* crt_primes);

//! @brief Get {{P/p_j} % q_j} from p crt primes
//! @return VL_VL_I64*
VL_VL_I64* Get_phatmodq(CRT_PRIMES* crt_primes);

//! @brief Get {{P/p_j} % q_j} at given idx from p crt primes
//! @return VL_I64*
VL_I64* Get_phatmodq_at(CRT_PRIMES* crt_primes, size_t idx);

//! @brief Get {((P/p_i)^-1) % p_i} from p crt primes
//! @return VL_I64*
VL_I64* Get_phatinvmodp(CRT_PRIMES* crt_primes);

//! @brief Get precomputed values for {((P/p_i)^-1)%p_i} from p crt primes
VL_I64* Get_phatinvmodp_prec(CRT_PRIMES* crt_primes);

//! @brief Get size of QPart precomputed
size_t Get_per_part_size(CRT_PRIMES* crt_primes);

//! @brief Get l_hat_inv_modq from QPart precomputed
//! @return VL_VL_VL_I64*
VL_VL_VL_I64* Get_qlhatinvmodq(CRT_PRIMES* crt_primes);

//! @brief Get l_hat_modp from QPart precomputed
//! @return VL_VL_VL_VL_I64*
VL_VL_VL_VL_I64* Get_qlhatmodp(CRT_PRIMES* crt_primes);

//! @brief Get _ql_div2_mod_qi at given idx from crt primes
VL_I64* Get_ql_div2_mod_qi_at(CRT_PRIMES* crt_primes, size_t idx);

//! @brief Get _ql_inv_mod_qi at given idx from crt primes
VL_I64* Get_ql_inv_mod_qi_at(CRT_PRIMES* crt_primes, size_t idx);

//! @brief Get _ql_inv_mod_qi_prec at given idx from crt primes
VL_I64* Get_ql_inv_mod_qi_prec_at(CRT_PRIMES* crt_primes, size_t idx);

//! @brief Get _ql_ql_inv_mod_ql_div_ql_mod_qi at given idx from crt primes
VL_I64* Get_ql_ql_inv_mod_ql_div_ql_mod_qi_at(CRT_PRIMES* crt_primes,
                                              size_t      idx);

//! @brief Get _ql_ql_inv_mod_ql_div_ql_mod_qi_prec at given idx from crt primes
VL_I64* Get_ql_ql_inv_mod_ql_div_ql_mod_qi_prec_at(CRT_PRIMES* crt_primes,
                                                   size_t      idx);

//! @brief Malloc memory for CRT_CONTEXT
//! @return CRT_CONTEXT*
CRT_CONTEXT* Alloc_crtcontext();

//! @brief Initialize CRT_CONTEXT with max depth of multiply
//! @param crt CRT_CONTEXT will be initialized
//! @param level security level
//! @param poly_degree polynomial degree of ring
//! @param mult_depth max depth of multiply
//! @param num_parts number of q parts
void Init_crtcontext(CRT_CONTEXT* crt, SECURITY_LEVEL level,
                     uint32_t poly_degree, size_t mult_depth, size_t num_parts);

//! @brief Initialize CRT_CONTEXT with mod size
//! @param crt CRT_CONTEXT will be initialized
//! @param level security level
//! @param poly_degree polynomial degree of ring
//! @param num_primes number of q primes
//! @param first_mod_size bit size of first mod
//! @param scaling_mod_size bit size of scaling factor
//! @param num_parts number of q parts
void Init_crtcontext_with_prime_size(CRT_CONTEXT* crt, SECURITY_LEVEL level,
                                     uint32_t poly_degree, size_t num_primes,
                                     size_t first_mod_size,
                                     size_t scaling_mod_size, size_t num_parts);

//! @brief Cleanup crtContext memory
void Free_crtcontext(CRT_CONTEXT* crt);

// Access APIs for CRT_CONTEXT
//! @brief Get crt primes for q
//! @return CRT_PRIMES*
CRT_PRIMES* Get_q(CRT_CONTEXT* crt);

//! @brief Get crt primes for p
//! @return CRT_PRIMES*
CRT_PRIMES* Get_p(CRT_CONTEXT* crt);

//! @brief Get crt primes for q part
//! @return CRT_PRIMES*
CRT_PRIMES* Get_qpart(CRT_CONTEXT* crt);

//! @brief Get crt primes for q part complement
//! @return CRT_PRIMES*
CRT_PRIMES* Get_qpart_compl(CRT_CONTEXT* crt);

//! @brief Get q modulus head from crt context
//! @return Modulus*
MODULUS* Get_q_modulus_head(CRT_CONTEXT* crt);

//! @brief Get p modulus head from crt context
//! @return Modulus*
MODULUS* Get_p_modulus_head(CRT_CONTEXT* crt);

//! @brief Get value list of crt primes for q
//! @return CRT_PRIMES*
VL_CRTPRIME* Get_q_primes(CRT_CONTEXT* crt);

//! @brief Get value list of crt primes for p
//! @return CRT_PRIMES*
VL_CRTPRIME* Get_p_primes(CRT_CONTEXT* crt);

//! @brief Get number of q prime
//! @return size_t
size_t Get_crt_num_q(CRT_CONTEXT* crt);

//! @brief Get number of p prime
//! @return size_t
size_t Get_crt_num_p(CRT_CONTEXT* crt);

//! @brief Get number of decompose part size with given number of q primes
//! @param crt CRT_CONTEXT
//! @param num_q the number of q primes
size_t Get_num_decomp(CRT_CONTEXT* crt, size_t num_q);

//! @brief Transform regular value to double-CRT representation values from
//! given primes
//! @param res values of double-CRT representation values
//! @param res_len length of double-CRT representation values
//! @param vals input original value list
//! @param q_cnt length of Q primes
//! @param extend_p extend P primes or not
//! @param without_mod within modulus or not
//! @param crt CRT_CONTEXT
void Transform_to_dcrt(int64_t* res, size_t res_len, VALUE_LIST* vals,
                       size_t q_cnt, bool extend_p, bool without_mod,
                       CRT_CONTEXT* crt);

//! @brief Reconstructs double-CRT representation values to the regular values
//! @param res regular value list
//! @param vals input double-CRT representation values
//! @param val_len length of double-CRT representation values
//! @param q_cnt length of Q primes
//! @param extend_p extend P primes or not
//! @param crt CRT_CONTEXT
//! @return A value list whose values are reconstructed.
void Reconstruct_from_dcrt(VALUE_LIST* res, int64_t* vals, size_t val_len,
                           size_t q_cnt, bool extend_p, CRT_CONTEXT* crt);

#ifdef __cplusplus
}
#endif

#endif