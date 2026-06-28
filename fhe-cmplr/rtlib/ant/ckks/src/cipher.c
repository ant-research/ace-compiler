//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "ckks/cipher.h"

#include "ckks/bootstrap.h"
#include "ckks/ciphertext.h"
#include "ckks/decryptor.h"
#include "ckks/encryptor.h"
#include "ckks/evaluator.h"
#include "ckks/key_gen.h"
#include "ckks/param.h"
#include "common/error.h"
#include "common/rt_api.h"
#include "common/rt_config.h"
#include "common/rtlib_timing.h"
#include "context/ckks_context.h"
#include "poly/rns_poly.h"

void Free_cipher(CIPHER ciph) { return Free_ciphertext(ciph); }

void Init_cipher(CIPHER res, CIPHER ciph, double sc, uint32_t sc_degree) {
  FMT_ASSERT(res, "invalid ciphertext");
  Init_ciphertext_from_ciph(res, ciph, sc, sc_degree);
  // hard code for now, ntt should be set by compiler
  Set_is_ntt(Get_c0(res), true);
  Set_is_ntt(Get_c1(res), true);
  Set_ciph_level(res, Get_ciph_level(ciph));
}

void Init_ciph_same_scale(CIPHER res, CIPHER ciph1, CIPHER ciph2) {
  RTLIB_TM_START(RTM_INIT_CIPH_SM_SC, rtm);
  CIPHER ciph = ciph2 != NULL ? Adjust_level(ciph1, ciph2, false, NULL) : ciph1;
  if (ciph == ciph2 && Get_poly_coeffs(Get_c0(ciph1)) == NULL) {
    // init ciph1
    Init_ciphertext_from_ciph(ciph1, ciph2, Get_ciph_sfactor(ciph2),
                              Sc_degree(ciph2));
  } else if (Get_poly_coeffs(Get_c0(ciph)) == NULL) {
    // both ciph1 and ciph2 are not initialized, clear res memory
    memset(res, 0, sizeof(CIPHERTEXT));
    RTLIB_TM_END(RTM_INIT_CIPH_SM_SC, rtm);
    return;
  }
  // do not clear result contents, set the size values
  Init_ciphertext(res, Get_ciph_degree(ciph), Level(ciph), Num_p(Get_c0(ciph)),
                  Get_ciph_sfactor(ciph), Sc_degree(ciph),
                  Get_ciph_slots(ciph));
  Set_is_ntt(Get_c0(res), true);
  Set_is_ntt(Get_c1(res), true);
  RTLIB_TM_END(RTM_INIT_CIPH_SM_SC, rtm);
}

void Init_ciph_same_scale_plain(CIPHER res, CIPHER ciph, PLAIN plain) {
  RTLIB_TM_START(RTM_INIT_CIPH_SM_SC, rtm);
  Init_cipher(res, ciph, Get_ciph_sfactor(ciph), Sc_degree(ciph));
  uint32_t plain_level = Poly_level(Get_plain_poly(plain));
  if (Level(ciph) > plain_level) Set_ciph_level(res, plain_level);
  RTLIB_TM_END(RTM_INIT_CIPH_SM_SC, rtm);
}

void Init_ciph_up_scale(CIPHER res, CIPHER ciph1, CIPHER ciph2) {
  RTLIB_TM_START(RTM_INIT_CIPH_UP_SC, rtm);
  CIPHER ciph = Adjust_level(ciph1, ciph2, false, NULL);
  Init_cipher(res, ciph, Get_ciph_sfactor(ciph1) * Get_ciph_sfactor(ciph2),
              Sc_degree(ciph1) + Sc_degree(ciph2));
  RTLIB_TM_END(RTM_INIT_CIPH_UP_SC, rtm);
}

void Init_ciph_up_scale_plain(CIPHER res, CIPHER ciph, PLAIN plain) {
  RTLIB_TM_START(RTM_INIT_CIPH_UP_SC, rtm);
  Init_cipher(res, ciph,
              Get_ciph_sfactor(ciph) * Get_plain_scaling_factor(plain),
              Sc_degree(ciph) + Get_plain_sf_degree(plain));

  uint32_t plain_level = Poly_level(Get_plain_poly(plain));
  if (Level(ciph) > plain_level) Set_ciph_level(res, plain_level);
  RTLIB_TM_END(RTM_INIT_CIPH_UP_SC, rtm);
}

void Init_ciph_up_scale_scalars(CIPHER res, CIPHER ciph, size_t len) {
  RTLIB_TM_START(RTM_INIT_CIPH_UP_SC, rtm);
  Init_cipher(res, ciph, Get_ciph_sfactor(ciph) * Get_default_sc(),
              Sc_degree(ciph) + 1);

  if (Level(ciph) > len) Set_ciph_level(res, len);
  RTLIB_TM_END(RTM_INIT_CIPH_UP_SC, rtm);
}

void Init_ciph_down_scale(CIPHER res, CIPHER ciph) {
  RTLIB_TM_START(RTM_INIT_CIPH_DN_SC, rtm);
  FMT_ASSERT(Sc_degree(ciph) >= 2, "Scale degree of result dec to zero");
  Init_cipher(res, ciph, Get_ciph_sfactor(ciph) / Get_default_sc(),
              Sc_degree(ciph) - 1);
  RTLIB_TM_END(RTM_INIT_CIPH_DN_SC, rtm);
}

