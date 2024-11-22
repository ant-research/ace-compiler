//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/sihe/vector2sihe_lower.h"

#include "air/base/container_decl.h"
#include "air/base/handler_retv.h"
#include "air/base/st.h"
#include "air/base/st_decl.h"
#include "air/base/st_iter.h"
#include "air/util/debug.h"
#include "fhe/core/scheme_info_config.h"
#include "fhe/sihe/tensor2sihe_impl.h"
#include "fhe/sihe/vector2sihe_ctx.h"

namespace fhe {
namespace sihe {

void VECTOR2SIHE_LOWER::Lower_formal(FUNC_SCOPE*      vec_func_scope,
                                     FUNC_SCOPE*      sihe_func_scope,
                                     VECTOR2SIHE_CTX& ctx) {
  FORMAL_ITER vec_formal_itr = vec_func_scope->Begin_formal();
  FORMAL_ITER end_vec_formal = vec_func_scope->End_formal();
  CMPLR_ASSERT(vec_formal_itr != end_vec_formal,
               "formal list must not be empty");
  FORMAL_ITER sihe_formal_itr = sihe_func_scope->Begin_formal();
  FORMAL_ITER end_sihe_formal = sihe_func_scope->End_formal();
  for (; vec_formal_itr != end_vec_formal && sihe_formal_itr != end_sihe_formal;
       ++vec_formal_itr, ++sihe_formal_itr) {
    ADDR_DATUM_PTR vec_formal = *vec_formal_itr;

    ADDR_DATUM_PTR sihe_formal = *sihe_formal_itr;
    ctx.Set_sihe_addr_datum(vec_formal->Id(), sihe_formal->Id());
  }
  AIR_ASSERT(vec_formal_itr == end_vec_formal &&
             sihe_formal_itr == end_sihe_formal);
}

FUNC_SCOPE& VECTOR2SIHE_LOWER::Lower_vec_func(FUNC_SCOPE* vec_func_scope) {
  CMPLR_ASSERT(_glob_scope != &vec_func_scope->Glob_scope(),
               "cannot lower function into its own global scope");

  // 1. gen function scope in SIHE domain:
  FUNC_PTR    vec_func        = vec_func_scope->Owning_func();
  FUNC_PTR    sihe_func       = Get_sihe_func(vec_func);
  FUNC_SCOPE* sihe_func_scope = &_glob_scope->New_func_scope(sihe_func->Id());
  sihe_func_scope->Clone_attr(*vec_func_scope);
  sihe_func_scope->Container().New_func_entry(sihe_func->Spos());

  CONTAINER*      sihe_func_cont = &sihe_func_scope->Container();
  VECTOR2SIHE_CTX trav_ctx(sihe_func_cont, *Lower_ctx(), *Driver_ctx(),
                           Config(), Entry_map());
  LOWER_VISITOR   visitor(trav_ctx);
  // 2. lower formals of vector function:
  Lower_formal(vec_func_scope, sihe_func_scope, trav_ctx);

  // 3. lower function body of vector function:
  //    lower vector IR to sihe IR
  NODE_PTR     node = vec_func_scope->Container().Entry_node();
  HANDLER_RETV retv = visitor.Visit<HANDLER_RETV>(node);
  AIR_ASSERT(retv.Node() != air::base::Null_ptr && retv.Node()->Is_entry());
  sihe_func_scope->Set_entry_stmt(retv.Node()->Stmt());

  // 4. update poly_degree N with max msg length
  uint32_t max_msg_len = trav_ctx.Get_max_msg_len();
  uint32_t poly_deg    = core::CTX_PARAM::Get_poly_degree_of_msg(max_msg_len);

  uint32_t old_poly_deg = Lower_ctx()->Get_ctx_param().Get_poly_degree();
  if (poly_deg > old_poly_deg) {
    Lower_ctx()->Get_ctx_param().Set_poly_degree(poly_deg);
    trav_ctx.Trace(core::TRACE_ANA_RES,
                   "VECTOR2SIHE_LOWER update poly degree from ", old_poly_deg,
                   " to ", poly_deg, "\n");
  }

  return *sihe_func_scope;
}

void VECTOR2SIHE_LOWER::Lower_func_tab(GLOB_SCOPE* vec_glob_scope) {
  AIR_ASSERT(Glob_scope()->Func_def_table().Size() == 0);

  TYPE_PTR  ciph_type = Lower_ctx()->Get_cipher_type(Glob_scope());
  FUNC_ITER func_iter = vec_glob_scope->Begin_func();
  FUNC_ITER end_iter  = vec_glob_scope->End_func();
  for (; func_iter != end_iter; ++func_iter) {
    FUNC_PTR func      = *func_iter;
    FUNC_PTR sihe_func = Glob_scope()->New_func(func->Name(), func->Spos());

    ENTRY_PTR          entry      = func->Entry_point();
    SIGNATURE_TYPE_PTR sig        = entry->Type()->Cast_to_sig();
    PARAM_ITER         param_iter = sig->Begin_param();
    PARAM_ITER         param_end  = sig->End_param();

    // gen param for SIHE domain function signature
    SIGNATURE_TYPE_PTR sihe_func_sig = Glob_scope()->New_sig_type();
    for (; param_iter != param_end; ++param_iter) {
      PARAM_PTR param = *param_iter;
      TYPE_PTR  param_type =
          (param->Type()->Is_array() ? ciph_type : param->Type());
      if (param->Is_ret()) {
        Glob_scope()->New_ret_param(param_type, sihe_func_sig);
      } else {
        Glob_scope()->New_param(param->Name(), param_type, sihe_func_sig,
                                sig->Spos());
      }
    }
    sihe_func_sig->Set_complete();
    ENTRY_PTR sihe_entry = Glob_scope()->New_entry_point(
        sihe_func_sig, sihe_func, entry->Name(), entry->Spos());
    if (entry->Is_program_entry()) sihe_entry->Set_program_entry();

    Func_map().insert({func->Id(), sihe_func->Id()});
    Entry_map().insert({entry->Id(), sihe_entry->Id()});
  }
}

NODE_PTR VECTOR2SIHE_IMPL::Lower_bin_arith_node(VECTOR2SIHE_CTX& ctx,
                                                NODE_PTR node, OPCODE sihe_op,
                                                NODE_PTR op0, NODE_PTR op1) {
  CMPLR_ASSERT(node->Operator() == nn::vector::ADD ||
                   node->Operator() == nn::vector::MUL,
               "only support add/mul/sub");
  TYPE_PTR rtype = node->Rtype();
  AIR_ASSERT(rtype->Is_array());
  uint64_t msg_len = rtype->Cast_to_arr()->Elem_count();
  ctx.Update_max_msg_len(msg_len);

  CONTAINER* cont = ctx.Container();
  // op0, op1 must mu lowered
  CMPLR_ASSERT(op0->Container() == cont, "opnd 0 must be from new container");
  CMPLR_ASSERT(op1->Container() == cont, "opnd 1 must be from new container");

  // canonicalize binary node: set cipher type child as child0
  TYPE_ID cipher_type_id = ctx.Lower_ctx().Get_cipher_type_id();
  if (op0->Rtype_id() != cipher_type_id) {
    std::swap(op0, op1);
  }
  CMPLR_ASSERT(cipher_type_id == op0->Rtype_id(), "child0 must be ciphertext");

  // encode child1 if it is raw vector.
  TYPE_PTR child1_rtype = op1->Rtype();
  TYPE_PTR plain_type   = ctx.Lower_ctx().Get_plain_type(cont->Glob_scope());
  if (op1->Rtype()->Is_array()) {
    SIHE_GEN sihe_gen(cont, &ctx.Lower_ctx());
    op1 = sihe_gen.Gen_encode(op1, plain_type, node->Spos());
  }
  CMPLR_ASSERT(cipher_type_id == op1->Rtype()->Id() ||
                   plain_type == op1->Rtype() || op1->Rtype()->Is_float(),
               "child1 must be ciphertext, plaintext, or float");

  // 4. gen binary arithmatic node of SIHE
  NODE_PTR bin_node = cont->New_bin_arith(sihe_op, op0, op1, node->Spos());
  bin_node->Set_rtype(cipher_type_id);
  return bin_node;
}

NODE_PTR VECTOR2SIHE_IMPL::Lower_rotate_node(VECTOR2SIHE_CTX& ctx,
                                             NODE_PTR node, NODE_PTR op0,
                                             NODE_PTR op1) {
  CONTAINER* cont = ctx.Container();
  // op0, op1 must mu lowered
  CMPLR_ASSERT(op0->Container() == cont, "opnd 0 must be from new container");
  CMPLR_ASSERT(op0->Rtype_id() == ctx.Lower_ctx().Get_cipher_type_id(),
               "only support rotate cipher type value");
  CMPLR_ASSERT(op1->Container() == cont, "opnd 1 must be from new container");
  CMPLR_ASSERT(op1->Rtype()->Is_signed_int(), "rotate index must be integer");

  // gen SIHE rotate node
  OPCODE   rotate_op(SIHE_DOMAIN::ID, SIHE_OPERATOR::ROTATE);
  NODE_PTR rotate_node =
      cont->New_cust_node(rotate_op, op0->Rtype(), node->Spos());
  rotate_node->Set_child(0, op0);
  rotate_node->Set_child(1, op1);

  const char* rot_idx_key   = "nums";
  uint32_t    rot_idx_count = 0;
  const int*  rot_idx       = node->Attr<int>(rot_idx_key, &rot_idx_count);
  AIR_ASSERT(rot_idx != nullptr && rot_idx_count > 0);
  rotate_node->Set_attr(rot_idx_key, rot_idx, rot_idx_count);
  return rotate_node;
}

}  // namespace sihe
}  // namespace fhe
