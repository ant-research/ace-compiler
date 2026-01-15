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
#include "ckks2hpoly.h"
#include "ckks2poly.h"
#include "flatten_util.h"
#include "h2lpoly.h"
#include "hp1tohp2.h"
#include "hpoly_attr_prop.h"
#include "hpoly_verify.h"
#include "nn/core/opcode.h"
#include "nn/vector/handler.h"

// #include "air/opt/hssa_analyze_ctx.h"
// #include "air/opt/hssa_builder.h"
// #include "air/opt/hssa_core_handler.h"
// #include "air/opt/hssa_func.h"
// #include "air/opt/hssa_visitor.h"
// #include "air/opt/ssa_build.h"
// #include "air/opt/ssapre.h"
// #include "fhe/opt/mdown_hoist_opt.h"
// #include "fhe/opt/op_fusion.h"

using namespace air::base;
using namespace air::driver;
// using namespace air::opt;

namespace fhe {

namespace poly {
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

#if HPAO
  if (config.Mdown_hoisting()) {
    new_glob = Run_mdown_opt(config, new_glob, driver_ctx, &lower_ctx);
  }

  if (config.Op_fusion()) {
    new_glob = Run_op_fusion_opt(config, new_glob, driver_ctx, &lower_ctx);
  }

  if (config.Mup_hoisting()) {
    new_glob = Run_mup_opt(config, new_glob, driver_ctx);
  }
#endif

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
  // Run flatten before HPOLY and LPOLY
  if (target_layer == HPOLY || target_layer == HPOLY_P2 ||
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
            opcode == fhe::ckks::OPC_UPSCALE ||
            opcode == fhe::ckks::OPC_MODSWITCH ||
            opcode == fhe::ckks::OPC_RELIN ||
            opcode == fhe::ckks::OPC_BOOTSTRAP) {
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
