//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/core/scheme_info_ana.h"

#include <cmath>

using namespace air::base;

namespace fhe {

namespace core {

// Mul_level_of_nn_op contains mul_level of each tensor operator.
// Index is the value of tensor operator in nn::core::OPCODE.
static const TENSOR_OP_INFO Mul_level_of_nn_op[nn::core::OPCODE::SUB + 1] = {
    TENSOR_OP_INFO(nn::core::OPC_INVALID, 0),
    TENSOR_OP_INFO(nn::core::OPC_ADD, 0),
    TENSOR_OP_INFO(nn::core::OPC_AVERAGE_POOL, 1),
    TENSOR_OP_INFO(nn::core::OPC_CONV, 4),
    TENSOR_OP_INFO(nn::core::OPC_FLATTEN, 1),
    TENSOR_OP_INFO(nn::core::OPC_GEMM, 1),
    TENSOR_OP_INFO(nn::core::OPC_GLOBAL_AVERAGE_POOL, 1),
    TENSOR_OP_INFO(nn::core::OPC_MAX_POOL, 1),
    TENSOR_OP_INFO(nn::core::OPC_MUL, 1),
    TENSOR_OP_INFO(nn::core::OPC_RELU, 9),
    TENSOR_OP_INFO(nn::core::OPC_RESHAPE, 0),
    TENSOR_OP_INFO(nn::core::OPC_STRIDED_SLICE, 2),
    TENSOR_OP_INFO(nn::core::OPC_SUB, 0),
};

uint32_t TENSOR_ANA_HANDLER::Mul_level_of_tensor_op(air::base::OPCODE opc) {
  const TENSOR_OP_INFO& op_info = Mul_level_of_nn_op[opc.Operator()];
  AIR_ASSERT(opc == op_info.Opcode());
  return op_info.Mul_level();
}

// Bit number of modulus at each security level.
// Bit number of modulus at HE_STD_NOT_SET is not constrained by poly_degree.
const MODULUS_INFO Mod_info[] = {
    {HE_STD_NOT_SET,     0,  {}                                },
    {HE_STD_128_CLASSIC, 10, {27, 54, 109, 218, 438, 881, 1772}},
    {HE_STD_192_CLASSIC, 10, {19, 37, 75, 152, 305, 611, 1228} },
    {HE_STD_256_CLASSIC, 10, {14, 29, 58, 118, 237, 476, 956}  },
};

static inline const MODULUS_INFO* Modulus_info(SECURITY_LEVEL sec_lev) {
  const MODULUS_INFO* mod_info = nullptr;
  switch (sec_lev) {
    case HE_STD_NOT_SET:
      mod_info = &Mod_info[0];
      break;
    case HE_STD_128_CLASSIC:
      mod_info = &Mod_info[1];
      break;
    case HE_STD_192_CLASSIC:
      mod_info = &Mod_info[2];
      break;
    case HE_STD_256_CLASSIC:
      mod_info = &Mod_info[3];
      break;
    default: {
      // not supported security level.
      AIR_ASSERT(false);
      return nullptr;
    }
  }
  AIR_ASSERT(mod_info->Sec_level() == sec_lev);
  return mod_info;
}

void SCHEME_INFO_ANA_CTX::Print(std::ostream& out) {
  out << "Scheme info of func: "
      << Func_scope()->Owning_func()->Name()->Char_str() << " {" << std::endl;
  out << "    max mul_level: " << Get_max_mul_level() << std::endl;
  out << "    max msg_len:   " << Get_max_msg_len() << std::endl;
  out << "}" << std::endl;
}

void SCHEME_INFO_ANA::Update_scheme_info_with_config() {
  Ctx_param().Set_security_level(Config()->Security_level());
  Ctx_param().Set_hamming_weight(Config()->Hamming_weight());

  uint32_t q0 = Config()->Bit_num_q0();
  if (q0 != 0) {
    Ctx_param().Set_first_prime_bit_num(q0);
  }

  uint32_t sf = Config()->Bit_num_sf();
  if (sf != 0) {
    Ctx_param().Set_scaling_factor_bit_num(sf);
  } else {
    AIR_ASSERT_MSG(q0 == 0, "must simutaneously configure q0 and sf.");
  }
}

void SCHEME_INFO_ANA::Update_scheme_info_with_ana_res() {
  // 1. cal min poly_deg that is great enough to store msg.
  // To store msg into ciphertext, length of msg must be <= poly_deg/2.
  Ctx_param().Set_mul_level(Max_mul_lev(), false);
  uint32_t       msg_len         = Max_msg_len();
  uint32_t       msg_len_bit_num = std::ceil(log2(msg_len));
  uint64_t       poly_deg        = (2ULL << msg_len_bit_num);
  SECURITY_LEVEL sec_level       = Config()->Security_level();
  if (sec_level == HE_STD_NOT_SET) {
    Ctx_param().Set_poly_degree(poly_deg);
    return;
  }

  // 2. cal min poly_deg that satisfies mul_level and security level.
  // Because poly_deg and modulus bit number have effect on each other,
  // following loop is needed to reach fixed point.
  const MODULUS_INFO* mod_info      = Modulus_info(sec_level);
  uint64_t            prev_poly_deg = 0;
  while (prev_poly_deg != poly_deg) {
    prev_poly_deg = poly_deg;

    Ctx_param().Set_poly_degree(poly_deg);
    uint32_t mod_bit_num          = Ctx_param().Get_modulus_bit_num();
    uint64_t poly_deg_for_mul_lev = mod_info->Min_poly_deg(mod_bit_num);
    poly_deg                      = std::max(poly_deg, poly_deg_for_mul_lev);
  }
}

R_CODE SCHEME_INFO_ANA::Ana_func(const FUNC_SCOPE& func_scope) {
  SIGNATURE_TYPE_PTR sig_type =
      func_scope.Owning_func()->Entry_point()->Type()->Cast_to_sig();
  uint32_t param_num = sig_type->Num_param();
  // 1. check type of parameters. currently, only support array_type function
  // parameter.
  if (param_num == 0) {
    return R_CODE::UNIMPLEMENTED;
  }
  PARAM_ITER param_iter = sig_type->Begin_param();
  PARAM_ITER param_end  = sig_type->End_param();
  for (; param_iter != param_end; ++param_iter) {
    PARAM_PTR param      = *param_iter;
    TYPE_PTR  param_type = param->Type();
    if (!param_type->Is_array()) {
      return R_CODE::UNIMPLEMENTED;
    }
  }

  // 2. analyze function body
  // calculate max mul_level and msg_length in current function.
  SCHEME_INFO_ANA_CTX ana_ctx(&Ctx_param(), &func_scope, 0);
  ANA_VISITOR         visitor(ana_ctx, {CORE_HANDLER(), TENSOR_HANDLER()});

  NODE_PTR func_entry = func_scope.Container().Entry_node();
  (void)visitor.template Visit<SCHEME_INFO_ANA_RETV>(func_entry);
  Trace_obj(TRACE_ANA_RES, &ana_ctx);

  // 3. update mul_level and msg_length of whole program
  Update_msg_len(ana_ctx.Get_max_msg_len());
  Update_mul_lev(ana_ctx.Get_max_mul_level());
  return R_CODE::NORMAL;
}

R_CODE SCHEME_INFO_ANA::Run() {
  // 1. update scheme info with config
  Trace_obj(TRACE_ANA_OPTION, Config());
  Update_scheme_info_with_config();

  // 2. analyze mul_depth and msg_length of whole program
  GLOB_SCOPE*                 glob_scope      = Glob_scope();
  GLOB_SCOPE::FUNC_SCOPE_ITER func_scope_iter = glob_scope->Begin_func_scope();
  GLOB_SCOPE::FUNC_SCOPE_ITER func_scope_end  = glob_scope->End_func_scope();
  for (; func_scope_iter != func_scope_end; ++func_scope_iter) {
    FUNC_SCOPE& func_scope = *func_scope_iter;
    R_CODE      r_code     = Ana_func(func_scope);
    if (r_code != R_CODE::NORMAL) {
      return r_code;
    }
  }

  // 3. update scheme info with analyzing result.
  // TODO: update scheme info that stored in target info
  Update_scheme_info_with_ana_res();
  Trace_obj(TRACE_ANA_RES, &Ctx_param());
  return R_CODE::NORMAL;
}

}  // namespace core
}  // namespace fhe
