//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "ckks/plain_eval.h"

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "fhe/core/rt_encode_api.h"
#include "fhe/core/rt_version.h"
#include "util/plaintext.h"

void Encode_plain(PLAIN plain, VALUE_LIST* input_vec, uint32_t sc_degree,
                  uint32_t level, bool is_ext) {
  uint32_t p_cnt =
      is_ext ? Get_crt_num_p(Get_param_crt((CKKS_PARAMETER*)Context->_params))
             : 0;
  Encode_at_level_with_sf(plain, (CKKS_ENCODER*)Context->_encoder, input_vec,
                          level, 0 /* slots */, sc_degree, p_cnt);
  Append_weight_plain((CKKS_ENCODER*)Context->_encoder,
                      Get_plain_mem_size(plain));
}

void Encode_plain_from_float(PLAIN plain, float* input, size_t len,
                             uint32_t sc_degree, uint32_t level, bool is_ext) {
  RTLIB_TM_START(RTM_PT_ENCODE, rtm);
  if (len == 1) {
    // Encode single value at given level
    Encode_val_at_level(plain, (CKKS_ENCODER*)Context->_encoder, (double)*input,
                        level, sc_degree, is_ext);
    RTLIB_TM_END(RTM_PT_ENCODE, rtm);

    return;
  }
  VALUE_LIST* input_vec = Alloc_value_list(DCMPLX_TYPE, len);
  FOR_ALL_ELEM(input_vec, idx) {
    DCMPLX_VALUE_AT(input_vec, idx) = (double)input[idx];
  }
  Encode_plain(plain, input_vec, sc_degree, level, is_ext);
  Free_value_list(input_vec);
  RTLIB_TM_END(RTM_PT_ENCODE, rtm);
}

void Encode_plain_from_double(PLAIN plain, double* input, size_t len,
                              uint32_t sc_degree, uint32_t level, bool is_ext) {
  RTLIB_TM_START(RTM_PT_ENCODE, rtm);
  if (len == 1) {
    // Encode single value at given level
    Encode_val_at_level(plain, (CKKS_ENCODER*)Context->_encoder, *input, level,
                        sc_degree, is_ext);
    RTLIB_TM_END(RTM_PT_ENCODE, rtm);
    return;
  }
  VALUE_LIST* input_vec = Alloc_value_list(DCMPLX_TYPE, len);
  FOR_ALL_ELEM(input_vec, idx) { DCMPLX_VALUE_AT(input_vec, idx) = input[idx]; }
  Encode_plain(plain, input_vec, sc_degree, level, is_ext);
  Free_value_list(input_vec);
  RTLIB_TM_END(RTM_PT_ENCODE, rtm);
}

void Encode_plain_from_float_mask(PLAIN plain, float cst, size_t len,
                                  uint32_t sc_degree, uint32_t level,
                                  bool is_ext) {
  float* mask = malloc(len * sizeof(float));
  for (uint32_t id = 0; id < len; ++id) mask[id] = cst;
  Encode_plain_from_float(plain, mask, len, sc_degree, level, is_ext);
  free(mask);
}

void Encode_plain_from_double_mask(PLAIN plain, double cst, size_t len,
                                   uint32_t sc_degree, uint32_t level,
                                   bool is_ext) {
  double* mask = malloc(len * sizeof(double));
  for (uint32_t id = 0; id < len; ++id) mask[id] = cst;
  Encode_plain_from_double(plain, mask, len, sc_degree, level, is_ext);
  free(mask);
}

