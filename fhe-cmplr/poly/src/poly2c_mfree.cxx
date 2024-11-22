//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/poly/poly2c_mfree.h"

#include "air/base/visitor.h"

namespace fhe {

namespace poly {

void MFREE_PASS::Perform(air::base::NODE_PTR body) {
  // forward pass to mark variables not to be freed
  MFREE_FWD_CTX                                fwd_ctx(*this);
  air::base::VISITOR<fhe::poly::MFREE_FWD_CTX> fwd_trav(fwd_ctx);
  fwd_trav.template Visit<void>(body);

  // backward pass to insert mfree
  MFREE_BWD_CTX                                bwd_ctx(*this);
  air::base::VISITOR<fhe::poly::MFREE_BWD_CTX> bwd_trav(bwd_ctx);
  bwd_trav.template Visit<void>(body);
}

}  // namespace poly

}  // namespace fhe
