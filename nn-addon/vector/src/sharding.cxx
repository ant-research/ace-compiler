//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "nn/vector/sharding.h"

#include <numeric>
#include <unordered_map>

#include "nn/core/opcode.h"
#include "nn/vector/vector_utils.h"

namespace nn {
namespace vector {

using namespace air::base;

// Implementation of Get_map method
std::vector<int64_t> ARRAY_SHARDING::Get_node_map(NODE_PTR op) const {
  FUNC_ID fid = op->Func_scope()->Id();
  NODE_ID nid = op->Id();
  auto    it1 = _smap_node.find(fid);
  if (it1 != _smap_node.end()) {
    auto it2 = it1->second.find(nid);
    if (it2 != it1->second.end()) {
      return it2->second;
    }
  }
  return {};
}

bool ARRAY_SHARDING::Is_sharding_op(NODE_PTR op) const {
  if (Get_node_map(op).empty()) return false;
  return true;
}

void ARRAY_SHARDING::Set_node_map(NODE_PTR                    op,
                                  const std::vector<int64_t>& vec) {
  FUNC_ID fid          = op->Func_scope()->Id();
  NODE_ID nid          = op->Id();
  _smap_node[fid][nid] = vec;  // Set the vector for the given id
}

// split cst into equally sized blocks, each const with "block_shape"
std::vector<CONSTANT_PTR> ARRAY_SHARDING::Split_array(
    CONST_CONSTANT_PTR cst, int num_blocks, std::vector<int64_t> block_shape,
    CONST_TYPE_PTR etype, const SPOS& spos) {
  AIR_ASSERT(cst->Kind() == CONSTANT_KIND::ARRAY);
  int64_t array_size = cst->Type()->Cast_to_arr()->Elem_count();
  int64_t block_size = std::accumulate(block_shape.begin(), block_shape.end(),
                                       1, std::multiplies<int64_t>());

  AIR_ASSERT(array_size == num_blocks * block_size);

  std::vector<CONSTANT_PTR>        blocks_const;
  std::vector<std::vector<float> > blocks_data;

  // Create a vector from the input array
  const float*       cptr_data = cst->Array_ptr<float>();
  std::vector<float> input(cptr_data, cptr_data + array_size);
  // Split the input vector into blocks
  for (int i = 0; i < num_blocks; ++i) {
    auto start_iter = input.begin() + i * block_size;
    blocks_data.emplace_back(start_iter, start_iter + block_size);
  }

  TYPE_PTR block_type =
      New_array_type(_gscope, "block", etype, block_shape, spos);
  int bsz = etype->Cast_to_prim()->Byte_size();

  for (int i = 0; i < num_blocks; ++i) {
    CONSTANT_PTR block_const =
        _gscope->New_const(CONSTANT_KIND::ARRAY, block_type,
                           (void*)blocks_data[i].data(), block_size * bsz);
    blocks_const.push_back(block_const);
  }
  return blocks_const;
}

// svar[i]
NODE_PTR ARRAY_SHARDING::New_sharding_loader(CONST_ADDR_DATUM_PTR svar, int i,
                                             const SPOS& spos) {
  CONST_TYPE_PTR s32_type   = _gscope->Prim_type(PRIMITIVE_TYPE::INT_S32);
  NODE_PTR       sharding_i = _cntr->New_array(
      _cntr->New_lda(svar, POINTER_KIND::FLAT32, spos), 1, spos);
  _cntr->Set_array_idx(sharding_i, 0, _cntr->New_intconst(s32_type, i, spos));

  return _cntr->New_ild(sharding_i, spos);
}

// svar[i] = rhs
STMT_PTR ARRAY_SHARDING::New_sharding_store(CONST_ADDR_DATUM_PTR svar, int i,
                                            NODE_PTR rhs, const SPOS& spos) {
  CONST_TYPE_PTR s32_type   = _gscope->Prim_type(PRIMITIVE_TYPE::INT_S32);
  NODE_PTR       sharding_i = _cntr->New_array(
      _cntr->New_lda(svar, POINTER_KIND::FLAT32, spos), 1, spos);
  _cntr->Set_array_idx(sharding_i, 0, _cntr->New_intconst(s32_type, i, spos));

  STMT_PTR sharding_update = _cntr->New_ist(sharding_i, rhs, spos);
  return sharding_update;
}

// svar[i] += adder
STMT_PTR ARRAY_SHARDING::New_sharding_update_store(CONST_ADDR_DATUM_PTR svar,
                                                   int i, NODE_PTR adder,
                                                   const SPOS& spos) {
  CONST_TYPE_PTR s32_type   = _gscope->Prim_type(PRIMITIVE_TYPE::INT_S32);
  NODE_PTR       sharding_i = _cntr->New_array(
      _cntr->New_lda(svar, POINTER_KIND::FLAT32, spos), 1, spos);
  _cntr->Set_array_idx(sharding_i, 0, _cntr->New_intconst(s32_type, i, spos));

  STMT_PTR sharding_update =
      _cntr->New_ist(sharding_i,
                     _cntr->New_bin_arith(
                         air::base::OPCODE(nn::core::NN, nn::core::OPCODE::ADD),
                         _cntr->New_ild(sharding_i, spos), adder, spos),
                     spos);
  return sharding_update;
}

// data and node: seperate. Actually node is essential to sharding.
void ARRAY_SHARDING::Set_data_map(FUNC_ID fid, STR_ID did,
                                  const std::vector<int64_t>& vec) {
  _smap_data[fid][did] = vec;
}

std::vector<int64_t> ARRAY_SHARDING::Get_data_map(FUNC_ID fid,
                                                  STR_ID  did) const {
  auto it1 = _smap_data.find(fid);
  if (it1 != _smap_data.end()) {
    auto it2 = it1->second.find(did);
    if (it2 != it1->second.end()) {
      return it2->second;
    }
  }
  return {};
}

std::vector<int64_t> ARRAY_SHARDING::Analyze_conv(int64_t channel_out,
                                                  int64_t channel_in,
                                                  int64_t height,
                                                  int64_t width) {
  std::vector<int64_t> dim3{channel_out, channel_in, 1};
  return dim3;
}

void ARRAY_SHARDING::Print() const {
  // TODO: trace
  std::cout << "ARRAY_SHARDING smap_node:  size=" << _smap_node.size()
            << std::endl;
  std::cout << "ARRAY_SHARDING smap_data:  size=" << _smap_data.size()
            << std::endl;
}

// sharding tensor/array: num blocks of block_shape
// It describes the distributions of the input/output tensors across computation
// mesh.
TYPE_PTR
ARRAY_SHARDING::New_sharding_type(const std::string& ty_name,
                                  uint32_t ty_name_suffix, int64_t num,
                                  CONST_TYPE_PTR block_type, const SPOS& spos) {
  std::vector<int64_t> num_shape{num};

  TYPE_PTR sharding_type =
      New_array_type(_gscope, "Tsharding_" + ty_name, ty_name_suffix,
                     block_type, num_shape, spos);
  return sharding_type;
}

void ARRAY_SHARDING::Print_sharding_type(TYPE_PTR shard_type) {
  std::vector<int64_t> shape = shard_type->Cast_to_arr()->Shape();
  std::cout << "Print_sharding_array_type:  size=" << shape.size() << std::endl;
}

// Build a conv node
NODE_PTR New_conv_node(CONTAINER* cntr, NODE_PTR input, NODE_PTR weight,
                       NODE_PTR bias, TYPE_PTR rtype, const SPOS& spos) {
  NODE_PTR conv_node = cntr->New_cust_node(
      air::base::OPCODE(nn::core::NN, nn::core::OPCODE::CONV), rtype, spos);
  conv_node->Set_child(0, input);
  conv_node->Set_child(1, weight);
  conv_node->Set_child(2, bias);
  return conv_node;
}

}  // namespace vector
}  // namespace nn
