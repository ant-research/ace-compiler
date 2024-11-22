//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_TYPE_H
#define AIR_BASE_TYPE_H

#include "air/base/st_data.h"
#include "air/base/st_decl.h"
#include "air/base/st_trait.h"

namespace air {
namespace base {

class FIELD {
  friend class GLOB_SCOPE;
  friend class RECORD_TYPE;
  friend class FIELD_ITER;
  PTR_FRIENDS(FIELD);

public:
  FIELD() : _glob(0), _fld() {}
  FIELD(const GLOB_SCOPE& glob, FIELD_ID id);

  GLOB_SCOPE& Glob_scope() const { return *_glob; }
  FIELD_KIND  Kind() const { return _fld->Kind(); }
  FIELD_ID    Id() const;

  bool Is_regular() const { return Kind() == FIELD_KIND::REGULAR; }
  bool Is_static() const { return Kind() == FIELD_KIND::STATIC; }
  bool Is_vtb_ptr() const { return Kind() == FIELD_KIND::VTABLE_PTR; }
  bool Is_bit_fld() const { return _fld->Is_bit_fld(); }

  TYPE_ID         Type_id() const { return _fld->Type(); }
  TYPE_PTR        Type() const;
  TYPE_ID         Owning_rec_type_id() const { return _fld->Owning_rec_type(); }
  RECORD_TYPE_PTR Owning_rec_type() const;
  SPOS            Spos() const { return _fld->Spos(); }
  STR_PTR         Name() const;
  uint64_t        Ofst() const { return _fld->Ofst(); }
  uint64_t        Byte_ofst() const { return _fld->Byte_ofst(); }
  uint64_t        Bit_ofst() const { return _fld->Bit_ofst(); }
  uint64_t        Bit_size() const;
  uint32_t        Bit_fld_size() const { return _fld->Bit_fld_size(); }
  int64_t         Vtb_ofst() const { return _fld->Vtb_ofst(); }
  DATA_ALIGN      Alignment() const { return _fld->Alignment(); }

  void Set_type(TYPE_ID id) { _fld->Set_type(id); }
  void Set_owning_rec_type(TYPE_ID id) { _fld->Set_owning_rec_type(id); }
  void Set_ofst(uint64_t bit) { _fld->Set_ofst(bit); }
  void Set_bit_fld() { _fld->Set_bit_fld(); }
  void Set_bit_fld_size(uint32_t sz) { _fld->Set_bit_fld_size(sz); }
  void Set_spos(const SPOS& s) { _fld->Set_spos(s); }
  void Set_name(STR_ID name) { _fld->Set_name(name); }
  void Set_alignment(DATA_ALIGN align) { _fld->Set_alignment(align); }

  void Print(std::ostream& os, uint32_t indent = 0) const;
  void Print() const;

private:
  FIELD(const GLOB_SCOPE& glob, FIELD_DATA_PTR ptr)
      : _glob(const_cast<GLOB_SCOPE*>(&glob)), _fld(ptr) {}

  bool Is_null() const { return _fld.Is_null(); }
  void Set_next(FIELD_ID id) { _fld->Set_next(id); }

  FIELD_DATA_PTR Data() const { return _fld; }
  FIELD_ID       Next() const { return _fld->Next(); }

  GLOB_SCOPE*    _glob;
  FIELD_DATA_PTR _fld;
};

//! Wrapper class to access ARB_DATA
class ARB {
  PTR_FRIENDS(ARB);

public:
  ARB() : _glob(0), _arb() {}
  ARB(GLOB_SCOPE* glob, ARB_DATA_PTR ptr);
  ARB(const ARB& o) : _glob(o._glob), _arb(o._arb) {}
  ARB(const GLOB_SCOPE& glob, ARB_ID id);

  GLOB_SCOPE& Glob_scope() const { return *_glob; }
  ARB_ID      Id() const { return _arb.Id(); }

