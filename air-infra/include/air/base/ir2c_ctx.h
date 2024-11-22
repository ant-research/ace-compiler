//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_IR2C_CTX_H
#define AIR_BASE_IR2C_CTX_H

#include <iomanip>
#include <limits>
#include <ostream>
#include <unordered_set>

#include "air/base/analyze_ctx.h"
#include "air/base/meta_info.h"
#include "air/base/st.h"

namespace air {
namespace base {

//! @brief Return value for IR2C to describe how kid is handled
class IR2C_RETV {
public:
  //! @brief Code to describe how kid is handled
  enum IR2C_RETC {
    NORMAL,    //!< kid is handled and return expression with value
    VAR_USED,  //!< kid is handled and value stored in var from parent
    TMP_USED,  //!< kid is handled and value stored in temp
  };

  //! @brief Construct a NORMAL IR2C_RETV object
  IR2C_RETV() : _retc(NORMAL), _var(Null_st_id) {}

  //! @brief Contruct a VAR/TMP IR2C_RETV object with ADDR_DATUM_ID
  IR2C_RETV(IR2C_RETC retc, ADDR_DATUM_ID var) : _retc(retc), _var(var) {
    AIR_ASSERT(var != Null_ptr);
  }

  //! @brief Check if retv uses variable
  bool Has_var() const { return _retc != IR2C_RETC::NORMAL; }

  //! @brief Get variable id if VAR_USED or TMP_USED
  ADDR_DATUM_ID Var_id() const {
    AIR_ASSERT(_retc == IR2C_RETC::VAR_USED || _retc == IR2C_RETC::TMP_USED);
    return _var;
  }

private:
  IR2C_RETC     _retc;
  ADDR_DATUM_ID _var;
};  // IR2C_RETV

//! @brief Context for IR2C
class IR2C_CTX : public ANALYZE_CTX {
public:
  //! @brief Construct a new ir2c ctx object
  //! @param os output stream
  IR2C_CTX(std::ostream& os) : _os(os), _level(0) {}

  //! @brief Special handling for BLOCK for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_block(VISITOR* visitor, NODE_PTR node) {
    AIR_ASSERT(node->Is_block());
    Begin_block(node);
    for (STMT_PTR stmt = node->Begin_stmt(); stmt != node->End_stmt();
         stmt          = stmt->Next()) {
      NODE_PTR snode = stmt->Node();
      Begin_stmt(snode);
      visitor->template Visit<RETV>(snode);
      End_stmt(snode);
    }
    End_block(node);
    return RETV();
  }

  //! @brief Level of current node in nested block structures
  //! @return int Current Level
  int Level() const { return _level; }

  //! @brief Emit initialization data for constant array
  //! @tparam ElemT Type of array element
  template <typename ElemT>
  void Emit_array_init(CONSTANT_PTR cst) {
    AIR_ASSERT(cst->Kind() == CONSTANT_KIND::ARRAY);
    AIR_ASSERT(cst->Type()->Is_array());
    const ElemT* cptr = cst->Array_ptr<ElemT>();
    uint32_t     size = cst->Array_byte_len() / sizeof(ElemT);
    _os << std::endl << "  ";
    for (uint32_t i = 0; i < size; ++i) {
      if (i > 0) {
        if ((i % 8) == 0) {
          _os << "," << std::endl << "  ";
        } else {
          _os << ", ";
        }
      }
      if constexpr (std::is_floating_point<ElemT>::value) {
        _os << std::setprecision(std::numeric_limits<ElemT>::max_digits10 + 1);
      }
      _os << cptr[i];
    }
  }

  //! @brief Emit a scalar init value
  void Emit_scalar_init(CONSTANT_PTR cst) {
    AIR_ASSERT(cst->Type()->Is_prim());
    PRIM_TYPE_PTR prim = cst->Type()->Cast_to_prim();
    switch (prim->Encoding()) {
      case PRIMITIVE_TYPE::FLOAT_32:
      case PRIMITIVE_TYPE::FLOAT_64:
        _os << std::setprecision(
                   std::numeric_limits<long double>::max_digits10 + 1)
            << cst->Float_literal().Val_as_long_double();
        break;
      default:
        AIR_ASSERT(false);
    }
  }

