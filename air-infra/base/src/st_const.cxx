//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/base/st.h"

#include <iostream>
#include <sstream>

namespace air {
namespace base {

//=============================================================================
// class CONSTANT member functions
//=============================================================================

const char* CONSTANT::Constant_kind_name_arr[static_cast<uint32_t>(
    CONSTANT_KIND::END)] = {
    "unknown",   "signed_integer",    "unsigned_integer", "pointer_integer",
    "float",     "complex",           "nullptr",          "var_ptr",
    "entry_ptr", "thunk_ptr",         "array_elem_ptr",   "field_ptr",
    "ptr_ofst",  "ptr_from_unsigned", "ptr_cast",         "str_array",
    "array",     "array_elem",        "struct",           "struct_field",
    "union",     "entry_func_desc",   "thunk_func_desc",  "named",
    "boolean",   "label_ptr",         "decimal",          "external_file"};

CONSTANT::CONSTANT(const GLOB_SCOPE& glob, CONSTANT_ID cst)
    : _glob(const_cast<GLOB_SCOPE*>(&glob)),
      _const(glob.Const_table().Find(ID<CONSTANT_DATA>(cst.Index()))) {
  AIR_ASSERT(cst.Scope() == 0);
}

bool CONSTANT::Is_zero() const {
  switch (Kind()) {
    case CONSTANT_KIND::SIGNED_INT:
    case CONSTANT_KIND::UNSIGNED_INT:
      return Integer_literal().Is_zero();
    case CONSTANT_KIND::FLOAT:
      return Float_literal().Is_zero();
    case CONSTANT_KIND::COMPLEX:
      return Complex_literal().Is_zero();
    case CONSTANT_KIND::NULL_PTR:
      return true;
    case CONSTANT_KIND::PTR_FROM_UNSIGNED:
      return !Ptr_val_as_unsigned();
  }
  return false;
}

bool CONSTANT::Is_one() const {
  switch (Kind()) {
    case CONSTANT_KIND::SIGNED_INT:
    case CONSTANT_KIND::UNSIGNED_INT:
      return Integer_literal().Is_one();
    case CONSTANT_KIND::FLOAT:
      return Float_literal().Is_one();
    case CONSTANT_KIND::COMPLEX:
      return Complex_literal().Is_one();
  }
  return false;
}

bool CONSTANT::Is_minus_one() const {
  switch (Kind()) {
    case CONSTANT_KIND::SIGNED_INT:
      return Integer_literal().Is_minus_one();
    case CONSTANT_KIND::FLOAT:
      return Float_literal().Is_minus_one();
    case CONSTANT_KIND::COMPLEX:
      return Complex_literal().Is_minus_one();
  }
  return false;
}

CONSTANT_KIND
CONSTANT::Kind() const { return _const->Kind(); }

POINTER_KIND
CONSTANT::Ptr_kind() const {
  switch (Kind()) {
    case CONSTANT_KIND::VAR_PTR:
    case CONSTANT_KIND::ENTRY_PTR:
    case CONSTANT_KIND::THUNK_PTR:
      return _const->Ptr_kind();
    default:
      AIR_ASSERT(0);
  }
  return POINTER_KIND::END;
}

CONSTANT_ID
CONSTANT::Id() const {
  return (_const == CONSTANT_DATA_PTR()) ? CONSTANT_ID(Null_st_id)
                                         : CONSTANT_ID(_const.Id().Value(), 0);
}

TYPE_ID
CONSTANT::Type_id() const {
  if (Kind() != CONSTANT_KIND::NAMED) {
    return _const->Type();
  } else {
    return Base_const()->Type_id();
  }
}

TYPE_PTR
CONSTANT::Type() const { return Glob_scope().Type(Type_id()); }

CONSTANT_ID
CONSTANT::Base_const_id() const {
  return (Kind() == CONSTANT_KIND::NAMED) ? _const->Named_base() : Id();
}

CONSTANT_PTR
CONSTANT::Base_const() const { return Glob_scope().Constant(Base_const_id()); }

bool CONSTANT::Bool_val() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::BOOLEAN);
  return _const->Bool_val();
}

INT_LITERAL CONSTANT::Integer_literal() const {
  TYPE_PTR type = Type();
  AIR_ASSERT(type->Base_type()->Is_prim());
  if (Kind() == CONSTANT_KIND::SIGNED_INT) {
    return INT_LITERAL::Int_literal_from_int64(_const->Integer_val());
  } else {
    AIR_ASSERT(Kind() == CONSTANT_KIND::UNSIGNED_INT);
    return INT_LITERAL::Int_literal_from_uint64(_const->Integer_val());
  }
}

FLOAT_LITERAL CONSTANT::Float_literal() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::FLOAT);
  return FLOAT_LITERAL::Float_literal_from_long_double(_const->Float_val());
}

COMPLEX_LITERAL CONSTANT::Complex_literal() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::COMPLEX);
  return COMPLEX_LITERAL::Complex_literal_from_long_double(
      _const->Complex_real_val(), _const->Complex_imag_val());
}

