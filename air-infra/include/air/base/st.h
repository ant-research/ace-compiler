//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_ST_H
#define AIR_BASE_ST_H

#include <unordered_map>

#include "air/base/container_decl.h"
#include "air/base/st_attr.h"
#include "air/base/st_const.h"
#include "air/base/st_decl.h"
#include "air/base/st_misc.h"
#include "air/base/st_sym.h"
#include "air/base/st_trait.h"
#include "air/base/st_type.h"
#include "air/base/targ_info.h"

namespace air {
namespace base {

template <typename I, typename O>
I Gen_id(PTR_FROM_DATA<O> ptr, uint32_t scope) {
  return I(ptr.Id().Value(), scope);
}

class AUX_TAB : public AUX_TAB_BASE {
public:
  AUX_TAB(ARENA_ALLOCATOR* allocator, uint32_t level, bool open);

  uint32_t     Scope_level() const { return _scope; }
  AUX_DATA_PTR Aux_entry(AUX_ID id) const;

  template <AUX_KIND T>
  AUX_DATA_PTR New_aux_entry(AUX_ID* id);

  template <typename T, AUX_KIND K>
  AUX_DATA_PTR New_aux_entry(T& head, AUX_ID* id,
                             AUX_DATA_PTR last = AUX_DATA_PTR());

private:
  uint32_t _scope;
};

template <typename T, AUX_KIND K>
AUX_DATA_PTR AUX_TAB::New_aux_entry(T& head, AUX_ID* id, AUX_DATA_PTR last) {
  AIR_ASSERT(&head.Aux_table() == this);
  typedef typename AUX_DATA_TRAITS<K>::AUX_DATA_TYPE AUX_DATA_TYPE;

  AUX_TAB& at = const_cast<AUX_TAB&>(*this);

  PTR_FROM_DATA<AUX_DATA_TYPE> ptr = at.template Allocate<AUX_DATA_TYPE>();
  ::new (ptr) AUX_DATA_TYPE();

  *id = Gen_id<AUX_ID, AUX_DATA_TYPE>(ptr, Scope_level());

  // short for Link_aux_entry
  head.Set_first_aux_entry(*id);

  return ptr;
}

class BLOCK {
  friend class GLOB_SCOPE;
  friend class FUNC_SCOPE;
  friend class AUX_TAB;
  friend class FUNC;
  PTR_FRIENDS(BLOCK);

public:
  BLOCK() : _glob(0), _blk() {}
  BLOCK(const GLOB_SCOPE& glob, BLOCK_ID id);
  BLOCK(const FUNC_SCOPE& func, BLOCK_ID id);

  BLOCK_KIND  Kind() const { return _blk->Kind(); }
  FUNC_ID     Owning_func_id() const;
  FUNC_PTR    Owning_func() const;
  BLOCK_ID    Parent_block_id() const;
  BLOCK_PTR   Parent_block() const;
  GLOB_SCOPE& Glob_scope() const { return *_glob; }
  BLOCK_ID    Id() const;
  STR_ID      Owning_func_name_id() const;
  STR_PTR     Owning_func_name() const;

  bool Is_func() const { return Kind() == BLOCK_KIND::FUNC; }
  bool Is_inner() const { return Kind() == BLOCK_KIND::INNER_BLK; }
  bool Is_named_scope() const { return Kind() == BLOCK_KIND::NAMED_SCOPE; }
  bool Is_comp_env() const { return Kind() == BLOCK_KIND::COMP_ENV; }

  void Set_parent_block(BLOCK_ID id);

private:
  BLOCK(const GLOB_SCOPE& glob, BLOCK_DATA_PTR ptr)
      : _blk(ptr), _glob(const_cast<GLOB_SCOPE*>(&glob)) {}

  bool Is_null() const { return (_blk == BLOCK_DATA_PTR()); }
  void Set_owning_func(FUNC_ID id);
  void Set_first_aux_entry(AUX_ID id);
  void Add_aux_entry(AUX_DATA_PTR aux, AUX_ID id);

  BLOCK_DATA_PTR Data() const { return _blk; }
  AUX_TAB&       Aux_table() const;
  AUX_ID         First_aux_entry_id() const;
  AUX_DATA_PTR   First_aux_entry() const;