void Init_ciph3_down_scale(CIPHER3 res, CIPHER3 ciph) {
  RTLIB_TM_START(RTM_INIT_CIPH_DN_SC, rtm);
  FMT_ASSERT(Get_ciph3_sf_degree(ciph) >= 2,
             "Scale degree of result dec to zero");
  Init_ciphertext3_from_ciph3(res, ciph,
                              Get_ciph3_sfactor(ciph) / Get_default_sc(),
                              Get_ciph3_sf_degree(ciph) - 1);
  RTLIB_TM_END(RTM_INIT_CIPH_DN_SC, rtm);
}

void Init_ciph_same_scale_ciph3(CIPHER res, CIPHER3 ciph) {
  RTLIB_TM_START(RTM_INIT_CIPH_SM_SC, rtm);
  FMT_ASSERT(res, "invalid ciphertext");
  Init_ciphertext_from_ciph3(res, ciph, Get_ciph3_sfactor(ciph),
                             Get_ciph3_sf_degree(ciph));
  // hard code for now, ntt should be set by compiler
  Set_is_ntt(Get_c0(res), true);
  Set_is_ntt(Get_c1(res), true);
  Set_ciph_level(res, Get_ciph3_level(ciph));
  RTLIB_TM_END(RTM_INIT_CIPH_SM_SC, rtm);
}

void Init_ciph3_same_scale_ciph3(CIPHER3 res, CIPHER3 ciph1, CIPHER3 ciph2) {
  RTLIB_TM_START(RTM_INIT_CIPH_SM_SC, rtm);
  FMT_ASSERT(res, "invalid ciphertext");
  CIPHER3 ciph =
      ciph2 != NULL ? Adjust_ciph3_level(ciph1, ciph2, false, NULL) : ciph1;
  Init_ciphertext3_from_ciph3(res, ciph, Get_ciph3_sfactor(ciph),
                              Get_ciph3_sf_degree(ciph));
  // hard code for now, ntt should be set by compiler
  Set_is_ntt(Get_ciph3_c0(res), true);
  Set_is_ntt(Get_ciph3_c1(res), true);
  Set_is_ntt(Get_ciph3_c2(res), true);
  Set_ciph3_level(res, Get_ciph3_level(ciph));
  RTLIB_TM_END(RTM_INIT_CIPH_SM_SC, rtm);
}

void Init_ciph3_same_scale(CIPHER3 res, CIPHER3 ciph1, CIPHER3 ciph2) {
  Init_ciph3_same_scale_ciph3(res, ciph1, ciph2);
}

void Init_ciph3_up_scale(CIPHER3 res, CIPHER ciph1, CIPHER ciph2) {
  RTLIB_TM_START(RTM_INIT_CIPH_UP_SC, rtm);
  FMT_ASSERT(res, "invalid ciphertext");
  CIPHER ciph = Adjust_level(ciph1, ciph2, false, NULL);
  Init_ciphertext3_from_ciph(res, ciph,
                             Get_ciph_sfactor(ciph1) * Get_ciph_sfactor(ciph2),
                             Sc_degree(ciph1) + Sc_degree(ciph2));
  // hard code for now, ntt should be set by compiler
  Set_is_ntt(Get_ciph3_c0(res), true);
  Set_is_ntt(Get_ciph3_c1(res), true);
  Set_is_ntt(Get_ciph3_c2(res), true);
  Set_ciph3_level(res, Get_ciph_level(ciph));
  RTLIB_TM_END(RTM_INIT_CIPH_UP_SC, rtm);
}

void Copy_ciph(CIPHER res, CIPHER ciph) {
  RTLIB_TM_START(RTM_COPY_CIPH, rtm);
  Copy_ciphertext(res, ciph);
  RTLIB_TM_END(RTM_COPY_CIPH, rtm);
}

size_t Level(CIPHER ciph) { return Get_ciph_level(ciph); }

uint32_t Sc_degree(CIPHER ciph) { return Get_ciph_sf_degree(ciph); }

uint32_t Get_slots(CIPHER ciph) { return Get_ciph_slots(ciph); }

void Set_slots(CIPHER ciph, uint32_t slots) { Set_ciph_slots(ciph, slots); }

bool Within_value_range(CIPHER ciph, double* msg, uint32_t len) {
  uint32_t slots = Get_slots(ciph);
  FMT_ASSERT(len <= slots, "length of msg should less than slots");
  VALUE_LIST* transformed = Alloc_value_list(DCMPLX_TYPE, slots);
  VALUE_LIST* input       = Alloc_value_list(DCMPLX_TYPE, len);
  for (size_t i = 0; i < len; i++) {
    DCMPLX_VALUE_AT(input, i) = msg[i];
  }
  bool within_range =
      Transform_msg_within_range(transformed, (CKKS_ENCODER*)Context->_encoder,
                                 input, Level(ciph), Get_ciph_sfactor(ciph));
  Free_value_list(input);
  Free_value_list(transformed);
  return within_range;
}

double* Get_msg(CIPHER ciph) {
  CIPHER clone  = Alloc_ciphertext();
  size_t size_p = Get_ciph_prime_p_cnt(ciph);
  if (size_p) {
    Init_ciphertext(clone, Get_ciph_degree(ciph), Get_ciph_prime_cnt(ciph), 0,
                    Get_ciph_sfactor(ciph), Get_ciph_sf_degree(ciph),
                    Get_ciph_slots(ciph));
    Mod_down(Get_c0(clone), Get_c0(ciph));
    Mod_down(Get_c1(clone), Get_c1(ciph));
  } else {
    Copy_ciph(clone, ciph);
  }
  PLAIN plain = Alloc_plaintext();
  Decrypt(plain, (CKKS_DECRYPTOR*)Context->_decryptor, clone, NULL);
  Free_ciphertext(clone);
  double* data = Get_msg_from_plain(plain);
  Free_plaintext(plain);
  return data;
}

