//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_ST_SYM_H
#define AIR_BASE_ST_SYM_H

#include <cstdio>
#include <string>

#include "air/base/st_data.h"
#include "air/base/st_decl.h"
#include "air/base/st_trait.h"

namespace air {
namespace base {

class SYM {
  friend class AUX_DATA_ITR;
  friend class AUX_TAB;
  friend class GLOB_SCOPE;
  friend class FUNC_SCOPE;
  friend class FUNC_ENTRY_ITR;
  PTR_FRIENDS(SYM);

public:
  SYM() : _sym(), _scope(0) {}
  SYM(const GLOB_SCOPE& glob, SYM_ID sym);
  SYM(const FUNC_SCOPE& func, SYM_ID sym);

  SYMBOL_CLASS Kind() const { return _sym->Kind(); }

  SYM_ID      Id() const;
  SPOS        Spos() const;
  STR_ID      Name_id() const;
  STR_PTR     Name() const;
  GLOB_SCOPE& Glob_scope() const;
  FUNC_SCOPE* Defining_func_scope() const;
  uint32_t    Scope_level() const;

  bool Is_var() const { return Kind() == SYMBOL_CLASS::VAR; }
  bool Is_formal() const { return Kind() == SYMBOL_CLASS::FORMAL; }
  bool Is_addr_datum() const { return Is_var() || Is_formal(); }
  bool Is_static_fld() const;
  bool Is_entry() const { return Kind() == SYMBOL_CLASS::ENTRY; }
  bool Is_thunk() const { return Kind() == SYMBOL_CLASS::THUNK; }
  bool Is_func() const { return Kind() == SYMBOL_CLASS::FUNC; }
  bool Is_packet() const { return Kind() == SYMBOL_CLASS::PACKET; }

  CONST_ADDR_DATUM_PTR& Cast_to_addr_datum() const;
  CONST_ENTRY_PTR&      Cast_to_entry() const;
  CONST_FUNC_PTR&       Cast_to_func() const;

  ADDR_DATUM_PTR& Cast_to_addr_datum();
  ENTRY_PTR&      Cast_to_entry();
  FUNC_PTR&       Cast_to_func();

  template <SYMBOL_CLASS T>
  PTR_TO_CONST<typename SYM_TYPE_TRAITS<T>::SYM_TYPE>& Cast_to() const {
    typedef typename SYM_TYPE_TRAITS<T>::SYM_TYPE TARGET_SYM_TYPE;
    typedef PTR_TO_CONST<TARGET_SYM_TYPE>         TARGET_SYM_PTR_TYPE;

    AIR_ASSERT(
        Is_null() || (Kind() == T) ||
        ((T == SYMBOL_CLASS::ADDR_DATUM) &&
         ((Kind() == SYMBOL_CLASS::VAR) || (Kind() == SYMBOL_CLASS::FORMAL))));
    Requires<CHECK_SIZE_EQUAL<SYM, TARGET_SYM_PTR_TYPE> >();

    return reinterpret_cast<TARGET_SYM_PTR_TYPE&>(const_cast<SYM&>(*this));
  }

  template <SYMBOL_CLASS T>
  PTR<typename SYM_TYPE_TRAITS<T>::SYM_TYPE>& Cast_to() {
    typedef typename SYM_TYPE_TRAITS<T>::SYM_TYPE TARGET_SYM_TYPE;
    typedef PTR<TARGET_SYM_TYPE>                  TARGET_SYM_PTR_TYPE;

    AIR_ASSERT(
        Is_null() || (Kind() == T) ||
        ((T == SYMBOL_CLASS::ADDR_DATUM) &&
         ((Kind() == SYMBOL_CLASS::VAR) || (Kind() == SYMBOL_CLASS::FORMAL))));
    Requires<CHECK_SIZE_EQUAL<SYM, TARGET_SYM_PTR_TYPE> >();

    return reinterpret_cast<TARGET_SYM_PTR_TYPE&>(*this);
  }

  void Set_name(STR_PTR name) { Set_name(name->Id()); }
  void Rename(STR_ID name);

  const char* Sym_class_name() const {
    return Symbol_class_name_arr[static_cast<int>(Kind())];
  }

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

  typedef SYM_ID ID_TYPE;
  typedef SYM    BASE_TYPE;

protected:
  SYM(SCOPE_BASE* scope, SYM_DATA_PTR ptr);