  BLOCK_DATA_PTR _blk;
  GLOB_SCOPE*    _glob;
};

//! Base class for global & local symbol tables
class SCOPE_BASE {
  friend class CONTAINER;
  friend class ATTR;
  friend class IR_WRITE;
  friend class IR_READ;

public:
  bool Is_glob() const { return _kind == SCOPE_KIND::GLOB; }
  bool Is_func() const { return _kind == SCOPE_KIND::FUNC; }
  bool Is_opened() const { return _ref_count != 0; }

  GLOB_SCOPE&       Cast_to_glob() const;
  const FUNC_SCOPE& Cast_to_func() const;
  ARENA_ALLOCATOR&  Mem_pool() const { return *_mem_pool; }
  uint32_t          Scope_level() const;
  ATTR_PTR          Attr(ATTR_ID id) const;
  ATTR_PTR          New_attr();

  GLOB_SCOPE& Glob_scope() const;

  bool operator==(const SCOPE_BASE& o) const;

protected:
  enum class SCOPE_KIND { GLOB, FUNC };

  SCOPE_BASE(SCOPE_KIND kind, uint32_t level, bool open);
  ~SCOPE_BASE(){};

  AUX_DATA_PTR Aux_data(AUX_ID id);
  AUX_TAB&     Aux_table() const { return *_aux_tab; }
  MAIN_TAB&    Main_table() const { return *_main_tab; }
  ATTR_TAB&    Attr_table() const { return *_attr_tab; }

  bool Open();
  bool Close();

protected:
  SCOPE_KIND       _kind;
  MAIN_TAB*        _main_tab;
  AUX_TAB*         _aux_tab;
  ATTR_TAB*        _attr_tab;
  int32_t          _ref_count;
  ARENA_ALLOCATOR* _mem_pool;
};

//! Local symbol tables
class FUNC_SCOPE : public SCOPE_BASE {
  friend class CONTAINER;
  friend class GLOB_SCOPE;
  friend class SYM;
  friend class TYPE;
  friend class CONSTANT;
  friend class LABEL;
  friend class REGION_INFO;
  template <class V, class I>
  friend class TAB_ITER_BASE;

public:
  ~FUNC_SCOPE();

  GLOB_SCOPE& Glob_scope() const { return *_glob; }
  FUNC_SCOPE* Parent_func_scope() const { return _parent; }
  FUNC_ID     Owning_func_id() const { return _func_id; }
  FUNC_PTR    Owning_func() const;
  const char* Owning_func_entry_id_str() const;
  FUNC_ID     Id() const { return _func_id; }
  uint32_t    Scope_level() const;
  CONTAINER&  Container() const;
  STMT_ID     Entry_stmt_id() const;
  uint32_t    New_annot_id();
  PREG_TAB&   Preg_table() const { return *_preg_tab; }

  //! New function local variable, name in char*
  ADDR_DATUM_PTR New_var(CONST_TYPE_PTR type, const char* name,
                         const SPOS& spos);
  //! New function local variable, name in STR_PTR
  ADDR_DATUM_PTR New_var(CONST_TYPE_PTR type, CONST_STR_PTR name,
                         const SPOS& spos);
  //! New PREG of type
  PREG_PTR New_preg(CONST_TYPE_PTR type) { return New_preg(type->Id()); }
  PREG_PTR New_preg(TYPE_ID type) { return New_preg(type, SYM_ID()); }
  //! New PREG of type with home location symbol
  PREG_PTR New_preg(CONST_TYPE_PTR type, CONST_SYM_PTR home) {
    return New_preg(type->Id(), home->Id());
  }
  PREG_PTR        New_preg(TYPE_ID type, SYM_ID home);
  REGION_INFO_PTR New_region_info(REGION_INFO_KIND);
  LABEL_PTR       New_label();
  LABEL_PTR       New_label(STR_ID name);

  ADDR_DATUM_PTR Addr_datum(ADDR_DATUM_ID id) const;
  //! Get i's formal parameter of the function
  ADDR_DATUM_PTR Formal(uint32_t idx) const;
  PACKET_PTR     Packet(PACKET_ID id) const;
  LABEL_PTR      Label(LABEL_ID id) const;
  SYM_PTR        Sym(SYM_ID id) const;
  SYM_PTR        Find_sym(SYM_ID id) const;
  PREG_PTR       Preg(PREG_ID id) const;