DCMPLX* Get_msg_with_imag(CIPHER ciph) {
  CIPHER clone  = Alloc_ciphertext();
  size_t size_p = Get_ciph_prime_p_cnt(ciph);
  if (size_p) {
    Init_ciphertext(clone, Get_ciph_degree(ciph), Get_ciph_prime_cnt(ciph), 0,
                    Get_ciph_sfactor(ciph), Get_ciph_sf_degree(ciph),
                    Get_ciph_slots(ciph));
    Mod_down(Get_c0(clone), Get_c0(ciph));
    Mod_down(Get_c1(clone), Get_c1(ciph));
  } else {
    Copy_ciph(clone, ciph);
  }
  PLAIN plain = Alloc_plaintext();
  Decrypt(plain, (CKKS_DECRYPTOR*)Context->_decryptor, clone, NULL);
  Free_ciphertext(clone);
  DCMPLX* data = Get_dcmplx_msg_from_plain(plain);
  Free_plaintext(plain);
  return data;
}

void Print_msg_range(FILE* fp, DCMPLX* msg, uint32_t msg_len) {
  double   max_real     = creal(msg[0]);
  double   max_imag     = cimag(msg[0]);
  double   min_real     = max_real;
  double   min_imag     = max_imag;
  uint32_t max_real_pos = 0;
  uint32_t min_real_pos = 0;
  uint32_t max_imag_pos = 0;
  uint32_t min_imag_pos = 0;
  for (uint32_t i = 1; i < msg_len; i++) {
    double real = creal(msg[i]);
    double imag = cimag(msg[i]);
    if (real > max_real) {
      max_real     = real;
      max_real_pos = i;
    } else if (real < min_real) {
      min_real     = real;
      min_real_pos = i;
    }
    if (imag > max_imag) {
      max_imag     = imag;
      max_imag_pos = i;
    } else if (imag < min_imag) {
      min_imag     = imag;
      min_imag_pos = i;
    }
  }
  fprintf(fp,
          "msg_range[%d]: real(%.17f[%d] ~ %.17f[%d]), imag(%.17f[%d] ~ "
          "%.17f[%d])\n",
          msg_len, min_real, min_real_pos, max_real, max_real_pos, min_imag,
          min_imag_pos, max_imag, max_imag_pos);
}

void Print_cipher_msg(FILE* fp, const char* name, CIPHER ciph, uint32_t len) {
  DCMPLX*  data     = Get_msg_with_imag(ciph);
  uint32_t slot_len = Get_ciph_slots(ciph);
  Print_cipher_info(fp, name, ciph);

  fprintf(fp, "[%s] msg: [ ", name);
  for (uint32_t i = 0; i < len && i < slot_len; i++) {
    fprintf(fp, "%.17f ", creal(data[i]));
  }
  fprintf(fp, "] ");
  Print_msg_range(fp, data, slot_len);

  free(data);
}

void Print_cipher_msg_by_block_len(FILE* fp, const char* name, CIPHER ciph,
                                   uint32_t len, uint32_t b_len) {
  DCMPLX*  data     = Get_msg_with_imag(ciph);
  uint32_t slot_len = Get_ciph_slots(ciph);
  Print_cipher_info(fp, name, ciph);

  fprintf(fp, "[%s] msg: [ \n", name);
  for (uint32_t i = 0; i < len && i < slot_len; i++) {
    if (i % b_len == 0) {
      fprintf(fp, "\n");
    }
    fprintf(fp, "%.17f ", creal(data[i]));
  }
  fprintf(fp, "] ");
  Print_msg_range(fp, data, slot_len);

  free(data);
}

void Print_cipher_msg_with_imag(FILE* fp, const char* name, CIPHER ciph,
                                uint32_t len) {
  DCMPLX*  data     = Get_msg_with_imag(ciph);
  uint32_t slot_len = Get_ciph_slots(ciph);
  Print_cipher_info(fp, name, ciph);

  fprintf(fp, "[%s] msg: [ ", name);
  for (uint32_t i = 0; i < len && i < slot_len; i++) {
    double real = creal(data[i]);
    double imag = cimag(data[i]);
    fprintf(fp, "(%.17f, %.17fI) ", creal(data[i]), cimag(data[i]));
  }
  fprintf(fp, "] ");
  Print_msg_range(fp, data, slot_len);

  free(data);
}

void Print_cipher_range(FILE* fp, const char* name, CIPHER ciph) {
  DCMPLX*  data     = Get_msg_with_imag(ciph);
  uint32_t slot_len = Get_ciph_slots(ciph);
  fprintf(fp, "[%s] ", name);
  Print_msg_range(fp, data, slot_len);
  free(data);
}

void Print_cipher_info(FILE* fp, const char* name, CIPHER ciph) {
  fprintf(fp, "\n[%s] ciph_info: %d %d %ld %ld\n", name,
          Get_ciph_sf_degree(ciph), Get_ciph_slots(ciph),
          Get_ciph_prime_cnt(ciph), Get_ciph_prime_p_cnt(ciph));
}

