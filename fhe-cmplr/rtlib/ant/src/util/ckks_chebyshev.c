//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "util/ckks_bootstrap_context.h"
#include "util/ckks_decryptor.h"
#include "util/ckks_evaluator.h"

//! @brief Populate statically the parameter m for the Paterson-Stockmeyer
//! algorithm up to the degree value of upperBoundDegree.
enum { UPPER_BOUND_PS = 2204 };

typedef VALUE_LIST VL_CIPHPTR;  // VALUE_LIST<CIPHERTEXT*>

void Eval_chebyshev_linear(CKKS_BTS_CTX* bts_ctx, CIPHERTEXT* out,
                           CIPHERTEXT* in, VL_DBL* coeffs, double a, double b) {
  FMT_ASSERT(FALSE, "TODO");
}

VL_CIPHPTR* Alloc_ciph_value_list(size_t cnt) {
  VL_CIPHPTR* vl_ciph = Alloc_value_list(PTR_TYPE, cnt);
  FOR_ALL_ELEM(vl_ciph, idx) {
    Set_ptr_value(vl_ciph, idx, (PTR)Alloc_ciphertext());
  }
  return vl_ciph;
}

void Free_ciph_value_list(VL_CIPHPTR* vl_ciph) {
  FOR_ALL_ELEM(vl_ciph, idx) {
    Free_ciphertext((CIPHERTEXT*)PTR_VALUE_AT(vl_ciph, idx));
  }
  Free_value_list(vl_ciph);
}

//! @brief Return the degree of the polynomial described by coefficients,
//! which is the index of the last non-zero element in the coefficients - 1.
//! Don't throw an error if all the coefficients are zero, but return 0.
uint32_t Get_degree_from_coeffs(VL_DBL* coeffs) {
  if (coeffs == NULL) return 0;

  uint32_t deg = 1;
  for (int32_t i = LIST_LEN(coeffs) - 1; i > 0; i--) {
    if (DBL_VALUE_AT(coeffs, i) == 0) {
      deg += 1;
    } else
      break;
  }
  return LIST_LEN(coeffs) - deg;
}

//! Return true if the polynomial is even. Otherwise, return false.
bool Is_even_poly(VL_DBL* coeffs) {
  if (coeffs == NULL) return false;
  uint32_t poly_deg = Get_degree_from_coeffs(coeffs);
  for (uint32_t deg = 1; deg <= poly_deg; deg += 2) {
    double coeff = Get_dbl_value_at(coeffs, deg);
    if (coeff != 0.) return false;
  }
  return true;
}

static VL_DBL* Precomp_mlist = NULL;
#pragma omp    threadprivate(Precomp_mlist)

VL_DBL* Populate_param_ps() {
  if (Precomp_mlist == NULL) {
    Precomp_mlist       = Alloc_value_list(DBL_TYPE, UPPER_BOUND_PS);
    uint32_t ranges[16] = {
        2,   11,  13,  17,   55,   59,   76,   239,
        247, 284, 991, 1007, 1083, 2015, 2031, UPPER_BOUND_PS};
    uint32_t values[16] = {1, 2, 3, 2, 3, 4, 3, 4, 5, 4, 5, 6, 5, 6, 7, 6};

    size_t j = 0;
    for (size_t i = 0; i < 16; i++) {
      for (; j < ranges[i]; j++) {
        Set_dbl_value(Precomp_mlist, j, values[i]);
      }
      j = ranges[i];
    }
  }
  return Precomp_mlist;
}

