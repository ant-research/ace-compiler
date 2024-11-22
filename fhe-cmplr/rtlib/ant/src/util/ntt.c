//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "util/ntt.h"

#include "common/rtlib_timing.h"
#include "common/trace.h"
#include "util/bit_operations.h"
#include "util/number_theory.h"

void Transform_to_rev(VALUE_LIST* input, NTT_CONTEXT* ntt, VALUE_LIST* rou);
void Transform_from_rev(VALUE_LIST* input, NTT_CONTEXT* ntt, VALUE_LIST* rou);
void Forward_transform(VALUE_LIST* res, NTT_CONTEXT* ntt, VALUE_LIST* rou);
void Inverse_transform(VALUE_LIST* res, NTT_CONTEXT* ntt, VALUE_LIST* rou_inv);

NTT_CONTEXT* Alloc_nttcontext() {
  NTT_CONTEXT* ntt = (NTT_CONTEXT*)malloc(sizeof(NTT_CONTEXT));
  memset(ntt, 0, sizeof(NTT_CONTEXT));
  return ntt;
}

void Init_nttcontext(NTT_CONTEXT* ntt, uint32_t poly_degree,
                     MODULUS* coeff_modulus) {
  // Polynomial degree must be a power of 2. d is not.
  IS_TRUE((poly_degree & (poly_degree - 1)) == 0,
          "Polynomial degree must be a power of 2. d");

  ntt->_coeff_modulus = coeff_modulus;
  ntt->_degree        = poly_degree;
  ntt->_degree_inv    = Mod_inv_prime(ntt->_degree, ntt->_coeff_modulus);
  ntt->_degree_inv_prec =
      Precompute_const(ntt->_degree_inv, Get_mod_val(coeff_modulus));
  ntt->_scaled_rou_inv = NULL;

  // We use the (2d)-th root of unity, since d of these are roots of x^d + 1,
  // which can be used to uniquely identify any polynomial mod x^d + 1 from the
  // CRT representation of x^d + 1.
  int64_t root = Root_of_unity(2 * poly_degree, coeff_modulus);
  Precompute_ntt(ntt, root);
}

void Free_ntt_members(NTT_CONTEXT* ntt) {
  if (ntt == NULL) return;
  if (ntt->_rou) {
    Free_value_list(ntt->_rou);
    ntt->_rou = NULL;
  }
  if (ntt->_rou_inv) {
    Free_value_list(ntt->_rou_inv);
    ntt->_rou_inv = NULL;
  }
  if (ntt->_scaled_rou_inv) {
    Free_value_list(ntt->_scaled_rou_inv);
    ntt->_scaled_rou_inv = NULL;
  }
  if (ntt->_reversed_bits) {
    Free_value_list(ntt->_reversed_bits);
    ntt->_reversed_bits = NULL;
  }
  if (ntt->_rou_prec) {
    Free_value_list(ntt->_rou_prec);
    ntt->_rou_prec = NULL;
  }
  if (ntt->_rou_inv_prec) {
    Free_value_list(ntt->_rou_inv_prec);
    ntt->_rou_inv_prec = NULL;
  }
}

void Free_nttcontext(NTT_CONTEXT* ntt) {
  Free_ntt_members(ntt);
  free(ntt);
}

