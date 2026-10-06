//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

/**
 * @file skip_lowering.cxx
 * @brief Implementation of Skip Lowering Registry singleton for SIHE domain
 * 
 * This file contains the ACTUAL singleton instance that is shared across
 * all code that links against libFHEsihe. The header (skip_lowering.h)
 * declares extern functions that call into this singleton.
 */

#include "fhe/sihe/skip_lowering.h"

namespace fhe {
namespace sihe {

// The ONE AND ONLY singleton instance - defined here, shared by all users
SKIP_LOWERING_REGISTRY& SKIP_LOWERING_REGISTRY::Instance() {
    static SKIP_LOWERING_REGISTRY instance;
    return instance;
}

// Convenience function implementations that are exported from libFHEsihe
bool Should_skip_lowering(const std::string& domain, const std::string& op_name) {
    return SKIP_LOWERING_REGISTRY::Instance().Should_skip(domain, op_name);
}

bool Should_skip_lowering(const std::string& full_op_name) {
    return SKIP_LOWERING_REGISTRY::Instance().Should_skip(full_op_name);
}

void Set_skip_lowering_ops(const std::vector<std::string>& ops) {
    SKIP_LOWERING_REGISTRY::Instance().Set_skip_ops(ops);
}

void Add_skip_lowering_op(const std::string& op) {
    SKIP_LOWERING_REGISTRY::Instance().Add_skip_op(op);
}

void Clear_skip_lowering_ops() {
    SKIP_LOWERING_REGISTRY::Instance().Clear_skip_ops();
}

}  // namespace sihe
}  // namespace fhe

