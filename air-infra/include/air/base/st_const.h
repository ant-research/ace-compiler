//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_ST_CONST_H
#define AIR_BASE_ST_CONST_H

#include "air/base/st_decl.h"

namespace air {
namespace base {

class CONSTANT {
  friend class GLOB_SCOPE;
  friend class ELEM_CONST_ITER;
  friend class FIELD_CONST_ITER;
  PTR_FRIENDS(CONSTANT);

public:
  CONSTANT() : _glob(0), _const() {}
  CONSTANT(const GLOB_SCOPE& glob, CONSTANT_ID cst);

  // bool Is_null() const { return _glob == 0; }
  bool Is_zero() const;
  bool Is_one() const;
  bool Is_minus_one() const;
  bool Is_signed_int() const { return (Kind() == CONSTANT_KIND::SIGNED_INT); }
  bool Is_unsigned_int() const {
    return (Kind() == CONSTANT_KIND::UNSIGNED_INT);
  }
  bool Is_int() const { return (Is_signed_int() || Is_unsigned_int()); }
  bool Is_float() const { return (Kind() == CONSTANT_KIND::FLOAT); }
  bool Is_bool() const { return (Kind() == CONSTANT_KIND::BOOLEAN); }

  TYPE_ID       Type_id() const;
  TYPE_PTR      Type() const;
  CONSTANT_ID   Base_const_id() const;
  CONSTANT_PTR  Base_const() const;
  TYPE_ID       Parent_type_id() const;
  TYPE_PTR      Parent_type() const;
  CONSTANT_KIND Kind() const;
  POINTER_KIND  Ptr_kind() const;
  CONSTANT_ID   Id() const;
  GLOB_SCOPE&   Glob_scope() const { return *_glob; }
  const char*   Const_kind_name() const {
    return Constant_kind_name_arr[static_cast<uint32_t>(Kind())];
  }

  // CONSTANT_KIND::BOOLEAN
  bool Bool_val() const;

  // CONSTANT_KIND::SIGNED_INT, CONSTANT_KIND::UNSIGNED_INT
  INT_LITERAL Integer_literal() const;

  // CONSTANT_KIND::FLOAT
  FLOAT_LITERAL Float_literal() const;

  // CONSTANT_KIND::COMPLEX
  COMPLEX_LITERAL Complex_literal() const;
  FLOAT_LITERAL   Complex_real() const;
  FLOAT_LITERAL   Complex_imag() const;

  // CONSTANT_KIND::PTR_INT, CONSTANT_KIND::PTR_CAST
  CONSTANT_ID  Addr_val_const_id() const;
  CONSTANT_PTR Addr_val() const;

  // CONSTANT_KIND::PTR_FROM_UNSIGNED
  uint64_t Ptr_val_as_unsigned() const;

  // CONSTANT_KIND::VAR_PTR
  ADDR_DATUM_ID  Var_id() const;
  ADDR_DATUM_PTR Var() const;

  // CONSTANT_KIND::ENTRY_PTR, CONSTANT_KIND::ENTRY_FUNC_DESC
  ENTRY_ID  Entry_id() const;
  ENTRY_PTR Entry() const;

  // CONSTANT_KIND::ARRAY_ELEM_PTR, CONSTANT_KIND::FIELD_PTR,
  // CONSTANT_KIND::PTR_OFST
  CONSTANT_ID  Base_addr_const_id() const;
  CONSTANT_PTR Base_add() const;
  // CONSTANT_KIND::PTR_OFST
  int64_t Ofst() const;
  // CONSTANT_KIND::ARRAY_ELEM_PTR
  int64_t Idx() const;
  // CONSTANT_KIND::FIELD_PTR, CONSTANT_KIND::UNION
  FIELD_ID  Field_id() const;
  FIELD_PTR Field() const;

