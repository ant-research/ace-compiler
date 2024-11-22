//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <cstring>

#include "air/base/st.h"
#include "air/base/st_misc.h"

namespace air {
namespace base {

//=============================================================================
// class SYM_DATA member functions
//=============================================================================

SYM_DATA::SYM_DATA(SYMBOL_CLASS kind) : _first_aux_entry(Null_st_id) {
  AIR_ASSERT(OFFSETOF(SYM_DATA, _attr) == 0);
  AIR_ASSERT(sizeof(SYM_ATTR) == sizeof(uint32_t));
  *((uint32_t*)&_attr) = 0;
  AIR_ASSERT(sizeof(SYM_DATA) == 5 * sizeof(uint32_t));
  AIR_ASSERT(sizeof(AUX_SYM_DATA) == 10 * sizeof(uint32_t));
  _attr._u._comm._kind = static_cast<uint32_t>(kind);

  for (int i = 0; i < (sizeof(_u._id) / sizeof(_u._id[0])); i++) {
    _u._id[i] = Null_st_id;
  }
}

bool SYM_DATA::Is_initialized() const {
  AIR_ASSERT((Kind() == SYMBOL_CLASS::VAR) || (Kind() == SYMBOL_CLASS::FORMAL));
  return _attr._u._addr_datum._initialized;
}

bool SYM_DATA::Is_addr_saved() const {
  AIR_ASSERT((Kind() == SYMBOL_CLASS::VAR) || (Kind() == SYMBOL_CLASS::FORMAL));
  return _attr._u._addr_datum._addr_saved;
}

bool SYM_DATA::Is_addr_passed() const {
  AIR_ASSERT((Kind() == SYMBOL_CLASS::VAR) || (Kind() == SYMBOL_CLASS::FORMAL));
  return _attr._u._addr_datum._addr_passed;
}

bool SYM_DATA::Is_const_var() const {
  AIR_ASSERT((Kind() == SYMBOL_CLASS::VAR) || (Kind() == SYMBOL_CLASS::FORMAL));
  return _attr._u._addr_datum._const_var;
}

bool SYM_DATA::Is_used() const {
  switch (Kind()) {
    case SYMBOL_CLASS::FORMAL:
    case SYMBOL_CLASS::VAR:
    case SYMBOL_CLASS::ADDR_DATUM:
      return _attr._u._addr_datum._used;
    default:
      AIR_ASSERT(false);
      return false;
  }
}

bool SYM_DATA::Is_modified() const {
  AIR_ASSERT((Kind() == SYMBOL_CLASS::VAR) || (Kind() == SYMBOL_CLASS::FORMAL));
  return _attr._u._addr_datum._modified;
}

void SYM_DATA::Set_addr_saved(bool s) {
  AIR_ASSERT((Kind() == SYMBOL_CLASS::VAR) || (Kind() == SYMBOL_CLASS::FORMAL));
  _attr._u._addr_datum._addr_saved = s;
}

void SYM_DATA::Set_addr_passed(bool p) {
  AIR_ASSERT((Kind() == SYMBOL_CLASS::VAR) || (Kind() == SYMBOL_CLASS::FORMAL));
  _attr._u._addr_datum._addr_passed = p;
}

void SYM_DATA::Set_const_var(bool v) {
  AIR_ASSERT((Kind() == SYMBOL_CLASS::VAR) || (Kind() == SYMBOL_CLASS::FORMAL));
  _attr._u._addr_datum._const_var = v;
}

void SYM_DATA::Set_initialized() {
  AIR_ASSERT((Kind() == SYMBOL_CLASS::VAR) || (Kind() == SYMBOL_CLASS::FORMAL));
  _attr._u._addr_datum._initialized = 1;
}

void SYM_DATA::Reset_initialized() {
  AIR_ASSERT((Kind() == SYMBOL_CLASS::VAR) || (Kind() == SYMBOL_CLASS::FORMAL));
  _attr._u._addr_datum._initialized = 0;
}

void SYM_DATA::Set_used(bool u) {
  switch (Kind()) {
    case SYMBOL_CLASS::FORMAL:
    case SYMBOL_CLASS::VAR:
    case SYMBOL_CLASS::ADDR_DATUM:
      _attr._u._addr_datum._used = u;
      break;
    default:
      AIR_ASSERT(false);
      break;
  }
}

void SYM_DATA::Set_modified(bool m) {
  AIR_ASSERT((Kind() == SYMBOL_CLASS::VAR) || (Kind() == SYMBOL_CLASS::FORMAL));
  _attr._u._addr_datum._modified = m;
}

TYPE_ID
SYM_DATA::Addr_datum_type() const {
  AIR_ASSERT((Kind() == SYMBOL_CLASS::VAR) || (Kind() == SYMBOL_CLASS::FORMAL));
  return TYPE_ID(_u._addr_datum._type);
}

bool SYM_DATA::Is_func_defined() const {
  AIR_ASSERT(Kind() == SYMBOL_CLASS::FUNC);
  return (_attr._u._func._has_def == 1);
}

bool SYM_DATA::Is_in_packet() const {
  AIR_ASSERT((Kind() == SYMBOL_CLASS::VAR) || (Kind() == SYMBOL_CLASS::FORMAL));
  AIR_ASSERT((Kind() != SYMBOL_CLASS::FORMAL) ||
             (_attr._u._addr_datum._in_packet == 0));
  return _attr._u._addr_datum._in_packet;
}

bool SYM_DATA::Is_entry_prg_entry() const {
  AIR_ASSERT(Kind() == SYMBOL_CLASS::ENTRY);
  return _attr._u._entry._program_entry;
}

bool SYM_DATA::Has_implicit_ref() const {
  AIR_ASSERT((Kind() == SYMBOL_CLASS::VAR) || (Kind() == SYMBOL_CLASS::FORMAL));
  return (_attr._u._addr_datum._ref_implic != 0);
}

void SYM_DATA::Set_addr_datum_type(TYPE_ID id) {
  AIR_ASSERT((Kind() == SYMBOL_CLASS::VAR) || (Kind() == SYMBOL_CLASS::FORMAL));
  _u._addr_datum._type = id.Value();
}

void SYM_DATA::Set_entry_prg_entry() {
  AIR_ASSERT(Kind() == SYMBOL_CLASS::ENTRY);
  _attr._u._entry._program_entry = 1;
}

PACKET_ID
SYM_DATA::Owning_packet() const {
  AIR_ASSERT(Kind() == SYMBOL_CLASS::VAR);
  return Is_in_packet() ? PACKET_ID(_u._addr_datum._owning_packet)
                        : PACKET_ID();
}

FUNC_DEF_ID
SYM_DATA::Func_def_id() const {
  AIR_ASSERT(Kind() == SYMBOL_CLASS::FUNC);
  return FUNC_DEF_ID(_u._func._func_def);
}

void SYM_DATA::Set_func_def_id(FUNC_DEF_ID id) {
  AIR_ASSERT(Kind() == SYMBOL_CLASS::FUNC);
  _u._func._func_def = id.Value();
}

BLOCK_ID
SYM_DATA::Func_block_id() const {
  AIR_ASSERT(Kind() == SYMBOL_CLASS::FUNC);
  return BLOCK_ID(_u._func._func_blk);
}

void SYM_DATA::Set_func_block_id(BLOCK_ID blk) {
  AIR_ASSERT(Kind() == SYMBOL_CLASS::FUNC);
  _u._func._func_blk = blk.Value();
}

void SYM_DATA::Set_func_defined(bool def) {
  AIR_ASSERT(Kind() == SYMBOL_CLASS::FUNC);
  _attr._u._func._has_def = def;
}

uint32_t SYM_DATA::Func_nesting_level() const {
  AIR_ASSERT(Kind() == SYMBOL_CLASS::FUNC);
  return _attr._u._func._nesting_level;
}

void SYM_DATA::Set_func_nesting_level(uint32_t l) {
  AIR_ASSERT(Kind() == SYMBOL_CLASS::FUNC);
  _attr._u._func._nesting_level = l;
}

FUNC_ID SYM_DATA::Entry_owning_func() const {
  AIR_ASSERT(Kind() == SYMBOL_CLASS::ENTRY);
  return FUNC_ID(_u._entry_point._owning_func);
}

void SYM_DATA::Set_entry_owning_func(FUNC_ID func) {
  AIR_ASSERT(Kind() == SYMBOL_CLASS::ENTRY);
  _u._entry_point._owning_func = func.Value();
}

TYPE_ID SYM_DATA::Entry_type() const {
  AIR_ASSERT(Kind() == SYMBOL_CLASS::ENTRY);
  return TYPE_ID(_u._entry_point._signature);
}

void SYM_DATA::Set_entry_type(TYPE_ID id) {
  AIR_ASSERT(Kind() == SYMBOL_CLASS::ENTRY);
  _u._entry_point._signature = id.Value();
}

void SYM_DATA::Set_implicit_ref() {
  AIR_ASSERT((Kind() == SYMBOL_CLASS::VAR) || (Kind() == SYMBOL_CLASS::FORMAL));
  _attr._u._addr_datum._ref_implic = 1;
}

//=============================================================================
// class BLOCK_DATA member functions
//=============================================================================

void BLOCK_DATA::Set_owning_func(FUNC_ID id) {
  AIR_ASSERT(Kind() == BLOCK_KIND::FUNC);
  _func = id.Value();
}

FUNC_ID BLOCK_DATA::Owning_func_id() const {
  AIR_ASSERT(Kind() == BLOCK_KIND::FUNC);
  return FUNC_ID(_func);
}

STR_ID BLOCK_DATA::Named_scope_name() const {
  AIR_ASSERT(Kind() == BLOCK_KIND::NAMED_SCOPE);
  return STR_ID(_named_scope_name);
}

//=============================================================================
// class CONSTANT_DATA member functions
//=============================================================================

bool CONSTANT_DATA::Bool_val() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::BOOLEAN);
  return _bool_val;
}

