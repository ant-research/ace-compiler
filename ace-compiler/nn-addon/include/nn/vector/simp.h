//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_SIMP_CTX_H
#define NN_VECTOR_SIMP_CTX_H

#include <functional>

#include "air/base/analyze_ctx.h"
#include "air/core/null_handler.h"
#include "air/opt/ssa_build.h"
#include "air/opt/ssa_container.h"
#include "nn/vector/config.h"
#include "nn/vector/vector_ctx.h"
#include "nn/vector/vector_enum.h"
#include "nn/vector/vector_utils.h"

namespace nn {

namespace vector {

using namespace air::base;
using namespace air::driver;

using SSA_VER_ID = air::opt::SSA_VER_ID;

class SIMP_CTX : public ANALYZE_CTX {
public:
  SIMP_CTX(FUNC_SCOPE* func_cope, VECTOR_CTX& ctx, const DRIVER_CTX* driver_ctx,
           const VECTOR_CONFIG& cfg)
      : _ctx(ctx),
        _driver_ctx(driver_ctx),
        _config(cfg),
        _ssa_cntr(&func_cope->Container()),
        _ssa_builder(func_cope, &_ssa_cntr, driver_ctx),
        _dest_node(air::base::Null_ptr) {}

  //! @brief folding scalar operand in multiply op to dest node's weight
  //! constant when dest node is conv/gemm, or folding it to dest node's
  //! constant operand when dest node is mul node.
  //! 1. get scalar operand of multiply op,
  //! 2. get dest node's first weight child or first constant node,
  //! 3. do constant folding,
  //! 4. set back weight or constant after folding to dest node.
  //! @param mul_scalar_node: multiply node whose second child is scalar.
  void Constant_folding(NODE_PTR mul_scalar_node) {
    AIR_ASSERT_MSG(_dest_node != air::base::Null_ptr,
                   "dest node should not be null!");
    AIR_ASSERT_MSG(
        (_dest_node->Opcode() == OPCODE(nn::core::NN, nn::core::GEMM)) ||
            (_dest_node->Opcode() == OPCODE(nn::core::NN, nn::core::CONV)) ||
            (_dest_node->Opcode() ==
             OPCODE(nn::vector::VECTOR, nn::vector::VECTOR_OPCODE::MUL)),
        "folding destination node should only be gemm, conv or vector.MUL "
        "till now");

    CONTAINER*  cntr   = _dest_node->Container();
    GLOB_SCOPE* gscope = cntr->Glob_scope();

    // 1. get scalar operand of multiply op
    NODE_PTR scalar_operand = mul_scalar_node->Child(1);
    AIR_ASSERT_MSG(scalar_operand->Rtype()->Is_prim(),
                   "multiply node's second child must be primitive type");
    float scalar_val = scalar_operand->Const()->Float_literal().Val_as_float();

    if (_dest_node->Opcode() == OPCODE(nn::core::NN, nn::core::GEMM) ||
        _dest_node->Opcode() == OPCODE(nn::core::NN, nn::core::CONV)) {
      // 2. get dest node's first weight child
      NODE_PTR weight_node = _dest_node->Child(1);
      int64_t  channel_out = 0, channel_in_kernel = 0, kernel_height = 0,
              kernel_width = 0;
      Get_array_nchw(weight_node->Rtype(), channel_out, channel_in_kernel,
                     kernel_height, kernel_width);

      const float* cptr = weight_node->Const()->Array_ptr<float>();
      FPVEC        weight(cptr, cptr + channel_out * channel_in_kernel *
                                           kernel_height * kernel_width);
      // 3. folding scalar constant value into dest node's weight operant
      std::transform(weight.begin(), weight.end(), weight.begin(),
                     std::bind(std::multiplies<float>(), std::placeholders::_1,
                               scalar_val));

      // 4. set back weight after constant folding to dest node
      std::string cf_weight_str =
          New_array_name("const_fuse_weight_float",
                         weight_node->Rtype()->Cast_to_arr()->Shape());

      CONSTANT_PTR cf_weight_const = New_array_const(
          gscope, cf_weight_str.c_str(),
          channel_out * channel_in_kernel * kernel_height * kernel_width,
          weight_node->Rtype()->Cast_to_arr()->Elem_type(),
          weight_node->Rtype()->Cast_to_arr()->Shape(), (void*)weight.data(),
          _dest_node->Spos());
      NODE_PTR cf_weight = cntr->New_ldc(cf_weight_const, _dest_node->Spos());

      _dest_node->Set_child(1, cf_weight);
    } else if (_dest_node->Opcode() ==
               OPCODE(nn::vector::VECTOR, nn::vector::VECTOR_OPCODE::MUL)) {
      // 2. get constant operand of dest multiply op
      NODE_PTR dest_const_operand = _dest_node->Child(1);
      AIR_ASSERT_MSG(dest_const_operand->Rtype()->Is_prim(),
                     "till now, only support destination multiply node's "
                     "second child must be primitive type");
      float dest_const_val =
          dest_const_operand->Const()->Float_literal().Val_as_float();

      // 3. folding scalar constant value into dest node's mul operand
      float folding_val = scalar_val * dest_const_val;

      // 4. set back operand after constant folding to dest node
      CONSTANT_PTR folding_const =
          gscope->New_const(CONSTANT_KIND::FLOAT, dest_const_operand->Rtype(),
                            (long double)folding_val);
      NODE_PTR folding_node = cntr->New_ldc(folding_const, _dest_node->Spos());
      _dest_node->Set_child(1, folding_node);
    } else {
      AIR_ASSERT_MSG(
          false,
          "only gemm, conv, VECTOR.MUL is supported for constant folding now");
    }
  }

