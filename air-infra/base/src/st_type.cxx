//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <iostream>
#include <sstream>

#include "air/base/st.h"

namespace air {
namespace base {

const char* Pointer_kind_names[static_cast<uint32_t>(POINTER_KIND::END)] = {
    "32flat", "64flat"};

const char* TYPE::Type_kind_name_arr[static_cast<uint32_t>(TYPE_TRAIT::END)] = {
    "unknown", "primitive", "va_list",   "pointer",
    "array",   "record",    "signature", "subtype"};

//=============================================================================
// class FIELD member functions
//=============================================================================

FIELD::FIELD(const GLOB_SCOPE& glob, FIELD_ID fld) : _glob(0), _fld() {
  if (fld.Is_null()) return;
  _glob = const_cast<GLOB_SCOPE*>(&glob);
  _fld  = glob.Field_table().Find(ID<FIELD_DATA>(fld.Index()));
}

FIELD_ID
FIELD::Id() const {
  if (_fld == FIELD_DATA_PTR()) return FIELD_ID(Null_st_id);
  return FIELD_ID(_fld.Id().Value(), 0);
}

TYPE_PTR
FIELD::Type() const { return Glob_scope().Type(Type_id()); }

RECORD_TYPE_PTR
FIELD::Owning_rec_type() const {
  return Glob_scope().Rec_type(Owning_rec_type_id());
}

STR_PTR
FIELD::Name() const { return Glob_scope().String(_fld->Name()); }

uint64_t FIELD::Bit_size() const {
  if (Is_bit_fld()) {
    return Bit_fld_size();
  } else {
    return Type()->Bit_size();
  }
}

void FIELD::Print(std::ostream& os, uint32_t indent) const {
  FIELD_PTR   fld                = FIELD_PTR(*this);
  uint32_t    fld_id             = fld->Id().Value();
  TYPE_PTR    fld_type           = fld->Type();
  const char* fld_name           = "Undefined";
  const char* fld_type_name      = fld_type->Name()->Char_str();
  const char* fld_type_kind_name = fld_type->Type_kind_name();

  os << std::string(indent * INDENT_SPACE, ' ');
  os << std::hex << std::showbase;
  if (fld->Kind() == FIELD_KIND::REGULAR) {
    fld_name = fld->Name()->Char_str();
    os << "FLD[" << fld_id << "] \"" << fld_name << "\", TYP["
       << fld->Type_id().Value() << "](" << fld_type_kind_name << ",\""
       << fld_type_name << "\")";
  }
}

void FIELD::Print() const { Print(std::cout, 0); }

//=============================================================================
// class ARB member functions
//=============================================================================

ARB::ARB(GLOB_SCOPE* glob, ARB_DATA_PTR ptr) : _glob(glob), _arb(ptr) {
  AIR_ASSERT(glob);
}

ARB::ARB(const GLOB_SCOPE& glob, ARB_ID id)
    : _glob(const_cast<GLOB_SCOPE*>(&glob)), _arb(glob.Arb_table().Find(id)) {}

void ARB::Set_next(ARB_ID id) {
  _arb->Set_next(id.Value());
  if (id != Null_id) {
    _arb->Unset_last_dim();
    ARB_PTR next = Glob_scope().Arb(id);
    next->Unset_first_dim();
  } else {
    _arb->Set_last_dim();
  }
}

void ARB::Print(std::ostream& os, uint32_t indent) const {
  uint32_t id = Id().Value();

  os << std::string(indent * INDENT_SPACE, ' ');
  os << std::hex << std::showbase;
  os << "ARB[" << id << "] lower(";
  if (Is_lb_const()) {
    os << "const:" << Lb_val();
  } else {
    os << "var:" << Lb_var();
  }
  os << "), upper(";
  if (Is_ub_const()) {
    os << "const:" << Ub_val();
  } else {
    os << "var:" << Ub_var();
  }
  os << "), stride(";
  if (Is_stride_const()) {
    os << "const:" << Stride_val();
  } else {
    os << "var:" << Stride_var();
  }
  os << ")\n";
  os << std::string((indent + 1) * INDENT_SPACE, ' ');
  os << "dimension(" << Dim() << "), first(" << (Is_first_dim() ? "yes" : "no")
     << "), last(" << (Is_last_dim() ? "yes" : "no") << "), next(ARB["
     << Next().Value() << "])";
}

void ARB::Print() const { Print(std::cout, 0); }

//=============================================================================
// class TYPE member functions
//=============================================================================

TYPE::TYPE(const GLOB_SCOPE& glob, TYPE_ID tid)
    : _glob(const_cast<GLOB_SCOPE*>(&glob)),
      _type(glob.Type_table().Find(ID<TYPE_DATA>(tid.Index()))) {}

TYPE_ID
TYPE::Id() const {
  if (_type == TYPE_DATA_PTR()) return TYPE_ID(Null_st_id);
  return TYPE_ID(_type.Id().Value(), 0);
}

void TYPE::Set_name(STR_ID name) {
  _type->Set_name(name.Is_null() ? Glob_scope().Undefined_name_id() : name);
}

TYPE_PTR
TYPE::Base_type() const {
  if (!Is_subtype()) return TYPE_PTR(*this);
  return Cast_to<TYPE_TRAIT::SUBTYPE>()->Immediate_base_type()->Base_type();
}

STR_PTR
TYPE::Name() const { return Glob_scope().String(_type->Name()); }

uint64_t TYPE::Bit_size() const {
  AIR_ASSERT(!Is_null());
  switch (Kind()) {
    case TYPE_TRAIT::PRIMITIVE:
    case TYPE_TRAIT::POINTER: {
      return _type->Bit_size();
    }
    case TYPE_TRAIT::ARRAY: {
      return Cast_to_arr()->Bit_size();
    }
    case TYPE_TRAIT::SUBTYPE: {
      return Cast_to_sub()->Bit_size();
    }
    case TYPE_TRAIT::RECORD: {
      return Cast_to_rec()->Bit_size();
    }
    case TYPE_TRAIT::VA_LIST: {
      return Cast_to_va_list()->Bit_size();
    }
    case TYPE_TRAIT::SIGNATURE:
    case TYPE_TRAIT::UNKNOWN:
    default: {
      AIR_ASSERT(0);
      return 0;
    }
  }
}

uint64_t TYPE::Byte_size() const {
  AIR_ASSERT(!Is_null());
  return (Bit_size() >> 3);
}

DATA_ALIGN
TYPE::Alignment() const {
  AIR_ASSERT(!Is_null());
  switch (Kind()) {
    case TYPE_TRAIT::PRIMITIVE:
    case TYPE_TRAIT::POINTER: {
      return _type->Alignment();
    }
    case TYPE_TRAIT::ARRAY: {
      return Cast_to_arr()->Alignment();
    }
    case TYPE_TRAIT::SUBTYPE: {
      return Cast_to_sub()->Alignment();
    }
    case TYPE_TRAIT::RECORD: {
      return Cast_to_rec()->Alignment();
    }
    case TYPE_TRAIT::VA_LIST: {
      return Cast_to_va_list()->Alignment();
    }
    case TYPE_TRAIT::SIGNATURE:
    case TYPE_TRAIT::UNKNOWN:
    default: {
      AIR_ASSERT(0);
      return DATA_ALIGN::BAD;
    }
  }
}

bool TYPE::Is_unsigned_int() const {
  if (!Is_prim()) return false;
  return Cast_to<TYPE_TRAIT::PRIMITIVE>()->Is_unsigned_int();
}

bool TYPE::Is_signed_int() const {
  if (!Is_prim()) return false;
  return Cast_to<TYPE_TRAIT::PRIMITIVE>()->Is_signed_int();
}

bool TYPE::Is_int() const {
  if (!Is_prim()) return false;
  return Cast_to<TYPE_TRAIT::PRIMITIVE>()->Is_int();
}

bool TYPE::Is_real_float() const {
  if (!Is_prim()) return false;
  return Cast_to<TYPE_TRAIT::PRIMITIVE>()->Is_real_float();
}

bool TYPE::Is_complex_float() const {
  if (!Is_prim()) return false;
  return Cast_to<TYPE_TRAIT::PRIMITIVE>()->Is_complex_float();
}

bool TYPE::Is_float() const {
  if (!Is_prim()) return false;
  return Cast_to<TYPE_TRAIT::PRIMITIVE>()->Is_float();
}

bool TYPE::Is_scalar() const {
  return ((Is_prim() || Is_ptr() || Is_va_list()) && !Is_complex_float());
}

bool TYPE::Has_alignment() const {
  AIR_ASSERT(!Is_null());
  switch (Kind()) {
    case TYPE_TRAIT::PRIMITIVE: {
      return _type->Is_alignment_set();
    }
    case TYPE_TRAIT::ARRAY: {
      return Cast_to_arr()->Elem_type()->Has_alignment();
    }
    case TYPE_TRAIT::RECORD: {
      return Cast_to_rec()->Is_complete();
    }
    case TYPE_TRAIT::POINTER: {
      return true;
    }
    case TYPE_TRAIT::VA_LIST: {
      return true;
    }
    case TYPE_TRAIT::SUBTYPE: {
      return (_type->Is_alignment_set() || Base_type()->Has_alignment());
    }
    case TYPE_TRAIT::SIGNATURE:
    case TYPE_TRAIT::UNKNOWN:
    default: {
      return false;
    }
  }
}

bool TYPE::Has_size() const {
  AIR_ASSERT(!Is_null());
  switch (Kind()) {
    case TYPE_TRAIT::PRIMITIVE: {
      return _type->Is_bit_size_set();
    }
    case TYPE_TRAIT::ARRAY: {
      return Cast_to_arr()->Has_size();
    }
    case TYPE_TRAIT::RECORD: {
      return Cast_to_rec()->Is_complete();
    }
    case TYPE_TRAIT::POINTER: {
      return true;
    }
    case TYPE_TRAIT::VA_LIST: {
      return true;
    }
    case TYPE_TRAIT::SUBTYPE: {
      return (_type->Is_bit_size_set() || Base_type()->Has_size());
    }
    case TYPE_TRAIT::SIGNATURE:
    case TYPE_TRAIT::UNKNOWN:
    default: {
      return false;
    }
  }
}

void TYPE::Print(std::ostream& os, uint32_t indent) const {
  uint32_t    id        = Id().Value();
  const char* name      = Name()->Char_str();
  const char* kind_name = Type_kind_name_arr[static_cast<uint32_t>(Kind())];

  os << std::string(indent * INDENT_SPACE, ' ');
  os << std::hex << std::showbase;
  os << "TYP[" << id << "] " << kind_name << "(";

  if (Kind() == TYPE_TRAIT::PRIMITIVE) {
    os << name << ")";
  } else if (Kind() == TYPE_TRAIT::POINTER) {
    CONST_POINTER_TYPE_PTR me = Cast_to<TYPE_TRAIT::POINTER>();

    os << Pointer_kind_names[static_cast<uint32_t>(me->Ptr_kind())]
       << "), domain(TYP[" << me->Domain_type_id().Value() << "])";
  } else if (Kind() == TYPE_TRAIT::ARRAY) {
    CONST_ARRAY_TYPE_PTR& me = Cast_to<TYPE_TRAIT::ARRAY>();

    os << "\"" << name << "\"), element(TYP[" << me->Elem_type_id().Value()
       << "])";
    DIM_ITER dim_iter = me->Begin_dim();
    DIM_ITER end_iter = me->End_dim();
    for (; dim_iter != end_iter; ++dim_iter) {
      os << std::endl;
      (*dim_iter)->Print(os, indent + 1);
    }
  } else if (Kind() == TYPE_TRAIT::RECORD) {
    CONST_RECORD_TYPE_PTR& me = Cast_to<TYPE_TRAIT::RECORD>();

    os << "\"" << name << "\"), complete(" << (me->Is_complete() ? "yes" : "no")
       << ")";
    FIELD_ITER iter = me->Begin();
    FIELD_ITER end  = me->End();
    for (; iter != end; ++iter) {
      FIELD_PTR fld = *iter;
      os << std::endl;
      fld->Print(os, indent + 1);
    }
  } else if (Kind() == TYPE_TRAIT::SIGNATURE) {
    CONST_SIGNATURE_TYPE_PTR& me    = Cast_to<TYPE_TRAIT::SIGNATURE>();
    TYPE_PTR                  rtype = me->Dwarf_rtype();
    os << "\"" << name << "\"), dwarf_ret(TYP[" << rtype->Id().Value()
       << "]), complete(" << (me->Is_complete() ? "yes" : "no") << ")";

    PARAM_ITER iter = me->Begin_param();
    PARAM_ITER end  = me->End_param();
    for (; iter != end; ++iter) {
      PARAM_PTR ptr = *iter;
      os << std::endl;
      ptr->Print(os, indent + 1);
    }
  }
  os << std::endl;
}

void TYPE::Print() const { Print(std::cout, 0); }

std::string TYPE::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os);
  return buf.str();
}

