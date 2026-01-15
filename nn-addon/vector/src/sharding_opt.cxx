//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "nn/vector/sharding_opt.h"

#include <numeric>

#include "air/base/handler_retv.h"
#include "air/base/transform_ctx.h"
#include "air/base/visitor.h"
#include "air/core/handler.h"
#include "nn/core/data_scheme.h"
#include "nn/core/handler.h"
#include "nn/vector/core_handler.h"
#include "nn/vector/handler.h"
#include "nn/vector/t2tsharding_analysis_ctx.h"
#include "nn/vector/t2tsharding_analysis_handler.h"
#include "nn/vector/t2tsharding_ctx.h"
#include "nn/vector/t2tsharding_handler.h"

using namespace air::base;
using namespace nn::vector;

namespace nn {
namespace vector {

using FUNCID_MAP = std::map<FUNC_ID, FUNC_ID>;

// Lower formal type to new type.
// Still both signature+formal. TODO: only formal.
void Lower_formal_type(GLOB_SCOPE* gscope, GLOB_SCOPE* new_sharding_gscope,
                       SHARDING_MAP& shmap, FUNCID_MAP& func_map) {
  FUNC_ITER f_iter = gscope->Begin_func();
  FUNC_ITER f_end  = gscope->End_func();

  for (; f_iter != f_end; ++f_iter) {
    FUNC_PTR    func   = *f_iter;
    FUNC_SCOPE* fscope = &(gscope->Open_func_scope(func->Id()));
    FUNC_PTR    new_func =
        new_sharding_gscope->New_func(func->Name(), func->Spos());
    ENTRY_PTR          entry      = func->Entry_point();
    SIGNATURE_TYPE_PTR sig        = entry->Type()->Cast_to_sig();
    PARAM_ITER         param_iter = sig->Begin_param();
    PARAM_ITER         param_end  = sig->End_param();

    SIGNATURE_TYPE_PTR new_func_sig = new_sharding_gscope->New_sig_type();
    int                param_idx    = 0;
    for (; param_iter != param_end; ++param_iter) {
      PARAM_PTR param      = *param_iter;
      TYPE_PTR  param_type = param->Type();

      if (param->Is_ret()) {
        new_sharding_gscope->New_ret_param(param_type, new_func_sig);
        continue;
      }

      // param is formal-split:
      const ARRAY_SHARDING* fsharding = shmap.Get_data_sharding(
          fscope->Id(), fscope->Formal(param_idx)->Id());
      ++param_idx;

      if (fsharding != nullptr) {
        std::vector<int64_t> xyz = fsharding->Spec();
        AIR_ASSERT_MSG(param_type->Is_array(),
                       "formal type is not an array type");
        std::vector<int64_t> param_shape = param_type->Cast_to_arr()->Shape();
        TYPE_PTR             etype = param_type->Cast_to_arr()->Elem_type();
        AIR_ASSERT(param_shape.size() == xyz.size());
        int64_t param_size =
            std::accumulate(param_shape.begin(), param_shape.end(), 1,
                            std::multiplies<int64_t>());
        int num_blocks = std::accumulate(xyz.begin(), xyz.end(), 1,
                                         std::multiplies<int64_t>());

        // reset block shape
        // If spatial partitioning, halo size is needed to overlap the boundary.
        std::vector<int64_t> param_block_shape(param_shape.size(), 1);
        for (size_t i = 0; i < param_shape.size(); ++i) {
          param_block_shape[i] = param_shape[i] / xyz[i];
        }
        // [N,C,H,W]: H dimension partition, top&down padding
        if ((xyz.size() == 4) && (xyz[2] > 1)) {
          AIR_ASSERT_MSG(param_shape[1] == xyz[1],
                         "H dimension partition, C's sharding = C");
          param_block_shape[2] += 2 * fsharding->Halosize();
        }

        TYPE_PTR param_block_type =
            New_array_type(new_sharding_gscope, "Tparam_block", etype,
                           param_block_shape, param_type->Spos());
        param_type =
            New_array_type(new_sharding_gscope, "Tparam_sharding",
                           param_block_type, {num_blocks}, param_type->Spos());
      }

      new_sharding_gscope->New_param(param->Name(), param_type, new_func_sig,
                                     sig->Spos());
    }
    new_func_sig->Set_complete();

    ENTRY_PTR new_entry = new_sharding_gscope->New_entry_point(
        new_func_sig, new_func, entry->Name(), entry->Spos());
    if (entry->Is_program_entry()) new_entry->Set_program_entry();
    // id map
    func_map.insert({func->Id(), new_func->Id()});
  }
}

// Sharding analysis and optimization
GLOB_SCOPE* Run_sharding_opt(GLOB_SCOPE* gscope, VECTOR_CTX& ctx,
                             const air::driver::DRIVER_CTX* driver_ctx,
                             const VECTOR_CONFIG&           cfg) {
  if (!cfg.Sharding()) return gscope;

  FUNCID_MAP   func_map;
  SHARDING_MAP shmap;
  // Analysis:
  for (GLOB_SCOPE::FUNC_SCOPE_ITER it = gscope->Begin_func_scope();
       it != gscope->End_func_scope(); ++it) {
    FUNC_SCOPE* fscope = &(*it);
    CONTAINER*  cntr   = &(fscope->Container());
    NODE_PTR    body   = cntr->Entry_node();

    nn::vector::T2TSHARDING_ANALYSIS_CTX ctx_analysis(cntr, driver_ctx, cfg,
                                                      &shmap);
    ctx_analysis.Trace(TF_SHARDING, "#### Starding analysis on ",
                       fscope->Owning_func()->Name()->Char_str(), ":\n");

    air::base::VISITOR<
        nn::vector::T2TSHARDING_ANALYSIS_CTX,
        air::core::HANDLER<nn::vector::T2TSHARDING_ANALYSIS_CORE_HANDLER>,
        nn::core::HANDLER<nn::vector::T2TSHARDING_ANALYSIS_HANDLER> >
        trav_analysis(ctx_analysis);
    trav_analysis.Visit<void>(body);
  }

  GLOB_SCOPE* new_sharding_gscope = new GLOB_SCOPE(gscope->Id(), true);
  AIR_ASSERT(new_sharding_gscope != nullptr);
  new_sharding_gscope->Clone(*gscope, false);

  Lower_formal_type(gscope, new_sharding_gscope, shmap, func_map);

  // Propagation&Transformation
  for (GLOB_SCOPE::FUNC_SCOPE_ITER it = gscope->Begin_func_scope();
       it != gscope->End_func_scope(); ++it) {
    FUNC_SCOPE* fscope = &(*it);
    CONTAINER*  cntr   = &(fscope->Container());
    NODE_PTR    body   = cntr->Entry_node();

    FUNC_SCOPE* new_fscope =
        &new_sharding_gscope->New_func_scope(func_map[fscope->Id()]);
    new_fscope->Clone_attr(*fscope);

    // No clone.
    CONTAINER* new_cntr = &(new_fscope->Container());
    // Formal is ready.
    new_cntr->New_func_entry(new_fscope->Owning_func()->Spos());

    nn::vector::T2TSHARDING_CTX ctx_trans(new_cntr, ctx, driver_ctx, cfg,
                                          &shmap);

    ctx_trans.Trace(TF_SHARDING, "#### Starding transform on ",
                    fscope->Owning_func()->Name()->Char_str(), ":\n");

    air::base::VISITOR<nn::vector::T2TSHARDING_CTX,
                       air::core::HANDLER<nn::vector::T2TSHARDING_CORE_HANDLER>,
                       nn::core::HANDLER<nn::vector::T2TSHARDING_HANDLER> >
        trav_trans(ctx_trans);

    // map old_formal to new_formal
    FORMAL_ITER formal_itr = fscope->Begin_formal();
    FORMAL_ITER end_formal = fscope->End_formal();

    FORMAL_ITER new_formal_itr = new_fscope->Begin_formal();
    FORMAL_ITER end_new_formal = new_fscope->End_formal();

    // idname type1 -formal-1, idname type2 -formal-2?
    // Handle_idname() change data_addr sym->formal?
    for (; (formal_itr != end_formal) && (new_formal_itr != end_new_formal);
         ++formal_itr, ++new_formal_itr) {
      ADDR_DATUM_PTR formal     = *formal_itr;
      ADDR_DATUM_PTR new_formal = *new_formal_itr;
      ctx_trans.Sharding_map()->Set_shard_datum(formal, new_formal);
    }

    air::base::HANDLER_RETV retv =
        trav_trans.Visit<air::base::HANDLER_RETV>(body);
    AIR_ASSERT(retv.Node() != air::base::Null_ptr && retv.Node()->Is_entry());
    new_fscope->Set_entry_stmt(retv.Node()->Stmt());

    // Set sharding for input
    NODE_PTR              input_node = cntr->Entry_node()->Child(0);
    const ARRAY_SHARDING* ishard =
        shmap.Get_data_sharding(fscope->Id(), input_node->Addr_datum_id());
    if (ishard != nullptr) {
      std::vector<int64_t>  xyz = ishard->Spec();
      const ARRAY_SHARDING* fsharding =
          shmap.Get_data_sharding(fscope->Id(), input_node->Addr_datum_id());
      nn::core::Set_scheme_attr(new_cntr->Entry_node()->Child(0),
                                input_node->Rtype(), 1, xyz[1], xyz[2], 1,
                                fsharding->Halosize());
    }
    ctx_trans.Trace(TF_SHARDING, "#### IR trace after sharding ",
                    new_fscope->Owning_func()->Name()->Char_str(), "\n");
    ctx_trans.Trace_obj(TF_SHARDING, new_fscope);
  }
  delete gscope;
  return new_sharding_gscope;
}

}  // namespace vector
}  // namespace nn
