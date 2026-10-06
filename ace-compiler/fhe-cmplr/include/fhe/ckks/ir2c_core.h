//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_IR2C_CORE_H
#define FHE_CKKS_IR2C_CORE_H
#include <cstdint>
#include <string>
#include <vector>

#include "air/base/container.h"
#include "air/base/container_decl.h"
#include "air/base/st_decl.h"
#include "air/core/ir2c_handler.h"
#include "air/util/debug.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/ckks/ir2c_ctx.h"
#include "fhe/core/lower_ctx.h"

namespace fhe {
namespace ckks {
class IR2C_CORE : public air::core::IR2C_HANDLER {
public:
  //! @brief Emit "0" for operator ZERO
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
  RETV Handle_ld(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX&           ctx    = visitor->Context();
    air::base::NODE_PTR parent = ctx.Parent(1);
    air::base::TYPE_ID  type   = node->Addr_datum()->Type_id();
    air::base::OPCODE   p_opc  = visitor->Parent(1)->Opcode();

    if (p_opc != air::core::OPC_RETV && p_opc != air::core::OPC_CALL &&
        (ctx.Is_cipher_type(type) || ctx.Is_cipher3_type(type) ||
         ctx.Is_plain_type(type))) {
      ctx << "&";
    }
    // const core::CTX_PARAM& param = ctx.Lower_ctx().Get_ctx_param();
    air::core::IR2C_HANDLER::template Handle_ld<RETV, VISITOR>(visitor, node);
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

  template <typename RETV, typename VISITOR>
  void Handle_ldc(VISITOR* visitor, air::base::NODE_PTR node) {
    // TODO Add special handling for multi_thread
    air::core::IR2C_HANDLER::template Handle_ldc<RETV, VISITOR>(visitor, node);
  }

  //! @brief Special handling for CORE RETV operator to call Set_output_data
  //! for return value
  template <typename RETV, typename VISITOR>
  void Handle_retv(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX&           ctx = visitor->Context();
    air::base::NODE_PTR val = node->Child(0);
    if (node->Func_scope()->Owning_func()->Entry_point()->Is_program_entry()) {
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
    if (val->Opcode().Domain() == fhe::ckks::CKKS_DOMAIN::ID) {
      visitor->template Visit<RETV>(val);
    } else {
      air::core::IR2C_HANDLER::template Handle_st<RETV, VISITOR>(visitor, node);
    }
  }

  //! @brief Special handling for CORE STP operator
  template <typename RETV, typename VISITOR>
  void Handle_stp(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX&           ctx = visitor->Context();
    air::base::NODE_PTR val = node->Child(0);

    if (val->Opcode().Domain() == fhe::ckks::CKKS_DOMAIN::ID) {
      visitor->template Visit<RETV>(val);
    } else if (ctx.Is_cipher_type(val->Rtype_id())) {
      ctx << "Copy_ciph(&";
      ctx.Emit_preg_id(node->Preg_id());
      ctx << ", ";
      if (val->Opcode() == air::core::OPC_ILD) {
        ctx << "&";
      }
      visitor->template Visit<RETV>(val);
      ctx << ")";
    } else {
      ctx.Emit_preg_id(node->Preg_id());
      ctx << " = ";
      visitor->template Visit<RETV>(val);
    }
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

    air::core::IR2C_HANDLER::template Handle_ist<RETV, VISITOR>(visitor, node);
  }

  //! @brief Handle CALL for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_call(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX& ctx = visitor->Context();
    if (node->Has_ret_var()) {
      ctx.Emit_preg_id(node->Ret_preg_id());
      ctx << " = ";
    }
    ctx.Emit_sym(node->Entry());
    ctx << "(";
    for (uint32_t i = 0; i < node->Num_child(); ++i) {
      if (i > 0) {
        ctx << ", ";
      }
      visitor->template Visit<RETV>(node->Child(i));
    }
    ctx << ")";
    return RETV();
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
    return;
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
    air::base::IR2C_CTX& ctx = visitor->Context();
    ctx << "Dump_cipher_msg(";
    visitor->template Visit<RETV>(node->Child(0));  // name
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(1));  // cipher
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(2));  // size
    ctx << ")";
  }
};
}  // namespace ckks
}  // namespace fhe

#endif