//=============================================================================
// class PRIM_TYPE member functions
//=============================================================================

PRIM_TYPE::PRIM_TYPE(const GLOB_SCOPE& glob, TYPE_ID id) : TYPE(glob, id) {
  Requires<CHECK_SIZE_EQUAL<PRIM_TYPE, TYPE> >();
  AIR_ASSERT(Kind() == TYPE_TRAIT::PRIMITIVE);
}

PRIMITIVE_TYPE
PRIM_TYPE::Encoding() const {
  return _type->Cast_type_data<TYPE_TRAIT::PRIMITIVE>().Prim_type_enc();
}

void PRIM_TYPE::Set_encoding(PRIMITIVE_TYPE enc) {
  _type->Cast_type_data<TYPE_TRAIT::PRIMITIVE>().Set_prim_type_enc(enc);
}

//=============================================================================
// class POINTER_TYPE member functions
//=============================================================================

POINTER_TYPE::POINTER_TYPE(const GLOB_SCOPE& glob, TYPE_ID id)
    : TYPE(glob, id) {
  Requires<CHECK_SIZE_EQUAL<POINTER_TYPE, TYPE> >();
  AIR_ASSERT(Kind() == TYPE_TRAIT::POINTER);
}

TYPE_ID
POINTER_TYPE::Domain_type_id() const {
  return _type->Cast_type_data<TYPE_TRAIT::POINTER>().Domain();
}

