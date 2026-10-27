
//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_INCLUDE_ANT_UTIL_INCLUDE_CRT_IMPL_H
#define RTLIB_INCLUDE_ANT_UTIL_INCLUDE_CRT_IMPL_H

#include <stdint.h>
#include <stdlib.h>

#include "ntt_impl.h"
#include "util/crt.h"
#include "util/type.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @brief Precomputed value of q part
typedef struct {
  size_t           _per_part_size;
  size_t           _num_p;
  VL_BIGINT*       _modulus;
  VL_VL_VL_I64*    _l_hat_inv_modq;  // [num_parts][part_size][part_size - l]
  VL_VL_VL_VL_I64* _l_hat_modp;  // [num_Q][l/part_size][part_size][comp_size]
} QPART_PRE_COMP;

//! @brief Precomputed value for CRT
typedef struct {
  BIG_INT    _modulus;                // BIG_INT
  VL_VL_I64* _m_mod_nb;               // For Q -> {Q % p_j}
                                      // For P -> {P % q_j}
  VL_VL_I64* _m_inv_mod_nb;           // For Q -> {Q^-1 % p_j}
                                      // For P -> {P^-1 % q_j}
  VL_VL_I64*  _m_inv_mod_nb_prec;     // Precomputed const for  _m_inv_mod_nb
  VL_BIGINT*  _hat;                   // BIG_INT, Q/q_i
  VALUE_LIST* _hat_mod_nb;            // For Q -> VL_VL_VL_I64 {(Q/q_j) % p_j}
                                      // For P -> VL_VL_I64 {(P/p_j) % q_j}
  VL_BIGINT* _hat_inv;                // BIG_INT, mod inverse of hat, (Q/q_i)^-1
  VL_VL_I64* _hat_inv_mod_self;       // For Q -> VL_VL_I64 {((Q/q_i)^-1) % q_i}
                                      // For P -> VL_I64 {((P/p_i)^-1) % p_i}
  VL_VL_I64* _hat_inv_mod_self_prec;  // Precomputed for barret mod
                                      // For Q, barret mod qi
                                      // For P, barret mod pi
  VL_BIGINT* _hat_mul_hat_inv;        // BIG_INT, _hat * _hat_inv
  VL_VL_I64* _ql_div2_mod_qi;         // {(q_l/2) % q_i}
  VL_VL_I64* _ql_inv_mod_qi;          // {q_l^-1 % q_i}
  VL_VL_I64* _ql_inv_mod_qi_prec;     // precomputed const for {q_l^-1 % q_i}
  VL_VL_I64* _ql_ql_inv_mod_ql_div_ql_mod_qi;       //{((Q/q_l)*((Q/q_l)^-1 %
                                                    // q_l))/ql) % q_i}
  VL_VL_I64* _ql_ql_inv_mod_ql_div_ql_mod_qi_prec;  // precomputed const for
                                                    // {((Q/q_l)*((Q/q_l)^-1 %
                                                    // q_l))/ql) % q_i}
} PRE_COMP;

//! @brief Type of primes
typedef enum { Q_TYPE, P_TYPE, QPART_TYPE, QPART_COMP_TYPE } PRIME_TYPE;

//! @brief Crt primes
struct CRT_PRIME {
  MODULUS     _moduli;
  NTT_CONTEXT _ntt;
};

//! @brief Crt primes, include primes & computed value
struct CRT_PRIMES {
  PRIME_TYPE   _type;
  VALUE_LIST*  _mod_val_list;  // list of MODULUS
  VL_CRTPRIME* _primes;        // lists of CRT_PRIME
                               // Q primes:         VALUE_LIST<CRT_PRIME>
                               // P primes:         VALUE_LIST<CRT_PRIME>
  // QPart primes:     VALUE_LIST<VALUE_LIST<CRT_PRIME>>
  // QPartComp primes:
  // VALUE_LIST<VALUE_LIST<VALUE_LIST<CRT_PRIME>>>
  union {
    PRE_COMP*       _precomp;
    QPART_PRE_COMP* _qpart_precomp;
  } _u;
};

