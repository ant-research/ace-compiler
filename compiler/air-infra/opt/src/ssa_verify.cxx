//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/opt/ssa_verify.h"

#include "air/base/visitor.h"
#include "air/core/handler.h"
#include "ssa_rename_handler.h"
#include "ssa_verify_ctx.h"

namespace air {

namespace opt {

void SSA_VERIFIER::Verify_simple() {
  SIMPLE_VERIFIER_CTX verify_ctx(_cont);
  air::base::VISITOR<SIMPLE_VERIFIER_CTX, air::core::HANDLER<RENAME_HANDLER> >
                      verify_trav(verify_ctx);
  air::base::NODE_PTR body = _cont->Container()->Entry_node();
  verify_ctx.Initialize(body->Id());
  verify_trav.template Visit<void>(body);
  verify_ctx.Finalize(body->Id());
}

}  // namespace opt

}  // namespace air