  DATUM_ITER  Begin_addr_datum() const;
  DATUM_ITER  End_addr_datum() const;
  VAR_ITER    Begin_var() const;
  VAR_ITER    End_var() const;
  FORMAL_ITER Begin_formal() const;
  FORMAL_ITER End_formal() const;
  PREG_ITER   Begin_preg() const;
  PREG_ITER   End_preg() const;

  //! @brief Clone function local tables only, cloning a function scope
  //! assumes it is defined and IRs will be generated for it
  void Clone(FUNC_SCOPE& func);
  //! @brief Only clone attr tab
  void Clone_attr(FUNC_SCOPE& func);

  void Set_entry_stmt(STMT_PTR stmt);
  void Set_entry_stmt(STMT_ID id);

  void        Print(std::ostream& os, bool rot = true) const;
  void        Print() const;
  std::string To_str(bool rot = true) const;

private:
  FUNC_SCOPE(const GLOB_SCOPE& glob, FUNC_ID id, bool open);
  FUNC_SCOPE(const FUNC_SCOPE& parent, FUNC_ID id, bool open);

  FUNC_SCOPE(const FUNC_SCOPE& scope);
  FUNC_SCOPE& operator=(FUNC_SCOPE& scope);

  PRIM_ID_ITER Begin(SYM_ID) const { return Main_table().Begin(); }

  ADDR_DATUM_PTR New_var(TYPE_ID type, STR_ID name, const SPOS& spos);
  ADDR_DATUM_PTR New_formal(TYPE_ID type, STR_ID name, const SPOS& spos);

  FUNC_ID     _func_id;
  PREG_TAB*   _preg_tab;
  LABEL_TAB*  _label_tab;
  FUNC_SCOPE* _parent;
  GLOB_SCOPE* _glob;
  CONTAINER*  _cont;
};

//! Global symbol tables
class GLOB_SCOPE : public SCOPE_BASE {
  friend class SYM;
  friend class BLOCK;
  template <class V, class I>
  friend class TAB_ITER_BASE;

public:
  ~GLOB_SCOPE();
  GLOB_SCOPE(uint32_t id, bool open);
  uint32_t     Id() const { return _id; }
  STR_PTR      String(STR_ID id) const;
  TYPE_PTR     Type(TYPE_ID id) const;
  FUNC_PTR     Func(FUNC_ID id) const;
  BLOCK_PTR    Block(BLOCK_ID id) const;
  FIELD_PTR    Field(FIELD_ID id) const;
  FILE_PTR     File(FILE_ID id) const;
  PARAM_PTR    Param(PARAM_ID id) const;
  SYM_PTR      Sym(SYM_ID id) const;
  PACKET_PTR   Packet(PACKET_ID id) const;
  ENTRY_PTR    Entry_point(ENTRY_ID id) const;
  CONSTANT_PTR Constant(CONSTANT_ID id) const;
  ARB_PTR      Arb(ARB_ID id) const;
  STR_PTR      Undefined_name() { return String(_undefined_name_id); }
  STR_ID       Undefined_name_id() const { return _undefined_name_id; }
  BLOCK_ID     Comp_env_id() const { return _comp_env_id; }
  BLOCK_PTR    Comp_env() const;
  SPOS         Unknown_simple_spos() const { return SPOS(); }

  //! New entry point, name in char*
  ENTRY_PTR New_entry_point(CONST_TYPE_PTR type, CONST_FUNC_PTR func,
                            const char* name, const SPOS& spos) {
    return New_entry_point(type, func, New_str(name), spos);
  }
  //! New entry point, name in STR_PTR
  ENTRY_PTR New_entry_point(CONST_TYPE_PTR type, CONST_FUNC_PTR func,
                            CONST_STR_PTR name, const SPOS& spos);
  ENTRY_PTR New_entry_point(TYPE_ID type, FUNC_ID func, STR_ID name,
                            const SPOS& spos);
  //! New global entry point, name in char*
  ENTRY_PTR New_global_entry_point(CONST_TYPE_PTR type, CONST_FUNC_PTR func,
                                   const char* name, const SPOS& spos) {
    return New_global_entry_point(type, func, New_str(name), spos);
  }
  //! New global entry point, name in STR_PTR
  ENTRY_PTR New_global_entry_point(CONST_TYPE_PTR type, CONST_FUNC_PTR func,
                                   CONST_STR_PTR name, const SPOS& spos);

