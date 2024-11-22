//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================
#include "util/ckks_bootstrap_context.h"

#include <omp.h>

#include "common/error.h"
#include "common/rt_config.h"
#include "common/rt_env.h"
#include "util/ckks_key_generator.h"
#include "util/ckks_parameters.h"

typedef VALUE_LIST
    VL_VL_VL_DCMPLX;  // VALUE_LIST<VALUE_LIST<VALUE_LIST<DCMPLX>>>

static const EVAL_SIN_POLY_INFO Eval_sin_poly_info[] = {
    {SPARSE_KIND,               K_SPARSE,              R_SPARSE,              UNIFORM_COEFF_SIZE,
     G_coefficients_sparse                                                                                                          },
    {UNIFORM_HW_UNDER_192,      K_UNIFORM_HW_192,      R_UNIFORM_HW_192,
     UNIFORM_COEFF_SIZE_HW_192,                                                                   G_coefficients_uniform_hw_192     },
    {UNIFORM_HW_ABOVE_192,      K_UNIFORM,             R_UNIFORM,             UNIFORM_COEFF_SIZE,
     G_coefficients_uniform                                                                                                         },
    {UNIFORM_EVEN_HW_UNDER_192, K_UNIFORM_EVEN_HW_192, R_UNIFORM_EVEN_HW_192,
     UNIFORM_EVEN_COEFF_SIZE_HW_192,                                                              G_even_coefficients_uniform_hw_192},
    {UNIFORM_EVEN_HW_ABOVE_192, K_UNIFORM_EVEN,        R_UNIFORM_EVEN,
     UNIFORM_EVEN_COEFF_SIZE,                                                                     G_even_coefficients_uniform       },
};

static bool Eval_sin_with_even_poly() {
  const char* bts_poly_env = getenv(ENV_BOOTSTRAP_EVEN_POLY);
  return bts_poly_env != NULL && (atoi(bts_poly_env) != 0);
}

const EVAL_SIN_POLY_INFO* Get_eval_sin_poly_info(uint32_t hamming_weight) {
  EVAL_SIN_POLY_KIND poly_kind = LAST_KIND;
  if (SPARSE_TERNARY) {
    poly_kind = SPARSE_KIND;
  } else {
    bool hw_under_192 =
        (hamming_weight > 0 && hamming_weight <= HAMMING_WEIGHT_THRESHOLD);
    if (Eval_sin_with_even_poly()) {
      poly_kind =
          hw_under_192 ? UNIFORM_EVEN_HW_UNDER_192 : UNIFORM_EVEN_HW_ABOVE_192;
    } else {
      poly_kind = hw_under_192 ? UNIFORM_HW_UNDER_192 : UNIFORM_HW_ABOVE_192;
    }
  }
  IS_TRUE(poly_kind != LAST_KIND, "not supported polynomial kind");
  const EVAL_SIN_POLY_INFO* poly_info = &Eval_sin_poly_info[poly_kind];
  IS_TRUE(poly_info->_poly_kind == poly_kind,
          "polynomial kind is inconsistent");
  return poly_info;
}

const uint32_t* Gen_depth_by_degree_table() {
  static const uint32_t depth_table[UPPER_BOUND_DEGREE + 1] = {
      [0 ... 4] = 3,  // degree in [0,4], depth = 3 - the Paterson-Stockmeyer
                      // algorithm is not used when degree < 5
      [5]                           = 4,   // degree in [5],         depth = 4
      [6 ... 13]                    = 5,   // degree in [6,13],      depth = 5
      [14 ... 27]                   = 6,   // degree in [14,27],     depth = 6
      [28 ... 59]                   = 7,   // degree in [28,59],     depth = 7
      [60 ... 119]                  = 8,   // degree in [60,119],    depth = 8
      [120 ... 247]                 = 9,   // degree in [120,247],   depth = 9
      [248 ... 495]                 = 10,  // degree in [248,495],   depth = 10
      [496 ... 1007]                = 11,  // degree in [496,1007],  depth = 11
      [1008 ... UPPER_BOUND_DEGREE] = 12   // degree in [1008,2031], depth = 12
  };
  return depth_table;
}

void Free_ckks_bts_precom(CKKS_BTS_PRECOM* precom) {
  if (precom == NULL) return;
  Free_value_list(precom->_encode_params);
  Free_value_list(precom->_decode_params);

  // free u0_pre and its contents
  VL_PTR* u0_pre = precom->_u0_pre;
  if (u0_pre) {
    FOR_ALL_ELEM(u0_pre, idx) {
      PLAINTEXT* plain = (PLAINTEXT*)Get_ptr_value_at(u0_pre, idx);
      Free_plaintext(plain);
    }
    Free_value_list(u0_pre);
  }

  // free u0hatt_pre and free innermost contents
  VL_PTR* u0_hatt_pre = precom->_u0hatt_pre;
  if (u0_hatt_pre) {
    FOR_ALL_ELEM(u0_hatt_pre, idx) {
      PLAINTEXT* plain = (PLAINTEXT*)Get_ptr_value_at(u0_hatt_pre, idx);
      Free_plaintext(plain);
    }
    Free_value_list(u0_hatt_pre);
  }

  // free u0_pre_fft and free innermost contents
  VL_VL_PTR* u0_pre_fft = precom->_u0_pre_fft;
  if (u0_pre_fft) {
    FOR_ALL_ELEM(u0_pre_fft, idx) {
      VL_PTR* vl1 = (VL_PTR*)Get_vl_value_at(u0_pre_fft, idx);
      FOR_ALL_ELEM(vl1, idx2) {
        PLAINTEXT* plain = (PLAINTEXT*)Get_ptr_value_at(vl1, idx2);
        Free_plaintext(plain);
      }
    }
    Free_value_list(u0_pre_fft);
  }

  // free u0hatt_pre_fft and free innermost contents
  VL_VL_PTR* u0hatt_pre_fft = precom->_u0hatt_pre_fft;
  if (u0hatt_pre_fft) {
    FOR_ALL_ELEM(u0hatt_pre_fft, idx) {
      VL_PTR* vl1 = (VL_PTR*)Get_vl_value_at(u0hatt_pre_fft, idx);
      FOR_ALL_ELEM(vl1, idx2) {
        PLAINTEXT* plain = (PLAINTEXT*)Get_ptr_value_at(vl1, idx2);
        Free_plaintext(plain);
      }
    }
    Free_value_list(u0hatt_pre_fft);
  }

  free(precom);
}

void Print_ckks_bts_precom(FILE* fp, CKKS_BTS_PRECOM* precom) {
  fprintf(fp, "Bootstrap preocmpute for %d\n", Get_slots_num(precom));
  fprintf(fp, "encode_params: ");
  Print_value_list(fp, Get_encode_params(precom));
  fprintf(fp, "decode_params: ");
  Print_value_list(fp, Get_decode_params(precom));

  VL_PLAIN* u0_pre = Get_u0_pre(precom);
  if (u0_pre) {
    fprintf(fp, "\nu0_pre: ");
    FOR_ALL_ELEM(u0_pre, idx) {
      PLAINTEXT* plain = (PLAINTEXT*)Get_ptr_value_at(u0_pre, idx);
      Print_plain(fp, plain);
    }
  }

  VL_PTR* u0_hatt_pre = precom->_u0hatt_pre;
  if (u0_hatt_pre) {
    fprintf(fp, "\nu0_hatt_pre: ");
    FOR_ALL_ELEM(u0_hatt_pre, idx) {
      PLAINTEXT* plain = (PLAINTEXT*)Get_ptr_value_at(u0_hatt_pre, idx);
      Print_plain(fp, plain);
    }
  }

  VL_VL_PTR* u0_pre_fft = precom->_u0_pre_fft;
  if (u0_pre_fft) {
    fprintf(fp, "\nu0_pre_fft[%ld][%ld]: ", Get_dim1_size(u0_pre_fft),
            Get_dim2_size(u0_pre_fft));
    FOR_ALL_ELEM(u0_pre_fft, idx) {
      VL_PTR* vl1 = (VL_PTR*)Get_vl_value_at(u0_pre_fft, idx);
      FOR_ALL_ELEM(vl1, idx2) {
        PLAINTEXT* plain = (PLAINTEXT*)Get_ptr_value_at(vl1, idx2);
        Print_plain(fp, plain);
      }
    }
  }

  VL_VL_PTR* u0hatt_pre_fft = precom->_u0hatt_pre_fft;
  if (u0hatt_pre_fft) {
    fprintf(fp, "\nu0hatt_pre_fft[%ld][%ld]: ", Get_dim1_size(u0hatt_pre_fft),
            Get_dim2_size(u0hatt_pre_fft));
    FOR_ALL_ELEM(u0hatt_pre_fft, idx) {
      VL_PTR* vl1 = (VL_PTR*)Get_vl_value_at(u0hatt_pre_fft, idx);
      FOR_ALL_ELEM(vl1, idx2) {
        PLAINTEXT* plain = (PLAINTEXT*)Get_ptr_value_at(vl1, idx2);
        Print_plain(fp, plain);
      }
    }
  }
}

void Print_ckks_bts_ctx(FILE* fp, CKKS_BTS_CTX* bts_ctx) {
  BTS_PRECOM_MAP* precomps = Get_bts_precomps(bts_ctx);
  BTS_PRECOM_MAP* iter;
  for (iter = precomps; iter != NULL; iter = iter->hh.next) {
    Print_ckks_bts_precom(fp, iter->_bts_precom);
  }
}

//! @brief Ensures that the index for rotation is positive and between 1 and
//! slots.
//! @param index signed rotation amount.
//! @param slots number of slots and size of vector that is rotated.
uint32_t Reduce_rotation(int32_t index, uint32_t slots) {
  int32_t is_lots = (int32_t)slots;

  // if slots is a power of 2
  if ((slots & (slots - 1)) == 0) {
    int32_t n = log2(slots);
    if (index >= 0) {
      return index - ((index >> n) << n);
    }
    return index + is_lots + (((-index) >> n) << n);
  }
  return (is_lots + index % is_lots) % is_lots;
}

void Find_linear_rot_idxs(LL* idx_list, CKKS_BTS_PRECOM* precom, uint32_t slots,
                          uint32_t m) {
  IS_TRUE(FALSE, "TO IMPL");
}

void Find_coeffslots_rot_index(LL* idx_list, CKKS_BTS_PRECOM* precom,
                               uint32_t slots, uint32_t m, bool encoding) {
  VL_I32* params =
      encoding ? Get_encode_params(precom) : Get_decode_params(precom);
  int32_t level_budget    = Get_i32_value_at(params, LEVEL_BUDGET);
  int32_t layers_collapse = Get_i32_value_at(params, LAYERS_COLL);
  int32_t rem_collapse    = Get_i32_value_at(params, LAYERS_REM);
  int32_t num_rot         = Get_i32_value_at(params, NUM_ROTATIONS);
  int32_t b               = Get_i32_value_at(params, BABY_STEP);
  int32_t g               = Get_i32_value_at(params, GIANT_STEP);
  int32_t num_rot_rem     = Get_i32_value_at(params, NUM_ROTATIONS_REM);
  int32_t b_rem           = Get_i32_value_at(params, BABY_STEP_REM);
  int32_t g_rem           = Get_i32_value_at(params, GIANT_STEP_REM);

  int32_t  stop     = -1;
  int32_t  flag_rem = 0;
  uint32_t mdiv4    = m / 4;

  if (rem_collapse != 0) {
    stop     = 0;
    flag_rem = 1;
  }

  int32_t start       = encoding ? stop + 1 : 0;
  int32_t end         = level_budget;
  int32_t slots_value = encoding ? slots : mdiv4;
  for (int32_t s = start; s < end; s++) {
    int32_t shift_value =
        encoding ? 1 << ((s - flag_rem) * layers_collapse + rem_collapse)
                 : 1 << (s * layers_collapse);
    for (int32_t j = 0; j < g; j++) {
      int32_t idx     = (j - ((num_rot + 1) / 2) + 1) * shift_value;
      int32_t rot_idx = Reduce_rotation(idx, slots_value);
      Insert_sort_uniq(idx_list, rot_idx);
    }
    for (int32_t i = 0; i < b; i++) {
      int32_t idx     = (g * i) * shift_value;
      int32_t rot_idx = Reduce_rotation(idx, mdiv4);
      Insert_sort_uniq(idx_list, rot_idx);
    }
  }

  if (flag_rem) {
    int32_t s           = level_budget - flag_rem;
    int32_t shift_value = encoding ? 1 : 1 << (s * layers_collapse);
    for (int32_t j = 0; j < g_rem; j++) {
      int32_t idx     = (j - ((num_rot_rem + 1) / 2) + 1) * shift_value;
      int32_t rot_idx = Reduce_rotation(idx, slots_value);
      Insert_sort_uniq(idx_list, rot_idx);
    }
    for (int32_t i = 0; i < b_rem; i++) {
      int32_t idx     = g_rem * i * shift_value;
      int32_t rot_idx = Reduce_rotation(idx, mdiv4);
      Insert_sort_uniq(idx_list, rot_idx);
    }
  }

  uint32_t slots4 = slots * 4;
  // additional automorphisms are needed for sparse bootstrapping
  if (slots4 != m) {
    for (uint32_t j = 1; j < m / slots4; j <<= 1) {
      Insert_sort_uniq(idx_list, j * slots);
    }
  }
}

