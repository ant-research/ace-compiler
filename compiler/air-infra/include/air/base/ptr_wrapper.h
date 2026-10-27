//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_PTR_WRAPPER_H
#define AIR_BASE_PTR_WRAPPER_H

#include <cstddef>
#include <cstdint>

#include "air/base/id_wrapper.h"

namespace air {
namespace base {
template <typename T>
class PTR_FROM_DATA;
}
}  // namespace air

template <typename T>
void* operator new(size_t len, const air::base::PTR_FROM_DATA<T>& ptr);

namespace air {
namespace base {

/**
 * @brief Null value of all type template<T> class PTR and
 * template<T> class PTR_TO_CONST
 *
 */
class NULL_PTR {
public:
  NULL_PTR() {}
};
const NULL_PTR Null_ptr;

template <typename T, typename U>
bool Ptr_equal(T ptr1, U ptr2) {
  return ptr1 == ptr2;
}

template <class T>
class PTR_TO_CONST;
template <class T>
class PTR;

template <class T, class U>
bool operator==(const PTR_TO_CONST<T>& lhs, const PTR_TO_CONST<U>& rhs) {
  return Ptr_equal(lhs._ptr.Data(), rhs._ptr.Data());
}

template <class T, class U>
bool operator!=(const PTR_TO_CONST<T>& lhs, const PTR_TO_CONST<U>& rhs) {
  return !(lhs == rhs);
}

template <class T>
bool operator==(PTR_TO_CONST<T> ptr, NULL_PTR) {
  return ptr.Is_null();
}
template <class T>
bool operator!=(PTR_TO_CONST<T> ptr, NULL_PTR) {
  return !(ptr == Null_ptr);
}
template <class T>
bool operator==(NULL_PTR, PTR_TO_CONST<T> ptr) {
  return (ptr == Null_ptr);
}
template <class T>
bool operator!=(NULL_PTR, PTR_TO_CONST<T> ptr) {
  return !(ptr == Null_ptr);
}

template <typename T, typename F>
T Const_cast(const PTR_TO_CONST<F>& ptr);
template <typename T, typename F>
T Reinterpret_cast(const PTR_TO_CONST<F>& ptr);
template <typename T, typename F>
T Static_cast(const PTR_TO_CONST<F>& ptr);

template <class T>
class PTR_TO_CONST {
  typedef PTR_TO_CONST<T> MY_TYPE;
  typedef const T         FWD_TYPE;

  friend class PTR<T>;
  template <class U>
  friend class PTR_TO_CONST;
  template <class U>
  friend class PTR;

  template <class U>
  friend bool operator==(PTR_TO_CONST<U> ptr, NULL_PTR n);
  template <class U, class V>
  friend bool operator==(const PTR_TO_CONST<U>&, const PTR_TO_CONST<V>&);

  template <typename U, typename F>
  friend U Const_cast(const PTR_TO_CONST<F>& ptr);
  template <typename U, typename F>
  friend U Reinterpret_cast(const PTR_TO_CONST<F>& ptr);
  template <typename U, typename F>
  friend U Static_cast(const PTR_TO_CONST<F>& ptr);

public:
  PTR_TO_CONST() {}
  PTR_TO_CONST(NULL_PTR) {}

  explicit PTR_TO_CONST(const T& ptr) : _ptr(ptr) {}

  template <typename U>
  PTR_TO_CONST(const PTR_TO_CONST<U>& ptr) : _ptr(ptr._ptr) {}

  FWD_TYPE* operator->() const { return const_cast<FWD_TYPE*>(&_ptr); }

protected:
  bool Is_null() const { return _ptr.Is_null(); }
  T    _ptr;
};

template <typename T>
class PTR : public PTR_TO_CONST<T> {
  typedef PTR<T>          MY_TYPE;
  typedef PTR_TO_CONST<T> BASE_TYPE;
  typedef T               FWD_TYPE;

  template <class U>
  friend class PTR;
  template <class U>
  friend class PTR_TO_CONST;

  template <typename U, typename F>
  friend U Const_cast(const PTR_TO_CONST<F>& ptr);
  template <typename U, typename F>
  friend U Reinterpret_cast(const PTR_TO_CONST<F>& ptr);
  template <typename U, typename F>
  friend U Static_cast(const PTR_TO_CONST<F>& ptr);

public:
  PTR() {}
  PTR(NULL_PTR) {}

