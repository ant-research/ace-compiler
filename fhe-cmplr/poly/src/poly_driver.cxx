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
#include "fhe/poly/opcode.h"
#include "flatten_util.h"
#include "h2lpoly.h"
#include "hp1tohp2.h"
#include "hpoly_attr_prop.h"
#include "hpoly_verify.h"
#include "nn/core/opcode.h"
#include "nn/vector/handler.h"

using namespace air::base;
using namespace air::driver;
using namespace air::opt;

namespace fhe {

namespace poly {

//! @brief HSSA builder context with operand normalization for SSAPRE
class POLY_HSSA_BUILDER_CTX : public HSSA_BUILDER_CTX {
public:
  POLY_HSSA_BUILDER_CTX(POLY_CONFIG& config) : _config(config) {}

  template <typename RETV, typename VISITOR>
  RETV Handle_node(VISITOR* visitor, air::base::NODE_PTR node) {
    HCONTAINER* cont = &(this->Hssa_cont());
    if (node->Is_root()) {
      HSTMT_PTR op_stmt = cont->New_op_stmt(node);
      for (uint32_t i = 0; i < node->Num_child(); ++i) {
        HEXPR_PTR child = visitor->template Visit<RETV>(node->Child(i));
        op_stmt->Cast_to_nary()->Set_kid(i, child->Id());
      }
      this->Append_stmt(op_stmt);
      return RETV();
    } else {
      OP_DATA* op_expr = OP_DATA::Alloc(node->Num_child());
      new (op_expr) OP_DATA(node);
      std::vector<uint32_t> normed_kids;
      for (uint32_t i = 0; i < node->Num_child(); ++i) {
        HEXPR_PTR child = visitor->template Visit<RETV>(node->Child(i));
        normed_kids.push_back(child->Id().Value());
      }
      if (_config.Normalize_order() && Can_normalize(node)) {
        std::sort(normed_kids.begin(), normed_kids.end(),
                  std::less<uint32_t>());
      }
      for (uint32_t kid_idx = 0; kid_idx < normed_kids.size(); kid_idx++) {
        op_expr->Set_kid(kid_idx, HEXPR_ID(normed_kids[kid_idx]));
      }
      HEXPR_DATA_PTR op_ptr(op_expr, HEXPR_ID());
      HEXPR_PTR ret = cont->Find_or_new_expr(HEXPR_PTR(HEXPR(cont, op_ptr)));
      free(op_expr);
      return ret;
    }
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_unknown_domain(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_node<RETV, VISITOR>(visitor, node);
  }

  bool Can_normalize(NODE_PTR node) {
    air::base::OPCODE opc = node->Opcode();
    if (opc == fhe::poly::OPC_MUL || opc == fhe::poly::OPC_ADD) {
      return true;
    }
    return false;
  }

private:
  POLY_CONFIG& _config;
};

using POLY_HSSA_VISITOR =
    air::base::VISITOR<POLY_HSSA_BUILDER_CTX,
                       air::core::HANDLER<HSSA_CORE_HANDLER>>;

GLOB_SCOPE* POLY_DRIVER::Clone_glob(GLOB_SCOPE* src_glob) {
  GLOB_SCOPE* res_glob = new GLOB_SCOPE(src_glob->Id(), true);
  AIR_ASSERT(res_glob != nullptr);
  res_glob->Clone(*src_glob);
  return res_glob;
}

GLOB_SCOPE* POLY_DRIVER::Run(POLY_CONFIG& config, GLOB_SCOPE* glob,
                             core::LOWER_CTX& lower_ctx,
                             DRIVER_CTX*      driver_ctx) {
  // lower ckks to HPOLY or SPOLY
  GLOB_SCOPE* new_glob = Lower_to_poly(config, glob, driver_ctx, lower_ctx,
                                       config.Lower_to_hpoly() ? HPOLY : SPOLY);

  if (config.Hoist_mdown()) {
    AIR_ASSERT(config.Lower_to_hpoly());
    new_glob = Run_mdown_opt(config, new_glob, driver_ctx, &lower_ctx);
  }

  if (config.Fuse_mdown_rescale()) {
    AIR_ASSERT(config.Lower_to_hpoly());
    new_glob = Run_op_fusion_opt(config, new_glob, driver_ctx, &lower_ctx);
  }

  if (config.Hoist_mdup()) {
    AIR_ASSERT(config.Lower_to_hpoly());
    new_glob = Run_mup_opt(config, new_glob, driver_ctx);
  }

  // continue lower HPOLY operations
  if (config.Lower_to_hpoly() && config.Lower_to_hpoly2()) {
    new_glob = Lower_to_poly(config, new_glob, driver_ctx, lower_ctx, HPOLY_P2);
  }

  // lower HPOLY to LPOLY
  if (config.Lower_to_lpoly()) {
    new_glob = Lower_to_poly(config, new_glob, driver_ctx, lower_ctx, LPOLY);
  }

  return new_glob;
}

GLOB_SCOPE* POLY_DRIVER::Lower_to_poly(POLY_CONFIG& config, GLOB_SCOPE* glob,
                                       DRIVER_CTX*      driver_ctx,
                                       core::LOWER_CTX& lower_ctx,
                                       POLY_LAYER       target_layer) {
  // Run flatten before HPOLY and LPOLY.
  // When KSW passes are enabled, use aggressive flatten (all non-CORE)
  // so the HSSA builder can process all variable references including
  // loop IVs nested inside ild(array(ldca, ld)) patterns.
  if (target_layer == HPOLY &&
      (config.Hoist_mdown() || config.Fuse_mdown_rescale() ||
       config.Hoist_mdup())) {
    glob = Flatten_for_hssa(glob);
  } else if (target_layer == HPOLY || target_layer == HPOLY_P2 ||
             target_layer == LPOLY) {
    glob = Run_flatten(glob, target_layer);
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
        POLY_LOWER_CTX ctx(config, driver_ctx, &lower_ctx, &cntr);
        ctx.Trace(IR_LOWER, "#### IR trace before HPOLY_P1\n");
        ctx.Trace_obj(IR_LOWER, func);
        CKKS2HPOLY_VISITOR visitor(ctx);
        NODE_PTR           body = func->Container().Entry_node();
        POLY_LOWER_RETV    retv = visitor.Visit<POLY_LOWER_RETV>(body);
        AIR_ASSERT(retv.Num_node() == 1 && retv.Node()->Is_entry());
        new_func->Set_entry_stmt(retv.Node()->Stmt());
        Verify_hpoly(config, new_glob, &lower_ctx);
        ctx.Trace(IR_LOWER, "#### IR trace after HPOLY_P1\n");
        ctx.Trace_obj(IR_LOWER, new_func);
        break;
      }
      case HPOLY_P2: {
        POLY_LOWER_CTX ctx(config, driver_ctx, &lower_ctx, &cntr);
        ctx.Trace(IR_LOWER, "#### IR trace before HPOLY_P2\n");
        ctx.Trace_obj(IR_LOWER, func);
        HP1TOHP2_VISITOR visitor(ctx);
        NODE_PTR         body = func->Container().Entry_node();
        POLY_LOWER_RETV  retv = visitor.Visit<POLY_LOWER_RETV>(body);
        AIR_ASSERT(retv.Num_node() == 1 && retv.Node()->Is_entry());
        new_func->Set_entry_stmt(retv.Node()->Stmt());
        Verify_hpoly(config, new_glob, &lower_ctx);
        ctx.Trace(IR_LOWER, "#### IR trace after HPOLY_P2\n");
        ctx.Trace_obj(IR_LOWER, new_func);
        break;
      }
      case LPOLY: {
        POLY_LOWER_CTX ctx(config, driver_ctx, &lower_ctx, &cntr);
        ctx.Trace(IR_LOWER, "#### IR trace before LPOLY\n");
        ctx.Trace_obj(IR_LOWER, func);
        H2LPOLY_VISITOR visitor(ctx);
        NODE_PTR        body = func->Container().Entry_node();
        POLY_LOWER_RETV retv = visitor.Visit<POLY_LOWER_RETV>(body);
        AIR_ASSERT(retv.Num_node() == 1 && retv.Node()->Is_entry());
        new_func->Set_entry_stmt(retv.Node()->Stmt());
        ctx.Trace(IR_LOWER, "#### IR trace after LPOLY\n");
        ctx.Trace_obj(IR_LOWER, new_func);
        break;
      }
      case SPOLY: {
        CKKS2POLY_CTX     ctx(config, &lower_ctx, &cntr);
        CKKS2POLY_VISITOR visitor(ctx);
        NODE_PTR          body = func->Container().Entry_node();
        POLY_LOWER_RETV   retv = visitor.Visit<POLY_LOWER_RETV>(body);
        AIR_ASSERT(retv.Num_node() == 1 && retv.Node()->Is_entry());
        new_func->Set_entry_stmt(retv.Node()->Stmt());
        break;
      }
      default:
        AIR_ASSERT_MSG(false, "unsupported POLY lower level");
    }
  }
  if (config.Prop_attr() &&
      (target_layer == HPOLY || target_layer == HPOLY_P2)) {
    Run_attr_prop(config, new_glob, driver_ctx, lower_ctx);
  }

