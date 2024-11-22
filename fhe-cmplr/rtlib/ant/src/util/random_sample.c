//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "util/random_sample.h"

#include <assert.h>
#include <memory.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#include "util/fhe_utils.h"
#include "util/prng.h"

void Srand_time() {
  struct timeval t1;
  gettimeofday(&t1, NULL);
  srand(t1.tv_usec * t1.tv_sec);
}

int64_t Random_range(int64_t min_val, int64_t max_val) {
  return min_val + (rand() % (max_val - min_val));
}

/**
 * @brief Generate random double values between [0, 1)
 *
 * @return double
 */
double Random_double() {
  return (double)rand() / (double)((unsigned)RAND_MAX + 1);
}

void Sample_uniform(VALUE_LIST* sample_list, uint64_t upper_bound) {
  int64_t* samples     = Get_i64_values(sample_list);
  size_t   num_samples = LIST_LEN(sample_list);
#if Is_Triage_On
  for (size_t i = 0; i < num_samples - 1; i++) {
    samples[i] = 1;
  }
#elif Use_Rand
  Srand_time();
  for (size_t i = 0; i < num_samples; i++) {
    samples[i] = Random_range(0, upper_bound);
  }
#else
  // PRNG each call return PRNG_VAL_SIZE_IN_BITS bits value
  // if required upper_bound bits larger than PRNG_VAL_SIZE_IN_BITS, needs to
  // call Get_prng_value several times to fullfill the requirement
  BLAKE2_PRNG* prng      = Get_prng();
  uint32_t per_rand_bits = (uint32_t)log2(upper_bound) / PRNG_VAL_SIZE_IN_BITS;
  uint32_t shift_chunk   = per_rand_bits * PRNG_VAL_SIZE_IN_BITS;
  uint32_t last_bound    = (uint32_t)(upper_bound >> shift_chunk);
  for (size_t idx = 0; idx < num_samples; idx++) {
    while (true) {
      uint64_t result = 0;
      for (uint32_t i = 0, shift = 0; i < per_rand_bits;
           ++i, shift += PRNG_VAL_SIZE_IN_BITS) {
        result += ((uint64_t)Uniform_uint_prng(prng, 0, MAX_UINT32)) << shift;
      }
      result += ((uint64_t)Uniform_uint_prng(prng, 0, last_bound))
                << shift_chunk;

      if (result < upper_bound) {
        samples[idx] = result;
        break;
      }
    }
  }
#endif
}

void Sample_triangle(VALUE_LIST* sample_list) {
  int64_t* samples     = Get_i64_values(sample_list);
  size_t   num_samples = LIST_LEN(sample_list);
#if Is_Triage_On
  for (size_t i = 1; i < num_samples - 1; i++) {
    samples[i] = 1;
  }
#else
  Srand_time();
  for (size_t i = 0; i < num_samples; i++) {
    int64_t r = Random_range(0, 4);
    if (r == 0)
      samples[i] = -1;
    else if (r == 1)
      samples[i] = 1;
    else
      samples[i] = 0;
  }
#endif
}

void Sample_ternary(VALUE_LIST* list, int64_t hamming_weight) {
  IS_TRUE(hamming_weight >= 0, "Invalid hamming weight");
  int64_t* samples = Get_i64_values(list);
  size_t   length  = LIST_LEN(list);
#if Is_Triage_On
  for (size_t i = 1; i < length - 1; i++) {
    samples[i] = 1;
  }
#elif Use_Rand
  Srand_time();
  int64_t total_weight = 0;
  memset(samples, 0, length * sizeof(int64_t));
  while (total_weight < hamming_weight) {
    int64_t index = Random_range(0, length);
    if (samples[index] == 0) {
      int64_t r = (int64_t)(rand() & 1);
      if (r == 0) {
        samples[index] = -1;
      } else {
        samples[index] = 1;
      }
      total_weight++;
    }
  }
#else
  BLAKE2_PRNG* prng = Get_prng();
  if (hamming_weight == 0) {
    for (size_t i = 0; i < length; i++) {
      samples[i] = (int64_t)Uniform_int_prng(prng, -1, 1);
    }
  } else {
    if (hamming_weight > length) hamming_weight = length;
    int32_t one_cnt = 0;

    // makes sure the +1's and -1's are roughly evenly distributed
    while ((one_cnt < hamming_weight / 2 - 1) ||
           (one_cnt > hamming_weight / 2 + 1)) {
      one_cnt = 0;
      memset(samples, 0, length * sizeof(int64_t));
      int64_t total_weight = 0;
      while (total_weight < hamming_weight) {
        uint32_t index = Uniform_uint_prng(prng, 0, length - 1);
        if (samples[index] == 0) {
          if (Uniform_int_prng(prng, 0, 1) == 0) {
            samples[index] = -1;
          } else {
            samples[index] = 1;
            one_cnt++;
          }
          total_weight++;
        }
      }
    }
  }
#endif
}

void Sample_random_complex_vector(DCMPLX* sample, size_t length) {
#if defined(Is_Triage_On)
  for (size_t i = 0; i < length; i++) {
    double real = 0.3261028120037244;
    double imag = 0.8195071784630048;
    sample[i]   = real + imag * I;
  }
#else
  Srand_time();
  for (size_t i = 0; i < length; i++) {
    double real = Random_double();
    double imag = Random_double();
    sample[i]   = real + imag * I;
  }
#endif
}

void Sample_random_real_vector(double* sample, size_t length) {
  Srand_time();
  for (size_t i = 0; i < length; i++) {
    sample[i] = Random_double();
  }
}
