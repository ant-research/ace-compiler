//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_T2TSHARDING_CTX_H
#define NN_T2TSHARDING_CTX_H

#include "air/base/transform_ctx.h"
#include "air/core/opcode.h"
#include "nn/vector/config.h"
#include "nn/vector/sharding.h"
#include "nn/vector/vector_ctx.h"
#include "nn/vector/vector_enum.h"
#include "nn/vector/vector_utils.h"

namespace nn {

namespace vector {

class T2TSHARDING_CTX : public air::base::TRANSFORM_CTX {
public:
  T2TSHARDING_CTX(air::base::CONTAINER* cont, VECTOR_CTX& ctx,
                  const air::driver::DRIVER_CTX* driver_ctx,
                  const VECTOR_CONFIG& cfg, ARRAY_SHARDING* sharding)
      : air::base::TRANSFORM_CTX(cont),
        _ctx(ctx),
        _driver_ctx(driver_ctx),
        _config(cfg),
        _sharding(sharding) {
    sharding->Set_cntr(cont);
  }

  using VAR_MAP = std::map<ADDR_DATUM_ID, ADDR_DATUM_ID>;

  ARRAY_SHARDING* Sharding() { return _sharding; }
  // declare access API for VECTOR_CTX
  DECLARE_VECTOR_CTX_ACCESS_API(_ctx)

  // declare access API for VECTOR_CONFIG
  DECLARE_VECTOR_CONFIG_ACCESS_API(_config)

  // declare trace API for detail tracing
  DECLARE_TRACE_DETAIL_API(_config, _driver_ctx)

  void Set_addr_datum_map(ADDR_DATUM_ID addr_datum_id,
                          ADDR_DATUM_ID new_addr_datum_id) {
    std::pair<VAR_MAP::iterator, bool> res =
        _addr_datum_map.insert({addr_datum_id, new_addr_datum_id});
    AIR_ASSERT(res.first->second == new_addr_datum_id);
  }

  ADDR_DATUM_ID Get_addr_datum_map(ADDR_DATUM_ID addr_datum_id) {
      VAR_MAP::iterator iter = _addr_datum_map.find(addr_datum_id);
      AIR_ASSERT(iter != _addr_datum_map.end());
      return iter->second;
  }

private:
  VECTOR_CTX&                    _ctx;
  const air::driver::DRIVER_CTX* _driver_ctx;
  const VECTOR_CONFIG&           _config;
  ARRAY_SHARDING*                _sharding;
  VAR_MAP                        _addr_datum_map;
};

}  // namespace vector

}  // namespace nn

#endif  // NN_SHARDING_CTX_H
