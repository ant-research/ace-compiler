//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_POLY_IR2C_CORE_H
#define FHE_POLY_IR2C_CORE_H

#include "air/base/container.h"
#include "air/core/ir2c_handler.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/core/lower_ctx.h"

namespace fhe {

namespace poly {

//! @brief Special IR2C handler for CORE operators
class IR2C_CORE : public air::core::IR2C_HANDLER {
public:
  //! @brief Emit "0" for operator ZERO
  //! @tparam RETV Return Type
  //! @tparam VISITOR Visitor Type
  //! @param visitor Pointer to visitor
  //! @param node ZERO node
  template <typename RETV, typename VISITOR>
  void Handle_zero(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->Context() << "0";
  }

  //! @brief Emit "1" for operator ONE
  template <typename RETV, typename VISITOR>
  void Handle_one(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->Context() << "1";
  }

  //! @brief Special handling for CORE LD operator
  template <typename RETV, typename VISITOR>
  void Handle_ld(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX&          ctx   = visitor->Context();
    air::base::TYPE_ID type  = node->Addr_datum()->Type_id();
    air::base::OPCODE  p_opc = visitor->Parent(1)->Opcode();
    bool               get_msg =
        ctx.Is_cipher_type(type) && (p_opc == nn::vector::OPC_CONV_REF);
    if (get_msg) {
      // CONV_REF requires message as input, call Get_msg here
      ctx << "Get_msg(";
    }
    if (p_opc != air::core::OPC_RETV && p_opc != air::core::OPC_CALL &&
        (ctx.Is_cipher_type(type) || ctx.Is_cipher3_type(type) ||
         ctx.Is_plain_type(type))) {
      ctx << "&";
    }
    air::core::IR2C_HANDLER::template Handle_ld<RETV, VISITOR>(visitor, node);
    if (get_msg) {
      ctx << ")";
    }
  }

  //! @brief Special handling for CORE LDP operator
  template <typename RETV, typename VISITOR>
  void Handle_ldp(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX&          ctx  = visitor->Context();
    air::base::TYPE_ID type = node->Preg()->Type_id();
    air::base::OPCODE  retv =
        air::base::OPCODE(air::core::CORE, air::core::RETV);
    air::base::OPCODE call =
        air::base::OPCODE(air::core::CORE, air::core::CALL);
    if (visitor->Parent(1)->Opcode() != retv &&
        visitor->Parent(1)->Opcode() != call &&
        (ctx.Is_cipher_type(type) || ctx.Is_cipher3_type(type) ||
         ctx.Is_plain_type(type))) {
      ctx << "&";
    }
    air::core::IR2C_HANDLER::template Handle_ldp<RETV, VISITOR>(visitor, node);
  }

  //! @brief Special handling for CORE LDPF operator
  template <typename RETV, typename VISITOR>
  void Handle_ldpf(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX& ctx = visitor->Context();
    if (ctx.Is_poly_type(node->Rtype()->Id())) {
      ctx << "&";
    }
    ctx.Emit_preg_id(node->Preg_id());
    ctx << ".";
    ctx.Emit_field(node);
  }

  //! @brief Special handling for CORE RETV operator to call Set_output_data
  //! for return value
  template <typename RETV, typename VISITOR>
  void Handle_retv(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX& ctx = visitor->Context();
    if (node->Func_scope()->Owning_func()->Entry_point()->Is_program_entry()) {
      air::base::NODE_PTR val = node->Child(0);
      AIR_ASSERT(val->Opcode() ==
                 air::base::OPCODE(air::core::CORE, air::core::OPCODE::LD));
      const char* name = val->Addr_datum()->Base_sym()->Name()->Char_str();
      ctx.Set_output_name(name);
      ctx << "Set_output_data(\"" << name << "\", 0, &";
      ctx.Emit_var(val);
      ctx << ")";
      ctx.End_stmt(node);
      ctx.Begin_stmt(node);
      ctx << "return true";
    } else {
      air::core::IR2C_HANDLER::template Handle_retv<RETV, VISITOR>(visitor,
                                                                   node);
    }
  }

