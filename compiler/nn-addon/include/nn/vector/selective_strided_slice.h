//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_SELECTIVE_STRIDED_SLICE_CTX_H
#define NN_VECTOR_SELECTIVE_STRIDED_SLICE_CTX_H
#include "air/base/instrument_ctx.h"
#include "air/core/default_handler.h"
#include "air/opt/ssa_build.h"
#include "air/opt/ssa_container.h"
#include "nn/core/attr.h"
#include "nn/core/default_handler.h"
#include "nn/vector/config.h"
#include "nn/vector/strided_slice_fusion.h"
#include "nn/vector/vector_ctx.h"
#include "nn/vector/vector_enum.h"
#include "nn/vector/vector_gen.h"
#include "nn/vector/vector_utils.h"

namespace nn {

namespace vector {

using namespace air::base;
using namespace air::driver;

using DEF_SYM_NODE_VEC = std::vector<NODE_PTR>;

class SELECTIVE_STRIDED_SLICE_CTX : public INSTRUMENT_CTX {
public:
  SELECTIVE_STRIDED_SLICE_CTX(FUNC_SCOPE* func_cope, VECTOR_CTX& ctx,
                              const DRIVER_CTX*    driver_ctx,
                              const VECTOR_CONFIG& cfg,
                              const SS_SFT_MAP&    ss_sft_map)
      : _ctx(ctx),
        _driver_ctx(driver_ctx),
        _config(cfg),
        _ssa_cntr(&func_cope->Container()),
        _ssa_builder(func_cope, &_ssa_cntr, driver_ctx),
        _ss_sft_map(ss_sft_map),
        _def_sym_node_vec() {}

  template <typename RETV, typename VISITOR>
  RETV Handle_node(VISITOR* visitor, NODE_PTR node) {
    for (uint32_t i = 0; i < node->Num_child(); ++i) {
      visitor->template Visit<RETV>(node->Child(i));
    }

    // after copy propagation, strided_slice node has been child node.
    if (Is_legal_op_dit_with_ss(node)) {
      // Do the identity transformation when strided_slice is its child.
      if (Number_of_strided_slice_in_children(node) > 0) {
        // currently ops add, mul, relu, conv, flatten, average/max pool can
        // do equivalent transformation with strided_slice.
        // ops(strided_slice(input))  --> strided_slice(ops'(input))
        Do_identity_transformation(node);
      }
    }

    // mark current processed strided_slice node
    if (_ss_sft_map.find(node->Id()) != _ss_sft_map.end()) {
      _cur_ss_id = node->Id();
    }

    // fuse (one or multiple)strided_slice into gemm or fuse multiple
    // strided_slice into one strided_slice
    if ((_cur_ss_id != air::base::Null_id) &&
        (_ss_sft_map[_cur_ss_id] == air::base::Null_id) &&
        (node->Opcode() ==
         OPCODE(nn::core::NN, nn::core::OPCODE::STRIDED_SLICE))) {
      Fuse_strided_slice_into_one(node);
    } else if ((_cur_ss_id != air::base::Null_id) &&
               (_ss_sft_map[_cur_ss_id] == node->Id())) {
      if (node->Opcode() == OPCODE(nn::core::NN, nn::core::OPCODE::GEMM) &&
          node->Child(0)->Opcode() ==
              OPCODE(nn::core::NN, nn::core::OPCODE::STRIDED_SLICE)) {
        Fuse_strided_slice_into_gemm(node);
      } else if (node->Opcode() ==
                     OPCODE(nn::core::NN, nn::core::OPCODE::STRIDED_SLICE) &&
                 node->Child(0)->Opcode() ==
                     OPCODE(nn::core::NN, nn::core::OPCODE::STRIDED_SLICE)) {
        Fuse_strided_slice_into_one(node);
      } else {
        AIR_ASSERT(this->Parent(1)->Child(0)->Opcode() ==
                   OPCODE(nn::core::NN, nn::core::OPCODE::STRIDED_SLICE));
        // mark this strided_slice node has no fusion target, then it will not
        // be propagated later.
        _ss_sft_map.insert(std::pair<NODE_ID, NODE_ID>(
            this->Parent(1)->Child(0)->Id(), air::base::Null_id));
      }
    }

    return RETV();
  }

