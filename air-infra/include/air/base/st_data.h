//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_ST_DATA_H
#define AIR_BASE_ST_DATA_H

#include "air/base/container_decl.h"
#include "air/base/spos.h"
#include "air/base/st_const.h"
#include "air/base/st_decl.h"
#include "air/base/st_trait.h"

namespace air {
namespace base {

/**
 * @brief Attribute data for TYPE/SYM/NODE/etc
 *
 */
class ATTR_DATA {
public:
  ATTR_DATA() : _key(Null_st_id), _value(Null_st_id), _next(Null_st_id) {}

  STR_ID   Key() const { return _key; }
  STR_ID   Value() const { return _value; }
  uint32_t Type() const { return _elem_type; }
  uint32_t Count() const { return _elem_count; }
  ATTR_ID  Next() const { return _next; }

  void Set_key(STR_ID k) { _key = k; }
  void Set_value(STR_ID v) { _value = v; }
  void Set_type(uint32_t t) {
    AIR_ASSERT(t < (1 << 8));
    _elem_type = t;
  }
  void Set_count(uint32_t c) {
    AIR_ASSERT(c < (1 << 24));
    _elem_count = c;
  }
  void Set_next(ATTR_ID n) { _next = n; }

private:
  STR_ID   _key;
  STR_ID   _value;
  ATTR_ID  _next;
  uint32_t _elem_type : 8;
  uint32_t _elem_count : 24;
};

/**
 * @brief Two word long attributes assumed to be the first field
 * of SYM_DATA
 *
 */
struct SYM_ATTR {
  enum class ACCESSED_BY_NESTED { YES = 0, NO = 1 };

  union {
    // common fields
    struct {
      uint32_t _kind : 4;
      uint32_t _padding : 28;
    } _comm;
    // variable & formal
    struct {
      uint32_t _kind : 4;
      uint32_t _in_packet : 1;
      uint32_t _initialized : 1;
      uint32_t _addr_saved : 1;
      uint32_t _addr_passed : 1;
      uint32_t _const_var : 1;
      uint32_t _storage_class : 4;
      uint32_t _mem_class : 3;
      uint32_t _binding : 3;
      uint32_t _register_hint : 2;
      uint32_t _acc_wrt_once : 2;
      uint32_t _ref_implic : 1;
      uint32_t _used : 1;
      uint32_t _modified : 1;
      uint32_t _acc_by_nested_func : 1;
      uint32_t _multi_def : 1;
      uint32_t _reserved : 4;
    } _addr_datum;
    // packet
    struct {
      uint32_t _kind : 4;
      uint32_t _storage_class : 4;
      uint32_t _mem_class : 3;
      uint32_t _binding : 3;
      uint32_t _reserved : 18;
    } _packet;
    // function
    struct {
      uint32_t _kind : 4;
      uint32_t _has_def : 1;
      uint32_t _multi_entry : 1;
      uint32_t _decl_member_func : 1;
      uint32_t _static_member_func : 1;
      uint32_t _decl_virt_member_func : 1;
      uint32_t _pure_virt : 1;
      uint32_t _parent_inner_blk : 1;
      uint32_t _relative_importance : 1;
      uint32_t _landing_pad_binding : 1;
      uint32_t _nesting_level : 4;
      uint32_t _clone_intent : 2;
      uint32_t _inline_intent : 2;
      uint32_t _accessibility : 2;
      uint32_t _creator : 2;
      uint32_t _spec_member_kind : 4;
      uint32_t _need_inner_list : 1;
      uint32_t _priviledged : 1;
      uint32_t _prototyped : 1;
    } _func;
    // entry
    struct {
      uint32_t _kind : 4;
      uint32_t _program_entry : 1;
      uint32_t _reserved : 27;
    } _entry;
    // thunk
    struct {
      uint32_t _kind : 4;
      uint32_t _external : 1;
      uint32_t _this_adjust : 1;
      uint32_t _ret_adjust : 1;
      uint32_t _weak : 1;
      uint32_t _reserved : 24;
    } _thunk;
    // temp variable
    struct {
      uint32_t _kind : 4;
      uint32_t _used : 1;
      uint32_t _callsite_defined : 1;
      uint32_t _compiler_gen : 1;
      uint32_t _reserved : 25;
    } _tmp;
  } _u;
};

class SYM_DATA {
public:
  SYM_DATA(SYMBOL_CLASS kind);

  SYMBOL_CLASS Kind() const { return (SYMBOL_CLASS)_attr._u._comm._kind; }

  bool      Has_aux_entry() const { return Is_null_id(_first_aux_entry); }
  AUX_ID    First_aux_entry() const { return _first_aux_entry; }
  TYPE_ID   Addr_datum_type() const;
  BLOCK_ID  Func_block_id() const;
  FUNC_ID   Entry_owning_func() const;
  TYPE_ID   Entry_type() const;
  uint32_t  Func_nesting_level() const;
  TYPE_ID   Tmp_type() const;
  PACKET_ID Owning_packet() const;

  FUNC_DEF_ID Func_def_id() const;

  bool Is_initialized() const;
  bool Is_in_packet() const;
  bool Is_multi_def() const;
  bool Is_addr_saved() const;
  bool Is_addr_passed() const;
  bool Is_const_var() const;
  bool Is_used() const;
  bool Is_acc_by_nested_func() const;
  bool Is_modified() const;
  bool Is_func_defined() const;
  bool Is_entry_prg_entry() const;
  bool Has_implicit_ref() const;

  void Set_first_aux_entry(AUX_ID id) { _first_aux_entry = id; }
  void Set_addr_datum_type(TYPE_ID id);
  void Set_addr_saved(bool s);
  void Set_addr_passed(bool p);
  void Set_const_var(bool v);
  void Set_initialized();
  void Reset_initialized();
  void Set_used(bool u);
  void Set_modified(bool m);
  void Set_tmp_type(TYPE_ID id);
  void Set_func_block_id(BLOCK_ID blk);
  void Set_entry_type(TYPE_ID id);
  void Set_func_nesting_level(uint32_t l);
  void Set_entry_owning_func(FUNC_ID func);
  void Set_func_def_id(FUNC_DEF_ID id);
  void Set_func_defined(bool def = true);
  void Set_tmp_host_func(FUNC_ID id);
  void Set_entry_prg_entry();
  void Set_implicit_ref();

private:
  SYM_ATTR _attr;

  union {
    struct ADDR_DATUM_DATA {
      union {
        uint32_t _alloc;
        uint32_t _owning_packet;
      };
      uint32_t _type;
      uint32_t _ofst_low_bit;
    } _addr_datum;

    struct PACKET_DATA {
      uint32_t _allocation;
      uint32_t _byte_sz_h;
      uint32_t _byte_sz_l;
    } _packet;

    struct FUNC_DATA {
      uint32_t _func_def;  // FUNC_DEF_ID
      uint32_t _func_blk;  // BLOCK_ID
      uint32_t _local_area;
    } _func;

    struct ENTRY_DATA {
      uint32_t _owning_func;
      uint32_t _signature;
      uint32_t _next;
    } _entry_point;