TYPE_PTR
POINTER_TYPE::Domain_type() const {
  return Glob_scope().Type(Domain_type_id());
}

POINTER_KIND
POINTER_TYPE::Ptr_kind() const {
  return _type->Cast_type_data<TYPE_TRAIT::POINTER>().Ptr_kind();
}

void POINTER_TYPE::Set_domain_type(TYPE_ID id) {
  _type->Cast_type_data<TYPE_TRAIT::POINTER>().Set_domain(id);
}

void POINTER_TYPE::Set_ptr_kind(POINTER_KIND kind) {
  _type->Cast_type_data<TYPE_TRAIT::POINTER>().Set_ptr_kind(kind);
}

//=============================================================================
// class ARRAY_TYPE member functions
//=============================================================================

ARRAY_TYPE::ARRAY_TYPE(const GLOB_SCOPE& glob, TYPE_ID id) : TYPE(glob, id) {
  Requires<CHECK_SIZE_EQUAL<ARRAY_TYPE, TYPE> >();
  AIR_ASSERT(Kind() == TYPE_TRAIT::ARRAY);
}

void ARRAY_TYPE::Set_elem_type(TYPE_ID idx) {
  _type->Cast_type_data<TYPE_TRAIT::ARRAY>().Set_elem_type(idx);
}

