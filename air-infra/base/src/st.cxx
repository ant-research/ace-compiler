//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/base/st.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <sstream>

#include "air/base/container.h"
#include "air/base/st_sym.h"
#include "air/util/debug.h"

namespace air {
namespace base {

//=============================================================================
// class AUX_TAB member functions
//=============================================================================

AUX_TAB::AUX_TAB(ARENA_ALLOCATOR* allocator, uint32_t level, bool open)
    : _scope(level),
      AUX_TAB_BASE(allocator, (level == 0) ? AK_GLOB_AUX : AK_FUNC_AUX,
                   (level == 0) ? "global_aux_tab" : "local_aux_tab", true) {}

AUX_DATA_PTR
AUX_TAB::Aux_entry(AUX_ID id) const {
  return (id == AUX_ID(Null_st_id))
             ? AUX_DATA_PTR()
             : this->Find<AUX_DATA>(ID<AUX_DATA>(id.Index()));
}

//=============================================================================
// class BLOCK member functions
//=============================================================================

BLOCK::BLOCK(const GLOB_SCOPE& glob, BLOCK_ID id)
    : _glob(const_cast<GLOB_SCOPE*>(&glob)),
      _blk(glob.Blk_table().Find(ID<BLOCK_DATA>(id.Index()))) {
  AIR_ASSERT(id.Scope() == 0);
}

BLOCK::BLOCK(const FUNC_SCOPE& func, BLOCK_ID id) {
  AIR_ASSERT(id.Scope() == 0);
  _glob = const_cast<GLOB_SCOPE*>(&func.Glob_scope());
  _blk  = _glob->Blk_table().Find(ID<BLOCK_DATA>(id.Index()));
}

BLOCK_ID
BLOCK::Id() const {
  return Is_null() ? BLOCK_ID(Null_st_id) : BLOCK_ID(_blk.Id().Value(), 0);
}

FUNC_ID
BLOCK::Owning_func_id() const {
  AIR_ASSERT(Kind() == BLOCK_KIND::FUNC);
  return _blk->Owning_func_id();
}

FUNC_PTR
BLOCK::Owning_func() const {
  AIR_ASSERT(_glob);
  return _glob->Func(Owning_func_id());
}

void BLOCK::Set_owning_func(FUNC_ID id) {
  AIR_ASSERT(Kind() == BLOCK_KIND::FUNC);
  _blk->Set_owning_func(id);
}

STR_ID
BLOCK::Owning_func_name_id() const { return Owning_func()->Name_id(); }

STR_PTR
BLOCK::Owning_func_name() const { return Owning_func()->Name(); }

BLOCK_ID
BLOCK::Parent_block_id() const { return _blk->Parent(); }

BLOCK_PTR
BLOCK::Parent_block() const {
  AIR_ASSERT(_glob);
  return _glob->Block(Parent_block_id());
}

void BLOCK::Set_parent_block(BLOCK_ID id) { _blk->Set_parent(id); }

//=============================================================================
// class SCOPE_BASE member functions
//=============================================================================

SCOPE_BASE::SCOPE_BASE(SCOPE_KIND kind, uint32_t lvl, bool open)
    : _kind(kind), _ref_count(open ? 1 : 0) {
  _mem_pool = new ARENA_ALLOCATOR;
  _attr_tab = new ATTR_TAB(_mem_pool, AK_ATTR, "attr_tab", true);
  _aux_tab  = new AUX_TAB(_mem_pool, lvl, true);
  _main_tab = new MAIN_TAB(_mem_pool, lvl == 0 ? AK_GLOB_SYM : AK_FUNC_SYM,
                           lvl == 0 ? "global_sym_tab" : "local_sym_tab", true);
}

SCOPE_BASE::~SCOPE_BASE() {
  if (_main_tab != nullptr) {
    delete _main_tab;
    _main_tab = nullptr;
  }
  if (_aux_tab != nullptr) {
    delete _aux_tab;
    _aux_tab = nullptr;
  }
  if (_attr_tab != nullptr) {
    delete _attr_tab;
    _attr_tab = nullptr;
  }
  if (_mem_pool != nullptr) {
    delete _mem_pool;
    _mem_pool = nullptr;
  }
}

GLOB_SCOPE& SCOPE_BASE::Cast_to_glob() const {
  AIR_ASSERT(Is_glob());
  return static_cast<GLOB_SCOPE&>(const_cast<SCOPE_BASE&>(*this));
}

const FUNC_SCOPE& SCOPE_BASE::Cast_to_func() const {
  AIR_ASSERT(Is_func());
  return static_cast<const FUNC_SCOPE&>(*this);
}

uint32_t SCOPE_BASE::Scope_level() const {
  return Is_glob() ? 0 : Cast_to_func().Scope_level();
}

ATTR_PTR SCOPE_BASE::Attr(ATTR_ID id) const {
  return ATTR_PTR(ATTR(*this, id));
}

ATTR_PTR SCOPE_BASE::New_attr() {
  PTR_FROM_DATA<ATTR_DATA> data = _attr_tab->Allocate<ATTR_DATA>();
  new (data) ATTR_DATA();
  return ATTR_PTR(ATTR(*this, data));
}

GLOB_SCOPE& SCOPE_BASE::Glob_scope() const {
  if (Is_glob()) {
    return Cast_to_glob();
  } else {
    return Cast_to_func().Glob_scope();
  }
}

//=============================================================================
// class FUNC_SCOPE member functions
//=============================================================================

FUNC_SCOPE::FUNC_SCOPE(const GLOB_SCOPE& glob, FUNC_ID id, bool open)
    : SCOPE_BASE(SCOPE_KIND::FUNC, glob.Func(id)->Nesting_level() + 1, open),
      _glob(const_cast<GLOB_SCOPE*>(&glob)),
      _func_id(id),
      _parent(0) {
  _preg_tab  = new PREG_TAB(_mem_pool, AK_PREG, "preg_tab", true);
  _label_tab = new LABEL_TAB(_mem_pool, AK_LABEL, "label_tab", true);
  _cont      = CONTAINER::New(this, open);
}

FUNC_SCOPE::~FUNC_SCOPE() {
  if (_cont != nullptr) {
    _cont->Delete();
    _cont = nullptr;
  }
  if (_preg_tab != nullptr) {
    delete _preg_tab;
    _preg_tab = nullptr;
  }
  if (_label_tab != nullptr) {
    delete _label_tab;
    _label_tab = nullptr;
  }
}

FUNC_PTR
FUNC_SCOPE::Owning_func() const { return Glob_scope().Func(Owning_func_id()); }

STMT_ID FUNC_SCOPE::Entry_stmt_id() const {
  FUNC_PTR ptr = Glob_scope().Func(Owning_func_id());
  return ptr->Entry_stmt_id();
}

uint32_t FUNC_SCOPE::New_annot_id() {
  FUNC_PTR func = Glob_scope().Func(Owning_func_id());
  return func->New_annot_id();
}

void FUNC_SCOPE::Set_entry_stmt(STMT_PTR stmt) { Set_entry_stmt(stmt->Id()); }

void FUNC_SCOPE::Set_entry_stmt(STMT_ID id) {
  FUNC_PTR ptr = Glob_scope().Func(Owning_func_id());
  ptr->Set_entry_stmt(id);
}

uint32_t FUNC_SCOPE::Scope_level() const {
  FUNC_PTR func = _glob->Func(_func_id);
  return func->Nesting_level() + 1;
}

ADDR_DATUM_PTR
FUNC_SCOPE::New_var(CONST_TYPE_PTR type, const char* name, const SPOS& spos) {
  return New_var(type, _glob->New_str(name), spos);
}

ADDR_DATUM_PTR
FUNC_SCOPE::New_var(CONST_TYPE_PTR type, CONST_STR_PTR name, const SPOS& spos) {
  return New_var(type->Id(), (name == Null_ptr) ? STR_ID() : name->Id(), spos);
}

ADDR_DATUM_PTR
FUNC_SCOPE::New_var(TYPE_ID type, STR_ID name, const SPOS& spos) {
  MAIN_TAB&    mt  = Main_table();
  SYM_DATA_PTR ptr = mt.Allocate<SYM_DATA>();
  ::new (ptr) SYM_DATA(SYMBOL_CLASS::VAR);

  AUX_ID           aux_id;
  SYM              sym(this, ptr);
  ADDR_DATUM_PTR&  var = sym.Cast_to<SYMBOL_CLASS::VAR>();
  AUX_SYM_DATA_PTR aux_sym =
      Static_cast<AUX_SYM_DATA_PTR>(var->New_first_aux_entry(&aux_id));
  aux_sym->Init_datum_flag();
  var->Set_type(type);
  var->Set_name(name);
  var->Set_spos(spos);
  return var;
}

ADDR_DATUM_PTR
FUNC_SCOPE::New_formal(TYPE_ID type, STR_ID name, const SPOS& spos) {
  MAIN_TAB&    mt  = Main_table();
  SYM_DATA_PTR ptr = mt.Allocate<SYM_DATA>();
  ::new (ptr) SYM_DATA(SYMBOL_CLASS::FORMAL);

  AUX_ID           aux_id;
  SYM              sym(this, ptr);
  ADDR_DATUM_PTR&  formal = sym.Cast_to<SYMBOL_CLASS::FORMAL>();
  AUX_SYM_DATA_PTR aux_sym =
      Static_cast<AUX_SYM_DATA_PTR>(formal->New_first_aux_entry(&aux_id));
  aux_sym->Init_datum_flag();

  formal->Set_type(type);
  formal->Set_name(name);
  formal->Set_spos(spos);
  return formal;
}

ADDR_DATUM_PTR FUNC_SCOPE::New_formal(TYPE_ID type, const char* name,
                                      const SPOS& spos) {
  return New_formal(type, _glob->New_str(name)->Id(), spos);
}

ADDR_DATUM_PTR FUNC_SCOPE::New_formal(TYPE_PTR type, STR_PTR name,
                                      const SPOS& spos) {
  return New_formal(type->Id(), name->Id(), spos);
}

PREG_PTR
FUNC_SCOPE::New_preg(TYPE_ID type, SYM_ID home) {
  PREG_DATA_PTR ptr = Preg_table().Allocate<PREG_DATA>();
  new (ptr) PREG_DATA(type, home);
  return PREG_PTR(PREG(this, ptr));
}

CONTAINER& FUNC_SCOPE::Container() const {
  AIR_ASSERT(_cont);
  return *_cont;
}

ADDR_DATUM_PTR
FUNC_SCOPE::Addr_datum(ADDR_DATUM_ID id) const {
  return ADDR_DATUM_PTR(ADDR_DATUM(*this, id));
}

ADDR_DATUM_PTR
FUNC_SCOPE::Formal(uint32_t idx) const {
  AIR_ASSERT(Owning_func()->Is_defined());
  NODE_PTR func_entry_node = _cont->Entry_stmt()->Node();
  NODE_PTR formal          = func_entry_node->Child(idx);
  return formal->Addr_datum();
}

uint32_t FUNC_SCOPE::Formal_cnt(void) const {
  AIR_ASSERT(Owning_func()->Is_defined());
  NODE_PTR func_entry_node = _cont->Entry_stmt()->Node();
  uint32_t child_cnt       = func_entry_node->Num_child();
  // last child of entry node is function body
  AIR_ASSERT(child_cnt >= 1);
  return child_cnt - 1;
}

SYM_PTR
FUNC_SCOPE::Sym(SYM_ID id) const { return SYM_PTR(SYM(*this, id)); }

SYM_PTR
FUNC_SCOPE::Find_sym(SYM_ID id) const {
  AIR_ASSERT(id.Is_local());
  if (id.Scope() < Scope_level()) {
    return Parent_func_scope()->Find_sym(id);
  }
  void* p = Main_table().Find(id.Index());
  if (p) return Sym(id);
  return SYM_PTR();
}

PREG_PTR
FUNC_SCOPE::Preg(PREG_ID id) const { return PREG_PTR(PREG(*this, id)); }

PACKET_PTR
FUNC_SCOPE::Packet(PACKET_ID id) const { return PACKET_PTR(PACKET(*this, id)); }

DATUM_ITER
FUNC_SCOPE::Begin_addr_datum() const {
  return DATUM_ITER(*this, ADDR_DATUM_SEL());
}

DATUM_ITER
FUNC_SCOPE::End_addr_datum() const { return DATUM_ITER(); }

VAR_ITER
FUNC_SCOPE::Begin_var() const { return VAR_ITER(*this, VAR_SEL()); }

VAR_ITER
FUNC_SCOPE::End_var() const { return VAR_ITER(); }

FORMAL_ITER
FUNC_SCOPE::Begin_formal() const { return FORMAL_ITER(*this, FORMAL_SEL()); }

FORMAL_ITER
FUNC_SCOPE::End_formal() const { return FORMAL_ITER(); }

PREG_ITER
FUNC_SCOPE::Begin_preg() const { return PREG_ITER(*this); }

PREG_ITER
FUNC_SCOPE::End_preg() const { return PREG_ITER(); }

void FUNC_SCOPE::Clone(FUNC_SCOPE& func) {
  Main_table().Clone(func.Main_table());
  Aux_table().Clone(func.Aux_table());
  Attr_table().Clone(func.Attr_table());
  Preg_table().Clone(func.Preg_table());
}

void FUNC_SCOPE::Clone_attr(FUNC_SCOPE& func) {
  Attr_table().Clone(func.Attr_table());
}

void FUNC_SCOPE::Replace_code(CONTAINER* cntr) {
  AIR_ASSERT(cntr != nullptr);
  _cont->Delete();
  _cont = cntr;
}

size_t FUNC_SCOPE::Print_mp_info(std::ostream& os) {
  size_t total_size = 0;
  os << std::dec << "Func Scope \"" << Owning_func()->Name()->Char_str()
     << "\":\n";
  os << "  MAIN TABLE (" << _main_tab->Size() << " Items, "
     << _main_tab->Mem_size() << " Bytes)\n";
  total_size += _main_tab->Mem_size();

  os << "  AUX TABLE (" << _aux_tab->Size() << " Items, "
     << _aux_tab->Mem_size() << " Bytes)\n";
  total_size += _aux_tab->Mem_size();

  os << "  ATTR TABLE (" << _attr_tab->Size() << " Items, "
     << _attr_tab->Mem_size() << " Bytes)\n";
  total_size += _attr_tab->Mem_size();

  os << "  PREG TABLE (" << _preg_tab->Size() << " Items, "
     << _preg_tab->Mem_size() << " Bytes)\n";
  total_size += _preg_tab->Mem_size();

  os << "  LABEL TABLE (" << _label_tab->Size() << " Items, "
     << _label_tab->Mem_size() << " Bytes)\n";
  total_size += _label_tab->Mem_size();

  os << "  CODE ARENA (" << _cont->Code_arena()->Size() << " Items, "
     << _cont->Code_arena()->Mem_size() << " Bytes)\n";
  total_size += _cont->Code_arena()->Mem_size();

  os << "  Total: " << total_size << " Bytes\n\n";
  return total_size;
}

void FUNC_SCOPE::Print(std::ostream& os, bool rot) const {
  CONST_FUNC_PTR func = Owning_func();
  os << std::hex << std::showbase;
  os << "FUN[" << func->Id().Value() << "] \"" << func->Name()->Char_str()
     << "\"\n";

  FORMAL_ITER f_iter = Begin_formal();
  FORMAL_ITER f_end  = End_formal();
  for (; f_iter != f_end; ++f_iter) {
    (*f_iter)->Print(os, 1);
  }
  VAR_ITER v_iter = Begin_var();
  VAR_ITER v_end  = End_var();
  for (; v_iter != v_end; ++v_iter) {
    (*v_iter)->Print(os, 1);
  }
  PREG_ITER p_iter = Begin_preg();
  PREG_ITER p_end  = End_preg();
  for (; p_iter != p_end; ++p_iter) {
    (*p_iter)->Print(os, 1);
  }
  for (auto attr = Attr_table().Begin(); attr != Attr_table().End(); ++attr) {
    ATTR_PTR ptr = Attr(ATTR_ID(*attr));
    os << std::hex << std::showbase;
    os << "  ATTR[" << *attr << "] ";
    ptr->Print(os, 0);
    os << std::endl;
  }
  os << std::endl;

  if (_cont) {
    _cont->Print(os, rot, 1);
  }
}

void FUNC_SCOPE::Print() const { Print(std::cout, true); }

std::string FUNC_SCOPE::To_str(bool rot) const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, rot);
  return buf.str();
}