    struct THUNK_DATA {
      uint32_t _callee_entry;
      uint32_t _signature;
      uint32_t _ajust;
    } _thunk;

    uint32_t _id[3];
  } _u;

  AUX_ID _first_aux_entry;
};

class PREG_DATA {
public:
  PREG_DATA(TYPE_ID type) : _type(type.Value()), _home(Null_st_id) {}
  PREG_DATA(TYPE_ID type, SYM_ID home)
      : _type(type.Value()), _home(home.Value()) {}

  TYPE_ID Type_id() const { return TYPE_ID(_type); }
  SYM_ID  Home_id() const { return SYM_ID(_home); }

  void Set_home(SYM_ID home) { _home = home.Value(); }
  void Set_type(TYPE_ID type) { _type = type.Value(); }

private:
  uint32_t _type;
  uint32_t _home;
};

class PARAM_DATA {
public:
  PARAM_DATA()
      : _name(Null_st_id),
        _spos(),
        _next(Null_st_id),
        _type(Null_st_id),
        _sig_type(Null_st_id),
        _this(0),
        _kind(static_cast<uint32_t>(PARAM_KIND::MANDATORY)),
        _creator(0) {}
  PARAM_KIND Kind() const { return (PARAM_KIND)_kind; }

  bool Is_param_ret() const { return Kind() == PARAM_KIND::RETURN; }
  bool Is_param_ellips() const { return Kind() == PARAM_KIND::ELLIPSIS; }
  bool Is_this() const { return _this; }

  STR_ID   Name() const { return _name; }
  TYPE_ID  Type() const { return _type; }
  TYPE_ID  Signature() const { return _sig_type; }
  PARAM_ID Next() const { return _next; }
  SPOS     Spos() const { return _spos; }

  void Set_name(STR_ID name) { _name = name; }
  void Set_type(TYPE_ID ty) { _type = ty; }
  void Set_signature(TYPE_ID sig) { _sig_type = sig; }
  void Set_next(PARAM_ID n) { _next = n; }
  void Set_spos(const SPOS& s) { _spos = s; }
  void Set_param_ret() { _kind = static_cast<uint32_t>(PARAM_KIND::RETURN); }
  void Set_param_ellips() {
    _kind = static_cast<uint32_t>(PARAM_KIND::ELLIPSIS);
  }
  void Set_this() { _this = 1; }

private:
  STR_ID   _name;
  TYPE_ID  _type;
  TYPE_ID  _sig_type;
  PARAM_ID _next;
  SPOS     _spos;

  uint32_t _kind : 3;
  uint32_t _this : 1;
  uint32_t _creator : 2;
  uint32_t _pass_mod : 4;
  uint32_t _reserved : 22;
};

class FILE_DATA {
public:
  FILE_DATA(STR_ID name, LANG lang) : _lang(lang), _fname(name) {}
  LANG   Lang() const { return _lang; }
  STR_ID Name() const { return _fname; }

protected:
  LANG   _lang;
  STR_ID _fname;
};

class LABEL_DATA {
public:
  LABEL_DATA() : _name(Null_st_id) {}
  STR_ID Label_name() const { return _name; }
  void   Set_label_name(STR_ID n) { _name = n; }

private:
  STR_ID _name;
};

class BLOCK_DATA {
public:
  BLOCK_DATA(BLOCK_KIND k)
      : _func(Null_st_id),
        _parent(Null_st_id),
        _aux_entry(Null_st_id),
        _local_area(Null_st_id),
        _kind(k) {}

  BLOCK_KIND Kind() const { return _kind; }

  void Set_owning_func(FUNC_ID id);
  void Set_parent(BLOCK_ID id) { _parent = id; }

  BLOCK_ID Parent() const { return _parent; }
  FUNC_ID  Owning_func_id() const;
  STR_ID   Named_scope_name() const;
  AUX_ID   Aux_entry() const { return AUX_ID(_aux_entry); }

private:
  BLOCK_KIND _kind;
  BLOCK_ID   _parent;
  union {
    uint32_t _inner_blk_name;
    uint32_t _named_scope_name;
    uint32_t _func;
    uint32_t _module_name;
  };
  uint32_t _aux_entry;
  uint32_t _local_area;
};

class CONSTANT_DATA {
public:
  CONSTANT_DATA(CONSTANT_KIND kind)
      : _type(Null_st_id), _const_kind(static_cast<uint32_t>(kind)) {}

  CONSTANT_KIND Kind() const { return (CONSTANT_KIND)_const_kind; }
  TYPE_ID       Type() const { return _type; }
  bool          Bool_val() const;
  uint64_t      Integer_val() const;
  long double   Float_val() const;
  long double   Complex_real_val() const;
  long double   Complex_imag_val() const;

  POINTER_KIND  Ptr_kind() const;
  ADDR_DATUM_ID Datum_ptr_val() const;
  ENTRY_ID      Entry_ptr_val() const;
  CONSTANT_ID   Array_elem_ptr_base_ptr() const;
  int64_t       Array_elem_ptr_idx() const;
  CONSTANT_ID   Field_ptr_base_ptr() const;
  FIELD_ID      Field_ptr_fld_id() const;
  CONSTANT_ID   Ptr_ofst_base_ptr() const;
  int64_t       Ptr_ofst_ofst() const;
  uint64_t      Ptr_from_unsigned_val() const;
  CONSTANT_ID   Ptr_val() const;
  STR_ID        Str_array_val() const;
  CONSTANT_ID   First_elem() const;
  CONSTANT_ID   Last_elem() const;
  CONSTANT_ID   Next_elem() const;
  CONSTANT_ID   Elem_val() const;
  uint32_t      Elem_index() const;
  uint32_t      Elem_count() const;
  CONSTANT_ID   First_fld() const;
  CONSTANT_ID   Last_fld() const;
  CONSTANT_ID   Next_fld() const;
  CONSTANT_ID   Field_val() const;
  FIELD_ID      Field_fld_id() const;
  FIELD_ID      Union_fld_id() const;
  CONSTANT_ID   Union_val() const;
  TYPE_ID       Union_type() const;
  ENTRY_ID      Func_desc_entry() const;
  CONSTANT_ID   Named_base() const;
  SPOS          Named_spos() const;
  STR_ID        Named_name() const;
  const char*   Array_buffer() const;
  size_t        Array_length() const;
  FILE_ID       Ext_file() const;
  uint64_t      Ext_ofst() const;
  uint64_t      Ext_size() const;