TYPE_ID
ARRAY_TYPE::Elem_type_id() const {
  return _type->Cast_type_data<TYPE_TRAIT::ARRAY>().Elem_type();
}

TYPE_PTR
ARRAY_TYPE::Elem_type() const { return Glob_scope().Type(Elem_type_id()); }

uint64_t ARRAY_TYPE::Elem_count() const {
  uint64_t count = 1;
  for (DIM_ITER it = Begin_dim(); it != End_dim(); ++it) {
    AIR_ASSERT((*it)->Is_lb_const());
    AIR_ASSERT((*it)->Is_ub_const());
    AIR_ASSERT((*it)->Is_stride_const());
    count *= ((*it)->Ub_val() - (*it)->Lb_val()) / (*it)->Stride_val();
  }
  if (Begin_dim() == End_dim()) {
    count = 0;
  }
  return count;
}

std::vector<int64_t> ARRAY_TYPE::Shape() const {
  std::vector<int64_t> res;
  for (DIM_ITER it = Begin_dim(); it != End_dim(); ++it) {
    AIR_ASSERT((*it)->Is_lb_const());
    AIR_ASSERT((*it)->Is_ub_const());
    AIR_ASSERT((*it)->Is_stride_const());
    res.push_back(((*it)->Ub_val() - (*it)->Lb_val()) / (*it)->Stride_val());
  }
  return res;
}