  explicit PTR(const T& ptr) : BASE_TYPE(ptr) {}

  template <typename U>
  PTR(const PTR<U>& ptr) : BASE_TYPE(ptr) {}

  FWD_TYPE* operator->() const { return const_cast<FWD_TYPE*>(&this->_ptr); }
};

template <typename T, typename F>
inline T Const_cast(const PTR_TO_CONST<F>& ptr) {
  return T(*const_cast<typename T::FWD_TYPE*>(&ptr._ptr));
}

template <typename T, typename F>
inline T Reinterpret_cast(const PTR_TO_CONST<F>& ptr) {
  return T(*reinterpret_cast<typename T::FWD_TYPE*>(&ptr._ptr));
}

template <typename T, typename F>
inline T Static_cast(const PTR_TO_CONST<F>& ptr) {
  return T(*static_cast<typename T::FWD_TYPE*>(&ptr._ptr));
}

template <typename T>
class PTR_FROM_DATA;

template <size_t S, size_t A, bool L>
class ARENA;

template <typename T, typename F>
T Const_cast(const PTR_FROM_DATA<F>& ptr);
template <typename T, typename F>
T Reinterpret_cast(const PTR_FROM_DATA<F>& ptr);
template <typename T, typename F>
T Static_cast(const PTR_FROM_DATA<F>& ptr);

template <typename T>
class PTR_DOMAIN {
  friend class PTR_FROM_DATA<T>;

public:
  static const bool Is_void = true;

private:
  typedef T& RET_TYPE;
};

template <>
class PTR_DOMAIN<void> {
  friend class PTR_FROM_DATA<void>;

public:
  static const bool Is_void = true;

private:
  typedef void RET_TYPE;
};

template <>
class PTR_DOMAIN<const void> {
  friend class PTR_FROM_DATA<const void>;

public:
  static const bool Is_void = true;

private:
  typedef void RET_TYPE;
};

template <typename T>
class PTR_FROM_DATA {
  template <typename U, typename F>
  friend U Const_cast(const PTR_FROM_DATA<F>& ptr);
  template <typename U, typename F>
  friend U Reinterpret_cast(const PTR_FROM_DATA<F>& ptr);
  template <typename U, typename F>
  friend U Static_cast(const PTR_FROM_DATA<F>& ptr);

  template <size_t S, size_t A, bool L>
  friend class ARENA;
  friend class CONTAINER;

  friend void* ::operator new<T>(size_t len, const PTR_FROM_DATA<T>& ptr);

public:
  PTR_FROM_DATA() : _obj(0), _id(Null_prim_id) {}

  template <typename F>
  PTR_FROM_DATA(const PTR_FROM_DATA<F>& ptr);

  template <typename U>
  friend class PTR_FROM_DATA;

  T*                               Addr() const;
  typename PTR_DOMAIN<T>::RET_TYPE Obj(int idx = 0) const;

  PTR_FROM_DATA<T>&      operator+=(int c);
  PTR_FROM_DATA<T>&      operator-=(int c);
  PTR_FROM_DATA<T>       operator+(int c) const;
  PTR_FROM_DATA<T>       operator-(int c) const;
  PTR_FROM_DATA<T>&      operator++() { return (*this += 1); }
  PTR_FROM_DATA<T>&      operator--() { return (*this -= 1); }
  const PTR_FROM_DATA<T> operator++(int);
  const PTR_FROM_DATA<T> operator--(int);

  ptrdiff_t operator-(const PTR_FROM_DATA<T>& o) const;

  T* operator->() const;

  bool operator==(const PTR_FROM_DATA<T>& o) const;
  bool operator!=(const PTR_FROM_DATA<T>& o) const;
  bool operator!() const { return !_obj; }

  ID<T> Id() const { return _id; }

  bool Is_null() const { return !*this; }
  bool Is_norm() const { return !Is_null() && !!_id; }
  bool Is_denorm() const { return !Is_null() && !_id; }

  typedef T DOMAIN_TYPE;

  PTR_FROM_DATA(T* obj, ID<T> id);

private:
  void Id(ID<T> id) { _id = id; }