  void Set_type(TYPE_ID type) { _type = type; }
  void Set_bool_val(bool val);
  void Set_integer_val(uint64_t val);
  void Set_float_val(long double val);
  void Set_complex_val(long double real, long double imag);
  void Set_datum_ptr_val(ADDR_DATUM_ID var, POINTER_KIND kind);
  void Set_entry_ptr_val(ENTRY_ID ent, POINTER_KIND kind);
  void Set_array_elem_ptr_val(CONSTANT_ID base, int64_t idx);
  void Set_fld_ptr_val(CONSTANT_ID record, FIELD_ID fld);
  void Set_ptr_ofst_val(CONSTANT_ID base, int64_t ofst);
  void Set_ptr_from_unsigned_val(uint64_t val);
  void Set_ptr_val(CONSTANT_ID ptr);
  void Set_str_array_val(STR_ID str);
  void Set_first_elem(CONSTANT_ID id);
  void Set_last_elem(CONSTANT_ID id);
  void Set_next_elem(CONSTANT_ID id);
  void Set_elem_val(uint64_t idx, uint64_t count, CONSTANT_ID val);
  void Set_first_fld(CONSTANT_ID id);
  void Set_last_fld(CONSTANT_ID id);
  void Set_next_fld(CONSTANT_ID id);
  void Set_fld_val(CONSTANT_ID id);
  void Set_fld_fld_id(FIELD_ID id);
  void Set_union_val(CONSTANT_ID val, FIELD_ID id, TYPE_ID type);
  void Set_func_desc_entry(ENTRY_ID entry);
  void Set_named_val(CONSTANT_ID base, STR_ID name, const SPOS& spos);
  void Set_ext_file(FILE_ID f, uint64_t ofst, uint64_t sz);

private:
  TYPE_ID  _type;
  uint32_t _const_kind : 8;
  uint32_t _ptr_kind : 4;
  uint32_t _bool_val : 1;
  uint32_t _reserved : 19;
};

class BOOL_CONSTANT_DATA : public CONSTANT_DATA {
public:
  BOOL_CONSTANT_DATA(CONSTANT_KIND k) : CONSTANT_DATA(k) {
    AIR_ASSERT(k == CONSTANT_KIND::BOOLEAN);
  }
};

class INT_CONSTANT_DATA : public CONSTANT_DATA {
  friend class CONSTANT_DATA;

public:
  INT_CONSTANT_DATA(CONSTANT_KIND k) : CONSTANT_DATA(k), _val(0) {
    AIR_ASSERT((k == CONSTANT_KIND::SIGNED_INT) ||
               (k == CONSTANT_KIND::UNSIGNED_INT) ||
               (k == CONSTANT_KIND::PTR_FROM_UNSIGNED));
  }

  static INT_CONSTANT_DATA&       Cast_to_me(CONSTANT_DATA& data);
  static const INT_CONSTANT_DATA& Cast_to_me(const CONSTANT_DATA& data);

private:
  uint64_t _val;
};

class FLOAT_CONSTANT_DATA : public CONSTANT_DATA {
  friend class CONSTANT_DATA;

public:
  FLOAT_CONSTANT_DATA(CONSTANT_KIND k) : CONSTANT_DATA(k), _val(0) {
    AIR_ASSERT(k == CONSTANT_KIND::FLOAT);
  }

  static FLOAT_CONSTANT_DATA&       Cast_to_me(CONSTANT_DATA& data);
  static const FLOAT_CONSTANT_DATA& Cast_to_me(const CONSTANT_DATA& data);

private:
  long double _val;
};

class COMPLEX_CONSTANT_DATA : public CONSTANT_DATA {
  friend class CONSTANT_DATA;

public:
  COMPLEX_CONSTANT_DATA(CONSTANT_KIND k)
      : CONSTANT_DATA(k), _real(0), _imag(0) {
    AIR_ASSERT(k == CONSTANT_KIND::COMPLEX);
  }

  static COMPLEX_CONSTANT_DATA&       Cast_to_me(CONSTANT_DATA& data);
  static const COMPLEX_CONSTANT_DATA& Cast_to_me(const CONSTANT_DATA& data);

private:
  long double _real;
  long double _imag;
};

class POINTER_CONSTANT_DATA : public CONSTANT_DATA {
  friend class CONSTANT_DATA;

public:
  POINTER_CONSTANT_DATA(CONSTANT_KIND k)
      : CONSTANT_DATA(k), _ptr_val(Null_st_id) {
    AIR_ASSERT((k == CONSTANT_KIND::PTR_INT) || (k == CONSTANT_KIND::PTR_CAST));
  }

  static POINTER_CONSTANT_DATA&       Cast_to_me(CONSTANT_DATA& data);
  static const POINTER_CONSTANT_DATA& Cast_to_me(const CONSTANT_DATA& data);

private:
  CONSTANT_ID _ptr_val;
};

class SYM_CONSTANT_DATA : public CONSTANT_DATA {
  friend class CONSTANT_DATA;

public:
  SYM_CONSTANT_DATA(CONSTANT_KIND k) : CONSTANT_DATA(k), _sym(Null_st_id) {
    AIR_ASSERT((k == CONSTANT_KIND::VAR_PTR) ||
               (k == CONSTANT_KIND::ENTRY_PTR) ||
               (k == CONSTANT_KIND::THUNK_PTR) ||
               (k == CONSTANT_KIND::ENTRY_FUNC_DESC) ||
               (k == CONSTANT_KIND::THUNK_FUNC_DESC));
  }

  static SYM_CONSTANT_DATA&       Cast_to_me(CONSTANT_DATA& data);
  static const SYM_CONSTANT_DATA& Cast_to_me(const CONSTANT_DATA& data);

private:
  uint32_t _sym;
};

class DERIVED_PTR_CONSTANT_DATA : public CONSTANT_DATA {
  friend class CONSTANT_DATA;

public:
  DERIVED_PTR_CONSTANT_DATA(CONSTANT_KIND k)
      : CONSTANT_DATA(k), _base(Null_st_id) {
    AIR_ASSERT((k == CONSTANT_KIND::ARRAY_ELEM_PTR) ||
               (k == CONSTANT_KIND::PTR_OFST) ||
               (k == CONSTANT_KIND::FIELD_PTR));
    if (k == CONSTANT_KIND::FIELD_PTR) {
      _fld = Null_st_id;
    } else {
      _idx = 0;
    }
  }

  static DERIVED_PTR_CONSTANT_DATA&       Cast_to_me(CONSTANT_DATA& data);
  static const DERIVED_PTR_CONSTANT_DATA& Cast_to_me(const CONSTANT_DATA& data);

private:
  union {
    int64_t  _idx;   // array element pointer
    int64_t  _ofst;  // pointer offset
    uint32_t _fld;   // field_id
  };
  CONSTANT_ID _base;
};

class STR_CONSTANT_DATA : public CONSTANT_DATA {
  friend class CONSTANT_DATA;

public:
  STR_CONSTANT_DATA(CONSTANT_KIND k) : CONSTANT_DATA(k), _str(Null_st_id) {
    AIR_ASSERT(k == CONSTANT_KIND::STR_ARRAY);
  }

  static STR_CONSTANT_DATA&       Cast_to_me(CONSTANT_DATA& data);
  static const STR_CONSTANT_DATA& Cast_to_me(const CONSTANT_DATA& data);

private:
  STR_ID _str;
};

class ARRAY_CONSTANT_DATA : public CONSTANT_DATA {
  friend class CONSTANT_DATA;

public:
  ARRAY_CONSTANT_DATA(CONSTANT_KIND k, TYPE_ID type, void* buf, size_t sz)
      : CONSTANT_DATA(k) {
    AIR_ASSERT(k == CONSTANT_KIND::ARRAY);
    Set_type(type);
    _len = sz;
    memcpy(_buf, buf, sz);
  }