  //! New function, name in char*
  FUNC_PTR New_func(const char* name, const SPOS& spos) {
    return New_func(New_str(name), spos);
  }
  //! New function, name in STR_PTR
  FUNC_PTR New_func(CONST_STR_PTR name, const SPOS& spos);
  FUNC_PTR New_func(STR_ID name, const SPOS& spos);

  BLOCK_PTR New_named_scope(CONST_STR_PTR name);
  BLOCK_PTR New_unnamed_scope(CONST_BLOCK_PTR parent);
  //! New integer constant
  CONSTANT_PTR New_const(CONSTANT_KIND ck, CONST_TYPE_PTR type, int64_t val);
  //! New unsigned integer constant
  CONSTANT_PTR New_const(CONSTANT_KIND ck, CONST_TYPE_PTR type, uint64_t val);
  CONSTANT_PTR New_const(CONSTANT_KIND ck);
  CONSTANT_PTR New_const(CONSTANT_KIND ck, CONST_CONSTANT_PTR base,
                         int64_t idx_or_ofst);
  //! New array constant
  CONSTANT_PTR New_const(CONSTANT_KIND ck, CONST_TYPE_PTR type, void* buf,
                         size_t byte);
  CONSTANT_PTR New_const(CONSTANT_KIND ck, STR_ID str);
  CONSTANT_PTR New_const(CONSTANT_KIND ck, const char* str, size_t len);
  //! New float constant
  CONSTANT_PTR New_const(CONSTANT_KIND ck, CONST_TYPE_PTR type,
                         long double val);
  //! New constant to read from external file
  CONSTANT_PTR New_const(CONSTANT_KIND ck, CONST_TYPE_PTR type,
                         CONST_FILE_PTR file, uint64_t ofst, uint64_t sz);
  //! New constant to write to external file
  CONSTANT_PTR New_const(CONSTANT_KIND ck, CONST_TYPE_PTR type,
                         CONST_FILE_PTR file, void* buf, uint64_t sz);

  RECORD_TYPE_PTR  Rec_type(TYPE_ID id) const;
  POINTER_TYPE_PTR Ptr_type(CONST_TYPE_PTR domain, POINTER_KIND kind);
  POINTER_TYPE_PTR Ptr_type(TYPE_ID domain, POINTER_KIND kind);
  ADDR_DATUM_PTR   Addr_datum(ADDR_DATUM_ID id) const;

  //! New global variable, name in char*
  ADDR_DATUM_PTR New_var(CONST_TYPE_PTR type, const char* name,
                         const SPOS& spos) {
    return New_var(type, New_str(name), spos);
  }
  //! New global variable, name in STR_PTR
  ADDR_DATUM_PTR New_var(CONST_TYPE_PTR type, CONST_STR_PTR name,
                         const SPOS& spos);
  ADDR_DATUM_PTR New_var(TYPE_ID type, STR_ID name, const SPOS& spos);

  static GLOB_SCOPE* Get() {
    static GLOB_SCOPE* glob = 0;
    if (glob == 0) glob = new GLOB_SCOPE(0, true);
    return glob;
  }

  //! Get primitive type pointer from enum
  PRIM_TYPE_PTR Prim_type(PRIMITIVE_TYPE ptype);
  //! Get primitive type pointer from enum with name in char*
  PRIM_TYPE_PTR Prim_type(PRIMITIVE_TYPE ptype, const char* name) {
    return Prim_type(ptype, New_str(name));
  }
  //! Get primitive type pointer from enum with name in STR_PTR
  PRIM_TYPE_PTR Prim_type(PRIMITIVE_TYPE ptype, CONST_STR_PTR name) {
    return Prim_type(ptype, name->Id());
  }
  PRIM_TYPE_PTR Prim_type(PRIMITIVE_TYPE ptype, STR_ID name);
  PRIM_TYPE_PTR New_prim_type(PRIMITIVE_TYPE ptype, STR_ID name);

  //! Get boolean type
  PRIM_TYPE_PTR Bool_type();