  // declare access API for VECTOR_CTX
  DECLARE_VECTOR_CTX_ACCESS_API(_ctx)

  // declare access API for VECTOR_CONFIG
  DECLARE_VECTOR_CONFIG_ACCESS_API(_config)

  // declare trace API for detail tracing
  DECLARE_TRACE_DETAIL_API(_config, _driver_ctx)

  DEF_SYM_NODE_VEC& Get_def_sym_node_vec() { return _def_sym_node_vec; }

  void Insert_def_sym_node_vec(NODE_PTR def_sym_node) {
    _def_sym_node_vec.push_back(def_sym_node);
  }

  void Build_ssa() { _ssa_builder.Perform(); }

  air::opt::SSA_CONTAINER* Ssa_cntr() { return &_ssa_cntr; }
  air::base::CONTAINER*    Container() { return _ssa_cntr.Container(); }

  //! @brief whehter this op can do identity transformation(dit) with
  //! strided_slice(ss). do not change the execution order of multiple ss
  //! nodes. do not change the execution order of gemm and ss since ss can be
  //! merged with gemm.
  bool Is_legal_op_dit_with_ss(NODE_PTR node) {
    if (node->Opcode() != OPCODE(nn::core::NN, nn::core::OPCODE::GEMM) &&
        node->Opcode() !=
            OPCODE(nn::core::NN, nn::core::OPCODE::STRIDED_SLICE)) {
      return true;
    }
    return false;
  }

  void Fuse_strided_slice_into_one(NODE_PTR node) {
    AIR_ASSERT(node->Opcode() ==
               OPCODE(nn::core::NN, nn::core::OPCODE::STRIDED_SLICE));
    // only one strided_slice, return directly
    if (node->Child(0)->Opcode() !=
        OPCODE(nn::core::NN, nn::core::OPCODE::STRIDED_SLICE)) {
      return;
    }

    // multiply strided_slice, need to fuse
    AIR_ASSERT(node->Child(0)->Opcode() ==
               OPCODE(nn::core::NN, nn::core::OPCODE::STRIDED_SLICE));

    NODE_PTR first_ss_node = node;
    NODE_PTR last_ss_node  = first_ss_node;

    // find the last strided_slice node
    int accumulated_stride = 1;
    last_ss_node   = Find_last_strided_slice(last_ss_node, accumulated_stride);
    NODE_PTR input = last_ss_node->Child(0);

    int64_t batch = 0, channel_in = 0, input_height = 0, input_width = 0;
    Get_array_nchw(input->Rtype(), batch, channel_in, input_height,
                   input_width);

    CONSTANT_PTR start_indice_const = last_ss_node->Child(1)->Const();
    int64_t      start_row = 0, start_column = 0;
    Get_const_array_value(start_indice_const, start_row, start_column);
    AIR_ASSERT_MSG(start_row == start_column, "only support same start indice");

    CONSTANT_PTR slice_size_const = last_ss_node->Child(2)->Const();
    int64_t      ss_height = 0, ss_width = 0;
    Get_const_array_value(slice_size_const, ss_height, ss_width);

    CONSTANT_PTR stride_size_const = last_ss_node->Child(3)->Const();
    int64_t      stride_row = 0, stride_col = 0;
    Get_const_array_value(stride_size_const, stride_row, stride_col);
    AIR_ASSERT_MSG(stride_row == stride_col, "only support same stride size");

    std::vector<int64_t> start_indiex{start_row, start_column};
    std::vector<int64_t> slice_size{ss_height, ss_width};
    std::vector<int64_t> stride_size{accumulated_stride, accumulated_stride};

    VECTOR_GEN vgen(this->Ssa_cntr()->Container());

    NODE_PTR strided_slice_node = vgen.New_strided_slice(
        input, start_indiex, slice_size, stride_size, channel_in, node->Spos());

    // get the parent node of current node.
    NODE_PTR parent_node = this->Parent(1);
    // change appropriate child to new strided_slice node
    for (uint32_t i = 0; i < parent_node->Num_child(); i++) {
      if (parent_node->Child(i)->Id() == node->Id()) {
        parent_node->Set_child(i, strided_slice_node);
      }
    }

    // make this new strided_slice avoid be propagated
    _ss_sft_map.insert(std::pair<NODE_ID, NODE_ID>(
        this->Parent(1)->Child(0)->Id(), air::base::Null_id));
  }

