//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_IR2C_UTIL_H
#define AIR_BASE_IR2C_UTIL_H

#include <iomanip>
#include <limits>
#include <ostream>
#include <unordered_set>

#include "air/base/st.h"

namespace air {
namespace base {

enum class DATA_FORMAT {
  DEC,  // Decimal
  HEX   // Hexadecimal
};

//! @brief Util for IR2C
class IR2C_UTIL {
public:
  //! @brief Construct a new ir2c util object
  //! @param os output stream
  IR2C_UTIL(std::ostream& os) : _os(os), _level(0) {}

  //! @brief Level of current node in nested block structures
  //! @return int Current Level
  int Level() const { return _level; }

  //! @brief Level of current node in nested block structures
  //! @return int Current Level
  int& Level() { return _level; }

  //! @brief Emit initialization data for constant array
  //! @tparam ElemT Type of array element
  template <typename ElemT>
  void Emit_array_init(CONSTANT_PTR cst,
                       DATA_FORMAT  format = DATA_FORMAT::DEC) {
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

      // Apply hexadecimal formatting only for integral types when requested.
      if (format == DATA_FORMAT::HEX && std::is_integral_v<ElemT>) {
        auto original_flags = _os.flags();
        _os << "0x" << std::hex << std::uppercase << cptr[i];
        _os.flags(original_flags);
      } else {
        if constexpr (std::is_floating_point<ElemT>::value) {
          _os << std::setprecision(std::numeric_limits<ElemT>::max_digits10 +
                                   1);
        }
        _os << cptr[i];
      }
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
    Emit_arb(type, false);
    if (decl_only) {
      _os << ";" << std::endl;
      return;
    }
    _os << " = {";
    if (type->Elem_type()->Is_array()) {
      type = type->Elem_type()->Cast_to_arr();
    }
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
        case PRIMITIVE_TYPE::COMPLEX_32:
          // Complex float: emit as pairs of floats
          Emit_array_init<float>(cst);
          break;
        case PRIMITIVE_TYPE::COMPLEX_64:
          // Complex double: emit as pairs of doubles
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
    } else if (type->Is_array()) {
      Emit_type(type->Cast_to_arr()->Elem_type(), name_only);
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
  uint64_t Emit_arb(ARRAY_TYPE_PTR type, bool flatten) {
    uint64_t count = 1;
    for (DIM_ITER it = type->Begin_dim(); it != type->End_dim(); ++it) {
      // assume lb == 0 and stride == 1
      AIR_ASSERT((*it)->Lb_val() == 0 && (*it)->Stride_val() == 1);
      uint64_t dim_size = (*it)->Ub_val();
      if (flatten) {
        count *= dim_size;
      } else {
        _os << "[" << dim_size << "]";
      }
    }
    if (type->Elem_type()->Is_array()) {
      count *= Emit_arb(type->Elem_type()->Cast_to_arr(), flatten);
    }
    return count;
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

  //! @brief Emit function signature for declaration
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

  //! @brief Emit function definition
  void Emit_func_def(FUNC_SCOPE* fscope) {
    _os << std::string(_level * 2, ' ');

    FUNC_PTR func = fscope->Owning_func();
    TYPE_PTR type = func->Entry_point()->Type();
    AIR_ASSERT(type->Is_signature());
    SIGNATURE_TYPE_PTR& sig = type->Cast_to_sig();
    PARAM_ITER          it  = sig->Begin_param();
    AIR_ASSERT(it != sig->End_param());
    Emit_type((*it)->Type(), true);

    _os << " " << func->Name()->Char_str() << "(";
    for (int i = 0; i < fscope->Formal_cnt(); ++i) {
      if (i > 0) {
        _os << ", ";
      }
      ADDR_DATUM_PTR formal = fscope->Formal(i);
      Emit_type(formal->Type(), true);
      _os << " ";
      Emit_var(formal);
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
        Emit_var(*it);
        Emit_arb(array_type, false);
      } else {
        Emit_type(sym_type, true);
        _os << " ";
        Emit_var(*it);
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

  //! @brief Emit a variable name from ADDR_DATUM_PTR
  //!  SYM Index is appended to avoid duplication
  void Emit_var(ADDR_DATUM_PTR var) {
    Emit_sym(var);
    _os << '_' << var->Id().Index();
  }

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

  //! @brief Emit a parameter declaration
  //! @param idx Index of the param. -1 means return value
  void Emit_param(PARAM_PTR param, uint32_t idx) {
    if (idx == -1) {
      Emit_type(param->Type(), true);
    } else {
      Emit_type(param->Type(), true);
      if (param->Name() != Null_ptr) {
        _os << " " << param->Name()->Char_str();
      }
    }
  }

  //! @brief Emit a few spaces for indent
  void Emit_indent() { _os << std::string(_level * 2, ' '); }

  //! @brief Emit any typed value
  template <typename T>
  IR2C_UTIL& operator<<(const T& val) {
    _os << val;
    return *this;
  }

  //! @brief Transparent stream manipulators (std::endl, std::hex, etc.)
  IR2C_UTIL& operator<<(std::ostream& (*manip)(std::ostream&)) {
    _os << manip;
    return *this;
  }

protected:
  std::ostream&                _os;
  std::unordered_set<uint32_t> _cst_used;
  int                          _level;
};  // IR2C_UTIL

}  // namespace base
}  // namespace air

#endif  // AIR_BASE_IR2C_UTIL_H