  //! New pointer type with pointed-to type
  POINTER_TYPE_PTR New_ptr_type(TYPE_PTR pt_type, POINTER_KIND kind) {
    return New_ptr_type(pt_type->Id(), kind);
  }
  //! New pointer type with pointed-to type
  POINTER_TYPE_PTR New_ptr_type(TYPE_ID pt_type, POINTER_KIND kind) {
    return New_ptr_type(pt_type, kind, _undefined_name_id);
  }
  //! New pointer type with pointed-to type & name in char*
  POINTER_TYPE_PTR New_ptr_type(TYPE_PTR pt_type, POINTER_KIND kind,
                                const char* name) {
    return New_ptr_type(pt_type, kind, New_str(name));
  }
  //! New pointer type with pointed-to type & name in STR_PTR
  POINTER_TYPE_PTR New_ptr_type(TYPE_PTR pt_type, POINTER_KIND kind,
                                CONST_STR_PTR name) {
    return New_ptr_type(pt_type->Id(), kind, name->Id());
  }
  POINTER_TYPE_PTR New_ptr_type(TYPE_ID pt_type, POINTER_KIND kind,
                                STR_ID name);

  //! New an arb entry, its field must be set before used
  ARB_PTR New_arb();
  //! @brief New an arb entry with constant bounds and stride,
  //! dimension starts from zero
  ARB_PTR New_arb(uint32_t dim, int64_t lb, int64_t ub, int64_t stride);

  //! New array type, name in char*
  ARRAY_TYPE_PTR New_arr_type(const char* name, CONST_TYPE_PTR etype,
                              CONST_ARB_PTR arb, const SPOS& spos) {
    return New_arr_type(New_str(name), etype, arb, spos);
  }
  //! New array type, name in STR_PTR
  ARRAY_TYPE_PTR New_arr_type(CONST_STR_PTR name, CONST_TYPE_PTR etype,
                              CONST_ARB_PTR arb, const SPOS& spos);
  ARRAY_TYPE_PTR New_arr_type(STR_ID name, TYPE_ID etype, ARB_ID arb,
                              const SPOS& spos);
  //! New array type with constant upper bounds of dimensions, name in char*
  ARRAY_TYPE_PTR New_arr_type(const char* name, CONST_TYPE_PTR etype,
                              const std::vector<int64_t>& dim,
                              const SPOS&                 spos) {
    return New_arr_type(New_str(name), etype, dim, spos);
  }
  //! New array type with constant upper bounds of dimensions, name in STR_PTR
  ARRAY_TYPE_PTR New_arr_type(CONST_STR_PTR name, CONST_TYPE_PTR etype,
                              const std::vector<int64_t>& dim,
                              const SPOS&                 spos);

  //! New record type, name in char*
  RECORD_TYPE_PTR New_rec_type(RECORD_KIND k, const char* name,
                               const SPOS& spos) {
    return New_rec_type(k, New_str(name), spos);
  }
  //! New record type, name in STR_PTR
  RECORD_TYPE_PTR New_rec_type(RECORD_KIND k, CONST_STR_PTR name,
                               const SPOS& spos);
  RECORD_TYPE_PTR New_rec_type(RECORD_KIND k, STR_ID name, const SPOS& spos);

  //! New signature type
  SIGNATURE_TYPE_PTR New_sig_type();

  //! New subtype, name in char*
  SUBTYPE_PTR New_subtype(const char* name, CONST_TYPE_PTR base,
                          const SPOS& spos) {
    return New_subtype(New_str(name), base, spos);
  }
  //! New subtype, name in STR_PTR
  SUBTYPE_PTR New_subtype(CONST_STR_PTR name, CONST_TYPE_PTR base,
                          const SPOS& spos);

