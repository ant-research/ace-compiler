//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_CONTEXT_H
#define AIR_CONTEXT_H

#include <memory>

#include "air/base/st.h"

// Forward declarations for FHE
namespace fhe {
namespace core {
class LOWER_CTX;
}
}

namespace ace {
namespace frontend {

//! @brief AIR_CONTEXT - Manages global AIR infrastructure
//!
//! Responsibilities:
//! - GLOB_SCOPE creation and lifecycle
//! - SPOS (source position) management
//! - Domain registration (core, nn, vector, fhe)
//! - LOWER_CTX for FHE type registration
//!
//! This is the foundational context that other components
//! (FUNC_BUILDER, TYPE_FACTORY, STMT_GENERATOR) depend on.
class AIR_CONTEXT {
public:
    AIR_CONTEXT();
    ~AIR_CONTEXT();

    // ========================================================================
    // Initialization
    // ========================================================================

    //! @brief Initialize or re-initialize the global scope
    //! Creates new GLOB_SCOPE and re-registers all domains
    void Initialize();

    // ========================================================================
    // Accessors
    // ========================================================================

    //! @brief Get global scope
    air::base::GLOB_SCOPE* Get_glob() { return _glob; }

    //! @brief Get source position
    const air::base::SPOS& Get_spos() const { return _spos; }

    //! @brief Get LOWER_CTX for FHE operations
    fhe::core::LOWER_CTX* Get_lower_ctx() { return _lower_ctx.get(); }

private:
    // Non-copyable
    AIR_CONTEXT(const AIR_CONTEXT&) = delete;
    AIR_CONTEXT& operator=(const AIR_CONTEXT&) = delete;

    // Member variables
    air::base::GLOB_SCOPE*                  _glob = nullptr;
    air::base::SPOS                         _spos;
    std::unique_ptr<fhe::core::LOWER_CTX>   _lower_ctx;

    // Member functions
    void Register_domains();
    void Register_fhe_types();
};

}  // namespace frontend
}  // namespace ace

#endif  // AIR_CONTEXT_H