//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "ckks/plain.h"

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "ckks/plaintext.h"
#include "common/rtlib_timing.h"
#include "context/ckks_context.h"
#include "fhe/core/rt_encode_api.h"
#include "fhe/core/rt_version.h"

void Encode_plain(PLAIN plain, VALUE_LIST* input_vec, uint32_t sc_degree,
                  uint32_t level) {
  Encode_at_level_with_sf(plain, (CKKS_ENCODER*)Context->_encoder, input_vec,
                          level, 0 /* slots */, sc_degree);
  Append_weight_plain((CKKS_ENCODER*)Context->_encoder,
                      Get_plain_mem_size(plain));
}

void Encode_float(PLAIN plain, float* input, size_t len, uint32_t sc_degree,
                  uint32_t level) {
  RTLIB_TM_START(RTM_PT_ENCODE, rtm);
  if (len == 1) {
    // Encode single value at given level
    Encode_val_at_level(plain, (CKKS_ENCODER*)Context->_encoder, (double)*input,
                        level, sc_degree);
    RTLIB_TM_END(RTM_PT_ENCODE, rtm);
    return;
  }
  VALUE_LIST* input_vec = Alloc_value_list(DCMPLX_TYPE, len);
  FOR_ALL_ELEM(input_vec, idx) {
    DCMPLX_VALUE_AT(input_vec, idx) = (double)input[idx];
  }
  Encode_plain(plain, input_vec, sc_degree, level);
  Free_value_list(input_vec);
  RTLIB_TM_END(RTM_PT_ENCODE, rtm);
}

void Encode_double(PLAIN plain, double* input, size_t len, uint32_t sc_degree,
                   uint32_t level) {
  RTLIB_TM_START(RTM_PT_ENCODE, rtm);
  if (len == 1) {
    // Encode single value at given level
    Encode_val_at_level(plain, (CKKS_ENCODER*)Context->_encoder, *input, level,
                        sc_degree);
    RTLIB_TM_END(RTM_PT_ENCODE, rtm);
    return;
  }
  VALUE_LIST* input_vec = Alloc_value_list(DCMPLX_TYPE, len);
  FOR_ALL_ELEM(input_vec, idx) { DCMPLX_VALUE_AT(input_vec, idx) = input[idx]; }
  Encode_plain(plain, input_vec, sc_degree, level);
  Free_value_list(input_vec);
  RTLIB_TM_END(RTM_PT_ENCODE, rtm);
}

void Encode_float_mask(PLAIN plain, float cst, size_t len, uint32_t sc_degree,
                       uint32_t level) {
  float* mask = malloc(len * sizeof(float));
  for (uint32_t id = 0; id < len; ++id) mask[id] = cst;
  Encode_float(plain, mask, len, sc_degree, level);
  free(mask);
}

void Encode_double_mask(PLAIN plain, double cst, size_t len, uint32_t sc_degree,
                        uint32_t level) {
  double* mask = malloc(len * sizeof(double));
  for (uint32_t id = 0; id < len; ++id) mask[id] = cst;
  Encode_double(plain, mask, len, sc_degree, level);
  free(mask);
}

void Encode_float_with_scale(PLAIN plain, float* input, size_t len,
                             double scale, uint32_t level) {
  RTLIB_TM_START(RTM_PT_ENCODE, rtm);
  if (len == 1) {
    // Encode single value at given level
    Encode_val_at_level_with_scale(plain, (CKKS_ENCODER*)Context->_encoder,
                                   (double)*input, level, scale);
    RTLIB_TM_END(RTM_PT_ENCODE, rtm);
    return;
  }
  VALUE_LIST* input_vec = Alloc_value_list(DCMPLX_TYPE, len);
  FOR_ALL_ELEM(input_vec, idx) {
    DCMPLX_VALUE_AT(input_vec, idx) = (double)input[idx];
  }
  Encode_at_level_with_scale(plain, (CKKS_ENCODER*)Context->_encoder, input_vec,
                             level, 0 /* default slots */, scale,
                             0 /* p_cnt */);
  Free_value_list(input_vec);
  RTLIB_TM_END(RTM_PT_ENCODE, rtm);
}

