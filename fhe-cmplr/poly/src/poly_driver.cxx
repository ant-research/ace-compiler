//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/poly/poly_driver.h"

#include <iostream>

#include "air/base/container.h"
#include "air/base/flatten_ctx.h"
#include "air/base/st.h"
#include "air/core/handler.h"
#include "air/opt/hssa_analyze_ctx.h"
#include "air/opt/hssa_builder.h"
#include "air/opt/hssa_core_handler.h"
#include "air/opt/hssa_func.h"
#include "air/opt/hssa_visitor.h"
#include "air/opt/ssa_build.h"
#include "air/opt/ssapre.h"
#include "ckks2hpoly.h"
#include "ckks2poly.h"
#include "fhe/opt/mdown_hoist_opt.h"
#include "fhe/opt/op_fusion.h"
#include "h2lpoly.h"
#include "nn/core/opcode.h"
#include "nn/vector/handler.h"

using namespace air::base;
using namespace air::opt;
using namespace air::driver;

namespace fhe {

namespace poly {

class POLY_HSSA_BUILDER_CTX : public HSSA_BUILDER_CTX {
public:
  POLY_HSSA_BUILDER_CTX(POLY_CONFIG& config) : _config(config){};
  template <typename RETV, typename VISITOR>
  RETV Handle_node(VISITOR* visitor, air::base::NODE_PTR node) {
    HSSA_CONTAINER* cont = &(Hssa_cont());
    if (node->Is_root()) {
      HSTMT_PTR op_stmt = cont->New_op_stmt(node);
      for (uint32_t i = 0; i < node->Num_child(); ++i) {
        HCR_PTR child = visitor->template Visit<RETV>(node->Child(i));
        op_stmt->Cast_to_op_sr()->Set_kid(i, child->Id());
      }
      Append_stmt(op_stmt);
      return RETV();
    } else {
      OP_HCR_DATA* op_cr = OP_HCR_DATA::Alloc(node->Num_child());
      new (op_cr) OP_HCR_DATA(node);
      std::vector<uint32_t> normed_kids;
      for (uint32_t i = 0; i < node->Num_child(); ++i) {
        HCR_PTR child = visitor->template Visit<RETV>(node->Child(i));
        // op_cr->Set_kid(i, child->Id());
        normed_kids.push_back(child->Id().Value());
      }
      if (_config.Normalize() && Can_normalize(node)) {
        std::sort(normed_kids.begin(), normed_kids.end(),
                  std::less<uint32_t>());
      }
      for (uint32_t kid_idx = 0; kid_idx < normed_kids.size(); kid_idx++) {
        op_cr->Set_kid(kid_idx, HCR_ID(normed_kids[kid_idx]));
      }
      HCR_DATA_PTR op_ptr(op_cr, HCR_ID());
      HCR_PTR      ret = cont->Find_or_new_cr(HCR_PTR(HCR(cont, op_ptr)));
      free(op_cr);
      return ret;
    }
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_unknown_domain(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_node<RETV, VISITOR>(visitor, node);
  }

  bool Can_normalize(NODE_PTR node) {
    air::base::OPCODE opc = node->Opcode();
    if (opc == fhe::poly::OPC_MUL || opc == fhe::poly::ADD) {
      return true;
    }
    return false;
  }

private:
  POLY_CONFIG& _config;
};

using POLY_HSSA_VISITOR =
    air::base::VISITOR<POLY_HSSA_BUILDER_CTX,
                       air::core::HANDLER<HSSA_CORE_HANDLER> >;

GLOB_SCOPE* POLY_DRIVER::Clone_glob(GLOB_SCOPE* src_glob) {
  GLOB_SCOPE* res_glob = new GLOB_SCOPE(src_glob->Id(), true);
  AIR_ASSERT(res_glob != nullptr);
  res_glob->Clone(*src_glob);
  return res_glob;
}

GLOB_SCOPE* POLY_DRIVER::Run(POLY_CONFIG& config, GLOB_SCOPE* glob,
                             core::LOWER_CTX& lower_ctx,
                             DRIVER_CTX*      driver_ctx) {
  GLOB_SCOPE* new_glob = Lower_to_poly(
      config, glob, lower_ctx, config.Lower_to_hpoly() ? HPOLY : SINGLE_POLY);

  if (config.Mdown_hoisting()) {
    AIR_ASSERT(config.Lower_to_hpoly());
    new_glob = Run_mdown_opt(config, new_glob, driver_ctx, &lower_ctx);
  }

  if (config.Mdown_fusion() || config.Mue_fusion()) {
    AIR_ASSERT(config.Lower_to_hpoly());
    new_glob = Run_op_fusion_opt(config, new_glob, driver_ctx, &lower_ctx);
  }

  if (config.Mup_hoisting()) {
    // only support IR from hpoly for now
    AIR_ASSERT(config.Lower_to_hpoly());
    new_glob = Run_mup_opt(config, new_glob, driver_ctx);
  }

  if (config.Lower_to_lpoly()) {
    new_glob = Lower_to_poly(config, new_glob, lower_ctx, LPOLY);
  }

  // std::cout << "After lower_to lpoly\n";
  // new_glob->Print_ir(std::cout);
  return new_glob;
}

GLOB_SCOPE* POLY_DRIVER::Lower_to_poly(POLY_CONFIG& config, GLOB_SCOPE* glob,
                                       core::LOWER_CTX& lower_ctx,
                                       POLY_LAYER       target_layer) {
  // Run flatten before HPOLY and LPOLY
  if (target_layer == HPOLY || target_layer == LPOLY) {
    glob = Flatten(glob);
  }
  GLOB_SCOPE* new_glob = Clone_glob(glob);

  for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
       it != glob->End_func_scope(); ++it) {
    FUNC_SCOPE* func = &(*it);

    FUNC_SCOPE* new_func = &new_glob->New_func_scope(func->Id());
    new_func->Clone(*func);
    CONTAINER& cntr = new_func->Container();

    switch (target_layer) {
      case HPOLY: {
        POLY_LOWER_CTX     ctx(config, &lower_ctx, &cntr);
        CKKS2HPOLY_VISITOR visitor(ctx);
        NODE_PTR           body = func->Container().Entry_node();
        POLY_LOWER_RETV    retv = visitor.Visit<POLY_LOWER_RETV>(body);
        AIR_ASSERT(retv.Num_node() == 1 && retv.Node()->Is_entry());
        new_func->Set_entry_stmt(retv.Node()->Stmt());
        break;
      }
      case LPOLY: {
        POLY_LOWER_CTX  ctx(config, &lower_ctx, &cntr);
        H2LPOLY_VISITOR visitor(ctx);
        NODE_PTR        body = func->Container().Entry_node();
        POLY_LOWER_RETV retv = visitor.Visit<POLY_LOWER_RETV>(body);
        AIR_ASSERT(retv.Num_node() == 1 && retv.Node()->Is_entry());
        new_func->Set_entry_stmt(retv.Node()->Stmt());
        break;
      }
      case SINGLE_POLY: {
        CKKS2POLY_CTX     ctx(config, &lower_ctx, &cntr);
        CKKS2POLY_VISITOR visitor(ctx);
        NODE_PTR          body = func->Container().Entry_node();
        POLY_LOWER_RETV   retv = visitor.Visit<POLY_LOWER_RETV>(body);
        AIR_ASSERT(retv.Num_node() == 1 && retv.Node()->Is_entry());
        new_func->Set_entry_stmt(retv.Node()->Stmt());
        break;
      }
    }
  }
  // new_glob->Print();
  return new_glob;
}

GLOB_SCOPE* POLY_DRIVER::Run_mup_opt(POLY_CONFIG& config, GLOB_SCOPE* glob,
                                     DRIVER_CTX* driver_ctx) {
  std::vector<HSSA_FUNC*> func_vec;
  for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
       it != glob->End_func_scope(); ++it) {
    FUNC_SCOPE* func = &(*it);

    // build hssa function
    POLY_HSSA_BUILDER_CTX build_ctx(config);
    POLY_HSSA_VISITOR     hssa_visitor(build_ctx);
    HSSA_FUNC*            hfunc = new HSSA_FUNC(func);
    hfunc->Build(hssa_visitor);
    func_vec.push_back(hfunc);

    // run ssa pre
    SSAPRE         ssa_pre(EPRE_K, hfunc->Cfg(), driver_ctx);
    SSAPRE_CONFIG& pre_config = ssa_pre.Pre_config();
    pre_config.Set_trace_ir_before_pre(
        config.Is_trace(poly::TRACE_DETAIL::TRACE_IR_BEFORE_MUP));
    pre_config.Set_trace_ir_after_pre(
        config.Is_trace(poly::TRACE_DETAIL::TRACE_IR_AFTER_MUP));
    pre_config.Set_trace_pre_flow(
        config.Is_trace(poly::TRACE_DETAIL::TRACE_MUP_FLOW));

    // add filter for pre
    pre_config.Add_cand_op(
        air::base::OPCODE(fhe::poly::POLYNOMIAL_DID, fhe::poly::PRECOMP));
    // pre_config.Add_cand_op(
    //     air::base::OPCODE(fhe::poly::POLYNOMIAL_DID, fhe::poly::MUL));
    ssa_pre.Run();
  }

