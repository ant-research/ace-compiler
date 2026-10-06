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

#include "nn/vector/t2tsharding_ctx.h"
#include "nn/vector/vector_gen.h"

namespace nn {
namespace vector {

using namespace air::base;

// Implementation of Get_map method
OP_SHARDING SHARDING_MAP::Get_op_sharding(NODE_PTR op) const {
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

bool SHARDING_MAP::Is_op_sharding(NODE_PTR op) const {
  FUNC_ID fid = op->Func_scope()->Id();
  NODE_ID nid = op->Id();
  auto    it1 = _smap_node.find(fid);
  if (it1 == _smap_node.end()) return false;
  auto it2 = it1->second.find(nid);
  if (it2 == it1->second.end()) return false;
  return true;
}

void SHARDING_MAP::Set_op_sharding(NODE_PTR op, const OP_SHARDING& vec) {
  FUNC_ID fid          = op->Func_scope()->Id();
  NODE_ID nid          = op->Id();
  _smap_node[fid][nid] = vec;  // Set the vector for the given id
}

// Input: bias is [channel_out]
// broadcast in H/W dimensions
// Output: [1, channel_out, height, width]
CONSTANT_PTR SHARDING_MAP::Broadcast_bias(CONST_CONSTANT_PTR cst,
                                          int64_t channel_out, int64_t height,
                                          int64_t width, const SPOS& spos) {
  AIR_ASSERT(cst->Kind() == CONSTANT_KIND::ARRAY);
  AIR_ASSERT(cst->Type()->Cast_to_arr()->Elem_count() == channel_out);
  TYPE_PTR           etype        = cst->Type()->Cast_to_arr()->Elem_type();
  const float*       cptr_data    = cst->Array_ptr<float>();
  int64_t            spatial_size = height * width;
  std::vector<float> bias_broadcast(channel_out * spatial_size, 0.0);

  for (int64_t c = 0; c < channel_out; c++) {
    float bv = cptr_data[c];
    for (int64_t s = 0; s < spatial_size; s++) {
      int64_t index         = c * spatial_size + s;
      bias_broadcast[index] = bv;
    }
  }

  std::vector<int64_t> broadcast_shape = {1, channel_out, height, width};
  TYPE_PTR             broadcast_type =
      New_array_type(_gscope, "TBRC_bias", etype, broadcast_shape, spos);
  int          bsz             = etype->Cast_to_prim()->Byte_size();
  CONSTANT_PTR largebias_const = _gscope->New_const(
      CONSTANT_KIND::ARRAY, broadcast_type, bias_broadcast.data(),
      channel_out * spatial_size * bsz);
  return largebias_const;
}

// split cst into equally sized blocks, each const with "block_shape"
CONSTANT_PTR SHARDING_MAP::Split_array_const(CONST_CONSTANT_PTR   cst,
                                             int                  num_blocks,
                                             std::vector<int64_t> block_shape,
                                             CONST_TYPE_PTR       etype,
                                             const SPOS&          spos) {
  AIR_ASSERT(cst->Kind() == CONSTANT_KIND::ARRAY);
  int64_t array_size = cst->Type()->Cast_to_arr()->Elem_count();
  int64_t block_size = std::accumulate(block_shape.begin(), block_shape.end(),
                                       1, std::multiplies<int64_t>());

  AIR_ASSERT(array_size == num_blocks * block_size);
  std::vector<CONSTANT_PTR> blocks_const;

  // Create a vector from the input array
  const float*       cptr_data = cst->Array_ptr<float>();
  std::vector<float> input(cptr_data, cptr_data + array_size);
  // Split the input const into blocks
  // Note the data is contiguous. Splitter just change the view.
  // [F, C, K, K] -> [x*y][F/x, C/y, K, K]  Very important.
  // T2V will use the view to transpose.

  TYPE_PTR block_type =
      New_array_type(_gscope, "Tconst_block", etype, block_shape, spos);
  TYPE_PTR     sharding_type  = New_array_type(_gscope, "Tconst_sharding",
                                               block_type, {num_blocks}, spos);
  int          bsz            = etype->Cast_to_prim()->Byte_size();
  CONSTANT_PTR const_sharding = _gscope->New_const(
      CONSTANT_KIND::ARRAY, sharding_type, input.data(), array_size * bsz);

  return const_sharding;
}

// Loader and storer for sharding
// svar[i]
NODE_PTR New_sharding_loader(CONTAINER* cntr, CONST_ADDR_DATUM_PTR svar,
                             NODE_PTR i, const SPOS& spos) {
  CONST_TYPE_PTR s32_type =
      cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_S32);
  NODE_PTR sharding_i =
      cntr->New_array(cntr->New_lda(svar, POINTER_KIND::FLAT32, spos), 1, spos);
  cntr->Set_array_idx(sharding_i, 0, i);

  return cntr->New_ild(sharding_i, spos);
}

// const[i]
NODE_PTR New_sharding_loader(CONTAINER* cntr, CONST_CONSTANT_PTR cvar,
                             NODE_PTR i, const SPOS& spos) {
  CONST_TYPE_PTR s32_type =
      cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_S32);
  NODE_PTR sharding_i = cntr->New_array(
      cntr->New_ldca(cvar, POINTER_KIND::FLAT32, spos), 1, spos);
  cntr->Set_array_idx(sharding_i, 0, i);

  return cntr->New_ild(sharding_i, spos);
}

NODE_PTR New_sharding_const_loader(CONTAINER* cntr, CONST_CONSTANT_PTR cvar,
                                   NODE_PTR i, const SPOS& spos) {
  CONST_TYPE_PTR s32t = cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_S32);

  AIR_ASSERT(cvar->Type()->Is_array());
  std::vector<int64_t> shape = cvar->Type()->Cast_to_arr()->Shape();
  AIR_ASSERT(shape.size() == 2);

  VECTOR_GEN vgen(cntr);
  NODE_PTR   block_slice =
      vgen.New_slice(cntr->New_ldc(cvar, spos), i,
                     cntr->New_intconst(s32t, shape[1], spos), spos);
  return block_slice;
}

// svar[i] = rhs
STMT_PTR New_sharding_store(CONTAINER* cntr, CONST_ADDR_DATUM_PTR svar,
                            NODE_PTR i, NODE_PTR rhs, const SPOS& spos) {
  CONST_TYPE_PTR s32_type =
      cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_S32);
  NODE_PTR sharding_i =
      cntr->New_array(cntr->New_lda(svar, POINTER_KIND::FLAT32, spos), 1, spos);
  cntr->Set_array_idx(sharding_i, 0, i);

  STMT_PTR sharding_update = cntr->New_ist(sharding_i, rhs, spos);
  return sharding_update;
}

// svar[i] += adder
STMT_PTR New_sharding_update_store(CONTAINER* cntr, CONST_ADDR_DATUM_PTR svar,
                                   NODE_PTR i, NODE_PTR adder,
                                   const SPOS& spos) {
  CONST_TYPE_PTR s32_type =
      cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_S32);
  NODE_PTR sharding_i =
      cntr->New_array(cntr->New_lda(svar, POINTER_KIND::FLAT32, spos), 1, spos);
  cntr->Set_array_idx(sharding_i, 0, i);

  STMT_PTR sharding_update = cntr->New_ist(
      sharding_i,
      cntr->New_bin_arith(
          air::base::OPCODE(nn::core::NN, nn::core::OPCODE::ADD),
          adder->Rtype(), cntr->New_ild(sharding_i, spos), adder, spos),
      spos);
  return sharding_update;
}

