//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_ST_DECL_H
#define AIR_BASE_ST_DECL_H

#include "air/base/arena.h"
#include "air/base/ptr_wrapper.h"
#include "air/base/st_enum.h"

namespace air {
namespace base {

class SCOPE_BASE;
class GLOB_SCOPE;
class FUNC_SCOPE;
class AUX_TAB;
class CONTAINER;

class ADDR_DATUM;
class ARRAY_TYPE;
class BLOCK;
class COND;
class CONSTANT;
class ENTRY;
class FIELD;
class FUNC;
class LABEL;
class PACKET;
class PARAM;
class POINTER_TYPE;
class PRIM_TYPE;
class RECORD_TYPE;
class REGION_INFO;
class SIGNATURE_TYPE;
class SRC_FILE;
class STR;
class SUBTYPE;
class SYM;
class THUNK;
class PREG;
class TYPE;
class VA_LIST_TYPE;
class ARB;
class ATTR;

typedef PTR<ADDR_DATUM>              ADDR_DATUM_PTR;
typedef PTR_TO_CONST<ADDR_DATUM>     CONST_ADDR_DATUM_PTR;
typedef PTR<ARRAY_TYPE>              ARRAY_TYPE_PTR;
typedef PTR_TO_CONST<ARRAY_TYPE>     CONST_ARRAY_TYPE_PTR;
typedef PTR<BLOCK>                   BLOCK_PTR;
typedef PTR_TO_CONST<BLOCK>          CONST_BLOCK_PTR;
typedef PTR<COND>                    COND_PTR;
typedef PTR_TO_CONST<COND>           CONST_COND_PTR;
typedef PTR<CONSTANT>                CONSTANT_PTR;
typedef PTR_TO_CONST<CONSTANT>       CONST_CONSTANT_PTR;
typedef PTR<ENTRY>                   ENTRY_PTR;
typedef PTR_TO_CONST<ENTRY>          CONST_ENTRY_PTR;
typedef PTR<FIELD>                   FIELD_PTR;
typedef PTR_TO_CONST<FIELD>          CONST_FIELD_PTR;
typedef PTR<FUNC>                    FUNC_PTR;
typedef PTR_TO_CONST<FUNC>           CONST_FUNC_PTR;
typedef PTR<LABEL>                   LABEL_PTR;
typedef PTR_TO_CONST<LABEL>          CONST_LABEL_PTR;
typedef PTR<PARAM>                   PARAM_PTR;
typedef PTR_TO_CONST<PARAM>          CONST_PARAM_PTR;
typedef PTR<POINTER_TYPE>            POINTER_TYPE_PTR;
typedef PTR_TO_CONST<POINTER_TYPE>   CONST_POINTER_TYPE_PTR;
typedef PTR<PRIM_TYPE>               PRIM_TYPE_PTR;
typedef PTR_TO_CONST<PRIM_TYPE>      CONST_PRIM_TYPE_PTR;
typedef PTR<RECORD_TYPE>             RECORD_TYPE_PTR;
typedef PTR_TO_CONST<RECORD_TYPE>    CONST_RECORD_TYPE_PTR;
typedef PTR<REGION_INFO>             REGION_INFO_PTR;
typedef PTR_TO_CONST<REGION_INFO>    CONST_REGION_INFO_PTR;
typedef PTR<SIGNATURE_TYPE>          SIGNATURE_TYPE_PTR;
typedef PTR_TO_CONST<SIGNATURE_TYPE> CONST_SIGNATURE_TYPE_PTR;
typedef PTR<SRC_FILE>                FILE_PTR;
typedef PTR_TO_CONST<SRC_FILE>       CONST_FILE_PTR;
typedef PTR<STR>                     STR_PTR;
typedef PTR_TO_CONST<STR>            CONST_STR_PTR;
typedef PTR<SUBTYPE>                 SUBTYPE_PTR;
typedef PTR_TO_CONST<SUBTYPE>        CONST_SUBTYPE_PTR;
typedef PTR<SYM>                     SYM_PTR;
typedef PTR_TO_CONST<SYM>            CONST_SYM_PTR;
typedef PTR<PREG>                    PREG_PTR;
typedef PTR_TO_CONST<PREG>           CONST_PREG_PTR;
typedef PTR<TYPE>                    TYPE_PTR;
typedef PTR_TO_CONST<TYPE>           CONST_TYPE_PTR;
typedef PTR<VA_LIST_TYPE>            VA_LIST_TYPE_PTR;
typedef PTR_TO_CONST<VA_LIST_TYPE>   CONST_VA_LIST_TYPE_PTR;
typedef PTR<PACKET>                  PACKET_PTR;
typedef PTR_TO_CONST<PACKET>         CONST_PACKET_PTR;
// typedef ARB*                         ARB_PTR;
// typedef const ARB*                   CONST_ARB_PTR;
typedef PTR<ARB>           ARB_PTR;
typedef PTR_TO_CONST<ARB>  CONST_ARB_PTR;
typedef PTR<ATTR>          ATTR_PTR;
typedef PTR_TO_CONST<ATTR> CONST_ATTR_PTR;

class INT_LITERAL;
class FLOAT_LITERAL;
class COMPLEX_LITERAL;

class ATTR_DATA;
class ARRAY_TYPE_DATA;
class AUX_DATA;
class AUX_SYM_DATA;
class AUX_SYM_ID_DATA;
class AUX_INIT_ENTRY_DATA;
class BLOCK_DATA;
class COND_DATA;
class CONSTANT_DATA;
class FIELD_DATA;
class FILE_DATA;
class FUNC_DEF_DATA;
class LABEL_DATA;
class PARAM_DATA;
class POINTER_TYPE_DATA;
class PRIM_TYPE_DATA;
class RECORD_TYPE_DATA;
class REGION_INFO_DATA;
class SIGNATURE_TYPE_DATA;
class STR_DATA;
class SUBTYPE_DATA;
class SYM_DATA;
class PREG_DATA;
class TYPE_DATA;
class VA_LIST_TYPE_DATA;
class ARB_DATA;

typedef PTR_FROM_DATA<AUX_DATA>          AUX_DATA_PTR;
typedef PTR_FROM_DATA<AUX_SYM_DATA>      AUX_SYM_DATA_PTR;
typedef PTR_FROM_DATA<BLOCK_DATA>        BLOCK_DATA_PTR;
typedef PTR_FROM_DATA<COND_DATA>         COND_DATA_PTR;
typedef PTR_FROM_DATA<CONSTANT_DATA>     CONSTANT_DATA_PTR;
typedef PTR_FROM_DATA<FIELD_DATA>        FIELD_DATA_PTR;
typedef PTR_FROM_DATA<FILE_DATA>         FILE_DATA_PTR;
typedef PTR_FROM_DATA<FUNC_DEF_DATA>     FUNC_DEF_DATA_PTR;
typedef PTR_FROM_DATA<LABEL_DATA>        LABEL_DATA_PTR;
typedef PTR_FROM_DATA<PARAM_DATA>        PARAM_DATA_PTR;
typedef PTR_FROM_DATA<REGION_INFO_DATA>  REGION_INFO_DATA_PTR;
typedef PTR_FROM_DATA<STR_DATA>          STR_DATA_PTR;
typedef PTR_FROM_DATA<SYM_DATA>          SYM_DATA_PTR;
typedef PTR_FROM_DATA<PREG_DATA>         PREG_DATA_PTR;
typedef PTR_FROM_DATA<TYPE_DATA>         TYPE_DATA_PTR;
typedef PTR_FROM_DATA<VA_LIST_TYPE_DATA> VA_LIST_TYPE_DATA_PTR;
typedef PTR_FROM_DATA<SUBTYPE_DATA>      SUBTYPE_DATA_PTR;
typedef PTR_FROM_DATA<ARB_DATA>          ARB_DATA_PTR;
typedef PTR_FROM_DATA<ATTR_DATA>         ATTR_DATA_PTR;

template <typename T>
bool operator==(ID_BASE<T> id, NULL_PTR) {
  return id.Is_null();
}
template <typename T>
bool operator!=(ID_BASE<T> id, NULL_PTR) {
  return !(id == Null_ptr);
}
template <typename T>
bool operator==(NULL_PTR, ID_BASE<T> id) {
  return id == Null_ptr;
}
template <typename T>
bool operator!=(NULL_PTR, ID_BASE<T> id) {
  return !(id == Null_ptr);
}

typedef ID<ARB_DATA>  ARB_ID;
typedef ID<ATTR_DATA> ATTR_ID;

typedef ID_BASE<AUX_DATA>      AUX_ID;
typedef ID_BASE<BLOCK>         BLOCK_ID;
typedef ID_BASE<COND>          COND_ID;
typedef ID_BASE<CONSTANT>      CONSTANT_ID;
typedef ID_BASE<FIELD>         FIELD_ID;
typedef ID_BASE<SRC_FILE>      FILE_ID;
typedef ID_BASE<FUNC_DEF_DATA> FUNC_DEF_ID;
typedef ID_BASE<LABEL>         LABEL_ID;
typedef ID_BASE<PARAM>         PARAM_ID;
typedef ID_BASE<REGION_INFO>   REGION_INFO_ID;
typedef ID_BASE<char*>         STR_ID;
typedef ID_BASE<SYM>           SYM_ID;
typedef ID_BASE<TYPE>          TYPE_ID;
typedef ID_BASE<PREG>          PREG_ID;

template <SYMBOL_CLASS T>
class SPEC_SYM_ID : public SYM_ID {
public:
  SPEC_SYM_ID() : SYM_ID() {}
  explicit SPEC_SYM_ID(uint32_t id) : SYM_ID(id) {}
  SPEC_SYM_ID(uint32_t idx, uint32_t scope) : SYM_ID(idx, scope) {}
};

typedef SPEC_SYM_ID<SYMBOL_CLASS::ENTRY>      ENTRY_ID;
typedef SPEC_SYM_ID<SYMBOL_CLASS::FUNC>       FUNC_ID;
typedef SPEC_SYM_ID<SYMBOL_CLASS::THUNK>      THUNK_ID;
typedef SPEC_SYM_ID<SYMBOL_CLASS::PACKET>     PACKET_ID;
typedef SPEC_SYM_ID<SYMBOL_CLASS::ADDR_DATUM> ADDR_DATUM_ID;

template <typename V, typename B>
class TAB_ITER_BASE;
template <class V, class B, class S>
class TAB_ITER;
template <SYMBOL_CLASS T>
class SYM_SEL;

class ADDR_DATUM_SEL;
typedef SYM_SEL<SYMBOL_CLASS::FUNC>   FUNC_SEL;
typedef SYM_SEL<SYMBOL_CLASS::VAR>    VAR_SEL;
typedef SYM_SEL<SYMBOL_CLASS::FORMAL> FORMAL_SEL;

typedef TAB_ITER<ADDR_DATUM, SYM_ID, ADDR_DATUM_SEL> DATUM_ITER;
typedef TAB_ITER<ADDR_DATUM, SYM_ID, VAR_SEL>        VAR_ITER;
typedef TAB_ITER<ADDR_DATUM, SYM_ID, FORMAL_SEL>     FORMAL_ITER;
typedef TAB_ITER<FUNC, SYM_ID, FUNC_SEL>             FUNC_ITER;

class FILE_ITER;
class FIELD_ITER;
class STR_ITER;
class TYPE_ITER;
class CONSTANT_ITER;
class PARAM_ITER;
class ELEM_CONST_ITER;
class FIELD_CONST_ITER;
class ARB_ITER;
class DIM_ITER;
class ATTR_ITER;
class PREG_ITER;

uint32_t constexpr INDENT_SPACE = 2;

}  // namespace base
}  // namespace air

#endif