  if (func_vec.empty()) return glob;

  // clone glob scope after optimization was performed to make sure
  // all new symbol/types are included in new emit global scope
  GLOB_SCOPE* new_glob = Clone_glob(glob);
  for (auto hfunc : func_vec) {
    hfunc->Emit(new_glob);
    delete hfunc;
  }

  delete glob;
  return new_glob;
}

GLOB_SCOPE* POLY_DRIVER::Flatten(GLOB_SCOPE* glob) {
  GLOB_SCOPE* new_glob = Clone_glob(glob);

  for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
       it != glob->End_func_scope(); ++it) {
    FUNC_SCOPE* func     = &(*it);
    FUNC_SCOPE* new_func = &new_glob->New_func_scope(func->Id());
    new_func->Clone(*func);
    CONTAINER& cntr = new_func->Container();

    auto flatten_func = [](NODE_PTR node) {
      if (node->Domain() == air::core::CORE ||
          node->Opcode() == nn::vector::OPC_SLICE) {
        return false;
      }
      return true;
    };
    FLATTEN_CTX          trav_ctx(&cntr, std::move(flatten_func));
    VISITOR<FLATTEN_CTX> trav(trav_ctx);
    NODE_PTR             entry = func->Container().Entry_node();
    NODE_PTR             retv  = trav.Visit<NODE_PTR>(entry);
    AIR_ASSERT(retv->Is_entry());
    new_func->Set_entry_stmt(retv->Stmt());
  }