void Print_cipher_poly(FILE* fp, const char* name, CIPHER ciph) {
  Print_cipher_info(fp, name, ciph);
  fprintf(fp, "@c0:\n");
  Print_poly_lite(fp, Get_c0(ciph));
  fprintf(fp, "@c1:\n");
  Print_poly_lite(fp, Get_c1(ciph));
}

void Dump_cipher_msg(const char* name, CIPHER ciph, uint32_t len) {
  Print_cipher_msg(Get_trace_file(), name, ciph, len);
}

CIPHER Real_relu(CIPHER ciph) {
  PLAIN plain1 = Alloc_plaintext();
  Decrypt(plain1, (CKKS_DECRYPTOR*)Context->_decryptor, ciph, NULL);
  VALUE_LIST* vec = Alloc_value_list(DCMPLX_TYPE, Get_plain_slots(plain1));
  Decode(vec, (CKKS_ENCODER*)Context->_encoder, plain1);

  FOR_ALL_ELEM(vec, idx) {
    DCMPLX val = DCMPLX_VALUE_AT(vec, idx);
    if (creal(val) <= 0) {
      DCMPLX_VALUE_AT(vec, idx) *= 0.0;
    }
  }

  // encode & encrypt
  PLAINTEXT* plain2 = Alloc_plaintext();
  Encode_at_level_with_sf(plain2, (CKKS_ENCODER*)Context->_encoder, vec,
                          Get_ciph_prime_cnt(ciph), Get_ciph_slots(ciph),
                          Get_ciph_sf_degree(ciph));
  CIPHER result = Alloc_ciphertext();
  Encrypt_msg(result, (CKKS_ENCRYPTOR*)Context->_encryptor, plain2);

  Free_value_list(vec);
  Free_plaintext(plain1);
  Free_plaintext(plain2);

  return result;
}

CIPHER Add_ciph(CIPHER res, CIPHER ciph1, CIPHER ciph2) {
  Add_ciphertext(res, ciph1, ciph2, (CKKS_EVALUATOR*)Eval());
  return res;
}

CIPHER Add_plain(CIPHER res, CIPHER ciph, PLAIN plain) {
  Add_plaintext(res, ciph, plain, (CKKS_EVALUATOR*)Eval());
  return res;
}

CIPHER Sub_ciph(CIPHER res, CIPHER ciph1, CIPHER ciph2) {
  Sub_ciphertext(res, ciph1, ciph2, (CKKS_EVALUATOR*)Eval());
  return res;
}

CIPHER Mul_ciph(CIPHER res, CIPHER ciph1, CIPHER ciph2) {
  Mul_ciphertext(res, ciph1, ciph2, (CKKS_EVALUATOR*)Eval());
  return res;
}

CIPHER3 Mul_ciph3(CIPHER3 res, CIPHER ciph1, CIPHER ciph2) {
  Mul_ciphertext3(res, ciph1, ciph2, (CKKS_EVALUATOR*)Eval());
  return res;
}

CIPHER Mul_plain(CIPHER res, CIPHER ciph, PLAIN plain) {
  Mul_plaintext(res, ciph, plain, (CKKS_EVALUATOR*)Eval());
  return res;
}

CIPHER Relin(CIPHER res, CIPHER3 ciph) {
  Relinearize_ciph3(res, Get_ciph3_c0(ciph), Get_ciph3_c1(ciph),
                    Get_ciph3_c2(ciph), Get_ciph3_sfactor(ciph),
                    Get_ciph3_sf_degree(ciph), Get_ciph3_slots(ciph),
                    (CKKS_EVALUATOR*)Eval());
  return res;
}

CIPHER Rescale_ciph(CIPHER res, CIPHER ciph) {
  Rescale_ciphertext(res, ciph, (CKKS_EVALUATOR*)Eval());
  return res;
}

CIPHER Upscale_ciph(CIPHER res, CIPHER ciph, uint32_t mod_size) {
  Upscale_ciphertext(res, ciph, mod_size, (CKKS_EVALUATOR*)Eval());
  return res;
}

CIPHER Raise_mod(CIPHER res, CIPHER ciph, uint32_t mod_size) {
  CKKS_EVALUATOR* eval       = (CKKS_EVALUATOR*)Eval();
  CKKS_PARAMETER* params     = eval->_params;
  size_t          ring_degree = params->_poly_degree;
  uint32_t        raise_level = mod_size ? mod_size : (uint32_t)Get_q_cnt();

  CIPHERTEXT* raised = Alloc_ciphertext();
  Init_ciphertext_from_ciph(raised, ciph, ciph->_scaling_factor,
                            ciph->_sf_degree);
  Copy_ciphertext(raised, ciph);

  while (Get_ciph_sf_degree(raised) > 1) {
    Rescale_ciphertext(raised, raised, eval);
  }

  if (Is_ntt(Get_c0(raised))) Conv_ntt2poly_inplace(Get_c0(raised));
  if (Is_ntt(Get_c1(raised))) Conv_ntt2poly_inplace(Get_c1(raised));

  Init_ciphertext(res, ring_degree, raise_level, 0, raised->_scaling_factor,
                  raised->_sf_degree, Get_ciph_slots(ciph));
  Transform_values_from_level0(Get_c0(res), Get_c0(raised));
  Transform_values_from_level0(Get_c1(res), Get_c1(raised));
  Free_ciphertext(raised);

  Conv_poly2ntt_inplace(Get_c0(res));
  Conv_poly2ntt_inplace(Get_c1(res));
  return res;
}