uint64_t CONSTANT::Ptr_val_as_unsigned() const {
  return _const->Ptr_from_unsigned_val();
}

void CONSTANT::Set_type(TYPE_ID id) {
  AIR_ASSERT(Kind() != CONSTANT_KIND::NAMED);
  _const->Set_type(id);
}

void CONSTANT::Set_val(bool val) {
  AIR_ASSERT(Kind() == CONSTANT_KIND::BOOLEAN);
  _const->Set_bool_val(val);
}

void CONSTANT::Set_val(int64_t val) {
  AIR_ASSERT(Kind() == CONSTANT_KIND::SIGNED_INT);
  _const->Set_integer_val((uint64_t)val);
}

void CONSTANT::Set_val(uint64_t val) {
  AIR_ASSERT(Kind() == CONSTANT_KIND::UNSIGNED_INT);
  _const->Set_integer_val(val);
}

void CONSTANT::Set_val(STR_ID str) {
  AIR_ASSERT(Kind() == CONSTANT_KIND::STR_ARRAY);
  _const->Set_str_array_val(str);
}

void CONSTANT::Set_val(long double val) {
  AIR_ASSERT(Kind() == CONSTANT_KIND::FLOAT);
  _const->Set_float_val(val);
}

void CONSTANT::Set_val(CONSTANT_ID base, int64_t idx_or_ofst) {
  if (Kind() == CONSTANT_KIND::PTR_OFST) {
    _const->Set_ptr_ofst_val(base, idx_or_ofst);
  } else {
    AIR_ASSERT(Kind() == CONSTANT_KIND::ARRAY_ELEM_PTR);
    _const->Set_array_elem_ptr_val(base, idx_or_ofst);
  }
}

void CONSTANT::Set_val(FILE_ID file, uint64_t ofst, uint64_t sz) {
  AIR_ASSERT(Kind() == CONSTANT_KIND::EXT_FILE);
  _const->Set_ext_file(file, ofst, sz);
}

const char* CONSTANT::Array_buffer() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::ARRAY);
  AIR_ASSERT(Type()->Is_array());
  return _const->Array_buffer();
}

size_t CONSTANT::Array_size() const {
  // TODO: need type->Size();
  AIR_ASSERT(false);
  return 0;
}

size_t CONSTANT::Array_byte_len() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::ARRAY);
  AIR_ASSERT(Type()->Is_array());
  return _const->Array_length();
}

STR_PTR CONSTANT::Str_val() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::STR_ARRAY);
  return _glob->String(_const->Str_array_val());
}

FILE_ID CONSTANT::Ext_file_id() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::EXT_FILE);
  return _const->Ext_file();
}

FILE_PTR CONSTANT::Ext_file() const { return Glob_scope().File(Ext_file_id()); }

uint64_t CONSTANT::Ext_ofst() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::EXT_FILE);
  return _const->Ext_ofst();
}

uint64_t CONSTANT::Ext_size() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::EXT_FILE);
  return _const->Ext_size();
}

void CONSTANT::Print(std::ostream& os, uint32_t indent) const {
  uint32_t    id        = Id().Value();
  TYPE_PTR    type      = Type();
  uint32_t    type_id   = type->Id().Value();
  const char* type_name = type->Name()->Char_str();
  const char* kind_name = Const_kind_name();
  const char* type_kind = type->Type_kind_name();

  os << std::string(indent * INDENT_SPACE, ' ');
  os << std::hex << std::showbase;
  os << "CST[" << id << "] " << kind_name << ", TYP[" << type_id << "]("
     << type_kind << ",\"" << type_name << "\"), value(";
  if (Is_unsigned_int()) {
    os << "#" << Integer_literal().Val_as_uint64();
  } else if (Is_signed_int()) {
    os << "#" << Integer_literal().Val_as_int64();
  } else if (Is_float()) {
    PRIMITIVE_TYPE ctype = type->Cast_to_prim()->Encoding();
    if (ctype == PRIMITIVE_TYPE::FLOAT_32) {
      os << " #" << Float_literal().Val_as_float();
    } else if (ctype == PRIMITIVE_TYPE::FLOAT_64) {
      os << " #" << Float_literal().Val_as_double();
    } else {
      os << " #" << Float_literal().Val_as_long_double();
    }
  } else if (Kind() == CONSTANT_KIND::ARRAY) {
    os << "...array...";
  } else if (Kind() == CONSTANT_KIND::STR_ARRAY) {
    os << "\"" << Str_val()->Char_str() << "\"";
  } else if (Kind() == CONSTANT_KIND::EXT_FILE) {
    os << "\"" << Ext_file()->File_name()->Char_str()
       << "\", offset:" << Ext_ofst() << ", size:" << Ext_size();
  } else {
    AIR_ASSERT(0);
  }
  os << ")\n";
}

void CONSTANT::Print() const { Print(std::cout, 0); }

std::string CONSTANT::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

}  // namespace base
}  // namespace air