  //! Get constant value of lower bound
  int64_t Lb_val() const { return _arb->Lb_val(); }
  //! Get variable index of lower bound
  uint32_t Lb_var() const { return _arb->Lb_var(); }
  //! Get constant value of upper bound
  int64_t Ub_val() const { return _arb->Ub_val(); }
  //! Get variable index of upper bound
  uint32_t Ub_var() const { return _arb->Ub_var(); }
  //! Get constant value of stride
  int64_t Stride_val() const { return _arb->Stride_val(); }
  //! Get variable index of stride
  uint32_t Stride_var() const { return _arb->Stride_var(); }
  //! Get dimensions
  uint32_t Dim() const { return _arb->Dim(); }
  //! Get index of next arb entry
  ARB_ID Next() const { return ARB_ID(_arb->Next()); }

  bool Is_lb_const() const { return _arb->Is_lb_const(); }
  bool Is_ub_const() const { return _arb->Is_ub_const(); }
  bool Is_stride_const() const { return _arb->Is_stride_const(); }
  bool Is_first_dim() const { return _arb->Is_first_dim(); }
  bool Is_last_dim() const { return _arb->Is_last_dim(); }
  bool Is_lb_unknown() const { return _arb->Is_lb_unknown(); }
  bool Is_ub_unknown() const { return _arb->Is_ub_unknown(); }
  bool Is_stride_unknown() const { return _arb->Is_stride_unknown(); }
  bool Is_null() const { return _arb.Is_null(); }

  //! Set flag indicating lower bound is a constant
  void Set_lb_const() { _arb->Set_lb_const(); }
  //! Unset flag indicating lower bound is a constant
  void Unset_lb_const() { _arb->Unset_lb_const(); }
  //! @brief Set lower bound constant value,
  //! the flag indicating lower bound is a constant is also set
  //! @param v const value of lower bound
  void Set_lb_val(int64_t v) { _arb->Set_lb_val(v); }
  //! @brief Set lower bound variable index,
  //! the flag indicating lower bound is a constant is also unset
  //! @param v variable index of lower bound
  void Set_lb_var(uint32_t v) { _arb->Set_lb_var(v); }
  //! Set flag indicating upper bound is a constant
  void Set_ub_const() { _arb->Set_ub_const(); }
  //! Unset flag indicating upper bound is a constant
  void Unset_ub_const() { _arb->Unset_ub_const(); }
  //! @brief Set upper bound constant value,
  //! the flag indicating upper bound is a constant is also set
  //! @param v const value of upper bound
  void Set_ub_val(int64_t v) { _arb->Set_ub_val(v); }
  //! @brief Set upper bound variable index,
  //! the flag indicating upper bound is a constant is also unset
  //! @param v variable index of upper bound
  void Set_ub_var(uint32_t v) { _arb->Set_ub_var(v); }
  //! Set flag indicating stride is a constant
  void Set_stride_const() { _arb->Set_stride_const(); }
  //! Unset flag indicating stride is a constant
  void Unset_stride_const() { _arb->Unset_stride_const(); }
  //! @brief Set stride constant value,
  //! the flag indicating stride is a constant is also set
  //! @param v const value of stride
  void Set_stride_val(int64_t v) { _arb->Set_stride_val(v); }
  //! @brief Set stride variable index,
  //! the flag indicating stride is a constant is also unset
  //! @param v variable index of stride
  void Set_stride_var(uint32_t v) { _arb->Set_stride_var(v); }
  //! Set flag indicating this is the first dimension
  void Set_first_dim() { _arb->Set_first_dim(); }
  //! Unset flag indicating this is the first dimension
  void Unset_first_dim() { _arb->Unset_first_dim(); }
  //! Set flag indicating this is the last dimension
  void Set_last_dim() { _arb->Set_last_dim(); }
  //! Unset flag indicating this is the last dimension
  void Unset_last_dim() { _arb->Unset_last_dim(); }
  //! @brief Set dimensions
  //! @param d dimension value
  void Set_dim(uint32_t d) { _arb->Set_dim(d); }
  //! @brief Set next arb entry
  //! @param id index of next arb entry
  void Set_next(ARB_ID id);