uint64_t CONSTANT_DATA::Integer_val() const {
  AIR_ASSERT((Kind() == CONSTANT_KIND::SIGNED_INT) ||
             (Kind() == CONSTANT_KIND::UNSIGNED_INT));
  return INT_CONSTANT_DATA::Cast_to_me(*this)._val;
}

long double CONSTANT_DATA::Float_val() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::FLOAT);
  return FLOAT_CONSTANT_DATA::Cast_to_me(*this)._val;
}

long double CONSTANT_DATA::Complex_real_val() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::COMPLEX);
  return COMPLEX_CONSTANT_DATA::Cast_to_me(*this)._real;
}

long double CONSTANT_DATA::Complex_imag_val() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::COMPLEX);
  return COMPLEX_CONSTANT_DATA::Cast_to_me(*this)._imag;
}

POINTER_KIND CONSTANT_DATA::Ptr_kind() const {
  AIR_ASSERT((Kind() == CONSTANT_KIND::VAR_PTR) ||
             (Kind() == CONSTANT_KIND::ENTRY_PTR) ||
             (Kind() == CONSTANT_KIND::THUNK_PTR));
  return (POINTER_KIND)_ptr_kind;
}

ADDR_DATUM_ID CONSTANT_DATA::Datum_ptr_val() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::VAR_PTR);
  return ADDR_DATUM_ID(SYM_CONSTANT_DATA::Cast_to_me(*this)._sym);
}

