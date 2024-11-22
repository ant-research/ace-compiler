
//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================
#ifndef RTLIB_INCLUDE_UTIL_PRNG_H
#define RTLIB_INCLUDE_UTIL_PRNG_H
#include <fcntl.h>
#include <unistd.h>

#include "blake2.h"
#include "fhe_types.h"

#define MAX_UINT32            0xffffffffL   // max value for uint32_t
#define BILLION               1000000000LL  // 1 sec = BILLION usec
#define SEED_CNT              16
#define PRNG_BUFFER_SIZE      1024
#define UNIX_LIKE_SYSTEM      1
#define PRNG_VAL_SIZE_IN_BITS 32

#ifdef __cplusplus
extern "C" {
#endif

//! @brief BLAKE2_PRNG stores blake2 seeds and random bufffers generated
typedef struct blake2_prng {
  uint64_t _counter;        // counter used as input to the BLAKE2 hash function
                            // gets incremented after each call
  VALUE_LIST* _seed;        // the seed for BLAKE2 hash function
  VALUE_LIST* _buffer;      // buffers that stores random samples generated
  uint16_t    _buffer_idx;  // index in _buffer corresponding to the
                            // current PRNG sample
} BLAKE2_PRNG;

// ！@brief Get BLAKE2_PRNG instance
BLAKE2_PRNG* Get_prng();

//! @brief Call blake2b to generate new hashes
static inline void Gen_prng_values(BLAKE2_PRNG* prng) {
  FMT_ASSERT(prng != NULL, "Prng not allocated yet");
  if (blake2xb(Get_ui32_values(prng->_buffer),       // out ptr
               PRNG_BUFFER_SIZE * sizeof(uint32_t),  // out len
               &prng->_counter,                      // in ptr
               sizeof(prng->_counter),               // in len
               Get_ui32_values(prng->_seed),         // key ptr
               SEED_CNT * sizeof(uint32_t)) != 0) {  // keylen
    FMT_ASSERT(false, "blake2xb failed");
  }
  prng->_counter++;
}

//! @brief Return the PRNG value, if buffer all used, call Generate to re-gen
//! buffer
static inline uint32_t Get_prng_value(BLAKE2_PRNG* prng) {
  FMT_ASSERT(prng != NULL, "Prng not allocated yet");
  if (prng->_buffer_idx == PRNG_BUFFER_SIZE) prng->_buffer_idx = 0;

  if (prng->_buffer_idx == 0) {
    Gen_prng_values(prng);
  }
  return Get_ui32_value_at(prng->_buffer, prng->_buffer_idx++);
}

//! @brief Return an uniform sampled Blake2 value

static inline uint32_t Uniform_uint_prng(BLAKE2_PRNG* prng, uint32_t min,
                                         uint32_t max) {
  uint32_t range = max - min;
  uint32_t ret   = 0;

  // downscaling
  if (range < MAX_UINT32) {
    range            = range + 1;  // range can be zero
    uint32_t scaling = MAX_UINT32 / range;
    uint32_t past    = range * scaling;
    do {
      ret = Get_prng_value(prng);
    } while (ret >= past);
    ret /= scaling;
  } else if (range == MAX_UINT32) {
    // range = MAX_UINT32
    ret = Get_prng_value(prng);
  } else {
    FMT_ASSERT(false, "range can not larger than UINT32");
  }
  return ret + min;
}

static inline int32_t Uniform_int_prng(BLAKE2_PRNG* prng, int32_t min,
                                       int32_t max) {
  return (int32_t)Uniform_uint_prng(prng, (uint32_t)min, (uint32_t)max);
}

#ifdef __cplusplus
}
#endif

#endif