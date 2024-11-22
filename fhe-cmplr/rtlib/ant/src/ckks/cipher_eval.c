//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "ckks/cipher_eval.h"

#include "common/rt_api.h"
#include "rtlib/context.h"
#include "util/ciphertext.h"
#include "util/ckks_bootstrap_context.h"
#include "util/ckks_decryptor.h"
#include "util/ckks_encryptor.h"
#include "util/ckks_evaluator.h"
#include "util/ckks_key_generator.h"
#include "util/ckks_parameters.h"

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
  // do not clear result contents, set the size values
  Init_ciphertext(res, Get_ciph_degree(ciph), Level(ciph),
                  Get_num_p(Get_c0(ciph)), Get_ciph_sfactor(ciph),
                  Sc_degree(ciph), Get_ciph_slots(ciph));
  Set_is_ntt(Get_c0(res), true);
  Set_is_ntt(Get_c1(res), true);
  RTLIB_TM_END(RTM_INIT_CIPH_SM_SC, rtm);
}

void Init_ciph_same_scale_plain(CIPHER res, CIPHER ciph, PLAIN plain) {
  RTLIB_TM_START(RTM_INIT_CIPH_SM_SC, rtm);
  Init_cipher(res, ciph, Get_ciph_sfactor(ciph), Sc_degree(ciph));
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
  RTLIB_TM_END(RTM_INIT_CIPH_UP_SC, rtm);
}