  //! @brief Emit a constant string
  void Emit_constant_str_init(CONSTANT_PTR cst) {
    AIR_ASSERT(cst->Kind() == CONSTANT_KIND::STR_ARRAY);
    _os << "\"";
    _os << cst->Str_val()->Char_str();
    _os << "\"";
  }

  //! @brief Emit a constant id as the name of the constant
  void Emit_constant_id(CONSTANT_ID cst) { _os << "_cst_" << cst.Value(); }

  //! @brief Emit a constant and mark constant used
  void Emit_constant_name(CONSTANT_ID cst) {
    _cst_used.insert(cst.Value());
    Emit_constant_id(cst);
  }

  //! @brief Emit a constant array
  void Emit_constant_array(CONSTANT_PTR cst, bool decl_only) {
    AIR_ASSERT(cst->Kind() == CONSTANT_KIND::ARRAY);
    AIR_ASSERT(cst->Type()->Is_array());
    if (!decl_only && _cst_used.find(cst->Id().Value()) == _cst_used.end()) {
      // only emit used constant array
      return;
    }
    ARRAY_TYPE_PTR type = cst->Type()->Cast_to_arr();
    if (decl_only) {
      // if decl only, add 'extern' to avoid duplication of these symbols
      _os << "extern ";
    }
    Emit_type(type->Elem_type(), true);
    _os << " ";
    Emit_constant_id(cst->Id());
    Emit_arb(type, true);
    if (decl_only) {
      _os << ";" << std::endl;
      return;
    }
    _os << " = {";
    if (type->Elem_type()->Is_prim()) {
      PRIM_TYPE_PTR prim = type->Elem_type()->Cast_to_prim();
      switch (prim->Encoding()) {
        case PRIMITIVE_TYPE::INT_S32:
          Emit_array_init<int32_t>(cst);
          break;
        case PRIMITIVE_TYPE::INT_U32:
          Emit_array_init<uint32_t>(cst);
          break;
        case PRIMITIVE_TYPE::INT_S64:
          Emit_array_init<int64_t>(cst);
          break;
        case PRIMITIVE_TYPE::INT_U64:
          Emit_array_init<uint64_t>(cst);
          break;
        case PRIMITIVE_TYPE::FLOAT_32:
          Emit_array_init<float>(cst);
          break;
        case PRIMITIVE_TYPE::FLOAT_64:
          Emit_array_init<double>(cst);
          break;
        default:
          AIR_ASSERT(false);
      }
    } else {
      AIR_ASSERT(false);
    }
    _os << std::endl << "};" << std::endl;
  }

  //! @brief Emit a constant scalar
  void Emit_constant_scalar(CONSTANT_PTR cst) {
    AIR_ASSERT(cst->Kind() == CONSTANT_KIND::FLOAT);
    AIR_ASSERT(cst->Type()->Is_prim());
    Emit_type(cst->Type(), true);
    _os << " ";
    Emit_constant_id(cst->Id());
    _os << " = ";
    Emit_scalar_init(cst);
    _os << ";" << std::endl;
  }

  //! @brief Emit all global constants
  void Emit_global_constants(GLOB_SCOPE* glob, bool decl_only) {
    for (CONSTANT_ITER it = glob->Begin_const(); it != glob->End_const();
         ++it) {
      if ((*it)->Kind() == CONSTANT_KIND::ARRAY) {
        Emit_constant_array(*it, decl_only);
      } else if (decl_only && (*it)->Kind() == CONSTANT_KIND::FLOAT) {
        Emit_constant_scalar(*it);
      }
    }
  }

  //! @brief Emit all global type definitions
  void Emit_global_type(GLOB_SCOPE* glob) {
    _os << "// global types definition" << std::endl;
    for (TYPE_ITER it = glob->Begin_type(); it != glob->End_type(); ++it) {
      if ((*it)->Is_prim()) {
        continue;
      }
      Emit_type(*it, false);
    }
    _os << std::endl;
  }

