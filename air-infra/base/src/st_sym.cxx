//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <iomanip>
#include <iostream>
#include <sstream>

#include "air/base/st.h"

namespace air {
namespace base {

//=============================================================================
// class SYM member functions
//=============================================================================

SYM::SYM(SCOPE_BASE* scope, SYM_DATA_PTR ptr) : _scope(scope), _sym(ptr) {
  AIR_ASSERT(scope);
  AIR_ASSERT(ptr != SYM_DATA_PTR());
}

SYM::SYM(const GLOB_SCOPE& glob, SYM_ID sym)
    : _scope(const_cast<GLOB_SCOPE*>(&glob)),
      _sym(glob.Main_table().Find(ID<SYM_DATA>(sym.Index()))) {
  AIR_ASSERT(sym.Scope() == 0);
}

SYM::SYM(const FUNC_SCOPE& func, SYM_ID sym) { Set(func, sym); }

void SYM::Set(const FUNC_SCOPE& func, SYM_ID sym) {
  if (sym.Is_null()) {
    Set_null();
    return;
  }

  if (!sym.Is_local()) {
    _scope = const_cast<GLOB_SCOPE*>(&func.Glob_scope());
    _sym   = func.Glob_scope().Main_table().Find(ID<SYM_DATA>(sym.Index()));
  } else if (sym.Scope() == func.Scope_level()) {
    _scope = const_cast<FUNC_SCOPE*>(&func);
    _sym   = func.Main_table().Find(ID<SYM_DATA>(sym.Index()));
  } else {
    FUNC_SCOPE& par = *(func.Parent_func_scope());
    _scope          = &par;
    _sym            = par.Main_table().Find(ID<SYM_DATA>(sym.Index()));
  }
}

void SYM::Set_null() {
  _sym   = SYM_DATA_PTR();
  _scope = 0;
}

SYM_ID
SYM::Id() const {
  if (Is_null()) return SYM_ID(Null_st_id);
  return SYM_ID(_sym.Id().Value(), Scope_level());
}

SPOS SYM::Spos() const {
  if (Is_null()) return SPOS();
  AUX_SYM_DATA_PTR ptr = Fixed_aux_entry();
  return ptr->Spos();
}

uint32_t SYM::Scope_level() const {
  AIR_ASSERT(_scope);
  return _scope->Is_glob() ? 0 : _scope->Cast_to_func().Scope_level();
}

AUX_TAB& SYM::Aux_table() const {
  AIR_ASSERT(_scope);
  return _scope->Is_glob() ? _scope->Cast_to_glob().Aux_table()
                           : _scope->Cast_to_func().Aux_table();
}

AUX_ID
SYM::First_aux_entry_id() const { return _sym->First_aux_entry(); }

AUX_DATA_PTR
SYM::First_aux_entry() const {
  return Aux_table().Aux_entry(First_aux_entry_id());
}

AUX_SYM_DATA_PTR
SYM::Fixed_aux_entry() const {
  AIR_ASSERT(!First_aux_entry_id().Is_null());
  AUX_SYM_DATA_PTR ptr = Static_cast<AUX_SYM_DATA_PTR>(First_aux_entry());
  AIR_ASSERT(ptr->Kind() == AUX_KIND::SYM);
  return ptr;
}

AUX_DATA_PTR
SYM::New_first_aux_entry(AUX_ID* aux) {
  return New_aux_entry<AUX_KIND::SYM>(aux);
}

STR_ID
SYM::Name_id() const {
  AUX_SYM_DATA_PTR ptr = Fixed_aux_entry();
  return ptr->Name();
}

STR_PTR
SYM::Name() const {
  if (Is_static_fld())
    return Cast_to<SYMBOL_CLASS::ADDR_DATUM>()->Field()->Name();
  return Glob_scope().String(Name_id());
}

void SYM::Set_name(STR_ID name) {
  AUX_SYM_DATA_PTR ptr = Fixed_aux_entry();
  ptr->Set_name(name.Is_null() ? Glob_scope().Undefined_name_id() : name);
}

FUNC_SCOPE* SYM::Defining_func_scope() const {
  AIR_ASSERT(_scope);
  return _scope->Is_func() ? const_cast<FUNC_SCOPE*>(&_scope->Cast_to_func())
                           : 0;
}

bool SYM::Is_static_fld() const {
  if (Kind() != SYMBOL_CLASS::VAR) return false;
  CONST_ADDR_DATUM_PTR& datum = Cast_to<SYMBOL_CLASS::VAR>();
  return datum->Is_static_fld();
}

void SYM::Set_spos(const SPOS& spos) {
  AUX_SYM_DATA_PTR ptr = Fixed_aux_entry();
  ptr->Set_spos(spos);
}

FUNC_DEF_ID
SYM::New_func_def_data() {
  AIR_ASSERT(Kind() == SYMBOL_CLASS::FUNC);
  return const_cast<GLOB_SCOPE&>(Glob_scope()).New_func_def_data();
}

FUNC_DEF_DATA_PTR
SYM::Func_def_data(FUNC_DEF_ID id) const {
  return Glob_scope().Func_def_table().Find(ID<FUNC_DEF_DATA>(id.Index()));
}

std::string SYM::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os);
  return buf.str();
}

