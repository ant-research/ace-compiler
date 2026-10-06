//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_SIHE_VECTOR2SIHE_LOWER_H
#define FHE_SIHE_VECTOR2SIHE_LOWER_H

#include <map>

#include "air/base/container.h"
#include "air/base/st.h"
#include "air/base/st_decl.h"
#include "air/base/transform_ctx.h"
#include "air/base/visitor.h"
#include "air/core/handler.h"
#include "air/driver/driver_ctx.h"
#include "air/util/debug.h"
#include "fhe/sihe/config.h"
#include "fhe/sihe/core_lower_impl.h"
#include "fhe/sihe/sihe_gen.h"
#include "fhe/sihe/sihe_handler.h"
#include "fhe/sihe/sihe_opcode.h"
#include "fhe/sihe/tensor2sihe_impl.h"
#include "fhe/sihe/vector2sihe_ctx.h"
#include "fhe/sihe/vector2sihe_impl.h"
#include "nn/core/handler.h"
#include "nn/vector/handler.h"
#include "nn/vector/vector_gen.h"

namespace fhe {
namespace sihe {
using namespace air::base;
using namespace air::core;
using namespace nn::vector;

class VECTOR2SIHE_LOWER {
public:
  using CORE_HANDLER   = air::core::HANDLER<core::CORE_LOWER>;
  using TENSOR_HANDLER = nn::core::HANDLER<TENSOR2SIHE_IMPL>;
  using VECTOR_HANDLER = nn::vector::HANDLER<VECTOR2SIHE_IMPL>;
  using LOWER_VISITOR =
      VISITOR<VECTOR2SIHE_CTX, CORE_HANDLER, TENSOR_HANDLER, VECTOR_HANDLER>;

  VECTOR2SIHE_LOWER(GLOB_SCOPE* glob_scope, core::LOWER_CTX* lower_ctx,
                    air::driver::DRIVER_CTX* driver_ctx, const SIHE_CONFIG& cfg)
      : _glob_scope(glob_scope),
        _lower_ctx(lower_ctx),
        _driver_ctx(driver_ctx),
        _config(cfg) {}
  ~VECTOR2SIHE_LOWER() {}

  FUNC_SCOPE& Lower_vec_func(FUNC_SCOPE* vec_func_scope);
  void        Lower_func_tab(GLOB_SCOPE* vec_glob_scope);

private:
  // REQUIRED UNDEFINED UNWANTED methods
  VECTOR2SIHE_LOWER(void);
  VECTOR2SIHE_LOWER(const VECTOR2SIHE_LOWER&);
  VECTOR2SIHE_LOWER& operator=(const VECTOR2SIHE_LOWER&);

  void Lower_formal(FUNC_SCOPE* vec_func_scope, FUNC_SCOPE* sihe_func_scope,
                    VECTOR2SIHE_CTX& ctx);

  void Lower_func_body(FUNC_SCOPE* vec_func_scope, FUNC_SCOPE* sihe_func_scope);

  GLOB_SCOPE*              Glob_scope() const { return _glob_scope; }
  core::LOWER_CTX*         Lower_ctx() const { return _lower_ctx; }
  air::driver::DRIVER_CTX* Driver_ctx() const { return _driver_ctx; }
  const SIHE_CONFIG&       Config() const { return _config; }

  std::map<FUNC_ID, FUNC_ID>& Func_map() { return _vec2sihe_func_map; }
  inline FUNC_PTR             Get_sihe_func(FUNC_PTR vec_func) {
    AIR_ASSERT(&vec_func->Glob_scope() != _glob_scope);
    std::map<FUNC_ID, FUNC_ID>::iterator res =
        _vec2sihe_func_map.find(vec_func->Id());
    AIR_ASSERT(res != _vec2sihe_func_map.end());
    return _glob_scope->Func(res->second);
  }

  std::map<ENTRY_ID, ENTRY_ID>& Entry_map() { return _vec2sihe_entry_map; }
  inline ENTRY_PTR              Get_sihe_entry(ENTRY_PTR vec_entry) {
    AIR_ASSERT(&vec_entry->Glob_scope() != _glob_scope);
    std::map<ENTRY_ID, ENTRY_ID>::iterator res =
        _vec2sihe_entry_map.find(vec_entry->Id());
    AIR_ASSERT(res != _vec2sihe_entry_map.end());
    return _glob_scope->Entry_point(res->second);
  }

  GLOB_SCOPE*                  _glob_scope;
  core::LOWER_CTX*             _lower_ctx;
  air::driver::DRIVER_CTX*     _driver_ctx;
  const SIHE_CONFIG&           _config;
  std::map<FUNC_ID, FUNC_ID>   _vec2sihe_func_map;
  std::map<ENTRY_ID, ENTRY_ID> _vec2sihe_entry_map;
};

}  // namespace sihe
}  // namespace fhe
#endif
