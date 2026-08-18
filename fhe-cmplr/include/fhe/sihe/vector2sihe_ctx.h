//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_SIHE_VECTOR2SIHE_CTX_H
#define FHE_SIHE_VECTOR2SIHE_CTX_H

#include <utility>

#include "air/base/st.h"
#include "air/base/st_data.h"
#include "air/base/st_decl.h"
#include "air/base/transform_ctx.h"
#include "air/driver/driver_ctx.h"
#include "air/util/debug.h"
#include "fhe/core/lower_ctx.h"
#include "fhe/sihe/config.h"

namespace fhe {

namespace sihe {

using namespace air::base;

class VECTOR2SIHE_CTX : public air::base::TRANSFORM_CTX {
public:
  VECTOR2SIHE_CTX(air::base::CONTAINER* cont, fhe::core::LOWER_CTX& ctx,
                  air::driver::DRIVER_CTX& driver_ctx, const SIHE_CONFIG& cfg,
                  const std::map<ENTRY_ID, ENTRY_ID>& entry_map)
      : air::base::TRANSFORM_CTX(cont),
        _lower_ctx(ctx),
        _driver_ctx(driver_ctx),
        _config(cfg),
        _vec2sihe_entry_map(entry_map) {}
  ~VECTOR2SIHE_CTX() {}

  // declare access API for SIHE_CONFIG
  DECLARE_SIHE_CONFIG_ACCESS_API(_config)

  using VEC2FHE_VAR_MAP = std::map<uint32_t, uint32_t>;

  fhe::core::LOWER_CTX& Lower_ctx() { return _lower_ctx; }

  PREG_PTR Get_sihe_preg(PREG_ID vec_preg, const FUNC_SCOPE& func_scope) {
    VEC2FHE_VAR_MAP::iterator iter = _vec2sihe_preg_map.find(vec_preg.Value());
    AIR_ASSERT(iter != _vec2sihe_preg_map.end());

    PREG_PTR sihe_preg = func_scope.Preg(PREG_ID(iter->second));
    return sihe_preg;
  }

  PREG_PTR Get_or_gen_sihe_preg(PREG_PTR vec_preg, FUNC_SCOPE& func_scope,
                                TYPE_PTR type) {
    uint32_t                  vec_preg_idx = vec_preg->Id().Value();
    VEC2FHE_VAR_MAP::iterator iter = _vec2sihe_preg_map.find(vec_preg_idx);
    if (iter != _vec2sihe_preg_map.end()) {
      PREG_PTR fhe_preg = func_scope.Preg(PREG_ID(iter->second));
      AIR_ASSERT(fhe_preg->Type_id() == type->Id());
      return fhe_preg;
    }

    // generate new preg in SIHE domain function scope
    PREG_PTR fhe_preg = func_scope.New_preg(type, vec_preg->Home_sym());
    _vec2sihe_preg_map.insert({vec_preg_idx, fhe_preg->Id().Value()});
    return fhe_preg;
  }

  ADDR_DATUM_PTR Get_sihe_addr_datum(ADDR_DATUM_ID     vec_addr_datum,
                                     const FUNC_SCOPE& func_scope) {
    VEC2FHE_VAR_MAP::iterator iter =
        _vec2sihe_addr_datum_map.find(vec_addr_datum.Value());
    AIR_ASSERT(iter != _vec2sihe_addr_datum_map.end());

    ADDR_DATUM_PTR sihe_addr_datum =
        func_scope.Addr_datum(ADDR_DATUM_ID(iter->second));
    AIR_ASSERT(sihe_addr_datum != ADDR_DATUM_PTR());
    return sihe_addr_datum;
  }

  ADDR_DATUM_PTR Get_or_gen_sihe_addr_datum(ADDR_DATUM_PTR vec_addr_datum,
                                            FUNC_SCOPE&    func_scope,
                                            TYPE_PTR       type) {
    uint32_t                  vec_addr_datum_idx = vec_addr_datum->Id().Value();
    VEC2FHE_VAR_MAP::iterator iter =
        _vec2sihe_addr_datum_map.find(vec_addr_datum_idx);
    if (iter != _vec2sihe_addr_datum_map.end()) {
      ADDR_DATUM_PTR addr_datum =
          func_scope.Addr_datum(ADDR_DATUM_ID(iter->second));
      AIR_ASSERT(addr_datum->Type_id() == type->Id());
      return addr_datum;
    }

    // generate new addr_datum in SIHE domain function scope
    const char*    fhe_sym_name(vec_addr_datum->Name()->Char_str());
    ADDR_DATUM_PTR fhe_addr_datum =
        func_scope.New_var(type, fhe_sym_name, vec_addr_datum->Spos());
    _vec2sihe_addr_datum_map.insert(
        {vec_addr_datum_idx, fhe_addr_datum->Id().Value()});
    return fhe_addr_datum;
  }

  void Set_sihe_addr_datum(ADDR_DATUM_ID vec_addr_datum,
                           ADDR_DATUM_ID fhe_addr_datum) {
    std::pair<VEC2FHE_VAR_MAP::iterator, bool> res =
        _vec2sihe_addr_datum_map.insert(
            {vec_addr_datum.Value(), fhe_addr_datum.Value()});
    AIR_ASSERT(res.first->second == fhe_addr_datum.Value());
  }

  ENTRY_PTR Get_sihe_entry(ENTRY_PTR         vec_entry,
                           const GLOB_SCOPE& glob_scope) const {
    AIR_ASSERT(&vec_entry->Glob_scope() != &glob_scope);
    std::map<ENTRY_ID, ENTRY_ID>::const_iterator res =
        _vec2sihe_entry_map.find(vec_entry->Id());
    AIR_ASSERT(res != _vec2sihe_entry_map.end());

    ENTRY_PTR sihe_entry = glob_scope.Entry_point(res->second);
    AIR_ASSERT(&sihe_entry->Glob_scope() == &glob_scope);
    return sihe_entry;
  }

  uint32_t Get_max_msg_len() const { return _max_msg_len; }
  void     Update_max_msg_len(uint32_t val) {
    _max_msg_len = (_max_msg_len > val ? _max_msg_len : val);
  }

  DECLARE_TRACE_DETAIL_API(_config, (&_driver_ctx));

private:
  // REQUIRED UNDEFINED UNWANTED methods
  VECTOR2SIHE_CTX(void);
  VECTOR2SIHE_CTX(const VECTOR2SIHE_CTX&);
  VECTOR2SIHE_CTX& operator=(const VECTOR2SIHE_CTX&);

  fhe::core::LOWER_CTX&               _lower_ctx;
  air::driver::DRIVER_CTX&            _driver_ctx;
  const SIHE_CONFIG&                  _config;
  const std::map<ENTRY_ID, ENTRY_ID>& _vec2sihe_entry_map;
  VEC2FHE_VAR_MAP                     _vec2sihe_preg_map;
  VEC2FHE_VAR_MAP                     _vec2sihe_addr_datum_map;
  uint32_t                            _max_msg_len = 0;
};

}  // namespace sihe

}  // namespace fhe
#endif  // FHE_SIHE_VECTOR2SIHE_CTX_H