void Init_ciph_down_scale(CIPHER res, CIPHER ciph) {
  RTLIB_TM_START(RTM_INIT_CIPH_DN_SC, rtm);
  Init_cipher(res, ciph, Get_ciph_sfactor(ciph) / Get_default_sc(),
              Sc_degree(ciph) - 1);
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

double* Get_msg(CIPHER ciph) {
  CRT_CONTEXT* crt   = Get_crt_context();
  CIPHER       clone = Alloc_ciphertext();
  size_t       sizeP = Get_ciph_prime_p_cnt(ciph);
  if (sizeP) {
    Init_ciphertext(clone, Get_ciph_degree(ciph), Get_ciph_prime_cnt(ciph), 0,
                    Get_ciph_sfactor(ciph), Get_ciph_sf_degree(ciph),
                    Get_ciph_slots(ciph));
    Reduce_rns_base(Get_c0(clone), Get_c0(ciph), crt);
    Reduce_rns_base(Get_c1(clone), Get_c1(ciph), crt);
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
  CRT_CONTEXT* crt   = Get_crt_context();
  CIPHER       clone = Alloc_ciphertext();
  size_t       sizeP = Get_ciph_prime_p_cnt(ciph);
  if (sizeP) {
    Init_ciphertext(clone, Get_ciph_degree(ciph), Get_ciph_prime_cnt(ciph), 0,
                    Get_ciph_sfactor(ciph), Get_ciph_sf_degree(ciph),
                    Get_ciph_slots(ciph));
    Reduce_rns_base(Get_c0(clone), Get_c0(ciph), crt);
    Reduce_rns_base(Get_c1(clone), Get_c1(ciph), crt);
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
  Add_ciphertext(res, ciph1, ciph2, (CKKS_EVALUATOR*)Get_eval(Context));
  return res;
}

CIPHER Add_plain(CIPHER res, CIPHER ciph, PLAIN plain) {
  Add_plaintext(res, ciph, plain, (CKKS_EVALUATOR*)Get_eval(Context));
  return res;
}

CIPHER Sub_ciph(CIPHER res, CIPHER ciph1, CIPHER ciph2) {
  Sub_ciphertext(res, ciph1, ciph2, (CKKS_EVALUATOR*)Get_eval(Context));
  return res;
}

CIPHER Mul_ciph(CIPHER res, CIPHER ciph1, CIPHER ciph2) {
  SWITCH_KEY* relin_key =
      Get_relin_key((CKKS_KEY_GENERATOR*)Get_key_gen(Context));
  Mul_ciphertext(res, ciph1, ciph2, relin_key,
                 (CKKS_EVALUATOR*)Get_eval(Context));
  return res;
}

CIPHER3 Mul_ciph3(CIPHER3 res, CIPHER ciph1, CIPHER ciph2) {
  Mul_ciphertext3(res, ciph1, ciph2, (CKKS_EVALUATOR*)Get_eval(Context));
  return res;
}

CIPHER Mul_plain(CIPHER res, CIPHER ciph, PLAIN plain) {
  Mul_plaintext(res, ciph, plain, (CKKS_EVALUATOR*)Get_eval(Context));
  return res;
}

CIPHER Relin(CIPHER res, CIPHER3 ciph) {
  SWITCH_KEY* relin_key =
      Get_relin_key((CKKS_KEY_GENERATOR*)Get_key_gen(Context));
  Relinearize_ciph3(res, relin_key, Get_ciph3_c0(ciph), Get_ciph3_c1(ciph),
                    Get_ciph3_c2(ciph), Get_ciph3_sfactor(ciph),
                    Get_ciph3_sf_degree(ciph), Get_ciph3_slots(ciph),
                    (CKKS_EVALUATOR*)Get_eval(Context));
  return res;
}

CIPHER Rescale_ciph(CIPHER res, CIPHER ciph) {
  Rescale_ciphertext(res, ciph, (CKKS_EVALUATOR*)Get_eval(Context));
  return res;
}

CIPHER Upscale_ciph(CIPHER res, CIPHER ciph, uint32_t mod_size) {
  Upscale_ciphertext(res, ciph, mod_size, (CKKS_EVALUATOR*)Get_eval(Context));
  return res;
}

CIPHER Downscale_ciph(CIPHER res, CIPHER ciph, uint32_t waterline) {
  Downscale_ciphertext(res, ciph, waterline,
                       (CKKS_EVALUATOR*)Get_eval(Context));
  return res;
}

void Modswitch_ciph(CIPHER ciph) {
  Modswitch_ciphertext(ciph, (CKKS_EVALUATOR*)Get_eval(Context));
}

CIPHER Rotate_ciph(CIPHER res, CIPHER ciph, int32_t rotation) {
  CKKS_KEY_GENERATOR* generator = (CKKS_KEY_GENERATOR*)Get_key_gen(Context);
  uint32_t            auto_idx  = Get_precomp_auto_idx(generator, rotation);
  FMT_ASSERT(auto_idx, "cannot get precompute automorphism index");
  SWITCH_KEY* rot_key = Get_auto_key(generator, auto_idx);
  FMT_ASSERT(rot_key, "cannot find auto key");
  Eval_fast_rotate(res, ciph, rotation, rot_key,
                   (CKKS_EVALUATOR*)Get_eval(Context));
  return res;
}

CIPHER Bootstrap(CIPHER res, CIPHER ciph, uint32_t level_after_bts) {
  RTLIB_TM_START(RTM_BOOTSTRAP, rtm);
  CKKS_BTS_CTX*    bts_ctx   = Get_bts_ctx((CKKS_EVALUATOR*)Get_eval(Context));
  uint32_t         num_slots = Get_ciph_slots(ciph);
  CKKS_BTS_PRECOM* precom    = Get_bts_precom(bts_ctx, num_slots);
  if (!precom) {
    Bootstrap_precom(num_slots);
    precom = Get_bts_precom(bts_ctx, num_slots);
  }
  VL_UI32* level_budget = Alloc_value_list(UI32_TYPE, 2);
  UI32_VALUE_AT(level_budget, 0) =
      Get_i32_value_at(Get_encode_params(precom), LEVEL_BUDGET);
  UI32_VALUE_AT(level_budget, 1) =
      Get_i32_value_at(Get_decode_params(precom), LEVEL_BUDGET);
  uint32_t num_iteration = 1;
  uint32_t precision     = 0;
  uint32_t q_cnt         = Get_primes_cnt(Get_q(Get_crt_context()));
  uint32_t bts_depth     = Get_bootstrap_depth(
      level_budget, num_iteration,
      ((CKKS_PARAMETER*)Get_param(Context))->_hamming_weight);
  Free_value_list(level_budget);
  // If we start with more towers, than we obtain from bootstrapping,
  // just return the original ciphertext
  if (Get_ciph_sf_degree(ciph) == 1 &&
      Get_ciph_prime_cnt(ciph) >= level_after_bts) {
    if (res != ciph) {
      Copy_ciph(res, ciph);
    }
    RTLIB_TM_END(RTM_BS_COPY, rtm);
    return res;
  }
  IS_TRUE(!level_after_bts || level_after_bts <= q_cnt - bts_depth,
          "The level set after bootstrapping is excessively high");
  // If level_after_bts not set, ciph will be raised to q_cnt
  uint32_t raise_level = level_after_bts ? level_after_bts + bts_depth : q_cnt;
  Eval_bootstrap(res, ciph, num_iteration, precision, raise_level, bts_ctx);
  RTLIB_TM_END(RTM_BOOTSTRAP, rtm);
  return res;
}

CIPHER Encrypt(CIPHER res, PLAIN plain) {
  Encrypt_msg(res, (CKKS_ENCRYPTOR*)Context->_encryptor, plain);
  return res;
}