void Precompute_ntt(NTT_CONTEXT* ntt, int64_t root_of_unity) {
  uint32_t degree = ntt->_degree;

  // Compute precomputed array of reversed bits for iterated NTT.
  ntt->_reversed_bits = Alloc_value_list(I64_TYPE, degree);
  size_t width        = log2(degree);
  for (uint32_t i = 0; i < degree; i++) {
    Set_i64_value(ntt->_reversed_bits, i, Reverse_bits(i, width) % degree);
  }

  // Find powers of root of unity.
  ntt->_rou = Alloc_value_list(I64_TYPE, degree);
  Set_i64_value(ntt->_rou, 0, 1);
  int64_t power = root_of_unity;
  for (uint32_t i = 1; i < degree; i++) {
    Set_i64_value(ntt->_rou, Get_i64_value_at(ntt->_reversed_bits, i), power);
    power = Mul_int64_mod_barret(power, root_of_unity, ntt->_coeff_modulus);
  }

  // Find powers of inverse root of unity.
  int64_t root_of_unity_inv = Mod_inv_prime(root_of_unity, ntt->_coeff_modulus);
  ntt->_rou_inv             = Alloc_value_list(I64_TYPE, degree);
  Set_i64_value(ntt->_rou_inv, 0, 1);
  power = root_of_unity_inv;
  for (uint32_t i = 1; i < degree; i++) {
#ifdef SEAL_NTT
    Set_i64_value(ntt->_rou_inv,
                  Get_i64_value_at(ntt->_reversed_bits, i - 1) + 1, power);
#else
    Set_i64_value(ntt->_rou_inv, Get_i64_value_at(ntt->_reversed_bits, i),
                  power);
#endif
    power = Mul_int64_mod_barret(power, root_of_unity_inv, ntt->_coeff_modulus);
  }

  // precomputed const for root_of_unity
  VALUE_LIST* rou_prec     = Alloc_value_list(UI64_TYPE, degree);
  VALUE_LIST* rou_inv_prec = Alloc_value_list(UI64_TYPE, degree);
  ntt->_rou_prec           = rou_prec;
  ntt->_rou_inv_prec       = rou_inv_prec;
  int64_t mod_val          = Get_mod_val(ntt->_coeff_modulus);
  for (uint32_t i = 0; i < degree; i++) {
    int64_t rou     = Get_i64_value_at(ntt->_rou, i);
    int64_t rou_inv = Get_i64_value_at(ntt->_rou_inv, i);
    Set_ui64_value(rou_prec, i, Precompute_const(rou, mod_val));
    Set_ui64_value(rou_inv_prec, i, Precompute_const(rou_inv, mod_val));
  }
}

void Run_ntt(VALUE_LIST* res, NTT_CONTEXT* ntt, VALUE_LIST* coeffs,
             VALUE_LIST* rou) {
  size_t coeffs_len = LIST_LEN(coeffs);
  IS_TRUE(LIST_LEN(rou) == coeffs_len,
          "Length of roots of unit not match with coeffs length");

  Bit_reverse_vec(Get_i64_values(res), Get_i64_values(coeffs), coeffs_len);
  size_t log_num_coeffs = log2(coeffs_len);

  int64_t  coeff_mod = Get_mod_val(ntt->_coeff_modulus);
  int64_t* rous      = Get_i64_values(rou);
  int64_t* res_data  = Get_i64_values(res);
  for (size_t logm = 1; logm < log_num_coeffs + 1; logm++) {
    for (size_t j = 0; j < coeffs_len; j = j + (1 << logm)) {
      size_t num = 1 << (logm - 1);
      for (size_t i = 0; i < num; i++) {
        size_t index_even = j + i;
        size_t index_odd  = index_even + num;

        size_t  rou_idx      = (i << (1 + log_num_coeffs - logm));
        int64_t omega_factor = Mul_int64_mod_barret(
            rous[rou_idx], res_data[index_odd], ntt->_coeff_modulus);
        int64_t butterfly_plus =
            Add_int64_with_mod(res_data[index_even], omega_factor, coeff_mod);
        int64_t butterfly_minus =
            Sub_int64_with_mod(res_data[index_even], omega_factor, coeff_mod);

        res_data[index_even] = butterfly_plus;
        res_data[index_odd]  = butterfly_minus;
      }
    }
  }
}

void Ftt_fwd(VALUE_LIST* res, NTT_CONTEXT* ntt, VALUE_LIST* coeffs) {
  //  Ftt_fwd: input length does not match context degree
  RTLIB_TM_START(RTM_NTT, rtm);
  size_t coeffs_len = LIST_LEN(coeffs);
  IS_TRUE(coeffs_len == ntt->_degree,
          "coeffs length does not match context degree");
  // fast forward ntt from openfhe lib
  if (res != coeffs) {
    Init_i64_value_list(res, coeffs_len, Get_i64_values(coeffs));
  }
  Forward_transform(res, ntt, ntt->_rou);
  RTLIB_TM_END(RTM_NTT, rtm);
}

void Ftt_inv(VALUE_LIST* res, NTT_CONTEXT* ntt, VALUE_LIST* coeffs) {
  RTLIB_TM_START(RTM_INTT, rtm);
  size_t coeffs_len = LIST_LEN(coeffs);
  IS_TRUE(coeffs_len == ntt->_degree,
          "input length does not match context degree");
  if (res != coeffs) {
    Init_i64_value_list(res, coeffs_len, Get_i64_values(coeffs));
  }
  Inverse_transform(res, ntt, ntt->_rou_inv);
  RTLIB_TM_END(RTM_INTT, rtm);
}