  delete glob;
  return new_glob;
}

GLOB_SCOPE* POLY_DRIVER::Run_mdown_opt(POLY_CONFIG& config, GLOB_SCOPE* glob,
                                       DRIVER_CTX*      driver_ctx,
                                       core::LOWER_CTX* lower_ctx) {
  std::vector<HSSA_FUNC*> func_vec;
  for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
       it != glob->End_func_scope(); ++it) {
    FUNC_SCOPE* func = &(*it);

    POLY_HSSA_BUILDER_CTX build_ctx(config);
    POLY_HSSA_VISITOR     hssa_visitor(build_ctx);
    HSSA_FUNC*            hfunc = new HSSA_FUNC(func);
    hfunc->Build(hssa_visitor);
    func_vec.push_back(hfunc);

    MDOWN_HOIST_OPT md_opt(config, driver_ctx, lower_ctx);
    md_opt.Run(hfunc);
  }

  GLOB_SCOPE* new_glob = Clone_glob(glob);
  for (auto hfunc : func_vec) {
    hfunc->Emit(new_glob);
    delete hfunc;
  }

  delete glob;
  return new_glob;
}

GLOB_SCOPE* POLY_DRIVER::Run_op_fusion_opt(POLY_CONFIG&     config,
                                           GLOB_SCOPE*      glob,
                                           DRIVER_CTX*      driver_ctx,
                                           core::LOWER_CTX* lower_ctx) {
  std::vector<HSSA_FUNC*> func_vec;
  for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
       it != glob->End_func_scope(); ++it) {
    FUNC_SCOPE* func = &(*it);

    POLY_HSSA_BUILDER_CTX build_ctx(config);
    POLY_HSSA_VISITOR     hssa_visitor(build_ctx);
    HSSA_FUNC*            hfunc = new HSSA_FUNC(func);
    hfunc->Build(hssa_visitor);
    func_vec.push_back(hfunc);

    OP_FUSION_OPT op_fusion_opt(config, driver_ctx, lower_ctx);
    if (config.Fuse_mdown_rescale()) {
      air::base::OPCODE fused_op = air::base::OPCODE(
          fhe::poly::POLYNOMIAL_DID, fhe::poly::OPCODE::MOD_DOWN_RESCALE);
      air::base::OPCODE op1     = air::base::OPCODE(fhe::poly::POLYNOMIAL_DID,
                                                    fhe::poly::OPCODE::RESCALE);
      air::base::OPCODE op2     = air::base::OPCODE(fhe::poly::POLYNOMIAL_DID,
                                                    fhe::poly::OPCODE::MOD_DOWN);
      OPLIST            op_list = {op1, op2};
      op_fusion_opt.Register_rules(0, fused_op, op_list);
    }
    op_fusion_opt.Run(hfunc);
  }

  GLOB_SCOPE* new_glob = Clone_glob(glob);
  for (auto hfunc : func_vec) {
    hfunc->Emit(new_glob);
    delete hfunc;
  }

  delete glob;
  return new_glob;
}