ARB_ID ARRAY_TYPE::First_dim_id() const {
  return _type->Cast_type_data<TYPE_TRAIT::ARRAY>().First_dim();
}

ARB_PTR ARRAY_TYPE::First_dim() const {
  return Glob_scope().Arb(First_dim_id());
}

uint32_t ARRAY_TYPE::Dim() const {
  uint32_t count = 0;
  for (DIM_ITER it = Begin_dim(); it != End_dim(); ++it) {
    count++;
  }
  return count;
}

DIM_ITER ARRAY_TYPE::Begin_dim() const { return DIM_ITER(*this); }

DIM_ITER ARRAY_TYPE::End_dim() const { return DIM_ITER(); }

DATA_ALIGN ARRAY_TYPE::Alignment() const {
  AIR_ASSERT(!Is_null());
  if (!Is_analyzed()) const_cast<ARRAY_TYPE&>(*this).Analyze();
  return _type->Alignment();
}

uint64_t ARRAY_TYPE::Bit_size() const {
  AIR_ASSERT(!Is_null() && Has_size());
  if (!Is_analyzed()) const_cast<ARRAY_TYPE&>(*this).Analyze();
  return _type->Bit_size();
}

bool ARRAY_TYPE::Has_size() const {
  AIR_ASSERT(!Is_null());
  if (Is_analyzed()) return _type->Is_bit_size_set();
  for (DIM_ITER it = Begin_dim(); it != End_dim(); ++it) {
    ARB_PTR arb = *it;
    if (arb->Is_lb_const() && arb->Is_ub_const() && arb->Is_stride_const())
      continue;
    return false;
  }
  return true;
}

bool ARRAY_TYPE::Is_analyzed() const {
  return _type->Cast_type_data<TYPE_TRAIT::ARRAY>().Is_analyzed();
}

void ARRAY_TYPE::Set_first_dim(ARB_ID id) {
  _type->Cast_type_data<TYPE_TRAIT::ARRAY>().Set_first_dim(id);
}

void ARRAY_TYPE::Analyze() {
  AIR_ASSERT(!Is_null());
  if (Is_analyzed()) return;
  if (Has_size()) {
    uint64_t sz = Elem_type()->Bit_size();
    for (DIM_ITER it = Begin_dim(); it != End_dim(); ++it) {
      sz *= ((*it)->Ub_val() - (*it)->Lb_val()) * (*it)->Stride_val();
    }
    if (Begin_dim() == End_dim()) {
      sz = 0;
    }
    _type->Set_bit_size(sz);
  }
  if (Elem_type()->Has_alignment()) {
    _type->Set_alignment(Elem_type()->Alignment());
  }
  _type->Cast_type_data<TYPE_TRAIT::ARRAY>().Set_analyzed();
}

//=============================================================================
// class RECORD_TYPE member functions
//=============================================================================

RECORD_TYPE::RECORD_TYPE(const GLOB_SCOPE& glob, TYPE_ID id) : TYPE(glob, id) {
  Requires<CHECK_SIZE_EQUAL<RECORD_TYPE, TYPE> >();
  AIR_ASSERT(Kind() == TYPE_TRAIT::RECORD);
}

RECORD_KIND
RECORD_TYPE::Rec_kind() const {
  return _type->Cast_type_data<TYPE_TRAIT::RECORD>().Kind();
}

uint32_t RECORD_TYPE::Num_fld() const {
  return _type->Cast_type_data<TYPE_TRAIT::RECORD>().Num_of_fld();
}

FIELD_ITER
RECORD_TYPE::Begin() const { return FIELD_ITER(*this); }

FIELD_ITER
RECORD_TYPE::End() const { return FIELD_ITER(); }

FIELD_ID
RECORD_TYPE::First_fld_id() const {
  return _type->Cast_type_data<TYPE_TRAIT::RECORD>().First_fld_id();
}

FIELD_PTR
RECORD_TYPE::First_fld() const { return Glob_scope().Field(First_fld_id()); }

FIELD_ID
RECORD_TYPE::Last_fld_id() const {
  return _type->Cast_type_data<TYPE_TRAIT::RECORD>().Last_fld_id();
}

FIELD_PTR
RECORD_TYPE::Last_fld() const { return Glob_scope().Field(Last_fld_id()); }

