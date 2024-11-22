//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_POLY_IR2C_CTX_H
#define FHE_POLY_IR2C_CTX_H

#include "fhe/ckks/ir2c_ctx.h"

namespace fhe {

namespace poly {

/**
 * @brief Context to convert polynomial IR to C
 *
 */
class IR2C_CTX : public fhe::ckks::IR2C_CTX {
public:
  /**
   * @brief Construct a new ir2c ctx object
   *
   * @param os Output stream
   */
  IR2C_CTX(std::ostream& os, const fhe::core::LOWER_CTX& lower_ctx,
           const fhe::poly::POLY2C_CONFIG& cfg)
      : fhe::ckks::IR2C_CTX(os, lower_ctx, cfg) {}

  /**
   * @brief Include "rt_ant.h" in generated C file
   *
   */
  void Emit_global_include() {
    _os << "// external header files" << std::endl;
    _os << "#include \"";
    _os << fhe::core::Provider_header(Provider());
    _os << "\"" << std::endl << std::endl;
    _os << "typedef double float64_t;" << std::endl;
    _os << "typedef float float32_t;" << std::endl << std::endl;
  }

  /**
   * @brief Emit fhe server function definition
   *
   * @param func Pointer to FUNC_SCOPE
   */
  void Emit_func_def(air::base::FUNC_SCOPE* func) {
    air::base::FUNC_PTR decl = func->Owning_func();
    if (decl->Entry_point()->Is_program_entry()) {
      _os << "bool " << decl->Name()->Char_str() << "()";
    } else {
      Emit_func_sig(decl);
    }
  }

  /**
   * @brief Emit all local variables
   *
   * @param func Pointer to function scope
   */
  void Emit_local_var(air::base::FUNC_SCOPE* func) {
    air::base::IR2C_CTX::Emit_local_var(func);
    air::base::ENTRY_PTR entry        = func->Owning_func()->Entry_point();
    bool                 is_prg_entry = entry->Is_program_entry();
    if (Provider() != fhe::core::PROVIDER::ANT) {
      // no need to call memset for SEAL/OpenFHE on ciphertext/plaintext
      if (is_prg_entry) {
        uint32_t num_args = entry->Type()->Cast_to_sig()->Num_param();
        for (uint32_t i = 0; i < num_args; ++i) {
          air::base::ADDR_DATUM_PTR parm = func->Formal(i);
          AIR_ASSERT(parm->Is_formal());
          AIR_ASSERT(Is_cipher_type(parm->Type_id()));
          const char* name = parm->Name()->Char_str();
          _os << "  ";
          Emit_identifier(name);
          _os << " = Get_input_data(\"" << name << "\", 0);" << std::endl;
        }
      }
      return;
    }

    // ANT library need memset on ciphertext/plaintext
    _os << "  uint32_t  degree = Degree();" << std::endl;
    for (auto it = func->Begin_addr_datum(); it != func->End_addr_datum();
         ++it) {
      air::base::TYPE_PTR type = (*it)->Type();
      if (type->Is_array()) {
        type = type->Cast_to_arr()->Elem_type();
      }
      air::base::TYPE_ID type_id = type->Id();

      if (Is_cipher_type(type_id) || Is_cipher3_type(type_id) ||
          Is_plain_type(type_id)) {
        const char* name = (*it)->Name()->Char_str();
        if (!(*it)->Is_formal()) {
          _os << "  memset(&";
          Emit_identifier(name);
          _os << ", 0, sizeof(";
          Emit_identifier(name);
          _os << "));" << std::endl;
        } else if (is_prg_entry) {
          AIR_ASSERT(Is_cipher_type(type_id));
          _os << "  ";
          Emit_identifier(name);
          _os << " = Get_input_data(\"" << name << "\", 0);" << std::endl;
        }
      }
    }
    for (auto it = func->Begin_preg(); it != func->End_preg(); ++it) {
      air::base::TYPE_ID type = (*it)->Type_id();
      if (Is_cipher_type(type) || Is_cipher3_type(type) ||
          Is_plain_type(type)) {
        _os << "  memset(&";
        Emit_preg_id((*it)->Id());
        _os << ", 0, sizeof(";
        Emit_preg_id((*it)->Id());
        _os << "));" << std::endl;
      }
    }
  }

};  // IR2C_CTX

}  // namespace poly

}  // namespace fhe

#endif  // FHE_POLY_IR2C_CTX_H
