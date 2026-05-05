//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "config_manager.h"
#include <cstring>

namespace ace {
namespace runtime {

const RUNTIME_CONFIG* Get_runtime_config() {
    return &CONFIG_MANAGER::Instance().Get_config();
}

std::string CONFIG_VALIDATOR::Validate(const RUNTIME_CONFIG& config) {
    std::ostringstream errors;

    // Basic checks
    if (config._input_cnt <= 0) {
        errors << "Invalid input_count: " << config._input_cnt << "\n";
    }
    if (config._output_cnt <= 0) {
        errors << "Invalid output_count: " << config._output_cnt << "\n";
    }

    // CKKS parameter checks
    if (auto* ckks = reinterpret_cast<CKKS_PARAMS*>(config._ctx_param)) {
        if (ckks->_poly_degree == 0) {
            errors << "CKKS: poly_degree cannot be zero\n";
        }
        if (ckks->_num_rot_idx > 0 && ckks->_rot_idxs == nullptr) {
            errors << "CKKS: rot_idxs is null but num_rot_idx > 0\n";
        }
    }

    // Encode/Decode scheme checks
    for (size_t i = 0; i < config._encode_sch.size(); ++i) {
        const auto& sch = config._encode_sch[i];
        if (!sch._name || strlen(sch._name) == 0) {
            errors << "Encode scheme[" << i << "]: name is empty\n";
        }
        if (sch._count <= 0) {
            errors << "Encode scheme[" << i << "]: count must be > 0\n";
        }
        if (sch._desc == nullptr && sch._count > 0) {
            errors << "Encode scheme[" << i << "]: desc is null\n";
        }
    }

    // Similar checks for decode_sch
    for (size_t i = 0; i < config._decode_sch.size(); ++i) {
        const auto& sch = config._decode_sch[i];
        if (!sch._name || strlen(sch._name) == 0) {
            errors << "Decode scheme[" << i << "]: name is empty\n";
        }
        if (sch._count <= 0) {
            errors << "Decode scheme[" << i << "]: count must be > 0\n";
        }
        if (sch._desc == nullptr && sch._count > 0) {
            errors << "Decode scheme[" << i << "]: desc is null\n";
        }
    }

    return errors.str();
}

void CONFIG_PRINTER::Print(const RUNTIME_CONFIG& config, std::ostream& os) {
    os << "=== Runtime Configuration ===\n";
    os << "Input Count: " << config._input_cnt << "\n";
    os << "Output Count: " << config._output_cnt << "\n\n";

    // CKKS Params
    if (auto* ckks = reinterpret_cast<CKKS_PARAMS*>(config._ctx_param)) {
        os << "CKKS Parameters:\n";
        os << "  Poly Degree: " << ckks->_poly_degree << "\n";
        os << "  Mul Depth: " << ckks->_mul_depth << "\n";
        os << "  Rot Idxs: [";
        for (size_t i = 0; i < ckks->_num_rot_idx; ++i) {
            if (i > 0) os << ", ";
            os << ckks->_rot_idxs[i];
        }
        os << "]\n\n";
    }

    // Encode Schemes
    os << "Encode Schemes (" << config._encode_sch.size() << "):\n";
    for (size_t i = 0; i < config._encode_sch.size(); ++i) {
        const auto& sch = config._encode_sch[i];
        os << "  [" << i << "] " << (sch._name ? sch._name : "unnamed")
            << " - Shape: [" << sch._shape._n << "," << sch._shape._c
            << "," << sch._shape._h << "," << sch._shape._w << "]\n";
        os << "      Count: " << sch._count << "\n";
    }

    // Decode Schemes
    os << "\nDecode Schemes (" << config._decode_sch.size() << "):\n";
    for (size_t i = 0; i < config._decode_sch.size(); ++i) {
        const auto& sch = config._decode_sch[i];
        os << "  [" << i << "] " << (sch._name ? sch._name : "unnamed")
            << " - Shape: [" << sch._shape._n << "," << sch._shape._c
            << "," << sch._shape._h << "," << sch._shape._w << "]\n";
        os << "      Count: " << sch._count << "\n";
    }
}

std::string CONFIG_PRINTER::To_string(const RUNTIME_CONFIG& config) {
    std::ostringstream oss;
    Print(config, oss);
    return oss.str();
}

} // namespace runtime
} // namespace ace