// data and node: seperate. Actually node is essential to sharding.
void SHARDING_MAP::Set_data_sharding(FUNC_ID fid, PREG_ID did,
                                     const ARRAY_SHARDING& sh) {
  _smap_preg[fid][did] = sh;
}

void SHARDING_MAP::Set_data_sharding(FUNC_ID fid, ADDR_DATUM_ID did,
                                     const ARRAY_SHARDING& sh) {
  _smap_data[fid][did] = sh;
}

const ARRAY_SHARDING* SHARDING_MAP::Get_data_sharding(FUNC_ID fid,
                                                      PREG_ID did) const {
  auto it1 = _smap_preg.find(fid);
  if (it1 != _smap_preg.end()) {
    auto it2 = it1->second.find(did);
    if (it2 != it1->second.end()) {
      return &(it2->second);
    }
  }
  return nullptr;
}

const ARRAY_SHARDING* SHARDING_MAP::Get_data_sharding(FUNC_ID       fid,
                                                      ADDR_DATUM_ID did) const {
  auto it1 = _smap_data.find(fid);
  if (it1 != _smap_data.end()) {
    auto it2 = it1->second.find(did);
    if (it2 != it1->second.end()) {
      return &(it2->second);
    }
  }
  return nullptr;
}

//! Get the sharding strategy of conv. N-C-H-W
//  Keep data contiguous in W dimension.
//  Assuming conv stride is not inplace!
std::vector<int64_t> SHARDING_MAP::Analyze_conv(int64_t slots,
                                                int64_t channel_out,
                                                int64_t channel_in,
                                                int64_t height, int64_t width) {
  int x = 1, y = 1, z = 1;
  // low dim to high dim
  int spatial_size = height * width;
  if (spatial_size >= slots) {
    z = (spatial_size % slots == 0) ? (spatial_size / slots)
                                    : (spatial_size / slots + 1);
    AIR_ASSERT(height % z == 0);
    y = channel_in;
    x = channel_out;
  } else {
    int input_size = channel_in * spatial_size;
    y              = (input_size % slots == 0) ? (input_size / slots)
                                               : (input_size / slots + 1);
    while (channel_in % y != 0) y++;
    int output_size = channel_out * spatial_size;
    x               = (output_size % slots == 0) ? (output_size / slots)
                                                 : (output_size / slots + 1);
    while (channel_out % x != 0) x++;
  }

  AIR_ASSERT(x * y * z > 1);
  return {x, y, z};
}

void SHARDING_MAP::Print(std::ostream& os) const {
  // TODO: trace
  os << "SHARDING_MAP smap_node:  size=" << _smap_node.size() << std::endl;
  os << "SHARDING_MAP smap_data:  size=" << _smap_data.size() << std::endl;
}

// sharding tensor/array: num blocks of block_shape
// It describes the distributions of the input/output tensors across computation
// mesh.
TYPE_PTR
SHARDING_MAP::New_sharding_type(const std::string& ty_name,
                                uint32_t ty_name_suffix, int64_t num,
                                CONST_TYPE_PTR block_type, const SPOS& spos) {
  std::vector<int64_t> num_shape{num};

  TYPE_PTR sharding_type =
      New_array_type(_gscope, "Tsharding_" + ty_name, ty_name_suffix,
                     block_type, num_shape, spos);
  return sharding_type;
}

// New sharding var: name[num_blocks][BLOCK_TYPE]
ADDR_DATUM_PTR New_sharding_var(CONTAINER* cntr, const std::string& name,
                                uint32_t suffix, int64_t num_blocks,
                                std::vector<int64_t> block_shape,
                                CONST_TYPE_PTR etype, const SPOS& spos) {
  GLOB_SCOPE* gscope = cntr->Glob_scope();
  FUNC_SCOPE* fscope = cntr->Parent_func_scope();

  TYPE_PTR block_type =
      New_array_type(gscope, "TB_" + name, suffix, etype, block_shape, spos);

  std::vector<int64_t> num_shape{num_blocks};
  TYPE_PTR             sharding_type =
      New_array_type(gscope, "TS_" + name, suffix, block_type, num_shape, spos);

  std::string sharding_str = std::string("VS_") + name + std::to_string(suffix);
  ADDR_DATUM_PTR sharding_var =
      fscope->New_var(sharding_type, sharding_str.c_str(), spos);
  return sharding_var;
}

void SHARDING_MAP::Print_sharding_type(TYPE_PTR shard_type) {
  std::vector<int64_t> shape = shard_type->Cast_to_arr()->Shape();
  std::cout << "Print_sharding_array_type:  size=" << shape.size() << std::endl;
}