// CRT_PRIME APIs

//! @brief Alloc crt primes
//! @param n length of primes
//! @return CRT_PRIME *
CRT_PRIME* Alloc_crt_primes(size_t n);

//! @brief Cleanup crt primes
void Free_crtprime(CRT_PRIME* prime);

//! @brief Print CRT_PRIME
void Print_crtprime(FILE* fp, CRT_PRIME* prime);

//! @brief Initialize crt primes
//! @param prime crt primes to be initialized
//! @param modulus prime modulus
//! @param ring_degree ring degree
void Init_crtprime(CRT_PRIME* prime, MODULUS* modulus, uint32_t ring_degree);

// PRE_COMP APIs
static inline PRE_COMP* Alloc_precompute() {
  PRE_COMP* p = (PRE_COMP*)malloc(sizeof(PRE_COMP));
  memset(p, 0, sizeof(PRE_COMP));
  BI_INIT(p->_modulus);
  return p;
}

//! @brief Cleanup precompute values
void Free_precompute(PRE_COMP* precomp);

// QPART_PRE_COMP APIs
//! @brief Alloc precomputed value of q part
static inline QPART_PRE_COMP* Alloc_qpart_precomp() {
  QPART_PRE_COMP* precomp = (QPART_PRE_COMP*)malloc(sizeof(QPART_PRE_COMP));
  memset(precomp, 0, sizeof(QPART_PRE_COMP));
  return precomp;
}

//! @brief Print precomputed value of q part
void Print_qpart_precompute(FILE* fp, QPART_PRE_COMP* precomp);

//! @brief Free precomputed value of q part
//! @param precomp
void Free_qpart_precompute(QPART_PRE_COMP* precomp);

// CRT_PRIMES APIs

//! @brief Copy value list of crtprimes from input to res: from input[0, cnt) to
//! res[begin, begin + cnt)
//! @param res return VL_CRTPRIME
//! @param input input VL_CRTPRIME
//! @param begin the starting index in the destination array where the copy
//! begins
//! @param cnt the number of elements to copy from input
void Copy_vl_crtprime(VL_CRTPRIME* res, VL_CRTPRIME* input, size_t begin,
                      size_t cnt);

//! @brief Print value list of crt primes
void Print_vl_crtprime(FILE* fp, VL_CRTPRIME* crt_primes);

//! @brief Print crt primes
void Print_crtprimes(FILE* fp, CRT_PRIMES* crt_primes);

//! @brief Cleanup crt primes
void Free_crtprimes(CRT_PRIMES* crt_primes);

// Access APIs for CRT_PRIMES
#define GET_BIG_M(crt_primes) crt_primes->_u._precomp->_modulus

// APIs for CRT_PRIMES & VL_CRTPRIME
//! @brief Precompute crt primes
//! @param for_reconstruct only precompute primes for reconstruct
void Precompute_primes(CRT_PRIMES* crt_primes, bool for_reconstruct);

//! @brief If crt_primes is P primes
static inline bool Is_p(CRT_PRIMES* crt_primes) {
  return crt_primes->_type == P_TYPE;
}

//! @brief If crt_primes is Q part primes
static inline bool Is_qpart(CRT_PRIMES* crt_primes) {
  return crt_primes->_type == QPART_TYPE;
}

//! @brief If crt_primes is Q part complement
static inline bool Is_qpart_compl(CRT_PRIMES* crt_primes) {
  return crt_primes->_type == QPART_COMP_TYPE;
}

//! @brief Set the prime with given PRIME_TYPE
static inline void Set_prime_type(CRT_PRIMES* crt_primes, PRIME_TYPE type) {
  crt_primes->_type = type;
}