LL* Find_rot_indices(CKKS_BTS_PRECOM* precom, uint32_t slots, uint32_t m) {
  VL_I32* encode_params = Get_encode_params(precom);
  VL_I32* decode_params = Get_decode_params(precom);
  int32_t enc_budget    = Get_i32_value_at(encode_params, LEVEL_BUDGET);
  int32_t dec_budget    = Get_i32_value_at(decode_params, LEVEL_BUDGET);
  bool    is_lt_bts     = (enc_budget == 1) && (dec_budget == 1);

  LL* idx_list = Alloc_link_list();
  if (is_lt_bts) {
    Find_linear_rot_idxs(idx_list, precom, slots, m);
  } else {
    Find_coeffslots_rot_index(idx_list, precom, slots, m, true);
    Find_coeffslots_rot_index(idx_list, precom, slots, m, false);
  }

  uint32_t mdiv4 = m / 4;
  Delete_node(idx_list, 0);
  Delete_node(idx_list, mdiv4);
  return idx_list;
}

VL_VL_PLAIN* Rotate_precomp(CKKS_BTS_CTX* bts_ctx, VL_VL_VL_DCMPLX* coeffs,
                            VL_DCMPLX* ksipows, VL_UI32* rot_group, bool flag,
                            double scale, uint32_t level, bool encoding) {
  CKKS_PARAMETER*  params      = Get_bts_params(bts_ctx);
  CKKS_ENCODER*    encoder     = Get_bts_encoder(bts_ctx);
  size_t           q_cnt       = params->_num_primes;
  size_t           p_cnt       = Get_primes_cnt(Get_p(params->_crt_context));
  size_t           ring_degree = params->_poly_degree;
  size_t           m           = ring_degree * 2;
  size_t           slots       = LIST_LEN(rot_group);
  CKKS_BTS_PRECOM* precom      = Get_bts_precom(bts_ctx, slots);
  IS_TRUE(precom,
          "Precomputations were not generated, Please call Bootstrap_setup");

  VL_I32* encode_params   = Get_encode_params(precom);
  int32_t level_budget    = Get_i32_value_at(encode_params, LEVEL_BUDGET);
  int32_t layers_collapse = Get_i32_value_at(encode_params, LAYERS_COLL);
  int32_t rem_collapse    = Get_i32_value_at(encode_params, LAYERS_REM);
  int32_t num_rot         = Get_i32_value_at(encode_params, NUM_ROTATIONS);
  int32_t b               = Get_i32_value_at(encode_params, BABY_STEP);
  int32_t g               = Get_i32_value_at(encode_params, GIANT_STEP);
  int32_t num_rot_rem     = Get_i32_value_at(encode_params, NUM_ROTATIONS_REM);
  int32_t b_rem           = Get_i32_value_at(encode_params, BABY_STEP_REM);
  int32_t g_rem           = Get_i32_value_at(encode_params, GIANT_STEP_REM);

  int32_t stop     = -1;
  int32_t flag_rem = 0;

  if (rem_collapse != 0) {
    stop     = 0;
    flag_rem = 1;
  }

  uint32_t     rem_index  = encoding ? 0 : level_budget - 1;
  VL_VL_PLAIN* rot_plains = Alloc_value_list(VL_PTR_TYPE, level_budget);
  for (uint32_t i = 0; i < (uint32_t)level_budget; i++) {
    if (flag_rem == 1 && i == rem_index) {
      // remainder corresponds to index 0 in encoding and
      // to last index in decoding
      VL_VALUE_AT(rot_plains, i) = Alloc_value_list(PTR_TYPE, num_rot_rem);
    } else {
      VL_VALUE_AT(rot_plains, i) = Alloc_value_list(PTR_TYPE, num_rot);
    }
  }

  int32_t  start     = encoding ? stop + 1 : 0;
  int32_t  end       = encoding ? level_budget : level_budget - flag_rem;
  int32_t  cond      = encoding ? start : end - 1;
  uint32_t enc_level = level ? level + 1 : q_cnt - level_budget + 1;
  uint32_t dec_level = level ? level + level_budget : q_cnt;
  for (int32_t s = start; s < end; s++) {
    uint32_t    plain_level = encoding ? enc_level + s : dec_level - s;
    VALUE_LIST* vl_plain    = Get_vl_value_at(rot_plains, s);
    for (int32_t i = 0; i < b; i++) {
      for (int32_t j = 0; j < g; j++) {
        int32_t dim2 = g * i + j;
        if (dim2 != num_rot) {
          int32_t shift_value =
              encoding ? ((s - flag_rem) * layers_collapse + rem_collapse)
                       : (s * layers_collapse);
          int32_t     index = -g * i * (1 << shift_value);
          uint32_t    rot   = Reduce_rotation(index, m / 4);
          VALUE_LIST* vl    = Get_vl_l2_value_at(coeffs, s, dim2);
          if ((flag_rem == 0) && (s == cond)) {
            // for encoding: do the scaling only at the first set of
            // coefficients for decoding: do the scaling only at the last set of
            // coefficients
            FOR_ALL_ELEM(vl, k) {
              DCMPLX vl_at_k         = Get_dcmplx_value_at(vl, k);
              DCMPLX_VALUE_AT(vl, k) = vl_at_k * scale;
            }
          }
          VALUE_LIST* rot_vl = Alloc_value_list(DCMPLX_TYPE, LIST_LEN(vl));
          Rotate_vector(rot_vl, vl, rot);

          PLAINTEXT* rot_plain = Alloc_plaintext();
          Encode_ext_at_level(rot_plain, encoder, rot_vl, plain_level,
                              LIST_LEN(rot_vl), p_cnt);
          PTR_VALUE_AT(vl_plain, dim2) = (PTR)rot_plain;

          Free_value_list(rot_vl);
        }
      }
    }
  }

  if (flag_rem) {
    int32_t     dim1        = encoding ? stop : level_budget - flag_rem;
    int32_t     shift_value = encoding ? 1 : (1 << (dim1 * layers_collapse));
    VALUE_LIST* vl_plain    = Get_vl_value_at(rot_plains, dim1);
    uint32_t    plain_level =
        encoding ? enc_level : dec_level - level_budget + flag_rem;
    for (int32_t i = 0; i < b_rem; i++) {
      for (int32_t j = 0; j < g_rem; j++) {
        int32_t dim2 = g_rem * i + j;
        if (dim2 != num_rot_rem) {
          uint32_t    rot = Reduce_rotation(-g_rem * i * shift_value, m / 4);
          VALUE_LIST* vl  = Get_vl_l2_value_at(coeffs, dim1, dim2);
          FOR_ALL_ELEM(vl, k) {
            DCMPLX vl_at_k         = Get_dcmplx_value_at(vl, k);
            DCMPLX_VALUE_AT(vl, k) = vl_at_k * scale;
          }

          VALUE_LIST* rot_vl = Alloc_value_list(DCMPLX_TYPE, LIST_LEN(vl));
          Rotate_vector(rot_vl, vl, rot);

          PLAINTEXT* rot_plain = Alloc_plaintext();
          Encode_ext_at_level(rot_plain, encoder, rot_vl, plain_level,
                              LIST_LEN(rot_vl), p_cnt);
          Free_value_list(rot_vl);
          PTR_VALUE_AT(vl_plain, dim2) = (PTR)rot_plain;
        }
      }
    }
  }
  return rot_plains;
}

VL_VL_DCMPLX* Coeff_enc_one_level(VL_DCMPLX* ksipows, VL_UI32* rot_group,
                                  bool flag) {
  uint32_t dim       = LIST_LEN(ksipows) - 1;
  uint32_t slots     = LIST_LEN(rot_group);
  uint32_t log_slots = (uint32_t)log2(slots);

  // Each outer iteration from the FFT algorithm can be written a weighted sum
  // of three terms: the input shifted right by a power of two, the unshifted
  // input, and the input shifted left by a power of two. For each outer
  // iteration (log2(size) in total), the matrix coeff stores the coefficients
  // in the following order: the coefficients associated to the input shifted
  // right, the coefficients for the non-shifted input and the coefficients
  // associated to the input shifted left.
  VL_VL_DCMPLX* coeff = Alloc_2d_value_list(DCMPLX_TYPE, 3 * log_slots, slots);

  for (uint32_t m = slots; m > 1; m >>= 1) {
    uint32_t   s               = log2(m) - 1;
    VL_DCMPLX* coeff_s         = Get_vl_value_at(coeff, s);
    VL_DCMPLX* coeff_logslots  = Get_vl_value_at(coeff, s + log_slots);
    VL_DCMPLX* coeff_2logslots = Get_vl_value_at(coeff, s + 2 * log_slots);
    for (uint32_t k = 0; k < slots; k += m) {
      uint32_t lenh = m >> 1;
      uint32_t lenq = m << 2;

      for (uint32_t j = 0; j < lenh; j++) {
        uint32_t j_twiddle =
            (lenq - Get_ui32_value_at(rot_group, j) % lenq) * (dim / lenq);
        if (flag && (m == 2)) {
          DCMPLX val = cexp(-M_PI / 2 * I);
          DCMPLX w   = val * Get_dcmplx_value_at(ksipows, j_twiddle);
          Set_dcmplx_value(coeff_logslots, j + k, val);        // not shifted
          Set_dcmplx_value(coeff_2logslots, j + k, val);       // shift left
          Set_dcmplx_value(coeff_logslots, j + k + lenh, -w);  // not shifted
          Set_dcmplx_value(coeff_s, j + k + lenh, w);          // shifted right
        } else {
          DCMPLX w = Get_dcmplx_value_at(ksipows, j_twiddle);
          Set_dcmplx_value(coeff_logslots, j + k, 1);          // not shifted
          Set_dcmplx_value(coeff_2logslots, j + k, 1);         // shifted left
          Set_dcmplx_value(coeff_logslots, j + k + lenh, -w);  // not shifted
          Set_dcmplx_value(coeff_s, j + k + lenh, w);          // shift right
        }
      }
    }
  }
  return coeff;
}