  //! @brief judge whether it is a scalar multiplication operation
  bool Is_mul_scalar_op(NODE_PTR node) {
    if (((node->Opcode() ==
          OPCODE(nn::vector::VECTOR, nn::vector::VECTOR_OPCODE::MUL)) ||
         (node->Opcode() == OPCODE(nn::core::NN, nn::core::MUL))) &&
        (node->Child(1)->Opcode() == air::core::OPC_LDC) &&
        node->Child(1)->Rtype()->Is_prim()) {
      return true;
    }
    return false;
  }

  //! @brief judge whether it is a destination op
  //! destination ops currenly are conv, gemm and
  //! mul op which multiply variable with constant
  //! these ops are the folding destination.
  bool Is_dest_op(NODE_PTR node) {
    if ((node->Opcode() == OPCODE(nn::core::NN, nn::core::GEMM)) ||
        (node->Opcode() == OPCODE(nn::core::NN, nn::core::CONV)) ||
        (((node->Opcode() ==
           OPCODE(nn::vector::VECTOR, nn::vector::VECTOR_OPCODE::MUL)) ||
          (node->Opcode() == OPCODE(nn::core::NN, nn::core::MUL))) &&
         (node->Child(1)->Opcode() == air::core::OPC_LDC))) {
      return true;
    }
    return false;
  }

  template <typename T>
  T Get_scalar(NODE_PTR node) {
    AIR_ASSERT_MSG(node->Rtype()->Cast_to_arr()->Elem_count() == 1,
                   "multiply node's second child must only contain 1 value");
    T val = node->Const()->Array_elem<T>(0);
    return val;
  }

  //! @brief only used for processing relu which is approximated by x^2*a + x*b
  //! in retraining models, change to a*(x^2 + x*(b/a)), later scalar a will be
  //! folded with other multiply operands
  //! @param node: input node whose first child node is add
  // TODO: hard code this relu pattern
  std::pair<NODE_PTR, bool> Do_identity_transformation(NODE_PTR node) {
    std::pair<NODE_PTR, bool> result(node, false);

    NODE_PTR add_node = node->Child(0);
    AIR_ASSERT_MSG(add_node->Opcode() == OPCODE(nn::core::NN, nn::core::ADD),
                   "relu should be approximated by x^2*a + x*b");
    // left and right may be changed, need to improve.
    if (add_node->Child(0)->Opcode() == OPCODE(nn::core::NN, nn::core::MUL) &&
        add_node->Child(0)->Child(0)->Opcode() ==
            OPCODE(nn::core::NN, nn::core::MUL) &&
        add_node->Child(1)->Opcode() == OPCODE(nn::core::NN, nn::core::MUL)) {
      CONTAINER*  cntr   = _dest_node->Container();
      GLOB_SCOPE* gscope = cntr->Glob_scope();

      TYPE_PTR elem_type = add_node->Rtype()->Cast_to_arr()->Elem_type();

      // get left scalar
      NODE_PTR l_mul_operand = add_node->Child(0)->Child(1);
      float    l_mul_val     = Get_scalar<float>(l_mul_operand);

      // get right scalar
      NODE_PTR r_mul_operand = add_node->Child(1)->Child(1);
      float    r_mul_val     = Get_scalar<float>(r_mul_operand);

      // Floating point division may cause loss of precision.
      CONSTANT_PTR cf_r_mul_val = gscope->New_const(
          CONSTANT_KIND::FLOAT, elem_type, (long double)r_mul_val / l_mul_val);
      NODE_PTR cf_r_mul_node = cntr->New_ldc(cf_r_mul_val, add_node->Spos());

      // get x * (b/a)
      CMPLR_ASSERT(0, "Fix rtype for New_bin_arith.");
      NODE_PTR new_r_node = cntr->New_bin_arith(
          OPCODE(nn::core::NN, nn::core::OPCODE::MUL),
          add_node->Child(1)->Child(0)->Rtype(), add_node->Child(1)->Child(0),
          cf_r_mul_node, add_node->Spos());

      // get x^2
      NODE_PTR new_l_node = add_node->Child(0)->Child(0);

      // new add node: x^2 + x*(b/a)
      add_node->Set_child(0, new_l_node);
      add_node->Set_child(1, new_r_node);

      // get ldc float
      CONSTANT_PTR l_mul_const = gscope->New_const(
          CONSTANT_KIND::FLOAT, elem_type, (long double)l_mul_val);
      NODE_PTR l_mul_node = cntr->New_ldc(l_mul_const, add_node->Spos());

      // get new node: (x^2 + x*(b/a))*a
      CMPLR_ASSERT(0, "Fix rtype for New_bin_arith.");
      NODE_PTR result_node = cntr->New_bin_arith(
          OPCODE(nn::core::NN, nn::core::OPCODE::MUL), add_node->Rtype(),
          add_node, l_mul_node, add_node->Spos());
      // use new result node as child node.
      node->Set_child(0, result_node);
      result.second = true;
    }
    return result;
  }