// Simple decision by H dim. Never handle H+W
void Compute_conv_halo(ARRAY_SHARDING& sh, int64_t kh, int64_t kw) {
  std::vector<int64_t> pspec = sh.Spec();
  if (pspec[2] > 1) {
    AIR_ASSERT_MSG(pspec[2] == 2, "TODO: >2 H dimension partitioning");
    sh.Set_halosize(kh / 2);
  }
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

// Infer conv output type by old node attr.
TYPE_PTR Infer_conv(CONTAINER* cntr, TYPE_PTR input_type, NODE_PTR node,
                    const SPOS& spos) {
  std::vector<int64_t> input_shape = input_type->Cast_to_arr()->Shape();
  GLOB_SCOPE*          gscope      = cntr->Glob_scope();

  int64_t          h       = input_shape[2];
  int64_t          w       = input_shape[3];
  std::vector<int> strides = Get_attr_data<int>(node, core::ATTR::STRIDE);
  std::vector<int> pads    = Get_attr_data<int>(node, core::ATTR::PAD);
  std::vector<int> ks      = Get_attr_data<int>(node, core::ATTR::KSHAPE);

  int64_t oh = (h - ks[0] + 2 * pads[0]) / strides[0] + 1;
  int64_t ow = (w - ks[0] + 2 * pads[0]) / strides[1] + 1;
  if ((oh == h) && (ow == w))
    return input_type;
  else {
    TYPE_PTR             etype = input_type->Cast_to_arr()->Elem_type();
    std::vector<int64_t> output_shape{input_shape[0], input_shape[1], oh, ow};
    TYPE_PTR             output_type =
        New_array_type(gscope, "TB_spatial_output", etype, output_shape, spos);
    return output_type;
  }
}

// concat as gather
NODE_PTR New_gather_node(CONTAINER* cntr, NODE_PTR input0, NODE_PTR input1,
                         TYPE_PTR rtype, int dim, const SPOS& spos) {
  NODE_PTR concat_node = cntr->New_cust_node(
      air::base::OPCODE(nn::core::NN, nn::core::OPCODE::CONCAT), rtype, spos);
  concat_node->Set_child(0, input0);
  concat_node->Set_child(1, input1);
  int axis[1] = {dim};  // H dim
  concat_node->Set_attr(nn::core::ATTR::AXIS, axis, 1);
  return concat_node;
}

// [0, upper): TODO: "High-level" loop with Elementwise/Reduce
STMT_PTR T2TSHARDING_CTX::New_mesh_loop(CONTAINER* cntr, const char* index_str,
                                        int upper, const SPOS& spos) {
  GLOB_SCOPE*    gscope   = cntr->Glob_scope();
  FUNC_SCOPE*    fscope   = cntr->Parent_func_scope();
  CONST_TYPE_PTR s32_type = gscope->Prim_type(PRIMITIVE_TYPE::INT_S32);

  std::string new_index_str(index_str);
  new_index_str += (std::string("_n") + std::to_string(Get_num_vloop()));
  STR_PTR index_name = gscope->New_str(new_index_str.c_str());

  // Generate a NULL loop
  ADDR_DATUM_PTR index_var  = fscope->New_var(s32_type, index_name, spos);
  NODE_PTR       index_node = cntr->New_ld(index_var, spos);

  NODE_PTR init_node  = cntr->New_intconst(s32_type, 0, spos);
  NODE_PTR upper_node = cntr->New_intconst(s32_type, upper, spos);

  NODE_PTR cond_node = cntr->New_bin_arith(air::core::OPCODE::LT, s32_type,
                                           index_node, upper_node, spos);
  NODE_PTR incr_node =
      cntr->New_bin_arith(air::core::OPCODE::ADD, s32_type, index_node,
                          cntr->New_intconst(s32_type, 1, spos), spos);

  NODE_PTR loop_body = cntr->New_stmt_block(spos);
  STMT_PTR loop_stmt = cntr->New_do_loop(index_var, init_node, cond_node,
                                         incr_node, loop_body, spos);

  AIR_ASSERT_MSG(loop_stmt->Node()->Child(3)->Is_block(),
                 "Loop body is not a block node!");
  return loop_stmt;
}

// Loop insertpoint
STMT_LIST Get_loop_sl(STMT_PTR loop) {
  return STMT_LIST::Enclosing_list(loop->Node()->Child(3)->End_stmt());
}

// Gen add body for a single sharding
// output_sharding[xiv][ziv] = add(input0[xiv][ziv], input1[xiv][ziv])
STMT_PTR New_add_sharding_body(CONTAINER* cntr, NODE_PTR node,
                               ADDR_DATUM_PTR input0_sharding_var,
                               ADDR_DATUM_PTR input1_sharding_var,
                               ADDR_DATUM_PTR output_sharding_var, NODE_PTR xiv,
                               NODE_PTR ziv, std::vector<int64_t> mesh,
                               TYPE_PTR output_block_type, const SPOS& spos) {
  TYPE_PTR type = output_sharding_var->Type();
  AIR_ASSERT(mesh.size() == 3 && mesh[0] == mesh[1]);
  AIR_ASSERT(type->Is_array() &&
             type->Is_compatible_type(input0_sharding_var->Type()) &&
             type->Is_compatible_type(input1_sharding_var->Type()));
  AIR_ASSERT(type->Cast_to_arr()->Elem_count() == mesh[1] * mesh[2]);
  CONST_TYPE_PTR s32t = cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_S32);

  // index = xiv*z + ziv
  NODE_PTR input0_indexer = New_muladd_s32(
      cntr, xiv, cntr->New_intconst(s32t, mesh[2], spos), ziv, spos);
  NODE_PTR input1_indexer = New_muladd_s32(
      cntr, xiv, cntr->New_intconst(s32t, mesh[2], spos), ziv, spos);
  NODE_PTR output_indexer = New_muladd_s32(
      cntr, xiv, cntr->New_intconst(s32t, mesh[2], spos), ziv, spos);

  NODE_PTR new_node =
      cntr->New_cust_node(nn::core::OPC_ADD, output_block_type, spos);
  new_node->Set_child(
      0, New_sharding_loader(cntr, input0_sharding_var, input0_indexer, spos));
  new_node->Set_child(
      1, New_sharding_loader(cntr, input1_sharding_var, input1_indexer, spos));
  new_node->Copy_attr(node);

  STMT_PTR new_stmt = New_sharding_update_store(cntr, output_sharding_var,
                                                output_indexer, new_node, spos);
  return new_stmt;
}

// Gen sharding body for concat
//   output_sharding[idx+iv] = input_sharding[iv]
// }
STMT_PTR New_concat_sharding_body(CONTAINER* cntr, uint64_t ofst, NODE_PTR iv,
                                  ADDR_DATUM_PTR input_sharding_var,
                                  ADDR_DATUM_PTR output_sharding_var,
                                  const SPOS&    spos) {
  CONST_TYPE_PTR s32t = cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_S32);
  NODE_PTR       lhs_idx = cntr->New_bin_arith(
      air::core::OPCODE::ADD, s32t, cntr->Clone_node_tree(iv),
      cntr->New_intconst(s32t, ofst, spos), spos);

  NODE_PTR rhs = New_sharding_loader(cntr, input_sharding_var, iv, spos);
  STMT_PTR new_stmt =
      New_sharding_store(cntr, output_sharding_var, lhs_idx, rhs, spos);
  return new_stmt;
}

// Gen average_pool body for a single sharding
// output_sharding[yiv][ziv] += average_pool(input[yiv][ziv])
STMT_PTR New_average_pool_sharding_body(CONTAINER* cntr, NODE_PTR node,
                                        ADDR_DATUM_PTR input_sharding_var,
                                        ADDR_DATUM_PTR output_sharding_var,
                                        NODE_PTR yiv, NODE_PTR ziv,
                                        std::vector<int64_t> mesh,
                                        TYPE_PTR             output_block_type,
                                        int64_t halosize, const SPOS& spos) {
  TYPE_PTR type = output_sharding_var->Type();
  AIR_ASSERT(mesh.size() == 3 && mesh[0] == mesh[1]);
  AIR_ASSERT(type->Is_array() && input_sharding_var->Type()->Is_array());
  AIR_ASSERT(type->Cast_to_arr()->Elem_count() == mesh[1] * mesh[2]);
  AIR_ASSERT(input_sharding_var->Type()->Cast_to_arr()->Elem_count() ==
             mesh[1] * mesh[2]);
  CONST_TYPE_PTR s32t = cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_S32);

  // index = yiv*z + ziv
  NODE_PTR input_indexer = New_muladd_s32(
      cntr, yiv, cntr->New_intconst(s32t, mesh[2], spos), ziv, spos);
  NODE_PTR output_indexer = New_muladd_s32(
      cntr, yiv, cntr->New_intconst(s32t, mesh[2], spos), ziv, spos);

  NODE_PTR new_node =
      cntr->New_cust_node(nn::core::OPC_AVERAGE_POOL, output_block_type, spos);
  new_node->Set_child(
      0, New_sharding_loader(cntr, input_sharding_var, input_indexer, spos));
  new_node->Copy_attr(node);

  STMT_PTR new_stmt = New_sharding_store(cntr, output_sharding_var,
                                         output_indexer, new_node, spos);

  return new_stmt;
}