void SYM::Print() const { Print(std::cout, 0); }

void SYM::Print(std::ostream& os, uint32_t indent) const {
  os << std::string(indent * INDENT_SPACE, ' ');
  os << std::hex << std::showbase;

  if (Kind() == SYMBOL_CLASS::VAR) {
    CONST_ADDR_DATUM_PTR& me   = Cast_to<SYMBOL_CLASS::VAR>();
    CONST_TYPE_PTR        type = me->Type();

    const char* var_name      = Name()->Char_str();
    const char* var_type_name = type->Name()->Char_str();

    os << "VAR[" << Id().Value() << "] "
       << "\"" << var_name << "\"\n"
       << std::string((indent + 1) * INDENT_SPACE, ' ') << "scope_level["
       << Scope_level() << "], TYP[" << type->Id().Value() << "]("
       << type->Type_kind_name() << ",\"" << var_type_name << "\")";
  } else if (Kind() == SYMBOL_CLASS::FUNC) {
    CONST_FUNC_PTR& me = Cast_to<SYMBOL_CLASS::FUNC>();
    uint32_t        parent;
    if (me->Is_parentless()) {
      parent = Null_st_id;
    } else {
      parent = me->Parent_block_id().Value();
    }
    const char* func_name = Name()->Char_str();

    os << "FUN[" << Id().Value() << "] "
       << "\"" << func_name << "\"\n"
       << std::string((indent + 1) * INDENT_SPACE, ' ') << "parentBLK["
       << parent << "], defined(" << (me->Is_defined() ? "yes" : "no") << ")";
  } else if (Kind() == SYMBOL_CLASS::ENTRY) {
    CONST_ENTRY_PTR&         me         = Cast_to<SYMBOL_CLASS::ENTRY>();
    CONST_TYPE_PTR           type       = me->Type();
    CONST_SIGNATURE_TYPE_PTR sig        = type->Base_type()->Cast_to_sig();
    const char*              type_name  = type->Name()->Char_str();
    const char*              entry_name = Name()->Char_str();

    os << "ENT[" << Id().Value() << "] "
       << "\"" << entry_name << "\"\n"
       << std::string((indent + 1) * INDENT_SPACE, ' ') << "FUN["
       << me->Owning_func_id().Value() << "], "
       << "TYP[" << type->Id().Value() << "](" << type->Type_kind_name()
       << ",\"" << type_name << "\")";
  } else if (Kind() == SYMBOL_CLASS::FORMAL) {
    CONST_ADDR_DATUM_PTR& me        = Cast_to<SYMBOL_CLASS::FORMAL>();
    CONST_TYPE_PTR        type      = me->Type();
    CONST_STR_PTR         name      = Name();
    const char*           name_str  = name->Char_str();
    const char*           type_name = type->Name()->Char_str();

    os << "FML[" << Id().Value() << "] "
       << "\"" << name_str << "\", TYP[" << type->Id().Value() << "]("
       << type->Type_kind_name() << ",\"" << type_name << "\")";
  }
  os << std::endl;
}

GLOB_SCOPE& SYM::Glob_scope() const {
  AIR_ASSERT(_scope);
  if (_scope->Is_glob()) return _scope->Cast_to_glob();
  return _scope->Cast_to_func().Glob_scope();
}