  // delete old glob
  delete glob;
  return new_glob;
}

GLOB_SCOPE* POLY_DRIVER::Run_mdown_opt(POLY_CONFIG& config, GLOB_SCOPE* glob,
                                       air::driver::DRIVER_CTX* driver_ctx,
                                       core::LOWER_CTX*         lower_ctx) {
  std::vector<HSSA_FUNC*> func_vec;
  for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
       it != glob->End_func_scope(); ++it) {
    FUNC_SCOPE* func = &(*it);

    // build hssa function

    POLY_HSSA_BUILDER_CTX build_ctx(config);
    POLY_HSSA_VISITOR     hssa_visitor(build_ctx);
    HSSA_FUNC*            hfunc = new HSSA_FUNC(func);
    hfunc->Build(hssa_visitor);
    func_vec.push_back(hfunc);

    if (config.Mdown_hoisting()) {
      MDOWN_HOIST_OPT md_opt(config, driver_ctx, lower_ctx);
      md_opt.Run(hfunc);
    }
  }

  GLOB_SCOPE* new_glob = Clone_glob(glob);
  for (auto hfunc : func_vec) {
    hfunc->Emit(new_glob);
    delete hfunc;
  }

  delete glob;
  return new_glob;
}

GLOB_SCOPE* POLY_DRIVER::Run_op_fusion_opt(POLY_CONFIG&             config,
                                           GLOB_SCOPE*              glob,
                                           air::driver::DRIVER_CTX* driver_ctx,
                                           core::LOWER_CTX*         lower_ctx) {
  std::vector<HSSA_FUNC*> func_vec;
  for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
       it != glob->End_func_scope(); ++it) {
    FUNC_SCOPE* func = &(*it);

    // build hssa function
    POLY_HSSA_BUILDER_CTX build_ctx(config);
    POLY_HSSA_VISITOR     hssa_visitor(build_ctx);
    HSSA_FUNC*            hfunc = new HSSA_FUNC(func);
    hfunc->Build(hssa_visitor);
    func_vec.push_back(hfunc);

    OP_FUSION_OPT op_fusion_opt(config, driver_ctx, lower_ctx);
    if (config.Mdown_fusion()) {
      air::base::OPCODE fused_op = air::base::OPCODE(fhe::poly::POLYNOMIAL_DID,
                                                     fhe::poly::MOD_DOWN_FUSE1);
      air::base::OPCODE op1 =
          air::base::OPCODE(fhe::poly::POLYNOMIAL_DID, fhe::poly::RESCALE);
      air::base::OPCODE op2 =
          air::base::OPCODE(fhe::poly::POLYNOMIAL_DID, fhe::poly::MOD_DOWN);
      OPLIST op_list = {op1, op2};
      op_fusion_opt.Register_rules(0, fused_op, op_list);
    }

    // only Apply on main graph, Ct_encoding has issue for multi function plain
    // reading
    if (config.Mue_fusion()) {
      // && strcmp(func->Owning_func()->Name()->Char_str(), "Main_graph") == 0)
      // {
      air::base::OPCODE fused_op = fhe::poly::OPC_MULP_FAST;
      air::base::OPCODE op1      = fhe::poly::OPC_MUL;
      air::base::OPCODE op2 = air::core::OPC_INVALID;  // for any op matching
      air::base::OPCODE op3 = fhe::ckks::OPC_ENCODE;
      air::base::OPCODE op4 = nn::vector::OPC_SLICE;
      OPLIST            op_list = {op1, op2, op3, op4};
      op_fusion_opt.Register_rules(0, fused_op, op_list);
    }
#if 0
    if (config.Dotprod_fusion()) {
      air::base::OPCODE fused_op = fhe::poly::OPC_MULADD_NOMOD;
      air::base::OPCODE op1      = fhe::poly::OPC_ADD;
      air::base::OPCODE op2 = air::core::OPC_MUL;
      OPLIST            op_list = {op1, op2};
      op_fusion_opt.Register_rules(1, fused_op, op_list);
    }
#endif
    op_fusion_opt.Run(hfunc);
  }
  GLOB_SCOPE* new_glob = Clone_glob(glob);
  for (auto hfunc : func_vec) {
    hfunc->Emit(new_glob);
    delete hfunc;
  }
  // std::cout << "After Mue fusion\n";
  // new_glob->Print_ir(std::cout);

  delete glob;
  return new_glob;
}

}  // namespace poly

}  // namespace fhe
