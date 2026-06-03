//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "ace/runtime/validator.h"
#include <spdlog/spdlog.h>
#include <sstream>
#include <cmath>

namespace ace {
namespace runtime {

std::string Compose_hex_str(const std::string& tag, double* buf, size_t sz) {
    std::ostringstream os;
    os << " " << tag << std::hex;
    for (size_t i = 0; i < sz; i++) {
        os << *(buf + i) << ", ";
    }
    return os.str();
}

std::string Compose_hex_str(const std::string& tag, int64_t* buf, size_t sz) {
    std::ostringstream os;
    os << tag << " " << std::hex;
    for (size_t i = 0; i < 4; i++) {
        os << *(buf + i) << ", ";
    }
    os << " ,,, ";
    for (size_t i = sz - 4; i < sz; i++) {
        os << *(buf + i) << ", ";
    }
    return os.str();
}

bool Validate_absolute_error(double* result, double* expect, int len) {
    bool print_all = false;
    const char* print_all_str = getenv("PRINT_ALL");
    if (print_all_str != NULL) {
        spdlog::debug("PRINT_ALL: {}", print_all_str);
        print_all = true;
    }

    const char* absolute_error_str = getenv("ABS_ERROR");
    double absolute_error = 0.0001;
    if (absolute_error_str != NULL) {
        spdlog::debug("ABS_ERROR: {}", absolute_error_str);
        absolute_error = atof(absolute_error_str);
    }
    spdlog::debug("expect absolute error less than: {}", absolute_error);
    int count = 0;
    for (int i = 0; i < len; i++) {
        double result_absolute_error = fabs(result[i] - expect[i]);
        if (print_all) {
            if (result_absolute_error > absolute_error) {
                count++;
                spdlog::debug("index: {}, result: {}, expect: {}, abs_error={}, {} failed", i, result[i], expect[i], result_absolute_error, count);
            } else {
                spdlog::debug("index: {}, result: {}, expect: {}, abs_error={}, ok", i, result[i], expect[i], result_absolute_error);
            }
        } else {
            if (result_absolute_error > absolute_error) {
                spdlog::debug("index: {}, value: {} != {}, abs_error={}", i, result[i], expect[i], result_absolute_error);
                return false;
            }
        }
    }
    if (print_all && (count != 0)) {
        return false;
    }
    return true;
}

bool Validate_relative_error(double* result, double* expect, int len) {
    bool print_all = false;
    const char* print_all_str = getenv("PRINT_ALL");
    if (print_all_str != NULL) {
        spdlog::debug("PRINT_ALL: {}", print_all_str);
        print_all = true;
    }

    const char* relative_error_str = getenv("REL_ERROR");
    double relative_error = 0.001;
    if (relative_error_str != NULL) {
        spdlog::debug("REL_ERROR: {}", relative_error_str);
        relative_error = atof(relative_error_str);
    }
    spdlog::debug("expect relative error less than: {}", relative_error);
    int count = 0;
    for (int i = 0; i < len; i++) {
        double result_relative_error = fabs(result[i] - expect[i]) / expect[i];
        if (print_all) {
            if (result_relative_error > relative_error) {
                count++;
                spdlog::debug("index: {}, result: {}, expect: {}, rel_error={}, {} failed", i, result[i], expect[i], result_relative_error, count);
            } else {
                spdlog::debug("index: {}, result: {}, expect: {}, rel_error={}, ok", i, result[i], expect[i], result_relative_error);
            }
        } else {
            if (result_relative_error > relative_error) {
                spdlog::debug("index: {}, value: {} != {}, rel_error={}", i, result[i], expect[i], result_relative_error);
                return false;
            }
        }
    }
    if (print_all && (count != 0)) {
        return false;
    }
    return true;
}

bool Validate_result(const torch::Tensor& result, const torch::Tensor& expected) {
    auto result_contiguous = result.to(torch::kFloat64).contiguous();
    auto expected_contiguous = expected.to(torch::kFloat64).contiguous();

    double* result_data = result_contiguous.data_ptr<double>();
    double* expect_data = expected_contiguous.data_ptr<double>();
    int sz = result_contiguous.numel();

    spdlog::debug(Compose_hex_str("Result: ", result_data, sz));
    spdlog::debug(Compose_hex_str("Expect: ", expect_data, sz));

    bool res_relative = Validate_relative_error(result_data, expect_data, sz);
    bool res_absolute = Validate_absolute_error(result_data, expect_data, sz);

    if (res_relative || res_absolute) {
        spdlog::info("Inference SUCCESS!");
        return true;
    } else {
        spdlog::info("Inference FAILED!");
        return false;
    }
}

} // namespace runtime
} // namespace ace