  static ARRAY_CONSTANT_DATA&       Cast_to_me(CONSTANT_DATA& data);
  static const ARRAY_CONSTANT_DATA& Cast_to_me(const CONSTANT_DATA& data);

private:
  size_t _len;
  char   _buf[0];
};

class ARRAY_CONST_HEADER_CONSTANT_DATA : public CONSTANT_DATA {
  friend class CONSTANT_DATA;

public:
  ARRAY_CONST_HEADER_CONSTANT_DATA(CONSTANT_KIND k)
      : CONSTANT_DATA(k), _first(Null_st_id), _last(Null_st_id) {
    AIR_ASSERT(k == CONSTANT_KIND::ARRAY);
  }

  static ARRAY_CONST_HEADER_CONSTANT_DATA& Cast_to_me(CONSTANT_DATA& data);
  static const ARRAY_CONST_HEADER_CONSTANT_DATA& Cast_to_me(
      const CONSTANT_DATA& data);

private:
  CONSTANT_ID _first;
  CONSTANT_ID _last;
};

class ARRAY_ELEM_CONSTANT_DATA : public CONSTANT_DATA {
  friend class CONSTANT_DATA;

public:
  ARRAY_ELEM_CONSTANT_DATA(CONSTANT_KIND k)
      : CONSTANT_DATA(k),
        _val(Null_st_id),
        _next(Null_st_id),
        _elem_idx(0xdeadbeef),
        _repeat_count(0xdeadbeef) {
    AIR_ASSERT(k == CONSTANT_KIND::ARRAY_ELEM);
  }

  static ARRAY_ELEM_CONSTANT_DATA&       Cast_to_me(CONSTANT_DATA& data);
  static const ARRAY_ELEM_CONSTANT_DATA& Cast_to_me(const CONSTANT_DATA& data);

private:
  uint32_t    _elem_idx;
  uint32_t    _repeat_count;
  CONSTANT_ID _val;
  CONSTANT_ID _next;
};

class STRUCT_CONST_HEADER_CONSTANT_DATA : public CONSTANT_DATA {
  friend class CONSTANT_DATA;

public:
  STRUCT_CONST_HEADER_CONSTANT_DATA(CONSTANT_KIND k)
      : CONSTANT_DATA(k), _first(Null_st_id), _last(Null_st_id) {
    AIR_ASSERT(k == CONSTANT_KIND::STRUCT);
  }

  static STRUCT_CONST_HEADER_CONSTANT_DATA& Cast_to_me(CONSTANT_DATA& data);
  static const STRUCT_CONST_HEADER_CONSTANT_DATA& Cast_to_me(
      const CONSTANT_DATA& data);

private:
  CONSTANT_ID _first;
  CONSTANT_ID _last;
};

class STRUCT_FIELD_CONSTANT_DATA : public CONSTANT_DATA {
  friend class CONSTANT_DATA;

public:
  STRUCT_FIELD_CONSTANT_DATA(CONSTANT_KIND k)
      : CONSTANT_DATA(k),
        _val(Null_st_id),
        _fld(Null_st_id),
        _next(Null_st_id) {
    AIR_ASSERT(k == CONSTANT_KIND::STRUCT_FIELD);
  }

  static STRUCT_FIELD_CONSTANT_DATA&       Cast_to_me(CONSTANT_DATA& data);
  static const STRUCT_FIELD_CONSTANT_DATA& Cast_to_me(
      const CONSTANT_DATA& data);

private:
  CONSTANT_ID _val;
  FIELD_ID    _fld;
  CONSTANT_ID _next;
};

class UNION_CONSTANT_DATA : public CONSTANT_DATA {
  friend class CONSTANT_DATA;

public:
  UNION_CONSTANT_DATA(CONSTANT_KIND k)
      : CONSTANT_DATA(k),
        _val(Null_st_id),
        _fld(Null_st_id),
        _type(Null_st_id) {
    AIR_ASSERT(k == CONSTANT_KIND::UNION);
  }

  static UNION_CONSTANT_DATA&       Cast_to_me(CONSTANT_DATA& data);
  static const UNION_CONSTANT_DATA& Cast_to_me(const CONSTANT_DATA& data);

private:
  CONSTANT_ID _val;
  FIELD_ID    _fld;
  TYPE_ID     _type;
};

class NAMED_CONSTANT_DATA : public CONSTANT_DATA {
  friend class CONSTANT_DATA;

public:
  NAMED_CONSTANT_DATA(CONSTANT_KIND k)
      : CONSTANT_DATA(k), _base(Null_st_id), _name(Null_st_id), _spos() {
    AIR_ASSERT(k == CONSTANT_KIND::NAMED);
  }

  static NAMED_CONSTANT_DATA&       Cast_to_me(CONSTANT_DATA& data);
  static const NAMED_CONSTANT_DATA& Cast_to_me(const CONSTANT_DATA& data);

private:
  CONSTANT_ID _base;
  STR_ID      _name;
  SPOS        _spos;
};

class EXTERN_CONSTANT_DATA : public CONSTANT_DATA {
  friend class CONSTANT_DATA;

public:
  EXTERN_CONSTANT_DATA(CONSTANT_KIND k)
      : CONSTANT_DATA(k), _file(Null_st_id), _ofst(0), _size(0) {
    AIR_ASSERT(k == CONSTANT_KIND::EXT_FILE);
  }

  static EXTERN_CONSTANT_DATA&       Cast_to_me(CONSTANT_DATA& data);
  static const EXTERN_CONSTANT_DATA& Cast_to_me(const CONSTANT_DATA& data);

private:
  FILE_ID  _file;
  uint64_t _ofst;
  uint64_t _size;
};

/**
 * @brief Auxilary symbol entry base class
 *
 */
class AUX_DATA {
public:
  AUX_DATA(AUX_KIND k)
      : _next(Null_st_id), _kind(static_cast<uint32_t>(k)), _ofst_bit_h(0) {}

  AUX_KIND Kind() const { return (AUX_KIND)_kind; }
  AUX_ID   Next() const { return _next; }
  void     Set_next(AUX_ID n) { _next = n; }

  template <AUX_KIND K>
  typename AUX_DATA_TRAITS<K>::AUX_DATA_TYPE& Cast_aux_data();

protected:
  AUX_ID   _next;
  uint32_t _kind : 5;
  uint32_t _ofst_bit_h : 27;
};

class AUX_SYM_DATA : public AUX_DATA {
public:
  AUX_SYM_DATA() : AUX_DATA(AUX_KIND::SYM), _name(Null_st_id), _spos() {}

  void Init_datum_flag();
  bool Is_this() const;
  bool Is_static_fld() const { return _u._datum._flag._static_fld; }
  bool Is_local_static() const;

  STR_ID   Name() const { return STR_ID(_name); }
  FIELD_ID Field() const { return FIELD_ID(_fld); }
  SPOS     Spos() const { return _spos; }
  ENTRY_ID First_entry_point() const { return ENTRY_ID(_u._func._first_entry); }