  //! @brief Emit a type definition. If name_only is true, only emit the type
  //! name
  //! @param name_only Only emit type name or full type definition
  void Emit_type(TYPE_PTR type, bool name_only) {
    if (type->Is_record()) {
      Emit_record_type(type->Cast_to_rec(), name_only);
    } else if (type->Is_ptr()) {
      Emit_pointer_type(type->Cast_to_ptr(), name_only);
    } else {
      _os << type->Name()->Char_str();
      if (name_only) {
        return;
      }
      _os << ";" << std::endl;
    }
  }

  //! @brief Emit dimensions for array type
  //! @param flatten Treat multi-dimention array as 1-D array or not
  void Emit_arb(ARRAY_TYPE_PTR type, bool flatten) {
    uint64_t count = 1;
    for (DIM_ITER it = type->Begin_dim(); it != type->End_dim(); ++it) {
      // assume lb == 0 and stride == 1
      uint64_t dim_size = (*it)->Ub_val();
      if (flatten) {
        count *= dim_size;
      } else {
        _os << "[" << dim_size << "]";
      }
    }
    if (flatten) {
      _os << "[" << count << "]";
    }
  }

  //! @brief Emit a pointer type. If name_only is true, only emit the type name
  //! @param name_only Only emit type name and star
  void Emit_pointer_type(POINTER_TYPE_PTR type, bool name_only) {
    if (name_only) {
      Emit_type(type->Domain_type(), true);
      _os << "*";
    } else if (type->Name()->Is_undefined()) {
      // ignore noname pointer type
    } else {
      AIR_ASSERT(false);
    }
  }

  //! @brief Emit a record type. If name_only is true, only emit the type name
  //! @param name_only Only emit type keyword and type name
  void Emit_record_type(RECORD_TYPE_PTR type, bool name_only) {
    if (!name_only) {
      _os << "struct ";
    }
    Emit_name(type->Name());
    if (name_only) {
      return;
    }
    if (type->Is_complete()) {
      for (FIELD_ITER it = type->Begin(); it != type->End(); ++it) {
        _os << "  ";
        Emit_type((*it)->Type(), true);
        _os << " ";
        Emit_name((*it)->Name());
        _os << ";" << std::endl;
      }
    }
    _os << ";" << std::endl;
    return;
  }

  //! @brief Emit function signature for either declaration or definition
  void Emit_func_sig(FUNC_PTR func) {
    _os << std::string(_level * 2, ' ');

    TYPE_PTR type = func->Entry_point()->Type();
    AIR_ASSERT(type->Is_signature());
    SIGNATURE_TYPE_PTR& sig = type->Cast_to_sig();
    PARAM_ITER          it  = sig->Begin_param();
    AIR_ASSERT(it != sig->End_param());
    Emit_param(*it, -1);
    ++it;

    _os << " " << func->Name()->Char_str() << "(";
    int idx = 0;
    while (it != sig->End_param()) {
      if (idx > 0) {
        _os << ", ";
      }
      Emit_param(*it, idx++);
      ++it;
    }
    _os << ")";
  }

  //! @brief Emit preg id
  void Emit_preg_id(PREG_ID id) { _os << "_preg_" << id.Value(); }

  //! @brief Emit all local variables at beginning of function
  //! body
  void Emit_local_var(FUNC_SCOPE* func) {
    bool is_prg_entry = func->Owning_func()->Entry_point()->Is_program_entry();
    // local var
    for (auto it = func->Begin_addr_datum(); it != func->End_addr_datum();
         ++it) {
      if (!is_prg_entry && (*it)->Is_formal()) {
        // formal is emitted in function signature
        continue;
      }

      _os << "  ";
      TYPE_PTR sym_type = (*it)->Type();
      if (sym_type->Is_array()) {
        ARRAY_TYPE_PTR array_type = sym_type->Cast_to_arr();
        Emit_type(array_type->Elem_type(), true);
        _os << " ";
        Emit_sym((*it)->Base_sym());
        Emit_arb(array_type, false);
      } else {
        Emit_type(sym_type, true);
        _os << " ";
        Emit_sym((*it)->Base_sym());
      }
      _os << ";" << std::endl;
    }
    // preg
    for (auto it = func->Begin_preg(); it != func->End_preg(); ++it) {
      _os << "  ";
      Emit_type((*it)->Type(), true);
      _os << " ";
      Emit_preg_id((*it)->Id());
      _os << ";" << std::endl;
    }
  }