// forward NTT implementation from OpenFHE lib, faster than seal
void Forward_transform(VALUE_LIST* res, NTT_CONTEXT* ntt, VALUE_LIST* rou) {
  IS_TRUE(LIST_LEN(rou) == LIST_LEN(res),
          "Length of roots of unit not match with coeffs length");

  int64_t   coeff_mod = Get_mod_val(ntt->_coeff_modulus);
  int64_t*  rous      = Get_i64_values(rou);
  uint64_t* rous_prec = Get_ui64_values(ntt->_rou_prec);
  int64_t*  res_data  = Get_i64_values(res);

  uint32_t n    = LIST_LEN(res) >> 1;
  uint32_t t    = n;
  uint32_t logt = log2(t) + 1;

  uint32_t index_omega, index_hi;
  size_t   omega, omega_factor, lo_val, hi_val;
  uint64_t omega_prec;
  for (uint32_t m = 1; m < n; m <<= 1, t >>= 1, --logt) {
    for (uint32_t i = 0; i < m; ++i) {
      omega      = rous[i + m];
      omega_prec = rous_prec[i + m];
      for (uint32_t j1 = (i << logt), j2 = j1 + t; j1 < j2; ++j1) {
        omega_factor = res_data[j1 + t];
        omega_factor =
            Fast_mul_const_with_mod(omega_factor, omega, omega_prec, coeff_mod);
        lo_val = res_data[j1];
#if defined(__GNUC__) && !defined(__clang__)
        hi_val = lo_val + omega_factor;
        if (hi_val >= coeff_mod) {
          hi_val -= coeff_mod;
        }
        if (lo_val < omega_factor) {
          lo_val += coeff_mod;
        }
        lo_val -= omega_factor;
        res_data[j1]     = hi_val;
        res_data[j1 + t] = lo_val;
#else
        res_data[j1] += omega_factor -
                        (omega_factor >= (coeff_mod - lo_val) ? coeff_mod : 0);
        if (omega_factor > lo_val) {
          lo_val += coeff_mod;
        }
        res_data[j1 + t] = lo_val - omega_factor;
#endif
      }
    }
  }
  for (uint32_t i = 0; i < (n << 1); i += 2) {
    omega_factor = res_data[i + 1];
    omega        = rous[(i >> 1) + n];
    omega_prec   = rous_prec[(i >> 1) + n];
    omega_factor =
        Fast_mul_const_with_mod(omega_factor, omega, omega_prec, coeff_mod);
    lo_val = res_data[i];
#if defined(__GNUC__) && !defined(__clang__)
    hi_val = lo_val + omega_factor;
    if (hi_val >= coeff_mod) {
      hi_val -= coeff_mod;
    }
    if (lo_val < omega_factor) {
      lo_val += coeff_mod;
    }
    lo_val -= omega_factor;
    res_data[i]     = hi_val;
    res_data[i + 1] = lo_val;
#else
    res_data[i] +=
        omega_factor - (omega_factor >= (coeff_mod - lo_val) ? coeff_mod : 0);
    if (omega_factor > lo_val) {
      lo_val += coeff_mod;
    }
    res_data[i + t] = lo_val - omega_factor;
#endif
  }
}