  void Set_name(STR_ID name) { _name = name.Value(); }
  void Set_field(FIELD_ID fld) { _fld = fld.Value(); }
  void Set_spos(const SPOS& s) { _spos = s; }
  void Set_datum_owning_func(FUNC_ID func) {
    _u._datum._owning_func = func.Value();
  }
  void Set_first_entry_point(ENTRY_ID id) {
    _u._func._first_entry = id.Value();
  }

private:
  SPOS _spos;
  union {
    uint32_t _name;
    uint32_t _fld;  // id for static field
  };
  union {
    struct DATUM {
      union {
        uint32_t _owning_func;  // function scope statics
        uint32_t _sz_check_mode;
      };
      uint32_t _initializer;  // CONSTANT_ID
      uint32_t _map_text;     // CONSTANT_ID
      struct FLAG {
        uint32_t _fpinfo : 8;
        uint32_t _this : 1;
        uint32_t _inlined_formal : 1;
        uint32_t _static_fld : 1;
        uint32_t _local_static : 1;
        uint32_t _creator : 2;
        uint32_t _reserved : 18;
      } _flag;
    } _datum;

    struct FUNC {
      uint32_t _first_entry;
      int32_t  _vtb_vcall_ofst;
    } _func;

    struct ENTRY {
      uint32_t _max_amb_arg_num;
    } _entry;

    struct THUNK {
      int32_t  _ret_adjust;
      int32_t  _vtb_call_ofst;
      uint32_t _vtb_call_ofst_type;
    } _thunk;

    struct RET_THUNK {
      int32_t  _vtb_base_ofst;
      uint32_t _vtb_base_ofst_type;
    } _ret_thunk;
  } _u;
};

/**
 * @brief String table data
 *
 */
class STR_DATA {
public:
  uint32_t    Len() const { return _len; }
  const char* Str() const { return _str; }
  void        Set_len(uint32_t l) { _len = l; }

private:
  uint32_t _len;
  char     _str[1];
};

class FIELD_DATA {
public:
  FIELD_DATA(FIELD_KIND k)
      : _name(Null_st_id),
        _type(Null_st_id),
        _rec_type(Null_st_id),
        _spos(),
        _next(Null_st_id),
        _ofst(0),
        _kind(static_cast<uint32_t>(k)),
        _access(0),
        _mutability(0),
        _creator(0),
        _alignment(static_cast<uint32_t>(DATA_ALIGN::BAD)),
        _bit_fld(0),
        _bit_fld_size(0) {}

  FIELD_KIND Kind() const { return (FIELD_KIND)_kind; }
  STR_ID     Name() const { return _name; }
  TYPE_ID    Type() const { return _type; }
  FIELD_ID   Next() const { return _next; }
  TYPE_ID    Owning_rec_type() const { return _rec_type; }
  SPOS       Spos() const;
  uint64_t   Ofst() const { return _ofst; }
  uint64_t   Byte_ofst() const {
    AIR_ASSERT((Kind() != FIELD_KIND::STATIC) &&
                 (Kind() != FIELD_KIND::VIRT_INH));
    return (_ofst >> 3);
  }
  uint64_t Bit_ofst() const {
    AIR_ASSERT((Kind() != FIELD_KIND::STATIC) &&
               (Kind() != FIELD_KIND::VIRT_INH) && Is_bit_fld());
    return (_ofst & 0x7UL);
  }
  uint32_t Bit_fld_size() const {
    AIR_ASSERT((Kind() != FIELD_KIND::STATIC) &&
               (Kind() != FIELD_KIND::VIRT_INH) && Is_bit_fld());
    return _bit_fld_size;
  }
  DATA_ALIGN Alignment() const { return (DATA_ALIGN)_alignment; }
  int64_t    Vtb_ofst() const;

  bool Is_bit_fld() const { return (_bit_fld != 0); }

  void Set_name(STR_ID n) { _name = n; }
  void Set_type(TYPE_ID t) { _type = t; }
  void Set_owning_rec_type(TYPE_ID t) { _rec_type = t; }
  void Set_next(FIELD_ID n) { _next = n; }
  void Set_spos(const SPOS& s);
  void Set_ofst(uint64_t bit) {
    AIR_ASSERT((Kind() != FIELD_KIND::STATIC) &&
               (Kind() != FIELD_KIND::VIRT_INH));
    _ofst = bit;
  }
  void Set_bit_fld_size(uint32_t sz) {
    AIR_ASSERT((Kind() != FIELD_KIND::STATIC) &&
               (Kind() != FIELD_KIND::VIRT_INH) && Is_bit_fld() &&
               (sz < 0xFFUL));
    _bit_fld_size = sz;
  }
  void Set_bit_fld() {
    AIR_ASSERT((Kind() != FIELD_KIND::STATIC) &&
               (Kind() != FIELD_KIND::VIRT_INH));
    _bit_fld = 1;
  }
  void Set_alignment(DATA_ALIGN align) {
    _alignment = static_cast<uint32_t>(align);
  }
  void Set_vtb_ofst(int64_t bytes);

private:
  STR_ID   _name;
  TYPE_ID  _type;
  TYPE_ID  _rec_type;
  FIELD_ID _next;
  SPOS     _spos;

  union {
    uint64_t _ofst;  // offset within the struct in bits
    int64_t  _vtb_ofst;
  };

  uint32_t _kind : 8;
  uint32_t _access : 4;
  uint32_t _mutability : 2;
  uint32_t _creator : 2;
  uint32_t _alignment : 4;     // same as type alignment if not set
  uint32_t _bit_fld : 1;       // bit field flag
  uint32_t _bit_fld_size : 8;  // bit field size in bits
  uint32_t _reserved : 3;
};

class FUNC_DEF_DATA {
public:
  FUNC_DEF_DATA();

  bool Has_nested_func() const { return _attr._has_nested_func; }
  bool Has_inline() const { return _attr._inlined; }

  SPOS     Func_begin_spos() const { return _begin; }
  SPOS     Func_end_spos() const { return _end; }
  STMT_ID  Entry_stmt_id() const { return _entry_stmt; }
  uint32_t Max_annot_id() const { return _annotation_id; }
  uint32_t New_annot_id() { return _annotation_id++; }

  void Set_has_inline(bool b) { _attr._inlined = b; }
  void Set_has_nested_func(bool b) { _attr._has_nested_func = b; }
  void Set_func_begin_spos(const SPOS& s) { _begin = s; }
  void Set_func_end_spos(const SPOS& s) { _end = s; }
  void Set_entry_stmt(STMT_ID id) { _entry_stmt = id; }
  void Set_annot_id(uint32_t id) { _annotation_id = id; }

private:
  SPOS     _begin;
  SPOS     _end;
  STMT_ID  _entry_stmt;
  uint32_t _annotation_id;