//=============================================================================
// class GLOB_SCOPE member functions
//=============================================================================

const char* Primitive_type_names[static_cast<uint32_t>(PRIMITIVE_TYPE::END)] = {
    "int8_t",       "int16_t",    "int32_t",     "int64_t",     "uint8_t",
    "uint16_t",     "uint32_t",   "uint64_t",    "float32_t",   "float64_t",
    "float80_t",    "float128_t", "complex32_t", "complex64_t", "complex80_t",
    "complex128_t", "void",       "bool"};

const char*
    Primitive_type_short_names[static_cast<uint32_t>(PRIMITIVE_TYPE::END)] = {
        "i8",  "i16", "i32",  "i64", "u8",  "u16", "u32",  "u64", "f32",
        "f64", "f80", "f128", "c32", "c64", "c80", "c128", "v",   "b"};

GLOB_SCOPE::GLOB_SCOPE(uint32_t id, bool open)
    : _id(id), SCOPE_BASE(SCOPE_KIND::GLOB, 0, true) {
  _str_tab   = new STR_TAB(_mem_pool, AK_STRING, "string_tab", true);
  _fld_tab   = new FIELD_TAB(_mem_pool, AK_FIELD, "field_tab", true);
  _arb_tab   = new ARB_TAB(_mem_pool, AK_ARB, "arb_tab", true);
  _type_tab  = new TYPE_TAB(_mem_pool, AK_TYPE, "type_tab", true);
  _blk_tab   = new BLOCK_TAB(_mem_pool, AK_BLOCK, "block_tab", true);
  _file_tab  = new FILE_TAB(_mem_pool, AK_FILE, "file_tab", true);
  _param_tab = new PARAM_TAB(_mem_pool, AK_PARAM, "param_tab", true);
  _func_def_tab =
      new FUNC_DEF_TAB(_mem_pool, AK_FUNC_DEF, "func_def_tab", true);
  _lit_tab   = new LITERAL_TAB(_mem_pool, AK_LITERAL, "literal_tab", true);
  _const_tab = new CONSTANT_TAB(_mem_pool, AK_CONSTANT, "constant_tab", true);

  BLOCK_TAB&     bt     = Blk_table();
  BLOCK_DATA_PTR ce_ptr = bt.Allocate<BLOCK_DATA>();
  ::new (ce_ptr) BLOCK_DATA(BLOCK_KIND::COMP_ENV);
  _comp_env_id = BLOCK_ID(ce_ptr.Id().Value(), 0);

  size_t prim_count = static_cast<size_t>(PRIMITIVE_TYPE::BOOL) + 1;
  size_t prim_tb_sz = prim_count * sizeof(TYPE_ID);
  _prim_type_tab    = (TYPE_ID*)_mem_pool->Allocate(prim_tb_sz);
  TYPE_ID* tptr     = _prim_type_tab;

  for (uint32_t i = 0; i < static_cast<uint32_t>(POINTER_KIND::END); i++) {
    _ptr_type_map[i] = new PTR_TYPE_MAP();
  }

  LITERAL_PTR undef_lit = New_literal("_undef");
  _undefined_lit_id     = undef_lit->Id();

  STR_PTR undef_name_str = New_str("_noname");
  _undefined_name_id     = undef_name_str->Id();

  for (int32_t i = 0; i < prim_count; i++, tptr++) {
    ::new (tptr) TYPE_ID();

    STR_PTR       name_str = New_str(Primitive_type_names[i]);
    PRIM_TYPE_PTR type     = New_prim_type((PRIMITIVE_TYPE)i, name_str->Id());
    _prim_type_tab[i]      = type->Id();
  }

  _targ_info = nullptr;
}