// Body for sharding with spatial partitioning
STMT_PTR New_conv_sharding_body_spatial(CONTAINER* cntr, NODE_PTR node,
                                        NODE_PTR       new_input,
                                        CONSTANT_PTR   weight_sharding_const,
                                        CONSTANT_PTR   bias_sharding_const,
                                        ADDR_DATUM_PTR output_sharding_var,
                                        NODE_PTR xiv, NODE_PTR yiv,
                                        NODE_PTR ziv, std::vector<int64_t> mesh,
                                        int64_t halosize, const SPOS& spos) {
  AIR_ASSERT_MSG(mesh.size() == 3, "mesh.size=3 for conv-spatial-partition");
  CONST_TYPE_PTR s32t = cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_S32);

  // y(C) input/weight
  // ^
  // | z(H) input/output
  // |/
  //  -----> x (F) output/weight/bias

  std::vector<int> groups = Get_attr_data<int>(node, core::ATTR::GROUP);
  int              group  = groups[0];

  // for grouped conv: yiv is always 0, y=mesh[1]=1.
  // So the indexer works for both conv and group_conv.
  // For group_conv: indexer is xiv :)
  // compute indexer of weight[x*y] for each mesh unit
  // index map: [x*y] -> [x*y*z]: weight sharding indexer= xiv*y + yiv
  NODE_PTR weight_indexer = New_muladd_s32(
      cntr, xiv, cntr->New_intconst(s32t, mesh[1], spos), yiv, spos);

  // for grouped conv: C is partitipated as F.
  // input indexer also uses xiv.
  // compute indexer of input[y*z] for each mesh unit
  // index map: [y*z] -> [x*y*z]: input sharding indexer= yiv*z + ziv
  if (group > 1) yiv = xiv;
  NODE_PTR input_indexer = New_muladd_s32(
      cntr, yiv, cntr->New_intconst(s32t, mesh[2], spos), ziv, spos);

  // block conv output type = input_block_type
  TYPE_PTR rtype;
  NODE_PTR new_ld;
  if (new_input->Opcode() == air::core::OPC_LD) {
    AIR_ASSERT(new_input->Rtype()->Is_array());
    rtype = new_input->Rtype()->Cast_to_arr()->Elem_type();
    new_ld =
        New_sharding_loader(cntr, new_input->Addr_datum(), input_indexer, spos);
  } else {
    rtype  = new_input->Rtype();
    new_ld = cntr->Clone_node_tree(new_input);
  }

  NODE_PTR pconv_node = New_conv_node(
      cntr, new_ld,
      New_sharding_loader(cntr, weight_sharding_const, weight_indexer, spos),
      cntr->New_ldc(bias_sharding_const, spos), rtype, spos);
  pconv_node->Copy_attr(node);

  if (group > 1) {
    // adjust GROUP attr to the channel_in of block.
    std::vector<int64_t> bshape    = rtype->Cast_to_arr()->Shape();
    std::vector<int>     new_group = {(int)bshape[1]};
    pconv_node->Set_attr(core::ATTR::GROUP, new_group.data(), new_group.size());
  }
  std::vector<int> orig_group = {group};
  pconv_node->Set_attr(core::ATTR::ORIG_GROUP, orig_group.data(),
                       orig_group.size());

  // by input block type: if stride>1, set stride=1 for new_conv.
  // Note: we cannot compute stride-conv once. so conbine halo-slice and
  // stride-slice
  std::vector<int> strides  = Get_attr_data<int>(node, core::ATTR::STRIDE);
  std::vector<int> stride11 = {1, 1};
  pconv_node->Set_attr(core::ATTR::STRIDE, stride11.data(), stride11.size());
  pconv_node->Set_attr(core::ATTR::ORIG_STRIDE, strides.data(), strides.size());

  // compute indexer of output[y*z] for each mesh unit
  // index map: [x*z] -> [x*y*z]: output sharding indexer= xiv*z + ziv
  NODE_PTR output_indexer = New_muladd_s32(
      cntr, xiv, cntr->New_intconst(s32t, mesh[2], spos), ziv, spos);
  STMT_PTR conv_add_output_sharding = New_sharding_update_store(
      cntr, output_sharding_var, output_indexer, pconv_node, spos);
  return conv_add_output_sharding;
}

// Gen global_avgpool body for a single sharding
// output_sharding[yiv][ziv] = global_avgpool(input[yiv][ziv])
STMT_PTR New_global_avgpool_sharding_body(
    CONTAINER* cntr, NODE_PTR node, ADDR_DATUM_PTR input_sharding_var,
    ADDR_DATUM_PTR output_sharding_var, NODE_PTR yiv, NODE_PTR ziv,
    std::vector<int64_t> mesh, TYPE_PTR output_block_type, const SPOS& spos) {
  TYPE_PTR type = output_sharding_var->Type();
  AIR_ASSERT(mesh.size() == 3 && mesh[0] == mesh[1]);
  AIR_ASSERT(type->Is_array() && input_sharding_var->Type()->Is_array());
  AIR_ASSERT(type->Cast_to_arr()->Elem_count() == mesh[1] * mesh[2]);
  CONST_TYPE_PTR s32t = cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_S32);

  // index = yiv*z + ziv
  NODE_PTR input_indexer = New_muladd_s32(
      cntr, yiv, cntr->New_intconst(s32t, mesh[2], spos), ziv, spos);
  NODE_PTR output_indexer = New_muladd_s32(
      cntr, yiv, cntr->New_intconst(s32t, mesh[2], spos), ziv, spos);

  NODE_PTR new_node = cntr->New_cust_node(nn::core::OPC_GLOBAL_AVERAGE_POOL,
                                          output_block_type, spos);
  new_node->Set_child(
      0, New_sharding_loader(cntr, input_sharding_var, input_indexer, spos));
  new_node->Copy_attr(node);

  STMT_PTR new_stmt = New_sharding_store(cntr, output_sharding_var,
                                         output_indexer, new_node, spos);

  return new_stmt;
}

// Gen relu body for a single sharding
// output_sharding[xiv][ziv] = relu(input[xiv][ziv])
STMT_PTR New_relu_sharding_body(CONTAINER* cntr, NODE_PTR node,
                                ADDR_DATUM_PTR input_sharding_var,
                                ADDR_DATUM_PTR output_sharding_var,
                                NODE_PTR xiv, NODE_PTR ziv,
                                std::vector<int64_t> mesh,
                                TYPE_PTR output_block_type, const SPOS& spos) {
  TYPE_PTR type = output_sharding_var->Type();
  AIR_ASSERT(mesh.size() == 3 && mesh[0] == mesh[1]);
  AIR_ASSERT(type->Is_array() &&
             type->Is_compatible_type(input_sharding_var->Type()));
  AIR_ASSERT(type->Cast_to_arr()->Elem_count() == mesh[1] * mesh[2]);
  CONST_TYPE_PTR s32t = cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_S32);

  // index = xiv*z + ziv
  NODE_PTR input_indexer = New_muladd_s32(
      cntr, xiv, cntr->New_intconst(s32t, mesh[2], spos), ziv, spos);
  NODE_PTR output_indexer = New_muladd_s32(
      cntr, xiv, cntr->New_intconst(s32t, mesh[2], spos), ziv, spos);

  NODE_PTR new_node =
      cntr->New_cust_node(nn::core::OPC_RELU, output_block_type, spos);
  new_node->Set_child(
      0, New_sharding_loader(cntr, input_sharding_var, input_indexer, spos));
  new_node->Copy_attr(node);

  STMT_PTR new_stmt = New_sharding_update_store(cntr, output_sharding_var,
                                                output_indexer, new_node, spos);
  return new_stmt;
}

// Abstraction for sharding: eltwise, reduce. It is determined by op+ss.
// output = Eltwise(op(input shard)) in all axes.
// TODO: body_lambda