  struct {
    uint32_t _inlined : 1;
    uint32_t _has_nested_func : 1;
    uint32_t _reserved : 30;
  } _attr;
};

//! Base class for type data structures
class TYPE_DATA {
public:
  TYPE_DATA(TYPE_TRAIT kind) : _spos(), _name(Null_st_id) {
    _bit_size                     = 0;
    _domain_tag                   = 0;
    _alignment                    = static_cast<uint32_t>(DATA_ALIGN::BAD);
    _u._comm_info._kind           = static_cast<uint32_t>(kind);
    _u._comm_info._record_scoped  = 0;
    _u._comm_info._implicit       = 0;
    _u._comm_info._strongly_typed = 0;
    _u._comm_info._size_set       = 0;
    _u._comm_info._align_set      = 0;
  }

  TYPE_TRAIT Kind() const { return (TYPE_TRAIT)_u._comm_info._kind; };
  uint32_t   Domain_tag() const { return _domain_tag; }
  uint64_t   Bit_size() const { return _bit_size; }
  DATA_ALIGN Alignment() const { return (DATA_ALIGN)_alignment; }
  SPOS       Spos() const { return _spos; }
  STR_ID     Name() const { return _name; }

  void Set_spos(const SPOS& s) { _spos = s; }
  void Set_name(STR_ID id) { _name = id; }
  void Set_domain_tag(uint32_t t) { _domain_tag = t; }
  void Set_bit_size(uint64_t sz) {
    _bit_size               = sz;
    _u._comm_info._size_set = 1;
  }
  void Set_alignment(DATA_ALIGN a) {
    _alignment               = static_cast<uint32_t>(a);
    _u._comm_info._align_set = 1;
  }

  bool Is_bit_size_set() const { return (_u._comm_info._size_set != 0); }
  bool Is_alignment_set() const { return (_u._comm_info._align_set != 0); }

  template <TYPE_TRAIT T>
  typename TYPE_DATA_TRAITS<T>::TYPE_DATA_TYPE& Cast_type_data() {
    AIR_ASSERT(Kind() == T);
    return *static_cast<typename TYPE_DATA_TRAITS<T>::TYPE_DATA_TYPE*>(this);
  }

protected:
  uint64_t _bit_size;
  SPOS     _spos;
  STR_ID   _name;
  uint32_t _domain_tag : 16;
  uint32_t _alignment : 4;
  uint32_t _reserved : 12;
  union {
    // common fields of all types
    struct {
      uint32_t _kind : 8;
      uint32_t _record_scoped : 1;
      uint32_t _implicit : 1;
      uint32_t _strongly_typed : 3;
      uint32_t _size_set : 1;
      uint32_t _align_set : 1;
      uint32_t _reserved : 1;
      uint32_t _padding : 16;
    } _comm_info;
    // primitive type info
    struct {
      uint32_t _comm_bit : 16;
      uint32_t _encoding : 8;
      uint32_t _reserved : 8;
    } _prim_info;
    // pointer type info
    struct {
      uint32_t _comm_bit : 16;
      uint32_t _ptr_kind : 4;
      uint32_t _reserved : 12;
      uint32_t _domain;  // pointed to type
    } _ptr_info;
    // array type info
    struct {
      uint32_t _comm_bit : 16;
      uint32_t _analyzed : 1;
      uint32_t _reserved : 15;
      uint32_t _elem_type;
      uint32_t _first_dim;
    } _arr_info;
    // record type info
    struct {
      uint32_t _comm_bit : 16;
      uint32_t _num_fld : 16;
      uint32_t _kind : 2;
      uint32_t _complete : 1;
      uint32_t _empty : 1;
      uint32_t _simple : 1;
      uint32_t _bit_size : 4;
      uint32_t _builtin : 1;
      uint32_t _analyzed : 1;
      uint32_t _reserved : 21;
      uint32_t _first_fld;
      uint32_t _last_fld;
    } _rec_info;
    // signature type info
    struct {
      uint32_t _comm_bit : 16;
      uint32_t _complete : 1;
      uint32_t _never_ret : 1;
      uint32_t _async_ret : 1;
      uint32_t _reserved : 13;
      uint32_t _first_param;
      uint32_t _last_param;
      uint32_t _dwarf_type;
    } _signature_info;
    // subtype type info
    struct {
      uint32_t _comm_bit : 16;
      uint32_t _subtype_kind : 4;
      uint32_t _spf_form : 4;
      uint32_t _reserved : 8;
      uint32_t _immediate_base_type;
    } _subtype_info;
  } _u;
};

class PRIM_TYPE_DATA : public TYPE_DATA {
public:
  PRIM_TYPE_DATA() : TYPE_DATA(TYPE_TRAIT::PRIMITIVE) {
    _u._prim_info._encoding = 0;
  }
  PRIMITIVE_TYPE Prim_type_enc() const {
    return (PRIMITIVE_TYPE)_u._prim_info._encoding;
  }
  void Set_prim_type_enc(PRIMITIVE_TYPE enc) {
    _u._prim_info._encoding  = static_cast<uint32_t>(enc);
    _u._comm_info._size_set  = 1;
    _u._comm_info._align_set = 1;
    switch (enc) {
      case PRIMITIVE_TYPE::BOOL:
      case PRIMITIVE_TYPE::INT_S8:
      case PRIMITIVE_TYPE::INT_U8:
        _bit_size  = 8;
        _alignment = static_cast<uint32_t>(DATA_ALIGN::BYTE1);
        break;
      case PRIMITIVE_TYPE::INT_S16:
      case PRIMITIVE_TYPE::INT_U16:
        _bit_size  = 16;
        _alignment = static_cast<uint32_t>(DATA_ALIGN::BYTE2);
        break;
      case PRIMITIVE_TYPE::INT_S32:
      case PRIMITIVE_TYPE::INT_U32:
      case PRIMITIVE_TYPE::FLOAT_32:
        _bit_size  = 32;
        _alignment = static_cast<uint32_t>(DATA_ALIGN::BYTE4);
        break;
      case PRIMITIVE_TYPE::COMPLEX_32:
        _bit_size  = 64;
        _alignment = static_cast<uint32_t>(DATA_ALIGN::BYTE4);
        break;
      case PRIMITIVE_TYPE::INT_S64:
      case PRIMITIVE_TYPE::INT_U64:
      case PRIMITIVE_TYPE::FLOAT_64:
        _bit_size  = 64;
        _alignment = static_cast<uint32_t>(DATA_ALIGN::BYTE8);
        break;
      case PRIMITIVE_TYPE::COMPLEX_64:
        _bit_size  = 128;
        _alignment = static_cast<uint32_t>(DATA_ALIGN::BYTE8);
        break;
      case PRIMITIVE_TYPE::FLOAT_80:
      case PRIMITIVE_TYPE::FLOAT_128:
        _bit_size  = 128;
        _alignment = static_cast<uint32_t>(DATA_ALIGN::BYTE16);
        break;
      case PRIMITIVE_TYPE::COMPLEX_80:
      case PRIMITIVE_TYPE::COMPLEX_128:
        _bit_size  = 256;
        _alignment = static_cast<uint32_t>(DATA_ALIGN::BYTE16);
        break;
      default:
        _u._comm_info._size_set  = 0;
        _u._comm_info._align_set = 0;
        break;
    }
    return;
  }
};

class VA_LIST_TYPE_DATA : public TYPE_DATA {
public:
  VA_LIST_TYPE_DATA() : TYPE_DATA(TYPE_TRAIT::VA_LIST) {}
};

class POINTER_TYPE_DATA : public TYPE_DATA {
public:
  POINTER_TYPE_DATA() : TYPE_DATA(TYPE_TRAIT::POINTER) {
    _u._ptr_info._ptr_kind = 0;
    _u._ptr_info._domain   = Null_st_id;
  }
  TYPE_ID      Domain() const { return TYPE_ID(_u._ptr_info._domain); }
  POINTER_KIND Ptr_kind() const { return (POINTER_KIND)_u._ptr_info._ptr_kind; }