// Inverse NTT conversion from OpenFHE lib InverseTransformFromBitReverseInPlace
// optimized version
void Inverse_transform(VALUE_LIST* res, NTT_CONTEXT* ntt, VALUE_LIST* rou_inv) {
  size_t n = LIST_LEN(res);
  IS_TRUE(LIST_LEN(rou_inv) == n,
          "Length of inverse roots of unit not match with coeffs length");

  int64_t   coeff_mod       = Get_mod_val(ntt->_coeff_modulus);
  int64_t*  rous_inv        = Get_i64_values(rou_inv);
  uint64_t* rous_inv_prec   = Get_ui64_values(ntt->_rou_inv_prec);
  int64_t*  res_data        = Get_i64_values(res);
  uint64_t  degree_inv      = ntt->_degree_inv;
  uint64_t  degree_inv_prec = ntt->_degree_inv_prec;

  int64_t  lo_val, hi_val, omega, omega_factor;
  uint64_t omega_prec;
  for (uint32_t i = 0; i < n; i += 2) {
    omega      = rous_inv[(i + n) >> 1];
    omega_prec = rous_inv_prec[(i + n) >> 1];
    hi_val     = res_data[i + 1];
    lo_val     = res_data[i];
#if defined(__GNUC__) && !defined(__clang__)
    omega_factor = lo_val;
    if (omega_factor < hi_val) {
      omega_factor += coeff_mod;
    }
    omega_factor -= hi_val;
    lo_val += hi_val;
    if (lo_val >= coeff_mod) {
      lo_val -= coeff_mod;
    }
    lo_val =
        Fast_mul_const_with_mod(lo_val, degree_inv, degree_inv_prec, coeff_mod);
    omega_factor =
        Fast_mul_const_with_mod(omega_factor, omega, omega_prec, coeff_mod);
    omega_factor    = Fast_mul_const_with_mod(omega_factor, degree_inv,
                                              degree_inv_prec, coeff_mod);
    res_data[i]     = lo_val;
    res_data[i + 1] = omega_factor;
#else
    omega_factor    = lo_val + (hi_val > lo_val ? coeff_mod : 0) - hi_val;
    lo_val += hi_val - (hi_val >= (coeff_mod - lo_val) ? coeff_mod : 0);
    lo_val =
        Fast_mul_const_with_mod(lo_val, degree_inv, degree_inv_prec, coeff_mod);
    res_data[i] = lo_val;
    omega_factor =
        Fast_mul_const_with_mod(omega_factor, omega, omega_prec, coeff_mod);
    omega_factor    = Fast_mul_const_with_mod(omega_factor, degree_inv,
                                              degree_inv_prec, coeff_mod);
    res_data[i + 1] = omega_factor;
#endif
  }

  uint32_t t    = 2;
  uint32_t logt = 2;
  for (uint32_t m = n >> 2; m >= 1; m >>= 1, t <<= 1, ++logt) {
    for (uint32_t i = 0; i < m; ++i) {
      omega      = rous_inv[i + m];
      omega_prec = rous_inv_prec[i + m];
      for (uint32_t j1 = i << logt, j2 = j1 + t; j1 < j2; ++j1) {
        hi_val = res_data[j1 + t];
        lo_val = res_data[j1];
#if defined(__GNUC__) && !defined(__clang__)
        omega_factor = lo_val;
        if (omega_factor < hi_val) {
          omega_factor += coeff_mod;
        }
        omega_factor -= hi_val;
        lo_val += hi_val;
        if (lo_val >= coeff_mod) {
          lo_val -= coeff_mod;
        }
        omega_factor =
            Fast_mul_const_with_mod(omega_factor, omega, omega_prec, coeff_mod);
        res_data[j1 + 0] = lo_val;
        res_data[j1 + t] = omega_factor;
#else
        res_data[j1 + 0] +=
            hi_val - (hi_val >= (coeff_mod - lo_val) ? coeff_mod : 0);
        omega_factor = lo_val + (hi_val > lo_val ? coeff_mod : 0) - hi_val;
        omega_factor =
            Fast_mul_const_with_mod(omega_factor, omega, omega_prec, coeff_mod);
        res_data[j1 + t] = omega_factor;
#endif
      }
    }
  }
}

#ifdef SEAL_NTT
// Seal equivalent NTT implementation
void Transform_to_rev(VALUE_LIST* input, NTT_CONTEXT* ntt, VALUE_LIST* rou) {
  size_t n = LIST_LEN(input);
  // registers to hold temporary values
  int64_t  r;
  uint64_t r_prec;
  int64_t  u;
  int64_t  v;
  // pointers for faster indexing
  int64_t* x = NULL;
  int64_t* y = NULL;
  // variables for indexing
  size_t    gap        = n >> 1;
  size_t    m          = 1;
  int64_t*  values     = Get_i64_values(input);
  int64_t*  roots      = Get_i64_values(rou);
  uint64_t* roots_prec = Get_ui64_values(ntt->_rou_prec);
  int64_t   mod        = Get_mod_val(ntt->_coeff_modulus);

  for (; m < (n >> 1); m <<= 1) {
    size_t offset = 0;
    if (gap < 4) {
      for (size_t i = 0; i < m; i++) {
        r      = *++roots;
        r_prec = *++roots_prec;
        x      = values + offset;
        y      = x + gap;
        for (size_t j = 0; j < gap; j++) {
          u    = *x;
          v    = Fast_mul_const_with_mod(*y, r, r_prec, mod);
          *x++ = Add_int64_with_mod(u, v, mod);
          *y++ = Sub_int64_with_mod(u, v, mod);
        }
        offset += gap << 1;
      }
    } else {
      for (size_t i = 0; i < m; i++) {
        r      = *++roots;
        r_prec = *++roots_prec;
        x      = values + offset;
        y      = x + gap;
        for (size_t j = 0; j < gap; j += 4) {
          u    = *x;
          v    = Fast_mul_const_with_mod(*y, r, r_prec, mod);
          *x++ = Add_int64_with_mod(u, v, mod);
          *y++ = Sub_int64_with_mod(u, v, mod);

          u    = *x;
          v    = Fast_mul_const_with_mod(*y, r, r_prec, mod);
          *x++ = Add_int64_with_mod(u, v, mod);
          *y++ = Sub_int64_with_mod(u, v, mod);

          u    = *x;
          v    = Fast_mul_const_with_mod(*y, r, r_prec, mod);
          *x++ = Add_int64_with_mod(u, v, mod);
          *y++ = Sub_int64_with_mod(u, v, mod);

          u    = *x;
          v    = Fast_mul_const_with_mod(*y, r, r_prec, mod);
          *x++ = Add_int64_with_mod(u, v, mod);
          *y++ = Sub_int64_with_mod(u, v, mod);
        }
        offset += gap << 1;
      }
    }
    gap >>= 1;
  }

  for (size_t i = 0; i < m; i++) {
    r         = *++roots;
    r_prec    = *++roots_prec;
    u         = values[0];
    v         = Fast_mul_const_with_mod(values[1], r, r_prec, mod);
    values[0] = Add_int64_with_mod(u, v, mod);
    values[1] = Sub_int64_with_mod(u, v, mod);
    values += 2;
  }
}