  //! @brief Special handling for CORE ST operator
  template <typename RETV, typename VISITOR>
  void Handle_st(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX&           ctx = visitor->Context();
    air::base::NODE_PTR val = node->Child(0);
    if (val->Opcode() ==
        air::base::OPCODE(fhe::poly::POLYNOMIAL_DID, fhe::poly::DECOMP)) {
      ctx << "Decomp(";
      ctx.Emit_var(node);
      ctx << ", ";
      visitor->template Visit<RETV>(val->Child(0));
      ctx << ", ";
      visitor->template Visit<RETV>(val->Child(1));
      ctx << ")";
      return;
    }
    if (val->Opcode() == air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                           fhe::ckks::CKKS_OPERATOR::ENCODE)) {
      ctx.template Emit_encode<RETV, VISITOR>(visitor, node, val);
      return;
    }
    if (val->Opcode() ==
        air::base::OPCODE(fhe::poly::POLYNOMIAL_DID, fhe::poly::MOD_DOWN)) {
      ctx << "Mod_down(";
      ctx.Emit_var(node);
      ctx << ", ";
      visitor->template Visit<RETV>(val->Child(0));
      ctx << ")";
      return;
    }
    if (val->Opcode() ==
        air::base::OPCODE(fhe::poly::POLYNOMIAL_DID, fhe::poly::MOD_UP)) {
      ctx << "Mod_up(";
      ctx.Emit_var(node);
      ctx << ", ";
      visitor->template Visit<RETV>(val->Child(0));
      ctx << ", ";
      visitor->template Visit<RETV>(val->Child(1));
      ctx << ")";
      return;
    }
    if (val->Opcode() ==
        air::base::OPCODE(fhe::poly::POLYNOMIAL_DID, fhe::poly::DECOMP_MODUP)) {
      ctx << "Decomp_modup(";
      ctx.Emit_var(node);
      ctx << ", ";
      visitor->template Visit<RETV>(val->Child(0));
      ctx << ", ";
      visitor->template Visit<RETV>(val->Child(1));
      ctx << ")";
      return;
    }
    if (ctx.Is_cipher_type(node->Addr_datum()->Type_id()) &&
        val->Opcode() == air::base::OPCODE(air::core::CORE, air::core::ZERO)) {
      ctx << "Zero_ciph(&";
      ctx.Emit_var(node);
      ctx << ")";
      return;
    }
    if (ctx.Is_cipher_type(node->Addr_datum()->Type_id()) &&
            val->Opcode() ==
                air::base::OPCODE(air::core::CORE, air::core::LD) ||
        val->Opcode() == air::base::OPCODE(air::core::CORE, air::core::LDP)) {
      ctx << "Copy_ciph(&";
      ctx.Emit_var(node);
      ctx << ", ";
      visitor->template Visit<RETV>(val);
      ctx << ")";
      return;
    }
    if (val->Is_lib_call()) {
      // LIB_CALL may turn store lhs into first parameter
      visitor->template Visit<RETV>(val);
      return;
    }
    ctx.Emit_var(node);
    ctx << " = ";
    visitor->template Visit<RETV>(val);
  }

  template <typename RETV, typename VISITOR>
  void Handle_stp(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX&           ctx = visitor->Context();
    air::base::NODE_PTR val = node->Child(0);
    if (ctx.Is_cipher_type(val->Rtype_id())) {
      if (val->Opcode() == fhe::ckks::OPC_BOOTSTRAP) {
        ctx << "Bootstrap(&";
        ctx.Emit_preg_id(node->Preg_id());
        ctx << ", ";
        visitor->template Visit<RETV>(val->Child(0));

        const char* mul_lev_attr_name =
            ctx.Lower_ctx().Attr_name(fhe::core::FHE_ATTR_KIND::LEVEL);
        const uint32_t* mul_lev = val->Attr<uint32_t>(mul_lev_attr_name);
        if (mul_lev == nullptr) {
          ctx << ", " << 0;
        } else {
          ctx << ", " << *mul_lev;
        }
      } else {
        ctx << "Copy_ciph(&";
        ctx.Emit_preg_id(node->Preg_id());
        ctx << ", ";
        if (val->Opcode() == air::core::OPC_ILD) {
          ctx << "&";
        }
        visitor->template Visit<RETV>(val);
      }
      ctx << ")";
      return;
    }
    if (val->Is_lib_call()) {
      // LIB_CALL may turn store lhs into first parameter
      visitor->template Visit<RETV>(val);
      return;
    }
    ctx.Emit_preg_id(node->Preg_id());
    ctx << " = ";
    visitor->template Visit<RETV>(val);
  }

  //! @brief Special handling for CORE IST operator
  template <typename RETV, typename VISITOR>
  void Handle_ist(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX&          ctx            = visitor->Context();
    air::base::TYPE_ID access_type_id = node->Access_type_id();
    if (ctx.Is_cipher_type(access_type_id)) {
      ctx << "Copy_ciph(";
      // 1. emit address
      air::base::NODE_PTR addr_child = node->Child(0);
      if (addr_child->Opcode() == air::core::OPC_ARRAY) {
        ctx << "&";
      }
      visitor->template Visit<RETV>(addr_child);
      ctx << ", ";

      // 2. emit istored cipher
      air::base::NODE_PTR rhs = node->Child(1);
      visitor->template Visit<RETV>(rhs);
      ctx << ")";
      return;
    }

    IR2C_HANDLER::Handle_ist<RETV>(visitor, node);
  }