  void Set_domain(TYPE_ID domain) { _u._ptr_info._domain = domain.Value(); }
  void Set_ptr_kind(POINTER_KIND k) {
    _u._ptr_info._ptr_kind   = static_cast<uint32_t>(k);
    _u._comm_info._size_set  = 1;
    _u._comm_info._align_set = 1;
    switch (k) {
      case POINTER_KIND::FLAT32:
        _bit_size  = 32;
        _alignment = static_cast<uint32_t>(DATA_ALIGN::BYTE4);
        break;
      case POINTER_KIND::FLAT64:
        _bit_size  = 64;
        _alignment = static_cast<uint32_t>(DATA_ALIGN::BYTE8);
        break;
      default:
        AIR_ASSERT(0);
        _u._comm_info._size_set  = 0;
        _u._comm_info._align_set = 0;
        break;
    }
  }
};

//! ARB_TAB entry
class ARB_DATA {
public:
  //! @brief Construct a new arb data object, lower bound, upper bound
  //! and stride are set to be none const with invalid id, all of its
  //! fields must be set before used
  ARB_DATA() {
    _flag = 0;
    _dim  = 0;
    _next = Null_st_id;
    Set_lb_var(Null_st_id);
    Set_ub_var(Null_st_id);
    Set_stride_var(Null_st_id);
  }

  //! @brief Construct a new arb data object of one dimension with const bounds
  //! and stride
  //! @param lb const lower bound value
  //! @param ub const upper bound value
  //! @param stride const stride value
  ARB_DATA(uint32_t dim, int64_t lb, int64_t ub, int64_t stride) {
    _flag = static_cast<uint32_t>(ARB_FLAG::FIRST_DIM) |
            static_cast<uint32_t>(ARB_FLAG::LAST_DIM);
    _dim  = dim;
    _next = Null_st_id;
    Set_lb_val(lb);
    Set_ub_val(ub);
    Set_stride_val(stride);
  }

  int64_t Lb_val() const {
    AIR_ASSERT(Is_lb_const());
    return _lb._val;
  }
  uint32_t Lb_var() const {
    AIR_ASSERT(!Is_lb_const());
    return _lb._var._idx;
  }
  int64_t Ub_val() const {
    AIR_ASSERT(Is_ub_const());
    return _ub._val;
  }
  uint32_t Ub_var() const {
    AIR_ASSERT(!Is_ub_const());
    return _ub._var._idx;
  }
  int64_t Stride_val() const {
    AIR_ASSERT(Is_stride_const());
    return _stride._val;
  }
  uint32_t Stride_var() const {
    AIR_ASSERT(!Is_stride_const());
    return _stride._var._idx;
  }
  uint32_t Dim() const { return _dim; }
  uint32_t Next() const { return _next; }

  bool Is_lb_const() const {
    return (_flag & static_cast<uint32_t>(ARB_FLAG::LB_CONST));
  }
  bool Is_ub_const() const {
    return (_flag & static_cast<uint32_t>(ARB_FLAG::UB_CONST));
  }
  bool Is_stride_const() const {
    return (_flag & static_cast<uint32_t>(ARB_FLAG::STRIDE_CONST));
  }
  bool Is_first_dim() const {
    return (_flag & static_cast<uint32_t>(ARB_FLAG::FIRST_DIM));
  }
  bool Is_last_dim() const {
    return ((_flag & static_cast<uint32_t>(ARB_FLAG::LAST_DIM)) &&
            (_next == Null_st_id));
  }
  bool Is_lb_unknown() const {
    return ((!Is_lb_const()) && (_lb._var._idx == Null_st_id));
  }
  bool Is_ub_unknown() const {
    return ((!Is_ub_const()) && (_ub._var._idx == Null_st_id));
  }
  bool Is_stride_unknown() const {
    return ((!Is_stride_const()) && (_stride._var._idx == Null_st_id));
  }

  void Set_lb_const() { _flag |= static_cast<uint32_t>(ARB_FLAG::LB_CONST); }
  void Unset_lb_const() { _flag &= ~static_cast<uint32_t>(ARB_FLAG::LB_CONST); }
  void Set_lb_val(int64_t v) {
    Set_lb_const();
    _lb._val = v;
  }
  void Set_lb_var(uint32_t v) {
    Unset_lb_const();
    _lb._var._idx = v;
    _lb._var._pad = 0;
  }
  void Set_ub_const() { _flag |= static_cast<uint32_t>(ARB_FLAG::UB_CONST); }
  void Unset_ub_const() { _flag &= ~static_cast<uint32_t>(ARB_FLAG::UB_CONST); }
  void Set_ub_val(int64_t v) {
    Set_ub_const();
    _ub._val = v;
  }
  void Set_ub_var(uint32_t v) {
    Unset_ub_const();
    _ub._var._idx = v;
    _ub._var._pad = 0;
  }
  void Set_stride_const() {
    _flag |= static_cast<uint32_t>(ARB_FLAG::STRIDE_CONST);
  }
  void Unset_stride_const() {
    _flag &= ~static_cast<uint32_t>(ARB_FLAG::STRIDE_CONST);
  }
  void Set_stride_val(int64_t v) {
    Set_stride_const();
    _stride._val = v;
  }
  void Set_stride_var(uint32_t v) {
    Unset_stride_const();
    _stride._var._idx = v;
    _stride._var._pad = 0;
  }
  void Set_first_dim() { _flag |= static_cast<uint32_t>(ARB_FLAG::FIRST_DIM); }
  void Unset_first_dim() {
    _flag &= ~static_cast<uint32_t>(ARB_FLAG::FIRST_DIM);
  }
  void Set_last_dim() { _flag |= static_cast<uint32_t>(ARB_FLAG::LAST_DIM); }
  void Unset_last_dim() { _flag &= ~static_cast<uint32_t>(ARB_FLAG::LAST_DIM); }
  void Set_dim(uint32_t d) { _dim = d; }
  void Set_next(uint32_t n) { _next = n; }

private:
  uint32_t _flag : 16;
  uint32_t _dim : 16;
  uint32_t _next;

  union {
    int64_t _val;
    struct {
      uint32_t _idx;
      uint32_t _pad;
    } _var;
  } _lb;

  union {
    int64_t _val;
    struct {
      uint32_t _idx;
      uint32_t _pad;
    } _var;
  } _ub;