// Seal equivalent INTT implementation
void Transform_from_rev(VALUE_LIST* input, NTT_CONTEXT* ntt, VALUE_LIST* rou) {
  size_t n = LIST_LEN(input);
  // registers to hold temporary values
  int64_t  r;
  uint64_t r_inv_prec;
  int64_t  u;
  int64_t  v;
  // pointers for faster indexing
  int64_t* x = NULL;
  int64_t* y = NULL;
  // variables for indexing
  size_t    gap             = 1;
  size_t    m               = n >> 1;
  int64_t*  values          = Get_i64_values(input);
  int64_t*  roots           = Get_i64_values(rou);
  int64_t   mod             = Get_mod_val(ntt->_coeff_modulus);
  uint64_t* roots_prec      = Get_ui64_values(ntt->_rou_inv_prec);
  uint64_t  degree_inv_prec = ntt->_degree_inv_prec;
  for (; m > 1; m >>= 1) {
    size_t offset = 0;
    if (gap < 4) {
      for (size_t i = 0; i < m; i++) {
        r          = *++roots;
        r_inv_prec = *++roots_prec;
        x          = values + offset;
        y          = x + gap;
        for (size_t j = 0; j < gap; j++) {
          u    = *x;
          v    = *y;
          *x++ = Add_int64_with_mod(u, v, mod);
          *y++ = Fast_mul_const_with_mod(Sub_int64_with_mod(u, v, mod), r,
                                         r_inv_prec, mod);
        }
        offset += gap << 1;
      }
    } else {
      for (size_t i = 0; i < m; i++) {
        r          = *++roots;
        r_inv_prec = *++roots_prec;
        x          = values + offset;
        y          = x + gap;
        for (size_t j = 0; j < gap; j += 4) {
          u    = *x;
          v    = *y;
          *x++ = Add_int64_with_mod(u, v, mod);
          *y++ = Fast_mul_const_with_mod(Sub_int64_with_mod(u, v, mod), r,
                                         r_inv_prec, mod);

          u    = *x;
          v    = *y;
          *x++ = Add_int64_with_mod(u, v, mod);
          *y++ = Fast_mul_const_with_mod(Sub_int64_with_mod(u, v, mod), r,
                                         r_inv_prec, mod);

          u    = *x;
          v    = *y;
          *x++ = Add_int64_with_mod(u, v, mod);
          *y++ = Fast_mul_const_with_mod(Sub_int64_with_mod(u, v, mod), r,
                                         r_inv_prec, mod);

          u    = *x;
          v    = *y;
          *x++ = Add_int64_with_mod(u, v, mod);
          *y++ = Fast_mul_const_with_mod(Sub_int64_with_mod(u, v, mod), r,
                                         r_inv_prec, mod);
        }
        offset += gap << 1;
      }
    }
    gap <<= 1;
  }
  r          = *++roots;
  r_inv_prec = *++roots_prec;
  int64_t scaled_r =
      Fast_mul_const_with_mod(r, ntt->_degree_inv, degree_inv_prec, mod);
  uint64_t scaled_r_prec = Precompute_const(scaled_r, mod);
  x                      = values;
  y                      = x + gap;
  if (gap < 4) {
    for (size_t j = 0; j < gap; j++) {
      u    = *x;
      v    = *y;
      *x++ = Fast_mul_const_with_mod(Add_int64_with_mod(u, v, mod),
                                     ntt->_degree_inv, degree_inv_prec, mod);
      *y++ = Fast_mul_const_with_mod(Sub_int64_with_mod(u, v, mod), scaled_r,
                                     scaled_r_prec, mod);
    }
  } else {
    for (size_t j = 0; j < gap; j += 4) {
      u    = *x;
      v    = *y;
      *x++ = Fast_mul_const_with_mod(Add_int64_with_mod(u, v, mod),
                                     ntt->_degree_inv, degree_inv_prec, mod);
      *y++ = Fast_mul_const_with_mod(Sub_int64_with_mod(u, v, mod), scaled_r,
                                     scaled_r_prec, mod);

      u    = *x;
      v    = *y;
      *x++ = Fast_mul_const_with_mod(Add_int64_with_mod(u, v, mod),
                                     ntt->_degree_inv, degree_inv_prec, mod);
      *y++ = Fast_mul_const_with_mod(Sub_int64_with_mod(u, v, mod), scaled_r,
                                     scaled_r_prec, mod);

      u    = *x;
      v    = *y;
      *x++ = Fast_mul_const_with_mod(Add_int64_with_mod(u, v, mod),
                                     ntt->_degree_inv, degree_inv_prec, mod);
      *y++ = Fast_mul_const_with_mod(Sub_int64_with_mod(u, v, mod), scaled_r,
                                     scaled_r_prec, mod);

      u    = *x;
      v    = *y;
      *x++ = Fast_mul_const_with_mod(Add_int64_with_mod(u, v, mod),
                                     ntt->_degree_inv, degree_inv_prec, mod);
      *y++ = Fast_mul_const_with_mod(Sub_int64_with_mod(u, v, mod), scaled_r,
                                     scaled_r_prec, mod);
    }
  }
}
#endif

