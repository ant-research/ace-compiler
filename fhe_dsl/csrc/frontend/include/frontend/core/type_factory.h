//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef TYPE_FACTORY_H
#define TYPE_FACTORY_H

#include <vector>
#include <cstdint>

#include "air/base/st.h"

namespace ace {
namespace frontend {

// Forward declaration
class AIR_CONTEXT;

//! @brief TYPE_FACTORY - Creates AIR types for tensor operations
//!
//! Responsibilities:
//! - Create tensor types from shapes
//! - Create array types with custom element types
//! - Support various primitive types (float32, int64, etc.)
//!
//! Depends on AIR_CONTEXT for GLOB_SCOPE and SPOS.
class TYPE_FACTORY {
public:
    inline explicit TYPE_FACTORY(AIR_CONTEXT* ctx) : _ctx(ctx) {}
    ~TYPE_FACTORY() = default;

    // ========================================================================
    // Type Creation
    // ========================================================================

    //! @brief Create a float32 tensor type from shape
    //! @param shape Tensor dimensions
    //! @return TYPE_PTR to the created array type
    air::base::TYPE_PTR New_tensor_type(const std::vector<int64_t>& shape);

    //! @brief Create an array type with custom element type
    //! @param elem_type Element type (e.g., INT_S64, FLOAT_32)
    //! @param shape Array dimensions
    //! @return TYPE_PTR to the created array type
    air::base::TYPE_PTR New_array_type(air::base::PRIMITIVE_TYPE elem_type,
                                       const std::vector<int64_t>& shape);

private:
    // Non-copyable
    TYPE_FACTORY(const TYPE_FACTORY&) = delete;
    TYPE_FACTORY& operator=(const TYPE_FACTORY&) = delete;

    // Member variables
    AIR_CONTEXT* _ctx;
};

}  // namespace frontend
}  // namespace ace

#endif  // TYPE_FACTORY_H