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
#include "air/core/opcode.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/ckks/ir2c_core.h"
#include "fhe/core/lower_ctx.h"
#include "fhe/core/rt_timing.h"
#include "fhe/poly/ir2c_ctx.h"
#include "fhe/poly/opcode.h"
#include "nn/core/data_scheme.h"

namespace fhe {

namespace poly {

//! @brief Special IR2C handler for CORE operators
class IR2C_CORE : public fhe::ckks::IR2C_CORE {
public:
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
        ctx.Is_rns_poly_type(type)) {
      ctx << "&";
    }
    fhe::ckks::IR2C_CORE::template Handle_ld<RETV, VISITOR>(visitor, node);
    if (get_msg) {
      ctx << ")";
    }
  }

  //! @brief Special handling for CORE LDP operator
  template <typename RETV, typename VISITOR>
  void Handle_ldp(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX&          ctx   = visitor->Context();
    air::base::TYPE_ID type  = node->Preg()->Type_id();
    air::base::OPCODE  p_opc = visitor->Parent(1)->Opcode();
    if (p_opc != air::core::OPC_RETV && p_opc != air::core::OPC_CALL &&
        ctx.Is_rns_poly_type(type)) {
      ctx << "&";
    }
    fhe::ckks::IR2C_CORE::template Handle_ldp<RETV, VISITOR>(visitor, node);
  }

  //! @brief Special handling for CORE LDPF operator
  template <typename RETV, typename VISITOR>
  void Handle_ldpf(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX&         ctx   = visitor->Context();
    air::base::OPCODE p_opc = visitor->Parent(1)->Opcode();
    if (p_opc != air::core::OPC_RETV && p_opc != air::core::OPC_CALL &&
        ctx.Is_rns_poly_type(node->Rtype()->Id())) {
      ctx << "&";
    }
    ctx.Emit_preg_id(node->Preg_id());
    ctx << ".";
    ctx.Emit_field(node);
  }

  //! @brief Special handling for CORE ILD operator
  template <typename RETV, typename VISITOR>
  void Handle_ild(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX& ctx = visitor->Context();
    if (ctx.Is_rns_poly_type(node->Rtype()->Id())) {
      air::base::NODE_PTR parent = visitor->Parent(1);
      if (parent != air::base::Null_ptr && parent->Is_call()) {
        ctx << "*";
      }
      this->template Handle_ild_ist_addr<RETV>(visitor, node->Child(0));
    } else {
      fhe::ckks::IR2C_CORE::template Handle_ild<RETV, VISITOR>(visitor, node);
    }
  }