GLOB_SCOPE::~GLOB_SCOPE() {
  for (ECF_MAP::iterator iter = _ecf_map.begin(); iter != _ecf_map.end();
       ++iter) {
    EXT_CONST_FILE* ecf_ptr = (*iter).second;
    int             err     = close(ecf_ptr->Fd());
    AIR_ASSERT_MSG((err != -1), "%s close errno: %d\n",
                   File(FILE_ID((*iter).first))->File_name()->Char_str(),
                   errno);
    delete ecf_ptr;

    if (_targ_info != nullptr) {
      delete _targ_info;
      _targ_info = nullptr;
    }
  }

  for (FUNC_SCOPE_MAP::iterator iter = _func_scope_map.begin();
       iter != _func_scope_map.end(); ++iter) {
    FUNC_SCOPE* func = (*iter).second;
    if (func != nullptr) {
      func->~FUNC_SCOPE();
    }
  }

  for (uint32_t i = 0; i < static_cast<uint32_t>(POINTER_KIND::END); i++) {
    delete _ptr_type_map[i];
  }

  if (_func_def_tab != nullptr) {
    delete _func_def_tab;
    _func_def_tab = nullptr;
  }
  if (_param_tab != nullptr) {
    delete _param_tab;
    _param_tab = nullptr;
  }
  if (_file_tab != nullptr) {
    delete _file_tab;
    _file_tab = nullptr;
  }
  if (_blk_tab != nullptr) {
    delete _blk_tab;
    _blk_tab = nullptr;
  }
  if (_type_tab != nullptr) {
    delete _type_tab;
    _type_tab = nullptr;
  }
  if (_arb_tab != nullptr) {
    delete _arb_tab;
    _arb_tab = nullptr;
  }
  if (_fld_tab != nullptr) {
    delete _fld_tab;
    _fld_tab = nullptr;
  }
  if (_str_tab != nullptr) {
    delete _str_tab;
    _str_tab = nullptr;
  }
  if (_const_tab != nullptr) {
    delete _const_tab;
    _const_tab = nullptr;
  }
  if (_lit_tab != nullptr) {
    delete _lit_tab;
    _lit_tab = nullptr;
  }
}

FUNC_PTR
GLOB_SCOPE::Func(FUNC_ID id) const { return FUNC_PTR(FUNC(*this, id)); }

BLOCK_PTR
GLOB_SCOPE::Comp_env() const { return Block(Comp_env_id()); }

BLOCK_PTR
GLOB_SCOPE::Block(BLOCK_ID id) const { return BLOCK_PTR(BLOCK(*this, id)); }

FIELD_PTR
GLOB_SCOPE::Field(FIELD_ID id) const { return FIELD_PTR(FIELD(*this, id)); }

FILE_PTR
GLOB_SCOPE::File(FILE_ID id) const { return FILE_PTR(SRC_FILE(*this, id)); }

SYM_PTR
GLOB_SCOPE::Sym(SYM_ID id) const {
  AIR_ASSERT(!id.Is_local());
  return SYM_PTR(SYM(*this, id));
}

PACKET_PTR
GLOB_SCOPE::Packet(PACKET_ID id) const {
  AIR_ASSERT(!id.Is_local());
  return PACKET_PTR(PACKET(*this, id));
}

CONSTANT_PTR
GLOB_SCOPE::Constant(CONSTANT_ID id) const {
  return CONSTANT_PTR(CONSTANT(*this, id));
}

ARB_PTR GLOB_SCOPE::Arb(ARB_ID id) const { return ARB_PTR(ARB(*this, id)); }

