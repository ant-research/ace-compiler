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

STMT_PTR CTX_PARAM_ANA_CTX::Gen_modswitch(air::opt::SSA_SYM_PTR sym,
                                          uint32_t mul_lev, uint32_t in_lev,
                                          SPOS spos) {
  CONTAINER*     cont = Ssa_cntr()->Container();
  FUNC_SCOPE*    fs   = cont->Parent_func_scope();
  ckks::CKKS_GEN ckks_gen(cont, Lower_ctx());

  if (sym->Is_preg()) {
    PREG_PTR preg = fs->Preg(PREG_ID(sym->Var_id()));
    TYPE_ID  type = preg->Type_id();
    AIR_ASSERT(Lower_ctx()->Is_cipher_type(type) ||
               Lower_ctx()->Is_cipher3_type(type));
    // single ciphertext
    NODE_PTR modswitch = cont->New_ldp(preg, spos);
    Set_node_mul_level(modswitch, in_lev);
    while (mul_lev < in_lev) {
      modswitch = ckks_gen.Gen_modswitch(modswitch);
      --in_lev;
      Set_node_mul_level(modswitch, in_lev);
    }
    return cont->New_stp(modswitch, preg, spos);
  }

  AIR_ASSERT(sym->Is_addr_datum());
  ADDR_DATUM_PTR datum = fs->Addr_datum(ADDR_DATUM_ID(sym->Var_id()));
  if (!datum->Type()->Is_array()) {
    // single ciphertext
    NODE_PTR modswitch = cont->New_ld(datum, spos);
    Set_node_mul_level(modswitch, in_lev);
    while (mul_lev < in_lev) {
      modswitch = ckks_gen.Gen_modswitch(modswitch);
      --in_lev;
      Set_node_mul_level(modswitch, in_lev);
    }
    return cont->New_st(modswitch, datum, spos);
  }

  AIR_ASSERT(datum->Type()->Cast_to_arr()->Dim() == 1);
  TYPE_PTR elem_type  = datum->Type()->Cast_to_arr()->Elem_type();
  uint64_t elem_count = datum->Type()->Cast_to_arr()->Elem_count();
  AIR_ASSERT(Lower_ctx()->Is_cipher_type(elem_type->Id()) ||
             Lower_ctx()->Is_cipher3_type(elem_type->Id()));
  NODE_PTR       lda   = cont->New_lda(datum, POINTER_KIND::FLAT64, spos);
  NODE_PTR       array = cont->New_array(lda, 1, spos);
  ADDR_DATUM_PTR iv;
  TYPE_PTR       s32_type =
      cont->Glob_scope()->Prim_type(air::base::PRIMITIVE_TYPE::INT_S32);
  if (sym->Index() != air::opt::SSA_SYM::NO_INDEX) {
    AIR_ASSERT(sym->Index() < elem_count);
    cont->Set_array_idx(array, 0,
                        cont->New_intconst(s32_type, sym->Index(), spos));
  } else {
    iv = cont->Parent_func_scope()->New_var(s32_type, "modswitch_iv", spos);
    cont->Set_array_idx(array, 0, cont->New_ld(iv, spos));
  }
  NODE_PTR modswitch = cont->New_ild(array, spos);
  while (mul_lev < in_lev) {
    modswitch = ckks_gen.Gen_modswitch(modswitch);
    --in_lev;
    Set_node_mul_level(modswitch, in_lev);
  }
  STMT_PTR st = cont->New_ist(cont->Clone_node_tree(array), modswitch, spos);
  if (sym->Index() == air::opt::SSA_SYM::NO_INDEX) {
    // generate a loop to process each element
    NODE_PTR init = cont->New_intconst(s32_type, 0, spos);
    NODE_PTR comp = cont->New_bin_arith(
        air::core::OPC_LT, s32_type, cont->New_ld(iv, spos),
        cont->New_intconst(s32_type, elem_count, spos), spos);
    NODE_PTR incr = cont->New_bin_arith(
        air::core::OPC_ADD, s32_type, cont->New_ld(iv, spos),
        cont->New_intconst(s32_type, 1, spos), spos);
    NODE_PTR body = cont->New_stmt_block(spos);
    STMT_LIST(body).Append(st);
    st = cont->New_do_loop(iv, init, comp, incr, body, spos);
  }
  return st;
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

void CORE_ANA_IMPL::Fixup_phi_res(CTX_PARAM_ANA_CTX& ctx, NODE_PTR node) {
  // check level of whole array and array elements and make sure they
  // have the same level
  typedef std::unordered_map<uint32_t, uint32_t> VER_LEVEL_MAP;
  typedef std::vector<air::opt::PHI_NODE_ID>     PHI_VEC;

  auto handler = [](PHI_NODE_PTR phi, CTX_PARAM_ANA_CTX& ctx,
                    VER_LEVEL_MAP& map, PHI_VEC& vec) {
    air::opt::SSA_SYM_PTR sym = phi->Sym();
    // only check addr_datum
    if (sym->Is_preg()) {
      return;
    }
    // add phi to list if it's whole array
    if (sym->Index() == air::opt::SSA_SYM::NO_INDEX) {
      vec.push_back(phi->Id());
      return;
    }
    // validate all elements have the same level
    VER_LEVEL_MAP::iterator it = map.find(sym->Var_id());
    if (it != map.end()) {
      AIR_ASSERT(ctx.Get_mul_level_of_ssa_ver(phi->Result_id()) == it->second);
    } else {
      map[sym->Var_id()] = ctx.Get_mul_level_of_ssa_ver(phi->Result_id());
    }
  };

  air::opt::PHI_NODE_ID phi_id = ctx.Ssa_cntr()->Node_phi(node->Id());
  air::opt::PHI_LIST    phi_list(ctx.Ssa_cntr(), phi_id);
  VER_LEVEL_MAP         map;
  PHI_VEC               vec;
  phi_list.For_each(handler, ctx, map, vec);

  for (PHI_VEC::iterator it = vec.begin(); it != vec.end(); ++it) {
    PHI_NODE_PTR          phi = ctx.Ssa_cntr()->Phi_node(*it);
    air::opt::SSA_SYM_PTR sym = phi->Sym();
    AIR_ASSERT(sym->Is_addr_datum() &&
               sym->Index() == air::opt::SSA_SYM::NO_INDEX);
    VER_LEVEL_MAP::iterator level = map.find(sym->Var_id());
    // if (level != map.end()) {
    if (level != map.end() &&
        ctx.Get_mul_level_of_ssa_ver(phi->Result_id()) <= level->second) {
      AIR_ASSERT(ctx.Get_mul_level_of_ssa_ver(phi->Result_id()) <=
                 level->second);

      ctx.Trace(ckks::TRACE_DETAIL::TD_CKKS_LEVEL_MGT,
                "  fixup: ", phi->To_str(), " l=", level->second, "\n");

      ctx.Update_mul_level_of_ssa_ver(phi->Result_id(), level->second);
    }
  }
}

void CTX_PARAM_ANA::Build_ssa() {
  air::opt::SSA_BUILDER ssa_builder(Func_scope(), &Ssa_cntr(), Driver_ctx());
  // update SSA_CONFIG
  air::opt::SSA_CONFIG& ssa_config = ssa_builder.Ssa_config();
  ssa_config.Set_trace_ir_before_ssa(
      Config()->Is_trace(ckks::TRACE_DETAIL::TD_IR_BEFORE_SSA));
  ssa_config.Set_trace_ir_after_insert_phi(
      Config()->Is_trace(ckks::TRACE_DETAIL::TD_IR_AFTER_SSA_INSERT_PHI));
  ssa_config.Set_trace_ir_after_ssa(
      Config()->Is_trace(ckks::TRACE_DETAIL::TD_IR_AFTER_SSA));
  ssa_builder.Perform();
}

R_CODE CTX_PARAM_ANA::Update_ctx_param_with_config() {
  CTX_PARAM& ctx_param = Lower_ctx()->Get_ctx_param();
  uint32_t   poly_deg  = Config()->Poly_deg();
  if (poly_deg != 0) {
    if (poly_deg < ctx_param.Get_poly_degree()) {
      // trust user provides correct poly degree. only issue a warning
      std::string err_msg = "Warning: poly_deg(N) must be >= " +
                            std::to_string(ctx_param.Get_poly_degree());
      CMPLR_WARN_MSG(Driver_ctx()->Tfile(), err_msg.c_str());
    }
    // poly_deg from config must be pow of 2
    if ((poly_deg & (poly_deg - 1U)) != 0) {
      CMPLR_USR_MSG(U_CODE::Incorrect_Option, "poly_deg(N) must be pow of 2");
      return R_CODE::USER;
    }
    ctx_param.Set_poly_degree(poly_deg, true);
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

  uint32_t lev_in = Config()->Input_cipher_lvl();
  if (lev_in > 0) {
    if (lev_in > ctx_param.Get_mul_level()) {
      CMPLR_USR_MSG(U_CODE::Incorrect_Option, "input level must not exceed ",
                    ctx_param.Get_mul_level());
      return R_CODE::USER;
    }
  }
  return R_CODE::NORMAL;
}

R_CODE CTX_PARAM_ANA::Run() {
  CTX_PARAM_ANA_CTX& ana_ctx = Get_ana_ctx();
  ana_ctx.Trace(ckks::TRACE_DETAIL::TD_CKKS_LEVEL_MGT,
                ">>> CKKS PARAM ANA AND LEVEL MANAGEMENT: \n\n");

  // 1. build ssa
  Build_ssa();

  ana_ctx.Trace(ckks::TRACE_DETAIL::TD_CKKS_LEVEL_MGT,
                ">>> SSA dump before level management:\n\n");
  ana_ctx.Trace_obj(ckks::TRACE_DETAIL::TD_CKKS_LEVEL_MGT, ana_ctx.Ssa_cntr());

  // 2. analyze mul_level and rotate index of function
  ANA_VISITOR visitor(ana_ctx, {CORE_HANDLER(), CKKS_HANDLER()});
  NODE_PTR    func_body = Func_scope()->Container().Entry_node();
  visitor.template Visit<ANA_RETV>(func_body);

  ana_ctx.Trace_obj(ckks::TRACE_DETAIL::TD_CKKS_LEVEL_MGT, &ana_ctx);

  // 3. update CTX_PARAM in LOWER_CTX
  CTX_PARAM& ctx_param = Lower_ctx()->Get_ctx_param();
  uint32_t   mul_lev   = ana_ctx.Get_mul_level();
  if (ana_ctx.Max_cipher_lvl() > 0) {
    if (mul_lev > ana_ctx.Max_cipher_lvl()) {
      CMPLR_ASSERT(false,
                   "Warning: The max cipher level set by the compiler option "
                   "is less than the required value: ",
                   mul_lev, "\n");
    } else {
      mul_lev = ana_ctx.Max_cipher_lvl();
    }
  }
  ctx_param.Set_mul_level(mul_lev, true);
  ctx_param.Add_rotate_index(ana_ctx.Get_rotate_index());

  // 4. update CTX_PARAM with Config
  R_CODE res = Update_ctx_param_with_config();
  if (res != R_CODE::NORMAL) {
    return res;
  }
  ana_ctx.Trace_obj(ckks::TRACE_DETAIL::TD_CKKS_LEVEL_MGT, &ctx_param);
  return R_CODE::NORMAL;
}

}  // namespace core
}  // namespace fhe