ENTRY_ID CONSTANT_DATA::Entry_ptr_val() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::ENTRY_PTR);
  return ENTRY_ID(SYM_CONSTANT_DATA::Cast_to_me(*this)._sym);
}

CONSTANT_ID CONSTANT_DATA::Array_elem_ptr_base_ptr() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::ARRAY_ELEM_PTR);
  return DERIVED_PTR_CONSTANT_DATA::Cast_to_me(*this)._base;
}

int64_t CONSTANT_DATA::Array_elem_ptr_idx() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::ARRAY_ELEM_PTR);
  return DERIVED_PTR_CONSTANT_DATA::Cast_to_me(*this)._idx;
}

CONSTANT_ID CONSTANT_DATA::Field_ptr_base_ptr() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::FIELD_PTR);
  return DERIVED_PTR_CONSTANT_DATA::Cast_to_me(*this)._base;
}

FIELD_ID CONSTANT_DATA::Field_ptr_fld_id() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::FIELD_PTR);
  return FIELD_ID(DERIVED_PTR_CONSTANT_DATA::Cast_to_me(*this)._fld);
}

CONSTANT_ID CONSTANT_DATA::Ptr_ofst_base_ptr() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::PTR_OFST);
  return DERIVED_PTR_CONSTANT_DATA::Cast_to_me(*this)._base;
}

int64_t CONSTANT_DATA::Ptr_ofst_ofst() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::PTR_OFST);
  return DERIVED_PTR_CONSTANT_DATA::Cast_to_me(*this)._ofst;
}