//! @brief Compute positive integers k,m such that n < k(2^m-1), k is close to
//! sqrt(n/2) and the depth = ceil(log2(k))+m is minimized. Moreover, for that
//! depth the
//!  number of homomorphic multiplications = k+2m+2^(m-1)-4 is minimized.
//!  Since finding these parameters involve testing many possible values, we
//!  hardcode them for commonly used degrees, and provide a heuristic which
//!  minimizes the number of homomorphic multiplications for the rest of the
//!  degrees.
void Compute_degree_ps(VL_UI32* res, uint32_t n) {
  IS_TRUE(n, "degree is zero, no need to evalute the polynomial");

  // index n-1 in the vector corresponds to degree n
  if (n <= UPPER_BOUND_PS) {
    Precomp_mlist = Populate_param_ps(UPPER_BOUND_PS);
    uint32_t m    = Get_dbl_value_at(Precomp_mlist, n - 1);
    uint32_t k    = floor(n / ((1 << m) - 1)) + 1;
    Set_ui32_value(res, 0, k);
    Set_ui32_value(res, 1, m);
  } else {
    // heuristic for larger degrees
    double   sqn2     = floor(log2(sqrt(n / 2)));
    uint32_t res_k    = 0;
    uint32_t res_m    = 0;
    uint32_t min_mult = 0xffffffff;

    for (uint32_t k = 1; k <= n; k++) {
      for (uint32_t m = 1; m <= ceil(log2(n / k) + 1) + 1; m++) {
        if (((int32_t)n - (int32_t)k * ((1 << m) - 1)) < 0) {
          if (abs(floor(log2(k)) - sqn2) <= 1) {
            uint32_t mul = k + 2 * m + (1 << (m - 1)) - 4;
            if (min_mult > mul) {
              min_mult = mul;
              res_k    = k;
              res_m    = m;
            }
          }
        }
      }
    }
    Set_ui32_value(res, 0, res_k);
    Set_ui32_value(res, 1, res_m);
  }
}

double Prec = 9.5367431640625e-07;  // pow(2, -20)

bool Is_not_equal_one(double val) {
  if (1 - Prec >= val) {
    return true;
  }
  if (1 + Prec <= val) {
    return true;
  }
  return false;
}

