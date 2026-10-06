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

// Context for sharding transformaiton: function-level
class T2TSHARDING_CTX : public air::base::TRANSFORM_CTX {
public:
  T2TSHARDING_CTX(air::base::CONTAINER* cont, VECTOR_CTX& ctx,
                  const air::driver::DRIVER_CTX* driver_ctx,
                  const VECTOR_CONFIG& cfg, SHARDING_MAP* sharding)
      : air::base::TRANSFORM_CTX(cont),
        _ctx(ctx),
        _driver_ctx(driver_ctx),
        _cntr(cont),
        _config(cfg),
        _sharding(sharding) {
    sharding->Set_cntr(cont);
  }

  SHARDING_MAP* Sharding_map() { return _sharding; }
  // declare access API for VECTOR_CTX
  //
  DECLARE_VECTOR_CTX_ACCESS_API(_ctx)

  // declare access API for VECTOR_CONFIG
  DECLARE_VECTOR_CONFIG_ACCESS_API(_config)

  // declare trace API for detail tracing
  DECLARE_TRACE_DETAIL_API(_config, _driver_ctx)

  air::base::CONTAINER* Get_cntr() const { return _cntr; }
  ADDR_DATUM_PTR        Reshard(ADDR_DATUM_PTR shard_var, int num_reshard,
                                int numtoone, std::vector<int64_t> reshard_block_shape,
                                std::vector<int64_t> xyz, const SPOS& spos);

  ADDR_DATUM_PTR Add_shard(ADDR_DATUM_PTR shard0, CONSTANT_PTR shard1,
                           std::vector<int64_t> mesh, const SPOS& spos);
  // Range based loop for sharding
  STMT_PTR New_mesh_loop(CONTAINER* cntr, const char* index_str, int upper,
                         const SPOS& spos);
  std::vector<STMT_PTR> New_ranged_loop(CONTAINER*           cntr,
                                        std::vector<int64_t> range,
                                        const SPOS&          spos);

  bool Is_shard_data(ADDR_DATUM_PTR data) {
    // TODO: if shmap has it in data_sharding_map.
    if (data->Type()->Is_array() &&
        data->Type()->Cast_to_arr()->Elem_type()->Is_array())
      return true;
    return false;
  }

  int64_t Get_shard_size(ADDR_DATUM_PTR data) {
    AIR_ASSERT_MSG(Is_shard_data(data), "data is shard type");
    return data->Type()->Cast_to_arr()->Elem_count();
  }

  TYPE_PTR Get_block_type(ADDR_DATUM_PTR data) {
    AIR_ASSERT_MSG(Is_shard_data(data), "data is shard type");
    return data->Type()->Cast_to_arr()->Elem_type();
  }

  void New_eshard_slice(ADDR_DATUM_PTR input, ADDR_DATUM_PTR output,
                        std::vector<int> strides, int64_t halosize,
                        const SPOS& spos);
  void New_rshard_conv2d(NODE_PTR node, NODE_PTR new_input, CONSTANT_PTR weight,
                         CONSTANT_PTR bias, ADDR_DATUM_PTR output,
                         std::vector<int64_t> mesh, int64_t halosize,
                         const SPOS& spos);

private:
  VECTOR_CTX&                    _ctx;
  const air::driver::DRIVER_CTX* _driver_ctx;
  const VECTOR_CONFIG&           _config;
  air::base::CONTAINER*          _cntr;
  SHARDING_MAP*                  _sharding;
};

}  // namespace vector

}  // namespace nn

#endif  // NN_SHARDING_CTX_H
