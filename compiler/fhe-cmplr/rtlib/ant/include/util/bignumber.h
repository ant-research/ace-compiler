//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_INCLUDE_FHE_BIGNUMBER_H
#define RTLIB_INCLUDE_FHE_BIGNUMBER_H

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define _GMP_H_HAVE_FILE 1  // NOLINT(readability-identifier-naming)
#include <gmp.h>

#include "common/trace.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_INT64 9223372036854775807LL
#define MIN_INT64 -9223372036854775807LL

typedef mpz_t BIG_INT;

// Big Integer related API

// Initialization
#define BI_INIT(b)                  mpz_init(b)
#define BI_INITS(...)               mpz_inits(__VA_ARGS__, NULL)
#define BI_FREES(...)               mpz_clears(__VA_ARGS__, NULL)
#define BI_ASSIGN(b1, b2)           mpz_set(b1, b2)
#define BI_ASSIGN_SI(b, si64)       mpz_set_si(b, si64)
#define BI_ASSIGN_STR(b, str, base) mpz_set_str(b, str, base)
#define BI_ASSIGN_D(b, d)           mpz_set_d(b, d)
#define BI_INIT_ASSIGN(b1, b2)      mpz_init_set(b1, b2)
#define BI_INIT_ASSIGN_SI(b, si64)  mpz_init_set_si(b, si64)
#define BI_INIT_ASSIGN_UI(b, ui64)  mpz_init_set_ui(b, ui64)
#define BI_INIT_ASSIGN_D(b, double) mpz_init_set_d(b, double)

// Arithmetic
#define BI_ADD(res, b1, b2)          mpz_add(res, b1, b2)
#define BI_ADD_UI(res, b1, b2)       mpz_add_ui(res, b1, b2)
#define BI_SUB(res, b1, b2)          mpz_sub(res, b1, b2)
#define BI_MUL(res, b1, b2)          mpz_mul(res, b1, b2)
#define BI_MUL_UI(res, b1, ui64)     mpz_mul_ui(res, b1, ui64)
#define BI_MUL_SI(res, b1, si64)     mpz_mul_si(res, b1, si64)
#define BI_ADD_MUL(res, b1, b2)      mpz_addmul(res, b1, b2)
#define BI_ADD_MUL_UI(res, b1, ui64) mpz_addmul_ui(res, b1, ui64)
#define BI_DIV(res, b1, b2)          mpz_fdiv_q(res, b1, b2)
#define BI_DIV_UI(res, b1, b2)       mpz_fdiv_q_ui(res, b1, b2)
#define BI_DIV_NO_TRUNC(b1, d2)      Bi_mpz_div_no_trunc(b1, d2)
#define BI_MOD(res, b1, b2)          mpz_fdiv_r(res, b1, b2)
#define BI_MOD_UI(res, b1, ui64)     mpz_fdiv_r_ui(res, b1, ui64)
#define BI_POW_MOD(res, b1, b2, b3)  mpz_powm(res, b1, b2, b3)
#define BI_SUB_UI(res, b1, b2)       mpz_sub_ui(res, b1, b2);
#define BI_SQRT(res, b1)             mpz_sqrt(res, b1);
#define BI_POW_UI(res, b1, ui64)     mpz_pow_ui(res, b1, ui64)
#define BI_LOG2(res, b1)             res = Bi_mpz_log2(b1)
#define BI_LSHIFT(res, b1)           mpz_mul_2exp(res, res, b1)
#define BI_RSHIFT(res, b1, bits)     mpz_fdiv_q_2exp(res, b1, bits)
#define BI_SIZE_INBASE(b, base)      mpz_sizeinbase(b, base)

// Comparisons
#define BI_CMP(b1, b2)     mpz_cmp(b1, b2)
#define BI_CMP_SI(b, si64) mpz_cmp_si(b, si64)
#define BI_GT(b1, b2)      mpz_cmp(b1, b2) > 0
#define BI_LT(b1, b2)      mpz_cmp(b1, b2) < 0

// Convert
#define BI_GET_SI(b) mpz_get_si(b)
#define BI_GET_UI(b) mpz_get_ui(b)
#define BI_GET_D(b)  mpz_get_d(b)

// Print
#define BI_FORMAT       "%Zd"
#define BI_FPRINTF(...) gmp_fprintf(__VA_ARGS__)
#define BI_PRINTF(...)  gmp_printf(__VA_ARGS__)

//! @brief If input bigint value is in 64bit
static inline bool Is_bigint_in_64bit(BIG_INT bi) {
  return mpz_fits_slong_p(bi);
}

//! @brief Get log2() for bigint
//! @return double
static inline double Bi_mpz_log2(mpz_t bi) {
  signed long int ex;
  const double    di  = mpz_get_d_2exp(&ex, bi);
  double          res = log(di) / log(2) + (double)ex;
  return res;
}

//! @brief Print bigint
static inline void Print_bi(FILE* fp, BIG_INT bi) {
  BI_FPRINTF(fp, BI_FORMAT, bi);
  fflush(fp);
}

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_INCLUDE_FHE_BIGNUMBER_H