  void Fuse_strided_slice_into_gemm(NODE_PTR node) {
    AIR_ASSERT_MSG(
        node->Opcode() == OPCODE(nn::core::NN, nn::core::OPCODE::GEMM) &&
            node->Child(0)->Opcode() ==
                OPCODE(nn::core::NN, nn::core::OPCODE::STRIDED_SLICE),
        "till now, only when strided_slcie is gemm's first child will fuse "
        "stride_slice optimization is supported");
    // since we do not invoke strided_slice immediately, gaps will remains in
    // data, input size is never changed. we want to fuse strided_slice with
    // gemm. need to make gemm's weight size equal gemm's input size so we
    // need to fill gemm's weight data with 0 corresponding to junk data in
    // gemm's input

    // to complete above tasks, we need to recover the data layout in gemm's
    // input by analyze consecutive strided_slice node

    // input data size can be get from last strided_slice's input
    // real valid data size can be get from the first stride_slice node
    // the interval size of valid data is determined by stride value in all
    // strided_slice. valid data is seperated by gaps whose value is final
    // stride - 1 and stride is cummulated by pow of 2 since all stride value
    // is 2 in current ai models pad operation may result in several junk data
    // before and after valid data in each row, and the junk data in the
    // previous position of the input may be processed by roll left by
    // previous op, this happend in lenet. fortunately, the layout of valid
    // data and invalid data is regular.

    // lenet as example: input data size is 16*32*32, 4 strided_slice is
    // inserted. first stride_slice is: [0,0;10,10;2,2], so valid data size is
    // (10/2)*(10/2)*16 = 400 which should equal to gemm's original weight
    // width we need to expand gemm's original weight width from 400 to
    // 16*32*32 with filling 0 between valid weight data. two strided_slice
    // contains stride=2, so final stride is 2*2 = 4, valid data is seperated
    // by 4-1=3

    // data layout details:
    // data in logical one row is (10/2)*4 = 20, which is v###v###v###v###v###
    // logical input column size is 32, so there are 32-20 = 12 trailing # in
    // one row stride affects columns and rows, so next 3 rows are all #
    // logical input row size is 32, so there are 32 - 20 = 12 traling rows
    // whose value is # all 16 channels are same format

    // need to expand gemm's weight data to above data layout.

    NODE_PTR first_ss_node = node->Child(0);
    NODE_PTR last_ss_node  = first_ss_node;

    // find the last strided_slice node
    int accumulated_stride = 1;
    last_ss_node = Find_last_strided_slice(last_ss_node, accumulated_stride);

    // change gemm's input child to last strided_slice's input child
    node->Set_child(0, last_ss_node->Child(0));
    // gemm's input
    NODE_PTR gemm_input = node->Child(0);

    int64_t batch = 0, channel_in = 0, input_height = 0, input_width = 0;
    Get_array_nchw(gemm_input->Rtype(), batch, channel_in, input_height,
                   input_width);

    // gemm's weight
    AIR_ASSERT_MSG(node->Child(1)->Rtype()->Is_array(),
                   "operand1 is not an array type");
    ARRAY_TYPE_PTR weight_ty_arr = node->Child(1)->Rtype()->Cast_to_arr();

    std::vector<int64_t> weight_shape = Get_array_dim_ubs(weight_ty_arr);
    AIR_ASSERT_MSG(weight_shape.size() == 2, "operand1_const dim %d != 2",
                   weight_shape.size());

    int64_t      height       = weight_shape[0];
    int64_t      width        = weight_shape[1];
    CONSTANT_PTR weight_const = node->Child(1)->Const();
    const float* cptr         = weight_const->Array_ptr<float>();

    int group_size = Get_valid_data_in_row(first_ss_node->Child(2)->Const(),
                                           first_ss_node->Child(3)->Const());

    std::vector<int> channel =
        Get_attr_data<int>(first_ss_node, core::ATTR::CHANNEL);
    AIR_ASSERT_MSG(channel.size() == 1,
                   "strided slice only contains 1 channel attribute");
    AIR_ASSERT_MSG(
        group_size * group_size * (int64_t)channel[0] == width,
        "result of strided_slice %d should be equal to gemm's width %d",
        group_size * group_size * (int64_t)channel[0], width);

    int   target_width   = input_height * input_width * (int64_t)channel[0];
    FPMAT new_weight_mat = Expand_with_zero(
        cptr, height, width, group_size, accumulated_stride,
        input_height * (accumulated_stride - 1) +
            (input_height - group_size * accumulated_stride),
        target_width, group_size * group_size, input_height * input_width);

    width = target_width;

    // update gemm's weight data
    std::vector<int64_t> new_weight_shape{height, width};
    FPVEC                new_weight_vec;
    for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
        new_weight_vec.push_back(new_weight_mat[i][j]);
      }
    }
    CONSTANT_PTR new_weight_const = New_array_const(
        this->Ssa_cntr()->Container()->Glob_scope(), "new_weight",
        height * width, weight_ty_arr->Elem_type(), new_weight_shape,
        (void*)new_weight_vec.data(), node->Spos());
    NODE_PTR new_weight_node =
        this->Ssa_cntr()->Container()->New_ldc(new_weight_const, node->Spos());
    node->Set_child(1, new_weight_node);
  }