  void Print(std::ostream& os, uint32_t indent = 0) const;
  void Print() const;

private:
  ARB_DATA_PTR Data() const { return _arb; }

  GLOB_SCOPE*  _glob;
  ARB_DATA_PTR _arb;
};

class TYPE {
  friend class GLOB_SCOPE;
  friend class TYPE_ITER;
  PTR_FRIENDS(TYPE);

public:
  TYPE() : _glob(0), _type() {}
  TYPE(const GLOB_SCOPE& glob, TYPE_ID tid);

  GLOB_SCOPE& Glob_scope() const { return *_glob; }
  TYPE_ID     Id() const;
  SPOS        Spos() const { return _type->Spos(); }
  TYPE_TRAIT  Kind() const { return _type->Kind(); }
  STR_PTR     Name() const;
  TYPE_PTR    Base_type() const;
  TYPE_ID     Base_type_id() const { return Base_type()->Id(); }
  DATA_ALIGN  Alignment() const;
  uint64_t    Bit_size() const;
  uint64_t    Byte_size() const;
  uint32_t    Domain_tag() const { return _type->Domain_tag(); }

  void Set_name(CONST_STR_PTR name) { Set_name(name->Id()); }
  void Set_name(STR_ID name);
  void Set_domain_tag(uint32_t t) { _type->Set_domain_tag(t); }

  const char* Type_kind_name() const {
    return Type_kind_name_arr[static_cast<uint32_t>(Kind())];
  }

  bool Is_prim() const { return Kind() == TYPE_TRAIT::PRIMITIVE; }
  bool Is_va_list() const { return Kind() == TYPE_TRAIT::VA_LIST; }
  bool Is_ptr() const { return Kind() == TYPE_TRAIT::POINTER; }
  bool Is_array() const { return Kind() == TYPE_TRAIT::ARRAY; }
  bool Is_record() const { return Kind() == TYPE_TRAIT::RECORD; }
  bool Is_signature() const { return Kind() == TYPE_TRAIT::SIGNATURE; }
  bool Is_subtype() const { return Kind() == TYPE_TRAIT::SUBTYPE; }
  bool Is_unsigned_int() const;
  bool Is_signed_int() const;
  bool Is_int() const;
  bool Is_real_float() const;
  bool Is_complex_float() const;
  bool Is_float() const;
  bool Is_void() const;
  bool Is_domain_type(uint32_t tag) { return (Domain_tag() == tag); }

  bool Is_scalar() const;
  bool Is_aggregate() const;
  bool Is_local() const;
  bool Is_same_type(CONST_TYPE_PTR o);
  bool Has_alignment() const;
  bool Has_size() const;

  template <TYPE_TRAIT T>
  PTR_TO_CONST<typename TYPE_DATA_TRAITS<T>::TYPE_TYPE>& Cast_to() const {
    typedef typename TYPE_DATA_TRAITS<T>::TYPE_TYPE TARGET_TYPE;
    typedef PTR_TO_CONST<TARGET_TYPE>               TARGET_PTR_TYPE;

    return reinterpret_cast<TARGET_PTR_TYPE&>(const_cast<TYPE&>(*this));
  }

  template <TYPE_TRAIT T>
  PTR<typename TYPE_DATA_TRAITS<T>::TYPE_TYPE>& Cast_to() {
    typedef typename TYPE_DATA_TRAITS<T>::TYPE_TYPE TARGET_TYPE;
    typedef PTR<TARGET_TYPE>                        TARGET_PTR_TYPE;

    return reinterpret_cast<TARGET_PTR_TYPE&>(*this);
  }

