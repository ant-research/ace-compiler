//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_SIHE_TENSOR2SIHE_IMPL_H
#define FHE_SIHE_TENSOR2SIHE_IMPL_H

#include <string>
#include <vector>

#include "air/base/container.h"
#include "air/base/st.h"
#include "air/base/transform_util.h"
#include "fhe/core/lower_ctx.h"
#include "fhe/sihe/sihe_gen.h"
#include "fhe/sihe/vector2sihe_ctx.h"
#include "fhe/util/app_composite_poly.h"
#include "nn/core/invalid_handler.h"
#include "nn/core/opcode.h"

namespace fhe {
namespace sihe {
#define DEFAULT_APP_RELU_MUL_DEPTH 11

//! Generator of approximate ReLU function.
//! index of _precompute_item is degree of item which is precomputed to minimize
//! multiplications in BSGS. index of _pow2_item is the power of item degree.
class APP_RELU_FUNC_GEN {
public:
  APP_RELU_FUNC_GEN(GLOB_SCOPE* glob_scope, core::LOWER_CTX* ctx,
                    uint32_t mul_depth, util::POLY_BASIS_TYPE basis_type)
      : _glob_scope(glob_scope),
        _lower_ctx(ctx),
        _mul_depth(mul_depth),
        _base_type(basis_type) {}

  ~APP_RELU_FUNC_GEN() {}

  //! gen function of approximate ReLU
  FUNC_SCOPE*           Gen_app_relu();
  uint32_t              App_relu_mul_depth() const { return _mul_depth; }
  util::POLY_BASIS_TYPE App_relu_base_poly_type() const { return _base_type; }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  APP_RELU_FUNC_GEN(void);
  APP_RELU_FUNC_GEN(const APP_RELU_FUNC_GEN&);
  APP_RELU_FUNC_GEN& operator=(const APP_RELU_FUNC_GEN&);

  //! store the node in a tmp_var, and return the tmp_var
  ADDR_DATUM_PTR Gen_tmp_for_node(NODE_PTR node, const SPOS& spos);

  //! gen SIHE IR for sub_poly at leaf tree node
  NODE_PTR Gen_leaf_node_poly(const util::POLY_TREE_NODE& leaf_node,
                              ADDR_DATUM_PTR arg, bool outmost_poly);

  //! gen SIHE IR for sub_poly at intermediate tree node
  NODE_PTR Gen_intermediate_node_poly(const util::POLY_TREE_NODE& inter_node,
                                      ADDR_DATUM_PTR arg, bool outmost_poly);

  NODE_PTR Gen_sub_poly(const util::POLY_TREE_NODE& tree_node,
                        ADDR_DATUM_PTR arg, bool outmost_poly);

  //! precompute items of which degree is lower than precompute_deg
  void Gen_pow_base_precompute_item(uint32_t       precompute_deg,
                                    ADDR_DATUM_PTR arg);

  //! precompute chebyshev items of which degree is lower than precompute_deg
  void Gen_chebyshev_base_precompute_item(uint32_t       precompute_deg,
                                          ADDR_DATUM_PTR arg);

  //! precompute items of which degree is power of 2 and lower than poly_deg
  void Gen_pow_base_pow2_item(uint32_t poly_deg, ADDR_DATUM_PTR arg);

  //! precompute chebyshev items of which degree is power of 2 and lower than
  //! poly_deg
  void Gen_chebyshev_base_pow2_item(uint32_t       precompute_deg,
                                    ADDR_DATUM_PTR arg);

  //! gen SIHE IR of polynomial with root POLY_TREE_NODE
  NODE_PTR Gen_poly(const util::POLY_TREE_NODE& root_node, ADDR_DATUM_PTR arg,
                    bool out_most_poly);

  void Gen_func_body();

  GLOB_SCOPE*      Glob_scope() const { return _glob_scope; }
  core::LOWER_CTX* Lower_ctx() const { return _lower_ctx; }
  CONTAINER*       Container() const { return &Func_scope()->Container(); }
  std::vector<ADDR_DATUM_PTR>& Precompute_item() { return _precompute_item; }
  std::vector<ADDR_DATUM_PTR>& Pow2_item() { return _pow2_item; }
  void Set_func_scope(FUNC_SCOPE* func_scope) { _func_scope = func_scope; }

  FUNC_SCOPE* Func_scope() const {
    AIR_ASSERT(_func_scope != nullptr);
    return _func_scope;
  }

