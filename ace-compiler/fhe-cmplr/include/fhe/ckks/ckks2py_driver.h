//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_CKKS2PY_DRIVER_H
#define FHE_CKKS_CKKS2PY_DRIVER_H

#include <string>

#include "air/base/container.h"
#include "air/base/visitor.h"
#include "air/core/handler.h"
#include "fhe/ckks/ckks2c_mfree.h"
#include "fhe/ckks/ckks_handler.h"
#include "fhe/ckks/ir2py_core.h"
#include "fhe/ckks/ir2py_handler.h"
#include "fhe/core/lower_ctx.h"
#include "fhe/ckks/ir2py_ctx.h"
#include "fhe/sihe/ir2c_handler.h"
#include "fhe/sihe/sihe_handler.h"
#include "nn/vector/handler.h"
#include "nn/vector/ir2c_vector.h"

namespace fhe {

namespace ckks {

//! @brief CKKS IR -> Python visitor (non-ant path).
//!
//! Mirrors CKKS2C_VISITOR (poly2c_driver.h:43-48): same domain handlers
//! (air::core + nn::vector + fhe::sihe + fhe::ckks) but with the Python IMPL
//! (IR2PY_CTX / IR2PY_CORE / IR2PY_HANDLER). There is NO fhe::poly handler --
//! the Python backend never lowers to poly IR.
typedef air::base::VISITOR<fhe::ckks::IR2PY_CTX,
                           air::core::HANDLER<fhe::ckks::IR2PY_CORE>,
                           nn::vector::HANDLER<nn::vector::IR2C_VECTOR>,
                           fhe::sihe::HANDLER<fhe::sihe::IR2C_HANDLER>,
                           fhe::ckks::HANDLER<fhe::ckks::IR2PY_HANDLER>>
    CKKS2PY_VISITOR;

/**
 * @brief CKKS IR To Python Main Driver
 *
 * Mirrors POLY2C_DRIVER but holds an IR2PY_CTX (whose TYPE selects the Python
 * Emit_* via static resolution). Run<VISITOR> walks each FUNC_SCOPE and the
 * visitor emits the Python body (Phase 1.1).
 */
class CKKS2PY_DRIVER {
public:
  CKKS2PY_DRIVER(std::ostream& os, fhe::core::LOWER_CTX& lower_ctx,
                 const fhe::ckks::CKKS2PY_CONFIG& cfg)
      : _ctx(os, lower_ctx, cfg) {}

  //! @brief Flatten the glob scope (same transformation the non-ant C path
  //! performs when poly lowering is disabled). Reused verbatim from
  //! POLY2C_DRIVER::Flatten -- it is ctx/provider independent.
  air::base::GLOB_SCOPE* Flatten(air::base::GLOB_SCOPE* in_scope);

  template <typename VISITOR>
  void Run(air::base::GLOB_SCOPE* in_scope, VISITOR& visitor);

  IR2PY_CTX& Ctx() { return _ctx; }

private:
  //! @brief Emit `def get_context_params(): return { ... }` -- a Python dict
  //! mirroring the C `CKKS_PARAMS Get_context_params()` fields.
  void Emit_get_context_params();

  //! @brief Emit `def get_rt_data_info()` returning a dict {file, uuid,
  //! entry_type} (or None when no `-CKKS2PY:df=`), mirroring C Get_rt_data_info
  //! (poly2c_driver.cxx:108-121). Phase 2.3: rt_ref reads `file` to locate the
  //! C binary side data file for offline encode (load_data_entry).
  void Emit_rt_data_info();

  //! @brief Emit `def get_input_count()` / `def get_output_count()` +
  //! `get_encode_scheme(idx)` / `get_decode_scheme(idx)` for the program entry,
  //! mirroring C Emit_helper_function (poly2c_driver.cxx:157-220). The scheme
  //! dicts are parity metadata (rt_ref's offline encode reads by index).
  void Emit_helper_function(air::base::FUNC_SCOPE* func_scope);

  //! @brief Emit a Python 4-int shape list `[n,c,h,w]` mirroring C
  //! Emit_data_shape (poly2c_driver.cxx:140-155).
  void Emit_data_shape_py(air::base::NODE_PTR node);

  //! @brief Emit one Python DATA_SCHEME dict mirroring C
  //! Get_encode_scheme/Get_decode_scheme's per-scheme struct
  //! (poly2c_driver.cxx:172-183) + DATA_CHUNK::To_str (data_scheme.cxx:21-34).
  void Emit_data_scheme_py(const std::string& name, air::base::NODE_PTR node);

  IR2PY_CTX _ctx;
};  // CKKS2PY_DRIVER

template <typename VISITOR>
void CKKS2PY_DRIVER::Run(air::base::GLOB_SCOPE* glob, VISITOR& visitor) {
  _ctx.Emit_global_include();
  _ctx.Emit_global_constants(glob, true);  // Python: decl-only is a no-op

  // Python has no forward declarations -> the C function-prototype loop is
  // skipped.

  for (air::base::GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
       it != glob->End_func_scope(); ++it) {
    air::base::FUNC_SCOPE* func = &(*it);
    air::base::NODE_PTR    body = func->Container().Stmt_list().Block_node();

    // Insert Free_cipher nodes for dead ciphers before emission, gated on the
    // explicit -CKKS2PY:fp flag (_ctx.Free_poly()). This is result-neutral on
    // every reachable Python provider -- rt_ref/rt_openfhe/rt_fhert
    // free_ciph/free_plain are no-ops (Python GC owns handles) -- so it is a
    // parity/cosmetic opt-in, not a resource-correctness requirement. Default
    // off keeps the default output byte-identical (_free_poly defaults false,
    // poly2c_config.h:28). Free_poly() is reachable via the IR2C_CTX base
    // config macro.
    if (_ctx.Free_poly()) {
      fhe::ckks::MFREE_PASS mfree(_ctx.Lower_ctx());
      mfree.Perform(body);
    }

    // emit Python code
    _ctx.Emit_func_def(func);
    _ctx.Begin_func_body(body);
    _ctx.Emit_local_var(func);

    if (body->Begin_stmt() == body->End_stmt()) {
      // Python requires a non-empty suite -- emit `pass` for empty bodies.
      _ctx << "  pass\n";
    } else {
      // Visit the body block. IR2PY_CORE::Handle_block routes statement
      // terminators through IR2PY_CTX (no `;`), and Handle_retv emits
      // `set_output_data(...)` + `return True` for the program entry.
      visitor.template Visit<void>(body);
    }

    _ctx.End_func_body(body);
    _ctx << "\n";

    if (func->Owning_func()->Entry_point()->Is_program_entry()) {
      Emit_helper_function(func);
      // Emit config file for the program entry (provider-agnostic, same as
      // the non-ant C path).
      std::string             conf_file_name(_ctx.Config());
      const fhe::core::LOWER_CTX& lower_ctx = _ctx.Lower_ctx();
      lower_ctx.Emit_conf(conf_file_name, func, _ctx.Data_file(),
                          static_cast<uint32_t>(_ctx.Provider()));
    }
  }

  Emit_get_context_params();
  Emit_rt_data_info();                    // Phase 2.3: offline data-file descriptor
  _ctx.Emit_need_bts();                   // Python: no-op
  _ctx.Emit_global_constants(glob, false);  // scalar `name = value` (or no-op)
}

}  // namespace ckks

}  // namespace fhe

#endif  // FHE_CKKS_CKKS2PY_DRIVER_H