//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_ST_ATTR_H
#define AIR_BASE_ST_ATTR_H

#include <string_view>

#include "air/base/st_data.h"
#include "air/base/st_decl.h"

namespace air {
namespace base {

class ATTR {
  friend class SCOPE_BASE;
  friend class ATTR_ITER;
  friend class ATTR_LIST;
  PTR_FRIENDS(ATTR);

public:
  ATTR() : _scope(nullptr), _attr() {}
  ATTR(const SCOPE_BASE& scope, ATTR_ID id);
  ATTR(const SCOPE_BASE& scope, ATTR_DATA_PTR ptr);

  SCOPE_BASE*      Scope() const { return _scope; }
  ATTR_ID          Id() const { return _attr.Id(); }
  const char*      Key() const;
  PRIMITIVE_TYPE   Type() const { return (PRIMITIVE_TYPE)_attr->Type(); }
  uint32_t         Count() const { return _attr->Count(); }
  std::string_view Value() const;

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

protected:
  bool Is_null() const { return _attr.Is_null(); }
  void Set_key(STR_ID id) { _attr->Set_key(id); }
  void Set_value(STR_ID id, PRIMITIVE_TYPE type, uint32_t count) {
    _attr->Set_value(id);
    _attr->Set_type((uint32_t)type);
    _attr->Set_count(count);
  }
  void Set_next(ATTR_ID id) { _attr->Set_next(id); }

  ATTR_DATA_PTR Data() const { return _attr; }
  ATTR_ID       Next() const { return _attr->Next(); }

  SCOPE_BASE*   _scope;
  ATTR_DATA_PTR _attr;
};

class ATTR_LIST {
  friend class NODE;
  friend class TYPE;
  friend class SYM;
  friend class TARG_INFO;

public:
  template <typename T>
  const T* Attr(const char* key, uint32_t* count) const {
    ATTR_PTR attr = Get_attr(key);
    if (attr == Null_ptr) {
      return nullptr;
    }
    AIR_ASSERT(attr->Type() == HOST_TYPE_ID<T> && attr->Count() > 0);
    AIR_ASSERT(attr->Value().size() == sizeof(T) * attr->Count());
    if (count != nullptr) {
      *count = attr->Count();
    }
    return (const T*)attr->Value().data();
  }

  const char* Attr(const char* key) const {
    ATTR_PTR attr = Get_attr(key);
    if (attr == Null_ptr) {
      return nullptr;
    }
    AIR_ASSERT(attr->Type() == HOST_TYPE_ID<char> && attr->Count() == 0);
    AIR_ASSERT(strlen(attr->Value().data()) == attr->Value().size());
    return attr->Value().data();
  }

  template <typename T>
  void Set_attr(const char* key, const T* val, uint32_t count) {
    AIR_ASSERT(val != nullptr && count > 0);
    std::string_view sv((const char*)val, sizeof(T) * count);
    Set_attr(key, sv, HOST_TYPE_ID<T>, count);
  }

  void Set_attr(const char* key, const char* val) {
    Set_attr(key, val, HOST_TYPE_ID<char>, 0);
  }

  ATTR_ITER Begin() const;
  ATTR_ITER End() const;

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

private:
  // only allow friend classes to construct ATTR_LIST object
  ATTR_LIST(const ATTR_ID& attr, const SCOPE_BASE* scope)
      : _head(const_cast<ATTR_ID&>(attr)),
        _scope(const_cast<SCOPE_BASE*>(scope)) {}

  ATTR_LIST(const ATTR_LIST&)            = delete;
  ATTR_LIST& operator=(const ATTR_LIST&) = delete;

  ATTR_PTR Get_attr(const char* key) const;
  ATTR_PTR Set_attr(const char* key, const std::string_view val,
                    PRIMITIVE_TYPE type, uint32_t count);

  ATTR_ID&    _head;
  SCOPE_BASE* _scope;
};

#define DECLATR_ATTR_ACCESS_API(attr_id, scope_ptr)                 \
  template <typename T>                                             \
  const T* Attr(const char* key, uint32_t* count = nullptr) const { \
    ATTR_LIST list(attr_id, scope_ptr);                             \
    return list.Attr<T>(key, count);                                \
  }                                                                 \
  const char* Attr(const char* key) const {                         \
    ATTR_LIST list(attr_id, scope_ptr);                             \
    return list.Attr(key);                                          \
  }                                                                 \
  template <typename T>                                             \
  void Set_attr(const char* key, const T* val, uint32_t count) {    \
    ATTR_LIST list(attr_id, scope_ptr);                             \
    list.Set_attr(key, val, count);                                 \
  }                                                                 \
  void Set_attr(const char* key, const char* val) {                 \
    ATTR_LIST list(attr_id, scope_ptr);                             \
    list.Set_attr(key, val);                                        \
  }

}  // namespace base
}  // namespace air

#endif