CIPHER Downscale_ciph(CIPHER res, CIPHER ciph, uint32_t waterline) {
  Downscale_ciphertext(res, ciph, waterline, (CKKS_EVALUATOR*)Eval());
  return res;
}

void Modswitch_ciph(CIPHER ciph) {
  Modswitch_ciphertext(ciph, (CKKS_EVALUATOR*)Eval());
}

CIPHER Rotate_ciph(CIPHER res, CIPHER ciph, int32_t rot_idx) {
  Eval_fast_rotate(res, ciph, rot_idx, (CKKS_EVALUATOR*)Eval());
  return res;
}

void Rotate_batch_ciph(CIPHER res_arr, CIPHER ciph, const int32_t* rot_idx,
                       uint32_t count) {
  CKKS_EVALUATOR* eval = (CKKS_EVALUATOR*)Eval();
  if (count == 0) {
    return;
  }

  VALUE_LIST* precomputed = Alloc_precomp(Get_c1(ciph));
  for (uint32_t idx = 0; idx < count; ++idx) {
    CIPHERTEXT* out = &res_arr[idx];
    int32_t     rot = rot_idx[idx];
    if (rot == 0) {
      Init_ciphertext_from_ciph(out, ciph, ciph->_scaling_factor,
                                ciph->_sf_degree);
      Copy_ciphertext(out, ciph);
    } else {
      Fast_rotate(out, ciph, rot, eval, precomputed);
    }
  }
  Free_precomp(precomputed);
}

CIPHER Conjugate_ciph(CIPHER res, CIPHER ciph) {
  Conjugate(res, ciph, (CKKS_EVALUATOR*)Eval());
  return res;
}

CIPHER Mul_mono_ciph(CIPHER res, CIPHER ciph, uint32_t power) {
  Mul_by_monomial(res, ciph, power, (CKKS_EVALUATOR*)Eval());
  return res;
}

static CKKS_BTS_PRECOM* Ensure_bts_precom(CIPHER ciph, uint32_t num_slots,
                                          CKKS_BTS_CTX** out_bts_ctx) {
  CKKS_EVALUATOR* eval    = (CKKS_EVALUATOR*)Eval();
  CKKS_BTS_CTX*   bts_ctx = Get_bts_ctx(eval);
  if (!num_slots) {
    num_slots = Get_ciph_slots(ciph);
  }
  CKKS_BTS_PRECOM* precom = Get_bts_precom(bts_ctx, num_slots);
  // if bootstrap's precomputations of num_slots were not generated,
  // call Bootstrap_precom to recompute.
  if (!precom) {
    Bootstrap_precom(num_slots);
    precom = Get_bts_precom(bts_ctx, num_slots);
  }
  FMT_ASSERT(precom != NULL,
             "Bootstrap precom is unavailable after Bootstrap_precom");
  if (!Get_keygen_status(precom)) {
    Bootstrap_keygen(bts_ctx, num_slots);
  }
  if (out_bts_ctx) {
    *out_bts_ctx = bts_ctx;
  }
  return precom;
}

static bool Is_lt_bootstrap(CKKS_BTS_PRECOM* precom) {
  VL_I32* encode_params = Get_encode_params(precom);
  VL_I32* decode_params = Get_decode_params(precom);
  int32_t enc_budget    = Get_i32_value_at(encode_params, LEVEL_BUDGET);
  int32_t dec_budget    = Get_i32_value_at(decode_params, LEVEL_BUDGET);
  return (enc_budget == 1) && (dec_budget == 1);
}

typedef struct BTS_STAGE_STATE {
  bool        active;
  bool        full_packed;
  uint32_t    slots;
  CIPHERTEXT* enc_ciph_sub;
} BTS_STAGE_STATE;

#if defined(__GNUC__) || defined(__clang__)
#define RTLIB_TLS __thread
#else
#define RTLIB_TLS
#endif

static RTLIB_TLS BTS_STAGE_STATE Bts_stage_state = {0};

static void Clear_bts_stage_state(void) {
  if (Bts_stage_state.enc_ciph_sub != NULL) {
    Free_ciphertext(Bts_stage_state.enc_ciph_sub);
  }
  memset(&Bts_stage_state, 0, sizeof(Bts_stage_state));
}

static void Normalize_evalmod_scale(CIPHERTEXT* ciph, CIPHERTEXT* paired,
                                    CKKS_EVALUATOR* eval) {
  if (AUTO_SCALE) {
    if (Get_ciph_sf_degree(ciph) == 2) {
      Rescale_ciphertext(ciph, ciph, eval);
      if (paired != NULL) {
        Rescale_ciphertext(paired, paired, eval);
      }
    }
  } else {
    while (Get_ciph_sf_degree(ciph) > 1) {
      Rescale_ciphertext(ciph, ciph, eval);
      if (paired != NULL) {
        Rescale_ciphertext(paired, paired, eval);
      }
    }
  }
}