DATA_ALIGN
RECORD_TYPE::Alignment() const {
  AIR_ASSERT(Has_alignment());
  if (!Is_analyzed()) const_cast<RECORD_TYPE&>(*this).Analyze();
  return _type->Alignment();
}

uint64_t RECORD_TYPE::Bit_size() const {
  AIR_ASSERT(Has_size());
  if (!Is_analyzed()) const_cast<RECORD_TYPE&>(*this).Analyze();
  return _type->Bit_size();
}

bool RECORD_TYPE::Is_analyzed() const {
  return _type->Cast_type_data<TYPE_TRAIT::RECORD>().Is_analyzed();
}

bool RECORD_TYPE::Is_complete() const {
  return _type->Cast_type_data<TYPE_TRAIT::RECORD>().Is_complete();
}

bool RECORD_TYPE::Is_empty() const {
  return _type->Cast_type_data<TYPE_TRAIT::RECORD>().Is_empty();
}

bool RECORD_TYPE::Is_simple() const {
  return _type->Cast_type_data<TYPE_TRAIT::RECORD>().Is_simple();
}

bool RECORD_TYPE::Is_builtin() const {
  return _type->Cast_type_data<TYPE_TRAIT::RECORD>().Is_builtin();
}

void RECORD_TYPE::Analyze() {
  if (!Is_complete() || Is_analyzed()) return;
  RECORD_TYPE_DATA& rec_data = _type->Cast_type_data<TYPE_TRAIT::RECORD>();
  rec_data.Set_analyzed();
  if (Is_empty()) {
    rec_data.Set_alignment(DATA_ALIGN::BIT8);
    rec_data.Set_simple(false);
    return;
  }
  bool       is_simple        = true;
  bool       is_prev_bit_fld  = false;
  DATA_ALIGN max_fld_align    = DATA_ALIGN::BIT8;
  uint64_t   max_fld_end_ofst = 0;
  FIELD_ITER fld_end          = End();
  for (FIELD_ITER fld_iter = Begin(); fld_iter != fld_end; ++fld_iter) {
    FIELD_PTR  fld   = *fld_iter;
    FIELD_KIND fkind = fld->Kind();
    TYPE_PTR   ftype = fld->Type();
    switch (fkind) {
      case FIELD_KIND::REGULAR: {
        // update alignment according to field
        DATA_ALIGN fld_align = fld->Alignment();
        uint64_t   align_bit = 1UL << static_cast<uint32_t>(fld_align);
        if ((fld_align > max_fld_align) && (fld_align != DATA_ALIGN::BAD)) {
          max_fld_align = fld_align;
        }
        // alignment current field within the struct
        bool is_aligned = false;
        if (((max_fld_end_ofst % align_bit) == 0) ||
            (is_prev_bit_fld && fld->Is_bit_fld())) {
          is_aligned = true;
        }
        if ((!is_aligned) && (max_fld_end_ofst != 0)) {
          max_fld_end_ofst = ((max_fld_end_ofst / align_bit) + 1) * align_bit;
        }
        // set field offset and update record bit size
        if (Rec_kind() != RECORD_KIND::UNION) {
          fld->Set_ofst(max_fld_end_ofst);
          max_fld_end_ofst += fld->Bit_size();
        } else {
          // offset of a union field should be 0 always
          fld->Set_ofst(0);
          if (fld->Bit_size() > max_fld_end_ofst) {
            max_fld_end_ofst = fld->Bit_size();
          }
        }
        break;
      }
      default: {
        // may deal with other FIELD_KIND
        AIR_ASSERT(0);
        break;
      }
    }
    is_prev_bit_fld = fld->Is_bit_fld();
  }
  uint64_t max_align_bit = 1UL << static_cast<uint32_t>(max_fld_align);
  if (max_fld_end_ofst % max_align_bit != 0) {
    max_fld_end_ofst = ((max_fld_end_ofst / max_align_bit) + 1) * max_align_bit;
  }
  rec_data.Set_simple(is_simple);
  if (!rec_data.Is_alignment_set()) rec_data.Set_alignment(max_fld_align);
  if (!rec_data.Is_bit_size_set()) rec_data.Set_bit_size(max_fld_end_ofst);
}