template <AUX_KIND T>
AUX_DATA_PTR SYM::New_aux_entry(AUX_ID* id, AUX_DATA_PTR last) {
  AIR_ASSERT(_scope);
  AIR_ASSERT(!last || last->Kind() == T);
  AUX_TAB* at = const_cast<AUX_TAB*>(&Aux_table());
  return at->template New_aux_entry<SYM, T>(*this, id, last);
}

//=============================================================================
// class FUNC member functions
//=============================================================================

FUNC::FUNC(const GLOB_SCOPE& glob, FUNC_ID id) : SYM(glob, id) {
  Requires<CHECK_SIZE_EQUAL<FUNC, SYM> >();
  AIR_ASSERT(Is_null() || Kind() == SYMBOL_CLASS::FUNC);
}

FUNC::FUNC(const FUNC_SCOPE& func, FUNC_ID id) : SYM(func, id) {
  Requires<CHECK_SIZE_EQUAL<FUNC, SYM> >();
  AIR_ASSERT(Is_null() || Kind() == SYMBOL_CLASS::FUNC);
}

bool FUNC::Is_nested_func() const { return (Nesting_level() > 0); }

bool FUNC::Is_parentless() const {
  BLOCK_PTR blk = Func_block();
  return ((blk != Null_ptr) && (blk->Parent_block_id().Is_null()));
}

SPOS FUNC::Begin_spos() const { return Func_def_data()->Func_begin_spos(); }

SPOS FUNC::End_spos() const { return Func_def_data()->Func_end_spos(); }

BLOCK_PTR
FUNC::Func_block() const { return Glob_scope().Block(Func_block_id()); }

BLOCK_ID
FUNC::Parent_block_id() const {
  BLOCK_PTR ptr = Func_block();
  return ptr->Parent_block_id();
}

FUNC_ID
FUNC::Parent_func_id() const {
  AIR_ASSERT(Is_defined() && !Is_parentless() && Is_nested_func());
  BLOCK_PTR ptr = Parent_block();
  AIR_ASSERT(ptr->Is_func());
  return ptr->Owning_func_id();
}

FUNC_DEF_DATA_PTR
FUNC::Func_def_data() const {
  AIR_ASSERT(Is_defined());
  FUNC_DEF_ID id = _sym->Func_def_id();
  AIR_ASSERT(!(id == FUNC_DEF_ID(Null_st_id)));
  return SYM::Func_def_data(id);
}

STMT_ID FUNC::Entry_stmt_id() const { return Func_def_data()->Entry_stmt_id(); }

uint32_t FUNC::New_annot_id() {
  AIR_ASSERT(Is_defined());
  return Func_def_data()->New_annot_id();
}

BLOCK_PTR
FUNC::Parent_block() const { return Glob_scope().Block(Parent_block_id()); }

ENTRY_ID
FUNC::Entry_point_id() const {
  ENTRY_ID entry = Fixed_aux_entry()->First_entry_point();
  AIR_ASSERT(!entry.Is_null());
  return entry;
}

ENTRY_PTR
FUNC::Entry_point() const { return Glob_scope().Entry_point(Entry_point_id()); }

void FUNC::Set_first_entry_point(ENTRY_ID id) {
  AIR_ASSERT(First_entry_point_id().Is_null());
  Fixed_aux_entry()->Set_first_entry_point(id);
}

ENTRY_ID
FUNC::First_entry_point_id() const {
  return Fixed_aux_entry()->First_entry_point();
}

void FUNC::Set_defined(FUNC_DEF_ID id) {
  AIR_ASSERT(!Is_defined());
  _sym->Set_func_defined();
  if (id == FUNC_DEF_ID(Null_st_id)) {
    id = SYM::New_func_def_data();
  }
  _sym->Set_func_def_id(id);

  Set_begin_spos(Glob_scope().Unknown_simple_spos());
  Set_end_spos(Glob_scope().Unknown_simple_spos());
}

void FUNC::Set_undefined() {
  AIR_ASSERT(Is_defined());
  _sym->Set_func_defined(false);
  _sym->Set_func_def_id(FUNC_DEF_ID());
}

void FUNC::Set_begin_spos(const SPOS& spos) {
  Func_def_data()->Set_func_begin_spos(spos);
}