uint64_t CONSTANT_DATA::Ptr_from_unsigned_val() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::PTR_FROM_UNSIGNED);
  return INT_CONSTANT_DATA::Cast_to_me(*this)._val;
}

CONSTANT_ID CONSTANT_DATA::Ptr_val() const {
  AIR_ASSERT((Kind() == CONSTANT_KIND::PTR_CAST) ||
             (Kind() == CONSTANT_KIND::PTR_INT));
  return POINTER_CONSTANT_DATA::Cast_to_me(*this)._ptr_val;
}

STR_ID CONSTANT_DATA::Str_array_val() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::STR_ARRAY);
  return STR_CONSTANT_DATA::Cast_to_me(*this)._str;
}

CONSTANT_ID CONSTANT_DATA::First_elem() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::ARRAY);
  return ARRAY_CONST_HEADER_CONSTANT_DATA::Cast_to_me(*this)._first;
}

CONSTANT_ID CONSTANT_DATA::Last_elem() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::ARRAY);
  return ARRAY_CONST_HEADER_CONSTANT_DATA::Cast_to_me(*this)._last;
}

CONSTANT_ID CONSTANT_DATA::Next_elem() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::ARRAY_ELEM);
  return ARRAY_ELEM_CONSTANT_DATA::Cast_to_me(*this)._next;
}

CONSTANT_ID CONSTANT_DATA::Elem_val() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::ARRAY_ELEM);
  return ARRAY_ELEM_CONSTANT_DATA::Cast_to_me(*this)._val;
}

uint32_t CONSTANT_DATA::Elem_index() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::ARRAY_ELEM);
  return ARRAY_ELEM_CONSTANT_DATA::Cast_to_me(*this)._elem_idx;
}

uint32_t CONSTANT_DATA::Elem_count() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::ARRAY_ELEM);
  return ARRAY_ELEM_CONSTANT_DATA::Cast_to_me(*this)._repeat_count;
}

CONSTANT_ID CONSTANT_DATA::First_fld() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::STRUCT);
  return STRUCT_CONST_HEADER_CONSTANT_DATA::Cast_to_me(*this)._first;
}

CONSTANT_ID CONSTANT_DATA::Last_fld() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::STRUCT);
  return STRUCT_CONST_HEADER_CONSTANT_DATA::Cast_to_me(*this)._last;
}

CONSTANT_ID CONSTANT_DATA::Next_fld() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::STRUCT_FIELD);
  return STRUCT_FIELD_CONSTANT_DATA::Cast_to_me(*this)._next;
}

CONSTANT_ID CONSTANT_DATA::Field_val() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::STRUCT_FIELD);
  return STRUCT_FIELD_CONSTANT_DATA::Cast_to_me(*this)._val;
}

FIELD_ID CONSTANT_DATA::Field_fld_id() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::STRUCT_FIELD);
  return STRUCT_FIELD_CONSTANT_DATA::Cast_to_me(*this)._fld;
}

FIELD_ID CONSTANT_DATA::Union_fld_id() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::UNION);
  return UNION_CONSTANT_DATA::Cast_to_me(*this)._fld;
}

CONSTANT_ID CONSTANT_DATA::Union_val() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::UNION);
  return UNION_CONSTANT_DATA::Cast_to_me(*this)._val;
}

TYPE_ID CONSTANT_DATA::Union_type() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::UNION);
  return UNION_CONSTANT_DATA::Cast_to_me(*this)._type;
}

ENTRY_ID CONSTANT_DATA::Func_desc_entry() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::ENTRY_FUNC_DESC);
  return ENTRY_ID(SYM_CONSTANT_DATA::Cast_to_me(*this)._sym);
}

CONSTANT_ID
CONSTANT_DATA::Named_base() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::NAMED);
  return NAMED_CONSTANT_DATA::Cast_to_me(*this)._base;
}

SPOS CONSTANT_DATA::Named_spos() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::NAMED);
  return NAMED_CONSTANT_DATA::Cast_to_me(*this)._spos;
}

STR_ID CONSTANT_DATA::Named_name() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::NAMED);
  return NAMED_CONSTANT_DATA::Cast_to_me(*this)._name;
}

const char* CONSTANT_DATA::Array_buffer() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::ARRAY);
  return ARRAY_CONSTANT_DATA::Cast_to_me(*this)._buf;
}