GLOB_SCOPE* POLY_DRIVER::Run_mup_opt(POLY_CONFIG& config, GLOB_SCOPE* glob,
                                     DRIVER_CTX* driver_ctx) {
  std::vector<HSSA_FUNC*> func_vec;
  for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
       it != glob->End_func_scope(); ++it) {
    FUNC_SCOPE* func = &(*it);

    POLY_HSSA_BUILDER_CTX build_ctx(config);
    POLY_HSSA_VISITOR     hssa_visitor(build_ctx);
    HSSA_FUNC*            hfunc = new HSSA_FUNC(func);
    hfunc->Build(hssa_visitor);
    func_vec.push_back(hfunc);

    SSAPRE         ssa_pre(EPRE_K, hfunc->Cfg(), driver_ctx);
    SSAPRE_CONFIG& pre_config = ssa_pre.Pre_config();
    pre_config.Set_trace_ir_before_pre(config.Is_trace(poly::IR_BEFORE_MUP));
    pre_config.Set_trace_ir_after_pre(config.Is_trace(poly::IR_AFTER_MUP));
    pre_config.Set_trace_pre_flow(config.Is_trace(poly::MUP_FLOW));

    pre_config.Add_cand_op(
        air::base::OPCODE(fhe::poly::POLYNOMIAL_DID, fhe::poly::PRECOMP));
    ssa_pre.Run();
  }

  if (func_vec.empty()) return glob;

  GLOB_SCOPE* new_glob = Clone_glob(glob);
  for (auto hfunc : func_vec) {
    hfunc->Emit(new_glob);
    delete hfunc;
  }

  delete glob;
  return new_glob;
}