void FUNC::Set_end_spos(const SPOS& spos) {
  Func_def_data()->Set_func_end_spos(spos);
}

void FUNC::Add_entry_point(ENTRY_ID id) {
  if (First_entry_point_id().Is_null()) {
    Set_first_entry_point(id);
  } else {
    // TODO FUNC_ITR for multiple entry
  }
}

void FUNC::Set_parent(CONST_BLOCK_PTR ptr) { Set_parent(ptr->Id()); }

void FUNC::Set_parent(BLOCK_ID id) {
  BLOCK_PTR blk = Func_block();
  blk->Set_parent_block(id);
  if (id.Is_null()) return;
  BLOCK_PTR id_blk = Glob_scope().Block(id);
  if (id_blk->Is_func()) {
    FUNC_PTR func_ptr = id_blk->Owning_func();
    Set_nesting_level(func_ptr->Nesting_level() + 1);
  }
}

void FUNC::Set_entry_stmt(STMT_ID id) {
  AIR_ASSERT(Is_defined());
  Func_def_data()->Set_entry_stmt(id);
}

//=============================================================================
// class PACKET member functions
//=============================================================================

PACKET::PACKET(const GLOB_SCOPE& glob, PACKET_ID id) : SYM(glob, id) {
  Requires<CHECK_SIZE_EQUAL<PACKET, SYM> >();
  AIR_ASSERT(Is_null() || Kind() == SYMBOL_CLASS::PACKET);
}

PACKET::PACKET(const FUNC_SCOPE& func, PACKET_ID id) : SYM(func, id) {
  Requires<CHECK_SIZE_EQUAL<PACKET, SYM> >();
  AIR_ASSERT(Is_null() || Kind() == SYMBOL_CLASS::PACKET);
}

PACKET_ID
PACKET::Id() const { return Sym_id<SYMBOL_CLASS::PACKET>(); }

//=============================================================================
// class ADDR_DATUM member functions
//=============================================================================

ADDR_DATUM::ADDR_DATUM(const GLOB_SCOPE& glob, ADDR_DATUM_ID id)
    : SYM(glob, id) {
  Requires<CHECK_SIZE_EQUAL<ADDR_DATUM, SYM> >();
  AIR_ASSERT(Is_null() || Kind() == SYMBOL_CLASS::VAR ||
             Kind() == SYMBOL_CLASS::FORMAL);
}

ADDR_DATUM::ADDR_DATUM(const FUNC_SCOPE& func, ADDR_DATUM_ID id)
    : SYM(func, id) {
  Requires<CHECK_SIZE_EQUAL<ADDR_DATUM, SYM> >();
  AIR_ASSERT(Is_null() || Kind() == SYMBOL_CLASS::VAR ||
             Kind() == SYMBOL_CLASS::FORMAL);
}

ADDR_DATUM_ID
ADDR_DATUM::Id() const { return Sym_id<SYMBOL_CLASS::ADDR_DATUM>(); }

bool ADDR_DATUM::Is_static_fld() const {
  AUX_SYM_DATA_PTR aux = Fixed_aux_entry();
  return aux->Is_static_fld();
}

bool ADDR_DATUM::Has_implicit_ref() const { return _sym->Has_implicit_ref(); }

void ADDR_DATUM::Set_type(CONST_TYPE_PTR type) { Set_type(type->Id()); }

void ADDR_DATUM::Set_type(TYPE_ID type) { _sym->Set_addr_datum_type(type); }

void ADDR_DATUM::Set_implicit_ref() { _sym->Set_implicit_ref(); }

TYPE_ID
ADDR_DATUM::Type_id() const { return _sym->Addr_datum_type(); }

TYPE_PTR
ADDR_DATUM::Type() const { return Glob_scope().Type(Type_id()); }

FIELD_ID
ADDR_DATUM::Field_id() const {
  AIR_ASSERT(Is_static_fld());
  AUX_SYM_DATA_PTR aux = Fixed_aux_entry();
  return aux->Field();
}

SYM_ID
ADDR_DATUM::Base_sym_id() const {
  if (Is_in_packet()) return Packet_id();
  return Id();
}

PACKET_ID
ADDR_DATUM::Packet_id() const { return _sym->Owning_packet(); }