  PRIM_TYPE_PTR&    Cast_to_prim() { return Cast_to<TYPE_TRAIT::PRIMITIVE>(); }
  POINTER_TYPE_PTR& Cast_to_ptr() { return Cast_to<TYPE_TRAIT::POINTER>(); }
  ARRAY_TYPE_PTR&   Cast_to_arr() { return Cast_to<TYPE_TRAIT::ARRAY>(); };
  RECORD_TYPE_PTR&  Cast_to_rec() { return Cast_to<TYPE_TRAIT::RECORD>(); }
  SIGNATURE_TYPE_PTR& Cast_to_sig() { return Cast_to<TYPE_TRAIT::SIGNATURE>(); }
  SUBTYPE_PTR&        Cast_to_sub() { return Cast_to<TYPE_TRAIT::SUBTYPE>(); }
  VA_LIST_TYPE_PTR& Cast_to_va_list() { return Cast_to<TYPE_TRAIT::VA_LIST>(); }

  CONST_PRIM_TYPE_PTR& Cast_to_prim() const {
    return Cast_to<TYPE_TRAIT::PRIMITIVE>();
  }
  CONST_POINTER_TYPE_PTR& Cast_to_ptr() const {
    return Cast_to<TYPE_TRAIT::POINTER>();
  }
  CONST_ARRAY_TYPE_PTR& Cast_to_arr() const {
    return Cast_to<TYPE_TRAIT::ARRAY>();
  }
  CONST_RECORD_TYPE_PTR& Cast_to_rec() const {
    return Cast_to<TYPE_TRAIT::RECORD>();
  }
  CONST_SIGNATURE_TYPE_PTR& Cast_to_sig() const {
    return Cast_to<TYPE_TRAIT::SIGNATURE>();
  }
  CONST_SUBTYPE_PTR& Cast_to_sub() const {
    return Cast_to<TYPE_TRAIT::SUBTYPE>();
  }
  CONST_VA_LIST_TYPE_PTR& Cast_to_va_list() const {
    return Cast_to<TYPE_TRAIT::VA_LIST>();
  }

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

protected:
  TYPE(GLOB_SCOPE* glob, TYPE_DATA_PTR type) : _glob(glob), _type(type) {}

  GLOB_SCOPE*   Glob_scope_ptr() const { return _glob; }
  TYPE_DATA_PTR Data() const { return _type; }

  bool Is_null() const { return Id().Is_null(); }
  void Set_spos(const SPOS& s) { _type->Set_spos(s); }

  static const char* Type_kind_name_arr[static_cast<uint32_t>(TYPE_TRAIT::END)];

  GLOB_SCOPE*   _glob;
  TYPE_DATA_PTR _type;
};

class PRIM_TYPE : public TYPE {
  friend class GLOB_SCOPE;

public:
  PRIM_TYPE(const GLOB_SCOPE& glob, TYPE_ID id);

  PRIMITIVE_TYPE Encoding() const;
  CONSTANT_KIND  Const_kind() const;

  bool Is_unsigned_int() const {
    PRIMITIVE_TYPE enc = Encoding();
    return (
        (enc == PRIMITIVE_TYPE::INT_U8) || (enc == PRIMITIVE_TYPE::INT_U16) ||
        (enc == PRIMITIVE_TYPE::INT_U32) || (enc == PRIMITIVE_TYPE::INT_U64));
  }

  bool Is_signed_int() const {
    PRIMITIVE_TYPE enc = Encoding();
    return (
        (enc == PRIMITIVE_TYPE::INT_S8) || (enc == PRIMITIVE_TYPE::INT_S16) ||
        (enc == PRIMITIVE_TYPE::INT_S32) || (enc == PRIMITIVE_TYPE::INT_S64));
  }

  bool Is_int() const { return (Is_unsigned_int() || Is_signed_int()); }

  bool Is_real_float() const {
    PRIMITIVE_TYPE enc = Encoding();
    return ((enc == PRIMITIVE_TYPE::FLOAT_32) ||
            (enc == PRIMITIVE_TYPE::FLOAT_64) ||
            (enc == PRIMITIVE_TYPE::FLOAT_80) ||
            (enc == PRIMITIVE_TYPE::FLOAT_128));
  }