//! @brief f and g are vectors of Chebyshev interpolation coefficients of the
//! two polynomials. We assume their dominant coefficient is not zero.
//! Long_div_chebyshev returns the vector of Chebyshev interpolation
//! coefficients for the quotient and remainder of the division f/g.
//! We assume that the zero-th coefficient is c0, not c0/2 and returns
//! the same format.
void Long_div_chebyshev(VL_DBL* q, VL_DBL* r, VL_DBL* f, VL_DBL* g) {
  IS_TRUE(f && g && q && r, "null input or output");

  uint32_t n = Get_degree_from_coeffs(f);
  uint32_t k = Get_degree_from_coeffs(g);

  FMT_ASSERT((n == LIST_LEN(f) - 1),
             "The dominant coefficient of the divident is zero");
  FMT_ASSERT((k == LIST_LEN(g) - 1),
             "The dominant coefficient of the divident is zero");

  Resize_dbl_value_list(r, LIST_LEN(f), 0.0);
  Copy_value_list(r, f);

  if (n >= k) {
    Resize_dbl_value_list(q, n - k + 1, 0.0);

    while (n > k) {
      double q_n_k = 2 * Get_dbl_value_at(r, LIST_LEN(r) - 1);
      Set_dbl_value(q, n - k, q_n_k);
      if (Is_not_equal_one(Get_dbl_value_at(g, k))) {
        Set_dbl_value(q, n - k, q_n_k / Get_dbl_value_at(g, LIST_LEN(g) - 1));
      }

      VL_DBL* d = Alloc_value_list(DBL_TYPE, n + 1);
      if (k == n - k) {
        Set_dbl_value(d, 0, 2 * Get_dbl_value_at(g, n - k));

        for (uint32_t i = 1; i < 2 * k + 1; i++) {
          Set_dbl_value(d, i, Get_dbl_value_at(g, abs((int32_t)(n - k - i))));
        }
      } else {
        if ((int32_t)k > (int32_t)(n - k)) {
          Set_dbl_value(d, 0, 2 * Get_dbl_value_at(g, n - k));

          for (uint32_t i = 1; i < k - (n - k) + 1; i++) {
            Set_dbl_value(d, i,
                          Get_dbl_value_at(g, abs((int32_t)(n - k - i))) +
                              Get_dbl_value_at(g, (int32_t)(n - k + i)));
          }
          for (uint32_t i = k - (n - k) + 1; i < n + 1; i++) {
            Set_dbl_value(d, i, Get_dbl_value_at(g, abs((int32_t)(i - n + k))));
          }
        } else {
          Set_dbl_value(d, n - k, Get_dbl_value_at(g, 0));
          for (uint32_t i = n - 2 * k; i < n + 1; i++) {
            Set_dbl_value(d, i, Get_dbl_value_at(g, abs((int32_t)(i - n + k))));
          }
        }
      }

      double r_back = Get_dbl_value_at(r, LIST_LEN(r) - 1);
      if (Is_not_equal_one(r_back)) {
        // d * r[n]
        FOR_ALL_ELEM(d, idx) {
          Set_dbl_value(d, idx, Get_dbl_value_at(d, idx) * r_back);
        }
      }
      double g_back = Get_dbl_value_at(g, LIST_LEN(g) - 1);
      if (Is_not_equal_one(g_back)) {
        // d / = g[k]
        FOR_ALL_ELEM(d, idx) {
          Set_dbl_value(d, idx, Get_dbl_value_at(d, idx) / g_back);
        }
      }
      // f -= d
      FOR_ALL_ELEM(r, idx) {
        Set_dbl_value(r, idx,
                      Get_dbl_value_at(r, idx) - Get_dbl_value_at(d, idx));
      }

      if (LIST_LEN(r) > 1) {
        n = Get_degree_from_coeffs(r);
        Resize_dbl_value_list(r, n + 1, 0);
      }
      Free_value_list(d);
    }

    if (n == k) {
      double r_back = Get_dbl_value_at(r, LIST_LEN(r) - 1);
      double g_back = Get_dbl_value_at(g, LIST_LEN(g) - 1);
      Set_dbl_value(q, 0, r_back);
      if (Is_not_equal_one(g_back)) {
        Set_dbl_value(q, 0, r_back / g_back);  // q[0] /= g[k]
      }
      VL_DBL* d = Alloc_copy_value_list(LIST_LEN(g), g);
      if (Is_not_equal_one(r_back)) {
        // d *= f[n]
        FOR_ALL_ELEM(d, idx) {
          Set_dbl_value(d, idx, Get_dbl_value_at(d, idx) * r_back);
        }
      }
      if (Is_not_equal_one(g_back)) {
        // d /= g[k]
        FOR_ALL_ELEM(d, idx) {
          Set_dbl_value(d, idx, Get_dbl_value_at(d, idx) / g_back);
        }
      }
      // f -= d
      FOR_ALL_ELEM(r, idx) {
        Set_dbl_value(r, idx,
                      Get_dbl_value_at(r, idx) - Get_dbl_value_at(d, idx));
      }
      if (LIST_LEN(r) > 1) {
        n = Get_degree_from_coeffs(r);
        Resize_dbl_value_list(r, n + 1, 0);
      }
      Free_value_list(d);
    }
    // Because we want to have [c0] in the last spot, not [c0/2]
    Set_dbl_value(q, 0, Get_dbl_value_at(q, 0) * 2);
  } else {
    Resize_dbl_value_list(q, 1, 0.0);
  }
}

void Eval_chebyshev(CKKS_BTS_CTX* bts_ctx, CIPHERTEXT* out, CIPHERTEXT* in,
                    VL_DBL* coeffs, double a, double b) {
  uint32_t n = Get_degree_from_coeffs(coeffs);

  if (n < 5) {
    Eval_chebyshev_linear(bts_ctx, out, in, coeffs, a, b);
  } else {
    Eval_chebyshev_ps(bts_ctx, out, in, coeffs, a, b);
  }
}