VL_VL_DCMPLX* Coeff_dec_one_level(VL_DCMPLX* ksipows, VL_UI32* rot_group,
                                  bool flag) {
  uint32_t dim       = LIST_LEN(ksipows) - 1;
  uint32_t slots     = LIST_LEN(rot_group);
  uint32_t log_slots = (uint32_t)log2(slots);

  // Each outer iteration from the FFT algorithm can be written a weighted sum
  // of three terms: the input shifted right by a power of two, the unshifted
  // input, and the input shifted left by a power of two. For each outer
  // iteration (log2(size) in total), the matrix coeff stores the coefficients
  // in the following order: the coefficients associated to the input shifted
  // right, the coefficients for the non-shifted input and the coefficients
  // associated to the input shifted left.
  VL_VL_DCMPLX* coeff = Alloc_2d_value_list(DCMPLX_TYPE, 3 * log_slots, slots);

  for (uint32_t m = slots; m > 1; m >>= 1) {
    uint32_t   s               = log2(m) - 1;
    VL_DCMPLX* coeff_s         = Get_vl_value_at(coeff, s);
    VL_DCMPLX* coeff_logslots  = Get_vl_value_at(coeff, s + log_slots);
    VL_DCMPLX* coeff_2logslots = Get_vl_value_at(coeff, s + 2 * log_slots);
    for (uint32_t k = 0; k < slots; k += m) {
      uint32_t lenh = m >> 1;
      uint32_t lenq = m << 2;

      for (uint32_t j = 0; j < lenh; j++) {
        uint32_t j_twiddle =
            (Get_ui32_value_at(rot_group, j) % lenq) * (dim / lenq);
        if (flag && (m == 2)) {
          DCMPLX val = cexp(M_PI / 2 * I);
          DCMPLX w   = val * Get_dcmplx_value_at(ksipows, j_twiddle);
          Set_dcmplx_value(coeff_logslots, j + k, val);        // not shifted
          Set_dcmplx_value(coeff_2logslots, j + k, w);         // shift left
          Set_dcmplx_value(coeff_logslots, j + k + lenh, -w);  // not shifted
          Set_dcmplx_value(coeff_s, j + k + lenh, val);        // shifted right
        } else {
          DCMPLX w = Get_dcmplx_value_at(ksipows, j_twiddle);
          Set_dcmplx_value(coeff_logslots, j + k, 1);          // not shifted
          Set_dcmplx_value(coeff_2logslots, j + k, w);         // shifted left
          Set_dcmplx_value(coeff_logslots, j + k + lenh, -w);  // not shifted
          Set_dcmplx_value(coeff_s, j + k + lenh, 1);          // shift right
        }
      }
    }
  }
  return coeff;
}

VL_UI32* Select_layers(uint32_t log_slots, uint32_t budget) {
  uint32_t layers = ceil((double)log_slots / budget);
  uint32_t rows   = log_slots / layers;
  uint32_t rem    = log_slots % layers;

  uint32_t dim = rows;
  if (rem != 0) {
    dim = rows + 1;
  }

  // the above choice ensures dim <= budget
  if (dim < budget) {
    layers -= 1;
    rows = log_slots / layers;
    rem  = log_slots - rows * layers;
    dim  = rows;

    if (rem != 0) {
      dim = rows + 1;
    }

    // the above choice ensures dim >= budget
    while (dim != budget) {
      rows -= 1;
      rem = log_slots - rows * layers;
      dim = rows;
      if (rem != 0) {
        dim = rows + 1;
      }
    }
  }
  VL_UI32* ret          = Alloc_value_list(UI32_TYPE, 3);
  UI32_VALUE_AT(ret, 0) = layers;
  UI32_VALUE_AT(ret, 1) = rows;
  UI32_VALUE_AT(ret, 2) = rem;
  return ret;
}

VL_I32* Get_colls_fft_params(uint32_t slots, uint32_t level_budget,
                             uint32_t dim1) {
  // Need to compute how many layers are collapsed in each of the
  // level from the budget.
  // If there is no exact division between the maximum number of possible
  // levels (log(slots)) and the level budget, the last level will contain
  // the remaining layers collapsed.
  int32_t  layers_coll;
  int32_t  rem_coll;
  uint32_t log_slots = (uint32_t)log2(slots);

  VL_UI32* dims = Select_layers(log_slots, level_budget);
  layers_coll   = Get_ui32_value_at(dims, 0);
  rem_coll      = Get_ui32_value_at(dims, 2);

  int32_t flag_rem = 1;
  if (rem_coll == 0) {
    flag_rem = 0;
  }

  uint32_t num_rot     = (1 << (layers_coll + 1)) - 1;
  uint32_t num_rot_rem = (1 << (rem_coll + 1)) - 1;

  // Computing the baby-step b and the giant-step g for the collapsed layers for
  // decoding.
  int32_t b, g;
  if (dim1 == 0 || dim1 > num_rot) {
    if (num_rot > 7) {
      g = (1 << ((int32_t)(layers_coll / 2) + 2));
    } else {
      g = (1 << ((int32_t)(layers_coll / 2) + 1));
    }
  } else {
    g = dim1;
  }

  b             = (num_rot + 1) / g;
  int32_t b_rem = 0;
  int32_t g_rem = 0;

  if (flag_rem) {
    if (num_rot_rem > 7) {
      g_rem = (1 << ((int32_t)(rem_coll / 2) + 2));
    } else {
      g_rem = (1 << ((int32_t)(rem_coll / 2) + 1));
    }
    b_rem = (num_rot_rem + 1) / g_rem;
  }

  // the order should be the same as CKKS_BOOT_PARAMS
  int32_t vals[TOTAL_PARAMS] = {(int32_t)level_budget, layers_coll, rem_coll,
                                (int32_t)num_rot,      b,           g,
                                (int32_t)num_rot_rem,  b_rem,       g_rem};
  VL_I32* ret                = Alloc_value_list(I32_TYPE, TOTAL_PARAMS);
  Init_i32_value_list(ret, TOTAL_PARAMS, vals);

  Free_value_list(dims);

  return ret;
}

VL_VL_VL_DCMPLX* Coeff_collapse(VL_DCMPLX* ksipows, VL_UI32* rot_group,
                                uint32_t level_budget, bool flag,
                                bool encoding) {
  VL_VL_VL_DCMPLX* coeff;
  uint32_t         slots     = LIST_LEN(rot_group);
  uint32_t         log_slots = (uint32_t)log2(slots);
  int32_t          layers_coll, rem_coll;

  // Need to compute how many layers are collapsed in each of the level
  // from the budget. If there is no exact division between the maximum
  // number of possible levels (log(slots)) and the level budget, the
  // last level will contain the remaining layers collapsed.
  VL_UI32* dims = Select_layers(log_slots, level_budget);
  layers_coll   = Get_ui32_value_at(dims, 0);
  rem_coll      = Get_ui32_value_at(dims, 2);

  int32_t dim_coll = (int32_t)level_budget;
  int32_t flag_rem = 1;
  if (rem_coll == 0) {
    flag_rem = 0;
  }

  uint32_t num_rot     = (1 << (layers_coll + 1)) - 1;
  uint32_t num_rot_rem = (1 << (rem_coll + 1)) - 1;

  // Computing the coefficients for encoding for the given level budget
  VL_VL_DCMPLX* coeff1 = encoding
                             ? Coeff_enc_one_level(ksipows, rot_group, flag)
                             : Coeff_dec_one_level(ksipows, rot_group, flag);

  // coeff stores the coefficients for the given budget of levels
  coeff = Alloc_value_list(VL_PTR_TYPE, dim_coll);
  FOR_ALL_ELEM(coeff, idx) {
    VL_VL_DCMPLX* dim2;
    if (flag_rem) {
      bool after_remainder =
          (encoding && idx >= 1) || (!encoding && idx < level_budget - 1);
      if (after_remainder) {
        // after remainder
        dim2 = Alloc_2d_value_list(DCMPLX_TYPE, num_rot, slots);
      } else {
        // remainder corresponds to the first index in encoding and to the last
        // one in decoding
        dim2 = Alloc_2d_value_list(DCMPLX_TYPE, num_rot_rem, slots);
      }
    } else {
      dim2 = Alloc_2d_value_list(DCMPLX_TYPE, num_rot, slots);
    }
    VL_VALUE_AT(coeff, idx) = dim2;
  }

  for (int32_t s = 0; s < dim_coll; s++) {
    int32_t top =
        encoding ? (int32_t)log_slots - (dim_coll - 1 - s) * layers_coll - 1
                 : s * layers_coll;
    bool is_rem =
        flag_rem && ((encoding && s == 0) || (!encoding && s == dim_coll - 1));
    int32_t end_l = is_rem ? rem_coll : layers_coll;
    for (int32_t l = 0; l < end_l; l++) {
      if (l == 0) {
        VL_DCMPLX* coeff_s0    = Get_vl_l2_value_at(coeff, s, 0);
        VL_DCMPLX* coeff_s1    = Get_vl_l2_value_at(coeff, s, 1);
        VL_DCMPLX* coeff_s2    = Get_vl_l2_value_at(coeff, s, 2);
        VL_DCMPLX* coeff1_top1 = Get_vl_value_at(coeff1, top);
        VL_DCMPLX* coeff1_top2 = Get_vl_value_at(coeff1, top + log_slots);
        VL_DCMPLX* coeff1_top3 = Get_vl_value_at(coeff1, top + 2 * log_slots);
        Init_dcmplx_value_list(coeff_s0, LIST_LEN(coeff1_top1),
                               Get_dcmplx_values(coeff1_top1));
        Init_dcmplx_value_list(coeff_s1, LIST_LEN(coeff1_top2),
                               Get_dcmplx_values(coeff1_top2));
        Init_dcmplx_value_list(coeff_s2, LIST_LEN(coeff1_top3),
                               Get_dcmplx_values(coeff1_top3));
      } else {
        uint32_t      t          = 0;
        VL_VL_DCMPLX* coeff_s    = Get_vl_value_at(coeff, s);
        VL_VL_DCMPLX* coeff_temp = Alloc_2d_value_list(
            DCMPLX_TYPE, Get_dim1_size(coeff_s), Get_dim2_size(coeff_s));
        if (encoding) {
          for (int32_t u = 0; u < (1 << (l + 1)) - 1; u++) {
            VL_DCMPLX* temp_u = Get_vl_l2_value_at(coeff, s, u);
            for (uint32_t k = 0; k < slots; k++) {
              int32_t  idx      = k - (1 << (top - l));
              uint32_t rot_idx  = Reduce_rotation(idx, slots);
              int32_t  idx2     = k + (1 << (top - l));
              uint32_t rot_idx2 = Reduce_rotation(idx2, slots);

              VL_DCMPLX* coeff_s0 = Get_vl_value_at(coeff_temp, u + t);
              VL_DCMPLX* coeff_s1 = Get_vl_value_at(coeff_temp, u + t + 1);
              VL_DCMPLX* coeff_s2 = Get_vl_value_at(coeff_temp, u + t + 2);

              VL_DCMPLX* coeff1_top = Get_vl_value_at(coeff1, top - l);
              VL_DCMPLX* coeff1_top_slots =
                  Get_vl_value_at(coeff1, top - l + log_slots);
              VL_DCMPLX* coeff1_top_2slots =
                  Get_vl_value_at(coeff1, top - l + 2 * log_slots);

              DCMPLX val1 = Get_dcmplx_value_at(coeff_s0, k);
              DCMPLX val2 = Get_dcmplx_value_at(coeff_s1, k);
              DCMPLX val3 = Get_dcmplx_value_at(coeff_s2, k);
              val1 += Get_dcmplx_value_at(coeff1_top, k) *
                      Get_dcmplx_value_at(temp_u, rot_idx);
              val2 += Get_dcmplx_value_at(coeff1_top_slots, k) *
                      Get_dcmplx_value_at(temp_u, k);
              val3 += Get_dcmplx_value_at(coeff1_top_2slots, k) *
                      Get_dcmplx_value_at(temp_u, rot_idx2);
              Set_dcmplx_value(coeff_s0, k, val1);
              Set_dcmplx_value(coeff_s1, k, val2);
              Set_dcmplx_value(coeff_s2, k, val3);
            }
            t += 1;
          }
        } else {
          for (; t < 3; t++) {
            for (int32_t u = 0; u < (1 << (l + 1)) - 1; u++) {
              VL_DCMPLX* temp_u = Get_vl_l2_value_at(coeff, s, u);
              for (uint32_t k = 0; k < slots; k++) {
                if (t == 0) {
                  VL_DCMPLX* coeff_s0   = Get_vl_value_at(coeff_temp, u);
                  VL_DCMPLX* coeff1_top = Get_vl_value_at(coeff1, top + l);
                  DCMPLX     val1       = Get_dcmplx_value_at(coeff_s0, k);
                  val1 += Get_dcmplx_value_at(coeff1_top, k) *
                          Get_dcmplx_value_at(temp_u, k);
                  Set_dcmplx_value(coeff_s0, k, val1);
                }
                if (t == 1) {
                  VL_DCMPLX* coeff_s1 =
                      Get_vl_value_at(coeff_temp, u + (1 << l));
                  VL_DCMPLX* coeff1_top_slots =
                      Get_vl_value_at(coeff1, top + l + log_slots);
                  DCMPLX val2 = Get_dcmplx_value_at(coeff_s1, k);
                  val2 += Get_dcmplx_value_at(coeff1_top_slots, k) *
                          Get_dcmplx_value_at(temp_u, k);
                  Set_dcmplx_value(coeff_s1, k, val2);
                }
                if (t == 2) {
                  VL_DCMPLX* coeff_s2 =
                      Get_vl_value_at(coeff_temp, u + (1 << (l + 1)));
                  VL_DCMPLX* coeff1_top_2slots =
                      Get_vl_value_at(coeff1, top + l + 2 * log_slots);
                  DCMPLX val3 = Get_dcmplx_value_at(coeff_s2, k);
                  val3 += Get_dcmplx_value_at(coeff1_top_2slots, k) *
                          Get_dcmplx_value_at(temp_u, k);
                  Set_dcmplx_value(coeff_s2, k, val3);
                }
              }
            }
          }
        }

        // copy coeff_temp values to coeff
        FOR_ALL_ELEM(coeff_s, idx) {
          VL_DCMPLX* dim2            = Get_vl_value_at(coeff_s, idx);
          VL_DCMPLX* coeff_temp_dim2 = Get_vl_value_at(coeff_temp, idx);
          Init_dcmplx_value_list(dim2, LIST_LEN(coeff_temp_dim2),
                                 Get_dcmplx_values(coeff_temp_dim2));
        }
        Free_value_list(coeff_temp);
      }
    }
  }

  Free_value_list(dims);
  Free_value_list(coeff1);
  return coeff;
}

