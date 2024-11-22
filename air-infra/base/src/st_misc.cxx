//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <cstring>

#include "air/base/st.h"

namespace air {
namespace base {

//=============================================================================
// class SRC_FILE member functions
//=============================================================================

SRC_FILE::SRC_FILE() : _glob(0), _file() {}

SRC_FILE::SRC_FILE(const GLOB_SCOPE& glob, FILE_ID id)
    : _glob(const_cast<GLOB_SCOPE*>(&glob)),
      _file(glob.File_table().Find(ID<FILE_DATA>(id.Index()))) {
  AIR_ASSERT(id.Scope() == 0);
}

FILE_ID
SRC_FILE::Id() const { return FILE_ID(_file.Id().Value(), 0); }

STR_PTR
SRC_FILE::File_name() const { return _glob->String(_file->Name()); }

//=============================================================================
// class STR member functions
//=============================================================================

STR::STR(const GLOB_SCOPE& glob, STR_ID id)
    : _glob(const_cast<GLOB_SCOPE*>(&glob)), _str() {
  if (id == STR_ID()) {
    _glob = 0;
    return;
  }
  _str = Reinterpret_cast<STR_DATA_PTR>(
      glob.Str_table().Find(ID<short>(id.Index())));
}

STR_ID
STR::Id() const {
  if (Is_null()) return STR_ID();
  return STR_ID(_str.Id().Value(), 0);
}

uint32_t STR::Len() const {
  AIR_ASSERT(!Is_null());
  return _str->Len();
}

const char* STR::Char_str() const {
  AIR_ASSERT(!Is_null());
  return _str->Str();
}

bool STR::Is_equal(STR_PTR str) const {
  if (Is_null() && str->Is_null()) return true;
  if ((!Is_null() && str->Is_null()) || (Is_null() && !str->Is_null()))
    return false;
  if (_str == str->_str) return true;
  if (Len() != str->Len()) return false;
  return !(strncmp(Char_str(), str->Char_str(), Len()));
}

bool STR::Is_undefined() const { return (Id() == _glob->Undefined_name_id()); }

void STR::Print(std::ostream& os, uint32_t indent) const {
  os << std::string(indent * INDENT_SPACE, ' ');
  os << std::hex << std::showbase;
  os << "STR[" << Id().Value() << "] \"" << Char_str() << "\" length(" << Len()
     << ")\n";
}

void STR::Print() const { Print(std::cout, 0); }

}  // namespace base
}  // namespace air
