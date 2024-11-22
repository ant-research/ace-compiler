//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_ID_WRAPPER_H
#define AIR_BASE_ID_WRAPPER_H

#include <cstddef>
#include <cstdint>

namespace air {
namespace base {

template <typename T>
class ID;

template <typename T, typename F>
T Const_cast(ID<F>);
template <typename T, typename F>
T Reinterpret_cast(ID<F>);
template <typename T, typename F>
T Static_cast(ID<F>);

/// @brief Null value of primitive id
const uint32_t Null_prim_id = UINT32_MAX;

/**
 * @brief Strongly typed identifier from uint32_t for NODE and STMT
 *
 * @tparam T
 */
template <typename T>
class ID {
public:
  explicit ID(uint32_t id = Null_prim_id) : _id(id) {}
  uint32_t Value() const { return _id; }

  bool operator==(ID<T> o) const { return _id == o._id; }
  bool operator!=(ID<T> o) const { return !(*this == o); }
  bool operator<(ID<T> o) const { return _id < o._id; }
  bool operator!() const { return _id == Null_prim_id; }

  template <typename F>
  ID(ID<F> id);

  typedef T DOMAIN_TYPE;

private:
  template <typename F>
  T* Check_iconv();

  uint32_t _id;
};

template <typename T>
template <typename F>
T* ID<T>::Check_iconv() {
  return (F*)0;
}

template <typename T>
template <typename F>
ID<T>::ID(ID<F> id) : _id(id.Value()) {
  Check_iconv<F>();
}

template <typename T, typename F>
void* Check_const_cast() {
  return const_cast<typename T::DOMAIN_TYPE*>((F*)0);
}

template <typename T, typename F>
inline T Const_cast(ID<F> id) {
  Check_const_cast<T, F>();
  return T(id.Value());
}

template <typename T, typename F>
void* Check_reinterpret_cast() {
  return reinterpret_cast<typename T::DOMAIN_TYPE*>((F*)0);
}

template <typename T, typename F>
inline T Reinterpret_cast(ID<F> id) {
  Check_reinterpret_cast<T, F>();
  return T(id.Value());
}

template <typename T, typename F>
void* Check_static_cast() {
  return static_cast<typename T::DOMAIN_TYPE*>((F*)0);
}

template <typename T, typename F>
inline T Static_cast(ID<F> id) {
  Check_static_cast<T, F>();
  return T(id.Value());
}

/**
 * @brief Null value of all types template<T> class ID
 *
 */
class NULL_ID {
public:
  NULL_ID(){};

  template <typename T>
  operator ID<T>() const {
    return ID<T>();
  }
};

const NULL_ID Null_id;

template <typename T>
bool operator==(NULL_ID, ID<T> id) {
  return !id;
}
template <typename T>
bool operator!=(NULL_ID, ID<T> id) {
  return !!id;
}
template <typename T>
bool operator==(ID<T> id, NULL_ID) {
  return !id;
}
template <typename T>
bool operator!=(ID<T> id, NULL_ID) {
  return !!id;
}

const uint32_t Null_st_id = UINT32_MAX;
#define ID_SCOPE_WIDTH 4
#define ID_SCOPE_MAX   ((1 << ID_SCOPE_WIDTH) - 1)
#define ID_WIDTH       28
const uint32_t Id_scope_mask = (0xFFFFFFFFUL >> ID_SCOPE_WIDTH);

/**
 * @brief Strongly typed identifier from uint32_t for symbol table
 *
 * @tparam T
 */
template <typename T>
class ID_BASE {
public:
  explicit ID_BASE(uint32_t id = Null_st_id) : _id(id) {}

  ID_BASE(uint32_t idx, uint32_t scope) { _id = (scope << ID_WIDTH) + idx; }

  bool operator==(const ID_BASE& o) const { return (o.Value() == _id); }
  bool operator!=(const ID_BASE& o) const { return (o.Value() != _id); }
  bool operator<(const ID_BASE& o) const { return (o.Value() < _id); }
  bool operator>(const ID_BASE& o) const { return (o.Value() > _id); }
  bool operator<=(const ID_BASE& o) const { return (o.Value() <= _id); }
  bool operator>=(const ID_BASE& o) const { return (o.Value() >= _id); }
  bool Is_null() const { return (_id == Null_st_id); }
  bool Is_local() const { return (Scope() != 0); }

  uint32_t Scope() const { return _id >> ID_WIDTH; }
  uint32_t Index() const { return _id & Id_scope_mask; }
  uint32_t Value() const { return _id; }

private:
  uint32_t _id;
};

/// @brief Null check for all types template<T> class ID_BASE;
template <typename T>
inline bool Is_null_id(T id) {
  return ((id.Value() | ~Id_scope_mask) == Null_st_id);
}

}  // namespace base
}  // namespace air

#endif
