//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_ST_TRAIT_H
#define AIR_BASE_ST_TRAIT_H

#include "air/base/st_decl.h"

namespace air {
namespace base {

template <AUX_KIND K>
struct AUX_DATA_TRAITS {
  typedef char  AUX_DATA_TYPE;
  typedef void* VALUE_TYPE;
};

template <>
struct AUX_DATA_TRAITS<AUX_KIND::SYM> {
  typedef AUX_SYM_DATA AUX_DATA_TYPE;
  typedef void*        VALUE_TYPE;
};

template <SYMBOL_CLASS T>
struct SYM_TYPE_TRAITS {
  typedef SYM    SYM_TYPE;
  typedef SYM_ID SYM_ID_TYPE;
};

template <>
struct SYM_TYPE_TRAITS<SYMBOL_CLASS::FUNC> {
  typedef FUNC    SYM_TYPE;
  typedef FUNC_ID SYM_ID_TYPE;
};

template <>
struct SYM_TYPE_TRAITS<SYMBOL_CLASS::ENTRY> {
  typedef ENTRY    SYM_TYPE;
  typedef ENTRY_ID SYM_ID_TYPE;
};

template <>
struct SYM_TYPE_TRAITS<SYMBOL_CLASS::THUNK> {
  typedef THUNK    SYM_TYPE;
  typedef THUNK_ID SYM_ID_TYPE;
};

template <>
struct SYM_TYPE_TRAITS<SYMBOL_CLASS::ADDR_DATUM> {
  typedef ADDR_DATUM    SYM_TYPE;
  typedef ADDR_DATUM_ID SYM_ID_TYPE;
};

template <>
struct SYM_TYPE_TRAITS<SYMBOL_CLASS::VAR> {
  typedef ADDR_DATUM    SYM_TYPE;
  typedef ADDR_DATUM_ID SYM_ID_TYPE;
};

template <>
struct SYM_TYPE_TRAITS<SYMBOL_CLASS::FORMAL> {
  typedef ADDR_DATUM    SYM_TYPE;
  typedef ADDR_DATUM_ID SYM_ID_TYPE;
};

template <>
struct SYM_TYPE_TRAITS<SYMBOL_CLASS::PACKET> {
  typedef PACKET    SYM_TYPE;
  typedef PACKET_ID SYM_ID_TYPE;
};

template <TYPE_TRAIT T>
struct TYPE_DATA_TRAITS {
  typedef TYPE      TYPE_TYPE;
  typedef TYPE_DATA TYPE_DATA_TYPE;
};

template <>
struct TYPE_DATA_TRAITS<TYPE_TRAIT::PRIMITIVE> {
  typedef PRIM_TYPE      TYPE_TYPE;
  typedef PRIM_TYPE_DATA TYPE_DATA_TYPE;
};

template <>
struct TYPE_DATA_TRAITS<TYPE_TRAIT::VA_LIST> {
  typedef VA_LIST_TYPE      TYPE_TYPE;
  typedef VA_LIST_TYPE_DATA TYPE_DATA_TYPE;
};

template <>
struct TYPE_DATA_TRAITS<TYPE_TRAIT::POINTER> {
  typedef POINTER_TYPE      TYPE_TYPE;
  typedef POINTER_TYPE_DATA TYPE_DATA_TYPE;
};

template <>
struct TYPE_DATA_TRAITS<TYPE_TRAIT::ARRAY> {
  typedef ARRAY_TYPE      TYPE_TYPE;
  typedef ARRAY_TYPE_DATA TYPE_DATA_TYPE;
};

template <>
struct TYPE_DATA_TRAITS<TYPE_TRAIT::RECORD> {
  typedef RECORD_TYPE      TYPE_TYPE;
  typedef RECORD_TYPE_DATA TYPE_DATA_TYPE;
};

template <>
struct TYPE_DATA_TRAITS<TYPE_TRAIT::SIGNATURE> {
  typedef SIGNATURE_TYPE      TYPE_TYPE;
  typedef SIGNATURE_TYPE_DATA TYPE_DATA_TYPE;
};

template <>
struct TYPE_DATA_TRAITS<TYPE_TRAIT::SUBTYPE> {
  typedef SUBTYPE      TYPE_TYPE;
  typedef SUBTYPE_DATA TYPE_DATA_TYPE;
};

// compile time checking
template <int T>
class TYPE_SIZE {};

template <typename T, typename U>
class CHECK_SIZE_EQUAL {
public:
  static void* Constraints() {
    TYPE_SIZE<sizeof(T)>* s = (TYPE_SIZE<sizeof(U)>*)0;
    return s;
  }
};

template <typename T, uint32_t TGT_SZ>
class CHECK_SIZE_EQ_TO {
public:
  static void* Constraints() {
    TYPE_SIZE<sizeof(T)>* s = (TYPE_SIZE<sizeof(uint32_t) * TGT_SZ>*)0;
    return s;
  }
};

template <bool cond>
class COND_CHECK_TYPE {};

template <typename T, typename U>
class CHECK_SIZE_NO_BIGGER_THAN {
public:
  static void* Constraints() {
    COND_CHECK_TYPE<sizeof(T) <= sizeof(U)>* s = (COND_CHECK_TYPE<true>*)0;
    return s;
  }
};

template <typename T>
void Requires() {
  T::Constraints();
}

template <typename T>
struct HOST_PRIMITIVE_TYPE;

template <>
struct HOST_PRIMITIVE_TYPE<char>
    : std::integral_constant<PRIMITIVE_TYPE, PRIMITIVE_TYPE::INT_S8> {};

template <>
struct HOST_PRIMITIVE_TYPE<int16_t>
    : std::integral_constant<PRIMITIVE_TYPE, PRIMITIVE_TYPE::INT_S16> {};

template <>
struct HOST_PRIMITIVE_TYPE<int32_t>
    : std::integral_constant<PRIMITIVE_TYPE, PRIMITIVE_TYPE::INT_S32> {};

template <>
struct HOST_PRIMITIVE_TYPE<int64_t>
    : std::integral_constant<PRIMITIVE_TYPE, PRIMITIVE_TYPE::INT_S64> {};

template <>
struct HOST_PRIMITIVE_TYPE<unsigned char>
    : std::integral_constant<PRIMITIVE_TYPE, PRIMITIVE_TYPE::INT_U8> {};

template <>
struct HOST_PRIMITIVE_TYPE<uint16_t>
    : std::integral_constant<PRIMITIVE_TYPE, PRIMITIVE_TYPE::INT_U16> {};

template <>
struct HOST_PRIMITIVE_TYPE<uint32_t>
    : std::integral_constant<PRIMITIVE_TYPE, PRIMITIVE_TYPE::INT_U32> {};

template <>
struct HOST_PRIMITIVE_TYPE<uint64_t>
    : std::integral_constant<PRIMITIVE_TYPE, PRIMITIVE_TYPE::INT_U64> {};

template <>
struct HOST_PRIMITIVE_TYPE<float>
    : std::integral_constant<PRIMITIVE_TYPE, PRIMITIVE_TYPE::FLOAT_32> {};

template <>
struct HOST_PRIMITIVE_TYPE<double>
    : std::integral_constant<PRIMITIVE_TYPE, PRIMITIVE_TYPE::FLOAT_64> {};

template <class T>
inline constexpr PRIMITIVE_TYPE HOST_TYPE_ID = HOST_PRIMITIVE_TYPE<T>::value;

}  // namespace base
}  // namespace air

#endif