  SYM_DATA_PTR      Data() const { return _sym; }
  AUX_DATA_PTR      First_aux_entry() const;
  AUX_ID            First_aux_entry_id() const;
  AUX_TAB&          Aux_table() const;
  AUX_DATA_PTR      New_first_aux_entry(AUX_ID* id);
  AUX_SYM_DATA_PTR  Fixed_aux_entry() const;
  FUNC_DEF_ID       New_func_def_data();
  FUNC_DEF_DATA_PTR Func_def_data(FUNC_DEF_ID id) const;

  bool Is_null() const { return _sym == SYM_DATA_PTR(); }

  template <SYMBOL_CLASS T>
  typename SYM_TYPE_TRAITS<T>::SYM_ID_TYPE Sym_id() const {
    return typename SYM_TYPE_TRAITS<T>::SYM_ID_TYPE(SYM::Id().Value());
  }

  void Set_spos(const SPOS& s);
  void Set_name(STR_ID id);
  void Set(const GLOB_SCOPE& scope, SYM_ID id);
  void Set(const FUNC_SCOPE& scope, SYM_ID id);
  void Set(SYM_ID id);
  void Set_null();
  void Set_first_aux_entry(AUX_ID id) { _sym->Set_first_aux_entry(id); }

  template <AUX_KIND T>
  AUX_DATA_PTR New_aux_entry(AUX_ID* id, AUX_DATA_PTR last = AUX_DATA_PTR());

  static const char* Symbol_class_name_arr[static_cast<int>(SYMBOL_CLASS::END)];

  SCOPE_BASE*  _scope;
  SYM_DATA_PTR _sym;
};

class ADDR_DATUM : public SYM {
  friend class GLOB_SCOPE;
  friend class FUNC_SCOPE;
  PTR_FRIENDS(ADDR_DATUM);

public:
  ADDR_DATUM() : SYM() {}
  ADDR_DATUM(const GLOB_SCOPE& glob, ADDR_DATUM_ID id);
  ADDR_DATUM(const FUNC_SCOPE& func, ADDR_DATUM_ID id);

  bool Is_static() const;
  bool Is_initialized() const { return _sym->Is_initialized(); }
  bool Is_const_initialized() const {
    return _sym->Is_initialized() && _sym->Is_const_var();
  }
  bool Is_in_packet() const { return _sym->Is_in_packet(); }
  bool Is_this() const;
  bool Is_static_fld() const;
  bool Is_local_static() const;
  bool Is_addr_passed() const { return _sym->Is_addr_passed(); }
  bool Is_addr_saved() const { return _sym->Is_addr_saved(); }
  bool Is_const_var() const { return _sym->Is_const_var(); }
  bool Is_used() const { return _sym->Is_used(); }
  bool Is_accessed_by_nested_func() const;
  bool Is_modified() const { return _sym->Is_modified(); }
  bool Has_implicit_ref() const;

  ADDR_DATUM_ID Id() const;
  TYPE_ID       Type_id() const;
  TYPE_PTR      Type() const;
  SYM_ID        Base_sym_id() const;
  SYM_PTR       Base_sym() const;
  uint64_t      Bit_sz() const;
  uint64_t      Byte_sz() const;
  FIELD_ID      Field_id() const;
  FIELD_PTR     Field() const;
  PACKET_ID     Packet_id() const;
  PACKET_PTR    Packet() const;

  void Set_type(CONST_TYPE_PTR type);
  void Set_type(TYPE_ID type);
  void Set_addr_passed() { _sym->Set_addr_passed(true); }
  void Set_addr_not_passed() { _sym->Set_addr_passed(false); }
  void Set_addr_saved() { _sym->Set_addr_saved(true); }
  void Set_addr_not_saved() { _sym->Set_addr_saved(false); }
  void Set_const_var() { _sym->Set_const_var(true); }
  void Set_not_const_var() { _sym->Set_const_var(false); }
  void Set_used() { _sym->Set_used(true); }
  void Set_not_used() { _sym->Set_used(false); }
  void Set_modified() { _sym->Set_modified(true); }
  void Set_not_modified() { _sym->Set_modified(false); }
  void Set_this();
  void Set_implicit_ref();

  typedef ADDR_DATUM_ID ID_TYPE;
  typedef SYM           BASE_TYPE;
};

class FUNC : public SYM {
  friend class CONTAINER;
  friend class GLOB_SCOPE;
  PTR_FRIENDS(FUNC)

public:
  FUNC() : SYM() {}
  FUNC(const GLOB_SCOPE& glob, FUNC_ID id);
  FUNC(const FUNC_SCOPE& func, FUNC_ID id);