GLOB_SCOPE* POLY_DRIVER::Flatten_for_hssa(GLOB_SCOPE* glob) {
  GLOB_SCOPE* new_glob = Clone_glob(glob);

  for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
       it != glob->End_func_scope(); ++it) {
    FUNC_SCOPE* func     = &(*it);
    FUNC_SCOPE* new_func = &new_glob->New_func_scope(func->Id());
    new_func->Clone(*func);
    CONTAINER& cntr = new_func->Container();

    // Flatten all non-CORE domain nodes for HSSA building compatibility.
    // The HSSA/SSA builder requires flat IR where each operation is a
    // separate statement so SSA versions can be assigned to all variables.
    // ILD and ARRAY must also be flattened because the SSA builder's
    // Handle_ild skips array index children when base is LDCA.
    auto flatten_func = [](NODE_PTR node) {
      if (node->Opcode() == air::core::OPC_ILD) {
        return true;
      }
      if (node->Domain() == air::core::CORE ||
          node->Opcode() == nn::vector::OPC_SLICE) {
        return false;
      }
      return true;
    };
    FLATTEN_CTX<FLATTEN_UTIL>          trav_ctx(&cntr, std::move(flatten_func));
    VISITOR<FLATTEN_CTX<FLATTEN_UTIL>> trav(trav_ctx);
    NODE_PTR                           entry = func->Container().Entry_node();
    NODE_PTR                           retv  = trav.Visit<NODE_PTR>(entry);
    AIR_ASSERT(retv->Is_entry());
    new_func->Set_entry_stmt(retv->Stmt());
  }

  delete glob;
  return new_glob;
}

