//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "hal/hal.h"

#include "common/rtlib_timing.h"
#include "context/ckks_context.h"
#include "util/modular.h"

int64_t* Hw_modadd(int64_t* res, int64_t* val1, int64_t* val2, MODULUS* modulus,
                   uint32_t degree) {
  RTLIB_TM_START(RTM_HW_ADD, rtm);
  int64_t mod = Get_mod_val(modulus);
  for (uint32_t idx = 0; idx < degree; idx++) {
    *res = Add_int64_with_mod(*val1, *val2, mod);
    val1++;
    val2++;
    res++;
  }
  RTLIB_TM_END(RTM_HW_ADD, rtm);
  return res;
}

int64_t* Hw_modadd_scalar(int64_t* res, int64_t* val1, int64_t scalar,
                          MODULUS* modulus, uint32_t degree) {
  RTLIB_TM_START(RTM_HW_ADD, rtm);
  int64_t mod = Get_mod_val(modulus);
  for (uint32_t idx = 0; idx < degree; idx++) {
    *res = Add_int64_with_mod(*val1, scalar, mod);
    val1++;
    res++;
  }
  RTLIB_TM_END(RTM_HW_ADD, rtm);
  return res;
}

int64_t* Hw_modsub(int64_t* res, int64_t* val1, int64_t* val2, MODULUS* modulus,
                   uint32_t degree) {
  RTLIB_TM_START(RTM_HW_SUB, rtm);
  int64_t mod = Get_mod_val(modulus);
  for (uint32_t idx = 0; idx < degree; idx++) {
    *res = Sub_int64_with_mod(*val1, *val2, mod);
    val1++;
    val2++;
    res++;
  }
  RTLIB_TM_END(RTM_HW_SUB, rtm);
  return res;
}

int64_t* Hw_modsub_scalar(int64_t* res, int64_t* val, int64_t scalar,
                          MODULUS* modulus, uint32_t degree) {
  RTLIB_TM_START(RTM_HW_SUB, rtm);
  int64_t mod = Get_mod_val(modulus);
  for (uint32_t idx = 0; idx < degree; idx++) {
    *res = Sub_int64_with_mod(*val, scalar, mod);
    val++;
    res++;
  }
  RTLIB_TM_END(RTM_HW_SUB, rtm);
  return res;
}

int64_t* Hw_modmul(int64_t* res, int64_t* val1, int64_t* val2, MODULUS* modulus,
                   uint32_t degree) {
  RTLIB_TM_START(RTM_HW_MUL, rtm);
  for (uint32_t idx = 0; idx < degree; idx++) {
    *res = Mul_int64_mod_barret(*val1, *val2, modulus);
    val1++;
    val2++;
    res++;
  }
  RTLIB_TM_END(RTM_HW_MUL, rtm);
  return res;
}

int64_t* Hw_modmul_scalar(int64_t* res, int64_t* val, int64_t scalar,
                          MODULUS* modulus, uint32_t degree) {
  RTLIB_TM_START(RTM_HW_MUL, rtm);
  for (uint32_t idx = 0; idx < degree; idx++) {
    *res = Mul_int64_mod_barret(*val, scalar, modulus);
    val++;
    res++;
  }
  RTLIB_TM_END(RTM_HW_MUL, rtm);
  return res;
}

int64_t* Hw_modmuladd(int64_t* res, int64_t* val1, int64_t* val2,
                      MODULUS* modulus, uint32_t degree) {
  RTLIB_TM_START(RTM_HW_MAC, rtm);
  int64_t mod = Get_mod_val(modulus);
  for (uint32_t idx = 0; idx < degree; idx++) {
    int64_t tmp = Mul_int64_mod_barret(*val1, *val2, modulus);
    *res        = Add_int64_with_mod(*res, tmp, mod);
    val1++;
    val2++;
    res++;
  }
  RTLIB_TM_END(RTM_HW_MAC, rtm);
  return res;
}

int64_t* Hw_modmuladd_scalar(int64_t* res, int64_t* val, int64_t scalar,
                             MODULUS* modulus, uint32_t degree) {
  RTLIB_TM_START(RTM_HW_MAC, rtm);
  int64_t mod = Get_mod_val(modulus);
  for (uint32_t idx = 0; idx < degree; idx++) {
    int64_t tmp = Mul_int64_mod_barret(*val, scalar, modulus);
    *res        = Add_int64_with_mod(*res, tmp, mod);
    val++;
    res++;
  }
  RTLIB_TM_END(RTM_HW_MAC, rtm);
  return res;
}

int64_t* Hw_rotate(int64_t* res, int64_t* val, int64_t* rot_precomp,
                   MODULUS* modulus, uint32_t degree) {
  RTLIB_TM_START(RTM_HW_ROT, rtm);
  int64_t mod = Get_mod_val(modulus);
  for (uint32_t poly_idx = 0; poly_idx < degree; poly_idx++) {
    int64_t map_idx = rot_precomp[poly_idx];
    if (map_idx >= 0) {
      *res = val[map_idx];
    } else {
      *res = mod - val[-map_idx];
    }
    res++;
  }
  RTLIB_TM_END(RTM_HW_ROT, rtm);
  return res;
}

void Hw_ntt(int64_t* res, int64_t* val, size_t idx, uint32_t degree) {
  VALUE_LIST tmp_ntt;
  VALUE_LIST res_ntt;
  Init_i64_value_list_no_copy(&tmp_ntt, degree, val);
  Init_i64_value_list_no_copy(&res_ntt, degree, res);
  Ftt_fwd(&res_ntt, Get_ntt_ctx(idx), &tmp_ntt);
}

void Hw_intt(int64_t* res, int64_t* val, size_t idx, uint32_t degree) {
  VALUE_LIST tmp_ntt;
  VALUE_LIST res_ntt;
  Init_i64_value_list_no_copy(&tmp_ntt, degree, val);
  Init_i64_value_list_no_copy(&res_ntt, degree, res);
  Ftt_inv(&res_ntt, Get_ntt_ctx(idx), &tmp_ntt);
}