//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "validator.h"
#include <sstream>
#include <cmath>

namespace ace {
namespace runtime {

std::string VALIDATOR::Compose_str(const std::string& tag, double* buf, size_t sz) {
    std::ostringstream os;
    os << " " << tag << std::hex;
    for (size_t i = 0; i < sz; i++) {
        os << *(buf + i) << ", ";
    }
    return os.str();
}

std::string VALIDATOR::Compose_str(const std::string& tag, int64_t* buf, size_t sz) {
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

bool VALIDATOR::Validate_absolute_error(double* result, double* expect, int len) {
    bool print_all = false;
    const char* print_all_str = getenv("PRINT_ALL");
    if (print_all_str != NULL) {
        printf("Value of PRINT_ALL: %s\n", print_all_str);
        print_all = true;
    }

    const char* absolute_error_str = getenv("ABS_ERROR");
    double absolute_error = 0.0001;
    if (absolute_error_str != NULL) {
        printf("Value of ABS_ERROR: %s\n", absolute_error_str);
        absolute_error = atof(absolute_error_str);
    }
    printf("expect absolute error less than: %f\n", absolute_error);
    int count = 0;
    for (int i = 0; i < len; i++) {
        double result_absolute_error = fabs(result[i] - expect[i]);
        if (print_all) {
            printf("index: %d, result: %f, expect: %f, result absolute error=%f, ", i, result[i], expect[i], result_absolute_error);
            if (result_absolute_error > absolute_error) {
                count++;
                printf("%d failed\n", count);
            } else {
                printf("ok\n");
            }
        } else {
            if (result_absolute_error > absolute_error) {
                printf("index: %d, value: %f != %f, result absolute error=%f\n", i, result[i], expect[i], result_absolute_error);
                return false;
            }
        }
    }
    if (print_all && (count != 0)) {
        return false;
    }
    return true;
}

bool VALIDATOR::Validate_relative_error(double* result, double* expect, int len) {
    bool print_all = false;
    const char* print_all_str = getenv("PRINT_ALL");
    if (print_all_str != NULL) {
        printf("Value of PRINT_ALL: %s\n", print_all_str);
        print_all = true;
    }

    const char* relative_error_str = getenv("REL_ERROR");
    double relative_error = 0.001;
    if (relative_error_str != NULL) {
        printf("Value of REL_ERROR: %s\n", relative_error_str);
        relative_error = atof(relative_error_str);
    }
    printf("expect relative error less than: %f\n", relative_error);
    int count = 0;
    for (int i = 0; i < len; i++) {
        double result_relative_error = fabs(result[i] - expect[i]) / expect[i];
        if (print_all) {
            printf("index: %d, result: %f, expect: %f, result relative error=%f, ", i, result[i], expect[i], result_relative_error);
            if (result_relative_error > relative_error) {
                count++;
                printf("%d failed\n", count);
            } else {
                printf("ok\n");
            }
        } else {
            if (result_relative_error > relative_error) {
                printf("index: %d, value: %f != %f, result relative error: %f\n", i, result[i], expect[i], result_relative_error);
                return false;
            }
        }
    }
    if (print_all && (count != 0)) {
        return false;
    }
    return true;
}

} // namespace runtime
} // namespace ace