  //! New struct/class/union field, name in char*
  FIELD_PTR New_fld(const char* name, CONST_TYPE_PTR type,
                    CONST_TYPE_PTR record, const SPOS& spos) {
    return New_fld(name, type, record, type->Bit_size(), type->Alignment(),
                   spos);
  }
  FIELD_PTR New_fld(const char* name, CONST_TYPE_PTR type,
                    CONST_TYPE_PTR record, uint64_t fld_sz,
                    DATA_ALIGN fld_align, const SPOS& spos) {
    return New_fld(New_str(name), type, record, fld_sz, fld_align, spos);
  }
  //! New struct/class/union field, name in STR_PTR
  FIELD_PTR New_fld(CONST_STR_PTR name, CONST_TYPE_PTR type,
                    CONST_TYPE_PTR record, const SPOS& spos) {
    return New_fld(name, type, record, type->Bit_size(), type->Alignment(),
                   spos);
  }
  FIELD_PTR New_fld(CONST_STR_PTR name, CONST_TYPE_PTR type,
                    CONST_TYPE_PTR record, uint64_t fld_sz,
                    DATA_ALIGN fld_align, const SPOS& spos);
  FIELD_PTR New_fld(STR_ID name, TYPE_ID type, TYPE_ID record, uint64_t fld_sz,
                    DATA_ALIGN fld_align, const SPOS& spos);

  //! New parameter for function signature, name in char*
  PARAM_PTR New_param(const char* name, CONST_TYPE_PTR type, CONST_TYPE_PTR sig,
                      const SPOS& spos) {
    return New_param(New_str(name), type, sig, spos);
  }
  //! New parameter for function signature, name in STR_PTR
  PARAM_PTR New_param(CONST_STR_PTR name, CONST_TYPE_PTR type,
                      CONST_TYPE_PTR sig, const SPOS& spos);
  PARAM_PTR New_param(STR_ID name, TYPE_ID type, TYPE_ID sig, const SPOS& spos);
  //! New return parameter for function signature
  PARAM_PTR New_ret_param(CONST_TYPE_PTR type, CONST_TYPE_PTR sig);
  PARAM_PTR New_ret_param(TYPE_ID type, TYPE_ID sig);
  FILE_PTR  New_file(const char* name, LANG lang) {
    return New_file(New_str(name), lang);
  }
  FILE_PTR New_file(CONST_STR_PTR name, LANG lang) {
    return New_file(name->Id(), lang);
  }
  //! New string literal
  STR_PTR New_str(const char* str, size_t len = 0);

  FUNC_SCOPE& New_func_scope(FUNC_PTR func, bool open = true);
  FUNC_SCOPE& New_func_scope(FUNC_ID     func,
                             FUNC_DEF_ID def  = (FUNC_DEF_ID)Null_st_id,
                             bool        open = true);
  FUNC_SCOPE& Open_func_scope(FUNC_ID id);

  // Create an array type given an element type & count
  TYPE_PTR Array_type_ptr(TYPE_PTR etype, size_t count);

  TYPE_TAB&     Type_table() const { return *_type_tab; }
  STR_TAB&      Str_table() const { return *_str_tab; }
  BLOCK_TAB&    Blk_table() const { return *_blk_tab; }
  FILE_TAB&     File_table() const { return *_file_tab; }
  PARAM_TAB&    Param_table() const { return *_param_tab; }
  FIELD_TAB&    Field_table() const { return *_fld_tab; }
  FUNC_DEF_TAB& Func_def_table() const { return *_func_def_tab; }
  CONSTANT_TAB& Const_table() const { return *_const_tab; }
  ARB_TAB&      Arb_table() const { return *_arb_tab; }
  TARG_INFO*    Targ_info() const { return _targ_info; }

  // Add write checker in all iterator
  FILE_ITER     Begin_file() const;
  FILE_ITER     End_file() const;
  STR_ITER      Begin_str() const;
  STR_ITER      End_str() const;
  TYPE_ITER     Begin_type() const;
  TYPE_ITER     End_type() const;
  CONSTANT_ITER Begin_const() const;
  CONSTANT_ITER End_const() const;
  FUNC_ITER     Begin_func() const;
  FUNC_ITER     End_func() const;
  ARB_ITER      Begin_arb() const;
  ARB_ITER      End_arb() const;

  //! Clone global tables, set all functions to undefined
  //! if function scopes are cloned as well
  void Clone(GLOB_SCOPE& glob, bool clone_func_scope = true);
  void Delete_sym(SYM_PTR);

  void Init_targ_info(ENDIANNESS e, ARCHITECTURE a);

  class FUNC_SCOPE_ITER;
  FUNC_SCOPE_ITER Begin_func_scope() const;
  FUNC_SCOPE_ITER End_func_scope() const;