VL_VL_PLAIN* Coeffs2slots_precomp(CKKS_BTS_CTX* bts_ctx, VL_DCMPLX* ksipows,
                                  VL_UI32* rot_group, bool flag, double scale,
                                  uint32_t level) {
  CKKS_PARAMETER*  params      = Get_bts_params(bts_ctx);
  size_t           ring_degree = params->_poly_degree;
  size_t           m           = ring_degree * 2;
  size_t           slots       = LIST_LEN(rot_group);
  CKKS_BTS_PRECOM* precom      = Get_bts_precom(bts_ctx, slots);
  IS_TRUE(precom,
          "Precomputations were not generated, Please call Bootstrap_setup");

  VL_I32* encode_params = Get_encode_params(precom);
  int32_t level_budget  = Get_i32_value_at(encode_params, LEVEL_BUDGET);

  VL_VL_VL_DCMPLX* coeffs;
  if (slots == m / 4) {
    // fully packed mode
    coeffs = Coeff_collapse(ksipows, rot_group, level_budget, flag, true);
  } else {
    // sparsely packed mode
    VL_VL_VL_DCMPLX* coeffs1 =
        Coeff_collapse(ksipows, rot_group, level_budget, false, true);
    VL_VL_VL_DCMPLX* coeffs2 =
        Coeff_collapse(ksipows, rot_group, level_budget, true, true);

    // merge coeffs1 and coeffs2
    coeffs = Alloc_value_list(VL_PTR_TYPE, Get_dim1_size(coeffs1));
    FOR_ALL_ELEM(coeffs, idx1) {
      VALUE_LIST* dim2 = Alloc_value_list(
          VL_PTR_TYPE, LIST_LEN(Get_vl_value_at(coeffs1, idx1)));
      VL_VALUE_AT(coeffs, idx1) = dim2;
      FOR_ALL_ELEM(dim2, idx2) {
        VALUE_LIST* dim3 = Alloc_value_list(
            DCMPLX_TYPE, Get_dim3_size(coeffs1) + Get_dim3_size(coeffs2));
        VL_VALUE_AT(dim2, idx2) = dim3;
        FOR_ALL_ELEM(Get_vl_l2_value_at(coeffs1, idx1, idx2), idx3) {
          DCMPLX_VALUE_AT(dim3, idx3) =
              DCMPLX_VALUE_AT(VL_L2_VALUE_AT(coeffs1, idx1, idx2), idx3);
        }
        size_t start = Get_dim3_size(coeffs1);
        FOR_ALL_ELEM(Get_vl_l2_value_at(coeffs2, idx1, idx2), idx3) {
          DCMPLX_VALUE_AT(dim3, start + idx3) =
              DCMPLX_VALUE_AT(VL_L2_VALUE_AT(coeffs2, idx1, idx2), idx3);
        }
      }
    }
    Free_value_list(coeffs1);
    Free_value_list(coeffs2);
  }

  // multiply coeffs with (1/N * 1/K * 1/(q0/sf)) to reduce redundant Mul_const
  // and mul_level consumption. N is poly_degree in (inv_SF = (1/N) *
  // trans(conj_SF)). K is norm of |I(X)| in ((q0/sf)*I + m). q0 is first prime,
  // and sf is scale factor. to reduce precision loss in encoding SF,
  // decompose factor into multiple-part, then multiply each part with
  // decomposed matrix of SF.
  double factor = 1.0 / ring_degree;
  if (UNIFORM_TERNARY) {
    factor /= Get_eval_sin_upper_bound_k(params->_hamming_weight);
  }

  CRT_CONTEXT* crt         = params->_crt_context;
  MODULUS*     mod_head    = Get_modulus_head(Get_q_primes(crt));
  double       sf          = params->_scaling_factor;
  double       q0_val      = Get_mod_val(mod_head);
  double       q0_sf_ratio = round(log2(q0_val / sf));
  factor /= pow(2, q0_sf_ratio);

  factor = pow(factor, 1. / level_budget);
  IS_TRUE(LIST_LEN(coeffs) == level_budget,
          "number of decomposed matrix should be level_budget");
  for (uint32_t idx0 = 0; idx0 < level_budget; ++idx0) {
    VL_VL_DCMPLX* coeff = Get_vl_value_at(coeffs, idx0);
    FOR_ALL_ELEM(coeff, idx1) {
      VL_DCMPLX* dim2 = Get_vl_value_at(coeff, idx1);
      FOR_ALL_ELEM(dim2, idx2) {
        DCMPLX val = Get_dcmplx_value_at(dim2, idx2);
        Set_dcmplx_value(dim2, idx2, val * factor);
      }
    }
  }

  VL_VL_PLAIN* rot_plains = Rotate_precomp(bts_ctx, coeffs, ksipows, rot_group,
                                           flag, scale, level, true);

  Free_value_list(coeffs);
  return rot_plains;
}

VL_VL_PLAIN* Slots2coeffs_precomp(CKKS_BTS_CTX* bts_ctx, VL_DCMPLX* ksipows,
                                  VL_UI32* rot_group, bool flag, double scale,
                                  uint32_t level) {
  CKKS_PARAMETER*  params      = Get_bts_params(bts_ctx);
  size_t           ring_degree = params->_poly_degree;
  size_t           m           = ring_degree * 2;
  size_t           slots       = LIST_LEN(rot_group);
  CKKS_BTS_PRECOM* precom      = Get_bts_precom(bts_ctx, slots);
  IS_TRUE(precom,
          "Precomputations were not generated, Please call Bootstrap_setup");

  VL_I32* encode_params = Get_encode_params(precom);
  int32_t level_budget  = Get_i32_value_at(encode_params, LEVEL_BUDGET);

  VL_VL_VL_DCMPLX* coeffs;
  if (slots == m / 4) {
    // fully packed mode
    coeffs = Coeff_collapse(ksipows, rot_group, level_budget, flag, false);
  } else {
    // sparsely packed mode
    VL_VL_VL_DCMPLX* coeffs1 =
        Coeff_collapse(ksipows, rot_group, level_budget, false, false);
    VL_VL_VL_DCMPLX* coeffs2 =
        Coeff_collapse(ksipows, rot_group, level_budget, true, false);

    // merge coeffs1 and coeffs2
    coeffs = Alloc_value_list(VL_PTR_TYPE, Get_dim1_size(coeffs1));
    FOR_ALL_ELEM(coeffs, idx1) {
      VALUE_LIST* dim2 = Alloc_value_list(
          VL_PTR_TYPE, LIST_LEN(Get_vl_value_at(coeffs1, idx1)));
      VL_VALUE_AT(coeffs, idx1) = dim2;
      FOR_ALL_ELEM(dim2, idx2) {
        VALUE_LIST* dim3 = Alloc_value_list(
            DCMPLX_TYPE, Get_dim3_size(coeffs1) + Get_dim3_size(coeffs2));
        VL_VALUE_AT(dim2, idx2) = dim3;
        FOR_ALL_ELEM(Get_vl_l2_value_at(coeffs1, idx1, idx2), idx3) {
          DCMPLX_VALUE_AT(dim3, idx3) =
              DCMPLX_VALUE_AT(VL_L2_VALUE_AT(coeffs1, idx1, idx2), idx3);
        }
        size_t start = Get_dim3_size(coeffs1);
        FOR_ALL_ELEM(Get_vl_l2_value_at(coeffs2, idx1, idx2), idx3) {
          DCMPLX_VALUE_AT(dim3, start + idx3) =
              DCMPLX_VALUE_AT(VL_L2_VALUE_AT(coeffs2, idx1, idx2), idx3);
        }
      }
    }
    Free_value_list(coeffs1);
    Free_value_list(coeffs2);
  }

  VL_VL_PLAIN* rot_plains = Rotate_precomp(bts_ctx, coeffs, ksipows, rot_group,
                                           flag, scale, level, false);

  Free_value_list(coeffs);
  return rot_plains;
}

VL_PLAIN* Linear_trans_precomp(CKKS_BTS_CTX* bts_ctx, MATRIX* matrix,
                               double scale, uint32_t level) {
  FMT_ASSERT(MATRIX_ROWS(matrix) == MATRIX_COLS(matrix),
             "linear trans precompute is not square matrix");

  uint32_t         slots   = MATRIX_ROWS(matrix);
  CKKS_BTS_PRECOM* precom  = Get_bts_precom(bts_ctx, slots);
  CKKS_ENCODER*    encoder = Get_bts_encoder(bts_ctx);
  IS_TRUE(precom,
          "Precomputations were not generated, Please call Bootstrap_setup");

  // comute the baby-step and giant step
  uint32_t b_step = Get_inner_dim(precom) == 0 ? (uint32_t)ceil(sqrt(slots))
                                               : Get_inner_dim(precom);
  uint32_t g_step = (uint32_t)ceil((double)slots / b_step);

  VL_PLAIN*   result   = Alloc_value_list(PTR_TYPE, slots);
  VL_DCMPLX*  diag     = Alloc_value_list(DCMPLX_TYPE, slots);
  VALUE_LIST* rot_diag = Alloc_value_list(DCMPLX_TYPE, slots);
  for (uint32_t j = 0; j < g_step; j++) {
    int32_t offset = -1 * (int32_t)b_step * j;
    for (uint32_t i = 0; i < b_step; i++) {
      uint32_t diag_idx = b_step * j + i;
      if (diag_idx < slots) {
        Diagonal_matrix(diag, matrix, diag_idx);
        FOR_ALL_ELEM(diag, idx) { DCMPLX_VALUE_AT(diag, idx) *= scale; }
        Rotate_vector(rot_diag, diag, offset);

        PLAINTEXT* rot_plain = Alloc_plaintext();
        ENCODE_AT_LEVEL(rot_plain, encoder, rot_diag, level + 1);
        PTR_VALUE_AT(result, diag_idx) = (PTR)rot_plain;
      }
    }
  }
  Free_value_list(diag);
  Free_value_list(rot_diag);
  return result;
}