STR_PTR
GLOB_SCOPE::New_str(const char* str) {
  AIR_ASSERT(str != nullptr);
  size_t            len  = strlen(str);
  STR_MAP::iterator iter = _str_map.find(str);
  if (iter != _str_map.end()) {
    STR_ID ret = iter->second;
    return String(ret);
  }

  PTR_FROM_DATA<char> ptr          = _str_tab->Allocate_array<char>(len + 1);
  STR_DATA*           str_data_ptr = (STR_DATA*)ptr.Addr();
  char* new_str = const_cast<char*>(str_data_ptr->Str());
  memcpy(new_str, str, len + 1);
  new_str[len] = '\0';

  STR_PTR str_ptr = STR_PTR(STR(*this, Reinterpret_cast<STR_DATA_PTR>(ptr)));
  _str_map[new_str]   = str_ptr->Id();
  return str_ptr;
}

LITERAL_PTR
GLOB_SCOPE::New_literal(const char* str, size_t len) {
  AIR_ASSERT(str != nullptr);
  if (len == 0) {
    len = strlen(str);
  }
  LIT_KEY           key(str, len);
  LIT_MAP::iterator iter = _lit_map.find(key);
  if (iter != _lit_map.end()) {
    LITERAL_ID ret = iter->second;
    return Literal(ret);
  }

  PTR_FROM_DATA<char> ptr = Reinterpret_cast<PTR_FROM_DATA<char> >(
      _lit_tab->Allocate_array<short>((sizeof(int) + len + 2) / 2));
  LITERAL_DATA* str_data_ptr = (LITERAL_DATA*)ptr.Addr();
  str_data_ptr->Set_len((uint32_t)len);
  char* new_data = const_cast<char*>(str_data_ptr->Str());
  memcpy(new_data, str, len);
  new_data[len] = '\0';

  LIT_KEY     new_key(new_data, len);
  LITERAL_PTR new_str =
      LITERAL_PTR(LITERAL(*this, Reinterpret_cast<LITERAL_DATA_PTR>(ptr)));
  _lit_map[new_key] = new_str->Id();

  return new_str;
}

PARAM_PTR
GLOB_SCOPE::Param(PARAM_ID id) const { return PARAM_PTR(PARAM(*this, id)); }

ARB_PTR
GLOB_SCOPE::New_arb() {
  PTR_FROM_DATA<ARB_DATA> data = _arb_tab->Allocate<ARB_DATA>();
  new (data) ARB_DATA();
  return ARB_PTR(ARB(this, data));
}

ARB_PTR
GLOB_SCOPE::New_arb(uint32_t dim, int64_t lb, int64_t ub, int64_t stride) {
  PTR_FROM_DATA<ARB_DATA> data = _arb_tab->Allocate<ARB_DATA>();
  new (data) ARB_DATA(dim, lb, ub, stride);
  return ARB_PTR(ARB(this, data));
}

ARRAY_TYPE_PTR
GLOB_SCOPE::New_arr_type(CONST_STR_PTR name, CONST_TYPE_PTR etype,
                         CONST_ARB_PTR arb, const SPOS& spos) {
  return New_arr_type(name == Null_ptr ? STR_ID() : name->Id(), etype->Id(),
                      arb->Id(), spos);
}

ARRAY_TYPE_PTR
GLOB_SCOPE::New_arr_type(STR_ID name, TYPE_ID etype, ARB_ID arb,
                         const SPOS& spos) {
  ARRAY_TYPE_PTR ret = New_type<TYPE_TRAIT::ARRAY>();
  ret->Set_name(name);
  ret->Set_elem_type(etype);
  ret->Set_first_dim(arb);
  ret->Set_spos(spos);
  return ret;
}

ARRAY_TYPE_PTR
GLOB_SCOPE::New_arr_type(CONST_STR_PTR name, CONST_TYPE_PTR etype,
                         const std::vector<int64_t>& dim, const SPOS& spos) {
  uint32_t dim_sz   = dim.size();
  ARB_PTR  head_arb = New_arb(0, 0, dim[0], 1);
  ARB_PTR  prev     = head_arb;
  for (uint32_t i = 1; i < dim_sz; i++) {
    ARB_PTR arb = New_arb(i, 0, dim[i], 1);
    prev->Set_next(arb->Id());
    prev = arb;
  }
  return New_arr_type(name, etype, head_arb, spos);
}

ARRAY_TYPE_PTR
GLOB_SCOPE::New_arr_type(CONST_TYPE_PTR etype, CONST_ARB_PTR arb,
                         const SPOS& spos) {
  const char* etype_name;
  if (etype->Is_prim()) {
    etype_name = Primitive_type_short_names[static_cast<uint32_t>(
        etype->Cast_to_prim()->Encoding())];
  } else {
    etype_name = etype->Name()->Char_str();
  }
  std::string buf(etype_name);
  ARB_ID      arb_id = arb->Id();
  do {
    ARB_PTR iter = Arb(arb_id);
    AIR_ASSERT(iter->Is_lb_const());
    AIR_ASSERT(iter->Is_ub_const());
    AIR_ASSERT(iter->Is_stride_const());
    uint32_t dim_sz = (iter->Ub_val() - iter->Lb_val()) / iter->Stride_val();
    if (arb_id == arb->Id()) {
      buf += "_";
    } else {
      buf += "x";
    }
    buf += std::to_string(dim_sz);
    arb_id = iter->Next();
  } while (arb_id != Null_id);
  return New_arr_type(New_str(buf.c_str()), etype, arb, spos);
}

ARRAY_TYPE_PTR
GLOB_SCOPE::New_arr_type(CONST_TYPE_PTR etype, const std::vector<int64_t>& dim,
                         const SPOS& spos) {
  const char* etype_name;
  if (etype->Is_prim()) {
    etype_name = Primitive_type_short_names[static_cast<uint32_t>(
        etype->Cast_to_prim()->Encoding())];
  } else {
    etype_name = etype->Name()->Char_str();
  }
  std::string buf(etype_name);
  uint32_t    dim_sz = dim.size();
  for (uint32_t i = 0; i < dim_sz; i++) {
    if (i == 0) {
      buf += "_";
    } else {
      buf += "x";
    }
    buf += std::to_string(dim[i]);
  }
  return New_arr_type(New_str(buf.c_str()), etype, dim, spos);
}

RECORD_TYPE_PTR
GLOB_SCOPE::New_rec_type(RECORD_KIND kind, CONST_STR_PTR name,
                         const SPOS& spos) {
  return New_rec_type(kind, name == Null_ptr ? STR_ID() : name->Id(), spos);
}

RECORD_TYPE_PTR
GLOB_SCOPE::New_rec_type(RECORD_KIND kind, STR_ID name, const SPOS& spos) {
  RECORD_TYPE_PTR ptr = New_type<TYPE_TRAIT::RECORD>();
  ptr->Set_kind(kind);
  ptr->Set_name(name);
  ptr->Set_spos(spos);
  return ptr;
}

SIGNATURE_TYPE_PTR
GLOB_SCOPE::New_sig_type() {
  SIGNATURE_TYPE_PTR ptr = New_type<TYPE_TRAIT::SIGNATURE>();
  ptr->Set_name(_undefined_name_id);
  return ptr;
}

FIELD_PTR
GLOB_SCOPE::New_fld(CONST_STR_PTR id, CONST_TYPE_PTR type,
                    CONST_TYPE_PTR record, uint64_t fld_sz,
                    DATA_ALIGN fld_align, const SPOS& spos) {
  return New_fld(id->Id(), type->Id(), record->Id(), fld_sz, fld_align, spos);
}

FIELD_PTR
GLOB_SCOPE::New_fld(STR_ID name, TYPE_ID type, TYPE_ID record, uint64_t fld_sz,
                    DATA_ALIGN fld_align, const SPOS& spos) {
  AIR_ASSERT(fld_align != DATA_ALIGN::BAD);
  FIELD_DATA_PTR ptr = _fld_tab->Allocate<FIELD_DATA>();
  new (ptr) FIELD_DATA(FIELD_KIND::REGULAR);
  FIELD fld(*this, ptr);
  fld.Set_type(type);
  fld.Set_owning_rec_type(record);
  fld.Set_spos(spos);
  fld.Set_name(name);
  TYPE_PTR fld_type = Type(type);
  if ((fld_type->Is_int()) && (fld_sz < fld_type->Bit_size())) {
    fld.Set_bit_fld();
    fld.Set_bit_fld_size(fld_sz);
  } else {
    AIR_ASSERT(fld_align >= fld_type->Alignment());
    // not a bit field, use type size as field size
  }
  fld.Set_alignment(fld_align);
  return FIELD_PTR(fld);
}