  union {
    int64_t _val;
    struct {
      uint32_t _idx;
      uint32_t _pad;
    } _var;
  } _stride;
};

class ARRAY_TYPE_DATA : public TYPE_DATA {
  friend class ARRAY_TYPE;

public:
  ARRAY_TYPE_DATA() : TYPE_DATA(TYPE_TRAIT::ARRAY) {
    _u._arr_info._analyzed  = 0;
    _u._arr_info._elem_type = Null_st_id;
    _u._arr_info._first_dim = Null_st_id;
  }

  TYPE_ID Elem_type() const { return TYPE_ID(_u._arr_info._elem_type); }
  ARB_ID  First_dim() const { return ARB_ID(_u._arr_info._first_dim); }

  bool Is_analyzed() const { return (_u._arr_info._analyzed != 0); }

  void Set_first_dim(ARB_ID arb) { _u._arr_info._first_dim = arb.Value(); }
  void Set_elem_type(TYPE_ID id) { _u._arr_info._elem_type = id.Value(); }
  void Set_analyzed() { _u._arr_info._analyzed = 1; }
};

class RECORD_TYPE_DATA : public TYPE_DATA {
public:
  RECORD_TYPE_DATA() : TYPE_DATA(TYPE_TRAIT::RECORD) {
    _u._rec_info._num_fld   = 0;
    _u._rec_info._kind      = 0;
    _u._rec_info._complete  = 0;
    _u._rec_info._empty     = 0;
    _u._rec_info._simple    = 0;
    _u._rec_info._bit_size  = 0;
    _u._rec_info._builtin   = 0;
    _u._rec_info._analyzed  = 0;
    _u._rec_info._first_fld = Null_st_id;
    _u._rec_info._last_fld  = Null_st_id;
  }

  RECORD_KIND Kind() const { return (RECORD_KIND)_u._rec_info._kind; }
  uint32_t    Num_of_fld() const {
    AIR_ASSERT(Is_complete());
    return _u._rec_info._num_fld;
  }
  FIELD_ID First_fld_id() const { return FIELD_ID(_u._rec_info._first_fld); }
  FIELD_ID Last_fld_id() const { return FIELD_ID(_u._rec_info._last_fld); }

  bool Is_complete() const { return (_u._rec_info._complete != 0); }
  bool Is_empty() const { return (_u._rec_info._empty != 0); }
  bool Is_simple() const { return (_u._rec_info._simple != 0); }
  bool Is_builtin() const { return (_u._rec_info._builtin != 0); }
  bool Is_analyzed() const { return (_u._rec_info._analyzed != 0); }

  void Set_kind(RECORD_KIND k) {
    _u._rec_info._kind = static_cast<uint32_t>(k);
  }
  void Set_complete() { _u._rec_info._complete = 1; }
  void Set_empty(bool v = true) { _u._rec_info._empty = v; }
  void Set_simple(bool v = true) { _u._rec_info._simple = v; }
  void Set_builtin(bool v = true) { _u._rec_info._builtin = v; }
  void Set_first_fld_id(FIELD_ID f) { _u._rec_info._first_fld = f.Value(); }
  void Set_last_fld_id(FIELD_ID f) { _u._rec_info._last_fld = f.Value(); }
  void Add_fld() {
    if (_u._rec_info._num_fld < 0xFFFFU) {
      _u._rec_info._num_fld++;
    }
  }
  void Set_analyzed() { _u._rec_info._analyzed = 1; }
};

class SIGNATURE_TYPE_DATA : public TYPE_DATA {
public:
  SIGNATURE_TYPE_DATA() : TYPE_DATA(TYPE_TRAIT::SIGNATURE) {
    _u._signature_info._complete    = 0;
    _u._signature_info._never_ret   = 0;
    _u._signature_info._async_ret   = 0;
    _u._signature_info._first_param = Null_st_id;
    _u._signature_info._last_param  = Null_st_id;
    _u._signature_info._dwarf_type  = Null_st_id;
  }

  PARAM_ID First_param_id() const {
    return PARAM_ID(_u._signature_info._first_param);
  }
  PARAM_ID Last_param_id() const {
    return PARAM_ID(_u._signature_info._last_param);
  }
  TYPE_ID Dwarf_type() const { return TYPE_ID(_u._signature_info._dwarf_type); }

  bool Is_complete() const { return (_u._signature_info._complete != 0); }
  bool Is_never_ret() const { return (_u._signature_info._never_ret != 0); }
  bool Is_asyn_ret() const { return (_u._signature_info._async_ret != 0); }

  void Set_sig_incomplete() { _u._signature_info._complete = 0; }
  void Set_sig_complete() { _u._signature_info._complete = 1; }
  void Set_first_param_id(PARAM_ID id) {
    _u._signature_info._first_param = id.Value();
  }
  void Set_last_param_id(PARAM_ID id) {
    _u._signature_info._last_param = id.Value();
  }
  void Set_dwarf_rtype(TYPE_ID id) {
    _u._signature_info._dwarf_type = id.Value();
  }
};

class SUBTYPE_DATA : public TYPE_DATA {
public:
  SUBTYPE_DATA() : TYPE_DATA(TYPE_TRAIT::SUBTYPE) {
    _u._subtype_info._subtype_kind =
        static_cast<uint32_t>(SUBTYPE_KIND::TYPEDEF);
    _u._subtype_info._spf_form            = 0;
    _u._subtype_info._immediate_base_type = Null_st_id;
  }

  SUBTYPE_KIND Subtype_kind() const {
    return (SUBTYPE_KIND)_u._subtype_info._subtype_kind;
  }
  TYPE_ID Immediate_base_type() const {
    return TYPE_ID(_u._subtype_info._immediate_base_type);
  }

  void Set_immediate_base_type(TYPE_ID id) {
    _u._subtype_info._immediate_base_type = id.Value();
  }
};

typedef ARENA<sizeof(SYM_DATA), 4, false>      MAIN_TAB;
typedef ARENA<sizeof(PREG_DATA), 4, false>     PREG_TAB;
typedef ARENA<sizeof(BLOCK_DATA), 4, false>    BLOCK_TAB;
typedef ARENA<2, 4, true>                      STR_TAB;
typedef ARENA<4, 8, true>                      TYPE_TAB;
typedef ARENA<4, 8, true>                      CONSTANT_TAB;
typedef ARENA<sizeof(PARAM_DATA), 4, false>    PARAM_TAB;
typedef ARENA<sizeof(FILE_DATA), 8, false>     FILE_TAB;
typedef ARENA<sizeof(LABEL_DATA), 4, false>    LABEL_TAB;
typedef ARENA<4, 8, true>                      AUX_TAB_BASE;
typedef ARENA<sizeof(FIELD_DATA), 4, false>    FIELD_TAB;
typedef ARENA<sizeof(FUNC_DEF_DATA), 8, false> FUNC_DEF_TAB;
typedef ARENA<sizeof(ARB_DATA), 8, false>      ARB_TAB;
typedef ARENA<sizeof(ATTR_DATA), 4, false>     ATTR_TAB;

}  // namespace base
}  // namespace air

#endif