VL_PLAIN* Linear_trans_precomp_sparse(CKKS_BTS_CTX* bts_ctx, MATRIX* matrix_a,
                                      MATRIX* matrix_b, uint32_t orientation,
                                      double scale, uint32_t level) {
  FMT_ASSERT(MATRIX_ROWS(matrix_a) == MATRIX_COLS(matrix_a),
             "linear trans precompute is not square matrix");

  uint32_t         slots   = MATRIX_ROWS(matrix_a);
  CKKS_BTS_PRECOM* precom  = Get_bts_precom(bts_ctx, slots);
  CKKS_ENCODER*    encoder = Get_bts_encoder(bts_ctx);
  IS_TRUE(precom,
          "Precomputations were not generated, Please call Bootstrap_setup");

  // comute the baby-step and giant step
  uint32_t b_step = Get_inner_dim(precom) == 0 ? (uint32_t)ceil(sqrt(slots))
                                               : Get_inner_dim(precom);
  uint32_t g_step = (uint32_t)ceil((double)slots / b_step);

  VL_PLAIN*  result   = Alloc_value_list(PTR_TYPE, slots);
  VL_DCMPLX* diag     = Alloc_value_list(DCMPLX_TYPE, 2 * slots);
  VL_DCMPLX* rot_diag = Alloc_value_list(DCMPLX_TYPE, 2 * slots);
  if (orientation == 0) {
    // vertical concatenation - used during homomorphic encoding
    VL_DCMPLX* diag_a = Alloc_value_list(DCMPLX_TYPE, slots);
    VL_DCMPLX* diag_b = Alloc_value_list(DCMPLX_TYPE, slots);
    for (uint32_t j = 0; j < g_step; j++) {
      int32_t offset = -1 * (int32_t)b_step * j;
      for (uint32_t i = 0; i < b_step; i++) {
        uint32_t diag_idx = b_step * j + i;
        if (diag_idx < slots) {
          Diagonal_matrix(diag_a, matrix_a, diag_idx);
          Diagonal_matrix(diag_b, matrix_b, diag_idx);
          FOR_ALL_ELEM(diag_a, idx_a) {
            DCMPLX_VALUE_AT(diag, idx_a) =
                DCMPLX_VALUE_AT(diag_a, idx_a) * scale;
          }
          FOR_ALL_ELEM(diag_b, idx_b) {
            DCMPLX_VALUE_AT(diag, slots + idx_b) =
                DCMPLX_VALUE_AT(diag_b, idx_b) * scale;
          }

          Rotate_vector(rot_diag, diag, offset);

          PLAINTEXT* rot_plain = Alloc_plaintext();
          ENCODE_AT_LEVEL(rot_plain, encoder, rot_diag, level + 1);
          PTR_VALUE_AT(result, diag_idx) = (PTR)rot_plain;
        }
      }
    }
  } else {
    // horizontal concatention - used during homomorpihc decoding
    MATRIX* matrix_ab = Alloc_dcmplx_matrix(
        MATRIX_ROWS(matrix_a), MATRIX_COLS(matrix_a) + MATRIX_COLS(matrix_b),
        NULL);
    FOR_ALL_MAT_ELEM(matrix_a, row, col) {
      DCMPLX_MATRIX_ELEM(matrix_ab, row, col) =
          DCMPLX_MATRIX_ELEM(matrix_a, row, col);
    }
    size_t col_start = MATRIX_COLS(matrix_a);
    FOR_ALL_MAT_ELEM(matrix_b, row, col) {
      DCMPLX_MATRIX_ELEM(matrix_ab, row, col + col_start) =
          DCMPLX_MATRIX_ELEM(matrix_b, row, col);
    }
    for (uint32_t j = 0; j < g_step; j++) {
      int32_t offset = -1 * (int32_t)b_step * j;
      for (uint32_t i = 0; i < b_step; i++) {
        uint32_t diag_idx = b_step * j + i;
        if (diag_idx < slots) {
          Diagonal_matrix(diag, matrix_ab, diag_idx);
          FOR_ALL_ELEM(diag, diag_idx) {
            DCMPLX_VALUE_AT(diag, diag_idx) *= scale;
          }

          Rotate_vector(rot_diag, diag, offset);

          PLAINTEXT* rot_plain = Alloc_plaintext();
          ENCODE_AT_LEVEL(rot_plain, encoder, rot_diag, level + 1);
          PTR_VALUE_AT(result, diag_idx) = (PTR)rot_plain;
        }
      }
    }
    Free_matrix(matrix_ab);
  }
  Free_value_list(diag);
  Free_value_list(rot_diag);
  return result;
}

void Bootstrap_setup(CKKS_BTS_CTX* bts_ctx, VL_UI32* level_budget,
                     VL_UI32* dim1, uint32_t num_slots) {
  CKKS_PARAMETER* params      = Get_bts_params(bts_ctx);
  CRT_CONTEXT*    crt         = params->_crt_context;
  size_t          ring_degree = params->_poly_degree;
  size_t          m           = ring_degree * 2;
  uint32_t        slots       = (num_slots == 0) ? m / 4 : num_slots;

  CKKS_BTS_PRECOM* precom = Get_bts_precom(bts_ctx, slots);
  if (precom != NULL) {
    FMT_ASSERT(Get_inner_dim(precom) == Get_ui32_value_at(dim1, 0),
               "Bootstrap_setup: inner dimension do not match");
    FMT_ASSERT(I32_VALUE_AT(Get_encode_params(precom), 0) ==
                   (int32_t)UI32_VALUE_AT(level_budget, 0),
               "Bootstrap_setup: encode params do not match level_budget");
    FMT_ASSERT(I32_VALUE_AT(Get_decode_params(precom), 0) ==
                   (int32_t)UI32_VALUE_AT(level_budget, 1),
               "Bootstrap_setup: decode params do not match level_budget");
    return;
  }
  precom = Alloc_ckks_bts_precom();
  Set_inner_dim(precom, Get_ui32_value_at(dim1, 0));
  Set_slots_num(precom, slots);
  Add_bts_precom(bts_ctx, precom);

  // Perform some checks on the level budget and compute parameters
  double log_slots = log2(slots);
  FOR_ALL_ELEM(level_budget, idx) {
    uint32_t val = UI32_VALUE_AT(level_budget, idx);
    if (idx < 2) {
      if (val > log_slots) {
        DEV_WARN(
            "WARNING: the level budget for encoding cannot be this large."
            "The budget was changed to %d\n",
            (uint32_t)log_slots);
        UI32_VALUE_AT(level_budget, idx) = (uint32_t)log_slots;
      }
      if (val < 1) {
        DEV_WARN(
            "WARNING: the level budget for encoding has to be at least 1."
            " The budget was changed to 1\n");
        UI32_VALUE_AT(level_budget, idx) = 1;
      }
    } else {
      break;
    }
  }
  VL_I32* encode_params = Get_colls_fft_params(
      slots, UI32_VALUE_AT(level_budget, 0), UI32_VALUE_AT(dim1, 0));
  VL_I32* decode_params = Get_colls_fft_params(
      slots, UI32_VALUE_AT(level_budget, 1), UI32_VALUE_AT(dim1, 1));
  Set_encode_params(precom, encode_params);
  Set_decode_params(precom, decode_params);

  uint32_t slots_4   = 4 * slots;
  bool     is_sparse = (m != slots_4) ? true : false;

  // computes indices for all primitive roots of unity
  VL_UI32* rot_group = Alloc_value_list(UI32_TYPE, slots);
  uint32_t five_pows = 1;
  FOR_ALL_ELEM(rot_group, idx) {
    UI32_VALUE_AT(rot_group, idx) = five_pows;
    five_pows *= 5;
    five_pows %= slots_4;
  }

  // computes all powers of a primitive root of unity exp(2 * M_PI/m)
  VL_DCMPLX* ksi_pows = Alloc_value_list(DCMPLX_TYPE, slots_4 + 1);
  for (size_t idx = 0; idx < slots_4; idx++) {
    double angle                   = 2.0 * M_PI * idx / slots_4;
    DCMPLX_VALUE_AT(ksi_pows, idx) = cos(angle) + sin(angle) * I;
  }
  DCMPLX_VALUE_AT(ksi_pows, slots_4) = DCMPLX_VALUE_AT(ksi_pows, 0);

  // extract the modulus prior to bootstrapping
  uint64_t q0     = Get_mod_val(Get_modulus_head(Get_q_primes(crt)));
  double   dbl_q0 = (double)q0;

  UINT128_T factor    = (UINT128_T)1 << (uint32_t)round(log2(dbl_q0));
  double    pre       = dbl_q0 / factor;
  double    k         = SPARSE_TERNARY ? K_SPARSE : 1.0;
  double    scale_enc = pre / k;
  double    scale_dec = 1 / pre;

  uint32_t approx_mod_depth = Get_mul_depth_internal(params->_hamming_weight);
  int32_t  enc_budget       = Get_i32_value_at(encode_params, LEVEL_BUDGET);
  int32_t  dec_budget       = Get_i32_value_at(decode_params, LEVEL_BUDGET);
  uint32_t bts_depth        = approx_mod_depth + enc_budget + dec_budget;

  // compute # of levels to remain when encoding the coefficients
  uint32_t level_0 = Get_mult_depth(params) + 1;
  if (AUTO_SCALE_EXT) {
    level_0 -= 1;
  }
  FMT_ASSERT(level_0 > enc_budget, "not enough levels");
  FMT_ASSERT(level_0 > bts_depth, "need set a larger multiply depth");
  uint32_t level_enc = level_0 - enc_budget;
  uint32_t level_dec = level_0 - bts_depth;

  bool is_lt_bts = (enc_budget == 1) && (dec_budget == 1);

  if (is_lt_bts) {
    DCMPLX  val     = I;
    MATRIX* u0      = Alloc_dcmplx_matrix(slots, slots, NULL);
    MATRIX* u1      = Alloc_dcmplx_matrix(slots, slots, NULL);
    MATRIX* u0hat_t = Alloc_dcmplx_matrix(slots, slots, NULL);
    MATRIX* u1hat_t = Alloc_dcmplx_matrix(slots, slots, NULL);

    FOR_ALL_MAT_ELEM(u0, row, col) {
      uint32_t temp_idx = col * Get_ui32_value_at(rot_group, row) % slots_4;
      DCMPLX   temp1    = Get_dcmplx_value_at(ksi_pows, temp_idx);
      DCMPLX   temp2    = val * temp1;
      DCMPLX_MATRIX_ELEM(u0, row, col)      = temp1;
      DCMPLX_MATRIX_ELEM(u0hat_t, col, row) = conj(temp1);
      DCMPLX_MATRIX_ELEM(u1, row, col)      = temp2;
      DCMPLX_MATRIX_ELEM(u1hat_t, col, row) = conj(temp2);
    }
    if (!is_sparse) {
      Set_u0hatt_pre(
          precom, Linear_trans_precomp(bts_ctx, u0hat_t, scale_enc, level_enc));
      Set_u0_pre(precom,
                 Linear_trans_precomp(bts_ctx, u0, scale_dec, level_dec));
    } else {
      Set_u0hatt_pre(precom,
                     Linear_trans_precomp_sparse(bts_ctx, u0hat_t, u1hat_t, 0,
                                                 scale_enc, level_enc));
      Set_u0_pre(precom, Linear_trans_precomp_sparse(bts_ctx, u0, u1, 1,
                                                     scale_dec, level_dec));
    }
    Free_matrix(u0);
    Free_matrix(u1);
    Free_matrix(u0hat_t);
    Free_matrix(u1hat_t);
  } else {
    Set_u0hatt_pre_fft(precom,
                       Coeffs2slots_precomp(bts_ctx, ksi_pows, rot_group, false,
                                            scale_enc, level_enc));
    Set_u0_pre_fft(precom, Slots2coeffs_precomp(bts_ctx, ksi_pows, rot_group,
                                                false, scale_dec, level_dec));
  }
  Free_value_list(rot_group);
  Free_value_list(ksi_pows);
}