  FUNC_ID   Id() const { return Sym_id<SYMBOL_CLASS::FUNC>(); }
  SPOS      Begin_spos() const;
  SPOS      End_spos() const;
  uint32_t  Nesting_level() const { return _sym->Func_nesting_level(); }
  BLOCK_ID  Func_block_id() const { return _sym->Func_block_id(); }
  BLOCK_PTR Func_block() const;
  BLOCK_ID  Parent_block_id() const;
  BLOCK_PTR Parent_block() const;
  FUNC_ID   Parent_func_id() const;
  STMT_ID   Entry_stmt_id() const;
  ENTRY_ID  Entry_point_id() const;
  ENTRY_PTR Entry_point() const;
  ENTRY_ID  First_entry_point_id() const;
  uint32_t  New_annot_id();

  FUNC_DEF_DATA_PTR Func_def_data() const;

  bool Is_defined() const { return _sym->Is_func_defined(); }
  bool Has_nested_func() const;
  bool Is_nested_func() const;
  bool Is_static_member_func() const;
  bool Is_parentless() const;

  void Set_parent(CONST_BLOCK_PTR p);
  void Set_parent(BLOCK_ID id);
  void Set_begin_spos(const SPOS& spos);
  void Set_end_spos(const SPOS& spos);
  void Set_entry_stmt(STMT_ID node_id);
  void Set_nesting_level(uint32_t l) { _sym->Set_func_nesting_level(l); }
  void Set_block(BLOCK_ID id) { _sym->Set_func_block_id(id); }
  void Set_first_entry_point(ENTRY_ID id);
  void Set_defined(FUNC_DEF_ID id = (FUNC_DEF_ID)Null_st_id);
  void Set_undefined();

  void Add_entry_point(ENTRY_ID id);

  typedef FUNC_ID IDTYPE;
  typedef SYM     BASE_TYPE;
};

class PACKET : public SYM {
  PTR_FRIENDS(PACKET);

public:
  PACKET() : SYM() {}
  PACKET(const GLOB_SCOPE& glob, PACKET_ID id);
  PACKET(const FUNC_SCOPE& func, PACKET_ID id);

  PACKET_ID Id() const;

  typedef PACKET_ID ID_TYPE;
  typedef SYM       BASE_TYPE;
};

class ENTRY : public SYM {
  friend class GLOB_SCOPE;
  PTR_FRIENDS(ENTRY);

public:
  ENTRY() : SYM() {}
  ENTRY(const GLOB_SCOPE& glob, ENTRY_ID id);
  ENTRY(const FUNC_SCOPE& func, ENTRY_ID id);

  bool Is_program_entry() const;
  bool Is_internal() const;
  bool Has_retv() const;
  bool Is_callable() const;

  ENTRY_ID Id() const { return Sym_id<SYMBOL_CLASS::ENTRY>(); }
  TYPE_ID  Type_id() const;
  TYPE_PTR Type() const;
  FUNC_ID  Owning_func_id() const;
  FUNC_PTR Owning_func() const;
  ENTRY_ID Next() const;

  const char* Id_str() const;

  void Set_program_entry();
  void Set_internal();
  void Set_owning_func(CONST_FUNC_PTR func);
  void Set_owning_func(FUNC_ID func);
  void Set_callable();
  void Set_type(TYPE_ID id) { _sym->Set_entry_type(id); }
  void Set_next(ENTRY_ID id);

  STR_PTR Add_sym_id(STR_ID sym);

  typedef ENTRY_ID ID_TYPE;
  typedef SYM      BASE_TYPE;
};

class PREG {
  friend class FUNC_SCOPE;
  PTR_FRIENDS(PREG);

public:
  PREG() : _func(0), _preg() {}
  PREG(const FUNC_SCOPE& func, PREG_ID id);

  FUNC_SCOPE* Defining_func_scope() const { return _func; }
  uint32_t    Scope_level() const;
  PREG_ID     Id() const;
  TYPE_ID     Type_id() const;
  TYPE_PTR    Type() const;
  SYM_ID      Home_sym_id() const;
  SYM_PTR     Home_sym() const;

  bool Is_null() const { return (_preg == PREG_DATA_PTR()); }

  void Set_home_sym(SYM_ID home);
  void Set_type(TYPE_ID type) { _preg->Set_type(type); }

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

private:
  PREG(FUNC_SCOPE* func, PREG_DATA_PTR ptr) : _func(func), _preg(ptr) {}

  FUNC_SCOPE*   _func;
  PREG_DATA_PTR _preg;
};

}  // namespace base
}  // namespace air

#endif
