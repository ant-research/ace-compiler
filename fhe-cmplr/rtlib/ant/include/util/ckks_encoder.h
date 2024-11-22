//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_INCLUDE_CKKS_ENCODER_H
#define RTLIB_INCLUDE_CKKS_ENCODER_H

#include <stdint.h>

#include "util/ckks_parameters.h"
#include "util/fhe_types.h"
#include "util/fhe_utils.h"
#include "util/macro_chooser.h"
#include "util/plaintext.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  MAX_BITS_IN_WORD = 61,
  MAX_LOG_STEP     = 60,
} LARGE_SF_CST;

//! @brief Stats of weight plaintext
typedef struct {
  size_t _weight_plain_size;
  size_t _weight_plain_cnt;
} WEIGHT_STATS;

/**
 * @brief An encoder for several complex numbers as specified in the CKKS
 * scheme.
 *
 */
typedef struct {
  CKKS_PARAMETER*
               _params;  // Parameters including degree & crt & scaling factor
  FFT_CONTEXT* _fft;     // FFT_CONTEXT object to encode/decode.
  WEIGHT_STATS _stats;   // stats of weight plaintext
} CKKS_ENCODER;

//! @brief Append stats of weigh plaintext
static inline void Append_weight_plain(CKKS_ENCODER* encoder, size_t mem_size) {
  WEIGHT_STATS* stats = &(encoder->_stats);
  stats->_weight_plain_size += mem_size;
  stats->_weight_plain_cnt++;
}

//! @brief Get memsize & count of weigh plaintext
static inline void Get_weight_plain(CKKS_ENCODER* encoder, size_t* size,
                                    size_t* cnt) {
  WEIGHT_STATS* stats = &(encoder->_stats);
  *size               = stats->_weight_plain_size;
  *cnt                = stats->_weight_plain_cnt;
}

/**
 * @brief Inits CKKS_ENCODER with the given parameters.
 *
 * @param params Parameters including polynomial degree & crt
 * @return CKKS_ENCODER*
 */
CKKS_ENCODER* Alloc_ckks_encoder(CKKS_PARAMETER* params);

/**
 * @brief cleanup CKKS_ENCODER
 *
 * @param encoder encoder to be cleanup
 */
void Free_ckks_encoder(CKKS_ENCODER* encoder);

/**
 * @brief Encodes complex numbers into a polynomial.
 * Encodes an array of complex number into a polynomial.
 * Usage Example:
 *   encode(plain, encoder, value)
 *   encode(plain, encoder, value, slots)
 *   Encode_at_level(plain, encoder, values, level)
 *   Encode_at_level(plain, encoder, values, level, slots)
 */
#ifndef MACRO_CHOOSER_4
#define MACRO_CHOOSER_4
#define DEF_ARG4    0
#define API4_NAME   Encode_internal
#define ENCODE(...) MACRO_CHOOSER_ARG4(__VA_ARGS__)(__VA_ARGS__)
#undef MACRO_CHOOSER_4
#endif

#ifndef MACRO_CHOOSER_5
#define MACRO_CHOOSER_5
#define DEF_ARG5             0
#define API5_NAME            Encode_at_level_internal
#define ENCODE_AT_LEVEL(...) MACRO_CHOOSER_ARG5(__VA_ARGS__)(__VA_ARGS__)
#endif

/**
 * @brief Internal APIs for encode
 *
 * @param res plaintext after encode
 * @param encoder ckks encoder including fft context
 * @param values input value list
 * @param slots slots of plaintext
 */
void Encode_internal(PLAINTEXT* res, CKKS_ENCODER* encoder, VALUE_LIST* values,
                     uint32_t slots);
/**
 * @brief encode at given level
 *
 * @param res plaintext after encode
 * @param encoder ckks encoder including fft context
 * @param values input value list
 * @param level level of plaintext
 * @param slots slots of plaintext
 */
void Encode_at_level_internal(PLAINTEXT* res, CKKS_ENCODER* encoder,
                              VALUE_LIST* values, uint32_t level,
                              uint32_t slots);

//! @brief Encode vector at give level & degree of scaling factor
void Encode_at_level_with_sf(PLAINTEXT* res, CKKS_ENCODER* encoder,
                             VALUE_LIST* values, uint32_t level, uint32_t slots,
                             uint32_t sf_degree);

//! @brief Encode vector at give level, slots, scale & p_cnt
void Encode_at_level_with_scale(PLAINTEXT* res, CKKS_ENCODER* encoder,
                                VALUE_LIST* values, uint32_t level,
                                uint32_t slots, double scale, uint32_t p_cnt);

//! @brief Encode single value at given level with given sf_degree
//! @param sf_degree degree of scaling factor, scale = sf_mod_size * sf_degree
void Encode_val_at_level(PLAINTEXT* res, CKKS_ENCODER* encoder, double value,
                         uint32_t level, uint32_t sf_degree);

//! @brief Encode single value at given level with given scale
void Encode_val_at_level_with_scale(PLAINTEXT* res, CKKS_ENCODER* encoder,
                                    double value, uint32_t level, double scale);

/**
 * @brief encode extension at given level & given p_cnt
 *
 * @param res plaintext after encode
 * @param encoder ckks encoder including fft context
 * @param values input value list
 * @param level level of plaintext
 * @param slots slots of plaintext
 * @param p_cnt number of p primes
 */
void Encode_ext_at_level(PLAINTEXT* res, CKKS_ENCODER* encoder,
                         VALUE_LIST* values, uint32_t level, uint32_t slots,
                         uint32_t p_cnt);

/**
 * @brief Decodes a plaintext polynomial back to value list
 *
 * @param res a decoded DCMPLX_TYPE value list
 * @param encoder ckks encoder including fft context
 * @param plain PLAINTEXT to decode
 */
void Decode(VALUE_LIST* res, CKKS_ENCODER* encoder, PLAINTEXT* plain);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_INCLUDE_CKKS_ENCODER_H