  bool Is_complex_float() const {
    PRIMITIVE_TYPE enc = Encoding();
    return ((enc == PRIMITIVE_TYPE::COMPLEX_32) ||
            (enc == PRIMITIVE_TYPE::COMPLEX_64) ||
            (enc == PRIMITIVE_TYPE::COMPLEX_80) ||
            (enc == PRIMITIVE_TYPE::COMPLEX_128));
  }

  bool Is_float() const { return (Is_real_float() || Is_complex_float()); }

  bool Is_void() const { return (Encoding() == PRIMITIVE_TYPE::VOID); }

private:
  void Set_encoding(PRIMITIVE_TYPE enc);
};

class POINTER_TYPE : public TYPE {
  friend class GLOB_SCOPE;

public:
  POINTER_TYPE() : TYPE() {}
  POINTER_TYPE(const GLOB_SCOPE& glob, TYPE_ID id);

  TYPE_ID      Domain_type_id() const;
  TYPE_PTR     Domain_type() const;
  POINTER_KIND Ptr_kind() const;

  void Set_domain_type(TYPE_ID id);

private:
  void Set_ptr_kind(POINTER_KIND kind);
};

class ARRAY_TYPE : public TYPE {
  friend class GLOB_SCOPE;

public:
  ARRAY_TYPE() : TYPE() {}
  ARRAY_TYPE(const GLOB_SCOPE& glob, TYPE_ID id);

  TYPE_ID  Elem_type_id() const;
  TYPE_PTR Elem_type() const;
  uint64_t Elem_count() const;
  ARB_PTR  First_dim() const;
  ARB_ID   First_dim_id() const;
  //! Number of dimensions of the array type
  uint32_t   Dim() const;
  DIM_ITER   Begin_dim() const;
  DIM_ITER   End_dim() const;
  DATA_ALIGN Alignment() const;
  uint64_t   Bit_size() const;

  std::vector<int64_t> Shape() const;

  bool Has_size() const;

  void Set_first_dim(ARB_ID id);
  void Set_elem_type(TYPE_ID etype);

private:
  bool Is_analyzed() const;

  void Analyze();
};

class RECORD_TYPE : public TYPE {
  friend class GLOB_SCOPE;
  friend class FIELD;
  friend class FIELD_ITER;

public:
  RECORD_TYPE() : TYPE() {}
  RECORD_TYPE(const GLOB_SCOPE& glob, TYPE_ID id);

  RECORD_KIND Rec_kind() const;
  uint32_t    Num_fld() const;
  DATA_ALIGN  Alignment() const;
  uint64_t    Bit_size() const;

  FIELD_ITER Begin() const;
  FIELD_ITER End() const;
  FIELD_ITER Last() const;

  bool Is_complete() const;
  bool Is_empty() const;
  bool Is_simple() const;
  bool Is_builtin() const;
  bool Is_class() const { return (Rec_kind() == RECORD_KIND::CLASS); }
  bool Is_struct() const { return (Rec_kind() == RECORD_KIND::STRUCT); }
  bool Is_union() const { return (Rec_kind() == RECORD_KIND::UNION); }

  void Set_complete();
  void Set_is_empty(bool val = true);
  void Set_is_builtin(bool val = true);

  void Add_fld(FIELD_ID id);
  void Analyze();

private:
  bool Is_analyzed() const;

  FIELD_ID  First_fld_id() const;
  FIELD_PTR First_fld() const;
  FIELD_ID  Last_fld_id() const;
  FIELD_PTR Last_fld() const;

  void Set_kind(RECORD_KIND k);
  void Set_first_fld_id(FIELD_ID id);
  void Set_last_fld_id(FIELD_ID id);
};

class SIGNATURE_TYPE : public TYPE {
  friend class PARAM;
  friend class PARAM_ITER;

public:
  SIGNATURE_TYPE() : TYPE() {}
  SIGNATURE_TYPE(const GLOB_SCOPE& glob, TYPE_ID id);