  uint32_t Number_of_strided_slice_in_children(NODE_PTR node) {
    // when there is strided_slice node in children nodes, it should be the
    // first child
    uint32_t nums = 0;
    for (uint32_t i = 0; i < node->Num_child(); ++i) {
      if (node->Child(i)->Opcode() ==
          OPCODE(nn::core::NN, nn::core::OPCODE::STRIDED_SLICE)) {
        nums++;
      }
    }
    return nums;
  }

  NODE_PTR Find_last_strided_slice(NODE_PTR last_ss_node,
                                   int&     accumulated_stride) {
    while (last_ss_node->Opcode() ==
           OPCODE(nn::core::NN, nn::core::OPCODE::STRIDED_SLICE)) {
      int64_t stride_row = 0, stride_col = 0;
      Get_const_array_value(last_ss_node->Child(3)->Const(), stride_row,
                            stride_col);
      AIR_ASSERT(stride_row == stride_col);

      accumulated_stride *= stride_row;

      if (last_ss_node->Child(0)->Opcode() !=
          OPCODE(nn::core::NN, nn::core::OPCODE::STRIDED_SLICE)) {
        break;
      }
      last_ss_node = last_ss_node->Child(0);
    }
    return last_ss_node;
  }

  NODE_PTR Find_last_strided_slice(NODE_PTR               last_ss_node,
                                   std::vector<NODE_PTR>& ss_node_vec) {
    while (last_ss_node->Opcode() ==
           OPCODE(nn::core::NN, nn::core::OPCODE::STRIDED_SLICE)) {
      ss_node_vec.push_back(last_ss_node);
      if (last_ss_node->Child(0)->Opcode() !=
          OPCODE(nn::core::NN, nn::core::OPCODE::STRIDED_SLICE)) {
        break;
      }
      last_ss_node = last_ss_node->Child(0);
    }
    return last_ss_node;
  }

  void Adjust_rtype(NODE_PTR node) {
    AIR_ASSERT(
        node->Opcode() == OPCODE(nn::core::NN, nn::core::OPCODE::CONV) ||
        node->Opcode() == OPCODE(nn::core::NN, nn::core::OPCODE::RELU) ||
        node->Opcode() == OPCODE(nn::core::NN, nn::core::OPCODE::RESHAPE) ||
        node->Opcode() == OPCODE(nn::core::NN, nn::core::OPCODE::FLATTEN));
    if (node->Opcode() == OPCODE(nn::core::NN, nn::core::OPCODE::CONV)) {
      // adjust shape of conv's rtype
      TYPE_PTR             rtype       = node->Rtype();
      std::vector<int64_t> rtype_shape = rtype->Cast_to_arr()->Shape();

      TYPE_PTR             input_rtype = node->Child(0)->Rtype();
      std::vector<int64_t> input_rtype_shape =
          input_rtype->Cast_to_arr()->Shape();
      // input height and width
      rtype_shape[2]     = input_rtype_shape[2];
      rtype_shape[3]     = input_rtype_shape[3];
      TYPE_PTR new_rtype = New_array_type(
          this->Ssa_cntr()->Container()->Glob_scope(), "conv",
          input_rtype->Cast_to_arr()->Elem_type(), rtype_shape, node->Spos());
      node->Set_rtype(new_rtype);
    } else {
      node->Set_rtype(node->Child(0)->Rtype());
    }
  }