void Encode_plain_from_float_with_scale(PLAIN plain, float* input, size_t len,
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

double* Get_msg_from_plain(PLAIN plain) {
  VALUE_LIST* vec = Alloc_value_list(DCMPLX_TYPE, Get_plain_slots(plain));
  Decode(vec, (CKKS_ENCODER*)Context->_encoder, plain);
  double* data = (double*)malloc(LIST_LEN(vec) * sizeof(double));
  FOR_ALL_ELEM(vec, idx) { data[idx] = creal(Get_dcmplx_value_at(vec, idx)); }
  Free_value_list(vec);
  return data;
}

void Print_plain_msg(FILE* fp, const char* name, PLAIN plain, uint32_t len) {
  PLAIN cloned_plain = Alloc_plaintext();
  Init_plaintext(cloned_plain, Get_rdgree(Get_plain_poly(plain)),
                 Get_plain_slots(plain), Get_num_q(Get_plain_poly(plain)),
                 Get_num_p(Get_plain_poly(plain)),
                 Get_plain_scaling_factor(plain), Get_plain_sf_degree(plain));
  Copy_polynomial(Get_plain_poly(cloned_plain), Get_plain_poly(plain));

  fprintf(fp, "\n[%s] plain_info: %d %d\n", name,
          Get_plain_sf_degree(cloned_plain), Get_plain_slots(cloned_plain));
  double*  msg      = Get_msg_from_plain(cloned_plain);
  uint32_t slot_len = Get_plain_slots(cloned_plain);
  for (uint32_t i = 0; i < len && i < slot_len; i++) {
    fprintf(fp, "%.17f ", msg[i]);
  }
}

DCMPLX* Get_dcmplx_msg_from_plain(PLAIN plain) {
  VALUE_LIST* vec = Alloc_value_list(DCMPLX_TYPE, Get_plain_slots(plain));
  Decode(vec, (CKKS_ENCODER*)Context->_encoder, plain);
  DCMPLX* data = (DCMPLX*)malloc(LIST_LEN(vec) * sizeof(DCMPLX));
  FOR_ALL_ELEM(vec, idx) { data[idx] = Get_dcmplx_value_at(vec, idx); }
  Free_value_list(vec);
  return data;
}

uint64_t Get_plaintext_length(uint32_t level, bool is_ext) {
  CKKS_PARAMETER* param      = (CKKS_PARAMETER*)Get_param(Context);
  uint32_t        degree     = param->_poly_degree;
  uint32_t        num_primes = (level > 0) ? level : param->_num_primes;
  uint32_t        num_p_primes =
      is_ext ? Get_crt_num_p(Get_param_crt((CKKS_PARAMETER*)Context->_params))
                    : 0;
  return sizeof(PLAINTEXT) +
         sizeof(uint64_t) * degree * (num_primes + num_p_primes);
}

struct PLAINTEXT_BUFFER* Encode_plain_buffer(const float* input, size_t len,
                                             uint32_t sc_degree, uint32_t level,
                                             bool is_ext) {
  uint32_t p_cnt =
      is_ext ? Get_crt_num_p(Get_param_crt((CKKS_PARAMETER*)Context->_params))
             : 0;
  size_t                   pt_len = Get_plaintext_length(level, is_ext);
  struct PLAINTEXT_BUFFER* buf    = (struct PLAINTEXT_BUFFER*)malloc(
      sizeof(struct PLAINTEXT_BUFFER) + pt_len);
  IS_TRUE(buf != NULL, "Failed to malloc sizeofPLAINTEXT_BUFFER");
  memcpy(buf->_magic, PT_BUFFER_MAGIC, sizeof(buf->_magic));
  buf->_version = RT_VERSION_FULL;
  buf->_size    = pt_len;
  memset(buf->_data, 0, pt_len);
  CKKS_PARAMETER* param       = (CKKS_PARAMETER*)Get_param(Context);
  PLAINTEXT*      pt          = (PLAINTEXT*)buf->_data;
  pt->_poly._ring_degree      = param->_poly_degree;
  pt->_poly._num_primes       = (level > 0) ? level : param->_num_primes;
  pt->_poly._num_primes_p     = p_cnt;
  pt->_poly._num_alloc_primes = pt->_poly._num_primes + pt->_poly._num_primes_p;
  pt->_poly._data = (uint64_t*)((char*)buf + sizeof(struct PLAINTEXT_BUFFER) +
                                sizeof(PLAINTEXT));
  Encode_plain_from_float(pt, (float*)input, len, sc_degree, level, is_ext);
  pt->_poly._data = NULL;
  return buf;
}

struct PLAINTEXT_BUFFER* Plain_prec(struct PLAINTEXT_BUFFER* buf) {
  CRT_CONTEXT* crt = Get_param_crt((CKKS_PARAMETER*)Context->_params);
  struct PLAINTEXT_BUFFER* res_buf = (struct PLAINTEXT_BUFFER*)malloc(
      sizeof(struct PLAINTEXT_BUFFER) + buf->_size);
  IS_TRUE(buf != NULL, "Failed to malloc sizeofPLAINTEXT_BUFFER");
  memcpy(res_buf->_magic, PT_BUFFER_MAGIC, sizeof(res_buf->_magic));
  res_buf->_version = RT_VERSION_FULL;
  res_buf->_size    = buf->_size;
  memset(res_buf->_data, 0, buf->_size);
  PLAINTEXT*  pt       = (PLAINTEXT*)Cast_buffer_to_plain(buf);
  PLAINTEXT*  res_pt   = (PLAINTEXT*)res_buf->_data;
  POLYNOMIAL* poly     = &(pt->_poly);
  POLYNOMIAL* res_poly = &(res_pt->_poly);
  Init_poly_data(res_poly, Get_rdgree(poly), Get_num_q(poly), Get_num_p(poly),
                 (int64_t*)((char*)res_buf + sizeof(struct PLAINTEXT_BUFFER) +
                            sizeof(PLAINTEXT)));

  Precomp_poly(res_poly, poly, Get_q_primes(crt), Get_p_primes(crt));
  res_pt->_poly._data = NULL;
  pt->_poly._data     = NULL;
  return res_buf;
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
  pt->_poly._data = (uint64_t*)(buf->_data + sizeof(PLAINTEXT));
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
  return sizeof(struct PLAINTEXT_BUFFER) + Get_plaintext_length(0, true);
}