void T2TSHARDING_CTX::New_eshard_slice(ADDR_DATUM_PTR   input,
                                       ADDR_DATUM_PTR   output,
                                       std::vector<int> strides,
                                       int64_t halosize, const SPOS& spos) {
  // TODO: mesh consistency check by SHARD_DATA?
  int64_t num_shards = Get_shard_size(input);
  AIR_ASSERT_MSG(num_shards == Get_shard_size(output),
                 "input and output have the same shards");
  Trace(TF_SHARDING, "eshard_slice num_shard=", num_shards, "\n");

  CONTAINER* cntr    = Get_cntr();
  TYPE_PTR   ib_type = Get_block_type(input);

  STMT_PTR  eloop = New_mesh_loop(cntr, "siv", num_shards, spos);
  STMT_LIST esl   = Get_loop_sl(eloop);
  NODE_PTR  siv   = cntr->New_ld(eloop->Node()->Iv(), spos);

  // Handle neighbors's overlapping region (H dim).
  // Note: for spatial partitioning, the first two dims must be 1.
  // [1, 1, HB+halo, W], [1, 1, halo+HB, W]
  std::vector<int64_t> bshape = ib_type->Cast_to_arr()->Shape();
  if (halosize > 1)
    AIR_ASSERT_MSG((bshape[0] == 1) && (bshape[1] == 1),
                   "sharding_body_spatial block_shape[1][1]");
  std::vector<int64_t> start_indices = {halosize, 0};
  std::vector<int64_t> slice_size    = {bshape[2] - 2 * halosize, bshape[3]};
  std::vector<int64_t> stride_size   = {strides[0], strides[1]};

  NODE_PTR halo_node =
      New_halo_node(cntr, New_sharding_loader(cntr, input, siv, spos),
                    start_indices, slice_size, stride_size, bshape[1], spos);
  STMT_PTR st = New_sharding_store(cntr, output, siv, halo_node, spos);

  esl.Append(st);
  Prepend(eloop);
}

// TODO: reduce axis for more cases; attr as params.
void T2TSHARDING_CTX::New_rshard_conv2d(NODE_PTR node, NODE_PTR new_input,
                                        CONSTANT_PTR weight, CONSTANT_PTR bias,
                                        ADDR_DATUM_PTR       output,
                                        std::vector<int64_t> mesh,
                                        int64_t halosize, const SPOS& spos) {
  AIR_ASSERT_MSG(mesh.size() == 3, "mesh is dim3");
  int64_t x = mesh[0], y = mesh[1], z = mesh[2];
  Trace(TF_SHARDING, "reduce_shard conv2d  mesh=[", x, ", ", y, ", ", z, "]\n");

  CONTAINER* cntr = Get_cntr();

  std::vector<STMT_PTR> rloops = New_ranged_loop(cntr, {y, z, x}, spos);

  NODE_PTR yiv = cntr->New_ld(rloops[0]->Node()->Iv(), spos);
  NODE_PTR ziv = cntr->New_ld(rloops[1]->Node()->Iv(), spos);
  NODE_PTR xiv = cntr->New_ld(rloops[2]->Node()->Iv(), spos);

  STMT_PTR rbody = New_conv_sharding_body_spatial(cntr, node, new_input, weight,
                                                  bias, output, xiv, yiv, ziv,
                                                  {x, y, z}, halosize, spos);

  STMT_LIST inner_sl = Get_loop_sl(rloops[rloops.size() - 1]);
  inner_sl.Append(rbody);
  Prepend(rloops[0]);
}

// Build ND nested loop for [0, mesh_range): do-loop
std::vector<STMT_PTR> T2TSHARDING_CTX::New_ranged_loop(
    CONTAINER* cntr, std::vector<int64_t> range, const SPOS& spos) {
  int level = range.size();
  AIR_ASSERT_MSG(level <= 3, "sharding loop range <= 3");
  int64_t num_iter = std::accumulate(range.begin(), range.end(), 1,
                                     std::multiplies<int64_t>());
  AIR_ASSERT_MSG(num_iter >= 1, "all range values >= 1");

  const char* ivs_str[3] = {"mesh_y", "mesh_z", "mesh_x"};

  std::vector<STMT_PTR> loops;
  for (int i = 0; i < level; i++) {
    loops.push_back(New_mesh_loop(cntr, ivs_str[i], range[i], spos));
  }

  // link loop
  for (int i = 0; i < level - 1; i++) {
    STMT_LIST loop_sl =
        STMT_LIST::Enclosing_list(loops[i]->Node()->Child(3)->End_stmt());
    loop_sl.Append(loops[i + 1]);
  }

  return loops;
}

// a*b+c node
NODE_PTR New_muladd_s32(CONTAINER* cntr, NODE_PTR a, NODE_PTR b, NODE_PTR c,
                        const SPOS& spos) {
  CONST_TYPE_PTR s32_type =
      cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_S32);

  NODE_PTR muladd_node = cntr->New_bin_arith(
      air::core::OPCODE::ADD, s32_type,
      cntr->New_bin_arith(air::core::OPCODE::MUL, s32_type, a, b, spos), c,
      spos);
  return muladd_node;
}

// Actually halo support is target dependent.
NODE_PTR New_halo_node(CONTAINER* cntr, NODE_PTR op0,
                       std::vector<int64_t> start_indices,
                       std::vector<int64_t> slice_size,
                       std::vector<int64_t> stride_size, int channel,
                       const SPOS& spos) {
  VECTOR_GEN vgen(cntr);
  NODE_PTR   halo_node = vgen.New_strided_slice(op0, start_indices, slice_size,
                                                stride_size, channel, spos);
  return halo_node;
}

