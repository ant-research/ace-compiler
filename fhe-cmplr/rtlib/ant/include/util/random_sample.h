//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_INCLUDE_RANDOM_SAMPLE_H
#define RTLIB_INCLUDE_RANDOM_SAMPLE_H

#include "util/fhe_types.h"

// A module to sample randomly from various distributions.

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief samples from a uniform distribution
 * samples num_samples integer values from the range [min, max)
 * uniformly at random.
 *
 * @param samples a list of randomly sampled values
 * @param upper_bound upper bound value (exclusive)
 */
void Sample_uniform(VALUE_LIST* samples, uint64_t upper_bound);

/**
 * @brief samples from a discrete triangle distribution
 * samples num_samples values from [-1, 0, 1] with probabilities
 * [0.25, 0.5, 0.25], respectively.
 *
 * @param samples a list of randomly sampled values
 */
void Sample_triangle(VALUE_LIST* samples);

/**
 * @brief samples from a Hamming weight distribution
 * samples uniformly from the set [-1, 0, 1] such that the
 * resulting vector has exactly h nonzero values.
 *
 * @param samples a list of randomly sampled values
 * @param hamming_weight hamming weight h of resulting vector
 */
void Sample_ternary(VALUE_LIST* samples, int64_t hamming_weight);

/**
 * @brief samples a random complex vector
 * samples a vector with elements of the form a + bi where a and b
 * are chosen uniformly at random from the set [0, 1).
 *
 * @param sample a list of randomly sampled complex values
 * @param length length of vector
 */
void Sample_random_complex_vector(DCMPLX* sample, size_t length);

/**
 * @brief Samples a random real vector
 * samples a vector with elements chosen uniformly at random from
 * the set [0, 1).
 *
 * @param sample a list of randomly sampled real values
 * @param length length of vector
 */
void Sample_random_real_vector(double* sample, size_t length);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_INCLUDE_RANDOM_SAMPLE_H