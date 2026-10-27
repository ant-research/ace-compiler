//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/cg/emit_c_ctx.h"

#include <cmath>
#include <iomanip>

namespace air {
namespace cg {

R_CODE EMIT_C_CTX::Emit_res_var(OPND_PTR res, const char* type,
                                const std::string& assign_prefix,
                                bool               hex_imm) {
  switch (res->Kind()) {
    case OPND_KIND::REGISTER: {
      AIR_ASSERT_MSG(!res->Is_preg(), "Illegal PREG opnd");
      std::string reg_name = Make_reg_name(res);
      if (First_occur(res->Id().Value())) {
        _ir2c_util << "    " << type << " ";
      } else {
        _ir2c_util << "    ";
      }
      _ir2c_util << reg_name << " = " << assign_prefix;
      break;
    }
    case OPND_KIND::IMMEDIATE: {
      if (hex_imm) {
        _ir2c_util << "0x" << std::hex << std::uppercase << res->Immediate()
                   << std::dec << ";\n";
      } else {
        _ir2c_util << res->Immediate();
      }
      break;
    }
    default: {
      AIR_ASSERT_MSG(false, "Not supported opnd kind");
      return R_CODE::INTERNAL;
    }
  }
  return R_CODE::NORMAL;
}

R_CODE EMIT_C_CTX::Handle_opnd(OPND_PTR opnd, bool hex_imm,
                               bool process_array_opnd) {
  static const std::vector<SYMBOL_HANDLER> symbol_handlers = {
      {"output",
       [](EMIT_C_CTX* self, auto val) {
         return "FHE_OUTPUT, " + std::to_string(self->Output_index(val));
       }},
      {"enc_weight",
       [](EMIT_C_CTX* self, auto val) {
         return "FHE_WEIGHT, " + std::to_string(self->Weight_index(val));
       }},
      {"_pgen_tmp",
       [](EMIT_C_CTX* self, auto val) {
         return "FHE_RELIN_KEY, " + std::to_string(self->Relin_key_index(val));
       }}
      // add more
  };

  switch (opnd->Kind()) {
    case air::cg::OPND_KIND::REGISTER: {
      AIR_ASSERT_MSG(!opnd->Is_preg(), "Illegal PREG opnd");
      _ir2c_util << Make_reg_name(opnd);
      break;
    }
    case air::cg::OPND_KIND::IMMEDIATE: {
      if (hex_imm) {
        _ir2c_util << "0x" << std::hex << opnd->Immediate() << std::dec
                   << ";\n";
      } else {
        _ir2c_util << opnd->Immediate();
        if (process_array_opnd) {
          _ir2c_util << "]";
        }
      }
      break;
    }
    case air::cg::OPND_KIND::CONSTANT: {
      air::base::CONSTANT_PTR cst = Glob()->Constant(opnd->Constant());
      _ir2c_util << Cst_sym_name(cst);
      if (process_array_opnd) {
        _ir2c_util << "[";
      }
      break;
    }
    case air::cg::OPND_KIND::SYMBOL: {
      air::base::SYM_ID id = opnd->Symbol();
      AIR_ASSERT(Container() != nullptr);
      air::base::SYM_PTR sym      = Container()->Func_scope()->Sym(id);
      std::string        sym_name = sym->Name()->Char_str();
      bool               handled  = false;
      if (sym->Is_formal()) {
        _ir2c_util << "FHE_INPUT" << ", " << Input_index(id.Value());
        handled = true;
      } else {
        for (const auto& entry : symbol_handlers) {
          if (sym_name.rfind(entry._prefix, 0) == 0) {
            _ir2c_util << entry._handler(this, id.Value());
            handled = true;
            break;  // find then return immediately
          }
        }
      }

      if (!handled) {
        // not find, use default
        _ir2c_util << sym_name;
      }
      break;
    }
    default:
      AIR_ASSERT_MSG(false, "Not supported opnd kind");
      return R_CODE::INTERNAL;
  }
  return R_CODE::NORMAL;
}

void EMIT_C_CTX::Handle_inst(const char* op_name, INST_PTR inst,
                             const char* res_decl_type, bool hex_imm,
                             bool pure_operand) {
  const auto* isa_info = Get_isa_info_meta(inst);
  if (Is_isa_core(inst)) return;

  uint32_t         res_num = isa_info->Res_num(inst->Operator());
  std::string_view name    = op_name ? op_name : "";
  if (pure_operand) {
    _ir2c_util << "    " << (name.empty() ? "" : (std::string(name) + "("));
  } else {
    for (uint32_t idx = 0; idx < res_num; ++idx) {
      Emit_res_var(inst->Res(idx), res_decl_type,
                   name.empty() ? "" : (std::string(name) + "("), hex_imm);
    }
  }

  uint32_t opnd_num = isa_info->Opnd_num(inst->Operator());
  for (uint32_t idx = 0; idx < opnd_num; ++idx) {
    Handle_opnd(inst->Opnd(idx), hex_imm);
    if ((idx + 1) != opnd_num) {
      _ir2c_util << ", ";
    }
  }
  if (!name.empty()) {
    _ir2c_util << ");";
  }

  _ir2c_util << "\n";
}

void EMIT_C_CTX::Handle_process_array_inst(const char* macro_name,
                                           INST_PTR    inst,
                                           const char* res_decl_type,
                                           bool        hex_imm) {
  const auto* isa_info = Get_isa_info_meta(inst);
  if (Is_isa_core(inst)) return;

  uint32_t         res_num = isa_info->Res_num(inst->Operator());
  std::string_view name    = macro_name ? macro_name : "";
  for (uint32_t idx = 0; idx < res_num; ++idx) {
    Emit_res_var(inst->Res(idx), res_decl_type,
                 name.empty() ? "" : (std::string(name) + "("), hex_imm);
  }

  uint32_t opnd_num = isa_info->Opnd_num(inst->Operator());
  for (uint32_t idx = 0; idx < opnd_num; ++idx) {
    Handle_opnd(inst->Opnd(idx), hex_imm, true);
    if ((inst->Opnd(idx)->Kind() == air::cg::OPND_KIND::IMMEDIATE) &&
        ((idx + 1) != opnd_num)) {
      _ir2c_util << "[";
    }
  }
  if (!name.empty()) {
    _ir2c_util << ");";
  }

  _ir2c_util << ";\n";
}

//! @brief Include "fhe_types.h" in generated C file
void EMIT_C_CTX::Emit_global_include() {
  _ir2c_util << "// external header files" << std::endl;
  _ir2c_util << "#include \"";
  _ir2c_util << "fhe_types.h";
  _ir2c_util << "\"" << std::endl << std::endl;
  _ir2c_util << "typedef double float64_t;" << std::endl;
  _ir2c_util << "typedef float float32_t;" << std::endl << std::endl;
}

void EMIT_C_CTX::Emit_func_header(CGIR_CONTAINER* cont) {
  const char* name = cont->Func_scope()->Owning_func()->Name()->Char_str();
  _ir2c_util << "bool" << " " << name << "(void)" << " " << "{" << std::endl;
}

void EMIT_C_CTX::Emit_func_end(CGIR_CONTAINER* cont) {
  const std::string& indent = Default_indent();
  _ir2c_util << indent << "return true;" << std::endl;
  _ir2c_util << "}" << std::endl;
}

//! @brief Emit a constant array
void EMIT_C_CTX::Emit_constant_array(base::CONSTANT_PTR cst, bool decl_only,
                                     DATA_FORMAT format) {
  AIR_ASSERT(cst->Kind() == base::CONSTANT_KIND::ARRAY);
  AIR_ASSERT(cst->Type()->Is_array());

  base::ARRAY_TYPE_PTR type = cst->Type()->Cast_to_arr();
  if (decl_only) {
    // if decl only, add 'extern' to avoid duplication of these symbols
    _ir2c_util << "extern ";
  }
  Emit_type(type->Elem_type(), true);
  _ir2c_util << " ";
  Emit_constant_id(cst->Id());
  Emit_arb(type, false);
  if (decl_only) {
    _ir2c_util << ";" << std::endl;
    return;
  }
  _ir2c_util << " = {";
  if (type->Elem_type()->Is_array()) {
    type = type->Elem_type()->Cast_to_arr();
  }
  if (type->Elem_type()->Is_prim()) {
    base::PRIM_TYPE_PTR prim = type->Elem_type()->Cast_to_prim();
    switch (prim->Encoding()) {
      case base::PRIMITIVE_TYPE::INT_S32:
        Emit_array_init<int32_t>(cst, format);
        break;
      case base::PRIMITIVE_TYPE::INT_U32:
        Emit_array_init<uint32_t>(cst, format);
        break;
      case base::PRIMITIVE_TYPE::INT_S64:
        Emit_array_init<int64_t>(cst, format);
        break;
      case base::PRIMITIVE_TYPE::INT_U64:
        Emit_array_init<uint64_t>(cst, format);
        break;
      case base::PRIMITIVE_TYPE::FLOAT_32:
        Emit_array_init<float>(cst, format);
        break;
      case base::PRIMITIVE_TYPE::FLOAT_64:
        Emit_array_init<double>(cst, format);
        break;
      default:
        AIR_ASSERT(false);
    }
  } else {
    AIR_ASSERT(false);
  }
  _ir2c_util << std::endl << "};" << std::endl;
}

//! @brief Emit global constants
void EMIT_C_CTX::Emit_cst(bool decl_only) {
  // emit constants in GLOB_SCOPE
  for (base::CONSTANT_ITER cst_itr = Glob()->Begin_const();
       cst_itr != Glob()->End_const(); ++cst_itr) {
    if ((*cst_itr)->Kind() == base::CONSTANT_KIND::ARRAY) {
      Emit_constant_array(*cst_itr, decl_only, DATA_FORMAT::HEX);
    }
  }
  _ir2c_util << "\n";
}

}  // namespace cg
}  // namespace air