//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <cstring>
#include <sstream>

#include "air/base/st.h"

namespace air {
namespace base {

ATTR::ATTR(const SCOPE_BASE& scope, ATTR_ID id)
    : _scope(const_cast<SCOPE_BASE*>(&scope)),
      _attr(scope.Attr_table().Find(id)) {
  AIR_ASSERT(_attr != ATTR_DATA_PTR());
}

ATTR::ATTR(const SCOPE_BASE& scope, ATTR_DATA_PTR ptr)
    : _scope(const_cast<SCOPE_BASE*>(&scope)), _attr(ptr) {
  AIR_ASSERT(_attr != ATTR_DATA_PTR());
}

const char* ATTR::Key() const {
  STR_PTR str = _scope->Glob_scope().String(_attr->Key());
  AIR_ASSERT(str != STR_PTR());
  return str->Char_str();
}

std::string_view ATTR::Value() const {
  STR_PTR val = _scope->Glob_scope().String(_attr->Value());
  AIR_ASSERT(val != STR_PTR());
  return std::string_view(val->Char_str(), val->Len());
}

void ATTR::Print(std::ostream& os, uint32_t indent) const {
  if (indent) {
    os << std::string(indent * 2, ' ');
  }
  os << Key() << "=";
  const char* val = _scope->Glob_scope().String(_attr->Value())->Char_str();
  if (Count() == 0) {
    AIR_ASSERT(Type() == HOST_TYPE_ID<char>);
    os << val;
    return;
  }
  os << std::dec;
  if (Count() > 1) {
    os << "(";
  }
  for (uint32_t i = 0; i < Count(); ++i) {
    if (i > 0) {
      os << ",";
    }
    switch (Type()) {
      case PRIMITIVE_TYPE::INT_S8:
        os << ((const int8_t*)val)[i];
        break;
      case PRIMITIVE_TYPE::INT_S16:
        os << ((const int16_t*)val)[i];
        break;
      case PRIMITIVE_TYPE::INT_S32:
        os << ((const int32_t*)val)[i];
        break;
      case PRIMITIVE_TYPE::INT_S64:
        os << ((const int64_t*)val)[i];
        break;
      case PRIMITIVE_TYPE::INT_U8:
        os << ((const uint8_t*)val)[i];
        break;
      case PRIMITIVE_TYPE::INT_U16:
        os << ((const uint16_t*)val)[i];
        break;
      case PRIMITIVE_TYPE::INT_U32:
        os << ((const uint32_t*)val)[i];
        break;
      case PRIMITIVE_TYPE::INT_U64:
        os << ((const uint64_t*)val)[i];
        break;
      case PRIMITIVE_TYPE::FLOAT_32:
        os << ((const float*)val)[i];
        break;
      case PRIMITIVE_TYPE::FLOAT_64:
        os << ((const double*)val)[i];
        break;
      default:
        AIR_ASSERT(false);
        break;
    }
  }
  if (Count() > 1) {
    os << ")";
  }
}

void ATTR::Print() const { Print(std::cout, 0); }

std::string ATTR::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os);
  return buf.str();
}

ATTR_PTR ATTR_LIST::Get_attr(const char* key) const {
  ATTR_ID id = _head;
  while (id != Null_id) {
    ATTR_PTR attr = _scope->Attr(id);
    if (std::strcmp(attr->Key(), key) == 0) {
      return attr;
    }
    id = attr->Next();
  }
  return ATTR_PTR();
}

ATTR_PTR ATTR_LIST::Set_attr(const char* key, const std::string_view val,
                             PRIMITIVE_TYPE type, uint32_t count) {
  STR_PTR val_ptr = _scope->Glob_scope().New_str(val.data(), val.size());
  ATTR_ID id      = _head;
  while (id != Null_id) {
    ATTR_PTR attr = _scope->Attr(id);
    if (std::strcmp(attr->Key(), key) == 0) {
      attr->Set_value(val_ptr->Id(), type, count);
      return attr;
    }
    id = attr->Next();
  }
  STR_PTR  key_ptr  = _scope->Glob_scope().New_str(key);
  ATTR_PTR new_attr = _scope->New_attr();
  new_attr->Set_key(key_ptr->Id());
  new_attr->Set_value(val_ptr->Id(), type, count);
  new_attr->Set_next(_head);
  _head = new_attr->Id();
  return new_attr;
}

ATTR_ITER ATTR_LIST::Begin() const {
  if (_head != Null_id) {
    return ATTR_ITER(_scope->Attr(_head));
  } else {
    return ATTR_ITER(ATTR_PTR());
  }
}

ATTR_ITER ATTR_LIST::End() const { return ATTR_ITER(ATTR_PTR()); }

void ATTR_LIST::Print(std::ostream& os, uint32_t indent) const {
  if (indent) {
    os << std::string(indent * 2, ' ');
  }
  ATTR_ID id = _head;
  while (id != Null_id) {
    if (id != _head) {
      os << ",";
    }
    ATTR_PTR attr = _scope->Attr(id);
    attr->Print(os);
    id = attr->Next();
  }
}

void ATTR_LIST::Print() const { Print(std::cout, 0); }

std::string ATTR_LIST::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

}  // namespace base
}  // namespace air