  // CONSTANT_KIND::ARRAY
  template <typename ELEM_TYPE>
  const ELEM_TYPE* Array_ptr() const {
    return (ELEM_TYPE*)Array_buffer();
  }
  template <typename ELEM_TYPE>
  ELEM_TYPE Array_elem(size_t index) const {
    AIR_ASSERT(index < (Array_byte_len() / sizeof(ELEM_TYPE)));
    return Array_ptr<ELEM_TYPE>()[index];
  }
  const char* Array_buffer() const;
  size_t      Array_size() const;
  size_t      Array_byte_len() const;

  // Deprecated CONSTANT_KIND::ARRAY
  void            Add_array_elem_const(uint64_t index, uint64_t repeat_count,
                                       CONST_CONSTANT_PTR elem_val);
  void            Add_array_elem_const(uint64_t index, uint64_t repeat_count,
                                       CONSTANT_ID elem_val);
  ELEM_CONST_ITER Begin_elem_const() const;
  ELEM_CONST_ITER End_elem_const() const;

  // CONSTANT_KIND::STR_ARRAY
  STR_PTR Str_val() const;

  // CONSTANT_KIND::STRUCT
  void             Add_fld_const(CONST_FIELD_PTR fld, CONST_CONSTANT_PTR val);
  void             Add_fld_const(FIELD_ID fld, CONSTANT_ID val);
  FIELD_CONST_ITER Begin_fld_const() const;
  FIELD_CONST_ITER End_fld_const() const;
  ENTRY_ID         Func_desc_entry_id() const;
  ENTRY_PTR        Func_desc_entry() const;

  // CONSTANT_KIND::UNION
  TYPE_ID      Union_type_id() const;
  TYPE_PTR     Union_type() const;
  CONSTANT_ID  Field_val_const_id() const;
  CONSTANT_PTR Field_val() const;

  // CONSTANT_KIND::STRUCT_FIELD
  CONSTANT_PTR Field_val_const() const;
  FIELD_PTR    Field_const_fld() const;

  // CONSTANT_KIND::NAMED
  STR_PTR Name() const;
  SPOS    Spos() const;

  // CONSTANT_KIND::EXT_FILE
  FILE_ID  Ext_file_id() const;
  FILE_PTR Ext_file() const;
  uint64_t Ext_ofst() const;
  uint64_t Ext_size() const;

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

private:
  CONSTANT(GLOB_SCOPE* glob, CONSTANT_DATA_PTR ptr) : _glob(glob), _const(ptr) {
    AIR_ASSERT(glob);
  }

  void Set_type(TYPE_ID id);
  // For CONSTANT_KIND::BOOLEAN
  void Set_val(bool val);
  // For CONSTANT_KIND::SIGNED_INT
  void Set_val(int64_t val);
  // For CONSTANT_KIND::UNSIGNED_INT
  void Set_val(uint64_t val);
  // For CONSTANT_KIND::PTR_INT
  void Set_val(CONSTANT_ID ptr);
  // For CONSTANT_KIND::FLOAT
  void Set_val(long double val);
  // For CONSTANT_KIND::COMPLEX
  void Set_val(long double real, long double imag);
  // For CONSTANT_KIND::VAR_PTR
  void Set_val(POINTER_KIND kind, ADDR_DATUM_ID var);
  // For CONSTANT_KIND::ENTRY_PTR
  void Set_val(POINTER_KIND kind, ENTRY_ID entry);
  // For CONSTANT_KIND::ARRAY_ELEM_PTR, CONSTANT_KIND::PTR_OFST
  void Set_val(CONSTANT_ID base, int64_t idx_or_ofst);
  // For CONSTANT_KIND::FIELD_PTR
  void Set_val(CONSTANT_ID record, FIELD_ID fld);
  // For CONSTANT_KIND::STR_ARRAY
  void Set_val(STR_ID str);
  // For CONSTANT_KIND::ENTRY_FUNC_DESC
  void Set_val(ENTRY_ID entry);
  // For CONSTANT_KIND::THUNK_FUNC_DESC
  void Set_val(THUNK_ID thunk);
  // For CONSTANT_KIND::UNION
  void Set_val(CONSTANT_ID fld_val, FIELD_ID fld_id, TYPE_ID type);
  // For CONSTANT_KIND::NAMED
  void Set_val(CONSTANT_ID base, STR_ID name, const SPOS& spos);
  // For CONSTANT_KIND::EXT_FILE
  void Set_val(FILE_ID file, uint64_t ofst, uint64_t sz);