CIPHER Eval_bootstrap_ciph(CIPHER res, CIPHER ciph, uint32_t level_after_bts,
                           uint32_t num_slots) {
  RTLIB_TM_START(RTM_BOOTSTRAP, rtm);
  uint32_t lev = Get_ciph_level(ciph);
  FMT_ASSERT(lev > 0, "Input cipher level is exhausted");
  if (lev != 1) {
    DEV_WARN("WARN: input cipher level of bootstrap, %u, is larger than 1\n",
             lev);
  }
  CKKS_BTS_CTX* bts_ctx = NULL;
  (void)Ensure_bts_precom(ciph, num_slots, &bts_ctx);
  Eval_bootstrap(res, ciph, level_after_bts, bts_ctx);
  RTLIB_TM_END(RTM_BOOTSTRAP, rtm);
  return res;
}

CIPHER Bootstrap(CIPHER res, CIPHER ciph, uint32_t level_after_bts) {
  return Eval_bootstrap_ciph(res, ciph, level_after_bts, Get_ciph_slots(ciph));
}

CIPHER Eval_bootstrap_coeffs_to_slots_ciph(CIPHER res, CIPHER ciph,
                                           uint32_t num_slots) {
  RTLIB_TM_START(RTM_BS_COEFF_TO_SLOT, rtm);
  Clear_bts_stage_state();
  CKKS_BTS_CTX*    bts_ctx = NULL;
  CKKS_BTS_PRECOM* precom  = Ensure_bts_precom(ciph, num_slots, &bts_ctx);
  CKKS_PARAMETER*  params   = Get_bts_params(bts_ctx);
  CKKS_EVALUATOR*  eval     = Get_bts_eval(bts_ctx);
  size_t           q_cnt    = Get_q_cnt();
  size_t           ring_deg = params->_poly_degree;
  size_t           m        = ring_deg * 2;
  uint32_t         slots    = num_slots ? num_slots : Get_ciph_slots(ciph);

  // Mirror full Eval_bootstrap preprocessing before coeffs-to-slots.
  CIPHERTEXT* raised = Alloc_ciphertext();
  Init_ciphertext_from_ciph(raised, ciph, ciph->_scaling_factor, ciph->_sf_degree);
  Copy_ciphertext(raised, ciph);
  while (Get_ciph_sf_degree(raised) > 1) {
    Rescale_ciphertext(raised, raised, eval);
  }
  if (Is_ntt(Get_c0(raised))) Conv_ntt2poly_inplace(Get_c0(raised));
  if (Is_ntt(Get_c1(raised))) Conv_ntt2poly_inplace(Get_c1(raised));

  CIPHERTEXT* new_ciph = Alloc_ciphertext();
  Init_ciphertext(new_ciph, ring_deg, (uint32_t)q_cnt, 0, raised->_scaling_factor,
                  raised->_sf_degree, slots);
  Transform_values_from_level0(Get_c0(new_ciph), Get_c0(raised));
  Transform_values_from_level0(Get_c1(new_ciph), Get_c1(raised));
  Free_ciphertext(raised);
  Conv_poly2ntt_inplace(Get_c0(new_ciph));
  Conv_poly2ntt_inplace(Get_c1(new_ciph));

  bool is_lt_bts    = Is_lt_bootstrap(precom);
  bool full_packed  = (slots == m / 4);

  if (!full_packed) {
    // Sparse-packed bootstrap requires partial-sum preprocessing.
    CIPHERTEXT* temp = Alloc_ciphertext();
    for (uint32_t j = 1; j < ring_deg / (2 * slots); j <<= 1) {
      Eval_fast_rotate(temp, new_ciph, j * slots, eval);
      Add_ciphertext(new_ciph, new_ciph, temp, eval);
    }
    Free_ciphertext(temp);
  }

  if (is_lt_bts) {
    Linear_transform(res, new_ciph, Get_u0hatt_pre(precom), bts_ctx);
  } else {
    Coeffs_to_slots(res, new_ciph, Get_u0hatt_pre_fft(precom), bts_ctx);
  }

  if (full_packed) {
    CIPHERTEXT* conj_ciph = Alloc_ciphertext();
    CIPHERTEXT* enc_sub   = Alloc_ciphertext();
    Conjugate(conj_ciph, res, eval);
    Sub_ciphertext(enc_sub, res, conj_ciph, eval);
    Add_ciphertext(res, res, conj_ciph, eval);
    Mul_by_monomial(enc_sub, enc_sub, 3 * m / 4, eval);
    Normalize_evalmod_scale(res, enc_sub, eval);
    Free_ciphertext(conj_ciph);

    Bts_stage_state.active       = true;
    Bts_stage_state.full_packed  = true;
    Bts_stage_state.slots        = slots;
    Bts_stage_state.enc_ciph_sub = enc_sub;
  } else {
    CIPHERTEXT* conj_ciph = Alloc_ciphertext();
    Conjugate(conj_ciph, res, eval);
    Add_ciphertext(res, res, conj_ciph, eval);
    Free_ciphertext(conj_ciph);
    Normalize_evalmod_scale(res, NULL, eval);

    Bts_stage_state.active      = true;
    Bts_stage_state.full_packed = false;
    Bts_stage_state.slots       = slots;
  }

  Free_ciphertext(new_ciph);
  RTLIB_TM_END(RTM_BS_COEFF_TO_SLOT, rtm);
  return res;
}