void Encode_single_float_with_scale(int64_t* res, float input, double scale,
                                    uint32_t level) {
  RTLIB_TM_START(RTM_PT_ENCODE, rtm);
  VALUE_LIST val;
  Init_i64_value_list_no_copy(&val, level, res);
  Encode_val(&val, (double)input, level, scale, 0 /* p_cnt */);
  RTLIB_TM_END(RTM_PT_ENCODE, rtm);
}

void Encode_single_float(int64_t* res, float input, uint32_t sc_degree,
                         uint32_t level) {
  Encode_single_double(res, (double)input, sc_degree, level);
}

void Encode_single_double(int64_t* res, double input, uint32_t sc_degree,
                          uint32_t level) {
  RTLIB_TM_START(RTM_PT_ENCODE, rtm);
  VALUE_LIST val;
  Init_i64_value_list_no_copy(&val, level, res);
  Encode_val(&val, input, level, pow(Get_default_sc(), sc_degree),
             0 /* p_cnt */);
  RTLIB_TM_END(RTM_PT_ENCODE, rtm);
}

double* Get_msg_from_plain(PLAIN plain) {
  VALUE_LIST* vec = Alloc_value_list(DCMPLX_TYPE, Get_plain_slots(plain));
  Decode(vec, (CKKS_ENCODER*)Context->_encoder, plain);
  double* data = (double*)malloc(LIST_LEN(vec) * sizeof(double));
  FOR_ALL_ELEM(vec, idx) { data[idx] = creal(Get_dcmplx_value_at(vec, idx)); }
  Free_value_list(vec);
  return data;
}

DCMPLX* Get_dcmplx_msg_from_plain(PLAIN plain) {
  VALUE_LIST* vec = Alloc_value_list(DCMPLX_TYPE, Get_plain_slots(plain));
  Decode(vec, (CKKS_ENCODER*)Context->_encoder, plain);
  DCMPLX* data = (DCMPLX*)malloc(LIST_LEN(vec) * sizeof(DCMPLX));
  FOR_ALL_ELEM(vec, idx) { data[idx] = Get_dcmplx_value_at(vec, idx); }
  Free_value_list(vec);
  return data;
}

static inline uint64_t Get_plaintext_length(uint32_t level) {
  CKKS_PARAMETER* param      = (CKKS_PARAMETER*)Param();
  uint32_t        degree     = param->_poly_degree;
  uint32_t        num_primes = (level > 0) ? level : param->_num_primes;
  return sizeof(PLAINTEXT) + sizeof(uint64_t) * degree * num_primes;
}

struct PLAINTEXT_BUFFER* Encode_plain_buffer(const float* input, size_t len,
                                             uint32_t sc_degree,
                                             uint32_t level) {
  size_t                   pt_len = Get_plaintext_length(level);
  struct PLAINTEXT_BUFFER* buf    = (struct PLAINTEXT_BUFFER*)malloc(
      sizeof(struct PLAINTEXT_BUFFER) + pt_len);
  IS_TRUE(buf != NULL, "Failed to malloc sizeofPLAINTEXT_BUFFER");
  memcpy(buf->_magic, PT_BUFFER_MAGIC, sizeof(buf->_magic));
  buf->_version = RT_VERSION_FULL;
  buf->_size    = pt_len;
  memset(buf->_data, 0, pt_len);
  CKKS_PARAMETER* param       = (CKKS_PARAMETER*)Param();
  PLAINTEXT*      pt          = (PLAINTEXT*)buf->_data;
  pt->_poly._ring_degree      = param->_poly_degree;
  pt->_poly._num_primes       = (level > 0) ? level : param->_num_primes;
  pt->_poly._num_alloc_primes = pt->_poly._num_primes;
  pt->_poly._data = (int64_t*)((char*)buf + sizeof(struct PLAINTEXT_BUFFER) +
                               sizeof(PLAINTEXT));
  Encode_float(pt, (float*)input, len, sc_degree, level);
  pt->_poly._data = NULL;
  return buf;
}