size_t CONSTANT_DATA::Array_length() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::ARRAY);
  return ARRAY_CONSTANT_DATA::Cast_to_me(*this)._len;
}

FILE_ID CONSTANT_DATA::Ext_file() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::EXT_FILE);
  return EXTERN_CONSTANT_DATA::Cast_to_me(*this)._file;
}

uint64_t CONSTANT_DATA::Ext_ofst() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::EXT_FILE);
  return EXTERN_CONSTANT_DATA::Cast_to_me(*this)._ofst;
}

uint64_t CONSTANT_DATA::Ext_size() const {
  AIR_ASSERT(Kind() == CONSTANT_KIND::EXT_FILE);
  return EXTERN_CONSTANT_DATA::Cast_to_me(*this)._size;
}

void CONSTANT_DATA::Set_bool_val(bool val) {
  AIR_ASSERT(Kind() == CONSTANT_KIND::BOOLEAN);
  _bool_val = val;
}

void CONSTANT_DATA::Set_integer_val(uint64_t val) {
  AIR_ASSERT((Kind() == CONSTANT_KIND::SIGNED_INT) ||
             (Kind() == CONSTANT_KIND::UNSIGNED_INT));
  INT_CONSTANT_DATA::Cast_to_me(*this)._val = val;
}

void CONSTANT_DATA::Set_float_val(long double val) {
  AIR_ASSERT(Kind() == CONSTANT_KIND::FLOAT);
  FLOAT_CONSTANT_DATA::Cast_to_me(*this)._val = val;
}

void CONSTANT_DATA::Set_array_elem_ptr_val(CONSTANT_ID base, int64_t idx) {
  AIR_ASSERT(Kind() == CONSTANT_KIND::ARRAY_ELEM_PTR);
  DERIVED_PTR_CONSTANT_DATA::Cast_to_me(*this)._idx  = idx;
  DERIVED_PTR_CONSTANT_DATA::Cast_to_me(*this)._base = base;
}

void CONSTANT_DATA::Set_ptr_ofst_val(CONSTANT_ID base, int64_t ofst) {
  AIR_ASSERT(Kind() == CONSTANT_KIND::PTR_OFST);
  DERIVED_PTR_CONSTANT_DATA::Cast_to_me(*this)._ofst = ofst;
  DERIVED_PTR_CONSTANT_DATA::Cast_to_me(*this)._base = base;
}

void CONSTANT_DATA::Set_str_array_val(STR_ID str) {
  AIR_ASSERT(Kind() == CONSTANT_KIND::STR_ARRAY);
  STR_CONSTANT_DATA::Cast_to_me(*this)._str = str;
}

void CONSTANT_DATA::Set_ext_file(FILE_ID f, uint64_t ofst, uint64_t sz) {
  AIR_ASSERT(Kind() == CONSTANT_KIND::EXT_FILE);
  EXTERN_CONSTANT_DATA::Cast_to_me(*this)._file = f;
  EXTERN_CONSTANT_DATA::Cast_to_me(*this)._ofst = ofst;
  EXTERN_CONSTANT_DATA::Cast_to_me(*this)._size = sz;
}

//=============================================================================
// class INT_CONSTANT_DATA member functions
//=============================================================================

INT_CONSTANT_DATA& INT_CONSTANT_DATA::Cast_to_me(CONSTANT_DATA& data) {
  AIR_ASSERT((data.Kind() == CONSTANT_KIND::SIGNED_INT) ||
             (data.Kind() == CONSTANT_KIND::UNSIGNED_INT) ||
             (data.Kind() == CONSTANT_KIND::PTR_FROM_UNSIGNED));
  return static_cast<INT_CONSTANT_DATA&>(data);
}

const INT_CONSTANT_DATA& INT_CONSTANT_DATA::Cast_to_me(
    const CONSTANT_DATA& data) {
  AIR_ASSERT((data.Kind() == CONSTANT_KIND::SIGNED_INT) ||
             (data.Kind() == CONSTANT_KIND::UNSIGNED_INT) ||
             (data.Kind() == CONSTANT_KIND::PTR_FROM_UNSIGNED));
  return static_cast<const INT_CONSTANT_DATA&>(data);
}

//=============================================================================
// class INT_CONSTANT_DATA member functions
//=============================================================================