  void Adjust_attr(NODE_PTR node, const std::vector<NODE_PTR>& ss_node_vec) {
    AIR_ASSERT(
        node->Opcode() == OPCODE(nn::core::NN, nn::core::OPCODE::MAX_POOL) ||
        node->Opcode() ==
            OPCODE(nn::core::NN, nn::core::OPCODE::AVERAGE_POOL) ||
        node->Opcode() ==
            OPCODE(nn::core::NN, nn::core::OPCODE::GLOBAL_AVERAGE_POOL) ||
        node->Opcode() == OPCODE(nn::core::NN, nn::core::OPCODE::CONV));

    int prev_stride = 1;
    int prev_pad    = 0;
    for (auto ss_node : ss_node_vec) {
      int64_t stride_row = 0, stride_col = 0;
      Get_const_array_value(ss_node->Child(3)->Const(), stride_row, stride_col);
      AIR_ASSERT_MSG(stride_row == stride_col, "only support same stride size");

      prev_stride *= stride_row;

      int64_t start_row = 0, start_column = 0;
      Get_const_array_value(ss_node->Child(1)->Const(), start_row,
                            start_column);
      AIR_ASSERT_MSG(start_row == start_column,
                     "only support same start indice");

      prev_pad += start_row;
    }

    std::vector<int> raw_strides;

    if (node->Opcode() !=
        OPCODE(nn::core::NN, nn::core::OPCODE::GLOBAL_AVERAGE_POOL)) {
      // adjust stride attribute
      raw_strides = Get_attr_data<int>(node, core::ATTR::STRIDE);
      if (prev_stride != 1) {
        for (auto& r_stride : raw_strides) {
          r_stride *= prev_stride;
        }
        node->Set_attr(core::ATTR::STRIDE, raw_strides.data(),
                       raw_strides.size());
      }
    }

    // adjust conv's padding attribute, add new attribute to indicate the real
    // padding.
    if (node->Opcode() == OPCODE(nn::core::NN, nn::core::OPCODE::CONV)) {
      std::vector<int> pads = Get_attr_data<int>(node, core::ATTR::PAD);

      NODE_PTR weight_node = node->Child(1);
      int64_t  channel_out = 0, channel_in = 0, kernel_height = 0,
              kernel_width = 0;
      Get_array_nchw(weight_node->Rtype(), channel_out, channel_in,
                     kernel_height, kernel_width);

      // No need to process additional pads for other pads
      if ((pads[0] == 0) && (kernel_height != 1)) {
        // input rtype shape
        TYPE_PTR             input_rtype = node->Child(0)->Rtype();
        std::vector<int64_t> input_rtype_shape =
            input_rtype->Cast_to_arr()->Shape();

        // first strided slice rtype shape
        TYPE_PTR             ss_rtype       = ss_node_vec[0]->Rtype();
        std::vector<int64_t> ss_rtype_shape = ss_rtype->Cast_to_arr()->Shape();

        // [2] here is height, height == width
        // if do strided_slice directly, output height = (ss_rtype_shape[2] -
        // kernel_height + 1) = 14-5+1=10 when there is previous stride, means
        // that output height is seperated by stride, so the output height
        // multiply with prev_stride 32 - 2*10 = 12; 12 / 2 = 6
        int64_t padsize = input_rtype_shape[2] -
                          (ss_rtype_shape[2] - kernel_height + 1) * prev_stride;
        if (prev_pad > 1) {
          padsize /= 2;  // we pad same in all sides
        }

        if (padsize != 0) {
          std::vector<int> pad_size = {(int)(padsize)};
          this->Trace(TF_LOWER, "add fhe pad attr to node ", node->Name(),
                      "value is ", pad_size[0], "\n");
          node->Set_attr(core::ATTR::KEEP_SHAPE_PAD, pad_size.data(),
                         pad_size.size());
        }
      }
    }

    // adjust pool's kernel shape attribute, and add new attribute to indicate
    // need extra roll op and valid data in kernel shape.
    if (node->Opcode() ==
        OPCODE(nn::core::NN, nn::core::OPCODE::GLOBAL_AVERAGE_POOL)) {
      // input rtype shape
      TYPE_PTR             input_rtype = node->Child(0)->Rtype();
      std::vector<int64_t> input_rtype_shape =
          input_rtype->Cast_to_arr()->Shape();
      int64_t height          = input_rtype_shape[2];
      int64_t width           = input_rtype_shape[3];
      int     vdn_in_ks       = (height / prev_stride) * (width / prev_stride);
      std::vector<int> vdn_ks = {vdn_in_ks};
      node->Set_attr(core::ATTR::VDN_IN_KS, vdn_ks.data(), vdn_ks.size());
    } else if (node->Opcode() ==
                   OPCODE(nn::core::NN, nn::core::OPCODE::MAX_POOL) ||
               node->Opcode() ==
                   OPCODE(nn::core::NN, nn::core::OPCODE::AVERAGE_POOL)) {
      std::vector<int> kernel_shape =
          Get_attr_data<int>(node, core::ATTR::KSHAPE);
      int vdn_in_ks = 1;
      if (kernel_shape[0] < raw_strides[0]) {
        for (auto& ks : kernel_shape) {
          vdn_in_ks *= ks;
          ks = raw_strides[0];
        }
        std::vector<int> vdn_ks = {vdn_in_ks};
        node->Set_attr(core::ATTR::VDN_IN_KS, vdn_ks.data(), vdn_ks.size());
        node->Set_attr(core::ATTR::KSHAPE, kernel_shape.data(),
                       kernel_shape.size());
      }

      // input rtype shape
      TYPE_PTR             input_rtype = node->Child(0)->Rtype();
      std::vector<int64_t> input_rtype_shape =
          input_rtype->Cast_to_arr()->Shape();

      // first strided slice rtype shape
      TYPE_PTR             ss_rtype       = ss_node_vec[0]->Rtype();
      std::vector<int64_t> ss_rtype_shape = ss_rtype->Cast_to_arr()->Shape();

      // [2] here is height, height == width
      // 32 - 2*10 = 12; 12 / 2 = 6, ks = 4
      int64_t padsize = input_rtype_shape[2] - ss_rtype_shape[2] * prev_stride;
      if (prev_pad > 1) {
        padsize /= 2;  // we pad same in all sides
      }

      if (padsize % kernel_shape[0] != 0) {
        std::vector<int> roll_size = {
            (int)(padsize * input_rtype_shape[2] + padsize)};
        this->Trace(TF_LOWER, "add pre_roll attr to node ", node->Name(),
                    "value is ", roll_size[0], "\n");
        node->Set_attr(core::ATTR::PROLL, roll_size.data(), roll_size.size());
        // TODO: when pre_roll value is not zero, adjust strided_slice start
        // index?
      }
    }
  }