  void Set_parent_type(TYPE_ID type);

  // For CONSTANT_KIND::STRUCT
  CONSTANT_ID First_fld_const_id() const;
  CONSTANT_ID Last_fld_const_id() const;
  void        Set_first_fld_const_id(CONSTANT_ID id);
  void        Set_last_fld_const_id(CONSTANT_ID id);

  // For CONSTANT_KIND::STRUCT_FIELD
  CONSTANT_ID Next_fld_const_id() const;
  void        Set_next_fld_const_id(CONSTANT_ID id);
  void        Set_fld_val_const_id(CONSTANT_ID id);
  void        Set_fld_const_fld_id(FIELD_ID id);

  // For CONSTANT_KIND::ARRAY
  CONSTANT_ID First_elem_const_id() const;
  CONSTANT_ID Last_elem_const_id() const;
  void        Set_first_elem_const_id(CONSTANT_ID id);
  void        Set_last_elem_const_id(CONSTANT_ID id);

  // For CONSTANT_KIND::ARRAY_ELEM
  CONSTANT_ID  Next_elem_const_id() const;
  CONSTANT_PTR Elem_val_const() const;
  uint32_t     Elem_index() const;
  uint32_t     Elem_count() const;
  void         Set_next_elem_const_id(CONSTANT_ID id);
  void         Set_elem_val(uint64_t index, uint64_t count, CONSTANT_ID val);

  bool              Is_null() const { return _const.Is_null(); }
  CONSTANT_DATA_PTR Data() const { return _const; }

  static const char*
      Constant_kind_name_arr[static_cast<uint32_t>(CONSTANT_KIND::END)];

  GLOB_SCOPE*       _glob;
  CONSTANT_DATA_PTR _const;
};

class INT_LITERAL {
public:
  INT_LITERAL(const INT_LITERAL& o) : _val(o._val), _positive(o._positive) {}

  static INT_LITERAL Int_literal_from_int64(int64_t val) {
    return INT_LITERAL(val, (val < 0) ? false : true);
  }
  static INT_LITERAL Int_literal_from_uint64(uint64_t val) {
    return INT_LITERAL(val, true);
  }

  bool Is_positive() const { return _positive; }
  bool Is_negative() const { return !_positive; }
  bool Is_zero() const { return (_val == 0); }
  bool Is_one() const { return (Is_positive() && (_val == 1)); }
  bool Is_minus_one() const {
    return (Is_negative() && (static_cast<int64_t>(_val) == -1));
  }

  bool Fit_uint8() const {
    return (Is_positive() && (_val <= static_cast<uint64_t>(UINT8_MAX)));
  }
  bool Fit_int8() const {
    return ((Is_negative() && (_val >= static_cast<uint64_t>(INT8_MIN))) ||
            (Is_positive() && (_val <= static_cast<uint64_t>(INT8_MAX))));
  }
  bool Fit_uint16() const {
    return (Is_positive() && (_val <= static_cast<uint64_t>(UINT16_MAX)));
  }
  bool Fit_int16() const {
    return ((Is_negative() && (_val >= static_cast<uint64_t>(INT16_MIN))) ||
            (Is_positive() && (_val <= static_cast<uint64_t>(INT16_MAX))));
  }
  bool Fit_uint32() const {
    return (Is_positive() && (_val <= static_cast<uint64_t>(UINT32_MAX)));
  }
  bool Fit_int32() const {
    return ((Is_negative() && (_val >= static_cast<uint64_t>(INT32_MIN))) ||
            (Is_positive() && (_val <= static_cast<uint64_t>(INT32_MAX))));
  }
  bool Fit_uint64() const { return Is_positive(); }
  bool Fit_int64() const {
    return (Is_negative() || (_val <= static_cast<uint64_t>(INT64_MAX)));
  }