void RECORD_TYPE::Add_fld(FIELD_ID fld) {
  RECORD_TYPE_DATA& rtype_data = _type->Cast_type_data<TYPE_TRAIT::RECORD>();
  rtype_data.Add_fld();
  FIELD_ID first_fld = First_fld_id();
  FIELD_ID last_fld  = Last_fld_id();
  if (first_fld.Is_null()) {
    Set_first_fld_id(fld);
    Set_last_fld_id(fld);
  } else {
    Glob_scope().Field(last_fld)->Set_next(fld);
    Set_last_fld_id(fld);
  }
}

void RECORD_TYPE::Set_complete() {
  _type->Cast_type_data<TYPE_TRAIT::RECORD>().Set_complete();
}

void RECORD_TYPE::Set_last_fld_id(FIELD_ID id) {
  _type->Cast_type_data<TYPE_TRAIT::RECORD>().Set_last_fld_id(id);
}

void RECORD_TYPE::Set_first_fld_id(FIELD_ID id) {
  _type->Cast_type_data<TYPE_TRAIT::RECORD>().Set_first_fld_id(id);
}

void RECORD_TYPE::Set_kind(RECORD_KIND kind) {
  _type->Cast_type_data<TYPE_TRAIT::RECORD>().Set_kind(kind);
}

//=============================================================================
// class SIGNATURE_TYPE member functions
//=============================================================================

SIGNATURE_TYPE::SIGNATURE_TYPE(const GLOB_SCOPE& glob, TYPE_ID id)
    : TYPE(glob, id) {
  Requires<CHECK_SIZE_EQUAL<SIGNATURE_TYPE, TYPE> >();
  AIR_ASSERT(Kind() == TYPE_TRAIT::SIGNATURE);
}

void SIGNATURE_TYPE::Add_param(PARAM_ID id) {
  PARAM_ID first = First_param_id();
  PARAM_ID last  = Last_param_id();
  if (first.Is_null()) {
    Set_first_param_id(id);
    Set_last_param_id(id);
  } else {
    Glob_scope().Param(last)->Set_next(id);
    Set_last_param_id(id);
  }
}

uint32_t SIGNATURE_TYPE::Num_param() const {
  uint32_t   count = 0;
  PARAM_ITER iter  = Begin_param();
  PARAM_ITER end   = End_param();
  for (; iter != end; ++iter) {
    if ((*iter)->Is_ret()) continue;
    count++;
  }
  return count;
}

PARAM_ITER
SIGNATURE_TYPE::Begin_param() const { return PARAM_ITER(*this); }

PARAM_ITER
SIGNATURE_TYPE::End_param() const { return PARAM_ITER(); }

void SIGNATURE_TYPE::Set_complete() {
  _type->Cast_type_data<TYPE_TRAIT::SIGNATURE>().Set_sig_complete();
}

PARAM_ID
SIGNATURE_TYPE::First_param_id() const {
  return _type->Cast_type_data<TYPE_TRAIT::SIGNATURE>().First_param_id();
}

void SIGNATURE_TYPE::Set_first_param_id(PARAM_ID id) {
  _type->Cast_type_data<TYPE_TRAIT::SIGNATURE>().Set_first_param_id(id);
}

PARAM_ID
SIGNATURE_TYPE::Last_param_id() const {
  return _type->Cast_type_data<TYPE_TRAIT::SIGNATURE>().Last_param_id();
}

void SIGNATURE_TYPE::Set_last_param_id(PARAM_ID id) {
  _type->Cast_type_data<TYPE_TRAIT::SIGNATURE>().Set_last_param_id(id);
}

TYPE_PTR
SIGNATURE_TYPE::Dwarf_rtype() const {
  TYPE_ID id = _type->Cast_type_data<TYPE_TRAIT::SIGNATURE>().Dwarf_type();
  if (id != TYPE_ID(Null_st_id)) {
    return Glob_scope().Type(id);
  } else {
    return Null_ptr;
  }
}

void SIGNATURE_TYPE::Set_dwarf_rtype(TYPE_PTR type) {
  _type->Cast_type_data<TYPE_TRAIT::SIGNATURE>().Set_dwarf_rtype(type->Id());
}

bool SIGNATURE_TYPE::Is_complete() const {
  return _type->Cast_type_data<TYPE_TRAIT::SIGNATURE>().Is_complete();
}