//! @brief Get value list of mod val from CRT_PRIMES
static inline VALUE_LIST* Get_mod_val_list(CRT_PRIMES* crt_primes) {
  return crt_primes->_mod_val_list;
}

//! @brief Get precomp from crt primes
//! @return PRE_COMP*
PRE_COMP* Get_precomp(CRT_PRIMES* crt_primes);

// Access APIs for Q & P precomputed
//! @brief Get Q/q_i from crt primes
//! @return VL_BIGINT*
VL_BIGINT* Get_hat(CRT_PRIMES* crt_primes);

//! @brief Get mod inverse of hat, (Q/q_i)^-1
//! @return VL_BIGINT*
VL_BIGINT* Get_hat_inv(CRT_PRIMES* crt_primes);

//! @brief Get mod inverse of hat, (Q/q_i) * ((Q/q_i)^-1)
//! @return VL_BIGINT*
VL_BIGINT* Get_hat_mul_hat_inv(CRT_PRIMES* crt_primes);

//! @brief Get qpart CRT_PRIME from CRT_PRIMES at given part_idx & prime_idx
//! @return CRT_PRIME*
static inline CRT_PRIME* Get_qpart_prime_at_l2(CRT_PRIMES* crt_primes,
                                               size_t      part_idx,
                                               size_t      prime_idx) {
  IS_TRUE(Is_qpart(crt_primes), "invalid crt prime type");
  return (CRT_PRIME*)Get_ptr_value_at(
      Get_vl_value_at(Get_primes(crt_primes), part_idx), prime_idx);
}

//! @brief Initialize CRT_PRIMES
//! @param crt_primes CRT_PRIMES to be initialized
//! @param primes input primes
//! @param ring_degree ring degree
void Set_primes(CRT_PRIMES* crt_primes, VALUE_LIST* primes,
                uint32_t ring_degree);

//! @brief Get recucible level from crt primes
//! @return size_t
static inline size_t Get_reducible_levels(CRT_PRIMES* crt_primes) {
  if (Is_q(crt_primes)) {
    return Get_primes_cnt(crt_primes);
  } else if (Is_p(crt_primes)) {
    // for p primes, there is no level decrease for p
    // first dimension size is 1, the array is [0][k]
    return 1;
  } else {
    IS_TRUE(FALSE, "unexpected primes");
    return 0;
  }
}

// Get API for QPart precomputed
//! @brief Get QPart precomputed
//! @return QPART_PRE_COMP*
static inline QPART_PRE_COMP* Get_qpart_precomp(CRT_PRIMES* crt_primes) {
  IS_TRUE(Is_qpart(crt_primes), "invliad type");
  return crt_primes->_u._qpart_precomp;
}

//! @brief Get modulus of QPart precomputed
//! @return VL_BIGINT*
static inline VL_BIGINT* Get_qpart_modulus(CRT_PRIMES* crt_primes) {
  IS_TRUE(Is_qpart(crt_primes), "invliad type");
  return crt_primes->_u._qpart_precomp->_modulus;
}

// APIs for CRT_CONTEXT

//! @brief An instance of Chinese Remainder Theorem parameters
//! We split a large number into its prime factors
struct CRT_CONTEXT {
  CRT_PRIMES _q;
  CRT_PRIMES _p;
  CRT_PRIMES _qpart;
  CRT_PRIMES _qpart_compl;
};

//! @brief Precompute with new base
//! @param base
//! @param new_base
void Precompute_new_base(CRT_PRIMES* base, CRT_PRIMES* new_base);

//! @brief Transform value to CRT representation (P * Q)
//! @param crt_ret CRT representation from value
//! @param crt CRT representation
//! @param value value to be transformed to CRT representation
void Transform_to_qpcrt(VALUE_LIST* crt_ret, CRT_CONTEXT* crt, BIG_INT value);

//! @brief Print crt context
void Print_crt(FILE* fp, CRT_CONTEXT* crt);

#ifdef __cplusplus
}
#endif

#endif
