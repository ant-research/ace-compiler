//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_PY_AIRGEN_H
#define FHE_CKKS_PY_AIRGEN_H

#include <pybind11/embed.h>
#include <pybind11/pybind11.h>

#include "fhe/ckks/sihe2ckks_ctx.h"

//! @brief CKKS IR GEN with python dsl

namespace fhe {
namespace ckks {

namespace py = pybind11;
class CKKS_PY_IMPL {
public:
  CKKS_PY_IMPL(SIHE2CKKS_CTX& ctx) : _ctx(ctx) {}

  NODE_PTR New_py_add(NODE_PTR node, NODE_PTR op0, NODE_PTR op1,
                      const SPOS& spos);

  FUNC_SCOPE* New_py_bts(NODE_PTR node, const SPOS& spos);

  FUNC_SCOPE* New_bts_func(NODE_PTR node, const SPOS& spos);

protected:
  SIHE2CKKS_CTX& _ctx;
};

}  // namespace ckks
}  // namespace fhe
#endif