FLOAT_CONSTANT_DATA& FLOAT_CONSTANT_DATA::Cast_to_me(CONSTANT_DATA& data) {
  AIR_ASSERT(data.Kind() == CONSTANT_KIND::FLOAT);
  return static_cast<FLOAT_CONSTANT_DATA&>(data);
}

const FLOAT_CONSTANT_DATA& FLOAT_CONSTANT_DATA::Cast_to_me(
    const CONSTANT_DATA& data) {
  AIR_ASSERT(data.Kind() == CONSTANT_KIND::FLOAT);
  return static_cast<const FLOAT_CONSTANT_DATA&>(data);
}

//=============================================================================
// class COMPLEX_CONSTANT_DATA member functions
//=============================================================================

COMPLEX_CONSTANT_DATA& COMPLEX_CONSTANT_DATA::Cast_to_me(CONSTANT_DATA& data) {
  AIR_ASSERT(data.Kind() == CONSTANT_KIND::COMPLEX);
  return static_cast<COMPLEX_CONSTANT_DATA&>(data);
}

const COMPLEX_CONSTANT_DATA& COMPLEX_CONSTANT_DATA::Cast_to_me(
    const CONSTANT_DATA& data) {
  AIR_ASSERT(data.Kind() == CONSTANT_KIND::COMPLEX);
  return static_cast<const COMPLEX_CONSTANT_DATA&>(data);
}

//=============================================================================
// class POINTER_CONSTANT_DATA member functions
//=============================================================================

POINTER_CONSTANT_DATA& POINTER_CONSTANT_DATA::Cast_to_me(CONSTANT_DATA& data) {
  CONSTANT_KIND k = data.Kind();
  AIR_ASSERT((k == CONSTANT_KIND::PTR_INT) || (k == CONSTANT_KIND::PTR_CAST));
  return static_cast<POINTER_CONSTANT_DATA&>(data);
}

const POINTER_CONSTANT_DATA& POINTER_CONSTANT_DATA::Cast_to_me(
    const CONSTANT_DATA& data) {
  CONSTANT_KIND k = data.Kind();
  AIR_ASSERT((k == CONSTANT_KIND::PTR_INT) || (k == CONSTANT_KIND::PTR_CAST));
  return static_cast<const POINTER_CONSTANT_DATA&>(data);
}

//=============================================================================
// class SYM_CONSTANT_DATA member functions
//=============================================================================

SYM_CONSTANT_DATA& SYM_CONSTANT_DATA::Cast_to_me(CONSTANT_DATA& data) {
  CONSTANT_KIND k = data.Kind();
  AIR_ASSERT((k == CONSTANT_KIND::VAR_PTR) || (k == CONSTANT_KIND::ENTRY_PTR) ||
             (k == CONSTANT_KIND::THUNK_PTR) ||
             (k == CONSTANT_KIND::ENTRY_FUNC_DESC) ||
             (k == CONSTANT_KIND::THUNK_FUNC_DESC));
  return static_cast<SYM_CONSTANT_DATA&>(data);
}

const SYM_CONSTANT_DATA& SYM_CONSTANT_DATA::Cast_to_me(
    const CONSTANT_DATA& data) {
  CONSTANT_KIND k = data.Kind();
  AIR_ASSERT((k == CONSTANT_KIND::VAR_PTR) || (k == CONSTANT_KIND::ENTRY_PTR) ||
             (k == CONSTANT_KIND::THUNK_PTR) ||
             (k == CONSTANT_KIND::ENTRY_FUNC_DESC) ||
             (k == CONSTANT_KIND::THUNK_FUNC_DESC));
  return static_cast<const SYM_CONSTANT_DATA&>(data);
}

//=============================================================================
// class DERIVED_PTR_CONSTANT_DATA member functions
//=============================================================================

DERIVED_PTR_CONSTANT_DATA& DERIVED_PTR_CONSTANT_DATA::Cast_to_me(
    CONSTANT_DATA& data) {
  CONSTANT_KIND k = data.Kind();
  AIR_ASSERT((k == CONSTANT_KIND::ARRAY_ELEM_PTR) ||
             (k == CONSTANT_KIND::PTR_OFST) || (k == CONSTANT_KIND::FIELD_PTR));
  return static_cast<DERIVED_PTR_CONSTANT_DATA&>(data);
}

