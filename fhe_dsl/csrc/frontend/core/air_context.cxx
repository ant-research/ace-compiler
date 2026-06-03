//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "ace/frontend/core/air_context.h"

// NN core headers
#include "nn/core/opcode.h"
#include "nn/core/pragma.h"
#include "nn/vector/vector_opcode.h"

// FHE domain headers
#include "fhe/sihe/sihe_opcode.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/poly/opcode.h"
#include "fhe/sihe/sihe_gen.h"
#include "fhe/ckks/ckks_gen.h"
#include "fhe/core/lower_ctx.h"

namespace ace {
namespace frontend {

AIR_CONTEXT::AIR_CONTEXT() {
    // Create global scope
    _glob = air::base::GLOB_SCOPE::Get();
    _spos = _glob->Unknown_simple_spos();

    // Create LOWER_CTX
    _lower_ctx = std::make_unique<fhe::core::LOWER_CTX>();

    // Register domains and FHE types
    Register_domains();
    Register_fhe_types();
}

AIR_CONTEXT::~AIR_CONTEXT() {
    // Note: Do not delete _glob here to avoid static destruction order issues.
    // The GLOB_SCOPE's arena may already be corrupted during static cleanup.
    // Memory will be reclaimed by OS on program exit.
    _glob = nullptr;
}

void AIR_CONTEXT::Initialize() {
    // Clean up previous GLOB_SCOPE
    if (_glob) {
        delete _glob;
        _glob = nullptr;
    }

    // Create new GLOB_SCOPE
    _glob = new air::base::GLOB_SCOPE(0, true);
    _spos = _glob->Unknown_simple_spos();

    // Re-register domains and FHE types
    Register_domains();
    Register_fhe_types();
}

void AIR_CONTEXT::Register_domains() {
    // Register core AIR domain
    air::core::Register_core();

    // Register NN domains
    nn::core::Register_nn();
    nn::vector::Register_vector_domain();

    // Register FHE domains
    fhe::sihe::Register_sihe_domain();
    fhe::ckks::Register_ckks_domain();
    fhe::poly::Register_polynomial();
}

void AIR_CONTEXT::Register_fhe_types() {
    // Register SIHE types
    fhe::sihe::SIHE_GEN(_glob, _lower_ctx.get()).Register_sihe_types();

    // Register CKKS types
    fhe::ckks::CKKS_GEN(_glob, _lower_ctx.get()).Register_ckks_types();
}

}  // namespace frontend
}  // namespace ace