  //! @brief Emit left brace to begin the function
  void Begin_func_body(NODE_PTR node) {
    AIR_ASSERT(_level == 0);
    _os << " {" << std::endl;
  }

  //! @brief Emit right brace to end the function
  void End_func_body(NODE_PTR node) {
    AIR_ASSERT(_level == 0);
    _os << "}" << std::endl;
  }

  //! @brief Emit left brace to begin a block
  void Begin_block(NODE_PTR node) {
    if (_level > 0) {
      _os << "{" << std::endl;
    }
    _level++;
  }

  //! @brief Emit right brace to end a block
  void End_block(NODE_PTR node) {
    _level--;
    if (_level > 0) {
      _os << std::string(_level * 2, ' ') << "}" << std::endl;
    }
  }

  //! @brief Begin a new statement
  void Begin_stmt(NODE_PTR node) { _os << std::string(_level * 2, ' '); }

  //! @brief End current statement
  void End_stmt(NODE_PTR node) {
    if (META_INFO::Op_category(node->Opcode()) != OPR_CAT::CFLOW) {
      _os << ";" << std::endl;
    }
  }

  //! @brief Begin a new expression
  void Begin_expr(NODE_PTR node, NODE_PTR parent) {
    if (!parent->Is_root()) {
      // only add '(' for sub-expression
      _os << "(";
    }
  }

  //! @brief End current expression
  void End_expr(NODE_PTR node, NODE_PTR parent) {
    if (!parent->Is_root()) {
      // only add ')' for sub-expression
      _os << ")";
    }
  }

  //! @brief Emit a variable name from NODE_PTR
  //! @param node Node with addressable data
  void Emit_var(NODE_PTR node) { Emit_sym(node->Addr_datum()->Base_sym()); }

  //! @brief Emit a field name from NODE_PTR
  //! @param node Node with field info
  void Emit_field(NODE_PTR node) { Emit_name(node->Field()->Name()); }

  //! @brief Emit a symbol name from SYM_PTR
  void Emit_sym(SYM_PTR sym) { Emit_name(sym->Name()); }

  //! @brief Emit a name from STR_PTR
  void Emit_name(STR_PTR name) { Emit_identifier(name->Char_str()); }

  //! @brief Emit a identifier from a C-string
  void Emit_identifier(const char* str) {
    char ch;
    while ((ch = *str) != '\0') {
      // replace illegal character with '_'
      if (!isdigit(ch) && !isalpha(ch) && ch != '_') {
        ch = '_';
      }
      _os << ch;
      ++str;
    }
  }

  //! @brief Emit variable name as string
  //! @param node Node with addressable data
  void Emit_var_str(NODE_PTR node) {
    const char* str = node->Addr_datum()->Base_sym()->Name()->Char_str();
    _os << '\"' << str << '\"';
  }

  //! @brief Emit a parameter declaration
  //! @param idx Index of the param. -1 means return value
  void Emit_param(PARAM_PTR param, uint32_t idx) {
    if (idx == -1) {
      Emit_type(param->Type(), true);
    } else {
      Emit_type(param->Type(), true);
      _os << " " << param->Name()->Char_str();
    }
  }

  //! @brief Emit a few spaces for indent
  void Emit_indent() { _os << std::string(_level * 2, ' '); }

  //! @brief Emit any typed value
  template <typename T>
  IR2C_CTX& operator<<(const T& val) {
    _os << val;
    return *this;
  }

protected:
  std::ostream&                _os;
  std::unordered_set<uint32_t> _cst_used;
  int                          _level;
};  // IR2C_CTX

}  // namespace base
}  // namespace air

#endif  // AIR_BASE_IR2C_CTX_H