  uint64_t Value() const { return _val; }
  uint8_t  Val_as_uint8() const {
    AIR_ASSERT(Fit_uint8());
    return static_cast<uint8_t>(Value());
  }
  int8_t Val_as_int8() const {
    AIR_ASSERT(Fit_int8());
    return static_cast<int8_t>(Value());
  }
  uint16_t Val_as_uint16() const {
    AIR_ASSERT(Fit_uint16());
    return static_cast<uint16_t>(Value());
  }
  int16_t Val_as_int16() const {
    AIR_ASSERT(Fit_int16());
    return static_cast<int16_t>(Value());
  }
  uint32_t Val_as_uint32() const {
    AIR_ASSERT(Fit_uint32());
    return static_cast<uint32_t>(Value());
  }
  int32_t Val_as_int32() const {
    AIR_ASSERT(Fit_int32());
    return static_cast<int32_t>(Value());
  }
  uint64_t Val_as_uint64() const {
    AIR_ASSERT(Fit_uint64());
    return Value();
  }
  int64_t Val_as_int64() const {
    AIR_ASSERT(Fit_int64());
    return static_cast<int64_t>(Value());
  }

private:
  INT_LITERAL(uint64_t i) : _val(i), _positive(true) {}
  INT_LITERAL(uint64_t i, bool positive) : _val(i) {
    _positive = ((i == 0) ? true : positive);
  }

  bool     _positive;
  uint64_t _val;
};

class FLOAT_LITERAL {
  friend class COMPLEX_LITERAL;

public:
  FLOAT_LITERAL(const FLOAT_LITERAL& o) : _val(o._val) {}

  static FLOAT_LITERAL Float_literal_from_long_double(long double val) {
    return FLOAT_LITERAL(val);
  }

  float       Val_as_float() const { return static_cast<float>(_val); }
  double      Val_as_double() const { return static_cast<double>(_val); }
  long double Val_as_long_double() const { return _val; }

  bool Is_negative() const { return (_val < +0.0); }
  bool Is_positive() const { return !Is_negative(); }
  bool Is_zero() const { return ((_val == +0.0) || (_val == -0.0)); }
  bool Is_one() const { return (_val == 1.0); }
  bool Is_minus_one() const { return (_val == static_cast<long double>(-1.0)); }

private:
  FLOAT_LITERAL(long double value) : _val(value) {}

  long double _val;
};

class COMPLEX_LITERAL {
public:
  COMPLEX_LITERAL(const COMPLEX_LITERAL& o) : _real(o._real), _imag(o._imag) {}

  static COMPLEX_LITERAL Complex_literal_from_long_double(long double real,
                                                          long double imag) {
    return COMPLEX_LITERAL(real, imag);
  }

  FLOAT_LITERAL Real_literal() const { return _real; }
  FLOAT_LITERAL Imag_literal() const { return _imag; }

  bool Is_zero() const { return (_real.Is_zero() && _imag.Is_zero()); }
  bool Is_one() const { return (_real.Is_one() && _imag.Is_one()); }
  bool Is_minus_one() const {
    return (_real.Is_minus_one() && _imag.Is_zero());
  }

private:
  COMPLEX_LITERAL(long double real, long double imag)
      : _real(real), _imag(imag) {}

  FLOAT_LITERAL _real;
  FLOAT_LITERAL _imag;
};

class EXT_CONST_FILE {
public:
  EXT_CONST_FILE() : _fd(-1), _ofst(0){};
  int      Fd() const { return _fd; }
  uint64_t Ofst() const { return _ofst; }
  uint64_t Size() const { return _size; }

  void Set_fd(int fd) { _fd = fd; }
  void Set_ofst(uint64_t ofst) { _ofst = ofst; }
  void Set_size(uint64_t sz) { _size = sz; }

private:
  int _fd;
  union {
    uint64_t _ofst;  // WO_EXT_CONST to mark current offset of the file
    uint64_t _size;  // RO_EXT_CONST to mark size of the file
  };
};

}  // namespace base
}  // namespace air

#endif