void Eval_linear_wsum_mutable(CKKS_BTS_CTX* bts_ctx, CIPHERTEXT* out,
                              VL_CIPHPTR* ctxs, VL_DBL* weight) {
  CKKS_EVALUATOR* eval = Get_bts_eval(bts_ctx);

  if (!FIXED_MANUAL) {
    IS_TRUE(FALSE, "auto rescale is not supported");
  }

  bool        first_valid_term = true;
  CIPHERTEXT* tmp              = Alloc_ciphertext();
  for (uint32_t i = 0; i < LIST_LEN(ctxs); i++) {
    CIPHERTEXT* ciph_i   = (CIPHERTEXT*)Get_ptr_value_at(ctxs, i);
    double      weight_i = Get_dbl_value_at(weight, i);
    if (weight_i == 0.) continue;
    Mul_const(tmp, ciph_i, weight_i, eval);
    if (first_valid_term) {
      Copy_ciphertext(out, tmp);
      first_valid_term = false;
    } else {
      Add_ciphertext(out, out, tmp, eval);
    }
  }
  IS_TRUE(!first_valid_term, "polynomial has no non_zero coefficient");
  Rescale_ciphertext(out, out, eval);
  Free_ciphertext(tmp);
}

void Eval_linear_wsum(CKKS_BTS_CTX* bts_ctx, CIPHERTEXT* out,
                      VL_CIPHPTR* in_list, size_t size, VL_DBL* weights) {
  VL_CIPHPTR* ctxs = Alloc_ciph_value_list(size);

  CIPHERTEXT** ctx_head = (CIPHERTEXT**)Get_ptr_values(ctxs);
  for (uint32_t i = 0; i < size; i++) {
    Copy_ciphertext(ctx_head[i], (CIPHERTEXT*)Get_ptr_value_at(in_list, i));
  }

  Eval_linear_wsum_mutable(bts_ctx, out, ctxs, weights);

  Free_ciph_value_list(ctxs);
}

void Eval_quot_or_rem(CKKS_BTS_CTX* bts_ctx, CIPHERTEXT* out,
                      VL_CIPHPTR* in_list, VL_DBL* quot_rem, uint32_t k,
                      bool is_quotient, bool in_recursion) {
  CKKS_EVALUATOR* eval = Get_bts_eval(bts_ctx);

  VL_DBL* qr_copy = Alloc_copy_value_list(LIST_LEN(quot_rem), quot_rem);
  Resize_dbl_value_list(qr_copy, k, 0.0);

  CIPHERTEXT* t_k_1 = (CIPHERTEXT*)Get_ptr_value_at(in_list, k - 1);
  size_t      dg    = Get_degree_from_coeffs(qr_copy);
  if (dg > 0) {
    VL_DBL weights;
    Init_dbl_value_list_no_copy(&weights, dg, Get_dbl_values(qr_copy) + 1);
    Eval_linear_wsum(bts_ctx, out, in_list, dg, &weights);

    if (is_quotient) {
      if (in_recursion) {
        // the highest order coefficient will always be a power of two up to
        // 2^{m-1} because q is "monic" but the Chebyshev rule adds a factor
        // of 2 we don't need to increase the depth by multiplying the highest
        // order coefficient, but instead checking and summing, since we work
        // with m <= 4.
        double quot_last = Get_dbl_value_at(quot_rem, LIST_LEN(quot_rem) - 1);
        CIPHERTEXT* sum  = Alloc_ciphertext();
        Copy_ciphertext(sum, t_k_1);
        for (uint32_t i = 0; i < log2(quot_last); i++) {
          Add_ciphertext(sum, sum, sum, eval);
        }
        Add_ciphertext(out, out, sum, eval);
        Free_ciphertext(sum);
      } else {
        // the highest order coefficient will always be 2 after
        // one division because of the Chebyshev division rule
        Add_ciphertext(out, out, t_k_1, eval);
        Add_ciphertext(out, out, t_k_1, eval);
      }
    } else {
      // is remainder
      // the highest order coefficient will always be 1 because remainder is
      // monic.
      Add_ciphertext(out, out, t_k_1, eval);
    }
  } else {
    if (is_quotient) {
      Copy_ciphertext(out, t_k_1);
      double   quot_last = Get_dbl_value_at(quot_rem, LIST_LEN(quot_rem) - 1);
      uint32_t end       = in_recursion ? log2(quot_last) : quot_last;
      for (uint32_t i = 0; i < end; i++) {
        Add_ciphertext(out, out, t_k_1, eval);
      }
    } else {
      Copy_ciphertext(out, t_k_1);
    }
  }

  // adds the free term (at x^0)
  Add_const(out, out, Get_dbl_value_at(quot_rem, 0) / 2, eval);
  // The number of levels of qu is the same as the number of levels of T[k-1]
  // + 1. Will only get here when m = 2, so the number of levels of qu and
  // T2[m-1] will be the same
  Free_value_list(qr_copy);
}