  //! @brief do identity transformation between node and children
  //! strided_slice node. after this transformation, strided_slice node
  //! becomes the parent node, and the original parent node will be
  //! strided_slice node's child node.
  void Do_identity_transformation(NODE_PTR node) {
    NODE_PTR              first_ss_node;
    NODE_PTR              last_ss_node;
    std::vector<NODE_PTR> ss_node_vec;

    if (Number_of_strided_slice_in_children(node) == 2) {
      // Do the identity transformation: mul(strided_slice(input),
      // strided_slice(input)) -> strided_slice(mul(input, input))
      AIR_ASSERT_MSG(
          node->Opcode() == OPCODE(nn::core::NN, nn::core::OPCODE::MUL) ||
              node->Opcode() == OPCODE(nn::core::NN, nn::core::OPCODE::ADD),
          "till now, only add/mul with two strided_slice child node is "
          "supported");
      NODE_PTR ss_node0   = node->Child(0);
      NODE_PTR l_ss_node0 = ss_node0;
      NODE_PTR ss_node1   = node->Child(1);
      NODE_PTR l_ss_node1 = ss_node1;

      std::vector<NODE_PTR> ss_node_vec0;
      std::vector<NODE_PTR> ss_node_vec1;

      // process consecutive strided_slice node
      // find the last strided_slice node, put all strided_slice into vector.
      l_ss_node0 = Find_last_strided_slice(l_ss_node0, ss_node_vec0);
      l_ss_node1 = Find_last_strided_slice(l_ss_node1, ss_node_vec1);

      // TODO: combine two child strided_slice node to one should make sure
      // these two strided_slice are same!

      // change add/mul node's two child to strided_slice's child0
      node->Set_child(0, l_ss_node0->Child(0));
      node->Set_child(1, l_ss_node1->Child(0));

      first_ss_node = ss_node0;
      last_ss_node  = l_ss_node0;
    } else if (Number_of_strided_slice_in_children(node) == 1) {
      // Do the identity transformation, for example:
      // flatten(strided_slice(input)) -> strided_slice(flatten(input))
      first_ss_node = node->Child(0);
      last_ss_node  = first_ss_node;

      // process consecutive strided_slice node
      // find the last strided_slice node, put all strided_slice into vector.
      last_ss_node = Find_last_strided_slice(last_ss_node, ss_node_vec);
      // change op node's child0 to strided_slice's child0
      node->Set_child(0, last_ss_node->Child(0));
    } else {
      AIR_ASSERT_MSG(false, "no supported in Do_identity_transformation");
    }
    // when do strided_slice op later, op's output shape should be adjusted.
    // make sure the first child of node has changed to last strided slice
    // node's first child in previous code.
    // TODO: improve here: which ops need to adjust rtype, which ops not.
    TYPE_PTR output_rtype = node->Child(0)->Rtype();
    if ((node->Opcode() == OPCODE(nn::core::NN, nn::core::OPCODE::CONV)) ||
        (node->Opcode() == OPCODE(nn::core::NN, nn::core::OPCODE::RELU)) ||
        (node->Opcode() == OPCODE(nn::core::NN, nn::core::OPCODE::RESHAPE)) ||
        (node->Opcode() == OPCODE(nn::core::NN, nn::core::OPCODE::FLATTEN))) {
      Adjust_rtype(node);
      output_rtype = node->Rtype();
    }

    // when do strided_slice op later, some op's attribute may be adjusted.
    if (node->Opcode() == OPCODE(nn::core::NN, nn::core::OPCODE::MAX_POOL) ||
        node->Opcode() ==
            OPCODE(nn::core::NN, nn::core::OPCODE::AVERAGE_POOL) ||
        node->Opcode() ==
            OPCODE(nn::core::NN, nn::core::OPCODE::GLOBAL_AVERAGE_POOL) ||
        node->Opcode() == OPCODE(nn::core::NN, nn::core::OPCODE::CONV)) {
      Adjust_attr(node, ss_node_vec);
    }

    // save op node's tmp result to preg
    CONTAINER* cntr     = this->Ssa_cntr()->Container();
    PREG_PTR   tmp_preg = cntr->Parent_func_scope()->New_preg(output_rtype);
    STMT_PTR   st_tmp   = cntr->New_stp(node, tmp_preg, node->Spos());
    this->Prepend(st_tmp);

    // change strided_slice node's child to op node.
    last_ss_node->Set_child(0, cntr->New_ldp(tmp_preg, node->Spos()));

    // get the parent node of current node.
    NODE_PTR parent_node = this->Parent(1);
    // change appropriate child to new strided_slice node
    for (uint32_t i = 0; i < parent_node->Num_child(); i++) {
      if (parent_node->Child(i)->Id() == node->Id()) {
        parent_node->Set_child(i, first_ss_node);
      }
    }
  }

