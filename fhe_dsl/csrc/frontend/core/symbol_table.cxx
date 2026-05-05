//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "frontend/core/symbol_table.h"

#include <iostream>

namespace ace {
namespace frontend {

air::base::NODE_PTR SYMBOL_TABLE::Resolve_input(const std::string& name,
                                                air::base::CONTAINER* cntr,
                                                const air::base::SPOS& spos) const {
    // First check if it's a constant
    air::base::CONSTANT_PTR cst = Get_const(name);
    if (cst != air::base::Null_ptr) {
        return cntr->New_ldc(cst, spos);
    }

    // Then check if it's a preister (intermediate result)
    air::base::PREG_PTR preg = Get_preg(name);
    if (preg != air::base::Null_ptr) {
        return cntr->New_ldp(preg, spos);
    }

    // Finally check if it's a variable (input parameter)
    air::base::ADDR_DATUM_PTR var = Get_var(name);
    if (var != air::base::Null_ptr) {
        return cntr->New_ld(var, spos);
    }

    // Not found - return null
    std::cerr << "[SYMBOL_TABLE] Resolve_input: name not found: " << name << std::endl;
    return air::base::NODE_PTR();
}

}  // namespace frontend
}  // namespace ace