GLOB_SCOPE* POLY_DRIVER::Run_flatten(GLOB_SCOPE* glob, POLY_LAYER tgt_layer) {
  GLOB_SCOPE* new_glob = Clone_glob(glob);

  for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
       it != glob->End_func_scope(); ++it) {
    FUNC_SCOPE* func     = &(*it);
    FUNC_SCOPE* new_func = &new_glob->New_func_scope(func->Id());
    new_func->Clone(*func);
    CONTAINER& cntr = new_func->Container();

    auto flatten_ckks = [](NODE_PTR node) {
      if (node->Domain() == fhe::ckks::CKKS_DOMAIN::ID) {
        air::base::OPCODE opcode = node->Opcode();
        if (opcode == fhe::ckks::OPC_ADD || opcode == fhe::ckks::OPC_SUB ||
            opcode == fhe::ckks::OPC_MUL || opcode == fhe::ckks::OPC_NEG ||
            opcode == fhe::ckks::OPC_ENCODE ||
            opcode == fhe::ckks::OPC_RESCALE ||
            opcode == fhe::ckks::OPC_ROTATE ||
            opcode == fhe::ckks::OPC_ROTATE_BATCH ||
            opcode == fhe::ckks::OPC_UPSCALE ||
            opcode == fhe::ckks::OPC_MODSWITCH ||
            opcode == fhe::ckks::OPC_RELIN ||
            opcode == fhe::ckks::OPC_BOOTSTRAP ||
            opcode == fhe::ckks::OPC_RAISE_MOD ||
            opcode == fhe::ckks::OPC_CONJUGATE ||
            opcode == fhe::ckks::OPC_MUL_MONO ||
            opcode == fhe::ckks::OPC_BOOTSTRAP_COEFFS_TO_SLOTS ||
            opcode == fhe::ckks::OPC_BOOTSTRAP_EVAL_MOD ||
            opcode == fhe::ckks::OPC_BOOTSTRAP_SLOTS_TO_COEFFS) {
          return true;
        }
        return false;
      }
      // flatten iload to make sure the there is an preg generated
      // for initialize the memory
      if (node->Opcode() == air::core::OPC_ILD) {
        return true;
      }
      return false;
    };
    auto flatten_poly = [](NODE_PTR node) {
      if (node->Domain() == fhe::poly::POLYNOMIAL_DID) {
        // Do not flatten HW_* ops that are SET_COEFFS value; IR2C emits
        // Coeffs(...) as first arg only when value is direct HW_*.
        air::base::OPCODE opc = node->Opcode();
        if (opc == fhe::poly::OPC_HW_MODADD ||
            opc == fhe::poly::OPC_HW_MODSUB ||
            opc == fhe::poly::OPC_HW_MODMUL ||
            opc == fhe::poly::OPC_HW_ROTATE) {
          return false;
        }
        return true;
      }
      return false;
    };
    FLATTEN_CTX<FLATTEN_UTIL> trav_ctx(
        &cntr, std::move(tgt_layer == HPOLY ? flatten_ckks : flatten_poly));
    VISITOR<FLATTEN_CTX<FLATTEN_UTIL>> trav(trav_ctx);
    NODE_PTR                           entry = func->Container().Entry_node();
    NODE_PTR                           retv  = trav.Visit<NODE_PTR>(entry);
    AIR_ASSERT(retv->Is_entry());
    new_func->Set_entry_stmt(retv->Stmt());
  }

  // delete old glob
  delete glob;
  return new_glob;
}

void POLY_DRIVER::Verify_hpoly(POLY_CONFIG& config, GLOB_SCOPE* glob,
                               core::LOWER_CTX* lower_ctx) {
  if (!config.Verify_ir()) return;
  for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
       it != glob->End_func_scope(); ++it) {
    FUNC_SCOPE*               func = &(*it);
    HPOLY_VERIFY_CTX          verify_ctx(lower_ctx);
    VISITOR<HPOLY_VERIFY_CTX> trav(verify_ctx);
    NODE_PTR                  entry = func->Container().Entry_node();
    trav.Visit<void>(entry);
  }
}

void POLY_DRIVER::Run_attr_prop(POLY_CONFIG& config, GLOB_SCOPE* glob,
                                DRIVER_CTX*      driver_ctx,
                                core::LOWER_CTX& lower_ctx) {
  for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
       it != glob->End_func_scope(); ++it) {
    FUNC_SCOPE* func = &(*it);
    ATTR_PGTR   prop(config, driver_ctx, &lower_ctx, func);
    prop.Run();
  }
}

}  // namespace poly

}  // namespace fhe