  //! @brief Special handling for CORE LDF operator
  template <typename RETV, typename VISITOR>
  void Handle_ldf(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX& ctx = visitor->Context();
    if (ctx.Is_poly_type(node->Rtype()->Id())) {
      ctx << "&";
    }
    ctx.Emit_var(node);
    ctx << ".";
    ctx.Emit_field(node);
  }

  //! @brief Special handling for CORE STF operator
  template <typename RETV, typename VISITOR>
  void Handle_stf(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX&           ctx = visitor->Context();
    air::base::NODE_PTR val = node->Child(0);
    if (ctx.Is_poly_type(val->Rtype_id()) &&
        val->Opcode() == air::base::OPCODE(air::base::OPCODE(
                             air::core::CORE, air::core::OPCODE::LDF))) {
      ctx << "Copy_poly(&";
      ctx.Emit_var(node);
      ctx << ".";
      ctx.Emit_field(node);
      ctx << ", ";
      visitor->template Visit<RETV>(val);
      ctx << ")";
      return;
    } else if (ctx.Is_poly_type(val->Rtype_id()) &&
               val->Opcode() == air::base::OPCODE(air::base::OPCODE(
                                    fhe::poly::POLYNOMIAL_DID,
                                    fhe::poly::OPCODE::RESCALE))) {
      ctx << "Rescale(&";
      ctx.Emit_var(node);
      ctx << ".";
      ctx.Emit_field(node);
      ctx << ", ";
      visitor->template Visit<RETV>(val);
      ctx << ")";
      return;
    }
    ctx.Emit_var(node);
    ctx << ".";
    ctx.Emit_field(node);
    ctx << " = ";
    visitor->template Visit<RETV>(node->Child(0));
  }

  //! @brief Special handling for CORE STID operator
  template <typename RETV, typename VISITOR>
  void Handle_stpf(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX&           ctx = visitor->Context();
    air::base::NODE_PTR val = node->Child(0);
    if (ctx.Is_poly_type(val->Rtype_id()) &&
        val->Opcode() == air::base::OPCODE(air::base::OPCODE(
                             air::core::CORE, air::core::OPCODE::LDF))) {
      ctx << "Copy_poly(&";
      ctx.Emit_preg_id(node->Preg_id());
      ctx << ".";
      ctx.Emit_field(node);
      ctx << ", ";
      visitor->template Visit<RETV>(val);
      ctx << ")";
      return;
    } else if (ctx.Is_poly_type(val->Rtype_id()) &&
               val->Opcode() == air::base::OPCODE(air::base::OPCODE(
                                    fhe::poly::POLYNOMIAL_DID,
                                    fhe::poly::OPCODE::RESCALE))) {
      ctx << "Rescale(&";
      ctx.Emit_preg_id(node->Preg_id());
      ctx << ".";
      ctx.Emit_field(node);
      ctx << ", ";
      visitor->template Visit<RETV>(val);
      ctx << ")";
      return;
    }
    ctx.Emit_preg_id(node->Preg_id());
    ctx << ".";
    ctx.Emit_field(node);
    ctx << " = ";
    visitor->template Visit<RETV>(node->Child(0));
  }

  //! @brief Special handling for CORE VALIDATE operator
  template <typename RETV, typename VISITOR>
  void Handle_validate(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX& ctx = visitor->Context();
    ctx << "Validate(";
    visitor->template Visit<RETV>(node->Child(0));
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(1));
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(2));
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(3));
    ctx << ")";
  }

  //! @brief Special handling for CORE TM_START operator
  template <typename RETV, typename VISITOR>
  void Handle_tm_start(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX& ctx = visitor->Context();
    ctx << "Tm_start(";
    ctx.Emit_constant_str_init(node->Const());
    ctx << ")";
  }

  //! @brief Special handling for CORE TM_TAKEN operator
  template <typename RETV, typename VISITOR>
  void Handle_tm_taken(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX& ctx = visitor->Context();
    ctx << "Tm_taken(";
    ctx.Emit_constant_str_init(node->Const());
    ctx << ")";
  }

  //! @brief Special handling for CORE DUMP operator
  template <typename RETV, typename VISITOR>
  void Handle_dump_var(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX& ctx = visitor->Context();
    ctx << "Dump_cipher_msg(";
    visitor->template Visit<RETV>(node->Child(0));  // name
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(1));  // cipher
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(2));  // size
    ctx << ")";
  }

};  // IR2C_CORE

}  // namespace poly

}  // namespace fhe

#endif
