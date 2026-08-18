//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/base/container.h"
#include "air/base/st.h"
#include "air/base/visitor.h"
#include "air/core/handler.h"
#include "air/core/null_handler.h"
#include "nn/core/handler.h"
#include "nn/core/opcode.h"
#include "nn/llama/llama.h"
#include "nn/vector/vector_utils.h"

using namespace air::base;
using namespace nn::core;
using namespace nn::vector;

namespace nn {
namespace llama {

#define ZERO       0
#define ARRAY_SIZE 4096

void LLAMA::Create_entry_func() {
  // name of entry function
  STR_PTR name_str = _glob->New_str("entry_func");
  // entry function
  FUNC_PTR entry_func = _glob->New_func(name_str, _spos);
  entry_func->Set_parent(_glob->Comp_env_id());
  // signature of entry function
  SIGNATURE_TYPE_PTR sig = _glob->New_sig_type();

  // parameter x of entry function
  TYPE_PTR base_type_idx = _glob->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
  std::vector<int64_t> shape{1, 1, ARRAY_SIZE};
  TYPE_PTR             array_type =
      New_array_type(_glob, "float", base_type_idx, shape, _spos);

  STR_PTR var_str = _glob->New_str("input");
  _glob->New_param(var_str, array_type, sig, _spos);

  // return type of entry function
  TYPE_PTR func_rtype =
      New_array_type(_glob, "float", base_type_idx, shape, _spos);
  ;
  _glob->New_ret_param(func_rtype, sig);

  sig->Set_complete();
  _glob->New_entry_point(sig, entry_func, name_str, _spos);

  // set define before create a new scope
  _func_scope     = &_glob->New_func_scope(entry_func);
  CONTAINER* cntr = &_func_scope->Container();
  cntr->New_func_entry(_spos);
}

ADDR_DATUM_PTR LLAMA::Create_rmsnorm(std::string result) {
  CONTAINER* cntr     = &_func_scope->Container();
  STMT_PTR   ent_stmt = cntr->New_func_entry(_spos);
  // array x;
  ADDR_DATUM_PTR var_x = _func_scope->Formal(ZERO);

  STR_PTR  z_str         = _glob->New_str(result.c_str());
  TYPE_PTR base_type_idx = _glob->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
  std::vector<int64_t> shape{1, 1, ARRAY_SIZE};
  TYPE_PTR             array_type =
      New_array_type(_glob, "float", base_type_idx, shape, _spos);
  ADDR_DATUM_PTR var_z = _func_scope->New_var(array_type, z_str, _spos);

  // z = nn::core::rmsnorm(x, w)
  NODE_PTR nnx_node = cntr->New_ld(var_x, _spos);

  std::vector<int64_t> weight_shape{ARRAY_SIZE};
  TYPE_PTR             weight_array_type =
      New_array_type(_glob, "float", base_type_idx, weight_shape, _spos);

  float floats[ARRAY_SIZE];
  // It is a workaround since we don't want to read the weight from file.
  for (int32_t i = 0; i < ARRAY_SIZE; i++) floats[i] = 0.1 * (i + 1);

  CONSTANT_PTR cst = _glob->New_const(CONSTANT_KIND::ARRAY, weight_array_type,
                                      floats, sizeof(floats));

  NODE_PTR nny_node = cntr->New_ldc(cst, _spos);
  NODE_PTR nn_node  = cntr->New_bin_arith(
      air::base::OPCODE(nn::core::NN, nn::core::OPCODE::RMSNORM), nnx_node,
      nny_node, _spos);
  STMT_PTR  nnstmt = cntr->New_st(nn_node, var_z, _spos);
  STMT_LIST sl     = cntr->Stmt_list();
  sl.Append(nnstmt);

  return var_z;
}

ADDR_DATUM_PTR LLAMA::Create_matmul(NODE_PTR nnx_node, NODE_PTR nny_node,
                                    TYPE_PTR array_type, std::string result) {
  CONTAINER* cntr    = &_func_scope->Container();
  NODE_PTR   nn_node = cntr->New_bin_arith(
      air::base::OPCODE(nn::core::NN, nn::core::OPCODE::MATMUL), nnx_node,
      nny_node, _spos);
  STR_PTR        z_str  = _glob->New_str(result.c_str());
  ADDR_DATUM_PTR var_z  = _func_scope->New_var(array_type, z_str, _spos);
  STMT_PTR       nnstmt = cntr->New_st(nn_node, var_z, _spos);
  STMT_LIST      sl     = cntr->Stmt_list();
  sl.Append(nnstmt);
  return var_z;
}

ADDR_DATUM_PTR LLAMA::Create_matmul(ADDR_DATUM_PTR input_1,
                                    ADDR_DATUM_PTR input_2,
                                    std::string    result) {
  CONTAINER* cntr     = &_func_scope->Container();
  NODE_PTR   nnx_node = cntr->New_ld(input_1, _spos);
  NODE_PTR   nny_node = cntr->New_ld(input_2, _spos);
  return Create_matmul(nnx_node, nny_node, input_1->Type(), result);
}

ADDR_DATUM_PTR LLAMA::Create_matmul(ADDR_DATUM_PTR input,
                                    // weight input is not used so far.
                                    std::string weight_input,
                                    std::string result) {
  CONTAINER* cntr     = &_func_scope->Container();
  NODE_PTR   nnx_node = cntr->New_ld(input, _spos);

  CONSTANT_PTR cst      = Read_value_from_file(weight_input);
  TYPE_PTR     cst_type = cst->Type();
  AIR_ASSERT_MSG(cst_type->Is_array(), "Expect tensor type");
  TYPE_PTR array_type = cst_type->Cast_to_arr();

  NODE_PTR nny_node = cntr->New_ldc(cst, _spos);
  return Create_matmul(nnx_node, nny_node, array_type, result);
}

ADDR_DATUM_PTR LLAMA::Create_sqrt(std::string weight_input,
                                  std::string result) {
  CONTAINER*   cntr     = &_func_scope->Container();
  CONSTANT_PTR cst      = Read_value_from_file(weight_input);
  TYPE_PTR     cst_type = cst->Type();
  AIR_ASSERT_MSG(cst_type->Is_array(), "Expect tensor type");
  TYPE_PTR       array_type = cst_type->Cast_to_arr();
  STR_PTR        z_str      = _glob->New_str(result.c_str());
  ADDR_DATUM_PTR var_z      = _func_scope->New_var(array_type, z_str, _spos);
  NODE_PTR       nnx_node   = cntr->New_ldc(cst, _spos);
  NODE_PTR       nn_node    = cntr->New_una_arith(
      air::base::OPCODE(nn::core::NN, nn::core::OPCODE::SQRT), nnx_node, _spos);
  STMT_PTR  nnstmt = cntr->New_st(nn_node, var_z, _spos);
  STMT_LIST sl     = cntr->Stmt_list();
  sl.Append(nnstmt);

  return var_z;
}

CONSTANT_PTR LLAMA::Read_value_from_file(std::string file_name) {
  // This code is a workaround since the weight file approach will
  // be used later.
  TYPE_PTR base_type_idx = _glob->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
  std::vector<int64_t> shape{ARRAY_SIZE, ARRAY_SIZE};
  TYPE_PTR             array_type =
      New_array_type(_glob, "float", base_type_idx, shape, _spos);
  std::vector<float> floats(ARRAY_SIZE * ARRAY_SIZE);
  for (int32_t i = 0; i < ARRAY_SIZE; i++)
    for (int32_t j = 0; j < ARRAY_SIZE; j++)
      floats[i * ARRAY_SIZE + j] = 0.1 * (i * j + 1);
  CONSTANT_PTR cst =
      _glob->New_const(CONSTANT_KIND::ARRAY, array_type, floats.data(),
                       4 * ARRAY_SIZE * ARRAY_SIZE);
  return cst;
}

std::pair<ADDR_DATUM_PTR, ADDR_DATUM_PTR> LLAMA::Create_kv_cache(
    ADDR_DATUM_PTR input_1, ADDR_DATUM_PTR input_2, ADDR_DATUM_PTR input_3,
    std::string start_pos_input, std::string output_name) {
  CONTAINER* cntr     = &_func_scope->Container();
  NODE_PTR   nnx_node = cntr->New_ld(input_1, _spos);
  NODE_PTR   nny_node = cntr->New_ld(input_2, _spos);
  NODE_PTR   nnz_node = cntr->New_ld(input_3, _spos);
  TYPE_PTR   type     = nnx_node->Rtype();
  AIR_ASSERT_MSG(type->Is_array(), "Expect tensor type");
  TYPE_PTR array_type = type->Cast_to_arr();
  NODE_PTR nn_node    = cntr->New_tern_arith(
      air::base::OPCODE(nn::core::NN, nn::core::OPCODE::RESHAPE_KV), nnx_node,
      nny_node, nnz_node, _spos);
  return Create_record_return(nn_node, array_type, output_name);
}

std::pair<ADDR_DATUM_PTR, ADDR_DATUM_PTR> LLAMA::Create_rope_rotary(
    ADDR_DATUM_PTR input_1, ADDR_DATUM_PTR input_2, std::string weight_input,
    std::string output_name) {
  CONTAINER* cntr = &_func_scope->Container();

  CONSTANT_PTR cst      = Read_value_from_file(weight_input);
  TYPE_PTR     cst_type = cst->Type();
  AIR_ASSERT_MSG(cst_type->Is_array(), "Expect tensor type");
  TYPE_PTR array_type = cst_type->Cast_to_arr();

  NODE_PTR nnx_node = cntr->New_ld(input_1, _spos);
  NODE_PTR nny_node = cntr->New_ld(input_2, _spos);
  NODE_PTR nnw_node = cntr->New_ldc(cst, _spos);
  NODE_PTR nn_node  = cntr->New_tern_arith(
      air::base::OPCODE(nn::core::NN, nn::core::OPCODE::ROPE_ROTARY), nnx_node,
      nny_node, nnw_node, _spos);

  return Create_record_return(nn_node, array_type, output_name);
}

std::pair<ADDR_DATUM_PTR, ADDR_DATUM_PTR> LLAMA::Create_record_return(
    NODE_PTR nn_node, TYPE_PTR array_type, std::string output_name) {
  CONTAINER* cntr  = &_func_scope->Container();
  STR_PTR    z_str = _glob->New_str(output_name.c_str());

  RECORD_TYPE_PTR rec_type = _glob->New_rec_type(
      RECORD_KIND::STRUCT, (output_name + "_rec").c_str(), _spos);
  STR_PTR   name_fld1 = _glob->New_str("fld1");
  FIELD_PTR fld1      = _glob->New_fld(name_fld1, array_type, rec_type, _spos);
  rec_type->Add_fld(fld1->Id());
  STR_PTR   name_fld2 = _glob->New_str("fld2");
  FIELD_PTR fld2      = _glob->New_fld(name_fld2, array_type, rec_type, _spos);
  rec_type->Add_fld(fld2->Id());
  rec_type->Set_complete();

  ADDR_DATUM_PTR var_z  = _func_scope->New_var(rec_type, z_str, _spos);
  STMT_PTR       nnstmt = cntr->New_st(nn_node, var_z, _spos);
  STMT_LIST      sl     = cntr->Stmt_list();
  sl.Append(nnstmt);

  FIELD_PTR      fld_ptr_0 = Get_fld(var_z, 0);
  STR_PTR        z_str_0   = _glob->New_str((output_name + "_0").c_str());
  ADDR_DATUM_PTR var_z_0   = _func_scope->New_var(array_type, z_str_0, _spos);
  NODE_PTR       var_f0    = cntr->New_ldf(var_z, fld_ptr_0, _spos);
  STMT_PTR       nnstmt_0  = cntr->New_st(var_f0, var_z_0, _spos);
  sl.Append(nnstmt_0);

  FIELD_PTR      fld_ptr_1 = Get_fld(var_z, 1);
  STR_PTR        z_str_1   = _glob->New_str((output_name + "_1").c_str());
  ADDR_DATUM_PTR var_z_1   = _func_scope->New_var(array_type, z_str_1, _spos);
  NODE_PTR       var_f1    = cntr->New_ldf(var_z, fld_ptr_1, _spos);
  STMT_PTR       nnstmt_1  = cntr->New_st(var_f1, var_z_1, _spos);
  sl.Append(nnstmt_1);

  return {var_z_0, var_z_1};
}

ADDR_DATUM_PTR LLAMA::Create_repeat_kv(ADDR_DATUM_PTR input, int32_t val,
                                       std::string result) {
  CONTAINER* cntr     = &_func_scope->Container();
  NODE_PTR   nnx_node = cntr->New_ld(input, _spos);
  TYPE_PTR   int_type = _glob->Prim_type(PRIMITIVE_TYPE::INT_S32);
  NODE_PTR   cst_node = cntr->New_intconst(int_type, val, _spos);
  NODE_PTR   nn_node  = cntr->New_bin_arith(
      air::base::OPCODE(nn::core::NN, nn::core::OPCODE::REPEAT_KV), nnx_node,
      cst_node, _spos);

  STR_PTR        z_str  = _glob->New_str(result.c_str());
  ADDR_DATUM_PTR var_z  = _func_scope->New_var(input->Type(), z_str, _spos);
  STMT_PTR       nnstmt = cntr->New_st(nn_node, var_z, _spos);
  STMT_LIST      sl     = cntr->Stmt_list();
  sl.Append(nnstmt);

  return var_z;
}
ADDR_DATUM_PTR LLAMA::Create_transpose(ADDR_DATUM_PTR input, int32_t val_1,
                                       int32_t val_2, std::string result) {
  CONTAINER* cntr       = &_func_scope->Container();
  NODE_PTR   nnx_node   = cntr->New_ld(input, _spos);
  TYPE_PTR   int_type   = _glob->Prim_type(PRIMITIVE_TYPE::INT_S32);
  NODE_PTR   cst_node_1 = cntr->New_intconst(int_type, val_1, _spos);
  NODE_PTR   cst_node_2 = cntr->New_intconst(int_type, val_2, _spos);
  NODE_PTR   nn_node    = cntr->New_tern_arith(
      air::base::OPCODE(nn::core::NN, nn::core::OPCODE::TRANSPOSE), nnx_node,
      cst_node_1, cst_node_2, _spos);

  STR_PTR        z_str  = _glob->New_str(result.c_str());
  ADDR_DATUM_PTR var_z  = _func_scope->New_var(input->Type(), z_str, _spos);
  STMT_PTR       nnstmt = cntr->New_st(nn_node, var_z, _spos);
  STMT_LIST      sl     = cntr->Stmt_list();
  sl.Append(nnstmt);

  return var_z;
}

ADDR_DATUM_PTR LLAMA::Create_divide(ADDR_DATUM_PTR input_1,
                                    ADDR_DATUM_PTR input_2,
                                    std::string    result) {
  CONTAINER*     cntr     = &_func_scope->Container();
  NODE_PTR       nnx_node = cntr->New_ld(input_1, _spos);
  NODE_PTR       nny_node = cntr->New_ld(input_2, _spos);
  STR_PTR        z_str    = _glob->New_str(result.c_str());
  ADDR_DATUM_PTR var_z    = _func_scope->New_var(input_1->Type(), z_str, _spos);
  NODE_PTR       nn_node  = cntr->New_bin_arith(
      air::base::OPCODE(nn::core::NN, nn::core::OPCODE::DIVIDE), nnx_node,
      nny_node, _spos);
  STMT_PTR  nnstmt = cntr->New_st(nn_node, var_z, _spos);
  STMT_LIST sl     = cntr->Stmt_list();
  sl.Append(nnstmt);

  return var_z;
}

ADDR_DATUM_PTR LLAMA::Create_softmax(ADDR_DATUM_PTR input, std::string result) {
  CONTAINER*     cntr     = &_func_scope->Container();
  NODE_PTR       nnx_node = cntr->New_ld(input, _spos);
  STR_PTR        z_str    = _glob->New_str(result.c_str());
  ADDR_DATUM_PTR var_z    = _func_scope->New_var(input->Type(), z_str, _spos);
  NODE_PTR       nn_node  = cntr->New_una_arith(
      air::base::OPCODE(nn::core::NN, nn::core::OPCODE::SQRT), nnx_node, _spos);
  STMT_PTR  nnstmt = cntr->New_st(nn_node, var_z, _spos);
  STMT_LIST sl     = cntr->Stmt_list();
  sl.Append(nnstmt);

  return var_z;
}

void LLAMA::Create_return(ADDR_DATUM_PTR input) {
  CONTAINER* cntr     = &_func_scope->Container();
  NODE_PTR   out_node = cntr->New_ld(input, _spos);
  STMT_PTR   ret_stmt = cntr->New_retv(out_node, _spos);
  STMT_LIST  sl       = cntr->Stmt_list();
  sl.Append(ret_stmt);
}

FIELD_PTR LLAMA::Get_fld(ADDR_DATUM_PTR var, uint32_t fld_id) {
  TYPE_PTR t_var = var->Type();
  CMPLR_ASSERT(t_var->Is_record(), "not a record type");

  FIELD_ITER fld_iter     = t_var->Cast_to_rec()->Begin();
  FIELD_ITER fld_iter_end = t_var->Cast_to_rec()->End();
  uint32_t   idx          = 0;
  while (idx < fld_id) {
    ++fld_iter;
    ++idx;
    CMPLR_ASSERT(fld_iter != fld_iter_end, "fld id outof range")
  }
  TYPE_PTR fld_ty = (*fld_iter)->Type();
  return *fld_iter;
}

}  // namespace llama
}  // namespace nn