FFT_CONTEXT* Alloc_fftcontext(size_t fft_length) {
  FFT_CONTEXT* fft = (FFT_CONTEXT*)malloc(sizeof(FFT_CONTEXT));
  fft->_fft_length = fft_length;
  Precompute_fft(fft);
  return fft;
}

void Free_fftcontext(FFT_CONTEXT* fft) {
  if (fft == NULL) return;
  if (fft->_rou) {
    Free_value_list(fft->_rou);
    fft->_rou = NULL;
  }
  if (fft->_rou_inv) {
    Free_value_list(fft->_rou_inv);
    fft->_rou_inv = NULL;
  }
  if (fft->_rot_group) {
    Free_value_list(fft->_rot_group);
    fft->_rot_group = NULL;
  }
  if (fft->_reversed_bits) {
    Free_value_list(fft->_reversed_bits);
    fft->_reversed_bits = NULL;
  }
  free(fft);
}

void Precompute_fft(FFT_CONTEXT* fft) {
  size_t len    = fft->_fft_length;
  fft->_rou     = Alloc_value_list(DCMPLX_TYPE, len);
  fft->_rou_inv = Alloc_value_list(DCMPLX_TYPE, len);
  for (size_t i = 0; i < len; i++) {
    double angle                      = 2 * M_PI * i / fft->_fft_length;
    DCMPLX_VALUE_AT(fft->_rou, i)     = cos(angle) + sin(angle) * I;
    DCMPLX_VALUE_AT(fft->_rou_inv, i) = cos(-angle) + sin(-angle) * I;
  }

  // Compute precomputed array of reversed bits for iterated FFT.
  size_t num_slots    = fft->_fft_length / 4;
  fft->_reversed_bits = Alloc_value_list(I64_TYPE, num_slots);
  size_t width        = log2(num_slots);
  for (size_t i = 0; i < num_slots; i++) {
    I64_VALUE_AT(fft->_reversed_bits, i) = Reverse_bits(i, width) % num_slots;
  }

  // Compute rotation group for EMB with powers of 5.
  fft->_rot_group                  = Alloc_value_list(I64_TYPE, num_slots);
  I64_VALUE_AT(fft->_rot_group, 0) = 1;
  for (size_t i = 1; i < num_slots; i++) {
    I64_VALUE_AT(fft->_rot_group, i) =
        (5 * I64_VALUE_AT(fft->_rot_group, i - 1)) % fft->_fft_length;
  }
}