CIPHER Eval_bootstrap_eval_mod_ciph(CIPHER res, CIPHER ciph,
                                    uint32_t num_slots) {
  RTLIB_TM_START(RTM_BS_APPROX_MOD, rtm);
  CKKS_BTS_CTX* bts_ctx = NULL;
  (void)Ensure_bts_precom(ciph, num_slots, &bts_ctx);
  CKKS_PARAMETER* params = Get_bts_params(bts_ctx);
  CKKS_EVALUATOR* eval = Get_bts_eval(bts_ctx);
  uint32_t slots = num_slots ? num_slots : Get_ciph_slots(ciph);

  const EVAL_SIN_POLY_INFO* eval_sin_poly_info =
      Get_eval_sin_poly_info(params->_hamming_weight);
  uint32_t      coeff_size = eval_sin_poly_info->_coeff_size;
  const double* dbl_coeff  = eval_sin_poly_info->_coeff;

  VL_DBL* coefficient = Alloc_value_list(DBL_TYPE, coeff_size);
  Init_dbl_value_list(coefficient, LIST_LEN(coefficient), (double*)dbl_coeff);

  bool has_full_packed_pair =
      Bts_stage_state.active && Bts_stage_state.full_packed &&
      Bts_stage_state.enc_ciph_sub != NULL && Bts_stage_state.slots == slots;

  if (has_full_packed_pair) {
    if (res != ciph) {
      Init_ciphertext_from_ciph(res, ciph, ciph->_scaling_factor, ciph->_sf_degree);
      Copy_ciphertext(res, ciph);
    }
    Eval_approx_mod(bts_ctx, res, res, coefficient, -1.0, 1.0);
    Eval_approx_mod(bts_ctx, Bts_stage_state.enc_ciph_sub,
                    Bts_stage_state.enc_ciph_sub, coefficient, -1.0, 1.0);
    Mul_by_monomial(Bts_stage_state.enc_ciph_sub, Bts_stage_state.enc_ciph_sub,
                    slots, eval);
    Add_ciphertext(res, res, Bts_stage_state.enc_ciph_sub, eval);
    Free_ciphertext(Bts_stage_state.enc_ciph_sub);
    Bts_stage_state.enc_ciph_sub = NULL;
  } else {
    Normalize_evalmod_scale(ciph, NULL, eval);
    Eval_approx_mod(bts_ctx, res, ciph, coefficient, -1.0, 1.0);
  }

  Free_value_list(coefficient);
  RTLIB_TM_END(RTM_BS_APPROX_MOD, rtm);
  return res;
}

CIPHER Eval_bootstrap_slots_to_coeffs_ciph(CIPHER res, CIPHER ciph,
                                           uint32_t num_slots) {
  RTLIB_TM_START(RTM_BS_SLOT_TO_COEFF, rtm);
  CKKS_BTS_CTX*    bts_ctx = NULL;
  CKKS_BTS_PRECOM* precom  = Ensure_bts_precom(ciph, num_slots, &bts_ctx);
  CKKS_PARAMETER*  params  = Get_bts_params(bts_ctx);
  CKKS_EVALUATOR*  eval    = Get_bts_eval(bts_ctx);
  size_t           degree  = params->_poly_degree;
  size_t           m       = degree * 2;
  uint32_t         slots   = num_slots ? num_slots : Get_ciph_slots(ciph);

  bool full_packed = (slots == m / 4);

  if (AUTO_SCALE) {
    Rescale_ciphertext(ciph, ciph, eval);
  }

  if (Is_lt_bootstrap(precom)) {
    Linear_transform(res, ciph, Get_u0_pre(precom), bts_ctx);
  } else {
    Slots_to_coeffs(res, ciph, Get_u0_pre_fft(precom), bts_ctx);
  }

  // Sparse-packed case needs rotate-add after slot-to-coeff.
  if (!full_packed) {
    CIPHERTEXT* rot_ciph = Alloc_ciphertext();
    Eval_fast_rotate(rot_ciph, res, slots, eval);
    Add_ciphertext(res, res, rot_ciph, eval);
    Free_ciphertext(rot_ciph);
  }

  // Match Eval_bootstrap postprocessing to map bootstrap output back to message.
  int64_t mod_head_val = Get_mod_val(Q_modulus());
  double  sf           = params->_scaling_factor;
  int32_t deg          = round(log2((double)mod_head_val / sf));
  if (Get_rtlib_config(CONF_BTS_CLEAR_IMAG) && deg >= 1) {
    CIPHERTEXT* conj_ciph = Alloc_ciphertext();
    Conjugate(conj_ciph, res, eval);
    Add_ciphertext(res, res, conj_ciph, eval);
    Free_ciphertext(conj_ciph);
    uint64_t q0_sf_ratio = pow(2., deg - 1);
    if (q0_sf_ratio > 1) {
      Mul_integer(res, res, q0_sf_ratio, eval);
    }
  } else {
    uint64_t q0_sf_ratio = pow(2., deg);
    Mul_integer(res, res, q0_sf_ratio, eval);
  }

  while (Get_ciph_sf_degree(res) > 1) {
    Rescale_ciphertext(res, res, eval);
  }

  Clear_bts_stage_state();
  RTLIB_TM_END(RTM_BS_SLOT_TO_COEFF, rtm);
  return res;
}

CIPHER Encrypt(CIPHER res, PLAIN plain) {
  Encrypt_msg(res, (CKKS_ENCRYPTOR*)Context->_encryptor, plain);
  return res;
}