  T*    _obj;
  ID<T> _id;
};

template <typename T>
template <typename F>
inline PTR_FROM_DATA<T>::PTR_FROM_DATA(const PTR_FROM_DATA<F>& ptr)
    : _obj(ptr._obj), _id(ptr._id) {}

template <typename T>
inline PTR_FROM_DATA<T>::PTR_FROM_DATA(T* obj, ID<T> id) : _obj(obj), _id(id) {}

template <typename T, typename F>
inline T Const_cast(const PTR_FROM_DATA<F>& ptr) {
  return T(const_cast<typename T::DOMAIN_TYPE*>(ptr._obj),
           Const_cast<ID<typename T::DOMAIN_TYPE> >(ptr._id));
}

template <typename T, typename F>
inline T Reinterpret_cast(const PTR_FROM_DATA<F>& ptr) {
  return T(reinterpret_cast<typename T::DOMAIN_TYPE*>(ptr._obj),
           Reinterpret_cast<ID<typename T::DOMAIN_TYPE> >(ptr._id));
}

template <typename T, typename F>
inline T Static_cast(const PTR_FROM_DATA<F>& ptr) {
  return T(static_cast<typename T::DOMAIN_TYPE*>(ptr._obj),
           Static_cast<ID<typename T::DOMAIN_TYPE> >(ptr._id));
}

template <typename T>
inline T* PTR_FROM_DATA<T>::Addr() const {
  return _obj;
}

template <typename T>
inline typename PTR_DOMAIN<T>::RET_TYPE PTR_FROM_DATA<T>::Obj(int idx) const {
  return _obj[idx];
}

template <typename T>
inline PTR_FROM_DATA<T>& PTR_FROM_DATA<T>::operator+=(int c) {
  _obj += c;
  _id = Null_id;
  return *this;
}

template <typename T>
inline PTR_FROM_DATA<T>& PTR_FROM_DATA<T>::operator-=(int c) {
  return *this += -c;
}

template <typename T>
inline PTR_FROM_DATA<T> PTR_FROM_DATA<T>::operator+(int c) const {
  PTR_FROM_DATA<T> sum(*this);
  sum += c;
  return sum;
}

template <typename T>
inline PTR_FROM_DATA<T> PTR_FROM_DATA<T>::operator-(int c) const {
  PTR_FROM_DATA<T> diff(*this);
  diff -= c;
  return diff;
}

template <typename T>
inline const PTR_FROM_DATA<T> PTR_FROM_DATA<T>::operator++(int) {
  PTR_FROM_DATA<T> old(*this);
  ++(*this);
  return old;
}

template <typename T>
inline const PTR_FROM_DATA<T> PTR_FROM_DATA<T>::operator--(int) {
  PTR_FROM_DATA<T> old(*this);
  --(*this);
  return old;
}

template <typename T>
inline ptrdiff_t PTR_FROM_DATA<T>::operator-(const PTR_FROM_DATA<T>& o) const {
  return _obj - o._obj;
}

template <typename T>
inline T* PTR_FROM_DATA<T>::operator->() const {
  return _obj;
}

template <typename T>
inline bool PTR_FROM_DATA<T>::operator==(const PTR_FROM_DATA<T>& o) const {
  return _obj == o._obj;
}

template <typename T>
inline bool PTR_FROM_DATA<T>::operator!=(const PTR_FROM_DATA<T>& o) const {
  return !(*this == o);
}

class NULL_DATA_PTR {
public:
  NULL_DATA_PTR() {}
  template <typename T>
  operator PTR_FROM_DATA<T>() const {
    return PTR_FROM_DATA<T>();
  }
};

const NULL_DATA_PTR Null_data_ptr;

template <typename T>
bool operator==(NULL_DATA_PTR, const PTR_FROM_DATA<T>& ptr) {
  return !ptr;
}
template <typename T>
bool operator!=(NULL_DATA_PTR, const PTR_FROM_DATA<T>& ptr) {
  return !!ptr;
}
template <typename T>
bool operator==(const PTR_FROM_DATA<T>& ptr, NULL_DATA_PTR) {
  return !ptr;
}
template <typename T>
bool operator!=(const PTR_FROM_DATA<T>& ptr, NULL_DATA_PTR) {
  return !!ptr;
}

#define PTR_FRIENDS(CLS)          \
  friend class PTR_TO_CONST<CLS>; \
  friend class PTR<CLS>;          \
  template <class T, class U>     \
  friend bool operator==(const PTR_TO_CONST<T>&, const PTR_TO_CONST<U>&);
}  // namespace base
}  // namespace air

template <typename T>
inline void* operator new(size_t, const air::base::PTR_FROM_DATA<T>& ptr) {
  return ptr._obj;
}

#endif