// Reshard shard_var to a reshard_var with new mesh
// [num_reshard][reshard_block_type].
// If num_reshard==1: simple.
// Actually any op use the new reshard_var should check the mesh consistency.
ADDR_DATUM_PTR T2TSHARDING_CTX::Reshard(
    ADDR_DATUM_PTR shard_var, int num_reshard, int numtoone,
    std::vector<int64_t> reshard_block_shape, std::vector<int64_t> xyz,
    const SPOS& spos) {
  ARRAY_TYPE_PTR shard_type = shard_var->Type()->Cast_to_arr();
  AIR_ASSERT(shard_type->Elem_type()->Is_array());
  ARRAY_TYPE_PTR shard_block_type = shard_type->Elem_type()->Cast_to_arr();
  // Assert total numel
  int64_t reshard_block_numel =
      std::accumulate(reshard_block_shape.begin(), reshard_block_shape.end(), 1,
                      std::multiplies<int64_t>());
  AIR_ASSERT(shard_type->Elem_count() * shard_block_type->Elem_count() ==
             num_reshard * reshard_block_numel);
  //  Assert block numel.
  AIR_ASSERT(numtoone * shard_block_type->Elem_count() == reshard_block_numel);

  CONTAINER*  cntr   = Get_cntr();
  GLOB_SCOPE* gscope = cntr->Glob_scope();
  FUNC_SCOPE* fscope = cntr->Parent_func_scope();

  TYPE_PTR etype              = shard_block_type->Elem_type();
  TYPE_PTR reshard_block_type = New_array_type(
      gscope, "TB_reshard", Get_num_vloop(), etype, reshard_block_shape, spos);

  // z=2 & numtoone=2: concat(H)
  // z=2 & numtoone=4: concat(H) then concat(C)
  // z=2 & numtoone=16: concat(H) then concat(C)...
  // z=1 & numtoone=2: concat(C)
  // z=1 & numtoone=4: concat(C) concat(C)
  Trace(TF_SHARDING, "Reshard: z=", xyz[2], " num_reshard=", num_reshard,
        " numtoone=", numtoone, "\n");
  std::string reshard_str =
      std::string("output_reshard") + std::to_string(Get_num_vloop());
  CONST_TYPE_PTR s32t = cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_S32);

  // the code needs to be reorgnized!
  // abstract multiple gather into a function.
  if (num_reshard == 1) {
    ADDR_DATUM_PTR reshard_var =
        fscope->New_var(reshard_block_type, reshard_str.c_str(), spos);

    if (xyz[2] == 2) {
      if (numtoone == 2) {
        STMT_PTR  reshard1_loop = New_mesh_loop(cntr, "reshard1", 1, spos);
        STMT_LIST reshard2_sl   = STMT_LIST::Enclosing_list(
            reshard1_loop->Node()->Child(3)->End_stmt());
        NODE_PTR riv = cntr->New_ld(reshard1_loop->Node()->Iv(), spos);
        NODE_PTR riv_add1 =
            cntr->New_bin_arith(air::core::OPCODE::ADD, s32t, riv,
                                cntr->New_intconst(s32t, 1, spos), spos);
        NODE_PTR gather_node = New_gather_node(
            cntr, New_sharding_loader(cntr, shard_var, riv, spos),
            New_sharding_loader(cntr, shard_var, riv_add1, spos),
            reshard_var->Type(), 2, spos);
        STMT_PTR new_st_gather = cntr->New_st(gather_node, reshard_var, spos);
        reshard2_sl.Append(new_st_gather);
        Prepend(reshard1_loop);

      } else if (numtoone == 4) {
        std::vector<int64_t> reshard_block2_shape = {
            reshard_block_shape[0], reshard_block_shape[1] / 2,
            reshard_block_shape[2], reshard_block_shape[3]};
        TYPE_PTR reshard_block2_type =
            New_array_type(gscope, "TB_reshard2", Get_num_vloop(), etype,
                           reshard_block2_shape, spos);
        STMT_PTR  reshard1_loop = New_mesh_loop(cntr, "reshard1", 1, spos);
        STMT_LIST reshard2_sl   = STMT_LIST::Enclosing_list(
            reshard1_loop->Node()->Child(3)->End_stmt());
        NODE_PTR riv = cntr->New_ld(reshard1_loop->Node()->Iv(), spos);
        NODE_PTR riv_add1 =
            cntr->New_bin_arith(air::core::OPCODE::ADD, s32t, riv,
                                cntr->New_intconst(s32t, 1, spos), spos);
        NODE_PTR riv_add2 =
            cntr->New_bin_arith(air::core::OPCODE::ADD, s32t, riv,
                                cntr->New_intconst(s32t, 2, spos), spos);
        NODE_PTR riv_add3 =
            cntr->New_bin_arith(air::core::OPCODE::ADD, s32t, riv,
                                cntr->New_intconst(s32t, 3, spos), spos);

        NODE_PTR gather_node1 = New_gather_node(
            cntr, New_sharding_loader(cntr, shard_var, riv, spos),
            New_sharding_loader(cntr, shard_var, riv_add1, spos),
            reshard_block2_type, 2, spos);
        NODE_PTR gather_node2 = New_gather_node(
            cntr, New_sharding_loader(cntr, shard_var, riv_add2, spos),
            New_sharding_loader(cntr, shard_var, riv_add3, spos),
            reshard_block2_type, 2, spos);

        NODE_PTR gather_node = New_gather_node(cntr, gather_node1, gather_node2,
                                               reshard_var->Type(), 1, spos);
        STMT_PTR new_st_gather = cntr->New_st(gather_node, reshard_var, spos);
        reshard2_sl.Append(new_st_gather);
        Prepend(reshard1_loop);
      } else
        AIR_ASSERT_MSG(0, "num_reshard == 1 case error")
    } else if (numtoone == 4) {
      // all in axis 1!
      AIR_ASSERT_MSG(numtoone == 4, "z==1 case error :)   numtoone == 4");
      std::vector<int64_t> reshard_block2_shape = {
          reshard_block_shape[0], reshard_block_shape[1] / 2,
          reshard_block_shape[2], reshard_block_shape[3]};
      TYPE_PTR reshard_block2_type =
          New_array_type(gscope, "TB_reshard2", Get_num_vloop(), etype,
                         reshard_block2_shape, spos);
      STMT_PTR  reshard1_loop = New_mesh_loop(cntr, "reshard1", 1, spos);
      STMT_LIST reshard2_sl   = STMT_LIST::Enclosing_list(
          reshard1_loop->Node()->Child(3)->End_stmt());
      NODE_PTR riv = cntr->New_ld(reshard1_loop->Node()->Iv(), spos);
      NODE_PTR riv_add1 =
          cntr->New_bin_arith(air::core::OPCODE::ADD, s32t, riv,
                              cntr->New_intconst(s32t, 1, spos), spos);
      NODE_PTR riv_add2 =
          cntr->New_bin_arith(air::core::OPCODE::ADD, s32t, riv,
                              cntr->New_intconst(s32t, 2, spos), spos);
      NODE_PTR riv_add3 =
          cntr->New_bin_arith(air::core::OPCODE::ADD, s32t, riv,
                              cntr->New_intconst(s32t, 3, spos), spos);

      NODE_PTR gather_node1 =
          New_gather_node(cntr, New_sharding_loader(cntr, shard_var, riv, spos),
                          New_sharding_loader(cntr, shard_var, riv_add1, spos),
                          reshard_block2_type, 1, spos);
      NODE_PTR gather_node2 = New_gather_node(
          cntr, New_sharding_loader(cntr, shard_var, riv_add2, spos),
          New_sharding_loader(cntr, shard_var, riv_add3, spos),
          reshard_block2_type, 1, spos);

      NODE_PTR gather_node   = New_gather_node(cntr, gather_node1, gather_node2,
                                               reshard_var->Type(), 1, spos);
      STMT_PTR new_st_gather = cntr->New_st(gather_node, reshard_var, spos);
      reshard2_sl.Append(new_st_gather);
      Prepend(reshard1_loop);
    } else if (numtoone == 2) {
      STMT_PTR  reshard1_loop = New_mesh_loop(cntr, "reshard1", 1, spos);
      STMT_LIST reshard2_sl   = STMT_LIST::Enclosing_list(
          reshard1_loop->Node()->Child(3)->End_stmt());
      NODE_PTR riv = cntr->New_ld(reshard1_loop->Node()->Iv(), spos);
      NODE_PTR riv_add1 =
          cntr->New_bin_arith(air::core::OPCODE::ADD, s32t, riv,
                              cntr->New_intconst(s32t, 1, spos), spos);
      NODE_PTR gather_node =
          New_gather_node(cntr, New_sharding_loader(cntr, shard_var, riv, spos),
                          New_sharding_loader(cntr, shard_var, riv_add1, spos),
                          reshard_var->Type(), 1, spos);
      STMT_PTR new_st_gather = cntr->New_st(gather_node, reshard_var, spos);
      reshard2_sl.Append(new_st_gather);
      Prepend(reshard1_loop);
    }
    return reshard_var;
  } else {
    // New sharding_var2
    // numtoone == 4
    TYPE_PTR reshard_type =
        New_array_type(gscope, "TS_reshard", Get_num_vloop(),
                       reshard_block_type, {num_reshard}, spos);
    ADDR_DATUM_PTR reshard_var =
        fscope->New_var(reshard_type, reshard_str.c_str(), spos);

    if (xyz[2] == 2) {
      // 16 shoule be changed to stride * stride
      if (numtoone == 16) {
        std::vector<int64_t> reshard_block2_shape = {
            reshard_block_shape[0], reshard_block_shape[1] / 2,
            reshard_block_shape[2], reshard_block_shape[3]};
        TYPE_PTR reshard_block2_type =
            New_array_type(gscope, "TB_reshard2", Get_num_vloop(), etype,
                           reshard_block2_shape, spos);
        STMT_PTR reshardm_loop =
            New_mesh_loop(cntr, "reshardm", num_reshard, spos);
        STMT_LIST reshardm_sl = STMT_LIST::Enclosing_list(
            reshardm_loop->Node()->Child(3)->End_stmt());
        NODE_PTR liv = cntr->New_ld(reshardm_loop->Node()->Iv(), spos);
        NODE_PTR riv =
            cntr->New_bin_arith(air::core::OPCODE::MUL, s32t,
                                cntr->New_ld(reshardm_loop->Node()->Iv(), spos),
                                cntr->New_intconst(s32t, numtoone, spos), spos);
        NODE_PTR riv_add1 =
            cntr->New_bin_arith(air::core::OPCODE::ADD, s32t, riv,
                                cntr->New_intconst(s32t, 1, spos), spos);
        NODE_PTR riv_add2 =
            cntr->New_bin_arith(air::core::OPCODE::ADD, s32t, riv,
                                cntr->New_intconst(s32t, 2, spos), spos);
        NODE_PTR riv_add3 =
            cntr->New_bin_arith(air::core::OPCODE::ADD, s32t, riv,
                                cntr->New_intconst(s32t, 3, spos), spos);

        NODE_PTR riv_add4 =
            cntr->New_bin_arith(air::core::OPCODE::ADD, s32t, riv,
                                cntr->New_intconst(s32t, 4, spos), spos);
        NODE_PTR riv_add5 =
            cntr->New_bin_arith(air::core::OPCODE::ADD, s32t, riv,
                                cntr->New_intconst(s32t, 5, spos), spos);
        NODE_PTR riv_add6 =
            cntr->New_bin_arith(air::core::OPCODE::ADD, s32t, riv,
                                cntr->New_intconst(s32t, 6, spos), spos);

        NODE_PTR riv_add7 =
            cntr->New_bin_arith(air::core::OPCODE::ADD, s32t, riv,
                                cntr->New_intconst(s32t, 7, spos), spos);
        NODE_PTR riv_add8 =
            cntr->New_bin_arith(air::core::OPCODE::ADD, s32t, riv,
                                cntr->New_intconst(s32t, 8, spos), spos);
        NODE_PTR riv_add9 =
            cntr->New_bin_arith(air::core::OPCODE::ADD, s32t, riv,
                                cntr->New_intconst(s32t, 9, spos), spos);

        NODE_PTR riv_add10 =
            cntr->New_bin_arith(air::core::OPCODE::ADD, s32t, riv,
                                cntr->New_intconst(s32t, 10, spos), spos);
        NODE_PTR riv_add11 =
            cntr->New_bin_arith(air::core::OPCODE::ADD, s32t, riv,
                                cntr->New_intconst(s32t, 11, spos), spos);
        NODE_PTR riv_add12 =
            cntr->New_bin_arith(air::core::OPCODE::ADD, s32t, riv,
                                cntr->New_intconst(s32t, 12, spos), spos);

        NODE_PTR riv_add13 =
            cntr->New_bin_arith(air::core::OPCODE::ADD, s32t, riv,
                                cntr->New_intconst(s32t, 13, spos), spos);
        NODE_PTR riv_add14 =
            cntr->New_bin_arith(air::core::OPCODE::ADD, s32t, riv,
                                cntr->New_intconst(s32t, 14, spos), spos);
        NODE_PTR riv_add15 =
            cntr->New_bin_arith(air::core::OPCODE::ADD, s32t, riv,
                                cntr->New_intconst(s32t, 15, spos), spos);

        NODE_PTR gather_node1 = New_gather_node(
            cntr, New_sharding_loader(cntr, shard_var, riv, spos),
            New_sharding_loader(cntr, shard_var, riv_add1, spos),
            reshard_block2_type, 2, spos);
        NODE_PTR gather_node2 = New_gather_node(
            cntr, New_sharding_loader(cntr, shard_var, riv_add2, spos),
            New_sharding_loader(cntr, shard_var, riv_add3, spos),
            reshard_block2_type, 2, spos);

        NODE_PTR gather_node3 = New_gather_node(
            cntr, New_sharding_loader(cntr, shard_var, riv_add4, spos),
            New_sharding_loader(cntr, shard_var, riv_add5, spos),
            reshard_block2_type, 2, spos);

        NODE_PTR gather_node4 = New_gather_node(
            cntr, New_sharding_loader(cntr, shard_var, riv_add6, spos),
            New_sharding_loader(cntr, shard_var, riv_add7, spos),
            reshard_block2_type, 2, spos);

        NODE_PTR gather_node5 = New_gather_node(
            cntr, New_sharding_loader(cntr, shard_var, riv_add8, spos),
            New_sharding_loader(cntr, shard_var, riv_add9, spos),
            reshard_block2_type, 2, spos);

        NODE_PTR gather_node6 = New_gather_node(
            cntr, New_sharding_loader(cntr, shard_var, riv_add10, spos),
            New_sharding_loader(cntr, shard_var, riv_add11, spos),
            reshard_block2_type, 2, spos);

        NODE_PTR gather_node7 = New_gather_node(
            cntr, New_sharding_loader(cntr, shard_var, riv_add12, spos),
            New_sharding_loader(cntr, shard_var, riv_add13, spos),
            reshard_block2_type, 2, spos);

        NODE_PTR gather_node8 = New_gather_node(
            cntr, New_sharding_loader(cntr, shard_var, riv_add14, spos),
            New_sharding_loader(cntr, shard_var, riv_add15, spos),
            reshard_block2_type, 2, spos);

        NODE_PTR gn1 = New_gather_node(cntr, gather_node1, gather_node2,
                                       reshard_block_type, 1, spos);

        NODE_PTR gn2 = New_gather_node(cntr, gather_node3, gather_node4,
                                       reshard_block_type, 1, spos);

        NODE_PTR gn3 = New_gather_node(cntr, gather_node5, gather_node6,
                                       reshard_block_type, 1, spos);

        NODE_PTR gn4 = New_gather_node(cntr, gather_node7, gather_node8,
                                       reshard_block_type, 1, spos);

        NODE_PTR gn11 =
            New_gather_node(cntr, gn1, gn2, reshard_block_type, 1, spos);

        NODE_PTR gn22 =
            New_gather_node(cntr, gn3, gn4, reshard_block_type, 1, spos);

        NODE_PTR gn111 =
            New_gather_node(cntr, gn11, gn22, reshard_block_type, 1, spos);
        STMT_PTR new_st_gather =
            New_sharding_store(cntr, reshard_var, liv, gn111, spos);

        reshardm_sl.Append(new_st_gather);
        Prepend(reshardm_loop);
        return reshard_var;
      } else {
        std::vector<int64_t> reshard_block2_shape = {
            reshard_block_shape[0], reshard_block_shape[1] / 2,
            reshard_block_shape[2], reshard_block_shape[3]};
        TYPE_PTR reshard_block2_type =
            New_array_type(gscope, "TB_reshard2", Get_num_vloop(), etype,
                           reshard_block2_shape, spos);
        STMT_PTR reshardm_loop =
            New_mesh_loop(cntr, "reshardm", num_reshard, spos);
        STMT_LIST reshardm_sl = STMT_LIST::Enclosing_list(
            reshardm_loop->Node()->Child(3)->End_stmt());
        NODE_PTR liv = cntr->New_ld(reshardm_loop->Node()->Iv(), spos);
        NODE_PTR riv =
            cntr->New_bin_arith(air::core::OPCODE::MUL, s32t,
                                cntr->New_ld(reshardm_loop->Node()->Iv(), spos),
                                cntr->New_intconst(s32t, numtoone, spos), spos);
        NODE_PTR riv_add1 =
            cntr->New_bin_arith(air::core::OPCODE::ADD, s32t, riv,
                                cntr->New_intconst(s32t, 1, spos), spos);
        NODE_PTR riv_add2 =
            cntr->New_bin_arith(air::core::OPCODE::ADD, s32t, riv,
                                cntr->New_intconst(s32t, 2, spos), spos);
        NODE_PTR riv_add3 =
            cntr->New_bin_arith(air::core::OPCODE::ADD, s32t, riv,
                                cntr->New_intconst(s32t, 3, spos), spos);

        NODE_PTR gather_node1 = New_gather_node(
            cntr, New_sharding_loader(cntr, shard_var, riv, spos),
            New_sharding_loader(cntr, shard_var, riv_add1, spos),
            reshard_block2_type, 2, spos);
        NODE_PTR gather_node2 = New_gather_node(
            cntr, New_sharding_loader(cntr, shard_var, riv_add2, spos),
            New_sharding_loader(cntr, shard_var, riv_add3, spos),
            reshard_block2_type, 2, spos);

        NODE_PTR gather_node = New_gather_node(cntr, gather_node1, gather_node2,
                                               reshard_block_type, 1, spos);
        STMT_PTR new_st_gather =
            New_sharding_store(cntr, reshard_var, liv, gather_node, spos);

        reshardm_sl.Append(new_st_gather);
        Prepend(reshardm_loop);
        return reshard_var;
      }
    } else if (numtoone == 4) {
      AIR_ASSERT_MSG(xyz[2] == 1, "TODO: numtoone == 4 need z == 1");
      std::vector<int64_t> reshard_block2_shape = {
          reshard_block_shape[0], reshard_block_shape[1] / 2,
          reshard_block_shape[2], reshard_block_shape[3]};
      TYPE_PTR reshard_block2_type =
          New_array_type(gscope, "TB_reshard2", Get_num_vloop(), etype,
                         reshard_block2_shape, spos);
      STMT_PTR reshardm_loop =
          New_mesh_loop(cntr, "reshardm", num_reshard, spos);
      STMT_LIST reshardm_sl = STMT_LIST::Enclosing_list(
          reshardm_loop->Node()->Child(3)->End_stmt());
      NODE_PTR liv = cntr->New_ld(reshardm_loop->Node()->Iv(), spos);
      NODE_PTR riv =
          cntr->New_bin_arith(air::core::OPCODE::MUL, s32t,
                              cntr->New_ld(reshardm_loop->Node()->Iv(), spos),
                              cntr->New_intconst(s32t, numtoone, spos), spos);
      NODE_PTR riv_add1 =
          cntr->New_bin_arith(air::core::OPCODE::ADD, s32t, riv,
                              cntr->New_intconst(s32t, 1, spos), spos);
      NODE_PTR riv_add2 =
          cntr->New_bin_arith(air::core::OPCODE::ADD, s32t, riv,
                              cntr->New_intconst(s32t, 2, spos), spos);
      NODE_PTR riv_add3 =
          cntr->New_bin_arith(air::core::OPCODE::ADD, s32t, riv,
                              cntr->New_intconst(s32t, 3, spos), spos);
      NODE_PTR gather_node1 =
          New_gather_node(cntr, New_sharding_loader(cntr, shard_var, riv, spos),
                          New_sharding_loader(cntr, shard_var, riv_add1, spos),
                          reshard_block2_type, 1, spos);
      NODE_PTR gather_node2 = New_gather_node(
          cntr, New_sharding_loader(cntr, shard_var, riv_add2, spos),
          New_sharding_loader(cntr, shard_var, riv_add3, spos),
          reshard_block2_type, 1, spos);
      NODE_PTR gather_node = New_gather_node(cntr, gather_node1, gather_node2,
                                             reshard_block_type, 1, spos);
      STMT_PTR new_st_gather =
          New_sharding_store(cntr, reshard_var, liv, gather_node, spos);
      reshardm_sl.Append(new_st_gather);
      Prepend(reshardm_loop);
      return reshard_var;
    } else {
      AIR_ASSERT_MSG(0, "num_reshard>1 case error :)")
    }
  }

  return shard_var;
}