void Run_fft(VALUE_LIST* res, FFT_CONTEXT* fft, VALUE_LIST* coeffs,
             VALUE_LIST* rou) {
  size_t coeffs_len = LIST_LEN(coeffs);
  IS_TRUE(LIST_LEN(rou) >= coeffs_len,
          "input length does not match context degree");
  IS_TRUE(LIST_TYPE(res) == DCMPLX_TYPE && LIST_TYPE(coeffs) == DCMPLX_TYPE &&
              LIST_TYPE(rou) == DCMPLX_TYPE,
          "type of input & output valuelist should be DCMPLX");

  Bit_reverse_vec_for_complex(DCMPLX_VALUES(res), DCMPLX_VALUES(coeffs),
                              coeffs_len);

  size_t log_num_coeffs = log2(coeffs_len);

  for (size_t logm = 1; logm < log_num_coeffs + 1; logm++) {
    for (size_t j = 0; j < coeffs_len; j = j + (1 << logm)) {
      size_t num = 1 << (logm - 1);
      for (size_t i = 0; i < num; i++) {
        size_t index_even = j + i;
        size_t index_odd  = j + i + num;

        size_t rou_idx = (i * fft->_fft_length) >> logm;
        DCMPLX omega_factor =
            DCMPLX_VALUE_AT(rou, rou_idx) * DCMPLX_VALUE_AT(res, index_odd);

        DCMPLX butterfly_plus = DCMPLX_VALUE_AT(res, index_even) + omega_factor;
        DCMPLX butterfly_minus =
            DCMPLX_VALUE_AT(res, index_even) - omega_factor;

        DCMPLX_VALUE_AT(res, index_even) = butterfly_plus;
        DCMPLX_VALUE_AT(res, index_odd)  = butterfly_minus;
      }
    }
  }
}

void Fft_fwd(VALUE_LIST* res, FFT_CONTEXT* fft, VALUE_LIST* coeffs) {
  IS_TRUE(LIST_TYPE(res) == DCMPLX_TYPE && LIST_TYPE(coeffs) == DCMPLX_TYPE,
          "type of input & output valuelist should be DCMPLX");
  IS_TRUE(LIST_LEN(res) == LIST_LEN(coeffs), "size not match");

  Run_fft(res, fft, coeffs, fft->_rou);
}

void Fft_inv(VALUE_LIST* res, FFT_CONTEXT* fft, VALUE_LIST* coeffs) {
  IS_TRUE(LIST_TYPE(res) == DCMPLX_TYPE && LIST_TYPE(coeffs) == DCMPLX_TYPE,
          "type of input & output valuelist should be DCMPLX");
  IS_TRUE(LIST_LEN(res) == LIST_LEN(coeffs), "size not match");

  Run_fft(res, fft, coeffs, fft->_rou_inv);

  size_t coeffs_len = LIST_LEN(coeffs);
  for (size_t i = 0; i < coeffs_len; i++) {
    DCMPLX_VALUE_AT(res, i) /= coeffs_len;
  }
}

void Check_embedding_input(FFT_CONTEXT* fft, VALUE_LIST* values) {
  // Input vector must have length at most fft->_fft_length / 4
  IS_TRUE(LIST_LEN(values) <= fft->_fft_length / 4,
          "Input vector must have length at most fft->_fft_length / 4");
  // length of 2 * values must be a power of 2.
  IS_TRUE((2 * LIST_LEN(values) & (2 * LIST_LEN(values) - 1)) == 0,
          "length of 2 * values must be a power of 2.");
}

