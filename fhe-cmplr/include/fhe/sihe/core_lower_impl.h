//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CORE_GENERAL_LOWER_HANDLER_H
#define FHE_CORE_GENERAL_LOWER_HANDLER_H

#include "air/base/container.h"
#include "air/base/container_decl.h"
#include "air/base/st.h"
#include "air/base/st_decl.h"
#include "air/core/default_handler.h"
#include "air/core/opcode.h"
#include "air/util/debug.h"
#include "fhe/sihe/sihe_gen.h"
#include "fhe/sihe/sihe_opcode.h"
#include "fhe/sihe/vector2sihe_ctx.h"

namespace fhe {
namespace core {
using namespace air::base;
using namespace fhe::sihe;

class CORE_LOWER : public air::core::DEFAULT_HANDLER {
public:
  template <typename RETV, typename VISITOR>
  RETV Handle_idname(VISITOR* visitor, NODE_PTR idname) {
    VECTOR2SIHE_CTX& ctx        = visitor->Context();
    CONTAINER*       cntr       = ctx.Container();
    FUNC_SCOPE*      func_scope = cntr->Parent_func_scope();

    TYPE_PTR ciph_type =
        ctx.Lower_ctx().Get_cipher_type(&func_scope->Glob_scope());
    ADDR_DATUM_PTR vec_formal = idname->Addr_datum();
    TYPE_PTR       vec_type   = vec_formal->Type();
    AIR_ASSERT_MSG(vec_type->Is_array(), "only support array type msg");
    ctx.Update_max_msg_len(vec_type->Cast_to_arr()->Elem_count());

    ADDR_DATUM_PTR fhe_formal =
        ctx.Get_sihe_addr_datum(vec_formal->Id(), *func_scope);
    AIR_ASSERT(fhe_formal != ADDR_DATUM_PTR());
    NODE_PTR fhe_idname = cntr->New_idname(fhe_formal, idname->Spos());
    return RETV(fhe_idname);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ld(VISITOR* visitor, NODE_PTR ld_node) {
    VECTOR2SIHE_CTX& ctx        = visitor->Context();
    CONTAINER*       cntr       = ctx.Container();
    FUNC_SCOPE*      func_scope = cntr->Parent_func_scope();

    ADDR_DATUM_ID  vec_sym_id = ld_node->Addr_datum_id();
    ADDR_DATUM_PTR fhe_sym = ctx.Get_sihe_addr_datum(vec_sym_id, *func_scope);
    AIR_ASSERT(fhe_sym != ADDR_DATUM_PTR());
    NODE_PTR fhe_ld = cntr->New_ld(fhe_sym, ld_node->Spos());
    return RETV(fhe_ld);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_lda(VISITOR* visitor, NODE_PTR lda_node) {
    VECTOR2SIHE_CTX& ctx        = visitor->Context();
    CONTAINER*       cntr       = ctx.Container();
    FUNC_SCOPE*      func_scope = cntr->Parent_func_scope();

    ADDR_DATUM_ID  array_sym_id = lda_node->Addr_datum_id();
    ADDR_DATUM_PTR array_sym =
        ctx.Get_sihe_addr_datum(array_sym_id, *func_scope);
    AIR_ASSERT(array_sym != ADDR_DATUM_PTR());
    NODE_PTR new_lda =
        cntr->New_lda(array_sym, POINTER_KIND::FLAT32, lda_node->Spos());
    return RETV(new_lda);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ldp(VISITOR* visitor, NODE_PTR ldp_node) {
    VECTOR2SIHE_CTX& ctx        = visitor->Context();
    CONTAINER*       cntr       = ctx.Container();
    FUNC_SCOPE*      func_scope = cntr->Parent_func_scope();

    PREG_ID  preg_id = ldp_node->Preg_id();
    PREG_PTR preg    = ctx.Get_sihe_preg(preg_id, *func_scope);
    NODE_PTR new_ldp = cntr->New_ldp(preg, ldp_node->Spos());
    return RETV(new_ldp);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ldc(VISITOR* visitor, NODE_PTR ldc_node) {
    VECTOR2SIHE_CTX& ctx     = visitor->Context();
    CONTAINER*       cntr    = ctx.Container();
    SPOS             spos    = ldc_node->Spos();
    NODE_PTR         new_ldc = cntr->New_ldc(ldc_node->Const(), spos);
    if (!new_ldc->Rtype()->Is_array()) {
      return new_ldc;
    }
    AIR_ASSERT(ldc_node->Rtype()->Is_array());
    ARRAY_TYPE_PTR arr_type = ldc_node->Rtype()->Cast_to_arr();
    uint64_t       len      = arr_type->Elem_count();
    // append SIHE.encode to LDC
    PRIM_TYPE_PTR u32_type =
        cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_U32);
    OPCODE   opcode(SIHE_DOMAIN::ID, SIHE_OPERATOR::ENCODE);
    TYPE_PTR plain_type = ctx.Lower_ctx().Get_plain_type(cntr->Glob_scope());
    NODE_PTR encode     = cntr->New_cust_node(opcode, plain_type, spos);
    encode->Set_child(0, new_ldc);
    encode->Set_child(1, cntr->New_intconst(u32_type, len, spos));
    return encode;
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_st(VISITOR* visitor, NODE_PTR store_node) {
    VECTOR2SIHE_CTX& ctx        = visitor->Context();
    CONTAINER*       cntr       = ctx.Container();
    FUNC_SCOPE*      func_scope = cntr->Parent_func_scope();

    NODE_PTR child     = store_node->Child(0);
    NODE_PTR new_child = visitor->template Visit<RETV>(child).Node();
    TYPE_PTR rhs_type  = new_child->Rtype();
    // set var init with OPCODE::ZERO as ciphertext
    if (new_child->Domain() == air::core::CORE &&
        new_child->Operator() == air::core::OPCODE::ZERO) {
      rhs_type = ctx.Lower_ctx().Get_cipher_type(&func_scope->Glob_scope());
    }

    // 2. gen new addr_datum in FHE domain function scope
    ADDR_DATUM_PTR fhe_addr_datum = ctx.Get_or_gen_sihe_addr_datum(
        store_node->Addr_datum(), *func_scope, rhs_type);
    STMT_PTR new_st =
        cntr->New_st(new_child, fhe_addr_datum, store_node->Spos());
    return RETV(new_st->Node());
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_stp(VISITOR* visitor, NODE_PTR stp_node) {
    VECTOR2SIHE_CTX& ctx        = visitor->Context();
    CONTAINER*       cntr       = ctx.Container();
    FUNC_SCOPE*      func_scope = cntr->Parent_func_scope();

    // 1. handle rhs node
    NODE_PTR child     = stp_node->Child(0);
    NODE_PTR new_child = visitor->template Visit<RETV>(child).Node();
    TYPE_PTR rhs_type  = new_child->Rtype();
    // set preg init with OPCODE::ZERO as ciphertext
    if (new_child->Domain() == air::core::CORE &&
        new_child->Operator() == air::core::OPCODE::ZERO) {
      rhs_type = ctx.Lower_ctx().Get_cipher_type(&func_scope->Glob_scope());
    }

    // 2. gen new preg in FHE domain function scope
    PREG_PTR fhe_preg =
        ctx.Get_or_gen_sihe_preg(stp_node->Preg(), *func_scope, rhs_type);

    STMT_PTR new_stp = cntr->New_stp(new_child, fhe_preg, stp_node->Spos());
    return RETV(new_stp->Node());
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_array(VISITOR* visitor, NODE_PTR arr_node) {
    CONTAINER*  cntr       = visitor->Context().Container();
    GLOB_SCOPE* glob_scope = visitor->Context().Glob_scope();

    // 1. handle CORE.array base node
    NODE_PTR arr_base     = arr_node->Array_base();
    NODE_PTR new_arr_base = visitor->template Visit<RETV>(arr_base).Node();
    NODE_PTR new_arr_node =
        cntr->New_array(new_arr_base, arr_node->Array_dim(), arr_node->Spos());

    // 2. handle array index
    for (uint32_t idx_id = 0; idx_id < arr_node->Array_dim(); ++idx_id) {
      NODE_PTR idx_child = arr_node->Array_idx(idx_id);
      NODE_PTR new_idx   = visitor->template Visit<RETV>(idx_child).Node();
      cntr->Set_array_idx(new_arr_node, idx_id, new_idx);
    }
    return RETV(new_arr_node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ild(VISITOR* visitor, NODE_PTR ild_node) {
    NODE_PTR   addr_child = ild_node->Child(0);
    NODE_PTR   new_addr   = visitor->template Visit<RETV>(addr_child).Node();
    CONTAINER* cntr       = visitor->Context().Container();
    NODE_PTR   new_ild    = cntr->New_ild(new_addr, ild_node->Spos());
    return RETV(new_ild);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ist(VISITOR* visitor, NODE_PTR ist_node) {
    VECTOR2SIHE_CTX& ctx        = visitor->Context();
    CONTAINER*       cntr       = ctx.Container();
    GLOB_SCOPE*      glob_scope = ctx.Glob_scope();
    FUNC_SCOPE*      func_scope = cntr->Parent_func_scope();

    // 1. handle rhs node of istore
    NODE_PTR rhs_child     = ist_node->Child(1);
    NODE_PTR new_rhs_child = visitor->template Visit<RETV>(rhs_child).Node();
    TYPE_PTR rhs_type      = new_rhs_child->Rtype();
    TYPE_ID  rhs_type_id   = new_rhs_child->Rtype_id();
    // set var init with OPCODE::ZERO as ciphertext
    if (new_rhs_child->Opcode() == air::core::OPC_ZERO) {
      rhs_type_id = ctx.Lower_ctx().Get_cipher_type_id();
    }

    NODE_PTR addr_child = ist_node->Child(0);
    AIR_ASSERT(addr_child->Opcode() == air::core::OPC_ARRAY);

    // 2. handle base address of istore.
    TYPE_PTR access_type = ist_node->Access_type();
    if (ctx.Lower_ctx().Is_cipher_type(rhs_type_id) ||
        ctx.Lower_ctx().Is_plain_type(rhs_type_id)) {
      NODE_PTR lda_child = addr_child->Array_base();
      AIR_ASSERT(lda_child->Opcode() == air::core::OPC_LDA);
      ADDR_DATUM_PTR vec_array_sym  = lda_child->Addr_datum();
      ARRAY_TYPE_PTR vec_array_type = vec_array_sym->Type()->Cast_to_arr();

      TYPE_PTR fhe_array_type = glob_scope->New_arr_type(
          vec_array_type->Name(), rhs_type, vec_array_type->First_dim(),
          rhs_type->Spos());
      (void)ctx.Get_or_gen_sihe_addr_datum(vec_array_sym, *func_scope,
                                           fhe_array_type);
    } else {
      AIR_ASSERT(access_type->Id() == rhs_type_id);
    }

    NODE_PTR new_addr_child = visitor->template Visit<RETV>(addr_child).Node();
    STMT_PTR new_ist =
        cntr->New_ist(new_addr_child, new_rhs_child, ist_node->Spos());
    return RETV(new_ist->Node());
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_do_loop(VISITOR* visitor, NODE_PTR do_loop) {
    VECTOR2SIHE_CTX& ctx        = visitor->Context();
    CONTAINER*       cntr       = ctx.Container();
    FUNC_SCOPE*      func_scope = cntr->Parent_func_scope();

    NODE_PTR loop_incr = do_loop->Loop_incr();
    NODE_PTR ld_iv     = loop_incr->Child(0);
    AIR_ASSERT(ld_iv->Opcode() == air::core::OPC_LD);
    ADDR_DATUM_PTR vec_loop_iv = ld_iv->Addr_datum();
    ADDR_DATUM_PTR fhe_loop_iv = ctx.Get_or_gen_sihe_addr_datum(
        vec_loop_iv, *func_scope, vec_loop_iv->Type());
    do_loop->Set_iv(fhe_loop_iv);

    return air::core::DEFAULT_HANDLER::Handle_do_loop<RETV>(visitor, do_loop);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_call(VISITOR* visitor, NODE_PTR call_node) {
    VECTOR2SIHE_CTX& ctx        = visitor->Context();
    CONTAINER*       cntr       = ctx.Container();
    GLOB_SCOPE*      glob_scope = cntr->Glob_scope();
    FUNC_SCOPE*      func_scope = cntr->Parent_func_scope();

    ENTRY_PTR sihe_entry = ctx.Get_sihe_entry(call_node->Entry(), *glob_scope);
    AIR_ASSERT(sihe_entry != ENTRY_PTR());

    // TODO: temperarily treat return val as cipher
    TYPE_PTR ciph_type = ctx.Lower_ctx().Get_cipher_type(glob_scope);
    PREG_PTR ret_preg =
        ctx.Get_or_gen_sihe_preg(call_node->Ret_preg(), *func_scope, ciph_type);
    STMT_PTR sihe_call = cntr->New_call(
        sihe_entry, ret_preg, call_node->Num_child(), call_node->Spos());

    // lower child nodes
    for (uint32_t id = 0; id < call_node->Num_child(); ++id) {
      NODE_PTR child      = call_node->Child(id);
      NODE_PTR sihe_child = visitor->template Visit<RETV>(child).Node();
      sihe_call->Node()->Set_child(id, sihe_child);
    }
    return RETV(sihe_call->Node());
  }
};

}  // namespace core
}  // namespace fhe
#endif