PACKET_PTR
ADDR_DATUM::Packet() const {
  AIR_ASSERT(Is_in_packet());
  FUNC_SCOPE* func = Defining_func_scope();
  if (func) return func->Packet(Packet_id());
  return Glob_scope().Packet(Packet_id());
}

SYM_PTR
ADDR_DATUM::Base_sym() const {
  if (!Is_in_packet()) return SYM_PTR(*this);
  FUNC_SCOPE* func = Defining_func_scope();
  if (func) return func->Sym(Packet_id());
  return Glob_scope().Sym(Packet_id());
}

FIELD_PTR
ADDR_DATUM::Field() const { return Glob_scope().Field(Field_id()); }

//=============================================================================
// class ENTRY member functions
//=============================================================================

ENTRY::ENTRY(const GLOB_SCOPE& glob, ENTRY_ID id) : SYM(glob, id) {
  Requires<CHECK_SIZE_EQUAL<ENTRY, SYM> >();
  AIR_ASSERT(Is_null() || Kind() == SYMBOL_CLASS::ENTRY);
}

ENTRY::ENTRY(const FUNC_SCOPE& func, ENTRY_ID id) : SYM(func.Glob_scope(), id) {
  Requires<CHECK_SIZE_EQUAL<ENTRY, SYM> >();
  AIR_ASSERT(Is_null() || Kind() == SYMBOL_CLASS::ENTRY);
}

TYPE_ID
ENTRY::Type_id() const { return _sym->Entry_type(); }

TYPE_PTR
ENTRY::Type() const { return Glob_scope().Type(Type_id()); }

FUNC_ID
ENTRY::Owning_func_id() const { return _sym->Entry_owning_func(); }

FUNC_PTR
ENTRY::Owning_func() const { return Glob_scope().Func(Owning_func_id()); }

bool ENTRY::Is_program_entry() const { return _sym->Is_entry_prg_entry(); }

void ENTRY::Set_owning_func(CONST_FUNC_PTR func) {
  Set_owning_func(func->Id());
}

void ENTRY::Set_owning_func(FUNC_ID func) {
  AIR_ASSERT(!func.Is_null() && !func.Is_local());
  _sym->Set_entry_owning_func(func);
}

void ENTRY::Set_program_entry() { _sym->Set_entry_prg_entry(); }

//=============================================================================
// class PREG member functions
//=============================================================================

PREG::PREG(const FUNC_SCOPE& func, PREG_ID id) {
  _func = const_cast<FUNC_SCOPE*>(&func);
  _preg = func.Preg_table().Find(ID<PREG_DATA>(id.Index()));
}

uint32_t PREG::Scope_level() const {
  AIR_ASSERT(_func);
  return _func->Scope_level();
}

PREG_ID PREG::Id() const {
  if (Is_null()) return PREG_ID(Null_st_id);
  return PREG_ID(_preg.Id().Value(), Scope_level());
}

TYPE_ID PREG::Type_id() const { return _preg->Type_id(); }

TYPE_PTR PREG::Type() const { return _func->Glob_scope().Type(Type_id()); }

SYM_ID PREG::Home_sym_id() const { return _preg->Home_id(); }

SYM_PTR PREG::Home_sym() const { return _func->Sym(Home_sym_id()); }

void PREG::Set_home_sym(SYM_ID home) { _preg->Set_home(home); }

std::string PREG::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os);
  return buf.str();
}

void PREG::Print() const { Print(std::cout, 0); }

void PREG::Print(std::ostream& os, uint32_t indent) const {
  os << std::string(indent * INDENT_SPACE, ' ');
  os << std::hex << std::showbase;
  CONST_TYPE_PTR type = Type();
  os << "PREG[" << Id().Value() << "] TYP[" << type->Id().Value() << "]("
     << type->Type_kind_name() << ",\"" << type->Name()->Char_str()
     << "\",size:" << std::dec << type->Byte_size() << "), ";
  SYM_PTR home = Home_sym();
  os << "Home[" << std::hex << home->Id().Value() << "]";
  if (home != Null_ptr) {
    os << "(" << home->Name()->Char_str() << ")";
  }
  os << std::endl;
}

}  // namespace base
}  // namespace air
