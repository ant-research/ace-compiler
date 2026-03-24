//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_POLY_POLY2C_DRIVER_H
#define FHE_POLY_POLY2C_DRIVER_H

#include "air/base/container.h"
#include "air/base/visitor.h"
#include "air/core/handler.h"
#include "fhe/ckks/ckks2c_mfree.h"
#include "fhe/ckks/ckks_handler.h"
#include "fhe/ckks/ir2c_core.h"
#include "fhe/ckks/ir2c_handler.h"
#include "fhe/core/lower_ctx.h"
#include "fhe/poly/handler.h"
#include "fhe/poly/ir2c_core.h"
#include "fhe/poly/ir2c_handler.h"
#include "fhe/poly/poly2c_mfree.h"
#include "fhe/sihe/ir2c_handler.h"
#include "fhe/sihe/sihe_handler.h"
#include "nn/vector/handler.h"
#include "nn/vector/ir2c_vector.h"

namespace fhe {

namespace poly {

typedef air::base::VISITOR<fhe::poly::IR2C_CTX,
                           air::core::HANDLER<fhe::poly::IR2C_CORE>,
                           nn::vector::HANDLER<nn::vector::IR2C_VECTOR>,
                           fhe::sihe::HANDLER<fhe::sihe::IR2C_HANDLER>,
                           fhe::ckks::HANDLER<fhe::ckks::IR2C_HANDLER>,
                           fhe::poly::HANDLER<fhe::poly::IR2C_HANDLER> >
    POLY2C_VISITOR;

typedef air::base::VISITOR<fhe::poly::IR2C_CTX,
                           air::core::HANDLER<fhe::ckks::IR2C_CORE>,
                           nn::vector::HANDLER<nn::vector::IR2C_VECTOR>,
                           fhe::sihe::HANDLER<fhe::sihe::IR2C_HANDLER>,
                           fhe::ckks::HANDLER<fhe::ckks::IR2C_HANDLER> >
    CKKS2C_VISITOR;

/**
 * @brief Polynomial To C Main Driver
 *
 */
class POLY2C_DRIVER {
public:
  POLY2C_DRIVER(std::ostream& os, core::LOWER_CTX& lower_ctx,
                const POLY2C_CONFIG& cfg)
      : _ctx(os, lower_ctx, cfg) {}

  air::base::GLOB_SCOPE* Flatten(air::base::GLOB_SCOPE* in_scope);

  template <typename VISITOR>
  void Run(air::base::GLOB_SCOPE* in_scope, VISITOR& visitor);

  IR2C_CTX& Ctx() { return _ctx; }

private:
  uint32_t Emit_chunk_info(air::base::NODE_PTR node, uint32_t idx);
  void     Emit_data_shape(air::base::NODE_PTR node);
  void     Emit_get_context_params();
  void     Emit_helper_function(air::base::FUNC_SCOPE* func_scope);

  IR2C_CTX _ctx;
};  // POLY2C_DRIVER

template <typename VISITOR>
void POLY2C_DRIVER::Run(air::base::GLOB_SCOPE* glob, VISITOR& visitor) {
  _ctx.Emit_global_include();
  _ctx.Emit_global_constants(glob, true);

  // emit function prototype
  for (air::base::FUNC_ITER it = glob->Begin_func(); it != glob->End_func();
       ++it) {
    if ((*it)->Entry_point()->Is_program_entry()) {
      continue;
    }
    _ctx.Emit_func_sig((*it));
    _ctx << ";\n";
  }
  _ctx << "\n";

  for (air::base::GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
       it != glob->End_func_scope(); ++it) {
    air::base::FUNC_SCOPE* func = &(*it);
    air::base::NODE_PTR    body = func->Container().Stmt_list().Block_node();

    if (_ctx.Provider() == core::PROVIDER::ANT && _ctx.Free_poly()) {
      // insert free before emit C code
      fhe::poly::MFREE_PASS mfree(_ctx.Lower_ctx());
      mfree.Perform(body);
    } else if (_ctx.Provider() == core::PROVIDER::SEAL ||
               _ctx.Provider() == core::PROVIDER::PHANTOM) {
      // insert free before emit C code
      fhe::ckks::MFREE_PASS mfree(_ctx.Lower_ctx());
      mfree.Perform(body);
    }

    // emit C code
    _ctx.Emit_func_def(func);
    _ctx.Begin_func_body(body);
    // Trace runtime of rotate and relin operations
    const core::LOWER_CTX& lower_ctx = _ctx.Lower_ctx();
    if (func->Id() ==
        lower_ctx.Get_func_info(core::FHE_FUNC::ROTATE).Get_func_id()) {
      _ctx << "  RTLIB_TM_START(" << (uint32_t)(core::RTM_FHE_ROTATE)
           << ", rtm);\n";
    } else if (func->Id() ==
               lower_ctx.Get_func_info(core::FHE_FUNC::RELIN).Get_func_id()) {
      _ctx << "  RTLIB_TM_START(" << (uint32_t)(core::RTM_FHE_RELIN)
           << ", rtm);\n";
    }
    _ctx.Emit_local_var(func);

    // Emit domain specific body to C code
    visitor.template Visit<void>(body);

    _ctx.End_func_body(body);
    _ctx << "\n";

    if (func->Owning_func()->Entry_point()->Is_program_entry()) {
      Emit_helper_function(func);
    }
  }

  Emit_get_context_params();

  _ctx.Emit_need_bts();
  _ctx.Emit_global_constants(glob, false);
}

}  // namespace poly

}  // namespace fhe

#endif  // FHE_POLY_POLY2C_DRIVER_H
