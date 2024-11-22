//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/opt/ssa_build.h"

#include "air/base/visitor.h"
#include "air/core/handler.h"
#include "ssa_rename_ctx.h"
#include "ssa_rename_handler.h"
#include "ssa_simple_builder.h"
#include "ssa_verify_ctx.h"

namespace air {

namespace opt {

void SSA_BUILDER::Build_simple() {
  Trace(TRACE_IR_BEFORE_SSA, "\nBefore SSA:\n");
  Trace_obj(TRACE_IR_BEFORE_SSA, _cont);

  // step 1: build SSA symtab by traversing IR
  SIMPLE_BUILDER_CTX build_ctx(_cont);
  _cont->Set_state(SSA_CONTAINER::SYM_CREATE);
  air::base::VISITOR<SIMPLE_BUILDER_CTX,
                     air::core::HANDLER<SIMPLE_SYMTAB_HANDLER> >
                      symtab_trav(build_ctx);
  air::base::NODE_PTR body = _scope->Container().Entry_node();
  symtab_trav.template Visit<void>(body);

  // step 2: insert PHI for each def along the block hierarchy
  _cont->Set_state(SSA_CONTAINER::PHI_INSERT);
  build_ctx.Insert_phi();

  Trace(TRACE_IR_AFTER_INSERT_PHI, "\nAfter phi insertion:\n");
  Trace_obj(TRACE_IR_AFTER_INSERT_PHI, _cont);

  // step 3: rename versions
  RENAME_CTX rename_ctx(_cont);
  _cont->Set_state(SSA_CONTAINER::RENAME);
  rename_ctx.Initialize(body->Id());
  air::base::VISITOR<RENAME_CTX, air::core::HANDLER<RENAME_HANDLER> >
      rename_trav(rename_ctx);
  rename_trav.template Visit<void>(body);
  rename_ctx.Finalize(body->Id());
  _cont->Set_state(SSA_CONTAINER::SSA);

  Trace(TRACE_IR_AFTER_SSA, "\nAfter renaming:\n");
  Trace_obj(TRACE_IR_AFTER_SSA, _cont);

  // step 4 (optional): verify SSA
  SIMPLE_VERIFIER_CTX verify_ctx(_cont);
  air::base::VISITOR<SIMPLE_VERIFIER_CTX, air::core::HANDLER<RENAME_HANDLER> >
      verify_trav(verify_ctx);
  verify_ctx.Initialize(body->Id());
  verify_trav.template Visit<void>(body);
  verify_ctx.Finalize(body->Id());
}

}  // namespace opt

}  // namespace air