const DERIVED_PTR_CONSTANT_DATA& DERIVED_PTR_CONSTANT_DATA::Cast_to_me(
    const CONSTANT_DATA& data) {
  CONSTANT_KIND k = data.Kind();
  AIR_ASSERT((k == CONSTANT_KIND::ARRAY_ELEM_PTR) ||
             (k == CONSTANT_KIND::PTR_OFST) || (k == CONSTANT_KIND::FIELD_PTR));
  return static_cast<const DERIVED_PTR_CONSTANT_DATA&>(data);
}

//=============================================================================
// class STR_CONSTANT_DATA member functions
//=============================================================================

STR_CONSTANT_DATA& STR_CONSTANT_DATA::Cast_to_me(CONSTANT_DATA& data) {
  AIR_ASSERT(data.Kind() == CONSTANT_KIND::STR_ARRAY);
  return static_cast<STR_CONSTANT_DATA&>(data);
}

const STR_CONSTANT_DATA& STR_CONSTANT_DATA::Cast_to_me(
    const CONSTANT_DATA& data) {
  AIR_ASSERT(data.Kind() == CONSTANT_KIND::STR_ARRAY);
  return static_cast<const STR_CONSTANT_DATA&>(data);
}

//=============================================================================
// class ARRAY_CONSTANT_DATA member functions
//=============================================================================

ARRAY_CONSTANT_DATA& ARRAY_CONSTANT_DATA::Cast_to_me(CONSTANT_DATA& data) {
  AIR_ASSERT(data.Kind() == CONSTANT_KIND::ARRAY);
  return static_cast<ARRAY_CONSTANT_DATA&>(data);
}

const ARRAY_CONSTANT_DATA& ARRAY_CONSTANT_DATA::Cast_to_me(
    const CONSTANT_DATA& data) {
  AIR_ASSERT(data.Kind() == CONSTANT_KIND::ARRAY);
  return static_cast<const ARRAY_CONSTANT_DATA&>(data);
}

//=============================================================================
// class ARRAY_CONST_HEADER_CONSTANT_DATA member functions
//=============================================================================

ARRAY_CONST_HEADER_CONSTANT_DATA& ARRAY_CONST_HEADER_CONSTANT_DATA::Cast_to_me(
    CONSTANT_DATA& data) {
  AIR_ASSERT(data.Kind() == CONSTANT_KIND::ARRAY);
  return static_cast<ARRAY_CONST_HEADER_CONSTANT_DATA&>(data);
}

const ARRAY_CONST_HEADER_CONSTANT_DATA&
ARRAY_CONST_HEADER_CONSTANT_DATA::Cast_to_me(const CONSTANT_DATA& data) {
  AIR_ASSERT(data.Kind() == CONSTANT_KIND::ARRAY);
  return static_cast<const ARRAY_CONST_HEADER_CONSTANT_DATA&>(data);
}

//=============================================================================
// class ARRAY_ELEM_CONSTANT_DATA member functions
//=============================================================================

ARRAY_ELEM_CONSTANT_DATA& ARRAY_ELEM_CONSTANT_DATA::Cast_to_me(
    CONSTANT_DATA& data) {
  AIR_ASSERT(data.Kind() == CONSTANT_KIND::ARRAY_ELEM);
  return static_cast<ARRAY_ELEM_CONSTANT_DATA&>(data);
}

const ARRAY_ELEM_CONSTANT_DATA& ARRAY_ELEM_CONSTANT_DATA::Cast_to_me(
    const CONSTANT_DATA& data) {
  AIR_ASSERT(data.Kind() == CONSTANT_KIND::ARRAY_ELEM);
  return static_cast<const ARRAY_ELEM_CONSTANT_DATA&>(data);
}

//=============================================================================
// class STRUCT_CONST_HEADER_CONSTANT_DATA member functions
//=============================================================================

STRUCT_CONST_HEADER_CONSTANT_DATA&
STRUCT_CONST_HEADER_CONSTANT_DATA::Cast_to_me(CONSTANT_DATA& data) {
  AIR_ASSERT(data.Kind() == CONSTANT_KIND::STRUCT);
  return static_cast<STRUCT_CONST_HEADER_CONSTANT_DATA&>(data);
}

const STRUCT_CONST_HEADER_CONSTANT_DATA&
STRUCT_CONST_HEADER_CONSTANT_DATA::Cast_to_me(const CONSTANT_DATA& data) {
  AIR_ASSERT(data.Kind() == CONSTANT_KIND::STRUCT);
  return static_cast<const STRUCT_CONST_HEADER_CONSTANT_DATA&>(data);
}