void Embedding(VALUE_LIST* res, FFT_CONTEXT* fft, VALUE_LIST* coeffs) {
  size_t coeffs_len = LIST_LEN(coeffs);
  IS_TRUE(LIST_LEN(res) == coeffs_len, "size not match");

  Check_embedding_input(fft, coeffs);

  Bit_reverse_vec_for_complex(DCMPLX_VALUES(res), DCMPLX_VALUES(coeffs),
                              coeffs_len);

  size_t log_num_coeffs = log2(coeffs_len);

  for (size_t logm = 1; logm < log_num_coeffs + 1; logm++) {
    size_t idx_mod = 1 << (logm + 2);
    size_t gap     = fft->_fft_length / idx_mod;
    for (size_t j = 0; j < coeffs_len; j = j + (1 << logm)) {
      size_t num = 1 << (logm - 1);
      for (size_t i = 0; i < num; i++) {
        size_t index_even = j + i;
        size_t index_odd  = j + i + num;

        size_t rou_idx = (I64_VALUE_AT(fft->_rot_group, i) % idx_mod) * gap;
        DCMPLX omega_factor = DCMPLX_VALUE_AT(fft->_rou, rou_idx) *
                              DCMPLX_VALUE_AT(res, index_odd);

        DCMPLX butterfly_plus = DCMPLX_VALUE_AT(res, index_even) + omega_factor;
        DCMPLX butterfly_minus =
            DCMPLX_VALUE_AT(res, index_even) - omega_factor;

        DCMPLX_VALUE_AT(res, index_even) = butterfly_plus;
        DCMPLX_VALUE_AT(res, index_odd)  = butterfly_minus;
      }
    }
  }
}

void Embedding_inv(VALUE_LIST* res, FFT_CONTEXT* fft, VALUE_LIST* coeffs) {
  size_t coeffs_len = LIST_LEN(coeffs);
  IS_TRUE(LIST_LEN(res) == coeffs_len, "size not match");

  Check_embedding_input(fft, coeffs);

  // copy from coeffs to result
  DCMPLX result[coeffs_len];
  memmove(result, DCMPLX_VALUES(coeffs), sizeof(DCMPLX) * coeffs_len);

  size_t log_num_coeffs = log2(coeffs_len);

  for (size_t logm = log_num_coeffs; logm > 0; logm--) {
    size_t idx_mod = 1 << (logm + 2);
    size_t gap     = fft->_fft_length / idx_mod;
    size_t num1    = 1 << logm;
    for (size_t j = 0; j < coeffs_len; j = j + num1) {
      size_t num2 = 1 << (logm - 1);
      for (size_t i = 0; i < num2; i++) {
        size_t index_even = j + i;
        size_t index_odd  = j + i + num2;

        size_t rou_idx =
            (idx_mod - (I64_VALUE_AT(fft->_rot_group, i) % idx_mod)) * gap;

        DCMPLX butterfly_plus  = result[index_even] + result[index_odd];
        DCMPLX butterfly_minus = result[index_even] - result[index_odd];
        butterfly_minus *= DCMPLX_VALUE_AT(fft->_rou, rou_idx);

        result[index_even] = butterfly_plus;
        result[index_odd]  = butterfly_minus;
      }
    }
  }

  Bit_reverse_vec_for_complex(DCMPLX_VALUES(res), result, coeffs_len);

  for (size_t i = 0; i < coeffs_len; i++) {
    DCMPLX_VALUE_AT(res, i) /= coeffs_len;
  }
}

void Print_ntt(FILE* fp, NTT_CONTEXT* ntt) {
  fprintf(fp, "\n    coeff_mod = %ld", Get_mod_val(ntt->_coeff_modulus));
  fprintf(fp, "    degree = %d\n", ntt->_degree);
  fprintf(fp, "    rous = ");
  Print_value_list(fp, ntt->_rou);
  fprintf(fp, "    rous_inv = ");
  Print_value_list(fp, ntt->_rou_inv);
  fprintf(fp, "    scaled_rou_inv = ");
  Print_value_list(fp, ntt->_scaled_rou_inv);
  fprintf(fp, "    reversed_bits = ");
  Print_value_list(fp, ntt->_reversed_bits);
  FOR_ALL_ELEM(ntt->_reversed_bits, idx) {
    if (Get_i64_value_at(ntt->_reversed_bits, idx) == 1) {
      fprintf(fp, "    rou = %ld\n", Get_i64_value_at(ntt->_rou, idx));
    }
  }
}

void Print_fft(FILE* fp, FFT_CONTEXT* fft) {
  fprintf(fp, "---------- Generate FFT ----------\n");
  fprintf(fp, "fft_length = %ld\n", fft->_fft_length);
  fprintf(fp, "rou = \n");
  Print_value_list(fp, fft->_rou);
  fprintf(fp, "rou_inv = \n");
  Print_value_list(fp, fft->_rou_inv);
  fprintf(fp, "rot_group = \n");
  Print_value_list(fp, fft->_rot_group);
  fprintf(fp, "reversed_bits = ");
  Print_value_list(fp, fft->_reversed_bits);
  fprintf(fp, "---------- End of FFT Dump ----------\n");
}
