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
          Is_plain_type(type_id) || Is_poly_type(type_id)) {
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
          Is_plain_type(type) || Is_poly_type(type)) {
        _os << "  memset(&";
        Emit_preg_id((*it)->Id());
        _os << ", 0, sizeof(";
        Emit_preg_id((*it)->Id());
        _os << "));" << std::endl;
      }
    }
  }

  bool Is_poly_ptr_ptr(air::base::TYPE_PTR type) {
    if (type->Is_ptr()) {
      air::base::TYPE_PTR pts_to = type->Cast_to_ptr()->Domain_type();
      if (pts_to->Is_ptr() &&
          Lower_ctx().Is_poly_type(pts_to->Cast_to_ptr()->Domain_type_id())) {
        return true;
      }
    }
    return false;
  }

  template <typename RETV, typename VISITOR>
  void Emit_encode_precom(VISITOR* visitor, air::base::NODE_PTR node) {
    bool            prec_plain = false;
    const uint32_t* prec       = node->Attr<uint32_t>(
        Lower_ctx().Attr_name(fhe::core::FHE_ATTR_KIND::PRECOMPUTE));
    if (prec != nullptr && *prec != 0) {
      prec_plain = true;
    }
    AIR_ASSERT(prec_plain);
    AIR_ASSERT(_rt_data_writer != nullptr);

    air::base::NODE_PTR parent = Parent(1);
    AIR_ASSERT(parent != air::base::Null_ptr);
    // when rt_data_file is full, no precompute will be generated
    // just return
    if (_rt_data_writer->Is_full()) {
      return;
    }

    Emit_st_var<RETV, VISITOR>(visitor, parent);
    _os << " = ";
    if (node->Child(0)->Opcode() == nn::vector::OPC_SLICE &&
        node->Child(1)->Opcode() == air::core::OPC_INTCONST) {
      air::base::NODE_PTR slice = node->Child(0);
      AIR_ASSERT(slice->Child(0)->Opcode() == air::core::OPC_LDC);
      air::base::CONSTANT_PTR cst = slice->Child(0)->Const();
      AIR_ASSERT(cst->Kind() == air::base::CONSTANT_KIND::ARRAY);
      AIR_ASSERT(cst->Type()->Is_array());
      AIR_ASSERT(cst->Type()->Cast_to_arr()->Elem_type()->Is_prim());
      AIR_ASSERT(
          cst->Type()->Cast_to_arr()->Elem_type()->Cast_to_prim()->Encoding() ==
          air::base::PRIMITIVE_TYPE::FLOAT_32);
      AIR_ASSERT(slice->Child(2)->Opcode() == air::core::OPC_INTCONST);
      air::base::NODE_PTR                       start = slice->Child(1);
      std::vector<std::pair<int64_t, int64_t> > subscript;
      bool ret = Parse_subscript_expr(start, subscript);
      AIR_ASSERT(ret == true && subscript.size() > 0);
      uint64_t loop_cnt = 1;
      for (uint64_t i = 0; i < subscript.size(); ++i) {
        air::base::NODE_PTR loop = visitor->Parent_loop(i);
        AIR_ASSERT(loop != air::base::Null_ptr &&
                   loop->Opcode() == air::core::DO_LOOP);
        int64_t lb, ub, stride;
        ret = Parse_do_loop(loop, lb, ub, stride);
        AIR_ASSERT(ret == true && lb == 0 && stride == 1);
        AIR_ASSERT(subscript[i].first == loop->Iv_id().Value());
        AIR_ASSERT(subscript[i].second == -1 || subscript[i].second == ub);
        loop_cnt *= ub;
      }
      uint64_t total_count = cst->Array_byte_len() / sizeof(float);
      uint64_t span        = slice->Child(2)->Intconst();
      uint64_t count       = node->Child(1)->Intconst();
      for (uint64_t i = 0; i < loop_cnt; ++i) {
        AIR_ASSERT(total_count >= i * span + count);
        char name[32];
        snprintf(name, 32, "cst_%d_%d", cst->Id().Value(), (int)i);
        if (i == 0) {
          // Pt_get_validate(cst, index, len, scale, level)
          // Pt_get(index, len, scale, level)
          if (Rt_validate()) {
            _os << "*(PLAIN)Pt_get_validate(";
            visitor->template Visit<RETV>(node->Child(0));  // buffer address
            _os << ", ";
          } else {
            _os << "*(PLAIN)Pt_get(";
          }
          _os << "(2 * ";
          visitor->template Visit<RETV>(start);
          _os << ") + " << _rt_data_writer->Cur_idx() - 2 * loop_cnt + 1
              << " /* " << name << " */";
          _os << ", ";
          visitor->template Visit<RETV>(node->Child(1));  // element count
          _os << ", ";
          visitor->template Visit<RETV>(node->Child(2));  // scale
          _os << ", ";
          visitor->template Visit<RETV>(node->Child(3));  // level
          _os << ")";
        }
      }
    } else if ((node->Child(0)->Opcode() == air::core::OPC_LDC ||
                node->Child(0)->Opcode() == air::core::OPC_LDCA) &&
               node->Child(1)->Opcode() == air::core::OPC_INTCONST) {
      air::base::CONSTANT_PTR cst = node->Child(0)->Const();
      char                    name[32];
      snprintf(name, 32, "cst_%d", cst->Id().Value());

      // Pt_get_validate(cst, index, len, scale, level)
      // Pt_get(index, len, scale, level)
      if (Rt_validate()) {
        _os << "*(PLAIN)Pt_get_validate(";
        visitor->template Visit<RETV>(node->Child(0));  // buffer address
        _os << ", ";
      } else {
        _os << "*(PLAIN)Pt_get(";
      }
      _os << _rt_data_writer->Cur_idx() - 1 << " /* " << name << " */";
      _os << ", ";
      visitor->template Visit<RETV>(node->Child(1));  // element count
      _os << ", ";
      visitor->template Visit<RETV>(node->Child(2));  // scale
      _os << ", ";
      visitor->template Visit<RETV>(node->Child(3));  // level
      _os << ")";
    }
  }
};  // IR2C_CTX

}  // namespace poly

}  // namespace fhe

#endif  // FHE_POLY_IR2C_CTX_H