//! @brief eval chebyshev inner transformation with paterson-stockmeyer
//! algorithm
void Inner_eval_chebyshev_ps(CKKS_BTS_CTX* bts_ctx, CIPHERTEXT* out,
                             CIPHERTEXT* in, VL_DBL* coeffs, uint32_t k,
                             uint32_t m, VL_CIPHPTR* t_list,
                             VL_CIPHPTR* t2_list, bool in_recursion) {
  CKKS_EVALUATOR* eval      = Get_bts_eval(bts_ctx);
  SWITCH_KEY*     relin_key = eval->_keygen->_relin_key;

  uint32_t k2m2k = k * (1 << (m - 1)) - k;

  // Divide coefficients by T^{k*2^{m-1}}
  VL_DBL* tkm = Alloc_value_list(DBL_TYPE, k2m2k + k + 1);
  Set_dbl_value(tkm, LIST_LEN(tkm) - 1, 1);

  VL_DBL* div_q = Alloc_value_list(DBL_TYPE, 0);
  VL_DBL* div_r = Alloc_value_list(DBL_TYPE, 0);
  Long_div_chebyshev(div_q, div_r, coeffs, tkm);

  // Subtract x^{k(2^{m-1} - 1)} from r
  VL_DBL* r2 = Alloc_copy_value_list(LIST_LEN(div_r), div_r);

  if (k2m2k <= Get_degree_from_coeffs(div_r)) {
    Set_dbl_value(r2, k2m2k, Get_dbl_value_at(r2, k2m2k) - 1);
    Resize_dbl_value_list(r2, Get_degree_from_coeffs(r2) + 1, 0.0);
  } else {
    Resize_dbl_value_list(r2, k2m2k + 1, 0.0);
    Set_dbl_value(r2, LIST_LEN(r2) - 1, -1);
  }

  // Divide r2 by q
  VL_DBL* divr2_q = Alloc_value_list(DBL_TYPE, 0);
  VL_DBL* divr2_r = Alloc_value_list(DBL_TYPE, 0);
  Long_div_chebyshev(divr2_q, divr2_r, r2, div_q);

  // Add x^{k(2^{m-1} - 1)} to s
  size_t s2_len = LIST_LEN(divr2_r);
  s2_len        = (s2_len > (k2m2k + 1)) ? s2_len : k2m2k + 1;
  VL_DBL* s2    = Alloc_copy_value_list(s2_len, divr2_r);
  Set_dbl_value(s2, s2_len - 1, 1);

  // Evaluate c at u
  CIPHERTEXT* t0     = (CIPHERTEXT*)PTR_VALUE_AT(t_list, 0);
  CIPHERTEXT* cu     = Alloc_ciphertext();
  uint32_t    dc     = Get_degree_from_coeffs(divr2_q);
  bool        flag_c = false;
  if (dc >= 1) {
    if (dc == 1) {
      double q1 = Get_dbl_value_at(divr2_q, 1);
      if (q1 != 1) {
        Mul_const(cu, t0, q1, eval);
        Rescale_ciphertext(cu, cu, eval);
      } else {
        Copy_ciphertext(cu, t0);
      }
    } else {
      VL_DBL weights;
      Init_dbl_value_list_no_copy(&weights, dc, Get_dbl_values(divr2_q) + 1);
      Eval_linear_wsum(bts_ctx, cu, t_list, dc, &weights);
    }
    // adds the free term (at x^0)
    Add_const(cu, cu, Get_dbl_value_at(divr2_q, 0) / 2, eval);
    flag_c = true;
  }
  // Evaluate q and s2 at u. If their degrees are larger than k,
  // then recursively apply the Paterson-Stockmeyer algorithm.
  CIPHERTEXT* qu = Alloc_ciphertext();
  if (Get_degree_from_coeffs(div_q) > k) {
    Inner_eval_chebyshev_ps(bts_ctx, qu, in, div_q, k, m - 1, t_list, t2_list,
                            true);
  } else {
    Eval_quot_or_rem(bts_ctx, qu, t_list, div_q, k, true, in_recursion);
  }

  CIPHERTEXT* su = Alloc_ciphertext();
  if (Get_degree_from_coeffs(s2) > k) {
    Inner_eval_chebyshev_ps(bts_ctx, su, in, s2, k, m - 1, t_list, t2_list,
                            true);
  } else {
    Eval_quot_or_rem(bts_ctx, su, t_list, s2, k, false, in_recursion);
  }

  // Need to reduce levels up to the level of T2[m-1].
  Set_ciph_level(cu,
                 Get_ciph_level((CIPHERTEXT*)Get_ptr_value_at(t2_list, m - 1)));
  CIPHERTEXT* t2_m_1 = (CIPHERTEXT*)Get_ptr_value_at(t2_list, m - 1);
  if (flag_c) {
    Add_ciphertext(out, t2_m_1, cu, eval);
  } else {
    Add_const(out, t2_m_1, Get_dbl_value_at(divr2_q, 0) / 2, eval);
  }

  Mul_ciphertext(out, out, qu, relin_key, eval);
  Rescale_ciphertext(out, out, eval);

  Add_ciphertext(out, out, su, eval);

  Free_ciphertext(qu);
  Free_ciphertext(su);
  Free_ciphertext(cu);
  Free_value_list(s2);
  Free_value_list(divr2_q);
  Free_value_list(divr2_r);
  Free_value_list(r2);
  Free_value_list(div_q);
  Free_value_list(div_r);
  Free_value_list(tkm);
}