L_POLY Lpoly_from_ciph(CIPHER ciph, size_t idx) {
  size_t num_pq = Get_ciph_num_pq(ciph);
  FMT_ASSERT(idx < 2 * num_pq, "invalid idx");
  POLY   tmp_poly;
  size_t poly_idx;
  if (idx < num_pq) {
    // get lpoly from c0
    tmp_poly = Get_c0(ciph);
    poly_idx = idx;
  } else {
    // get lpoly from c1
    tmp_poly = Get_c1(ciph);
    poly_idx = idx - num_pq;
  }
  // init lpoly from rns_poly of ciph
  L_POLY poly;
  size_t degree = Degree();
  Init_lpoly_data(&poly, degree, Coeffs(tmp_poly, poly_idx, degree));
  Set_lpoly_ntt(&poly, Is_ntt(tmp_poly));
  return poly;
}

L_POLY Lpoly_from_ciph3(CIPHER3 ciph, size_t idx) {
  size_t num_pq = Get_ciph3_num_pq(ciph);
  FMT_ASSERT(idx < 3 * num_pq, "invalid idx");
  POLY   tmp_poly;
  size_t poly_idx;
  if (idx < num_pq) {
    tmp_poly = Get_ciph3_c0(ciph);
    poly_idx = idx;
  } else if (idx < 2 * num_pq) {
    tmp_poly = Get_ciph3_c1(ciph);
    poly_idx = idx - num_pq;
  } else {
    tmp_poly = Get_ciph3_c2(ciph);
    poly_idx = idx - 2 * num_pq;
  }
  // init lpoly from rns_poly of ciph
  L_POLY poly;
  size_t degree = Degree();
  Init_lpoly_data(&poly, degree, Coeffs(tmp_poly, poly_idx, degree));
  Set_lpoly_ntt(&poly, Is_ntt(tmp_poly));
  return poly;
}

void Set_ciph_data(CIPHER ciph, L_POLY poly, size_t idx) {
  size_t num_pq = Get_ciph_num_pq(ciph);
  FMT_ASSERT(idx < 2 * num_pq, "invalid idx");
  POLY   tmp_poly;
  size_t poly_idx;
  if (idx < num_pq) {
    tmp_poly = Get_c0(ciph);
    poly_idx = idx;
  } else {
    tmp_poly = Get_c1(ciph);
    poly_idx = idx - num_pq;
  }
  FMT_ASSERT(Get_poly_coeffs(tmp_poly), "invalid coeffs");
  Set_coeffs(tmp_poly, poly_idx, Degree(), Get_lpoly_coeffs(&poly));
  Set_is_ntt(tmp_poly, Is_lpoly_ntt(&poly));
}

void Set_ciph3_data(CIPHER3 ciph, L_POLY poly, size_t idx) {
  size_t num_pq = Get_ciph3_num_pq(ciph);
  FMT_ASSERT(idx < 3 * num_pq, "invalid idx");
  POLY   tmp_poly;
  size_t poly_idx;
  if (idx < num_pq) {
    tmp_poly = Get_ciph3_c0(ciph);
    poly_idx = idx;
  } else if (idx < 2 * num_pq) {
    tmp_poly = Get_ciph3_c1(ciph);
    poly_idx = idx - num_pq;
  } else {
    tmp_poly = Get_ciph3_c2(ciph);
    poly_idx = idx - 2 * num_pq;
  }
  FMT_ASSERT(Get_poly_coeffs(tmp_poly), "invalid coeffs");
  Set_coeffs(tmp_poly, poly_idx, Degree(), Get_lpoly_coeffs(&poly));
  Set_is_ntt(tmp_poly, Is_lpoly_ntt(&poly));
}

void Reset_ciph(CIPHER ciph, size_t level) {
  FMT_ASSERT(Get_ciph_prime_p_cnt(ciph) == 0, "unsupport ciph with expanded");
  size_t q_cnt = Get_ciph_prime_cnt(ciph);
  FMT_ASSERT(level <= q_cnt, "level overflow");
  if (level == q_cnt) return;
  // step1: perform memory rearrangement of c0 & c1
  size_t   degree      = Get_ciph_degree(ciph);
  size_t   cur_level   = Get_ciph_num_pq(ciph);
  size_t   copy_size   = sizeof(int64_t) * degree;
  int64_t* c0_data     = (int64_t*)malloc(copy_size * level);
  int64_t* c1_data     = (int64_t*)malloc(copy_size * level);
  int64_t* old_c0_data = Get_poly_coeffs(Get_c0(ciph));
  int64_t* old_c1_data = Get_poly_coeffs(Get_c1(ciph));
  bool     is_ntt      = Is_ntt(Get_c0(ciph));
  // copy poly data to c0
  memcpy(c0_data, old_c0_data, copy_size * level);
  // copy poly data to c1
  if (2 * level <= cur_level) {
    memcpy(c1_data, old_c0_data + level * degree, copy_size * level);
  } else {
    memcpy(c1_data, old_c0_data + level * degree,
           (cur_level - level) * copy_size);
    memcpy(c1_data + (cur_level - level) * degree, old_c1_data,
           (2 * level - cur_level) * copy_size);
  }
  free(old_c0_data);
  free(old_c1_data);
  Init_poly_data(Get_c0(ciph), degree, level, 0, c0_data);
  Init_poly_data(Get_c1(ciph), degree, level, 0, c1_data);
  Set_is_ntt(Get_c0(ciph), is_ntt);
  Set_is_ntt(Get_c1(ciph), is_ntt);
  // step2: modswtich ciph to the final level
  for (size_t i = Get_ciph_prime_cnt(ciph); i > level; i--) {
    Modswitch_ciph(ciph);
  }
}