void Bootstrap_keygen(CKKS_BTS_CTX* bts_ctx, uint32_t slots) {
  CKKS_PARAMETER*  params = Get_bts_params(bts_ctx);
  CKKS_BTS_PRECOM* precom = Get_bts_precom(bts_ctx, slots);
  IS_TRUE(precom,
          "Precomputations were not generated, Please call Bootstrap_setup");
  // if bootstrap keys has been generated for slots, just return
  if (Get_keygen_status(precom)) return;

  CKKS_KEY_GENERATOR* gen         = Get_bts_gen(bts_ctx);
  size_t              ring_degree = params->_poly_degree;
  size_t              m           = ring_degree * 2;
  if (slots == 0) {
    slots = m / 4;
  }

  // computing all indices for baby-step giant-step procedure
  LL*      rot_idxs = Find_rot_indices(precom, slots, m);
  size_t   num_rots = Get_link_list_cnt(rot_idxs);
  int32_t  rot_vals[num_rots];
  uint32_t idx = 0;
  FOR_ALL_LL_ELEM(rot_idxs, node) { rot_vals[idx++] = node->_val; }
  Generate_rot_maps(gen, num_rots, rot_vals);
  Free_link_list(rot_idxs);

  // generate conjunction key
  uint32_t auto_idx = m - 1;
  if (!Get_auto_key(gen, auto_idx)) {
    SWITCH_KEY* conj_key = Alloc_switch_key();
    Generate_conj_key(conj_key, gen);
    Insert_auto_key(gen, auto_idx, conj_key);
  }
  Set_keygen_status(precom, slots);
}

SWITCH_KEY* Get_rotation_key(CKKS_BTS_CTX* bts_ctx, int32_t rot_val) {
  CKKS_KEY_GENERATOR* keygen   = Get_bts_gen(bts_ctx);
  uint32_t            auto_idx = Get_precomp_auto_idx(keygen, rot_val);
  FMT_ASSERT(auto_idx, "cannot get precompute automorphism index");
  SWITCH_KEY* key = Get_auto_key(keygen, auto_idx);
  FMT_ASSERT(key, "cannot find auto key");
  return key;
}

CIPHERTEXT* Rotate_iteration(CIPHERTEXT* result, CKKS_BTS_CTX* bts_ctx,
                             VL_VL_PLAIN* conj_pre, VL_VL_I32* rot_in,
                             VL_VL_I32* rot_out, int32_t step, bool encoding,
                             bool is_rem) {
  CKKS_PARAMETER*  ckks_params = Get_bts_params(bts_ctx);
  CRT_CONTEXT*     crt         = ckks_params->_crt_context;
  CKKS_EVALUATOR*  eval        = Get_bts_eval(bts_ctx);
  size_t           degree      = ckks_params->_poly_degree;
  size_t           slots       = Get_ciph_slots(result);
  CKKS_BTS_PRECOM* precom      = Get_bts_precom(bts_ctx, slots);
  IS_TRUE(precom,
          "Precomputations were not generated, Please call Bootstrap_setup");

  VL_I32* param =
      encoding ? Get_encode_params(precom) : Get_decode_params(precom);
  IS_TRUE(param->_length == TOTAL_PARAMS, "invalid length of param");
  int32_t level_budget = Get_i32_value_at(param, LEVEL_BUDGET);
  int32_t num_rots     = Get_i32_value_at(param, NUM_ROTATIONS);
  int32_t g            = Get_i32_value_at(param, GIANT_STEP);
  int32_t b            = Get_i32_value_at(param, BABY_STEP);
  int32_t num_rots_rem = Get_i32_value_at(param, NUM_ROTATIONS_REM);
  int32_t g_rem        = Get_i32_value_at(param, GIANT_STEP_REM);
  int32_t b_rem        = Get_i32_value_at(param, BABY_STEP_REM);

  CIPHERTEXT* outer     = Alloc_ciphertext();
  CIPHERTEXT* inner     = Alloc_ciphertext();
  CIPHERTEXT* temp_ciph = Alloc_ciphertext();

  int32_t giant_step = is_rem ? g_rem : g;
  int32_t baby_step  = is_rem ? b_rem : b;
  int32_t num_rot    = is_rem ? num_rots_rem : num_rots;
  int32_t level_idx  = encoding ? level_budget - 1 : 0;

  if (is_rem || step != level_idx) {
    Rescale_ciphertext(result, result, eval);
  }

  size_t       prime_cnt   = Get_ciph_prime_cnt(result);
  VL_CRTPRIME* p_modulus   = Get_p_primes(crt);
  size_t       p_prime_cnt = ckks_params->_num_p_primes;

  POLYNOMIAL first, temp_poly;
  Alloc_poly_data(&first, degree, prime_cnt, p_prime_cnt);
  Alloc_poly_data(&temp_poly, degree, prime_cnt, p_prime_cnt);

  // computes the NTTs for each CRT limb (for the hoisted automorphisms used
  // later on)
  VALUE_LIST* digits = Switch_key_precompute(Get_c1(result), crt);

  VL_CIPH*    fast_rot  = Alloc_value_list(PTR_TYPE, giant_step);
  VALUE_LIST* vl_rot_in = Get_vl_value_at(rot_in, step);

  for (int32_t j = 0; j < giant_step; j++) {
    int32_t val               = Get_i32_value_at(vl_rot_in, j);
    PTR_VALUE_AT(fast_rot, j) = (PTR)Alloc_ciphertext();
    CIPHERTEXT* rot_ciph      = (CIPHERTEXT*)Get_ptr_value_at(fast_rot, j);
    if (val != 0) {
      SWITCH_KEY* rot_key = Get_rotation_key(bts_ctx, val);
      Fast_rotate_ext(rot_ciph, result, val, rot_key, eval, digits, true);
    } else {
      Switch_key_ext(rot_ciph, result, eval, true);
    }
  }

  for (int32_t i = 0; i < baby_step; i++) {
    int32_t     giant      = giant_step * i;
    CIPHERTEXT* rot_ciph_0 = (CIPHERTEXT*)Get_ptr_value_at(fast_rot, 0);
    VALUE_LIST* vl_pre     = Get_vl_value_at(conj_pre, step);
    PLAINTEXT*  plain      = (PLAINTEXT*)Get_ptr_value_at(vl_pre, giant);
    // derive a sub plain to make sure poly level consistent with rot_ciph
    PLAINTEXT sub_plain;
    Derive_plain(&sub_plain, plain, Get_num_q(Get_c0(rot_ciph_0)),
                 Get_num_p(Get_c0(rot_ciph_0)));
    Mul_plaintext(inner, rot_ciph_0, &sub_plain, eval);
    // continue the loop
    for (int32_t j = 1; j < giant_step; j++) {
      CIPHERTEXT* rot_ciph_j = (CIPHERTEXT*)Get_ptr_value_at(fast_rot, j);
      if ((giant + j) != (int32_t)(num_rot)) {
        PLAINTEXT* plain_j = (PLAINTEXT*)Get_ptr_value_at(vl_pre, giant + j);
        // derive a sub plain to make sure poly level consistent with rot_ciph
        PLAINTEXT sub_plain_j;
        Derive_plain(&sub_plain_j, plain_j, Get_num_q(Get_c0(rot_ciph_j)),
                     Get_num_p(Get_c0(rot_ciph_j)));
        Mul_plaintext(temp_ciph, rot_ciph_j, &sub_plain_j, eval);
        Add_ciphertext(inner, inner, temp_ciph, eval);
      }
    }

    if (i == 0) {
      Copy_polynomial(&first, Get_c0(inner));
      memset(Get_c0(inner)->_data, 0, Get_poly_mem_size(Get_c0(inner)));
      Copy_ciphertext(outer, inner);
    } else {
      VALUE_LIST* vl_rot_out = Get_vl_value_at(rot_out, step);
      int32_t     val        = Get_i32_value_at(vl_rot_out, i);
      if (val != 0) {
        CIPHERTEXT* reduce_ciph = Alloc_ciphertext();
        Init_ciphertext_from_poly(reduce_ciph, &first, inner->_scaling_factor,
                                  inner->_sf_degree, Get_ciph_slots(inner));
        Copy_polynomial(Get_c0(reduce_ciph), Get_c0(inner));
        Reduce_rns_base(Get_c1(reduce_ciph), Get_c1(inner), crt);
        uint32_t auto_idx = Get_precomp_auto_idx(eval->_keygen, val);
        IS_TRUE(auto_idx, "cannot get auto_idx");
        VALUE_LIST* precomp = Get_precomp_auto_order(eval->_keygen, auto_idx);
        Rotate_poly(&temp_poly, Get_c0(reduce_ciph), auto_idx, precomp, crt);
        Add_poly(&first, &first, &temp_poly, crt, p_modulus);

        VALUE_LIST* inner_digits =
            Switch_key_precompute(Get_c1(reduce_ciph), crt);
        SWITCH_KEY* rot_key = Get_rotation_key(bts_ctx, val);
        Fast_rotate_ext(temp_ciph, reduce_ciph, val, rot_key, eval,
                        inner_digits, FALSE);
        Add_ciphertext(outer, outer, temp_ciph, eval);

        Free_ciphertext(reduce_ciph);
        Free_switch_key_precomputed(inner_digits);
      } else {
        Copy_polynomial(&temp_poly, Get_c0(inner));
        Add_poly(&first, &first, &temp_poly, crt, p_modulus);
        memset(Get_c0(inner)->_data, 0, Get_poly_mem_size(Get_c0(inner)));
        Add_ciphertext(outer, outer, inner, eval);
      }
    }
  }
  Add_poly(Get_c0(outer), Get_c0(outer), &first, crt, p_modulus);
  Reduce_rns_base(Get_c0(result), Get_c0(outer), crt);
  Reduce_rns_base(Get_c1(result), Get_c1(outer), crt);
  // set scaling_factor from outer
  result->_scaling_factor = outer->_scaling_factor;
  result->_sf_degree      = outer->_sf_degree;

  FOR_ALL_ELEM(fast_rot, idx) {
    CIPHERTEXT* ciph = (CIPHERTEXT*)Get_ptr_value_at(fast_rot, idx);
    Free_ciphertext(ciph);
  }
  Free_value_list(fast_rot);
  Free_switch_key_precomputed(digits);
  Free_ciphertext(outer);
  Free_ciphertext(inner);
  Free_ciphertext(temp_ciph);
  Free_poly_data(&first);
  Free_poly_data(&temp_poly);

  return result;
}