  //! @brief Partial copy propation, only copy propagate strided_slice define
  //! site to use site.
  void Partial_copy_prop(NODE_PTR node) {
    air::opt::SSA_CONTAINER* ssa_cntr = this->Ssa_cntr();
    air::opt::SSA_VER_PTR    ssa_ver  = ssa_cntr->Node_ver(node->Id());
    AIR_ASSERT_MSG(ssa_ver->Kind() == air::opt::VER_DEF_KIND::STMT,
                   "must define by stmt");

    CONTAINER* cntr     = ssa_cntr->Container();
    NODE_PTR   def_node = cntr->Stmt(ssa_ver->Def_stmt_id())->Node();

    // only copy prop strided_slice(who has fusion target) define to use site
    if ((def_node->Child(0)->Opcode() ==
         OPCODE(nn::core::NN, nn::core::OPCODE::STRIDED_SLICE)) &&
        ((_ss_sft_map.find(def_node->Child(0)->Id()) == _ss_sft_map.end()) ||
         ((_ss_sft_map.find(def_node->Child(0)->Id()) != _ss_sft_map.end()) &&
          (_ss_sft_map[def_node->Child(0)->Id()] != air::base::Null_id)))) {
      NODE_PTR def_expr = cntr->Clone_node_tree(def_node->Child(0));
      // get the parent node of this ld node.
      NODE_PTR parent_node = this->Parent(1);
      // substitute use with def
      for (uint32_t i = 0; i < parent_node->Num_child(); i++) {
        if (parent_node->Child(i)->Id() == node->Id()) {
          parent_node->Set_child(i, def_expr);
        }
      }
      // record old define node, later will be deleted(DSE)
      DEF_SYM_NODE_VEC def_sym_node_vec = this->Get_def_sym_node_vec();
      if (std::find(def_sym_node_vec.begin(), def_sym_node_vec.end(),
                    def_node) == def_sym_node_vec.end()) {
        this->Insert_def_sym_node_vec(def_node);
      }
    }
  }