//=============================================================================
// class STRUCT_FIELD_CONSTANT_DATA member functions
//=============================================================================

STRUCT_FIELD_CONSTANT_DATA& STRUCT_FIELD_CONSTANT_DATA::Cast_to_me(
    CONSTANT_DATA& data) {
  AIR_ASSERT(data.Kind() == CONSTANT_KIND::STRUCT_FIELD);
  return static_cast<STRUCT_FIELD_CONSTANT_DATA&>(data);
}

const STRUCT_FIELD_CONSTANT_DATA& STRUCT_FIELD_CONSTANT_DATA::Cast_to_me(
    const CONSTANT_DATA& data) {
  AIR_ASSERT(data.Kind() == CONSTANT_KIND::STRUCT_FIELD);
  return static_cast<const STRUCT_FIELD_CONSTANT_DATA&>(data);
}

//=============================================================================
// class UNION_CONSTANT_DATA member functions
//=============================================================================

UNION_CONSTANT_DATA& UNION_CONSTANT_DATA::Cast_to_me(CONSTANT_DATA& data) {
  AIR_ASSERT(data.Kind() == CONSTANT_KIND::UNION);
  return static_cast<UNION_CONSTANT_DATA&>(data);
}

const UNION_CONSTANT_DATA& UNION_CONSTANT_DATA::Cast_to_me(
    const CONSTANT_DATA& data) {
  AIR_ASSERT(data.Kind() == CONSTANT_KIND::UNION);
  return static_cast<const UNION_CONSTANT_DATA&>(data);
}

//=============================================================================
// class NAMED_CONSTANT_DATA member functions
//=============================================================================

NAMED_CONSTANT_DATA& NAMED_CONSTANT_DATA::Cast_to_me(CONSTANT_DATA& data) {
  AIR_ASSERT(data.Kind() == CONSTANT_KIND::NAMED);
  return static_cast<NAMED_CONSTANT_DATA&>(data);
}

const NAMED_CONSTANT_DATA& NAMED_CONSTANT_DATA::Cast_to_me(
    const CONSTANT_DATA& data) {
  AIR_ASSERT(data.Kind() == CONSTANT_KIND::NAMED);
  return static_cast<const NAMED_CONSTANT_DATA&>(data);
}

//=============================================================================
// class EXTERN_CONSTANT_DATA member functions
//=============================================================================

EXTERN_CONSTANT_DATA& EXTERN_CONSTANT_DATA::Cast_to_me(CONSTANT_DATA& data) {
  AIR_ASSERT(data.Kind() == CONSTANT_KIND::EXT_FILE);
  return static_cast<EXTERN_CONSTANT_DATA&>(data);
}

const EXTERN_CONSTANT_DATA& EXTERN_CONSTANT_DATA::Cast_to_me(
    const CONSTANT_DATA& data) {
  AIR_ASSERT(data.Kind() == CONSTANT_KIND::EXT_FILE);
  return static_cast<const EXTERN_CONSTANT_DATA&>(data);
}

//=============================================================================
// class AUX_SYM_DATA member functions
//=============================================================================

void AUX_SYM_DATA::Init_datum_flag() {
  AIR_ASSERT(sizeof(_u._datum._flag) == sizeof(int32_t));
  memset(&_u._datum._flag, 0, sizeof(_u._datum._flag));
  _u._datum._initializer = Null_st_id;
  _u._datum._map_text    = Null_st_id;
}

//=============================================================================
// class FIELD_DATA member functions
//=============================================================================

SPOS FIELD_DATA::Spos() const {
  AIR_ASSERT(Kind() == FIELD_KIND::REGULAR || Kind() == FIELD_KIND::STATIC);
  return _spos;
}

void FIELD_DATA::Set_spos(const SPOS& s) {
  AIR_ASSERT(Kind() == FIELD_KIND::REGULAR || Kind() == FIELD_KIND::STATIC);
  _spos = s;
}

//=============================================================================
// class FUNC_DEF_DATA member functions
//=============================================================================

FUNC_DEF_DATA::FUNC_DEF_DATA()
    : _begin(), _end(), _entry_stmt(Null_st_id), _annotation_id(Null_st_id) {
  memset(&_attr, 0, sizeof(_attr));
}

}  // namespace base
}  // namespace air