PARAM_PTR
GLOB_SCOPE::New_param(CONST_STR_PTR name, CONST_TYPE_PTR type,
                      CONST_TYPE_PTR sig, const SPOS& spos) {
  return New_param(name->Id(), type->Id(), sig->Id(), spos);
}

PARAM_PTR
GLOB_SCOPE::New_param(STR_ID name, TYPE_ID type, TYPE_ID sig,
                      const SPOS& spos) {
  PARAM_DATA_PTR ptr = _param_tab->Allocate<PARAM_DATA>();
  ::new (ptr) PARAM_DATA();
  PARAM p(*this, ptr);
  p.Set_name(name);
  p.Set_type(type);
  p.Set_owning_sig(sig);
  p.Set_spos(spos);
  return PARAM_PTR(p);
}

PARAM_PTR
GLOB_SCOPE::New_ret_param(CONST_TYPE_PTR type, CONST_TYPE_PTR sig) {
  return New_ret_param(type->Id(), sig->Id());
}

PARAM_PTR
GLOB_SCOPE::New_ret_param(TYPE_ID type, TYPE_ID sig) {
  PARAM_DATA_PTR ptr = _param_tab->Allocate<PARAM_DATA>();
  ::new (ptr) PARAM_DATA();
  PARAM p(*this, ptr);
  p.Set_ret();
  p.Set_owning_sig(sig);
  p.Set_type(type);
  return PARAM_PTR(p);
}