  //! @brief recursive process tree.
  //! The input must be processed by copy propagation in advance,
  //! after copy propagation, all trees are in reverse order.
  //! Make sure multiply op is canonical, operand0 is variable, operand1 is
  //! constant. From onnx perspective, folding scalar operand in multiply op(var
  //! * const) with subsequent conv/gemm's weights operand or constant(array)
  //! operand in multiply op. Conv/gemm op and multiply constant(array) operand
  //! op will be regarded as constant folding destination. When encounter the
  //! Relu operator here, it means that Relu will be approximated by a
  //! high-order polynomial later. Constant folding cannot cross this kind
  //! approximate implementation of Relu, because this constant needs to
  //! participate in the power operation in Relu. Constant folding region is
  //! divided by the destination ops and the Relu.
  //! @param node: tree root node which is processed by copy propagation.
  void Process_tree(NODE_PTR node) {
    if ((node->Opcode() == air::core::OPC_LD) ||
        (node->Opcode() == air::core::OPC_LDP)) {
      return;
    }

    if (Is_dest_op(node)) {
      _dest_node = node;
    } else if (node->Opcode() == OPCODE(nn::core::NN, nn::core::RELU)) {
      _dest_node = air::base::Null_ptr;
    }

    int      bypass_num = 1;
    NODE_PTR child_node = node->Child(0);
    if (child_node->Opcode() == OPCODE(nn::core::NN, nn::core::ADD)) {
      std::pair<NODE_PTR, bool> node_pair = Do_identity_transformation(node);
      if (node_pair.second) {
        node       = node_pair.first;
        bypass_num = 3;  // TODO: hard code here: bypass relu pattern introduced
                         // in Do_identity_transformation.
      }
      child_node = node->Child(0);
    }

    if (Is_mul_scalar_op(child_node) && (_dest_node != air::base::Null_ptr)) {
      // TODO: need to make sure that reorder op node between mul and dest
      // node with mul node keeps integrity

      // since multiply scalar op will be eliminated due to constant folding
      // change multiply node's first child to its parent node's first child
      node->Set_child(0, child_node->Child(0));

      Constant_folding(child_node);
    }
    for (int i = 0; i < bypass_num; i++) {
      node = node->Child(0);
    }
    Process_tree(node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_node(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT((node->Opcode() == air::core::OPC_ST) ||
               (node->Opcode() == air::core::OPC_STP));
    Process_tree(node->Child(0));
    return RETV();
  }

  // declare access API for VECTOR_CTX
  DECLARE_VECTOR_CTX_ACCESS_API(_ctx)

  // declare access API for VECTOR_CONFIG
  DECLARE_VECTOR_CONFIG_ACCESS_API(_config)

  // declare trace API for detail tracing
  DECLARE_TRACE_DETAIL_API(_config, _driver_ctx)

  void Build_ssa() { _ssa_builder.Perform(); }

  air::opt::SSA_CONTAINER* Ssa_cntr() { return &_ssa_cntr; }

private:
  VECTOR_CTX&             _ctx;
  const DRIVER_CTX*       _driver_ctx;
  const VECTOR_CONFIG&    _config;
  air::opt::SSA_CONTAINER _ssa_cntr;
  air::opt::SSA_BUILDER   _ssa_builder;
  NODE_PTR _dest_node;  // currently is conv, gemm and mul constant(array)
};

//! @brief CORE handler
class SIMP_CORE_HANDLER : public air::core::NULL_HANDLER {
public:
  template <typename RETV, typename VISITOR>
  RETV Handle_st(VISITOR* visitor, air::base::NODE_PTR node) {
    return visitor->Context().template Handle_node<RETV>(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_stp(VISITOR* visitor, air::base::NODE_PTR node) {
    return visitor->Context().template Handle_node<RETV>(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_block(VISITOR* visitor, air::base::NODE_PTR node) {
    return visitor->Context().template Handle_block<RETV>(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_func_entry(VISITOR* visitor, air::base::NODE_PTR node) {
    return visitor->template Visit<RETV>(node->Body_blk());
  }
};  // SIMP_CORE_HANDLER

}  // namespace vector

}  // namespace nn

#endif  // NN_VECTOR_SIMP_CTX_H