CIPHERTEXT* Coeff_slots_transform(CIPHERTEXT* result, CIPHERTEXT* ciph,
                                  VL_VL_PLAIN* conj_pre, CKKS_BTS_CTX* bts_ctx,
                                  bool encoding) {
  CKKS_PARAMETER*  ckks_params = Get_bts_params(bts_ctx);
  size_t           degree      = ckks_params->_poly_degree;
  size_t           order       = degree * 2;
  size_t           slots       = Get_ciph_slots(ciph);
  CKKS_BTS_PRECOM* precom      = Get_bts_precom(bts_ctx, slots);
  IS_TRUE(precom,
          "Precomputations were not generated, Please call Bootstrap_setup");

  VL_I32* param =
      encoding ? Get_encode_params(precom) : Get_decode_params(precom);
  IS_TRUE(param->_length == TOTAL_PARAMS, "invalid length of param");
  int32_t level_budget    = Get_i32_value_at(param, LEVEL_BUDGET);
  int32_t layers_collapse = Get_i32_value_at(param, LAYERS_COLL);
  int32_t rem_collapse    = Get_i32_value_at(param, LAYERS_REM);
  int32_t num_rots        = Get_i32_value_at(param, NUM_ROTATIONS);
  int32_t g               = Get_i32_value_at(param, GIANT_STEP);
  int32_t b               = Get_i32_value_at(param, BABY_STEP);
  int32_t num_rots_rem    = Get_i32_value_at(param, NUM_ROTATIONS_REM);
  int32_t g_rem           = Get_i32_value_at(param, GIANT_STEP_REM);
  int32_t b_rem           = Get_i32_value_at(param, BABY_STEP_REM);

  int32_t stop     = -1;
  int32_t flag_rem = 0;
  if (rem_collapse) {
    stop     = 0;
    flag_rem = 1;
  }

  int32_t start       = encoding ? stop + 1 : 0;
  int32_t end         = encoding ? level_budget : level_budget - flag_rem;
  int32_t slots_value = encoding ? slots : order / 4;
  // precompute the inner and outer rotations
  VL_VL_I32* rot_in    = Alloc_value_list(VL_PTR_TYPE, level_budget);
  uint32_t   rem_index = encoding ? 0 : level_budget - 1;
  for (uint32_t i = 0; i < (uint32_t)level_budget; i++) {
    // remainder corresponds to index 0 in encoding and to last index in
    // decoding
    if (flag_rem == 1 && i == rem_index) {
      VL_VALUE_AT(rot_in, i) = Alloc_value_list(I32_TYPE, num_rots_rem + 1);
    } else {
      VL_VALUE_AT(rot_in, i) = Alloc_value_list(I32_TYPE, num_rots + 1);
    }
  }

  VL_VL_I32* rot_out = Alloc_value_list(VL_PTR_TYPE, level_budget);
  for (size_t i = 0; i < (size_t)level_budget; i++) {
    VL_VALUE_AT(rot_out, i) = Alloc_value_list(I32_TYPE, b + b_rem);
  }

  for (int32_t s = start; s < end; s++) {
    VALUE_LIST* vl_rot_in = Get_vl_value_at(rot_in, s);
    int32_t     shift_value =
        encoding ? ((s - flag_rem) * layers_collapse + rem_collapse)
                     : (s * layers_collapse);
    for (int32_t j = 0; j < g; j++) {
      I32_VALUE_AT(vl_rot_in, j) = Reduce_rotation(
          (j - ((num_rots + 1) / 2) + 1) * (1 << shift_value), slots_value);
    }
    VALUE_LIST* vl_rot_out = Get_vl_value_at(rot_out, s);
    for (int32_t i = 0; i < b; i++) {
      I32_VALUE_AT(vl_rot_out, i) =
          Reduce_rotation((g * i) * (1 << shift_value), order / 4);
    }
  }

  if (flag_rem) {
    int32_t     s           = encoding ? stop : level_budget - flag_rem;
    int32_t     shift_value = encoding ? 1 : (1 << (s * layers_collapse));
    VALUE_LIST* vl_rot_in   = Get_vl_value_at(rot_in, s);
    for (int32_t j = 0; j < g_rem; j++) {
      I32_VALUE_AT(vl_rot_in, j) = Reduce_rotation(
          (j - ((num_rots_rem + 1) / 2) + 1) * shift_value, slots_value);
    }
    VALUE_LIST* vl_rot_out = Get_vl_value_at(rot_out, s);
    for (int32_t i = 0; i < b_rem; i++) {
      I32_VALUE_AT(vl_rot_out, i) =
          Reduce_rotation((g_rem * i) * shift_value, order / 4);
    }
  }

  Init_ciphertext_from_ciph(result, ciph, ciph->_scaling_factor,
                            ciph->_sf_degree);
  Copy_ciphertext(result, ciph);

  // hoisted automorphisms
  if (encoding) {
    for (int32_t s = end - 1; s > start - 1; s--) {
      Rotate_iteration(result, bts_ctx, conj_pre, rot_in, rot_out, s, encoding,
                       FALSE);
    }
  } else {
    for (int32_t s = start; s < end; s++) {
      Rotate_iteration(result, bts_ctx, conj_pre, rot_in, rot_out, s, encoding,
                       FALSE);
    }
  }
  if (flag_rem) {
    int32_t s = encoding ? stop : level_budget - flag_rem;
    Rotate_iteration(result, bts_ctx, conj_pre, rot_in, rot_out, s, encoding,
                     TRUE);
  }

  Free_value_list(rot_in);
  Free_value_list(rot_out);

  return result;
}

CIPHERTEXT* Coeffs_to_slots(CIPHERTEXT* result, CIPHERTEXT* ciph,
                            VL_VL_PLAIN* conj_pre, CKKS_BTS_CTX* bts_ctx) {
  Coeff_slots_transform(result, ciph, conj_pre, bts_ctx, TRUE);
  return result;
}

CIPHERTEXT* Slots_to_coeffs(CIPHERTEXT* result, CIPHERTEXT* ciph,
                            VL_VL_PLAIN* conj_pre, CKKS_BTS_CTX* bts_ctx) {
  Coeff_slots_transform(result, ciph, conj_pre, bts_ctx, FALSE);
  return result;
}

CIPHERTEXT* Linear_transform(CIPHERTEXT* result, CIPHERTEXT* ciph,
                             VL_PLAIN* conj_pre, CKKS_BTS_CTX* bts_ctx) {
  IS_TRUE(FALSE, "unimplemented");
  return NULL;
}

void Apply_double_angle_iterations(CIPHERTEXT* ciph, uint32_t num_iter,
                                   CKKS_BTS_CTX* bts_ctx) {
  CKKS_EVALUATOR* eval      = Get_bts_eval(bts_ctx);
  SWITCH_KEY*     relin_key = eval->_keygen->_relin_key;
  int32_t         r         = num_iter;
  for (int32_t j = 1; j < r + 1; j++) {
    Mul_ciphertext(ciph, ciph, ciph, relin_key, eval);
    Add_ciphertext(ciph, ciph, ciph, eval);
    double scalar = -1.0 / pow((2.0 * M_PI), pow(2.0, j - r));
    Add_const(ciph, ciph, scalar, eval);
    Rescale_ciphertext(ciph, ciph, eval);
  }
}

// Transform values(level 0 of poly) to rns polynomial
void Transform_values_from_level0(POLYNOMIAL* res, POLYNOMIAL* poly,
                                  CRT_CONTEXT* crt) {
  CRT_PRIMES* q_primes    = Get_q(crt);
  size_t      q_prime_cnt = Get_primes_cnt(q_primes);
  CRT_PRIME*  prime_head  = Get_prime_head(q_primes);
  uint32_t    degree      = Get_rdgree(res);
  IS_TRUE(degree == Get_rdgree(poly), "unmatched ring degree");
  IS_TRUE(Get_poly_alloc_len(res) >= Get_poly_len(poly) &&
              Get_num_alloc_primes(res) <= q_prime_cnt,
          "not enough memory");
  // just copy data for first level
  int64_t* res_data = Get_poly_coeffs(res);
  memcpy(res_data, Get_poly_coeffs(poly), sizeof(int64_t) * degree);
  res_data += degree;
  // fill up the rest data with proper moduli
  int64_t old_mod = Get_modulus_val(Get_nth_prime(prime_head, 0));
  for (size_t i = 1; i < Get_num_q(res); i++) {
    int64_t new_mod = Get_modulus_val(Get_nth_prime(prime_head, i));
    for (size_t val_idx = 0; val_idx < degree; val_idx++) {
      *res_data = Switch_modulus(Get_coeff_at(poly, val_idx), old_mod, new_mod);
      res_data++;
    }
  }
}

// Perform approx mod reduction: chebyshev + double angle
void Eval_approx_mod(CKKS_BTS_CTX* bts_ctx, CIPHERTEXT* out, CIPHERTEXT* in,
                     VL_DBL* coeffs, double lb, double ub) {
  CKKS_PARAMETER*           params         = Get_bts_params(bts_ctx);
  CKKS_EVALUATOR*           eval           = Get_bts_eval(bts_ctx);
  size_t                    hamming_weight = params->_hamming_weight;
  const EVAL_SIN_POLY_INFO* eval_sin_poly_info =
      Get_eval_sin_poly_info(hamming_weight);
  EVAL_SIN_POLY_KIND poly_kind = eval_sin_poly_info->_poly_kind;
  uint32_t num_iter = Get_eval_sin_double_angle_num(params->_hamming_weight);

  // for even polynomial, perform coordinate transformation: y= x-1/(4*K)
  if (poly_kind == UNIFORM_EVEN_HW_UNDER_192 ||
      poly_kind == UNIFORM_EVEN_HW_ABOVE_192) {
    double const_val = -1. / (4. * Get_eval_sin_upper_bound_k(hamming_weight));
    Add_const(in, in, const_val, eval);
  }

  Eval_chebyshev(bts_ctx, out, in, coeffs, lb, ub);
  IS_TRACE_CMD(Print_cipher_poly(Get_trace_file(), "chebyshev", out));

  // double-angle iterations
  if (UNIFORM_TERNARY || SPARSE_TERNARY) {
    if (AUTO_SCALE) {
      Rescale_ciphertext(out, out, eval);
    }

    Apply_double_angle_iterations(out, num_iter, bts_ctx);
    IS_TRACE_CMD(Print_cipher_poly(Get_trace_file(), "double_angle", out));
  }
}