  //! @brief Special handling for CORE RETV operator to call Set_output_data
  //! for return value
  template <typename RETV, typename VISITOR>
  void Handle_retv(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX&           ctx  = visitor->Context();
    air::base::FUNC_PTR func = node->Func_scope()->Owning_func();
    if (func->Entry_point()->Is_program_entry()) {
      air::base::NODE_PTR val = node->Child(0);
      AIR_ASSERT(val->Opcode() ==
                 air::base::OPCODE(air::core::CORE, air::core::OPCODE::LD));
      const char* name = val->Addr_datum()->Base_sym()->Name()->Char_str();
      ctx.Set_output_name(name);
      uint32_t num_chunk = nn::core::Num_data_chunk(node);
      if (num_chunk > 1) {
        AIR_ASSERT(val->Rtype()->Is_array());
        AIR_ASSERT(val->Rtype()->Cast_to_arr()->Elem_count() == num_chunk);
        ctx << "for (int output_idx = 0; output_idx < " << num_chunk
            << "; ++output_idx) {\n";
        ctx << "    Set_output_data(\"" << name << "\", output_idx, &";
        ctx.Emit_var(val);
        ctx << "[output_idx]);\n";
        ctx << "  }";
      } else {
        ctx << "Set_output_data(\"" << name << "\", 0, &";
        ctx.Emit_var(val);
        ctx << ")";
      }
      ctx.End_stmt(node);
      ctx.Begin_stmt(node);
      ctx << "return true";
    } else {
      // Trace runtime of rotate and relin operations
      const core::LOWER_CTX& lower_ctx = ctx.Lower_ctx();
      if (lower_ctx.Get_func_info(core::FHE_FUNC::ROTATE).Get_func_id() ==
          func->Id()) {
        ctx << "RTLIB_TM_END(" << (uint32_t)(core::RTM_FHE_ROTATE)
            << ", rtm);\n";
        ctx.Begin_stmt(node);
      } else if (lower_ctx.Get_func_info(core::FHE_FUNC::RELIN).Get_func_id() ==
                 func->Id()) {
        ctx << "RTLIB_TM_END(" << (uint32_t)(core::RTM_FHE_RELIN)
            << ", rtm);\n";
        ctx.Begin_stmt(node);
      }
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
      ctx << "Decomp(&";
      ctx.Emit_var(node);
      ctx << ", ";
      visitor->template Visit<RETV>(val->Child(0));
      ctx << ", ";
      visitor->template Visit<RETV>(val->Child(1));
      ctx << ")";
      return;
    }
    if (val->Opcode() ==
        air::base::OPCODE(fhe::poly::POLYNOMIAL_DID, fhe::poly::MOD_DOWN)) {
      ctx << "Mod_down(&";
      ctx.Emit_var(node);
      ctx << ", ";
      visitor->template Visit<RETV>(val->Child(0));
      ctx << ")";
      return;
    }
    if (val->Opcode() ==
        air::base::OPCODE(fhe::poly::POLYNOMIAL_DID, fhe::poly::MOD_UP)) {
      ctx << "Mod_up(&";
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
      ctx << "Decomp_modup(&";
      ctx.Emit_var(node);
      ctx << ", ";
      visitor->template Visit<RETV>(val->Child(0));
      ctx << ", ";
      visitor->template Visit<RETV>(val->Child(1));
      ctx << ")";
      return;
    }
    if (node->Addr_datum()->Type()->Is_array() &&
        val->Opcode() == air::base::OPCODE(air::core::CORE, air::core::ZERO)) {
      ctx << "memset(&";
      ctx.Emit_var(node);
      ctx << ", 0, sizeof(";
      ctx.Emit_var(node);
      ctx << "))";
      return;
    }
    if (ctx.Is_cipher_type(node->Addr_datum()->Type_id())) {
      air::base::OPCODE opc = val->Opcode();
      switch (opc) {
        case air::core::OPC_LD:
        case air::core::OPC_LDP: {
          ctx << "Copy_ciph(&";
          ctx.Emit_var(node);
          ctx << ", ";
          visitor->template Visit<RETV>(val);
          ctx << ")";
          return;
        }
        case air::core::OPC_ZERO: {
          ctx << "Zero_ciph(&";
          ctx.Emit_var(node);
          ctx << ")";
          return;
        }
        case ckks::OPC_BOOTSTRAP: {
          ctx << "Bootstrap(&";
          ctx.Emit_var(node);
          ctx << ", ";
          visitor->template Visit<RETV>(val->Child(0));

          const uint32_t* mul_lev =
              val->Attr<uint32_t>(fhe::core::FHE_ATTR_KIND::LEVEL);
          ctx << ", " << (mul_lev == nullptr ? 0 : *mul_lev) << ")";
          return;
        }
        default:
          break;
      }
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

        const uint32_t* mul_lev =
            val->Attr<uint32_t>(fhe::core::FHE_ATTR_KIND::LEVEL);
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
    } else if (ctx.Is_poly_type(val->Rtype_id())) {
      visitor->template Visit<RETV>(val);
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
    if (ctx.Is_rns_poly_type(access_type_id)) {
      ctx << "Copy_poly(";
      // 1. emit address
      air::base::NODE_PTR addr_child = node->Child(0);
      this->template Handle_ild_ist_addr<RETV>(visitor, addr_child);
      ctx << ", ";

      // 2. emit istored cipher
      air::base::NODE_PTR rhs = node->Child(1);
      visitor->template Visit<RETV>(rhs);
      ctx << ")";
      return;
    }

    fhe::ckks::IR2C_CORE::Handle_ist<RETV>(visitor, node);
  }

  //! @brief Special handling for CORE LDF operator
  template <typename RETV, typename VISITOR>
  void Handle_ldf(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX&         ctx   = visitor->Context();
    air::base::OPCODE p_opc = visitor->Parent(1)->Opcode();
    if (p_opc != air::core::OPC_RETV && p_opc != air::core::OPC_CALL &&
        ctx.Is_rns_poly_type(node->Rtype()->Id())) {
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
    air::base::OPCODE   opc = val->Opcode();
    if (ctx.Is_rns_poly_type(val->Rtype_id())) {
      switch (opc) {
        case air::core::OPC_LDF:
        case air::core::OPC_LDPF:
          ctx << "Copy_poly(&";
          ctx.Emit_var(node);
          ctx << ".";
          ctx.Emit_field(node);
          ctx << ", ";
          visitor->template Visit<RETV>(val);
          ctx << ")";
          return;
        case air::core::OPC_ZERO:
          ctx << "memset(&";
          ctx.Emit_var(node);
          ctx << ".";
          ctx.Emit_field(node);
          ctx << ", 0, sizeof(";
          ctx.Emit_var(node);
          ctx << ".";
          ctx.Emit_field(node);
          ctx << "))";
          return;
        case air::core::OPC_ILD:
          ctx << "Copy_poly(&";
          ctx.Emit_var(node);
          ctx << ".";
          ctx.Emit_field(node);
          ctx << ",";
          this->template Handle_ild_ist_addr<RETV>(visitor, val->Child(0));
          ctx << ")";
          return;
        default:
          break;
      }
    }
    if (val->Is_lib_call()) {
      // LIB_CALL may turn store lhs into first parameter
      visitor->template Visit<RETV>(val);
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
    air::base::OPCODE   opc = val->Opcode();
    if (ctx.Is_rns_poly_type(val->Rtype_id())) {
      switch (opc) {
        case air::core::OPC_LDF:
        case air::core::OPC_LDPF:
          ctx << "Copy_poly(&";
          ctx.Emit_preg_id(node->Preg_id());
          ctx << ".";
          ctx.Emit_field(node);
          ctx << ", ";
          visitor->template Visit<RETV>(val);
          ctx << ")";
          return;
        case air::core::OPC_ZERO:
          ctx << "memset(&";
          ctx.Emit_preg_id(node->Preg_id());
          ctx << ".";
          ctx.Emit_field(node);
          ctx << ", 0, sizeof(";
          ctx.Emit_var(node);
          ctx << ".";
          ctx.Emit_field(node);
          ctx << "))";
          return;
        case air::core::OPC_ILD:
          ctx << "Copy_poly(&";
          ctx.Emit_preg_id(node->Preg_id());
          ctx << ".";
          ctx.Emit_field(node);
          ctx << ",";
          this->template Handle_ild_ist_addr<RETV>(visitor, val->Child(0));
          ctx << ")";
          return;
        default:
          break;
      }
    }
    if (val->Is_lib_call()) {
      // LIB_CALL may turn store lhs into first parameter
      visitor->template Visit<RETV>(val);
      return;
    }
    ctx.Emit_preg_id(node->Preg_id());
    ctx << ".";
    ctx.Emit_field(node);
    ctx << " = ";
    visitor->template Visit<RETV>(node->Child(0));
  }

  template <typename RETV, typename VISITOR>
  void Handle_ild_ist_addr(VISITOR* visitor, air::base::NODE_PTR addr_node) {
    IR2C_CTX& ctx = visitor->Context();
    AIR_ASSERT(addr_node->Opcode() == air::core::ADD &&
               addr_node->Child(0)->Opcode() == air::core::OPC_ARRAY &&
               ctx.Is_rns_poly_type(
                   addr_node->Rtype()->Cast_to_ptr()->Domain_type_id()));
    air::base::NODE_PTR n_ofst = addr_node->Child(1);
    AIR_ASSERT(n_ofst->Opcode() == air::core::OPC_INTCONST);
    if (n_ofst->Intconst() == 0) {
      ctx << "Get_c0(&";
    } else if (n_ofst->Intconst() == 1) {
      ctx << "Get_c1(&";
    } else {
      AIR_ASSERT_MSG(false, "unexpected ist");
    }

    visitor->template Visit<RETV>(addr_node->Child(0));
    ctx << ")";
  }
};  // IR2C_CORE

}  // namespace poly

}  // namespace fhe

#endif