FILE_PTR
GLOB_SCOPE::New_file(STR_ID name, LANG lang) {
  uint64_t fname_key = (static_cast<uint64_t>(lang) << 32 | name.Value());
  AIR_ASSERT(_name_file_map.find(fname_key) == _name_file_map.end());
  FILE_DATA_PTR ptr = _file_tab->Allocate<FILE_DATA>();
  ::new (ptr) FILE_DATA(name, lang);
  FILE_PTR new_file = FILE_PTR(SRC_FILE(*this, ptr));
  if ((lang == LANG::RO_CONST) || (lang == LANG::WO_CONST)) {
    EXT_CONST_FILE* ecf_ptr = new EXT_CONST_FILE;
    const char*     fname   = String(name)->Char_str();
    int             flags   = 0;
    if (lang == LANG::RO_CONST) {
      flags = O_RDONLY;
    } else {
      flags = O_RDWR | O_CREAT;
    }
    int fd = open64(fname, flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    AIR_ASSERT_MSG((fd != -1), "%s open errno: %d\n", fname, errno);
    struct stat64 st;
    int           err = fstat64(fd, &st);
    AIR_ASSERT_MSG((err != -1), "%s fstat errno: %d\n", fname, errno);
    ecf_ptr->Set_fd(fd);
    if (lang == LANG::RO_CONST) {
      ecf_ptr->Set_size(st.st_size);
    } else {
      ecf_ptr->Set_ofst(0);
    }
    _ecf_map[new_file->Id().Value()] = ecf_ptr;
  }
  _name_file_map[fname_key] = new_file->Id();
  return new_file;
}

TYPE_PTR
GLOB_SCOPE::Type(TYPE_ID id) const { return TYPE_PTR(TYPE(*this, id)); }

RECORD_TYPE_PTR
GLOB_SCOPE::Rec_type(TYPE_ID id) const {
  return RECORD_TYPE_PTR(RECORD_TYPE(*this, id));
}

POINTER_TYPE_PTR
GLOB_SCOPE::Ptr_type(CONST_TYPE_PTR domain, POINTER_KIND kind) {
  return Ptr_type(domain->Id(), kind);
}

POINTER_TYPE_PTR
GLOB_SCOPE::Ptr_type(TYPE_ID domain, POINTER_KIND kind) {
  PTR_TYPE_MAP& ptm = *(_ptr_type_map[static_cast<uint32_t>(kind)]);
  TYPE_ID       id  = ptm[domain];
  if (id != TYPE_ID()) return Type(id)->Cast_to_ptr();

  POINTER_TYPE_PTR pt = New_ptr_type(domain, kind);
  id                  = pt->Id();
  ptm[domain]         = id;
  return pt;
}

ENTRY_PTR
GLOB_SCOPE::Entry_point(ENTRY_ID id) const {
  return ENTRY_PTR(ENTRY(*this, id));
}

LITERAL_PTR
GLOB_SCOPE::Literal(LITERAL_ID id) const {
  return LITERAL_PTR(LITERAL(*this, id));
}

STR_PTR
GLOB_SCOPE::String(STR_ID id) const { return STR_PTR(STR(*this, id)); }

PRIM_TYPE_PTR
GLOB_SCOPE::Prim_type(PRIMITIVE_TYPE ptype) {
  return Type(_prim_type_tab[static_cast<uint32_t>(ptype)])->Cast_to_prim();
}

PRIM_TYPE_PTR
GLOB_SCOPE::Prim_type(PRIMITIVE_TYPE ptype, STR_ID name) {
  AIR_ASSERT(ptype <= PRIMITIVE_TYPE::END);
  PRIM_TYPE_PTR ret = New_prim_type(ptype, name);
  return ret;
}

PRIM_TYPE_PTR
GLOB_SCOPE::New_prim_type(PRIMITIVE_TYPE ptype, STR_ID name) {
  PRIM_TYPE_PTR ret = New_type<TYPE_TRAIT::PRIMITIVE>();
  ret->Set_encoding(ptype);
  ret->Set_name(name);
  return ret;
}

POINTER_TYPE_PTR
GLOB_SCOPE::New_ptr_type(TYPE_ID type, POINTER_KIND kind, STR_ID name) {
  POINTER_TYPE_PTR ptr = New_type<TYPE_TRAIT::POINTER>();
  ptr->Set_domain_type(type);
  ptr->Set_ptr_kind(kind);
  ptr->Set_name(name);
  return ptr;
}

ADDR_DATUM_PTR
GLOB_SCOPE::Addr_datum(ADDR_DATUM_ID id) const {
  AIR_ASSERT(!id.Is_local());
  return ADDR_DATUM_PTR(ADDR_DATUM(*this, id));
}

ADDR_DATUM_PTR
GLOB_SCOPE::New_var(CONST_TYPE_PTR type, CONST_STR_PTR name, const SPOS& spos) {
  return New_var(type->Id(), name == Null_ptr ? STR_ID() : name->Id(), spos);
}

ADDR_DATUM_PTR
GLOB_SCOPE::New_var(TYPE_ID type, STR_ID name, const SPOS& spos) {
  ADDR_DATUM_PTR   var = New_sym<SYMBOL_CLASS::ADDR_DATUM>(SYMBOL_CLASS::VAR);
  AUX_SYM_DATA_PTR aux = var->Fixed_aux_entry();
  aux->Set_datum_owning_func(FUNC_ID());
  aux->Init_datum_flag();
  var->Set_type(type);
  var->Set_name(name);
  var->Set_spos(spos);
  return var;
}

FUNC_PTR
GLOB_SCOPE::New_func(CONST_STR_PTR name, const SPOS& spos) {
  return New_func((name == Null_ptr) ? STR_ID() : name->Id(), spos);
}

FUNC_DEF_ID
GLOB_SCOPE::New_func_def_data() {
  FUNC_DEF_DATA_PTR ptr = _func_def_tab->Allocate<FUNC_DEF_DATA>();
  ::new (ptr) FUNC_DEF_DATA();
  FUNC_DEF_ID id = Gen_id<FUNC_DEF_ID, FUNC_DEF_DATA>(ptr, 0);
  return id;
}

FUNC_PTR
GLOB_SCOPE::New_func(STR_ID name, const SPOS& spos) {
  FUNC_PTR func = New_sym<SYMBOL_CLASS::FUNC>(SYMBOL_CLASS::FUNC);
  func->Set_name(name);
  func->Set_spos(spos);

  AUX_SYM_DATA_PTR aux = func->Fixed_aux_entry();
  aux->Set_first_entry_point(ENTRY_ID());

  BLOCK_DATA_PTR ptr = _blk_tab->Allocate<BLOCK_DATA>();
  ::new (ptr) BLOCK_DATA(BLOCK_KIND::FUNC);
  BLOCK_ID blk_id = Gen_id<BLOCK_ID, BLOCK_DATA>(ptr, 0);
  func->Set_block(blk_id);
  BLOCK_PTR blk = func->Func_block();
  blk->Set_owning_func(func->Id());
  blk->Set_parent_block(Comp_env_id());

  return func;
}

CONSTANT_PTR
GLOB_SCOPE::New_const(CONSTANT_KIND ck, CONST_TYPE_PTR type, int64_t val) {
  return New_const(ck, type->Id(), val);
}

CONSTANT_PTR
GLOB_SCOPE::New_const(CONSTANT_KIND ck, TYPE_ID type, int64_t val) {
  CMPLR_ASSERT(false, "This API will be deleted!");
  AIR_ASSERT(!type.Is_null());
  CONSTANT_PTR new_const = New_const(ck);
  new_const->Set_type(type);
  new_const->Set_val(val);
  return new_const;
}

CONSTANT_PTR
GLOB_SCOPE::New_const(CONSTANT_KIND ck, CONST_TYPE_PTR type, uint64_t val) {
  return New_const(ck, type->Id(), val);
}

CONSTANT_PTR
GLOB_SCOPE::New_const(CONSTANT_KIND ck, TYPE_ID type, uint64_t val) {
  CMPLR_ASSERT(false, "This API will be deleted!");
  AIR_ASSERT(!type.Is_null());
  CONSTANT_PTR new_const = New_const(ck);
  new_const->Set_type(type);
  new_const->Set_val(val);
  return new_const;
}

CONSTANT_PTR
GLOB_SCOPE::New_const(CONSTANT_KIND ck) {
  CONSTANT_DATA_PTR ptr;
  switch (ck) {
    case CONSTANT_KIND::BOOLEAN: {
      PTR_FROM_DATA<BOOL_CONSTANT_DATA> data =
          _const_tab->Allocate<BOOL_CONSTANT_DATA>();
      new (data) BOOL_CONSTANT_DATA(ck);
      ptr = data;
      break;
    }

    case CONSTANT_KIND::SIGNED_INT:
    case CONSTANT_KIND::UNSIGNED_INT:
    case CONSTANT_KIND::PTR_FROM_UNSIGNED: {
      PTR_FROM_DATA<INT_CONSTANT_DATA> data =
          _const_tab->Allocate<INT_CONSTANT_DATA>();
      new (data) INT_CONSTANT_DATA(ck);
      ptr = data;
      break;
    }

    case CONSTANT_KIND::FLOAT: {
      PTR_FROM_DATA<FLOAT_CONSTANT_DATA> data =
          _const_tab->Allocate<FLOAT_CONSTANT_DATA>();
      new (data) FLOAT_CONSTANT_DATA(ck);
      ptr = data;
      break;
    }

    case CONSTANT_KIND::STR_ARRAY: {
      PTR_FROM_DATA<STR_CONSTANT_DATA> data =
          _const_tab->Allocate<STR_CONSTANT_DATA>();
      new (data) STR_CONSTANT_DATA(ck);
      ptr = data;
      break;
    }

    case CONSTANT_KIND::EXT_FILE: {
      PTR_FROM_DATA<EXTERN_CONSTANT_DATA> data =
          _const_tab->Allocate<EXTERN_CONSTANT_DATA>();
      new (data) EXTERN_CONSTANT_DATA(ck);
      ptr = data;
      break;
    }
    default:
      AIR_ASSERT(0);
  }

  AIR_ASSERT(ptr != CONSTANT_DATA_PTR());
  return CONSTANT_PTR(CONSTANT(this, ptr));
}

CONSTANT_PTR
GLOB_SCOPE::New_const(CONSTANT_KIND ck, CONST_CONSTANT_PTR base,
                      int64_t idx_or_ofst) {
  return New_const(ck, base->Id(), idx_or_ofst);
}

CONSTANT_PTR
GLOB_SCOPE::New_const(CONSTANT_KIND ck, CONSTANT_ID base, int64_t idx_or_ofst) {
  AIR_ASSERT((ck == CONSTANT_KIND::ARRAY_ELEM_PTR) ||
             (ck == CONSTANT_KIND::PTR_OFST));
  CONST_TYPE_PTR type;
  if (ck == CONSTANT_KIND::PTR_OFST) {
    type = Constant(base)->Type();
  } else {
    CONST_POINTER_TYPE_PTR btype =
        Constant(base)->Type()->Base_type()->Cast_to_ptr();
    CONST_TYPE_PTR etype = btype->Domain_type()->Base_type();
    type                 = Ptr_type(etype->Id(), btype->Ptr_kind());
  }

  CONSTANT_PTR new_const = New_const(ck);
  new_const->Set_type(type->Id());
  new_const->Set_val(base, idx_or_ofst);
  return new_const;
}

CONSTANT_PTR
GLOB_SCOPE::New_const(CONSTANT_KIND ck, CONST_TYPE_PTR type, void* buf,
                      size_t byte) {
  AIR_ASSERT(ck == CONSTANT_KIND::ARRAY && type->Is_array());
  AIR_ASSERT(type->Byte_size() == byte);
  size_t sz   = sizeof(ARRAY_CONSTANT_DATA) + byte;
  size_t unit = sz / _const_tab->Unit_size();
  if ((sz % _const_tab->Unit_size()) != 0) {
    unit += 1;
  }
  PTR_FROM_DATA<void> data = _const_tab->Malloc(unit);
  new (data) ARRAY_CONSTANT_DATA(ck, type->Id(), buf, byte);
  return CONSTANT_PTR(
      CONSTANT(this, Reinterpret_cast<CONSTANT_DATA_PTR>(data)));
}

CONSTANT_PTR
GLOB_SCOPE::New_const(CONSTANT_KIND ck, LITERAL_ID str) {
  CONSTANT_PTR new_const = New_const(ck);
  // assume type is always "char*"
  TYPE_PTR type = New_ptr_type(Prim_type(PRIMITIVE_TYPE::INT_S8)->Id(),
                               POINTER_KIND::FLAT32);
  new_const->Set_type(type->Id());
  new_const->Set_val(str);
  return new_const;
}

CONSTANT_PTR
GLOB_SCOPE::New_const(CONSTANT_KIND ck, const char* str, size_t len) {
  LITERAL_PTR str_ptr = New_literal(str, len);
  return New_const(ck, str_ptr->Id());
}

CONSTANT_PTR
GLOB_SCOPE::New_const(CONSTANT_KIND ck, CONST_TYPE_PTR type, long double val) {
  return New_const(ck, type->Id(), val);
}

CONSTANT_PTR
GLOB_SCOPE::New_const(CONSTANT_KIND ck, TYPE_ID type, long double val) {
  AIR_ASSERT(ck == CONSTANT_KIND::FLOAT);
  CONSTANT_PTR new_cst = New_const(ck);
  new_cst->Set_type(type);
  new_cst->Set_val(val);
  return new_cst;
}

CONSTANT_PTR
GLOB_SCOPE::New_const(CONSTANT_KIND ck, CONST_TYPE_PTR type, const char* fname,
                      void* buf, uint64_t sz) {
  AIR_ASSERT(fname != nullptr);
  STR_MAP::iterator siter = _str_map.find(fname);
  AIR_ASSERT(siter != _str_map.end());
  STR_ID fname_id = siter->second;
  AIR_ASSERT(!Is_null_id(fname_id));
  uint64_t fname_key =
      (static_cast<uint64_t>(LANG::WO_CONST) << 32) | fname_id.Value();
  NAME_FILE_MAP::iterator fiter = _name_file_map.find(fname_key);
  AIR_ASSERT(fiter != _name_file_map.end());
  FILE_PTR file = File(fiter->second);
  AIR_ASSERT(file != Null_ptr);
  return New_const(ck, type, file, buf, sz);
}

CONSTANT_PTR
GLOB_SCOPE::New_const(CONSTANT_KIND ck, CONST_TYPE_PTR type,
                      CONST_FILE_PTR file, void* buf, uint64_t sz) {
  AIR_ASSERT(ck == CONSTANT_KIND::EXT_FILE);
  AIR_ASSERT(type->Byte_size() == sz);
  CONSTANT_PTR new_cst = New_const(ck);
  new_cst->Set_type(type->Id());
  // Set constant offset in RDWR external file
  ECF_MAP::iterator ecf_iter = _ecf_map.find(file->Id().Value());
  AIR_ASSERT_MSG((ecf_iter != _ecf_map.end()), "%s not available\n",
                 file->File_name()->Char_str());
  EXT_CONST_FILE* ecf_ptr  = (*ecf_iter).second;
  uint64_t        old_ofst = ecf_ptr->Ofst();
  new_cst->Set_val(file->Id(), old_ofst, sz);
  // Update EXT_CONST_FILE data
  uint64_t new_ofst = old_ofst + sz;
  ecf_ptr->Set_ofst(new_ofst);
  ssize_t len = pwrite64(ecf_ptr->Fd(), buf, sz, old_ofst);
  AIR_ASSERT_MSG((len == sz), "%s write errno: %d\n",
                 file->File_name()->Char_str(), errno);
  return new_cst;
}

CONSTANT_PTR
GLOB_SCOPE::New_const(CONSTANT_KIND ck, CONST_TYPE_PTR type, const char* fname,
                      uint64_t ofst, uint64_t sz) {
  AIR_ASSERT(fname != nullptr);
  STR_MAP::iterator siter = _str_map.find(fname);
  AIR_ASSERT(siter != _str_map.end());
  STR_ID fname_id = siter->second;
  AIR_ASSERT(!Is_null_id(fname_id));
  uint64_t fname_key =
      (static_cast<uint64_t>(LANG::RO_CONST) << 32) | fname_id.Value();
  NAME_FILE_MAP::iterator fiter = _name_file_map.find(fname_key);
  AIR_ASSERT(fiter != _name_file_map.end());
  FILE_PTR file = File(fiter->second);
  AIR_ASSERT(file != Null_ptr);
  return New_const(ck, type, file, ofst, sz);
}

CONSTANT_PTR
GLOB_SCOPE::New_const(CONSTANT_KIND ck, CONST_TYPE_PTR type,
                      CONST_FILE_PTR file, uint64_t ofst, uint64_t sz) {
  AIR_ASSERT(ck == CONSTANT_KIND::EXT_FILE);
  AIR_ASSERT(type->Byte_size() == sz);
  // Delay ofst + sz < file.size check until the const is read
  CONSTANT_PTR new_cst = New_const(ck);
  new_cst->Set_type(type->Id());
  new_cst->Set_val(file->Id(), ofst, sz);
  return new_cst;
}

ENTRY_PTR
GLOB_SCOPE::New_entry_point(CONST_TYPE_PTR type, CONST_FUNC_PTR func,
                            CONST_STR_PTR name, const SPOS& spos) {
  return New_entry_point(type->Id(), func->Id(),
                         (name == Null_ptr) ? STR_ID() : name->Id(), spos);
}

ENTRY_PTR
GLOB_SCOPE::New_entry_point(TYPE_ID type, FUNC_ID func, STR_ID name,
                            const SPOS& spos) {
  ENTRY_PTR entry = New_sym<SYMBOL_CLASS::ENTRY>(SYMBOL_CLASS::ENTRY);
  entry->Set_type(type);
  entry->Set_owning_func(func);
  entry->Set_name(name);
  entry->Set_spos(spos);

  AUX_SYM_DATA_PTR aux = entry->Fixed_aux_entry();
  Func(func)->Add_entry_point(entry->Id());

  return entry;
}

ENTRY_PTR
GLOB_SCOPE::New_global_entry_point(CONST_TYPE_PTR type, CONST_FUNC_PTR func,
                                   CONST_STR_PTR name, const SPOS& spos) {
  ENTRY_PTR entry = New_entry_point(type, func, name, spos);
  // entry->Add_linker_id(name->Id());
  return entry;
}

FUNC_SCOPE& GLOB_SCOPE::New_func_scope(FUNC_PTR func, bool open) {
  return New_func_scope(func->Id(), (FUNC_DEF_ID)Null_st_id, open);
}

FUNC_SCOPE& GLOB_SCOPE::New_func_scope(FUNC_ID func, FUNC_DEF_ID def,
                                       bool open) {
  FUNC_PTR    func_ptr = Func(func);
  FUNC_SCOPE* new_func_scope;

  if (!func_ptr->Is_nested_func()) {
    new_func_scope = (FUNC_SCOPE*)Mem_pool().Allocate(sizeof(FUNC_SCOPE));
    ::new (new_func_scope) FUNC_SCOPE(*this, func, open);
  } else {
    AIR_ASSERT(0);
    // TODO
  }

  _func_scope_map[func] = new_func_scope;
  if (def == (FUNC_DEF_ID)Null_st_id) {
    func_ptr->Set_defined(def);
  }

  return *new_func_scope;
}

void GLOB_SCOPE::Delete_func_scope(FUNC_SCOPE* func) {
  _func_scope_map.erase(func->Id());
  func->~FUNC_SCOPE();
}

FUNC_SCOPE& GLOB_SCOPE::Open_func_scope(FUNC_ID id) {
  FUNC_SCOPE_MAP::iterator map_iter = _func_scope_map.find(id);
  AIR_ASSERT(map_iter != _func_scope_map.end());
  FUNC_SCOPE* scope = (*map_iter).second;
  AIR_ASSERT(scope);
  return *scope;
}

LITERAL_ITER
GLOB_SCOPE::Begin_lit() const { return LITERAL_ITER(*this); }

LITERAL_ITER
GLOB_SCOPE::End_lit() const { return LITERAL_ITER(); }

STR_ITER
GLOB_SCOPE::Begin_str() const { return STR_ITER(*this); }

STR_ITER
GLOB_SCOPE::End_str() const { return STR_ITER(); }

TYPE_ITER
GLOB_SCOPE::Begin_type() const { return TYPE_ITER(*this); }

TYPE_ITER
GLOB_SCOPE::End_type() const { return TYPE_ITER(); }

CONSTANT_ITER
GLOB_SCOPE::Begin_const() const { return CONSTANT_ITER(*this); }

CONSTANT_ITER
GLOB_SCOPE::End_const() const { return CONSTANT_ITER(); }

FUNC_ITER
GLOB_SCOPE::Begin_func() const { return FUNC_ITER(*this, FUNC_SEL()); }

FUNC_ITER
GLOB_SCOPE::End_func() const { return FUNC_ITER(); }

ARB_ITER
GLOB_SCOPE::Begin_arb() const { return ARB_ITER(*this); }

ARB_ITER
GLOB_SCOPE::End_arb() const { return ARB_ITER(); }

GLOB_SCOPE::FUNC_SCOPE_ITER GLOB_SCOPE::Begin_func_scope() const {
  return GLOB_SCOPE::FUNC_SCOPE_ITER(*this);
}

GLOB_SCOPE::FUNC_SCOPE_ITER GLOB_SCOPE::End_func_scope() const {
  return GLOB_SCOPE::FUNC_SCOPE_ITER();
}

void GLOB_SCOPE::Clone(GLOB_SCOPE& glob, bool clone_func_scope) {
  // Null-terminated string
  Str_table().Clone(glob.Str_table());
  // Type
  Type_table().Clone(glob.Type_table());
  Arb_table().Clone(glob.Arb_table());
  Field_table().Clone(glob.Field_table());
  Param_table().Clone(glob.Param_table());
  // Global symbol
  Main_table().Clone(glob.Main_table());
  Aux_table().Clone(glob.Aux_table());
  // None null-terminated string
  Lit_table().Clone(glob.Lit_table());
  // Constant
  Const_table().Clone(glob.Const_table());
  // Attribute
  Attr_table().Clone(glob.Attr_table());
  // File
  File_table().Clone(glob.File_table());

  if (clone_func_scope) {
    // Function definition
    Func_def_table().Clone(glob.Func_def_table());
    Blk_table().Clone(glob.Blk_table());
  }

  // Set all functions of this global scope undefined
  FUNC_ITER iter = Begin_func();
  FUNC_ITER end  = End_func();
  for (; iter != end; ++iter) {
    FUNC_PTR func = *iter;
    if (func->Is_defined()) {
      func->Set_undefined();
    }
  }
}

void GLOB_SCOPE::Init_targ_info(ENDIANNESS e, ARCHITECTURE a) {
  AIR_ASSERT(_targ_info == nullptr);
  _targ_info = new TARG_INFO(this, e, a);
}

bool GLOB_SCOPE::Verify_ir() const {
  bool            ret             = true;
  FUNC_SCOPE_ITER func_scope_iter = Begin_func_scope();
  FUNC_SCOPE_ITER end_iter        = End_func_scope();
  for (; func_scope_iter != end_iter; ++func_scope_iter) {
    CONTAINER& cntr = (*func_scope_iter).Container();
    if (!cntr.Verify()) {
      CMPLR_USR_MSG(U_CODE::IR_Verify_Error,
                    (*func_scope_iter).Owning_func()->Name()->Char_str());
      ret = false;
    }
  }
  return ret;
}

void GLOB_SCOPE::Print_mp_info(std::ostream& os, bool func) const {
  size_t total_size = 0;
  os << std::dec << "Global Scope:\n";
  os << "  STRING TABLE (" << _str_tab->Size() << " Items, "
     << _str_tab->Mem_size() << " Bytes)\n";
  total_size += _str_tab->Mem_size();

  os << "  TYPE TABLE (" << _type_tab->Size() << " Items, "
     << _type_tab->Mem_size() << " Bytes)\n";
  total_size += _type_tab->Mem_size();

  os << "  ARB TABLE (" << _arb_tab->Size() << " Items, "
     << _arb_tab->Mem_size() << " Bytes)\n";
  total_size += _arb_tab->Mem_size();

  os << "  FIELD TABLE (" << _fld_tab->Size() << " Items, "
     << _fld_tab->Mem_size() << " Bytes)\n";
  total_size += _fld_tab->Mem_size();

  os << "  PARAM TABLE (" << _param_tab->Size() << " Items, "
     << _param_tab->Mem_size() << " Bytes)\n";
  total_size += _param_tab->Mem_size();

  os << "  MAIN TABLE (" << _main_tab->Size() << " Items, "
     << _main_tab->Mem_size() << " Bytes)\n";
  total_size += _main_tab->Mem_size();

  os << "  AUX TABLE (" << _aux_tab->Size() << " Items, "
     << _aux_tab->Mem_size() << " Bytes)\n";
  total_size += _aux_tab->Mem_size();

  os << "  LITERAL TABLE (" << _lit_tab->Size() << " Items, "
     << _lit_tab->Mem_size() << " Bytes)\n";
  total_size += _lit_tab->Mem_size();

  os << "  CONSTANT TABLE (" << _const_tab->Size() << " Items, "
     << _const_tab->Mem_size() << " Bytes)\n";
  total_size += _const_tab->Mem_size();

  os << "  ATTR TABLE (" << _attr_tab->Size() << " Items, "
     << _attr_tab->Mem_size() << " Bytes)\n";
  total_size += _attr_tab->Mem_size();

  os << "  FILE TABLE (" << _file_tab->Size() << " Items, "
     << _file_tab->Mem_size() << " Bytes)\n";
  total_size += _file_tab->Mem_size();

  os << "  FUNC DEF TABLE (" << _func_def_tab->Size() << " Items, "
     << _func_def_tab->Mem_size() << " Bytes)\n";
  total_size += _func_def_tab->Mem_size();

  os << "  BLOCK TABLE (" << _blk_tab->Size() << " Items, "
     << _blk_tab->Mem_size() << " Bytes)\n";
  total_size += _blk_tab->Mem_size();

  os << "  Total: " << total_size << " Bytes\n\n";

  if (func) {
    FUNC_SCOPE_ITER func_scope_iter = Begin_func_scope();
    FUNC_SCOPE_ITER end_iter        = End_func_scope();
    for (; func_scope_iter != end_iter; ++func_scope_iter) {
      total_size += (*func_scope_iter).Print_mp_info(os);
    }
  }
  os << "Total Mempool in Used: " << total_size << " Bytes\n\n";
}

void GLOB_SCOPE::Print_st(std::ostream& os) const {
  os << "\nTYPE TABLE\n";
  TYPE_ITER t_iter = Begin_type();
  TYPE_ITER t_end  = End_type();
  for (; t_iter != t_end; ++t_iter) {
    (*t_iter)->Print(os, 1);
  }
  os << "\nFUNCTION TABLE\n";
  FUNC_ITER f_iter = Begin_func();
  FUNC_ITER f_end  = End_func();
  for (; f_iter != f_end; ++f_iter) {
    (*f_iter)->Print(os, 1);
  }
}

void GLOB_SCOPE::Print_ir(std::ostream& os, bool rot) const {
  FUNC_SCOPE_ITER func_scope_iter = Begin_func_scope();
  FUNC_SCOPE_ITER end_iter        = End_func_scope();
  for (; func_scope_iter != end_iter; ++func_scope_iter) {
    (*func_scope_iter).Print(os, rot);
    os << std::endl;
  }
}

void GLOB_SCOPE::Print(std::ostream& os, bool rot) const {
  os << "STRING TABLE (" << std::dec << _str_tab->Mem_size() << " Bytes)\n";
  STR_ITER s_iter = Begin_str();
  STR_ITER s_end  = End_str();
  for (; s_iter != s_end; ++s_iter) {
    (*s_iter)->Print(os, 1);
  }
  os << "\nTYPE TABLE (" << std::dec << _type_tab->Mem_size() << " Bytes)\n";
  TYPE_ITER t_iter = Begin_type();
  TYPE_ITER t_end  = End_type();
  for (; t_iter != t_end; ++t_iter) {
    (*t_iter)->Print(os, 1);
  }
  os << "\nARB TABLE (" << std::dec << _arb_tab->Mem_size() << " Bytes)\n";
  ARB_ITER a_iter = Begin_arb();
  ARB_ITER a_end  = End_arb();
  for (; a_iter != a_end; ++a_iter) {
    (*a_iter)->Print(os, 1);
    os << std::endl;
  }
  os << "\nLITERAL TABLE (" << std::dec << _lit_tab->Mem_size() << " Bytes)\n";
  LITERAL_ITER l_iter = Begin_lit();
  LITERAL_ITER l_end  = End_lit();
  for (; l_iter != l_end; ++l_iter) {
    (*l_iter)->Print(os, 1);
  }
  os << "\nCONSTANT TABLE (" << std::dec << _const_tab->Mem_size()
     << " Bytes)\n";
  CONSTANT_ITER c_iter = Begin_const();
  CONSTANT_ITER c_end  = End_const();
  for (; c_iter != c_end; ++c_iter) {
    (*c_iter)->Print(os, 1);
  }
  os << "\nFUNCTION TABLE\n";
  FUNC_ITER f_iter = Begin_func();
  FUNC_ITER f_end  = End_func();
  for (; f_iter != f_end; ++f_iter) {
    (*f_iter)->Print(os, 1);
  }
  os << std::endl;
  Print_ir(os, rot);
}

void GLOB_SCOPE::Print() const { Print(std::cout, true); }

}  // namespace base
}  // namespace air
