//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef ACE_RUNTIME_VALIDATOR_H
#define ACE_RUNTIME_VALIDATOR_H

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

#include "common/rtlib.h"

namespace ace {
namespace runtime {

/**
 * @brief Utility class for result validation.
 *
 * Provides static methods for comparing computation results against
 * expected values using absolute and relative error metrics.
 * Error thresholds can be configured via environment variables.
 */
class VALIDATOR {
    public:

    /**
     * @brief Compose a hex string from double buffer.
     * @param tag Label prefix for the string
     * @param buf Pointer to double buffer
     * @param sz Buffer size
     * @return Formatted hex string
     */
    static std::string Compose_str(const std::string& tag, double* buf, size_t sz);

    /**
     * @brief Compose a hex string from int64_t buffer (head and tail only).
     * @param tag Label prefix for the string
     * @param buf Pointer to int64_t buffer
     * @param sz Buffer size
     * @return Formatted hex string showing first 4 and last 4 elements
     */
    static std::string Compose_str(const std::string& tag, int64_t* buf, size_t sz);

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
    static bool Validate_absolute_error(double* result, double* expect, int len);

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
    static bool Validate_relative_error(double* result, double* expect, int len);
};

} // namespace runtime
} // namespace ace

#endif // ACE_RUNTIME_VALIDATOR_H