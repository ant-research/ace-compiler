//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "nn/vector/vec2c_driver.h"

#include "air/base/visitor.h"
#include "air/core/handler.h"
#include "air/core/ir2c_handler.h"
#include "nn/vector/handler.h"
#include "nn/vector/ir2c_vector.h"

using namespace air::base;

namespace nn {

namespace vector {

void VEC2C_DRIVER::Run(air::base::GLOB_SCOPE* glob) {
  // emit include
  _ctx.Emit_global_include();
  // emit global constants
  _ctx.Emit_global_constants(glob, true);

  // emit function prototype
  for (FUNC_ITER it = glob->Begin_func(); it != glob->End_func(); ++it) {
    if ((*it)->Entry_point()->Is_program_entry()) {
      continue;
    }
    _ctx.Emit_func_sig((*it));
    _ctx << ";\n";
  }
  _ctx << "\n";

  // emit functiom body
  for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
       it != glob->End_func_scope(); ++it) {
    FUNC_SCOPE* func = &(*it);
    NODE_PTR    body = func->Container().Stmt_list().Block_node();

    // emit C code
    air::base::VISITOR<nn::vector::IR2C_CTX,
                       air::core::HANDLER<air::core::IR2C_HANDLER>,
                       nn::vector::HANDLER<nn::vector::IR2C_VECTOR> >
        visitor(_ctx);

    _ctx.Emit_func_def(func);
    _ctx.Begin_func_body(body);
    _ctx.Emit_local_var(func);
    visitor.template Visit<void>(body);
    _ctx.End_func_body(body);
    _ctx << "\n";
  }
}

}  // namespace vector

}  // namespace nn
