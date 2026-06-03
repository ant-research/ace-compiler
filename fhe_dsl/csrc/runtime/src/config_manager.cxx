//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "ace/runtime/config_manager.h"
#include <cstdlib>
#include <cstring>

namespace ace {
namespace runtime {

static void Free_data_scheme(DATA_SCHEME& sch) {
    if (sch._name) {
        std::free(const_cast<char*>(sch._name));
        sch._name = nullptr;
    }
    if (sch._desc) {
        delete[] sch._desc;
        sch._desc = nullptr;
    }
}

void RUNTIME_CONFIG::Cleanup() {
    // Free CKKS_PARAMS allocated with malloc
    if (_ctx_param != 0) {
        std::free(reinterpret_cast<void*>(_ctx_param));
        _ctx_param = 0;
    }

    // Free DATA_SCHEME heap allocations
    for (auto& sch : _encode_sch) {
        Free_data_scheme(sch);
    }
    _encode_sch.clear();

    for (auto& sch : _decode_sch) {
        Free_data_scheme(sch);
    }
    _decode_sch.clear();

    // Clear owned string storage (RT_DATA_INFO pointers become dangling)
    _weight_file_name.clear();
    _weight_file_uuid.clear();
    _weight_info = {};
    _input_cnt = 0;
    _output_cnt = 0;
}

std::string RUNTIME_CONFIG::Validate() const {
    std::ostringstream errors;

    // Basic checks
    if (_input_cnt <= 0) {
        errors << "Invalid input_count: " << _input_cnt << "\n";
    }
    if (_output_cnt <= 0) {
        errors << "Invalid output_count: " << _output_cnt << "\n";
    }

    // CKKS parameter checks
    if (auto* ckks = reinterpret_cast<CKKS_PARAMS*>(_ctx_param)) {
        if (ckks->_poly_degree == 0) {
            errors << "CKKS: poly_degree cannot be zero\n";
        }
        if (ckks->_num_rot_idx > 0 && ckks->_rot_idxs == nullptr) {
            errors << "CKKS: rot_idxs is null but num_rot_idx > 0\n";
        }
    }

    // Encode/Decode scheme checks
    for (size_t i = 0; i < _encode_sch.size(); ++i) {
        const auto& sch = _encode_sch[i];
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
    for (size_t i = 0; i < _decode_sch.size(); ++i) {
        const auto& sch = _decode_sch[i];
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

std::string RUNTIME_CONFIG::To_string() const {
    std::ostringstream os;
    os << "=== Runtime Configuration ===\n";
    os << "Input Count: " << _input_cnt << "\n";
    os << "Output Count: " << _output_cnt << "\n\n";

    // CKKS Params
    if (auto* ckks = reinterpret_cast<CKKS_PARAMS*>(_ctx_param)) {
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
    os << "Encode Schemes (" << _encode_sch.size() << "):\n";
    for (size_t i = 0; i < _encode_sch.size(); ++i) {
        const auto& sch = _encode_sch[i];
        os << "  [" << i << "] " << (sch._name ? sch._name : "unnamed")
            << " - Shape: [" << sch._shape._n << "," << sch._shape._c
            << "," << sch._shape._h << "," << sch._shape._w << "]\n";
        os << "      Count: " << sch._count << "\n";
    }

    // Decode Schemes
    os << "\nDecode Schemes (" << _decode_sch.size() << "):\n";
    for (size_t i = 0; i < _decode_sch.size(); ++i) {
        const auto& sch = _decode_sch[i];
        os << "  [" << i << "] " << (sch._name ? sch._name : "unnamed")
            << " - Shape: [" << sch._shape._n << "," << sch._shape._c
            << "," << sch._shape._h << "," << sch._shape._w << "]\n";
        os << "      Count: " << sch._count << "\n";
    }

    return os.str();
}

} // namespace runtime
} // namespace ace