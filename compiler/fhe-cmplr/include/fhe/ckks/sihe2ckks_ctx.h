//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_VECTOR2CKKS_CTX_H
#define FHE_CKKS_VECTOR2CKKS_CTX_H

#include <map>

#include "air/base/transform_ctx.h"
#include "fhe/ckks/ckks_gen.h"
#include "fhe/ckks/config.h"
#include "fhe/core/lower_ctx.h"

namespace fhe {

namespace ckks {

class SIHE2CKKS_CTX : public air::base::TRANSFORM_CTX {
public:
  SIHE2CKKS_CTX(air::base::CONTAINER* cont, fhe::core::LOWER_CTX& ctx,
                const CKKS_CONFIG& cfg)
      : air::base::TRANSFORM_CTX(cont),
        _ckks_gen(cont, &ctx),
        _lower_ctx(ctx),
        _config(cfg) {}

  DECLARE_CKKS_OPTION_CONFIG_ACCESS_API(_config)

  fhe::core::LOWER_CTX& Lower_ctx() { return _lower_ctx; }

  CKKS_GEN& Ckks_gen() { return _ckks_gen; }

  //! @brief Get cached bootstrap function ID for given level, or null if not
  //! cached
  air::base::FUNC_ID Get_bts_func(uint32_t level) const {
    auto it = _bts_func_map.find(level);
    if (it != _bts_func_map.end()) {
      return it->second;
    }
    return air::base::FUNC_ID();
  }

  //! @brief Cache bootstrap function ID for given level
  void Set_bts_func(uint32_t level, air::base::FUNC_ID func_id) {
    _bts_func_map[level] = func_id;
  }

private:
  CKKS_GEN              _ckks_gen;
  fhe::core::LOWER_CTX& _lower_ctx;
  const CKKS_CONFIG&    _config;
  // Cache bootstrap functions by level to avoid duplicate generation
  std::map<uint32_t, air::base::FUNC_ID> _bts_func_map;
};

}  // namespace ckks

}  // namespace fhe
#endif  // FHE_CKKS_VECTOR2CKKS_CTX_H
