//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef ACE_RUNTIME_VALIDATOR_H
#define ACE_RUNTIME_VALIDATOR_H

#include <string>
#include <vector>
#include <cstddef>

#include <torch/extension.h>

namespace ace {
namespace runtime {

/**
 * @brief Compose a hex string from double buffer.
 * @param tag Label prefix for the string
 * @param buf Pointer to double buffer
 * @param sz Buffer size
 * @return Formatted hex string
 */
std::string Compose_hex_str(const std::string& tag, double* buf, size_t sz);

/**
 * @brief Compose a hex string from int64_t buffer (head and tail only).
 * @param tag Label prefix for the string
 * @param buf Pointer to int64_t buffer
 * @param sz Buffer size
 * @return Formatted hex string showing first 4 and last 4 elements
 */
std::string Compose_hex_str(const std::string& tag, int64_t* buf, size_t sz);

/**
 * @brief Validate results using absolute error threshold.
 *
 * Checks if all elements in the result buffer are within the absolute
 * error threshold of the expected values. Threshold can be set via
 * ABS_ERROR environment variable (default: 0.0001).
 *
 * @param result Computed result buffer
 * @param expect Expected result buffer
 * @param len Number of elements to compare
 * @return true if all elements within threshold, false otherwise
 */
bool Validate_absolute_error(double* result, double* expect, int len);

/**
 * @brief Validate results using relative error threshold.
 *
 * Checks if all elements in the result buffer are within the relative
 * error threshold of the expected values. Threshold can be set via
 * REL_ERROR environment variable (default: 0.001).
 *
 * @param result Computed result buffer
 * @param expect Expected result buffer
 * @param len Number of elements to compare
 * @return true if all elements within threshold, false otherwise
 */
bool Validate_relative_error(double* result, double* expect, int len);

/**
 * @brief Validate a result tensor against an expected tensor.
 *
 * Compares the result and expected tensors using both absolute and relative
 * error metrics. Logs the comparison at debug level.
 *
 * @param result Result tensor from FHE inference
 * @param expected Expected (plaintext) tensor
 * @return true if validation passes (either error metric within threshold)
 */
bool Validate_result(const torch::Tensor& result, const torch::Tensor& expected);

} // namespace runtime
} // namespace ace

#endif // ACE_RUNTIME_VALIDATOR_H