//=============================================================================
// class SUBTYPE member functions
//=============================================================================

SUBTYPE::SUBTYPE(const GLOB_SCOPE& glob, TYPE_ID id) : TYPE(glob, id) {
  Requires<CHECK_SIZE_EQUAL<SUBTYPE, TYPE> >();
  AIR_ASSERT(Kind() == TYPE_TRAIT::SUBTYPE);
}

SUBTYPE_KIND
SUBTYPE::Subtype_kind() const {
  return _type->Cast_type_data<TYPE_TRAIT::SUBTYPE>().Subtype_kind();
}

TYPE_ID
SUBTYPE::Immediate_base_type_id() const {
  return _type->Cast_type_data<TYPE_TRAIT::SUBTYPE>().Immediate_base_type();
}

TYPE_PTR
SUBTYPE::Immediate_base_type() const {
  return Glob_scope().Type(Immediate_base_type_id());
}

DATA_ALIGN
SUBTYPE::Alignment() const {
  if (!_type->Is_alignment_set()) {
    _type->Set_alignment(Base_type()->Alignment());
  }
  return _type->Alignment();
}

uint64_t SUBTYPE::Bit_size() const {
  if (!_type->Is_bit_size_set()) {
    _type->Set_bit_size(Base_type()->Bit_size());
  }
  return _type->Bit_size();
}

//=============================================================================
// class VA_LIST_TYPE member functions
//=============================================================================

DATA_ALIGN
VA_LIST_TYPE::Alignment() const {
  // TODO
  AIR_ASSERT(0);
  return DATA_ALIGN::BAD;
}

uint64_t VA_LIST_TYPE::Bit_size() const {
  // TODO
  AIR_ASSERT(0);
  return 0;
}

//=============================================================================
// class PARAM member functions
//=============================================================================

PARAM::PARAM(const GLOB_SCOPE& glob, PARAM_ID id) : _glob(0), _param() {
  AIR_ASSERT(id.Scope() == 0);
  if (id.Is_null()) return;
  _glob  = const_cast<GLOB_SCOPE*>(&glob);
  _param = glob.Param_table().Find(ID<PARAM_DATA>(id.Index()));
}

void PARAM::Set_name(STR_ID name) {
  _param->Set_name(name.Is_null() ? Glob_scope().Undefined_name_id() : name);
}

PARAM_ID
PARAM::Id() const {
  if (_param == PARAM_DATA_PTR()) return PARAM_ID(Null_st_id);
  return PARAM_ID(_param.Id().Value(), 0);
}

STR_PTR
PARAM::Name() const { return Glob_scope().String(_param->Name()); }

TYPE_PTR
PARAM::Type() const { return Glob_scope().Type(_param->Type()); }

void PARAM::Set_owning_sig(TYPE_ID sig) {
  _param->Set_signature(sig);
  TYPE_PTR            ptr      = Glob_scope().Type(sig);
  SIGNATURE_TYPE_PTR& sig_type = ptr->Cast_to_sig();
  sig_type->Add_param(Id());
  if (!Is_ret()) sig_type->Set_complete();
}

void PARAM::Print(std::ostream& os, uint32_t indent) const {
  PARAM_PTR   ptr            = PARAM_PTR(*this);
  uint32_t    type_id        = Null_st_id;
  const char* name           = "_noname";
  const char* type_name      = "_noname";
  const char* type_kind_name = "_notype";
  const char* attr           = ptr->Is_ellips() ? "ellipsis"
                               : ptr->Is_ret()  ? "return"
                               : ptr->Is_this() ? "this"
                                                : "regular";
  if (!ptr->Is_ellips()) {
    TYPE_PTR type  = ptr->Type();
    type_id        = type->Id().Value();
    type_name      = type->Name()->Char_str();
    type_kind_name = type->Type_kind_name();

    if (!ptr->Is_ret()) {
      name = ptr->Name()->Char_str();
    }
  }

  os << std::string(indent * INDENT_SPACE, ' ');
  os << std::hex << std::showbase;
  os << "PAR[" << ptr->Id().Value() << "] " << attr << ", \"" << name
     << "\", TYP[" << type_id << "](" << type_kind_name << ", \"" << type_name
     << "\")";
}

void PARAM::Print() const { Print(std::cout, 0); }

}  // namespace base
}  // namespace air