  void Print_ir(std::ostream& os, bool rot = true) const;
  void Print(std::ostream& os, bool rot = true) const;
  void Print() const;

private:
  PRIM_ID_ITER Begin(SYM_ID) const { return Main_table().Begin(); }
  PRIM_ID_ITER Begin(BLOCK_ID) const { return Blk_table().Begin(); }

  template <SYMBOL_CLASS K>
  PTR<typename SYM_TYPE_TRAITS<K>::SYM_TYPE> New_sym(SYMBOL_CLASS k);

  template <TYPE_TRAIT T>
  PTR<typename TYPE_DATA_TRAITS<T>::TYPE_TYPE> New_type();

  FUNC_DEF_ID  New_func_def_data();
  CONSTANT_PTR New_const(CONSTANT_KIND ck, TYPE_ID type, int64_t val);
  CONSTANT_PTR New_const(CONSTANT_KIND ck, TYPE_ID type, uint64_t val);
  CONSTANT_PTR New_const(CONSTANT_KIND ck, CONSTANT_ID base,
                         int64_t idx_or_ofst);
  CONSTANT_PTR New_const(CONSTANT_KIND ck, TYPE_ID type, long double val);
  FILE_PTR     New_file(STR_ID name, LANG lang);

  TYPE_TAB*     _type_tab;
  CONSTANT_TAB* _const_tab;
  PARAM_TAB*    _param_tab;
  STR_TAB*      _str_tab;
  FILE_TAB*     _file_tab;
  BLOCK_TAB*    _blk_tab;
  FIELD_TAB*    _fld_tab;
  FUNC_DEF_TAB* _func_def_tab;
  ARB_TAB*      _arb_tab;
  TARG_INFO*    _targ_info;

  uint32_t _id;
  BLOCK_ID _comp_env_id;
  STR_ID   _undefined_name_id;
  TYPE_ID* _prim_type_tab;

  typedef std::map<FUNC_ID, FUNC_SCOPE*> FUNC_SCOPE_MAP;
  FUNC_SCOPE_MAP                         _func_scope_map;

  typedef std::map<TYPE_ID, TYPE_ID> PTR_TYPE_MAP;
  PTR_TYPE_MAP* _ptr_type_map[static_cast<uint32_t>(POINTER_KIND::END)];

  typedef std::map<uint32_t, EXT_CONST_FILE*> ECF_MAP;
  ECF_MAP                                     _ecf_map;
};

template <SYMBOL_CLASS T>
PTR<typename SYM_TYPE_TRAITS<T>::SYM_TYPE> GLOB_SCOPE::New_sym(SYMBOL_CLASS k) {
  AIR_ASSERT(k != SYMBOL_CLASS::UNKNOWN && k != SYMBOL_CLASS::ADDR_DATUM);
  MAIN_TAB&    mt  = Main_table();
  SYM_DATA_PTR ptr = mt.template Allocate<SYM_DATA>();
  ::new (ptr) SYM_DATA(k);

  typedef typename SYM_TYPE_TRAITS<T>::SYM_ID_TYPE ID_TYPE;
  ID_TYPE id = Gen_id<ID_TYPE, SYM_DATA>(ptr, 0);

  typedef typename SYM_TYPE_TRAITS<T>::SYM_TYPE SYM_TYPE;

  AUX_ID aux;
  SYM    sym(this, ptr);
  sym.New_first_aux_entry(&aux);
  PTR<SYM_TYPE>& derived = sym.template Cast_to<T>();
  return derived;
}

template <TYPE_TRAIT T>
PTR<typename TYPE_DATA_TRAITS<T>::TYPE_TYPE> GLOB_SCOPE::New_type() {
  typedef typename TYPE_DATA_TRAITS<T>::TYPE_TYPE      TYPE_TYPE;
  typedef typename TYPE_DATA_TRAITS<T>::TYPE_DATA_TYPE TYPE_DATA_TYPE;

  PTR_FROM_DATA<TYPE_DATA_TYPE> ptr =
      _type_tab->template Allocate<TYPE_DATA_TYPE>();
  ::new (ptr) TYPE_DATA_TYPE();
  return TYPE(this, ptr).template Cast_to<T>();
}

}  // namespace base
}  // namespace air

#include "air/base/st_iter.h"

#endif
