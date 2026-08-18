//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_SHARDING_H
#define NN_VECTOR_SHARDING_H

#include <cmath>
#include <unordered_map>
#include <vector>

#include "air/base/container.h"
#include "air/base/container_decl.h"
#include "air/base/st.h"
#include "air/util/debug.h"

namespace nn {
namespace vector {

using namespace air::base;

class ARRAY_SHARDING {
public:
  ARRAY_SHARDING() {}
  ARRAY_SHARDING(CONTAINER* cntr)
      : _cntr(cntr),
        _fscope(cntr->Parent_func_scope()),
        _gscope(cntr->Glob_scope()) {}
  ~ARRAY_SHARDING() = default;

  void Set_cntr(CONTAINER* cntr) {
    _cntr   = cntr;
    _fscope = cntr->Parent_func_scope();
    _gscope = cntr->Glob_scope();
  }

  bool                 Is_sharding_op(NODE_PTR op) const;
  std::vector<int64_t> Get_node_map(NODE_PTR op) const;
  void Set_node_map(NODE_PTR op, const std::vector<int64_t>& vec);
  std::vector<int64_t> Get_data_map(FUNC_ID fid, STR_ID did) const;
  void Set_data_map(FUNC_ID fid, STR_ID did, const std::vector<int64_t>& vec);
  void Print() const;

  TYPE_PTR New_sharding_type(const std::string& ty_name,
                             uint32_t ty_name_suffix, int64_t num,
                             CONST_TYPE_PTR block_type, const SPOS& spos);
  void     Print_sharding_type(TYPE_PTR shard_type);

  std::vector<CONSTANT_PTR> Split_array(CONST_CONSTANT_PTR cst, int num_chunks,
                                        std::vector<int64_t> shape,
                                        CONST_TYPE_PTR etype, const SPOS& spos);
  std::vector<int64_t> Analyze_conv(int64_t channel_out, int64_t channel_in,
                                    int64_t height, int64_t width);

  NODE_PTR New_sharding_loader(CONST_ADDR_DATUM_PTR svar, int i,
                               const SPOS& spos);
  STMT_PTR New_sharding_store(CONST_ADDR_DATUM_PTR svar, int i, NODE_PTR rhs,
                              const SPOS& spos);
  STMT_PTR New_sharding_update_store(CONST_ADDR_DATUM_PTR svar, int i,
                                     NODE_PTR adder, const SPOS& spos);

  using SHMAP_NODE = std::map<FUNC_ID, std::map<NODE_ID, std::vector<int64_t>>>;
  using SHMAP_DATA = std::map<FUNC_ID, std::map<STR_ID, std::vector<int64_t>>>;

private:
  // <FUNC_ID, NODE_ID> -> strategy for each operation
  SHMAP_NODE  _smap_node;
  SHMAP_DATA  _smap_data;
  CONTAINER*  _cntr;
  FUNC_SCOPE* _fscope;
  GLOB_SCOPE* _gscope;
};

NODE_PTR New_conv_node(CONTAINER* cntr, NODE_PTR input, NODE_PTR weight,
                       NODE_PTR bias, TYPE_PTR rtype, const SPOS& spos);

}  // namespace vector
}  // namespace nn

#endif