  //! @brief delete all dead store node after partial copy propagation
  void Dead_store_elimination() {
    DEF_SYM_NODE_VEC def_sym_node_vec = this->Get_def_sym_node_vec();
    for (DEF_SYM_NODE_VEC::iterator it = def_sym_node_vec.begin();
         it != def_sym_node_vec.end(); ++it) {
      NODE_PTR  def_node  = *it;
      STMT_LIST stmt_list = STMT_LIST::Enclosing_list(def_node->Stmt());
      stmt_list.Remove(def_node->Stmt());
    }
  }

  NODE_PTR Store_and_load_new_preg(NODE_PTR input) {
    CONTAINER* cntr = this->Ssa_cntr()->Container();

    PREG_PTR input_preg = cntr->Parent_func_scope()->New_preg(input->Rtype());
    STMT_PTR st_stmt    = cntr->New_stp(input, input_preg, input->Spos());
    this->Prepend(st_stmt);

    NODE_PTR new_input = cntr->New_ldp(input_preg, input->Spos());
    return new_input;
  }

private:
  VECTOR_CTX&             _ctx;
  const DRIVER_CTX*       _driver_ctx;
  const VECTOR_CONFIG&    _config;
  air::opt::SSA_CONTAINER _ssa_cntr;
  air::opt::SSA_BUILDER   _ssa_builder;
  SS_SFT_MAP              _ss_sft_map;
  DEF_SYM_NODE_VEC        _def_sym_node_vec;
  NODE_ID                 _cur_ss_id = air::base::Null_id;
};

//! @brief CORE handler
class SELECTIVE_STRIDED_SLICE_CORE_HANDLER : public air::core::DEFAULT_HANDLER {
public:
  template <typename RETV, typename VISITOR>
  RETV Handle_st(VISITOR* visitor, NODE_PTR node) {
    visitor->template Visit<RETV>(node->Child(0));
    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_stp(VISITOR* visitor, NODE_PTR node) {
    visitor->template Visit<RETV>(node->Child(0));
    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ld(VISITOR* visitor, NODE_PTR node) {
    // input is the parameter of the function, do not process ld "input" here.
    if (!node->Has_sym() ||
        (strncmp(node->Addr_datum()->Name()->Char_str(), "input", 5) != 0)) {
      visitor->Context().Partial_copy_prop(node);
    }
    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ldp(VISITOR* visitor, NODE_PTR node) {
    return Handle_ld<RETV, VISITOR>(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_retv(VISITOR* visitor, NODE_PTR node) {
    AIR_ASSERT_MSG(node->Num_child() == 1, "retv should only contains 1 child");
    visitor->Context().Dead_store_elimination();
    return RETV();
  }

};  // SELECTIVE_STRIDED_SLICE_CORE_HANDLER

}  // namespace vector

}  // namespace nn

#endif  // NN_VECTOR_SELECTIVE_STRIDED_SLICE_CTX_H