void Free_plain_buffer(struct PLAINTEXT_BUFFER* buf) { free(buf); }

void* Cast_buffer_to_plain(struct PLAINTEXT_BUFFER* buf) {
  if (memcmp(buf->_magic, PT_BUFFER_MAGIC, sizeof(buf->_magic)) != 0) {
    FMT_ASSERT(false, "Plaintext buffer magic mismatch");
    return NULL;
  }
  if (buf->_version != RT_VERSION_FULL) {
    FMT_ASSERT(false, "Plaintext buffer version mismatch");
    return NULL;
  }
  uint64_t sz = Plain_buffer_length(buf);
  if (buf->_size + sizeof(struct PLAINTEXT_BUFFER) > sz) {
    FMT_ASSERT(false, "Plaintext buffer too small");
    return NULL;
  }

  PLAINTEXT* pt = (PLAINTEXT*)(buf->_data);
  if (pt->_poly._data != NULL) {
    FMT_ASSERT(false, "Plaintext poly data is not NULL");
    return NULL;
  }
  uint32_t data_sz = sizeof(pt->_poly._data[0]) * pt->_poly._num_alloc_primes *
                     pt->_poly._ring_degree;
  if (buf->_size != data_sz + sizeof(PLAINTEXT)) {
    FMT_ASSERT(false, "Plaintext size mismatch");
    return NULL;
  }
  pt->_poly._data = (int64_t*)(buf->_data + sizeof(PLAINTEXT));
  return (void*)pt;
}

bool Compare_plain_buffer(const struct PLAINTEXT_BUFFER* pb_x,
                          const struct PLAINTEXT_BUFFER* pb_y) {
  if (memcmp(pb_x, pb_y, sizeof(struct PLAINTEXT_BUFFER)) != 0) {
    return false;
  }
  PLAINTEXT* pt_x = (PLAINTEXT*)pb_x->_data;
  PLAINTEXT* pt_y = (PLAINTEXT*)pb_y->_data;
  if (pt_x->_poly._ring_degree != pt_y->_poly._ring_degree ||
      pt_x->_poly._num_alloc_primes != pt_y->_poly._num_alloc_primes ||
      pt_x->_poly._num_primes != pt_y->_poly._num_primes ||
      pt_x->_poly._num_primes_p != pt_y->_poly._num_primes_p ||
      pt_x->_poly._is_ntt != pt_y->_poly._is_ntt) {
    return false;
  }
  if (pt_x->_slots != pt_y->_slots ||
      pt_x->_scaling_factor != pt_y->_scaling_factor ||
      pt_x->_sf_degree != pt_y->_sf_degree) {
    return false;
  }
  uint64_t sz = sizeof(uint64_t) * pt_x->_poly._ring_degree *
                pt_x->_poly._num_alloc_primes;
  const char* data_x = pb_x->_data + sizeof(PLAINTEXT);
  const char* data_y = pb_y->_data + sizeof(PLAINTEXT);
  if (memcmp(data_x, data_y, sz) != 0) {
    return false;
  }
  return true;
}

uint64_t Max_plain_buffer_length() {
  return sizeof(struct PLAINTEXT_BUFFER) + Get_plaintext_length(0);
}

L_POLY Lpoly_from_plain(PLAIN plain, size_t idx) {
  FMT_ASSERT(idx < Get_num_pq(Get_plain_poly(plain)), "invalid idx");
  // init lpoly from rns poly of plain
  POLY   c0 = Get_plain_poly(plain);
  L_POLY poly;
  size_t degree = Get_rdgree(Get_plain_poly(plain));
  Init_lpoly_data(&poly, degree, Coeffs(c0, idx, degree));
  Set_lpoly_ntt(&poly, Is_ntt(c0));
  return poly;
}