//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_INCLUDE_CRT_H
#define RTLIB_INCLUDE_CRT_H

#include <stdint.h>
#include <stdlib.h>

#include "util/fhe_std_parms.h"
#include "util/fhe_utils.h"
#include "util/ntt.h"

// A module to split a large number into its prime factors using
// the Chinese Remainder Theorem (CRT).

#ifdef __cplusplus
extern "C" {
#endif

typedef VALUE_LIST VL_I64;        // VALUE_LIST<int64_t>
typedef VALUE_LIST VL_VL_I64;     // VALUE_LIST<VALUE_LIST<int64_t>>>
typedef VALUE_LIST VL_VL_VL_I64;  // VALUE_LIST<VALUE_LIST<VALUE_LIST<int64_t>>>
typedef VALUE_LIST VL_CRTPRIME;   // VALUE_LIST<CRT_PRIME *>
typedef VALUE_LIST VL_VL_CRTPRIME;  // VALUE_LIST<VALUE_LIST<CRT_PRIME *>>
typedef VALUE_LIST
    VL_VL_VL_CRTPRIME;  // VALUE_LIST<VALUE_LIST<VALUE_LIST<CRT_PRIME *>>>
typedef VALUE_LIST VL_BIGINT;  // VALUE_LIST<BIG_INT>

/**
 * @brief type of primes
 *
 */
typedef enum { Q_TYPE, P_TYPE, QPART_TYPE, QPART_COMP_TYPE } PRIME_TYPE;

/**
 * @brief crt primes
 *
 */
typedef struct {
  MODULUS     _moduli;
  NTT_CONTEXT _ntt;
} CRT_PRIME;

/**
 * @brief alloc crt primes
 *
 * @param n length of primes
 * @return CRT_PRIME *
 */
static inline CRT_PRIME* Alloc_crt_primes(size_t n) {
  CRT_PRIME* primes = (CRT_PRIME*)malloc(sizeof(CRT_PRIME) * n);
  return primes;
}

/**
 * @brief print CRT_PRIME
 *
 * @param fp
 * @param prime
 */
void Print_crtprime(FILE* fp, CRT_PRIME* prime);

/**
 * @brief cleanup crt primes
 *
 * @param prime
 */
void Free_crtprime(CRT_PRIME* prime);

/**
 * @brief Get the modulus from CRT_PRIME
 *
 * @param prime
 * @return MODULUS*
 */
static inline MODULUS* Get_modulus(CRT_PRIME* prime) {
  return &(prime->_moduli);
}

/**
 * @brief get modulus value from crt primes
 *
 * @param prime
 * @return int64_t
 */
static inline int64_t Get_modulus_val(CRT_PRIME* prime) {
  return prime->_moduli._val;
}

/**
 * @brief Get the NTT from CRT_PRIME
 *
 * @param prime
 * @return NTT_CONTEXT*
 */
static inline NTT_CONTEXT* Get_ntt(CRT_PRIME* prime) { return &(prime->_ntt); }

/**
 * @brief CRT_PRIME memory are continus
 * adjust the pointers to get next modulus
 *
 * @param modulus
 * @return MODULUS*
 */
static inline MODULUS* Get_next_modulus(MODULUS* modulus) {
  return (MODULUS*)((CRT_PRIME*)modulus + 1);
}

/**
 * @brief initialize crt primes
 *
 * @param prime crt primes to be initialized
 * @param val init val
 * @param ring_degree ring degree
 */
static inline void Init_crtprime(CRT_PRIME* prime, int64_t val,
                                 uint32_t ring_degree) {
  MODULUS* modulus = Get_modulus(prime);
  Init_modulus(modulus, val);
  Init_nttcontext(Get_ntt(prime), ring_degree, modulus);
}

/**
 * @brief precomputed value for CRT
 *
 */
typedef struct {
  BIG_INT    _modulus;           // BIG_INT
  VL_VL_I64* _m_mod_nb;          // For Q -> {Q % p_j}
                                 // For P -> {P % q_j}
  VL_VL_I64* _m_inv_mod_nb;      // For Q -> {Q^-1 % p_j}
                                 // For P -> {P^-1 % q_j}
  VL_BIGINT*  _hat;              // BIG_INT, Q/q_i
  VALUE_LIST* _hat_mod_nb;       // For Q -> VL_VL_VL_I64 {(Q/q_j) % p_j}
                                 // For P -> VL_VL_I64 {(P/p_j) % q_j}
  VL_BIGINT* _hat_inv;           // BIG_INT, mod inverse of hat, (Q/q_i)^-1
  VL_VL_I64* _hat_inv_mod_self;  // For Q -> VL_VL_I64 {((Q/q_i)^-1) % q_i}
                                 // For P -> VL_I64 {((P/p_i)^-1) % p_i}
  VL_VL_VL_I64* _hat_inv_mod_self_prec;  // Precomputed for barret mod
                                         // For Q, barret mod qi
                                         // For P, barret mod pi
  VL_VL_I64* _ql_div2_mod_qi;            // {(q_l/2) % q_i}
  VL_VL_I64* _ql_inv_mod_qi;             // {q_l^-1 % q_i}
  VL_VL_I64* _ql_inv_mod_qi_prec;        // precomputed const for {q_l^-1 % q_i}
  VL_VL_I64* _ql_ql_inv_mod_ql_div_ql_mod_qi;       //{((Q/q_l)*((Q/q_l)^-1 %
                                                    // q_l))/ql) % q_i}
  VL_VL_I64* _ql_ql_inv_mod_ql_div_ql_mod_qi_prec;  // precomputed const for
                                                    // {((Q/q_l)*((Q/q_l)^-1 %
                                                    // q_l))/ql) % q_i}
} PRE_COMP;

/**
 * @brief alloc precompute values
 *
 * @return PRE_COMP*
 */
static inline PRE_COMP* Alloc_precompute() {
  PRE_COMP* p = (PRE_COMP*)malloc(sizeof(PRE_COMP));
  memset(p, 0, sizeof(PRE_COMP));
  BI_INIT(p->_modulus);
  return p;
}

/**
 * @brief cleanup precompute values
 *
 * @param precomp
 */
void Free_precompute(PRE_COMP* precomp);

/**
 * @brief precomputed value of q part
 *
 */
typedef struct {
  size_t     _per_part_size;
  size_t     _num_p;
  VL_BIGINT* _modulus;
  VL_VL_I64* _l_hat_inv_modq;  // int64_t [num_parts][part_size][part_size - l]
  VL_VL_VL_I64
  *_l_hat_modp;  // int64_t [num_Q][l/part_size][part_size][complement_size]
} QPART_PRE_COMP;

/**
 * @brief alloc precomputed value of q part
 *
 * @return
 */
static inline QPART_PRE_COMP* Alloc_qpart_precomp() {
  QPART_PRE_COMP* precomp = (QPART_PRE_COMP*)malloc(sizeof(QPART_PRE_COMP));
  memset(precomp, 0, sizeof(QPART_PRE_COMP));
  return precomp;
}

/**
 * @brief print precomputed value of q part
 *
 * @param fp
 * @param precomp
 */
void Print_qpart_precompute(FILE* fp, QPART_PRE_COMP* precomp);

/**
 * @brief free precomputed value of q part
 *
 * @param precomp
 */
void Free_qpart_precompute(QPART_PRE_COMP* precomp);

/**
 * @brief crt primes, include primes & computed value
 *
 */
typedef struct {
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
} CRT_PRIMES;

/**
 * @brief precompute crt primes
 *
 * @param crt_primes
 */
void Precompute_primes(CRT_PRIMES* crt_primes, bool without_rescale);

/**
 * @brief copy value list of crtprimes from input to res
 *
 * @param res return VL_CRTPRIME
 * @param input input VL_CRTPRIME
 * @param cnt length of VL_CRTPRIME
 */
void Copy_vl_crtprime(VL_CRTPRIME* res, VL_CRTPRIME* input, size_t cnt);

/**
 * @brief print value list of crt primes
 *
 * @param fp
 * @param crt_primes
 */
void Print_vl_crtprime(FILE* fp, VL_CRTPRIME* crt_primes);

/**
 * @brief print crt primes
 *
 * @param fp
 * @param crt_primes
 */
void Print_crtprimes(FILE* fp, CRT_PRIMES* crt_primes);

/**
 * @brief cleanup crt primes
 *
 * @param crt_primes
 */
void Free_crtprimes(CRT_PRIMES* crt_primes);

// Access APIs for CRT_PRIMES
#define GET_BIG_M(crt_primes) (crt_primes)->_u._precomp->_modulus

/**
 * @brief if crt_primes is Q primes
 *
 * @param crt_primes
 * @return true if crt_primes is Q primes
 * @return false if crt_primes is not Q primes
 */
static inline bool Is_q(CRT_PRIMES* crt_primes) {
  return crt_primes->_type == Q_TYPE;
}

/**
 * @brief if crt_primes is P primes
 *
 * @param crt_primes
 * @return true if crt-primes is P primes
 * @return false if crt-primes is not P primes
 */
static inline bool Is_p(CRT_PRIMES* crt_primes) {
  return crt_primes->_type == P_TYPE;
}

/**
 * @brief if crt_primes is Q part primes
 *
 * @param crt_primes
 * @return true if crt_primes is Q part primes
 * @return false if crt_primes is not Q part primes
 */
static inline bool Is_qpart(CRT_PRIMES* crt_primes) {
  return crt_primes->_type == QPART_TYPE;
}

/**
 * @brief if crt_primes is Q part complement
 *
 * @param crt_primes
 * @return true
 * @return false
 */
static inline bool Is_qpart_compl(CRT_PRIMES* crt_primes) {
  return crt_primes->_type == QPART_COMP_TYPE;
}

/**
 * @brief Set the prime with given PRIME_TYPE
 *
 * @param crt_primes CRT_PRIMES
 * @param type prime type
 */
static inline void Set_prime_type(CRT_PRIMES* crt_primes, PRIME_TYPE type) {
  crt_primes->_type = type;
}

/**
 * @brief get value list of mod val from CRT_PRIMES
 *
 * @param crt_primes
 * @return VALUE_LIST*
 */
static inline VALUE_LIST* Get_mod_val_list(CRT_PRIMES* crt_primes) {
  return crt_primes->_mod_val_list;
}

/**
 * @brief get value list of CRT_PRIME from CRT_PRIMES
 *
 * @param crt_primes
 * @return VALUE_LIST*
 */
static inline VALUE_LIST* Get_primes(CRT_PRIMES* crt_primes) {
  return crt_primes->_primes;
}

/**
 * @brief get CRT_PRIME from CRT_PRIMES
 *
 * @param crt_primes
 * @param idx index of CRT_PRIME in CRT_PRIMES
 * @return CRT_PRIME*
 */
static inline CRT_PRIME* Get_prime_at(CRT_PRIMES* crt_primes, size_t idx) {
  IS_TRUE(Is_q(crt_primes) || Is_p(crt_primes), "invalid crt prime type");
  VALUE_LIST* primes = Get_primes(crt_primes);
  IS_TRUE(idx < LIST_LEN(primes), "index outof bound");
  return (CRT_PRIME*)PTR_VALUE_AT(primes, idx);
}

/**
 * @brief get head address of CRT_PRIME from CRT_PRIMES
 *
 * @param crt_primes
 * @return CRT_PRIME*
 */
static inline CRT_PRIME* Get_prime_head(CRT_PRIMES* crt_primes) {
  return Get_prime_at(crt_primes, 0);
}

/**
 * @brief get address of CRT_PRIME at given idx from CRT_PRIMES
 * RT_PRIME are put into continus memory
 *
 * @param prime_head
 * @param idx index of CRT_PRIME
 * @return CRT_PRIME*
 */
static inline CRT_PRIME* Get_nth_prime(CRT_PRIME* prime_head, size_t idx) {
  return prime_head + idx;
}

/**
 * @brief get next address of CRT_PRIME
 * CRT_PRIME are put into continus memory
 *
 * @param prime_head
 * @return CRT_PRIME*
 */
static inline CRT_PRIME* Get_next_prime(CRT_PRIME* prime_head) {
  return prime_head + 1;
}

/**
 * @brief get CRT_PRIME from VL_CRTPRIME at given idx
 *
 * @param vl_primes
 * @param idx
 * @return CRT_PRIME*
 */
static inline CRT_PRIME* Get_vlprime_at(VL_CRTPRIME* vl_primes, size_t idx) {
  IS_TRUE(idx < LIST_LEN(vl_primes), "index outof bound");
  return (CRT_PRIME*)Get_ptr_value_at(vl_primes, idx);
}

/**
 * @brief get head address of modulus from VL_CRTPRIME
 *
 * @param primes
 * @return MODULUS*
 */
static inline MODULUS* Get_modulus_head(VL_CRTPRIME* primes) {
  CRT_PRIME* prime_head = Get_vlprime_at(primes, 0);
  return Get_modulus(prime_head);
}

/**
 * @brief get VL_CRTPRIME from CRT_PRIMES at given part_idx
 *
 * @param crt_primes
 * @param part_idx
 * @return VL_CRTPRIME*
 */
static inline VL_CRTPRIME* Get_qpart_prime_at_l1(CRT_PRIMES* crt_primes,
                                                 size_t      part_idx) {
  IS_TRUE(Is_qpart(crt_primes), "invalid crt prime type");
  return VL_VALUE_AT(Get_primes(crt_primes), part_idx);
}

/**
 * @brief get qpart CRT_PRIME from CRT_PRIMES at given part_idx & prime_idx
 *
 * @param crt_primes
 * @param part_idx
 * @param prime_idx
 * @return CRT_PRIME*
 */
static inline CRT_PRIME* Get_qpart_prime_at_l2(CRT_PRIMES* crt_primes,
                                               size_t      part_idx,
                                               size_t      prime_idx) {
  IS_TRUE(Is_qpart(crt_primes), "invalid crt prime type");
  return (CRT_PRIME*)VL_L2_VALUE_AT(Get_primes(crt_primes), part_idx,
                                    prime_idx);
}

/**
 * @brief get qpart_compl VL_CRTPRIME* from CRT_PRIMES
 *
 * @param crt_primes
 * @param idx1
 * @param idx2
 * @return VL_CRTPRIME*
 */
static inline VL_CRTPRIME* Get_qpart_compl_at(CRT_PRIMES* crt_primes,
                                              size_t idx1, size_t idx2) {
  IS_TRUE(Is_qpart_compl(crt_primes), "invalid crt prime type");
  return VL_L2_VALUE_AT(Get_primes(crt_primes), idx1, idx2);
}

/**
 * @brief initialize CRT_PRIMES
 *
 * @param crt_primes CRT_PRIMES to be initialized
 * @param primes input primes
 * @param ring_degree ring degree
 */
static inline void Set_primes(CRT_PRIMES* crt_primes, VALUE_LIST* primes,
                              uint32_t ring_degree) {
  size_t prime_cnt          = LIST_LEN(primes);
  crt_primes->_mod_val_list = Alloc_value_list(PTR_TYPE, prime_cnt);
  MODULUS* modulus          = Alloc_modulus(prime_cnt);
  crt_primes->_primes       = Alloc_value_list(PTR_TYPE, prime_cnt);
  int64_t*   mods           = Get_i64_values(primes);
  CRT_PRIME* entries        = Alloc_crt_primes(prime_cnt);
  CRT_PRIME* entry          = entries;
  for (size_t idx = 0; idx < prime_cnt; idx++) {
    Init_modulus(modulus, *mods);
    Init_crtprime(entry, *mods, ring_degree);
    PTR_VALUE_AT(Get_primes(crt_primes), idx)       = (PTR)entry;
    PTR_VALUE_AT(Get_mod_val_list(crt_primes), idx) = (PTR)modulus;
    entry++;
    mods++;
    modulus++;
  }
}

/**
 * @brief get length of primes
 *
 * @param crt_primes
 * @return size_t
 */
static inline size_t Get_primes_cnt(CRT_PRIMES* crt_primes) {
  return LIST_LEN(Get_primes(crt_primes));
}

/**
 * @brief get number of parts from crt primes
 *
 * @param crt_primes
 * @return size_t
 */
static inline size_t Get_num_parts(CRT_PRIMES* crt_primes) {
  IS_TRUE(Is_qpart(crt_primes), "invalid crtprime type");
  return LIST_LEN(Get_primes(crt_primes));
}

/**
 * @brief get precomp from crt primes
 *
 * @param crt_primes
 * @return PRE_COMP*
 */
static inline PRE_COMP* Get_precomp(CRT_PRIMES* crt_primes) {
  IS_TRUE(Is_q(crt_primes) || Is_p(crt_primes), "invliad type");
  return crt_primes->_u._precomp;
}

/**
 * @brief get recucible level from crt primes
 *
 * @param crt_primes
 * @return size_t
 */
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

// Access APIs for Q & P precomputed
/**
 * @brief get Q/q_i from crt primes
 *
 * @param crt_primes
 * @return VL_BIGINT*
 */
static inline VL_BIGINT* Get_hat(CRT_PRIMES* crt_primes) {
  IS_TRUE(Is_q(crt_primes) || Is_p(crt_primes), "invalid type");
  return crt_primes->_u._precomp->_hat;
}

/**
 * @brief get mod inverse of hat, (Q/q_i)^-1
 *
 * @param crt_primes
 * @return VL_BIGINT*
 */
static inline VL_BIGINT* Get_hat_inv(CRT_PRIMES* crt_primes) {
  IS_TRUE(Is_q(crt_primes) || Is_p(crt_primes), "invalid type");
  return crt_primes->_u._precomp->_hat_inv;
}

// Access APIs for Q precomputed
/**
 * @brief get {Q % p_j} from q crt primes
 *
 * @param crt_primes
 * @return VL_VL_I64*
 */
static inline VL_VL_I64* Get_qmodp(CRT_PRIMES* crt_primes) {
  IS_TRUE(Is_q(crt_primes), "invliad type");
  return crt_primes->_u._precomp->_m_mod_nb;
}

/**
 * @brief get {Q % p_j} at given idx from q crt primes
 *
 * @param crt_primes
 * @param idx
 * @return VL_I64*
 */
static inline VL_I64* Get_qmodp_at(CRT_PRIMES* crt_primes, size_t idx) {
  IS_TRUE(Is_q(crt_primes), "invliad type");
  return Get_vl_value_at(crt_primes->_u._precomp->_m_mod_nb, idx);
}

/**
 * @brief get {Q^-1 % p_j} from q crt primes
 *
 * @param crt_primes
 * @return VL_VL_I64*
 */
static inline VL_VL_I64* Get_qinvmodp(CRT_PRIMES* crt_primes) {
  IS_TRUE(Is_q(crt_primes), "invliad type");
  return crt_primes->_u._precomp->_m_inv_mod_nb;
}

/**
 * @brief get {Q^-1 % p_j} at given idx from q crt primes
 *
 * @param crt_primes
 * @param idx
 * @return VL_I64*
 */
static inline VL_I64* Get_qinvmodp_at(CRT_PRIMES* crt_primes, size_t idx) {
  IS_TRUE(Is_q(crt_primes), "invliad type");
  return Get_vl_value_at(crt_primes->_u._precomp->_m_inv_mod_nb, idx);
}

/**
 * @brief get Q/q_i from q crt primes
 *
 * @param crt_primes
 * @return VL_BIGINT*
 */
static inline VL_BIGINT* Get_qhat(CRT_PRIMES* crt_primes) {
  IS_TRUE(Is_q(crt_primes), "invliad type");
  return crt_primes->_u._precomp->_hat;
}

/**
 * @brief get {{Q/q_j} % p_j} from q crt primes
 *
 * @param crt_primes
 * @return VL_VL_VL_I64*
 */
static inline VL_VL_VL_I64* Get_qhatmodp(CRT_PRIMES* crt_primes) {
  IS_TRUE(Is_q(crt_primes), "invliad type");
  return crt_primes->_u._precomp->_hat_mod_nb;
}

/**
 * @brief get {{Q/q_j} % p_j} at given idx from q crt primes
 *
 * @param crt_primes
 * @param idx1
 * @param idx2
 * @return VL_I64*
 */
static inline VL_I64* Get_qhatmodp_at(CRT_PRIMES* crt_primes, size_t idx1,
                                      size_t idx2) {
  IS_TRUE(Is_q(crt_primes), "invliad type");
  VL_VL_VL_I64* p = crt_primes->_u._precomp->_hat_mod_nb;
  return Get_vl_value_at(Get_vl_value_at(p, idx1), idx2);
}

/**
 * @brief get mod inverse of hat, (Q/q_i)^-1 for q prime
 *
 * @param crt_primes
 * @return VL_BIGINT*
 */
static inline VL_BIGINT* Get_qhatinv(CRT_PRIMES* crt_primes) {
  IS_TRUE(Is_q(crt_primes), "invliad type");
  return crt_primes->_u._precomp->_hat_inv;
}

/**
 * @brief get {((Q/q_i)^-1)%q_i} from crt primes
 *
 * @param crt_primes
 * @return VL_VL_I64*
 */
static inline VL_VL_I64* Get_qhatinvmodq(CRT_PRIMES* crt_primes) {
  IS_TRUE(Is_q(crt_primes), "invliad type");
  return crt_primes->_u._precomp->_hat_inv_mod_self;
}

/**
 * @brief get {((Q/q_i)^-1)%q_i} at given idx from crt primes
 *
 * @param crt_primes
 * @param idx
 * @return VL_I64*
 */
static inline VL_I64* Get_qhatinvmodq_at(CRT_PRIMES* crt_primes, size_t idx) {
  IS_TRUE(Is_q(crt_primes), "invliad type");
  return Get_vl_value_at(crt_primes->_u._precomp->_hat_inv_mod_self, idx);
}

//! @brief get precomputed values for {((Q/q_i)^-1)%q_i} at given level index
static inline VL_I64* Get_qhatinvmodq_prec_at(CRT_PRIMES* crt_primes,
                                              size_t      idx) {
  IS_TRUE(Is_q(crt_primes), "invliad type");
  return Get_vl_value_at(crt_primes->_u._precomp->_hat_inv_mod_self_prec, idx);
}

// Get API for P precomputed
/**
 * @brief get {P % q_j} from p crt primes
 *
 * @param crt_primes
 * @return VL_I64*
 */
static inline VL_I64* Get_pmodq(CRT_PRIMES* crt_primes) {
  IS_TRUE(Is_p(crt_primes), "invliad type");
  return Get_vl_value_at(crt_primes->_u._precomp->_m_mod_nb, 0);
}

/**
 * @brief get {P^-1 % q_j} from p crt primes
 *
 * @param crt_primes
 * @return
 */
static inline VL_I64* Get_pinvmodq(CRT_PRIMES* crt_primes) {
  IS_TRUE(Is_p(crt_primes), "invliad type");
  return Get_vl_value_at(crt_primes->_u._precomp->_m_inv_mod_nb, 0);
}

/**
 * @brief get P/p_i from p crt primes
 *
 * @param crt_primes
 * @return
 */
static inline VL_BIGINT* Get_phat(CRT_PRIMES* crt_primes) {
  IS_TRUE(Is_p(crt_primes), "invliad type");
  return crt_primes->_u._precomp->_hat;
}

/**
 * @brief get {{P/p_j} % q_j} from p crt primes
 *
 * @param crt_primes
 * @return VL_VL_I64*
 */
static inline VL_VL_I64* Get_phatmodq(CRT_PRIMES* crt_primes) {
  IS_TRUE(Is_p(crt_primes), "invliad type");
  return crt_primes->_u._precomp->_hat_mod_nb;
}

/**
 * @brief get {{P/p_j} % q_j} at given idx from p crt primes
 *
 * @param crt_primes
 * @param idx
 * @return VL_I64*
 */
static inline VL_I64* Get_phatmodq_at(CRT_PRIMES* crt_primes, size_t idx) {
  IS_TRUE(Is_p(crt_primes), "invliad type");
  return Get_vl_value_at(
      Get_vl_value_at(crt_primes->_u._precomp->_hat_mod_nb, 0), idx);
}

/**
 * @brief get mod inverse of hat, (P/p_i)^-1 for p crt prime
 *
 * @param crt_primes
 * @return VL_BIGINT*
 */
static inline VL_BIGINT* Get_phatinv(CRT_PRIMES* crt_primes) {
  IS_TRUE(Is_p(crt_primes), "invliad type");
  return crt_primes->_u._precomp->_hat_inv;
}

/**
 * @brief get {((P/p_i)^-1) % p_i} from p crt primes
 *
 * @param crt_primes
 * @return VL_I64*
 */
static inline VL_I64* Get_phatinvmodp(CRT_PRIMES* crt_primes) {
  IS_TRUE(Is_p(crt_primes), "invliad type");
  return Get_vl_value_at(crt_primes->_u._precomp->_hat_inv_mod_self, 0);
}

//! @brief get precomputed values for {((P/p_i)^-1)%p_i} from p crt primes
static inline VL_I64* Get_phatinvmodp_prec(CRT_PRIMES* crt_primes) {
  IS_TRUE(Is_p(crt_primes), "invliad type");
  return Get_vl_value_at(crt_primes->_u._precomp->_hat_inv_mod_self_prec, 0);
}

// Get API for QPart precomputed
/**
 * @brief get QPart precomputed
 *
 * @param crt_primes
 * @return QPART_PRE_COMP*
 */
static inline QPART_PRE_COMP* Get_qpart_precomp(CRT_PRIMES* crt_primes) {
  IS_TRUE(Is_qpart(crt_primes), "invliad type");
  return crt_primes->_u._qpart_precomp;
}

/**
 * @brief get size of QPart precomputed
 *
 * @param crt_primes
 * @return
 */
static inline size_t Get_per_part_size(CRT_PRIMES* crt_primes) {
  IS_TRUE(Is_qpart(crt_primes), "invliad type");
  return crt_primes->_u._qpart_precomp->_per_part_size;
}

/**
 * @brief get modulus of QPart precomputed
 *
 * @param crt_primes
 * @return VL_BIGINT*
 */
static inline VL_BIGINT* Get_qpart_modulus(CRT_PRIMES* crt_primes) {
  IS_TRUE(Is_qpart(crt_primes), "invliad type");
  return crt_primes->_u._qpart_precomp->_modulus;
}

/**
 * @brief get l_hat_inv_modq from QPart precomputed
 *
 * @param crt_primes
 * @return VL_VL_I64*
 */
static inline VL_VL_I64* Get_qlhatinvmodq(CRT_PRIMES* crt_primes) {
  IS_TRUE(Is_qpart(crt_primes), "invliad type");
  return crt_primes->_u._qpart_precomp->_l_hat_inv_modq;
}

/**
 * @brief get l_hat_modp from QPart precomputed
 *
 * @param crt_primes
 * @return VL_VL_VL_I64*
 */
static inline VL_VL_VL_I64* Get_qlhatmodp(CRT_PRIMES* crt_primes) {
  IS_TRUE(Is_qpart(crt_primes), "invliad type");
  return crt_primes->_u._qpart_precomp->_l_hat_modp;
}

//! @brief get _ql_div2_mod_qi at given idx from crt primes
static inline VL_I64* Get_ql_div2_mod_qi_at(CRT_PRIMES* crt_primes,
                                            size_t      idx) {
  IS_TRUE(Is_q(crt_primes), "invliad type");
  return Get_vl_value_at(crt_primes->_u._precomp->_ql_div2_mod_qi, idx);
}

//! @brief get _ql_inv_mod_qi at given idx from crt primes
static inline VL_I64* Get_ql_inv_mod_qi_at(CRT_PRIMES* crt_primes, size_t idx) {
  IS_TRUE(Is_q(crt_primes), "invliad type");
  return Get_vl_value_at(crt_primes->_u._precomp->_ql_inv_mod_qi, idx);
}

//! @brief get _ql_inv_mod_qi_prec at given idx from crt primes
static inline VL_I64* Get_ql_inv_mod_qi_prec_at(CRT_PRIMES* crt_primes,
                                                size_t      idx) {
  IS_TRUE(Is_q(crt_primes), "invliad type");
  return Get_vl_value_at(crt_primes->_u._precomp->_ql_inv_mod_qi_prec, idx);
}

//! @brief get _ql_ql_inv_mod_ql_div_ql_mod_qi at given idx from crt primes
static inline VL_I64* Get_ql_ql_inv_mod_ql_div_ql_mod_qi_at(
    CRT_PRIMES* crt_primes, size_t idx) {
  IS_TRUE(Is_q(crt_primes), "invliad type");
  return Get_vl_value_at(
      crt_primes->_u._precomp->_ql_ql_inv_mod_ql_div_ql_mod_qi, idx);
}

//! @brief get _ql_ql_inv_mod_ql_div_ql_mod_qi_prec at given idx from crt primes
static inline VL_I64* Get_ql_ql_inv_mod_ql_div_ql_mod_qi_prec_at(
    CRT_PRIMES* crt_primes, size_t idx) {
  IS_TRUE(Is_q(crt_primes), "invliad type");
  return Get_vl_value_at(
      crt_primes->_u._precomp->_ql_ql_inv_mod_ql_div_ql_mod_qi_prec, idx);
}

/**
 * @brief An instance of Chinese Remainder Theorem parameters
 * We split a large number into its prime factors
 *
 */
typedef struct {
  CRT_PRIMES _q;
  CRT_PRIMES _p;
  CRT_PRIMES _qpart;
  CRT_PRIMES _qpart_compl;
} CRT_CONTEXT;

/**
 * @brief malloc memory for CRT_CONTEXT
 *
 * @return CRT_CONTEXT*
 */
CRT_CONTEXT* Alloc_crtcontext();

/**
 * @brief initialize CRT_CONTEXT with max depth of multiply
 *
 * @param crt CRT_CONTEXT will be initialized
 * @param level security level
 * @param poly_degree polynomial degree of ring
 * @param mult_depth max depth of multiply
 * @param num_parts number of q parts
 */
void Init_crtcontext(CRT_CONTEXT* crt, SECURITY_LEVEL level,
                     uint32_t poly_degree, size_t mult_depth, size_t num_parts);

/**
 * @brief initialize CRT_CONTEXT with mod size
 *
 * @param crt CRT_CONTEXT will be initialized
 * @param level security level
 * @param poly_degree polynomial degree of ring
 * @param num_primes number of q primes
 * @param first_mod_size bit size of first mod
 * @param scaling_mod_size bit size of scaling factor
 * @param num_parts number of q parts
 */
void Init_crtcontext_with_prime_size(CRT_CONTEXT* crt, SECURITY_LEVEL level,
                                     uint32_t poly_degree, size_t num_primes,
                                     size_t first_mod_size,
                                     size_t scaling_mod_size, size_t num_parts);

/**
 * @brief cleanup crtContext memory
 *
 * @param crt
 */
void Free_crtcontext(CRT_CONTEXT* crt);

/**
 * @brief precompute with new base
 *
 * @param base
 * @param new_base
 */
void Precompute_new_base(CRT_PRIMES* base, CRT_PRIMES* new_base);

/**
 * @brief transform value to CRT representation (Q)
 *
 * @param crt_ret CRT representation from value
 * @param crt CRT representation
 * @param value value to be transformed to CRT representation
 */
void Transform_to_crt(VALUE_LIST* crt_ret, CRT_PRIMES* crt, BIG_INT value);

/**
 * @brief transform value to CRT representation (P * Q)
 *
 * @param crt_ret CRT representation from value
 * @param crt CRT representation
 * @param value value to be transformed to CRT representation
 */
void Transform_to_qpcrt(VALUE_LIST* crt_ret, CRT_CONTEXT* crt, BIG_INT value);

/**
 * @brief print crt context
 *
 * @param fp
 * @param crt
 */
void Print_crt(FILE* fp, CRT_CONTEXT* crt);

// Access APIs for CRT_CONTEXT
/**
 * @brief get crt primes for q
 *
 * @param crt
 * @return CRT_PRIMES*
 */
static inline CRT_PRIMES* Get_q(CRT_CONTEXT* crt) { return &(crt->_q); }

/**
 * @brief get crt primes for p
 *
 * @param crt
 * @return CRT_PRIMES*
 */
static inline CRT_PRIMES* Get_p(CRT_CONTEXT* crt) { return &(crt->_p); }

/**
 * @brief get crt primes for q part
 *
 * @param crt
 * @return CRT_PRIMES*
 */
static inline CRT_PRIMES* Get_qpart(CRT_CONTEXT* crt) { return &(crt->_qpart); }

/**
 * @brief get crt primes for q part complement
 *
 * @param crt
 * @return CRT_PRIMES*
 */
static inline CRT_PRIMES* Get_qpart_compl(CRT_CONTEXT* crt) {
  return &(crt->_qpart_compl);
}

/**
 * @brief Get q modulus head from crt context
 *
 * @param crt
 * @return Modulus*
 */
static inline MODULUS* Get_q_modulus_head(CRT_CONTEXT* crt) {
  return (MODULUS*)Get_ptr_value_at(crt->_q._mod_val_list, 0);
}

/**
 * @brief Get p modulus head from crt context
 *
 * @param crt
 * @return Modulus*
 */
static inline MODULUS* Get_p_modulus_head(CRT_CONTEXT* crt) {
  return (MODULUS*)Get_ptr_value_at(crt->_p._mod_val_list, 0);
}

/**
 * @brief get value list of crt primes for q
 *
 * @param crt
 * @return CRT_PRIMES*
 */
static inline VL_CRTPRIME* Get_q_primes(CRT_CONTEXT* crt) {
  return crt->_q._primes;
}

/**
 * @brief get value list of crt primes for p
 *
 * @param crt
 * @return CRT_PRIMES*
 */
static inline VL_CRTPRIME* Get_p_primes(CRT_CONTEXT* crt) {
  return crt->_p._primes;
}

/**
 * @brief get number of p prime
 *
 * @param crt
 * @return size_t
 */
static inline size_t Get_crt_num_p(CRT_CONTEXT* crt) {
  return Get_qpart(crt)->_u._qpart_precomp->_num_p;
}

/**
 * @brief get value list of crt primes for q part
 *
 * @param crt
 * @return
 */
static inline VL_VL_CRTPRIME* Get_qpart_primes(CRT_CONTEXT* crt) {
  return crt->_qpart._primes;
}

/**
 * @brief get value list of crt primes for q part complement
 *
 * @param crt
 * @return VL_VL_VL_CRTPRIME*
 */
static inline VL_VL_VL_CRTPRIME* Get_qpart_compl_primes(CRT_CONTEXT* crt) {
  return crt->_qpart_compl._primes;
}

#ifdef __cplusplus
}
#endif

#endif