//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/sihe/tensor2sihe_impl.h"

#include <cmath>
#include <cstddef>
#include <string>
#include <vector>

#include "air/base/container.h"
#include "air/base/container_decl.h"
#include "air/base/node.h"
#include "air/base/ptr_wrapper.h"
#include "air/base/st.h"
#include "air/base/st_decl.h"
#include "air/base/st_enum.h"
#include "air/base/st_sym.h"
#include "air/util/debug.h"
#include "fhe/core/lower_ctx.h"
#include "fhe/sihe/sihe_gen.h"
#include "fhe/util/app_composite_poly.h"

namespace fhe {
namespace sihe {
using namespace fhe::util;

#define SMALL_COEFF_VALUE 1.0E-30

enum class RELU_FUNC_INFO {
  FUNC_NAME        = 0,
  INIT_FORMAL_NAME = 1,
  NORM_FORMAL_NAME = 2,
  ENTRY_NAME       = 3,
  TMP_PREFIX       = 4,
  END              = 5,
};

static const char* Relu_func_info[static_cast<uint32_t>(RELU_FUNC_INFO::END)] =
    {"App_relu", "input_ct0", "input_ct1", "App_relu", "_relu_tmp"};

ADDR_DATUM_PTR APP_RELU_FUNC_GEN::Gen_tmp_for_node(NODE_PTR    node,
                                                   const SPOS& spos) {
  const char* tmp_prefix =
      Relu_func_info[static_cast<uint32_t>(RELU_FUNC_INFO::TMP_PREFIX)];
  std::string    tmp_name(tmp_prefix + std::to_string(node->Id().Value()));
  ADDR_DATUM_PTR tmp_var =
      Func_scope()->New_var(node->Rtype(), tmp_name.c_str(), spos);

  STMT_PTR st_tmp = Container()->New_st(node, tmp_var, spos);
  Container()->Stmt_list().Append(st_tmp);
  return tmp_var;
}

NODE_PTR APP_RELU_FUNC_GEN::Gen_leaf_node_poly(
    const util::POLY_TREE_NODE& leaf_node, ADDR_DATUM_PTR arg,
    bool outmost_poly) {
  // current poly_tree_node is leaf node.
  // quotient and remainder polynomial must be null.
  AIR_ASSERT(leaf_node.Get_poly_q() == nullptr);
  AIR_ASSERT(leaf_node.Get_poly_r() == nullptr);

  SIHE_GEN sihe_gen(Container(), Lower_ctx());
  SPOS     spos = Glob_scope()->Unknown_simple_spos();
  NODE_PTR poly_node;

  const POLY_DATA& poly_data = leaf_node.Get_poly_data();
  TYPE_PTR         f64_type = Glob_scope()->Prim_type(PRIMITIVE_TYPE::FLOAT_64);
  uint32_t         deg      = 0U;
  POLY_DATA::POLY_COEFF_CONST_ITER coeff_iter = poly_data.Get_coeff().begin();
  for (; deg <= poly_data.Get_deg(); deg += poly_data.Deg_stride()) {
    long double coeff = *coeff_iter;
    ++coeff_iter;
    // coefficient is too small to impact value of polynomial
    if (fabs(coeff) < SMALL_COEFF_VALUE) {
      continue;
    }

    // coefficient of outmost polynomial is halved to merge (0.5*x).
    if (outmost_poly) {
      coeff *= 0.5;
    }

    // gen ldc coeff
    CONSTANT_PTR cst_coeff =
        Glob_scope()->New_const(CONSTANT_KIND::FLOAT, f64_type, coeff);
    NODE_PTR item_node = Container()->New_ldc(cst_coeff, spos);

    if (outmost_poly) {
      // gen item of outmost polynomial
      ADDR_DATUM_PTR formal0   = Func_scope()->Formal(0);
      NODE_PTR       ld_formal = Container()->New_ld(formal0, spos);
      item_node                = sihe_gen.Gen_mul(ld_formal, item_node, spos);
    }
    // gen item of inner polynomial item = coeff * poly_arg^deg
    if (deg > 0) {
      NODE_PTR load_tmp = Container()->New_ld(Precompute_item()[deg], spos);
      item_node         = sihe_gen.Gen_mul(load_tmp, item_node, spos);
    }

    // accumulate item into polynomial
    poly_node = (poly_node == NODE_PTR())
                    ? item_node
                    : sihe_gen.Gen_add(item_node, poly_node, spos);
  }
  return poly_node;
}

NODE_PTR APP_RELU_FUNC_GEN::Gen_intermediate_node_poly(
    const util::POLY_TREE_NODE& inter_node, ADDR_DATUM_PTR arg,
    bool outmost_poly) {
  // tree_node is intermediate node.
  // current polynomial must have non-null quotient polynomial.
  // current polynomial = divisor * quotient + remainder.
  const POLY_TREE_NODE* poly_q = inter_node.Get_poly_q();
  AIR_ASSERT(poly_q != nullptr);

  SIHE_GEN sihe_gen(Container(), Lower_ctx());
  SPOS     spos = Glob_scope()->Unknown_simple_spos();
  // 1. gen divisor
  ADDR_DATUM_PTR pow2_item_tmp = Pow2_item()[inter_node.Get_divisor_pow()];
  NODE_PTR       divisor       = Container()->New_ld(pow2_item_tmp, spos);

  // 2. gen quotient polynomial
  NODE_PTR quotient = Gen_sub_poly(*poly_q, arg, outmost_poly);
  // gen divisor * quotient
  NODE_PTR poly_node;
  if (quotient != NODE_PTR()) {
    poly_node = sihe_gen.Gen_mul(divisor, quotient, spos);
  }

  // 3. gen remainder polynomial
  const POLY_TREE_NODE* poly_r = inter_node.Get_poly_r();
  AIR_ASSERT(poly_r != nullptr);
  NODE_PTR remainder = Gen_sub_poly(*poly_r, arg, outmost_poly);
  // gen divisor * quotient + remainder
  if (remainder != NODE_PTR()) {
    poly_node = (poly_node == NODE_PTR())
                    ? remainder
                    : sihe_gen.Gen_add(poly_node, remainder, spos);
  }
  return poly_node;
}

NODE_PTR APP_RELU_FUNC_GEN::Gen_sub_poly(const POLY_TREE_NODE& tree_node,
                                         ADDR_DATUM_PTR        arg,
                                         bool                  outmost_poly) {
  const POLY_TREE_NODE* poly_q = tree_node.Get_poly_q();
  NODE_PTR              poly_node;
  if (poly_q != nullptr) {
    // quotient polynomial is non-null.
    // current poly_tree_node must be intermediate node.
    poly_node = Gen_intermediate_node_poly(tree_node, arg, outmost_poly);
  } else {
    // quotient polynomial is null, current poly_tree_node must be leaf node.
    // remainder polynomial must be null as well.
    AIR_ASSERT(tree_node.Get_poly_r() == nullptr);
    poly_node = Gen_leaf_node_poly(tree_node, arg, outmost_poly);
  }
  return poly_node;
}

void APP_RELU_FUNC_GEN::Gen_pow_base_precompute_item(uint32_t precompute_deg,
                                                     ADDR_DATUM_PTR arg) {
  SPOS     spos = Glob_scope()->Unknown_simple_spos();
  SIHE_GEN sihe_gen(Container(), Lower_ctx());
  Precompute_item().push_back(ADDR_DATUM_PTR());
  Precompute_item().push_back(arg);

  for (uint32_t deg = 2; deg < precompute_deg; ++deg) {
    ADDR_DATUM_PTR sub_item_a = Precompute_item()[deg / 2];
    ADDR_DATUM_PTR sub_item_b =
        (deg % 2 == 0) ? sub_item_a : Precompute_item()[(deg + 1) / 2];
    NODE_PTR       ld_item_a = Container()->New_ld(sub_item_a, spos);
    NODE_PTR       ld_item_b = Container()->New_ld(sub_item_b, spos);
    NODE_PTR       mul_node  = sihe_gen.Gen_mul(ld_item_a, ld_item_b, spos);
    ADDR_DATUM_PTR tmp_var   = Gen_tmp_for_node(mul_node, spos);
    Precompute_item().push_back(tmp_var);
  }
}

void APP_RELU_FUNC_GEN::Gen_pow_base_pow2_item(uint32_t       poly_deg,
                                               ADDR_DATUM_PTR arg) {
  SPOS     spos = Glob_scope()->Unknown_simple_spos();
  SIHE_GEN sihe_gen(Container(), Lower_ctx());
  for (uint32_t deg = 1U; deg <= poly_deg; deg <<= 1U) {
    if (deg < Precompute_item().size()) {
      Pow2_item().push_back(Precompute_item()[deg]);
      continue;
    }
    ADDR_DATUM_PTR sub_item  = Pow2_item().back();
    NODE_PTR       ld_item_a = Container()->New_ld(sub_item, spos);
    NODE_PTR       ld_item_b = Container()->New_ld(sub_item, spos);
    NODE_PTR       mul_node  = sihe_gen.Gen_mul(ld_item_a, ld_item_b, spos);
    ADDR_DATUM_PTR tmp_var   = Gen_tmp_for_node(mul_node, spos);
    Pow2_item().push_back(tmp_var);
  }
}

void APP_RELU_FUNC_GEN::Gen_chebyshev_base_precompute_item(
    uint32_t precompute_deg, ADDR_DATUM_PTR arg) {
  SPOS     spos = Glob_scope()->Unknown_simple_spos();
  SIHE_GEN sihe_gen(Container(), Lower_ctx());
  Precompute_item().push_back(ADDR_DATUM_PTR());
  Precompute_item().push_back(arg);

  for (uint32_t deg = 2; deg < precompute_deg; ++deg) {
    // for chebyshev polynomial T_deg:
    // if deg is even, set n = deg/2, then T_deg = 2 * T_n * T_n - 1;
    // if deg is odd, set n = deg/2, m = n+1, then T_deg = 2 * T_n * T_m - T_1
    ADDR_DATUM_PTR sub_item_a = Precompute_item()[deg / 2];
    NODE_PTR       ld_item_a  = Container()->New_ld(sub_item_a, spos);
    ADDR_DATUM_PTR sub_item_b =
        (deg % 2 == 0) ? sub_item_a : Precompute_item()[(deg + 1) / 2];
    NODE_PTR ld_item_b = Container()->New_ld(sub_item_b, spos);

    // T_n * T_m
    NODE_PTR       mul_node   = sihe_gen.Gen_mul(ld_item_a, ld_item_b, spos);
    ADDR_DATUM_PTR tmp_of_mul = Gen_tmp_for_node(mul_node, spos);

    // 2 * T_n * T_m
    NODE_PTR ld_mul_tmp_a = Container()->New_ld(tmp_of_mul, spos);
    NODE_PTR ld_mul_tmp_b = Container()->New_ld(tmp_of_mul, spos);
    NODE_PTR double_mul_res =
        sihe_gen.Gen_add(ld_mul_tmp_a, ld_mul_tmp_b, spos);

    // 2 * T_n * T_m - T_(m-n)
    TYPE_PTR f32_type = Glob_scope()->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
    CONST_CONSTANT_PTR minus_1 = Glob_scope()->New_const(
        CONSTANT_KIND::FLOAT, f32_type, (long double)(-1.0));
    NODE_PTR sub_term = Container()->New_ldc(minus_1, spos);
    if (sub_item_a != sub_item_b) {
      NODE_PTR ld_arg = Container()->New_ld(arg, spos);
      sub_term        = sihe_gen.Gen_mul(ld_arg, sub_term, spos);
    }
    NODE_PTR       sub_node = sihe_gen.Gen_add(double_mul_res, sub_term, spos);
    ADDR_DATUM_PTR tmp_of_term = Gen_tmp_for_node(sub_node, spos);
    Precompute_item().push_back(tmp_of_term);
  }
}

void APP_RELU_FUNC_GEN::Gen_chebyshev_base_pow2_item(uint32_t       poly_deg,
                                                     ADDR_DATUM_PTR arg) {
  SPOS     spos = Glob_scope()->Unknown_simple_spos();
  SIHE_GEN sihe_gen(Container(), Lower_ctx());
  for (uint32_t deg = 1U; deg <= poly_deg; deg <<= 1U) {
    if (deg < Precompute_item().size()) {
      Pow2_item().push_back(Precompute_item()[deg]);
      continue;
    }

    // for Chebyshev polynomial: T_2n = 2 * T_n * T_n - 1
    // T_n * T_n
    ADDR_DATUM_PTR sub_item      = Pow2_item().back();
    NODE_PTR       ld_item_a     = Container()->New_ld(sub_item, spos);
    NODE_PTR       ld_item_b     = Container()->New_ld(sub_item, spos);
    NODE_PTR       mul_node      = sihe_gen.Gen_mul(ld_item_a, ld_item_b, spos);
    ADDR_DATUM_PTR tmp_of_square = Gen_tmp_for_node(mul_node, spos);

    // 2 * T_n * T_n
    NODE_PTR ld_tmp_a = Container()->New_ld(tmp_of_square, spos);
    NODE_PTR ld_tmp_b = Container()->New_ld(tmp_of_square, spos);
    NODE_PTR add_node = sihe_gen.Gen_add(ld_tmp_a, ld_tmp_b, spos);

    // 2 * T_n * T_n - 1
    TYPE_PTR f32_type = Glob_scope()->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
    CONST_CONSTANT_PTR minus_1 = Glob_scope()->New_const(
        CONSTANT_KIND::FLOAT, f32_type, (long double)(-1.0));
    NODE_PTR       ldc_minus_1 = Container()->New_ldc(minus_1, spos);
    NODE_PTR       sub_node    = sihe_gen.Gen_add(add_node, ldc_minus_1, spos);
    ADDR_DATUM_PTR tmp_of_term = Gen_tmp_for_node(sub_node, spos);
    Pow2_item().push_back(tmp_of_term);
  }
}

NODE_PTR APP_RELU_FUNC_GEN::Gen_poly(const util::POLY_TREE_NODE& root_node,
                                     ADDR_DATUM_PTR arg, bool outmost_poly) {
  // 1. precompute low degree and pow-of-two degree items,
  // reuse these items to minimize multiplications
  if (root_node.Get_poly_data().Is_power_basis()) {
    Gen_pow_base_precompute_item(root_node.Get_poly_data().Get_precompute_deg(),
                                 arg);
    Gen_pow_base_pow2_item(root_node.Get_actual_deg(), arg);
  } else {
    AIR_ASSERT(root_node.Get_poly_data().Is_chebyshev_basis());
    Gen_chebyshev_base_precompute_item(
        root_node.Get_poly_data().Get_precompute_deg(), arg);
    Gen_chebyshev_base_pow2_item(root_node.Get_actual_deg(), arg);
  }

  // 2. gen SIHE IR for polynomial
  NODE_PTR poly_node = Gen_sub_poly(root_node, arg, outmost_poly);
  AIR_ASSERT(poly_node != NODE_PTR());

  // clear precomputed items for current polynomial
  Precompute_item().clear();
  Pow2_item().clear();
  return poly_node;
}

void APP_RELU_FUNC_GEN::Gen_func_body() {
  // according to definition of relu, relu(x) = 0.5 * x * (sign(x) + 1).
  // in current impl, sign(x) is approximated via composite polynomial.
  // 1. gen SIHE IR for composite polynomial that approximate (0.5 * x *
  // sign(x))
  const std::vector<POLY_TREE_NODE>& comp_poly_root_node =
      util::APP_SIGN_COMP_POLY().Get_app_sign_comp_poly(
          {App_relu_base_poly_type(), App_relu_mul_depth()});

  CONTAINER*     cont     = Container();
  SPOS           spos     = Glob_scope()->Unknown_simple_spos();
  ADDR_DATUM_PTR formal0  = Func_scope()->Formal(0);
  ADDR_DATUM_PTR poly_arg = Func_scope()->Formal(1);
  SIHE_GEN       sihe_gen(cont, Lower_ctx());
  TYPE_PTR       f32_type = Glob_scope()->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
  for (size_t poly_id = 0; poly_id < comp_poly_root_node.size(); ++poly_id) {
    // 1. gen SIHE IR for component polynomial
    // (0.5 * x) is merged into outmost polynomial to reduce mul_depth
    const POLY_TREE_NODE& root_node = comp_poly_root_node[poly_id];
    bool     outmost_poly = ((poly_id + 1) == comp_poly_root_node.size());
    NODE_PTR inner_poly   = Gen_poly(root_node, poly_arg, outmost_poly);

    // 2. store result of polynomial in tmp_var, and reset poly_arg for outter
    // polynomial
    poly_arg = Gen_tmp_for_node(inner_poly, spos);
  }
  // load result of composite polynomial
  NODE_PTR ld_sign_res = cont->New_ld(poly_arg, spos);

  // 2. gen SIHE IR of (0.5 * x)
  CONSTANT_PTR half_cst    = Glob_scope()->New_const(CONSTANT_KIND::FLOAT,
                                                     f32_type, (long double)(0.5));
  NODE_PTR     ld_half_cst = cont->New_ldc(half_cst, spos);
  NODE_PTR     ld_formal   = cont->New_ld(formal0, spos);
  NODE_PTR     half_formal = sihe_gen.Gen_mul(ld_formal, ld_half_cst, spos);

  // 3. gen relu(x) = (0.5 * x * sign(x)) + (0.5 * x)
  NODE_PTR       relu_res = sihe_gen.Gen_add(ld_sign_res, half_formal, spos);
  ADDR_DATUM_PTR tmp_var  = Gen_tmp_for_node(relu_res, spos);

  NODE_PTR ld_tmp = cont->New_ld(tmp_var, spos);
  STMT_PTR retv   = cont->New_retv(ld_tmp, spos);
  cont->Stmt_list().Append(retv);
}

FUNC_SCOPE* APP_RELU_FUNC_GEN::Gen_app_relu() {
  GLOB_SCOPE* glob_scope = Glob_scope();
  SPOS        spos       = glob_scope->Unknown_simple_spos();

  const char* func_name =
      Relu_func_info[static_cast<uint32_t>(RELU_FUNC_INFO::FUNC_NAME)];
  FUNC_PTR relu = glob_scope->New_func(func_name, spos);

  const char* formal0_name =
      Relu_func_info[static_cast<uint32_t>(RELU_FUNC_INFO::INIT_FORMAL_NAME)];
  const char* formal1_name =
      Relu_func_info[static_cast<uint32_t>(RELU_FUNC_INFO::NORM_FORMAL_NAME)];
  SIGNATURE_TYPE_PTR sig_type    = glob_scope->New_sig_type();
  TYPE_PTR           cipher_type = Lower_ctx()->Get_cipher_type(glob_scope);
  glob_scope->New_ret_param(cipher_type, sig_type);
  STR_PTR param0_name = glob_scope->New_str(formal0_name);
  glob_scope->New_param(param0_name, cipher_type, sig_type, spos);
  STR_PTR param1_name = glob_scope->New_str(formal1_name);
  glob_scope->New_param(param1_name, cipher_type, sig_type, spos);
  sig_type->Set_complete();

  const char* entry_name =
      Relu_func_info[static_cast<uint32_t>(RELU_FUNC_INFO::ENTRY_NAME)];
  ENTRY_PTR entry_point =
      glob_scope->New_entry_point(sig_type, relu, entry_name, spos);
  relu->Add_entry_point(entry_point->Id());

  FUNC_SCOPE* relu_func = &glob_scope->New_func_scope(relu, true);
  relu_func->Container().New_func_entry(spos);
  Set_func_scope(relu_func);

  Gen_func_body();
  return relu_func;
}

}  // namespace sihe
}  // namespace fhe
