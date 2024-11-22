//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/core/ctx_param_ana.h"

#include <algorithm>
#include <cstddef>
#include <string>

#include "air/base/analyze_ctx.h"
#include "air/base/container.h"
#include "air/base/meta_info.h"
#include "air/base/opcode.h"
#include "air/base/st_decl.h"
#include "air/core/opcode.h"
#include "air/opt/ssa_build.h"
#include "air/util/debug.h"
#include "air/util/error.h"
#include "air/util/messg.h"
#include "err_msg.inc.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/core/scheme_info.h"

namespace fhe {
namespace core {
uint32_t CTX_PARAM_ANA_CTX::Get_mul_level_of_ssa_ver(SSA_VER_ID id) {
  AIR_ASSERT(id.Value() < Ssa_cntr()->Num_ver());
  MUL_LEVEL_MAP::iterator itr = Get_mul_level_of_ssa_ver().find(id.Value());
  if (itr == Get_mul_level_of_ssa_ver().end()) {
    return 0;
  }
  return itr->second;
}

bool CTX_PARAM_ANA_CTX::Update_mul_level_of_ssa_ver(SSA_VER_ID id,
                                                    uint32_t   new_level) {
  if (id.Value() >= Ssa_cntr()->Num_ver()) {
    AIR_ASSERT_MSG(false, "version id out of range.");
  }
  std::pair<MUL_LEVEL_MAP::iterator, bool> res =
      Get_mul_level_of_ssa_ver().insert({id.Value(), new_level});
  // mul_level of variable set for the first time
  if (res.second) {
    return true;
  }

  // mul_level of variable remain unchanged
  if (new_level <= res.first->second) {
    return false;
  }

  // mul_level of variable inc to new_level
  res.first->second = new_level;
  return true;
}

void CTX_PARAM_ANA_CTX::Print(std::ostream& out) const {
  std::string indent0(4, ' ');
  std::string indent1(8, ' ');
  const char* func_name = Func_scope()->Owning_func()->Name()->Char_str();
  out << "CTX_PARAM_ANA res of func: " << func_name << " {" << std::endl;
  out << indent0 << "max mul_level= " << Get_mul_level() << std::endl;
  out << indent0 << "mul_level of ssa_ver {" << std::endl;
  for (std::pair<uint32_t, uint32_t> ver_lev : Get_mul_level_of_ssa_ver()) {
    air::opt::SSA_VER_PTR ssa_ver = Ssa_cntr()->Ver(SSA_VER_ID(ver_lev.first));
    out << indent1 << ssa_ver->To_str() << ": " << ver_lev.second << std::endl;
  }
  out << indent0 << "}" << std::endl;
  out << "}" << std::endl;
}

int64_t CORE_ANA_IMPL::Get_init_val_of_iv(NODE_PTR loop) {
  // only support do_loop
  AIR_ASSERT(loop->Is_do_loop());

  NODE_PTR init_val_node = loop->Child(0);
  if (init_val_node->Opcode().Domain() != air::core::CORE ||
      init_val_node->Operator() != air::core::OPCODE::INTCONST) {
    CMPLR_ASSERT(std::cout, "init value of iv must constant integer");
  }
  return init_val_node->Intconst();
}

int64_t CORE_ANA_IMPL::Get_strid_of_iv(NODE_PTR loop) {
  // only support do_loop
  AIR_ASSERT(loop->Is_do_loop());

  NODE_PTR iv_incr_node = loop->Child(2);
  AIR_ASSERT(iv_incr_node->Opcode().Domain() == air::core::CORE);

  NODE_PTR stride = iv_incr_node->Child(1);
  // stride of iv must be constant integer
  AIR_ASSERT(stride->Opcode().Domain() == air::core::CORE &&
             stride->Operator() == air::core::OPCODE::INTCONST);

  if (iv_incr_node->Operator() == air::core::OPCODE::ADD) {
    return stride->Intconst();
  } else if (iv_incr_node->Operator() == air::core::OPCODE::SUB) {
    return -stride->Intconst();
  } else {
    CMPLR_ASSERT(std::cout, "iv must incr via ADD/SUB");
    AIR_ASSERT(false);
    return 0;
  }
}

int64_t CORE_ANA_IMPL::Get_bound_of_iv(NODE_PTR loop) {
  // only support do_loop
  AIR_ASSERT(loop->Is_do_loop());

  NODE_PTR cmp_node       = loop->Child(1);
  NODE_PTR bound_val_node = cmp_node->Child(1);
  // bound of iv must be constant integer
  AIR_ASSERT(bound_val_node->Opcode().Domain() == air::core::CORE &&
             bound_val_node->Operator() == air::core::OPCODE::INTCONST);
  return bound_val_node->Intconst();
}

int64_t CORE_ANA_IMPL::Get_itr_cnt(air::base::OPCODE cmp_op, int64_t init,
                                   int64_t stride, int64_t bound) {
  AIR_ASSERT(cmp_op.Domain() == air::core::CORE);
  switch (cmp_op.Operator()) {
    case air::core::OPCODE::LT: {
      int64_t itr_cnt = (bound - init + (stride - 1)) / stride;
      return itr_cnt;
    }
    case air::core::OPCODE::GT: {
      int64_t itr_cnt = (bound - init + (stride + 1)) / stride;
      return itr_cnt;
      ;
    }
    case air::core::OPCODE::GE:
    case air::core::OPCODE::LE: {
      int64_t itr_cnt = (bound - init + stride) / stride;
      return itr_cnt;
    }
    default: {
      Templ_print(std::cout, "only support LT/LE/GT/GE");
      AIR_ASSERT(false);
      return 0;
    }
  }
}

IV_INFO CORE_ANA_IMPL::Get_loop_iv_info(NODE_PTR loop) {
  // only support do_loop
  AIR_ASSERT(loop->Is_do_loop());
  // 1. get init value of iv
  int64_t init_val = Get_init_val_of_iv(loop);

  // 2. get stride value of iv
  int64_t stride_val = Get_strid_of_iv(loop);

  // 3. get bound value of iv
  int64_t bound_val = Get_bound_of_iv(loop);

  // 4. cal iteration number of loop
  NODE_PTR cmp_node = loop->Child(1);
  int64_t  itr_cnt =
      Get_itr_cnt(cmp_node->Opcode(), init_val, stride_val, bound_val);

  NODE_PTR iv_node = cmp_node->Child(0);
  AIR_ASSERT(META_INFO::Has_prop<OPR_PROP::LOAD>(iv_node->Opcode()));
  SYM_ID iv_sym_id = iv_node->Addr_datum()->Id();
  return IV_INFO(iv_sym_id, init_val, stride_val, itr_cnt);
}

void CTX_PARAM_ANA::Build_ssa() {
  air::opt::SSA_BUILDER ssa_builder(Func_scope(), &Ssa_cntr(), Driver_ctx());
  // update SSA_CONFIG
  air::opt::SSA_CONFIG& ssa_config = ssa_builder.Ssa_config();
  ssa_config.Set_trace_ir_before_ssa(
      Config()->Is_trace(ckks::TRACE_DETAIL::TRACE_IR_BEFORE_SSA));
  ssa_config.Set_trace_ir_after_insert_phi(
      Config()->Is_trace(ckks::TRACE_DETAIL::TRACE_IR_AFTER_SSA_INSERT_PHI));
  ssa_config.Set_trace_ir_after_ssa(
      Config()->Is_trace(ckks::TRACE_DETAIL::TRACE_IR_AFTER_SSA));
  ssa_builder.Perform();
}

R_CODE CTX_PARAM_ANA::Update_ctx_param_with_config() {
  CTX_PARAM& ctx_param = Lower_ctx()->Get_ctx_param();
  uint32_t   poly_deg  = Config()->Poly_deg();
  if (poly_deg != 0) {
    if (poly_deg < ctx_param.Get_poly_degree()) {
      std::string err_msg = "poly_deg(N) must be >= " +
                            std::to_string(ctx_param.Get_poly_degree());
      CMPLR_USR_MSG(U_CODE::Incorrect_Option, err_msg.c_str());
      return R_CODE::USER;
    }
    // poly_deg from config must be pow of 2
    if ((poly_deg & (poly_deg - 1U)) != 0) {
      CMPLR_USR_MSG(U_CODE::Incorrect_Option, "poly_deg(N) must be pow of 2");
      return R_CODE::USER;
    }
    ctx_param.Set_poly_degree(poly_deg);
  }

  uint32_t q0_bit_num = Config()->Q0_bit_num();
  if (q0_bit_num != 0) {
    uint32_t sf_bit_num = Config()->Scale_factor_bit_num();
    if (sf_bit_num == 0) {
      CMPLR_USR_MSG(U_CODE::Incorrect_Option,
                    "must configure scale_factor along with q0");
      return R_CODE::USER;
    }
    if (sf_bit_num >= q0_bit_num) {
      CMPLR_USR_MSG(U_CODE::Incorrect_Option,
                    "scale_factor must be less than q0");
      return R_CODE::USER;
    }
    ctx_param.Set_scaling_factor_bit_num(sf_bit_num);
    ctx_param.Set_first_prime_bit_num(q0_bit_num);
  }
  return R_CODE::NORMAL;
}

R_CODE CTX_PARAM_ANA::Run() {
  // 1. build ssa
  Build_ssa();

  // 2. analyze mul_level and rotate index of function
  CTX_PARAM_ANA_CTX& ana_ctx = Get_ana_ctx();
  ANA_VISITOR        visitor(ana_ctx, {CORE_HANDLER(), CKKS_HANDLER()});
  NODE_PTR           func_body = Func_scope()->Container().Entry_node();
  visitor.template Visit<ANA_RETV>(func_body);

  ana_ctx.Trace_obj(ckks::TRACE_DETAIL::TRACE_CKKS_ANA_RES, &ana_ctx);

  // 3. update CTX_PARAM in LOWER_CTX
  CTX_PARAM& ctx_param = Lower_ctx()->Get_ctx_param();
  ctx_param.Set_mul_level(ana_ctx.Get_mul_level(), true);
  ctx_param.Add_rotate_index(ana_ctx.Get_rotate_index());

  // 4. update CTX_PARAM with Config
  R_CODE res = Update_ctx_param_with_config();
  if (res != R_CODE::NORMAL) {
    return res;
  }
  ana_ctx.Trace_obj(ckks::TRACE_DETAIL::TRACE_CKKS_ANA_RES, &ctx_param);
  return R_CODE::NORMAL;
}

}  // namespace core
}  // namespace fhe