  void Set_incomplete();
  void Set_complete();
  void Set_dwarf_rtype(TYPE_PTR t);

  bool Is_complete() const;
  bool Is_asyn_ret() const;
  bool Is_never_ret() const;
  bool Has_non_void_ret() const;

  uint32_t  Num_param() const;
  PARAM_ID  Ret_param_id() const;
  PARAM_PTR Ret_param() const;
  TYPE_PTR  Dwarf_rtype() const;

  PARAM_ITER Begin_param() const;
  PARAM_ITER End_param() const;

private:
  void Add_param(PARAM_ID id);
  void Set_first_param_id(PARAM_ID id);
  void Set_last_param_id(PARAM_ID id);

  PARAM_ID First_param_id() const;
  PARAM_ID Last_param_id() const;
};

class SUBTYPE : public TYPE {
  friend class GLOB_SCOPE;

public:
  SUBTYPE() : TYPE() {}
  SUBTYPE(const GLOB_SCOPE& glob, TYPE_ID id);

  SUBTYPE_KIND Subtype_kind() const;
  DATA_ALIGN   Alignment() const;
  uint64_t     Bit_size() const;

  TYPE_ID  Immediate_base_type_id() const;
  TYPE_PTR Immediate_base_type() const;

  void Set_immediate_base_type(CONST_TYPE_PTR base);
  void Set_immediate_base_type(TYPE_ID base);
  void Set_is_ref();
  void Set_is_addr();
};

class VA_LIST_TYPE : public TYPE {
public:
  VA_LIST_TYPE() : TYPE() {}
  VA_LIST_TYPE(const GLOB_SCOPE& glob, TYPE_ID type);

  DATA_ALIGN Alignment() const;
  uint64_t   Bit_size() const;
};

class PARAM {
  friend class GLOB_SCOPE;
  friend class SIGNATURE_TYPE;
  friend class PARAM_ITER;
  PTR_FRIENDS(PARAM);

public:
  PARAM() : _glob(0), _param() {}
  PARAM(const GLOB_SCOPE& glob, PARAM_ID id);

  PARAM_KIND         Kind() const { return _param->Kind(); }
  GLOB_SCOPE&        Glob_scope() const { return *_glob; }
  PARAM_ID           Id() const;
  TYPE_ID            Type_id() const { return _param->Type(); }
  TYPE_PTR           Type() const;
  PARAM_ID           Next() const { return _param->Next(); }
  STR_PTR            Name() const;
  SPOS               Spos() const { return _param->Spos(); }
  TYPE_ID            Owning_sig_type_id() const;
  SIGNATURE_TYPE_PTR Owning_sig_type() const;

  bool Is_this() const { return _param->Is_this(); }
  bool Is_ret() const { return _param->Is_param_ret(); }
  bool Is_ellips() const { return _param->Is_param_ellips(); }

  void Set_this();
  void Set_ret() { _param->Set_param_ret(); }

  void Print(std::ostream& os, uint32_t indent = 0) const;
  void Print() const;

protected:
  PARAM(const GLOB_SCOPE& glob, PARAM_DATA_PTR ptr)
      : _glob(const_cast<GLOB_SCOPE*>(&glob)), _param(ptr) {}

  bool Is_null() const { return _param.Is_null(); }
  void Set_type(TYPE_ID id) { _param->Set_type(id); }
  void Set_next(PARAM_ID id) { _param->Set_next(id); }
  void Set_param_ret() { _param->Set_param_ret(); }
  void Set_owning_sig(TYPE_ID id);
  void Set_name(STR_ID id);
  void Set_spos(const SPOS& s) { _param->Set_spos(s); }

  PARAM_DATA_PTR Data() const { return _param; }

  GLOB_SCOPE*    _glob;
  PARAM_DATA_PTR _param;
};

}  // namespace base
}  // namespace air

#endif