  void Set_outmost_poly_tmp(ADDR_DATUM_PTR tmp_var) {
    _outmost_poly_tmp = tmp_var;
  }
  ADDR_DATUM_PTR Get_outmost_poly_tmp() const { return _outmost_poly_tmp; }

  GLOB_SCOPE*                 _glob_scope;  // global scope of app_relu function
  core::LOWER_CTX*            _lower_ctx;  // lower context of app_relu function
  uint32_t                    _mul_depth;  // mul_depth of app_relu
  util::POLY_BASIS_TYPE       _base_type;  // type of base polynomial
  FUNC_SCOPE*                 _func_scope;  // function scope of ReLU function
  std::vector<ADDR_DATUM_PTR> _precompute_item;
  std::vector<ADDR_DATUM_PTR> _pow2_item;
  ADDR_DATUM_PTR _outmost_poly_tmp;  // tmp for (x*y) that used in outmost poly
};

//! impl of handler that lower tensor IR to sihe IR
class TENSOR2SIHE_IMPL : public nn::core::INVALID_HANDLER {
public:
  TENSOR2SIHE_IMPL() = default;
  ~TENSOR2SIHE_IMPL() {}

  template <typename RETV, typename VISITOR>
  RETV Handle_relu(VISITOR* visitor, NODE_PTR node);

private:
  // REQUIRED UNDEFINED UNWANTED methods
  TENSOR2SIHE_IMPL(const TENSOR2SIHE_IMPL&);
  TENSOR2SIHE_IMPL& operator=(const TENSOR2SIHE_IMPL&);
};

template <typename RETV, typename VISITOR>
RETV TENSOR2SIHE_IMPL::Handle_relu(VISITOR* visitor, NODE_PTR node) {
  VALIDATE_UTIL<NUM_CHILD::ONE> util(visitor->Context().Container(), false);
  OPCODE                        v_op(SIHE_DOMAIN::ID, SIHE_OPERATOR::RELU_MSG);
  util.template Initialize<RETV>(visitor, node, v_op);
  NODE_PTR op0 = util.Child(0);
  AIR_ASSERT(op0->Opcode() == air::core::OPC_LD ||
             op0->Opcode() == air::core::OPC_LDP);
  AIR_ASSERT(node->Rtype()->Is_array());

  VECTOR2SIHE_CTX&      ctx       = visitor->Context();
  air::base::CONTAINER* cntr      = ctx.Container();
  core::LOWER_CTX&      lower_ctx = ctx.Lower_ctx();
  SIHE_GEN              sihe_gen(cntr, &lower_ctx);

  SPOS        spos        = node->Spos();
  FUNC_SCOPE* func_scope  = cntr->Parent_func_scope();
  GLOB_SCOPE* glob_scope  = &func_scope->Glob_scope();
  TYPE_PTR    cipher_type = lower_ctx.Get_cipher_type(glob_scope);

  // 1. bootstapping before relu: bs_tmp = SIHE.bootstrap(op0)
  PREG_PTR bs_tmp = func_scope->New_preg(cipher_type);
  {
    TIMING_UTIL timing(ctx, node->Spos(), "FHE::bootstrap", true);
    NODE_PTR    bs_node    = sihe_gen.Gen_bootstrap(op0, spos);
    STMT_PTR    st_bs_node = cntr->New_stp(bs_node, bs_tmp, spos);
    visitor->Context().Prepend(st_bs_node);
    if (ctx.Rt_validate()) {
      NODE_PTR bts_val = cntr->Clone_node_tree(op0);
      NODE_PTR bts_msg = cntr->New_cust_node(fhe::sihe::OPC_BOOTSTRAP_MSG,
                                             node->Rtype(), spos);
      bts_msg->Set_child(0, bts_val);
      uint64_t elem_count = node->Rtype()->Cast_to_arr()->Elem_count();
      TYPE_PTR s32        = glob_scope->Prim_type(PRIMITIVE_TYPE::INT_S32);
      NODE_PTR len        = cntr->New_intconst(s32, elem_count, spos);
      NODE_PTR epi        = cntr->New_intconst(s32, -3, spos);
      visitor->Context().Prepend(cntr->New_validate_stmt(
          cntr->New_ldp(bs_tmp, spos), bts_msg, len, epi, spos));
    }
  }

  // 2. call App_relu: CORE.call App_relu(bs_tmp)
  TIMING_UTIL          timing(ctx, node->Spos(), "FHE::relu", false);
  PREG_PTR             relu_ret_var     = func_scope->New_preg(cipher_type);
  core::FHE_FUNC_INFO& approx_relu_info = lower_ctx.Get_approx_relu_func_info();
  FUNC_SCOPE* approx_relu = approx_relu_info.Get_func_scope(glob_scope);
  if (approx_relu == nullptr) {
    uint32_t relu_mul_depth = (ctx.Relu_mul_depth() > 0)
                                  ? ctx.Relu_mul_depth()
                                  : DEFAULT_APP_RELU_MUL_DEPTH;

    util::POLY_BASIS_TYPE base_poly_type =
        (ctx.Relu_base_type() != (uint32_t)(util::POLY_BASIS_TYPE::POWER))
            ? util::POLY_BASIS_TYPE::CHEBYSHEV
            : util::POLY_BASIS_TYPE::POWER;
    APP_RELU_FUNC_GEN gen(glob_scope, &lower_ctx, relu_mul_depth,
                          base_poly_type);
    approx_relu = gen.Gen_app_relu();
    approx_relu_info.Set_func_id(approx_relu->Owning_func_id());
    approx_relu_info.Set_mul_depth(relu_mul_depth);
  }
  AIR_ASSERT(approx_relu != nullptr);
  ENTRY_PTR entry_point = approx_relu->Owning_func()->Entry_point();
  STMT_PTR  relu_call   = cntr->New_call(entry_point, relu_ret_var, 2, spos);
  NODE_PTR  ld_formal0  = cntr->New_ldp(bs_tmp, spos);
  relu_call->Node()->Set_child(0, ld_formal0);

  NODE_PTR ld_formal1       = cntr->New_ldp(bs_tmp, spos);
  double   relu_value_range = ctx.Relu_value_range(node->Attr("name"));
  ctx.Trace(TRACE_RELU_VR, "Relu range for ", node->Attr("name"), " is [-",
            relu_value_range, ", ", relu_value_range, "]\n");

  // Normalize the ReLU parameter to fit within the [-1,1], if it extends beyond
  // it.
  if (relu_value_range > 1.0) {
    TYPE_PTR     f32_type = glob_scope->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
    CONSTANT_PTR vr_cst   = glob_scope->New_const(
        CONSTANT_KIND::FLOAT, f32_type, (long double)(1. / relu_value_range));
    NODE_PTR ld_cst   = cntr->New_ldc(vr_cst, spos);
    NODE_PTR mul_node = sihe_gen.Gen_mul(ld_formal1, ld_cst, spos);
    relu_call->Node()->Set_child(1, mul_node);
  } else {
    relu_call->Node()->Set_child(1, ld_formal1);
  }

  // set mul_depth attr for App_relu
  const char* attr_name =
      ctx.Lower_ctx().Attr_name(core::FHE_ATTR_KIND::MUL_DEPTH);
  uint32_t attr_val = approx_relu_info.Get_mul_depth();
  relu_call->Node()->Set_attr(attr_name, &attr_val, 1);

  visitor->Context().Prepend(relu_call);
  NODE_PTR ret = cntr->New_ldp(relu_ret_var, spos);
  ret          = util.Finalize(visitor, ret, -5);
  if (ctx.Rt_validate()) {
    NODE_PTR relu_val = cntr->Clone_node_tree(ld_formal0);
    NODE_PTR relu_msg =
        cntr->New_cust_node(fhe::sihe::OPC_RELU_MSG, node->Rtype(), spos);
    relu_msg->Set_child(0, relu_val);
    AIR_ASSERT(node->Rtype()->Is_array());
    std::vector<int64_t> shape = node->Rtype()->Cast_to_arr()->Shape();
    relu_msg->Set_attr("x_shape", shape.data(), shape.size());
    uint64_t elem_count = node->Rtype()->Cast_to_arr()->Elem_count();
    TYPE_PTR s32        = glob_scope->Prim_type(PRIMITIVE_TYPE::INT_S32);
    NODE_PTR len        = cntr->New_intconst(s32, elem_count, spos);
    NODE_PTR epi        = cntr->New_intconst(s32, -3, spos);
    visitor->Context().Append(cntr->New_validate_stmt(
        cntr->New_ldp(relu_ret_var, spos), relu_msg, len, epi, spos));
  }
  return RETV(ret);
}

}  // namespace sihe
}  // namespace fhe

#endif