CIPHERTEXT* Eval_bootstrap(CIPHERTEXT* res, CIPHERTEXT* ciph,
                           uint32_t num_iterations, uint32_t precision,
                           uint32_t raise_level, CKKS_BTS_CTX* bts_ctx) {
  RTLIB_TM_START(RTM_BS_EVAL, rtm);
  IS_TRUE(
      num_iterations == 1 || num_iterations == 2,
      "CKKS Iterative Bootstrapping is only supported for 1 or 2 iterations");

  if (num_iterations > 1) {
    IS_TRUE(FALSE, "TODO: bootstrap for num_iterations > 1");
  }
  uint32_t            slots  = Get_ciph_slots(ciph);
  CKKS_BTS_PRECOM*    precom = Get_bts_precom(bts_ctx, slots);
  CKKS_PARAMETER*     params = Get_bts_params(bts_ctx);
  CKKS_EVALUATOR*     eval   = Get_bts_eval(bts_ctx);
  CKKS_KEY_GENERATOR* keygen = Get_bts_gen(bts_ctx);
  IS_TRUE(precom,
          "Precomputations were not generated, Please call Bootstrap_setup");
  IS_TRUE(Get_keygen_status(precom),
          "Bootstrap keys were not generated, Please call Bootstrap_keygen");

  size_t      ring_degree = params->_poly_degree;
  size_t      m           = ring_degree * 2;
  SWITCH_KEY* conj_key    = Get_auto_key(keygen, m - 1);
  FMT_ASSERT(conj_key, "cannot find conj key");

  CRT_CONTEXT* crt          = params->_crt_context;
  MODULUS*     mod_head     = Get_modulus_head(Get_q_primes(crt));
  int64_t      mod_head_val = Get_mod_val(mod_head);
  double       sf           = params->_scaling_factor;
  int32_t      deg          = round(log2((double)mod_head_val / sf));

  IS_TRACE_CMD(
      Print_cipher_msg(Get_trace_file(), "before bts", ciph, DEF_MSG_LEN));
  // raise mod (q)
  CIPHERTEXT* raised = Alloc_ciphertext();
  Init_ciphertext_from_ciph(raised, ciph, ciph->_scaling_factor,
                            ciph->_sf_degree);
  Copy_ciphertext(raised, ciph);

  // Please note that only sf_degree = 1 is supported, if sf_degree greater than
  // 1, it will be automatically rescaled to 1.
  while (Get_ciph_sf_degree(raised) > 1) {
    Rescale_ciphertext(raised, raised, eval);
  }

  // We only use the level 0 ciphertext here. All other towers are automatically
  // ignored to make CKKS bootstrapping faster.
  if (Is_ntt(Get_c0(raised))) Conv_ntt2poly_inplace(Get_c0(raised), crt);
  if (Is_ntt(Get_c1(raised))) Conv_ntt2poly_inplace(Get_c1(raised), crt);
  CIPHERTEXT* new_ciph = Alloc_ciphertext();
  size_t      q_cnt    = Get_primes_cnt(Get_q(crt));
  // If raise level not set, ciph will be raised to q_cnt
  if (!raise_level) raise_level = q_cnt;
  IS_TRUE(raise_level <= q_cnt,
          "The raise level must be less than or equal to q_cnt");
  Init_ciphertext(new_ciph, ring_degree, raise_level, 0,
                  raised->_scaling_factor, raised->_sf_degree, slots);
  Transform_values_from_level0(Get_c0(new_ciph), Get_c0(raised), crt);
  Transform_values_from_level0(Get_c1(new_ciph), Get_c1(raised), crt);
  Free_ciphertext(raised);
  // convert new_ciph to ntt
  Conv_poly2ntt_inplace(Get_c0(new_ciph), crt);
  Conv_poly2ntt_inplace(Get_c1(new_ciph), crt);

  IS_TRACE_CMD(Print_cipher_poly(Get_trace_file(), "raised", new_ciph));

  // mod reduction
  size_t                    hamming_weight = params->_hamming_weight;
  const EVAL_SIN_POLY_INFO* eval_sin_poly_info =
      Get_eval_sin_poly_info(hamming_weight);
  uint32_t      coeff_size  = eval_sin_poly_info->_coeff_size;
  const double* dbl_coeff   = eval_sin_poly_info->_coeff;
  VL_DBL*       coefficient = Alloc_value_list(DBL_TYPE, coeff_size);
  Init_dbl_value_list(coefficient, LIST_LEN(coefficient), (double*)dbl_coeff);

  // no linear transformations are needed for Chebyshev series
  // as the range has been normalized to [-1,1]
  double coeff_lower_bound = -1;
  double coeff_upper_bound = 1;

  VL_I32* encode_params = Get_encode_params(precom);
  VL_I32* decode_params = Get_decode_params(precom);
  int32_t enc_budget    = Get_i32_value_at(encode_params, LEVEL_BUDGET);
  int32_t dec_budget    = Get_i32_value_at(decode_params, LEVEL_BUDGET);
  bool    is_lt_bts     = (enc_budget == 1) && (dec_budget == 1);

  VL_PLAIN*    conj_hat_pre     = Get_u0hatt_pre(precom);
  VL_VL_PLAIN* conj_hat_pre_fft = Get_u0hatt_pre_fft(precom);
  VL_PLAIN*    conj_pre         = Get_u0_pre(precom);
  VL_VL_PLAIN* conj_pre_fft     = Get_u0_pre_fft(precom);

  if (slots == m / 4) {
    // fully packed
    // step 1: running coeffs_to_slot
    IS_TRUE(Get_ciph_sf_degree(new_ciph) == 1,
            "scale factor degree of new_ciph is 1");
    CIPHERTEXT* enc_ciph = Alloc_ciphertext();
    RTLIB_TM_START(RTM_BS_COEFF_TO_SLOT, rtm_c2s);
    if (is_lt_bts) {
      Linear_transform(enc_ciph, new_ciph, conj_hat_pre, bts_ctx);
    } else {
      Coeffs_to_slots(enc_ciph, new_ciph, conj_hat_pre_fft, bts_ctx);
    }
    RTLIB_TM_END(RTM_BS_COEFF_TO_SLOT, rtm_c2s);
    IS_TRACE_CMD(
        Print_cipher_poly(Get_trace_file(), "coeffs2_slots", enc_ciph));

    // get conj key
    CIPHERTEXT* conj_ciph    = Alloc_ciphertext();
    CIPHERTEXT* enc_ciph_sub = Alloc_ciphertext();
    Conjugate(conj_ciph, enc_ciph, conj_key, eval);
    Sub_ciphertext(enc_ciph_sub, enc_ciph, conj_ciph, eval);
    Add_ciphertext(enc_ciph, enc_ciph, conj_ciph, eval);
    Mul_by_monomial(enc_ciph_sub, enc_ciph_sub, 3 * m / 4, eval);

    if (AUTO_SCALE) {
      if (Get_ciph_sf_degree(enc_ciph) == 2) {
        Rescale_ciphertext(enc_ciph, enc_ciph, eval);
        Rescale_ciphertext(enc_ciph_sub, enc_ciph_sub, eval);
      }
    } else {
      while (Get_ciph_sf_degree(enc_ciph) > 1) {
        Rescale_ciphertext(enc_ciph, enc_ciph, eval);
        Rescale_ciphertext(enc_ciph_sub, enc_ciph_sub, eval);
      }
    }

    // step 2: approximate mod reduction
    RTLIB_TM_START(RTM_BS_APPROX_MOD, rtm_mod);
    {
      Eval_approx_mod(bts_ctx, enc_ciph, enc_ciph, coefficient,
                      coeff_lower_bound, coeff_upper_bound);
      Eval_approx_mod(bts_ctx, enc_ciph_sub, enc_ciph_sub, coefficient,
                      coeff_lower_bound, coeff_upper_bound);
    }
    RTLIB_TM_END(RTM_BS_APPROX_MOD, rtm_mod);

    Mul_by_monomial(enc_ciph_sub, enc_ciph_sub, m / 4, eval);
    Add_ciphertext(enc_ciph, enc_ciph, enc_ciph_sub, eval);

    // step 3: running Slots_to_coeffs
    RTLIB_TM_START(RTM_BS_SLOT_TO_COEFF, rtm_s2c);
    if (AUTO_SCALE) {
      Rescale_ciphertext(enc_ciph, enc_ciph, eval);
    }

    // Only one linear transform is needed
    if (is_lt_bts) {
      Linear_transform(res, enc_ciph, conj_pre, bts_ctx);
    } else {
      Slots_to_coeffs(res, enc_ciph, conj_pre_fft, bts_ctx);
    }
    RTLIB_TM_END(RTM_BS_SLOT_TO_COEFF, rtm_s2c);
    IS_TRACE_CMD(Print_cipher_poly(Get_trace_file(), "slots2coeffs", enc_ciph));

    Free_ciphertext(enc_ciph);
    Free_ciphertext(enc_ciph_sub);
    Free_ciphertext(conj_ciph);
  } else {
    // sparsely packed case
    // step1: Running PartialSum
    CIPHERTEXT* temp = Alloc_ciphertext();
    RTLIB_TM_START(RTM_BS_PARTIAL_SUM, rtm_ps);
    for (uint32_t j = 1; j < ring_degree / (2 * slots); j <<= 1) {
      SWITCH_KEY* rot_key = Get_rotation_key(bts_ctx, j * slots);
      Eval_fast_rotate(temp, new_ciph, j * slots, rot_key, eval);
      Add_ciphertext(new_ciph, new_ciph, temp, eval);
    }
    RTLIB_TM_END(RTM_BS_PARTIAL_SUM, rtm_ps);
    Free_ciphertext(temp);

    // step2: running coeffs_to_slot
    IS_TRUE(Get_ciph_sf_degree(new_ciph) == 1,
            "scale factor degree of new_ciph is 1");

    CIPHERTEXT* enc_ciph = Alloc_ciphertext();
    RTLIB_TM_START(RTM_BS_COEFF_TO_SLOT, rtm_c2s);
    if (is_lt_bts) {
      Linear_transform(enc_ciph, new_ciph, conj_hat_pre, bts_ctx);
    } else {
      Coeffs_to_slots(enc_ciph, new_ciph, conj_hat_pre_fft, bts_ctx);
    }
    RTLIB_TM_END(RTM_BS_COEFF_TO_SLOT, rtm_c2s);

    // double real part, clear imag part
    CIPHERTEXT* conj_ciph = Alloc_ciphertext();
    Conjugate(conj_ciph, enc_ciph, conj_key, eval);
    Add_ciphertext(enc_ciph, enc_ciph, conj_ciph, eval);
    Free_ciphertext(conj_ciph);

    if (AUTO_SCALE) {
      if (Get_ciph_sf_degree(enc_ciph) == 2) {
        Rescale_ciphertext(enc_ciph, enc_ciph, eval);
      }
    } else {
      while (Get_ciph_sf_degree(enc_ciph) > 1) {
        Rescale_ciphertext(enc_ciph, enc_ciph, eval);
      }
    }

    // step 3: approximate mod reduction
    RTLIB_TM_START(RTM_BS_APPROX_MOD, rtm_mod);
    Eval_approx_mod(bts_ctx, enc_ciph, enc_ciph, coefficient, coeff_lower_bound,
                    coeff_upper_bound);
    RTLIB_TM_END(RTM_BS_APPROX_MOD, rtm_mod);

    // step 3: running Slots_to_coeffs
    RTLIB_TM_START(RTM_BS_SLOT_TO_COEFF, rtm_s2c);
    if (AUTO_SCALE) {
      Rescale_ciphertext(enc_ciph, enc_ciph, eval);
    }
    // linear transform for decoding
    if (is_lt_bts) {
      Linear_transform(res, enc_ciph, conj_pre, bts_ctx);
    } else {
      Slots_to_coeffs(res, enc_ciph, conj_pre_fft, bts_ctx);
    }
    RTLIB_TM_END(RTM_BS_SLOT_TO_COEFF, rtm_s2c);

    SWITCH_KEY* rot_key  = Get_rotation_key(bts_ctx, slots);
    CIPHERTEXT* rot_ciph = Alloc_ciphertext();
    Eval_fast_rotate(rot_ciph, res, slots, rot_key, eval);
    Add_ciphertext(res, res, rot_ciph, eval);
    Free_ciphertext(enc_ciph);
    Free_ciphertext(rot_ciph);
  }

  if (Get_rtlib_config(CONF_BTS_CLEAR_IMAG) && deg >= 1) {
    // use conjugate + add to clear the image part after slot2coeff.
    // As real part will be doubled, 0.5 should be multiplied to the result,
    // we can incorporate the value with deg. Mul_integer is no-need to perform
    // if deg = 1 (for ex: q0=60, sf=59)
    CIPHERTEXT* conj_ciph = Alloc_ciphertext();
    Conjugate(conj_ciph, res, conj_key, eval);
    Add_ciphertext(res, res, conj_ciph, eval);
    Free_ciphertext(conj_ciph);

    uint64_t q0_sf_ratio = pow(2., deg - 1);
    if (q0_sf_ratio > 1) {
      // mul q0/sf to transform (msg / (q0/sf)) to msg
      Mul_integer(res, res, q0_sf_ratio, eval);
    }
  } else {
    // mul q0/sf to transform (msg / (q0/sf)) to msg
    uint64_t q0_sf_ratio = pow(2., deg);
    Mul_integer(res, res, q0_sf_ratio, eval);
  }

  Free_ciphertext(new_ciph);
  Free_value_list(coefficient);

  // TODO: This is actually not necessary for bootstrap,
  // but for now, we should make sure sf_degree of res is 1,
  // since rescale cannot be inserted correctly at callsite.
  while (Get_ciph_sf_degree(res) > 1) {
    Rescale_ciphertext(res, res, eval);
  }

  size_t remaind_q   = Get_ciph_prime_cnt(res);
  size_t init_size_q = Get_ciph_prime_cnt(ciph);

  // If we start with more towers, than we obtain from bootstrapping, return the
  // original ciphertext. need improve later
  if (remaind_q <= init_size_q) {
    DEV_WARN(
        "WARNING: q_cnt(after):%ld <= q_cnt(before):%ld, bootstrapping earns "
        "too small, just return.\n",
        remaind_q, init_size_q);
    Copy_ciphertext(res, ciph);
  }

  IS_TRACE_CMD(
      Print_cipher_msg(Get_trace_file(), "after bts", res, DEF_MSG_LEN));
  RTLIB_TM_END(RTM_BS_EVAL, rtm);
  return res;
}
