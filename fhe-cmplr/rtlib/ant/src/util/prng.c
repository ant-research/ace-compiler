//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "util/prng.h"

#include <sys/time.h>

BLAKE2_PRNG* Prng = NULL;
#pragma omp  threadprivate(Prng)

uint32_t Uniform_uint_rdev(uint32_t min, uint32_t max);

//! @brief Allocate a new BLAKE2_PRNG instance
BLAKE2_PRNG* Alloc_blake2_prng() {
  BLAKE2_PRNG* prng = (BLAKE2_PRNG*)malloc(sizeof(BLAKE2_PRNG));
  prng->_counter    = 0;
  prng->_seed       = Alloc_value_list(UI32_TYPE, SEED_CNT);
  prng->_buffer     = Alloc_value_list(UI32_TYPE, PRNG_BUFFER_SIZE);
  prng->_buffer_idx = 0;
  return prng;
}

//! @brief Initialize an blake2 prng with seed composed by timestamp and thread
//! id Note multi-thread is not supported yet, so seed now is only from
//! timestramp seed comming from
//! 1. time
//! 2. thread_id (if multi-thread enabled)
void Init_blake2_prng(BLAKE2_PRNG* prng) {
  FMT_ASSERT(prng != NULL, "prng not allocated yet");
  prng->_seed = Alloc_value_list(UI32_TYPE, SEED_CNT);
  struct timeval curr_time;
  gettimeofday(&curr_time, NULL);
  uint64_t time_val = curr_time.tv_sec * BILLION + curr_time.tv_usec;
  Set_ui32_value(prng->_seed, 0, (uint32_t)(curr_time.tv_sec >> 32));
  Set_ui32_value(prng->_seed, 1, (uint32_t)curr_time.tv_usec);
#ifdef MULTI_THREAD
  Set_ui32_value(prng->_seed, 2,
                 std::hash<std::thread::id>{}(std::this_thread::get_id()));
#endif

  // heap variable; we are going to use the least 32 bits of its memory
  // location as the counter for BLAKE2 This will increase the entropy of
  // the BLAKE2 sample
  void*    mem     = malloc(1);
  uint32_t counter = (intptr_t)mem;
  free(mem);

  prng->_counter = counter;
}

BLAKE2_PRNG* Get_prng() {
  if (Prng == NULL) {
    Prng             = Alloc_blake2_prng();
    BLAKE2_PRNG* gen = Alloc_blake2_prng();
    Init_blake2_prng(gen);

    VALUE_LIST* seed = Prng->_seed;
    for (uint32_t i = 0; i < SEED_CNT; i++) {
      uint32_t seed_1 = Uniform_uint_prng(gen, 0, MAX_UINT32);
      uint32_t seed_2 = Uniform_uint_rdev(0, MAX_UINT32);
      Set_ui32_value(seed, i, seed_1 + seed_2);
    }
  }
  return Prng;
}

#ifdef UNIX_LIKE_SYSTEM
uint32_t Get_random_device_seed() {
  uint32_t seed;
  int      urandom_fd = open("/dev/urandom", O_RDONLY);
  if (urandom_fd == -1) {
    FMT_ASSERT(false, "Failed to open /dev/urandom");
  }
  ssize_t bytes_read = read(urandom_fd, &seed, sizeof(seed));
  if (bytes_read != sizeof(seed)) {
    FMT_ASSERT(false, "Failed to read from /dev/urandom");
  }
  close(urandom_fd);
  return seed;
}
#else
uint32_t Get_random_device_seed() {
  FMT_ASSERT(false, "only unix-like system supported");
  return 0;
}
#endif

uint32_t Uniform_uint_rdev(uint32_t min, uint32_t max) {
  IS_TRUE(max >= min, "invalid min max");
  uint32_t range = max - min;
  uint32_t ret   = 0;

  // downscaling
  if (range < MAX_UINT32) {
    range            = range + 1;  // range can be zero
    uint32_t scaling = MAX_UINT32 / range;
    uint32_t past    = range * scaling;
    do {
      ret = Get_random_device_seed();
    } while (ret >= past);
    ret /= scaling;
  } else if (range == MAX_UINT32) {
    ret = Get_random_device_seed();
  } else {
    FMT_ASSERT(false, "range can not larger than UINT32");
  }
  return ret + min;
}