void Eval_chebyshev_ps(CKKS_BTS_CTX* bts_ctx, CIPHERTEXT* out, CIPHERTEXT* in,
                       VL_DBL* coeffs, double a, double b) {
  CKKS_EVALUATOR* eval      = Get_bts_eval(bts_ctx);
  SWITCH_KEY*     relin_key = eval->_keygen->_relin_key;

  uint32_t n            = Get_degree_from_coeffs(coeffs);
  bool     is_even_poly = Is_even_poly(coeffs);

  VL_DBL* f2 = NULL;
  // Make sure the coefficients do not have the zero dominant terms
  if (VL_VALUE_AT(coeffs, LIST_LEN(coeffs) - 1) == 0) {
    f2 = Alloc_value_list(DBL_TYPE, n + 1);
  } else {
    f2 = Alloc_value_list(DBL_TYPE, LIST_LEN(coeffs));
  }
  Init_dbl_value_list(f2, LIST_LEN(f2), Get_dbl_values(coeffs));

  VL_UI32* degs = Alloc_value_list(UI32_TYPE, 2);
  Compute_degree_ps(degs, n);
  uint32_t k = Get_ui32_value_at(degs, 0);
  uint32_t m = Get_ui32_value_at(degs, 1);
  // to avoid using odd terms in decomposing polynomial,
  // use even k for even polynomials.
  if (is_even_poly && (k % 2 == 1)) {
    k += 1;
  }

  // computes linear transformation y = -1 + 2 (x-a)/(b-a)
  // consumes one level when a <> -1 && b <> 1
  VL_CIPHPTR* t_list = Alloc_ciph_value_list(k);
  CIPHERTEXT* t0     = (CIPHERTEXT*)PTR_VALUE_AT(t_list, 0);

  double rnd_a = round(a);
  double rnd_b = round(b);
  if ((rnd_a == -1) && (rnd_b == 1) && (a - rnd_a < 1E-10) &&
      (b - rnd_b < 1E-10)) {
    // no linear transformation needed if a = -1, b = 1
    Copy_ciphertext(t0, in);
  } else {
    // perform linear transformation, convert x to from [a, b] to range [-1, 1]
    double alpha = 2 / (b - a);
    double beta  = alpha * a;

    Mul_const(t0, in, alpha, eval);
    Rescale_ciphertext(t0, t0, eval);
    Add_const(t0, t0, -1.0 - beta, eval);
  }

  CIPHERTEXT* y = Alloc_ciphertext();
  Copy_ciphertext(y, t0);

  // computes chebyshev polynomial up to degree k
  // for y: T_1(y) = y, T2(y), ..., T_k(y)
  // use binary tree multiplication
  CIPHERTEXT* prod        = Alloc_ciphertext();
  PLAINTEXT*  neg_1_plain = Alloc_plaintext();
  POLYNOMIAL* neg_1_poly  = Get_plain_poly(neg_1_plain);
  size_t      neg_1_level = Get_ciph_level(t0);
  Encode_val_at_level(neg_1_plain, eval->_encoder, (DCMPLX)-1.0, neg_1_level,
                      Get_ciph_sf_degree(t0));
  for (uint32_t i = 2; i <= k; i++) {
    uint32_t    j   = i - 1;
    CIPHERTEXT* t_j = (CIPHERTEXT*)PTR_VALUE_AT(t_list, j);
    // if i is power of 2
    if (!(i & (i - 1))) {
      // compute T_{2i}(y) = 2 * T_i(y) ^ 2 - 1
      CIPHERTEXT* t_ihalf_1 = (CIPHERTEXT*)PTR_VALUE_AT(t_list, i / 2 - 1);
      Mul_ciphertext(prod, t_ihalf_1, t_ihalf_1, relin_key, eval);
      Add_ciphertext(t_j, prod, prod, eval);
      Rescale_ciphertext(t_j, t_j, eval);
      size_t saved_level = Save_poly_level(neg_1_poly, Get_ciph_level(t_j));
      Add_plaintext(t_j, t_j, neg_1_plain, eval);
      Restore_poly_level(neg_1_poly, saved_level);
    } else {
      // non-power of 2
      if (i % 2 == 1) {
        // if i is odd
        // for even polynomial, not need odd terms in decomposing polynomial
        if (is_even_poly) {
          continue;
        }
        // compute T_{2i+1}(y) = 2*T_i(y)*T_{i+1}(y) - y
        CIPHERTEXT* t_ihalf   = (CIPHERTEXT*)PTR_VALUE_AT(t_list, i / 2);
        CIPHERTEXT* t_ihalf_1 = (CIPHERTEXT*)PTR_VALUE_AT(t_list, i / 2 - 1);
        Mul_ciphertext(prod, t_ihalf_1, t_ihalf, relin_key, eval);
        Add_ciphertext(t_j, prod, prod, eval);
        Rescale_ciphertext(t_j, t_j, eval);
        Sub_ciphertext(t_j, t_j, y, eval);
      } else {
        // i is even
        // compute T_i(y) = 2*T_{i/2}(y)^2 - 1
        // for even polynomial and odd i/2, T_i= 2*T_{i/2+1}*T_{i/2-1} - T_2
        uint32_t ihalf_1 = i / 2;
        if (is_even_poly && (ihalf_1 % 2 == 1)) {
          ihalf_1 += 1;
        }
        uint32_t    ihalf_2   = i - ihalf_1;
        CIPHERTEXT* t_ihalf_1 = (CIPHERTEXT*)PTR_VALUE_AT(t_list, ihalf_1 - 1);
        CIPHERTEXT* t_ihalf_2 = (CIPHERTEXT*)PTR_VALUE_AT(t_list, ihalf_2 - 1);
        Mul_ciphertext(prod, t_ihalf_1, t_ihalf_2, relin_key, eval);
        Add_ciphertext(t_j, prod, prod, eval);
        Rescale_ciphertext(t_j, t_j, eval);
        if (ihalf_1 == ihalf_2) {
          size_t saved_level = Save_poly_level(neg_1_poly, Get_ciph_level(t_j));
          Add_plaintext(t_j, t_j, neg_1_plain, eval);
          Restore_poly_level(neg_1_poly, saved_level);
        } else {
          CIPHERTEXT* t_2 = (CIPHERTEXT*)PTR_VALUE_AT(t_list, 1);
          Sub_ciphertext(t_j, t_j, t_2, eval);
        }
      }
    }
  }

  if (FIXED_MANUAL) {
    CIPHERTEXT* tk_1 = (CIPHERTEXT*)Get_ptr_value_at(t_list, k - 1);
    for (size_t i = 1; i < k; i++) {
      // for even polynomial, not need update mul_level of odd terms.
      if (is_even_poly && i % 2 == 1) continue;
      CIPHERTEXT* ti_1 = (CIPHERTEXT*)Get_ptr_value_at(t_list, i - 1);
      int64_t     level_diff =
          (int64_t)(Get_ciph_level(ti_1) - Get_ciph_level(tk_1));
      while (level_diff > 0) {
        Mod_down_q_primes(Get_c0(ti_1));
        Mod_down_q_primes(Get_c1(ti_1));
        level_diff--;
      }
      Set_ciph_level(ti_1, Get_ciph_level(tk_1));
    }
  } else {
    for (size_t i = 1; i < k; i++) {
      // adjust level and depth
      FMT_ASSERT(false, "TODO");
    }
  }

  VL_CIPHPTR*  t2_list   = Alloc_ciph_value_list(m);
  CIPHERTEXT** t2_values = (CIPHERTEXT**)Get_ptr_values(t2_list);
  CIPHERTEXT*  t_tail    = (CIPHERTEXT*)Get_ptr_value_at(t_list, k - 1);

  // compute chebyshev polynomials T_{2k}(y), T_{4k}(y), ..., T_{2^{m-1}k}(y)
  Copy_ciphertext(t2_values[0], t_tail);
  for (uint32_t i = 1; i < m; i++) {
    CIPHERTEXT* t2_i_1 = t2_values[i - 1];
    CIPHERTEXT* t2_i   = t2_values[i];
    Mul_ciphertext(prod, t2_i_1, t2_i_1, relin_key, eval);
    Add_ciphertext(t2_i, prod, prod, eval);
    Rescale_ciphertext(t2_i, t2_i, eval);
    size_t saved_level = Save_poly_level(neg_1_poly, Get_ciph_level(t2_i));
    Add_plaintext(t2_i, t2_i, neg_1_plain, eval);
    Restore_poly_level(neg_1_poly, saved_level);
  }

  // compute T_{k(2m -1)}(y)
  CIPHERTEXT* t2km1 = Alloc_ciphertext();
  Copy_ciphertext(t2km1, t2_values[0]);
  for (uint32_t i = 1; i < m; i++) {
    // compute T_{k(2*m - 1)} = 2*T_{k(2^{m-1}-1)}(y)*T_{k*2^{m-1}}(y) - T_k(y)
    Mul_ciphertext(prod, t2km1, t2_values[i], relin_key, eval);
    Add_ciphertext(t2km1, prod, prod, eval);
    Rescale_ciphertext(t2km1, t2km1, eval);
    Sub_ciphertext(t2km1, t2km1, t2_values[0], eval);
  }

  uint32_t k2m2k = k * (1 << (m - 1)) - k;

  // Add T^{k(2^m - 1)}(y) to the polynomial that has to be evaluated
  Resize_dbl_value_list(f2, 2 * k2m2k + k + 1, 0.0);
  Set_dbl_value(f2, LIST_LEN(f2) - 1, 1);

  Inner_eval_chebyshev_ps(bts_ctx, out, in, f2, k, m, t_list, t2_list, false);

  Sub_ciphertext(out, out, t2km1, eval);

  Free_plaintext(neg_1_plain);
  Free_ciphertext(y);
  Free_ciphertext(t2km1);
  Free_ciphertext(prod);
  Free_value_list(f2);
  Free_value_list(degs);
  Free_ciph_value_list(t_list);
  Free_ciph_value_list(t2_list);
}