// Add two shard_vars in mesh
// TODO: each var should map to a mesh.
ADDR_DATUM_PTR T2TSHARDING_CTX::Add_shard(ADDR_DATUM_PTR       shard0,
                                          CONSTANT_PTR         shard1,
                                          std::vector<int64_t> mesh,
                                          const SPOS&          spos) {
  AIR_ASSERT(mesh.size() == 2);
  return shard0;
  // mesh consistency
  // AIR_ASSERT(shard0->Type()->Cast_to_arr()->Elem_count() ==
  //           shard1->Type()->Cast_to_arr()->Elem_count());
  // AIR_ASSERT(shard0->Type()->Cast_to_arr()->Elem_count() == mesh[0] *
  // mesh[1]);

  CONTAINER*  cntr   = Get_cntr();
  GLOB_SCOPE* gscope = cntr->Glob_scope();
  FUNC_SCOPE* fscope = cntr->Parent_func_scope();

  bool is_shard = Is_shard_data(shard0);
  // TODO: Elementwise add
  std::string addshard_str =
      std::string("addshard") + std::to_string(Get_num_vloop());
  ADDR_DATUM_PTR addshard_var =
      fscope->New_var(shard0->Type(), addshard_str.c_str(), spos);

  if (!is_shard) {
    NODE_PTR add_node = cntr->New_bin_arith(
        air::base::OPCODE(nn::core::NN, nn::core::OPCODE::ADD), shard0->Type(),
        cntr->New_ld(shard0, spos), cntr->New_ldc(shard1, spos), spos);

    STMT_PTR new_st = cntr->New_st(add_node, addshard_var, spos);
    Prepend(new_st);
  } else {
    //
    STMT_PTR reshard1_loop = New_mesh_loop(
        cntr, "add_bias", shard0->Type()->Cast_to_arr()->Elem_count(), spos);
    STMT_LIST reshard2_sl =
        STMT_LIST::Enclosing_list(reshard1_loop->Node()->Child(3)->End_stmt());
    NODE_PTR riv = cntr->New_ld(reshard1_loop->Node()->Iv(), spos);

    // New_sharding_const_loader
    NODE_PTR add_node = cntr->New_bin_arith(
        air::base::OPCODE(nn::core::NN, nn::core::OPCODE::ADD),
        shard0->Type()->Cast_to_arr()->Elem_type(),
        New_sharding_loader(cntr, shard0, riv, spos),
        New_sharding_const_loader(cntr, shard1, riv, spos), spos);

    STMT_PTR new_st_add =
        New_sharding_store(cntr, addshard_var, riv, add_node, spos);

    reshard2_sl.Append(new_st_add);
    Prepend(reshard1_loop);
  }
  return addshard_var;
}

}  // namespace vector
}  // namespace nn
