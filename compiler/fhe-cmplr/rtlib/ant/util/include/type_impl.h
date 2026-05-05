//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_ANT_UTIL_INCLUDE_TYPE_IMPL_H
#define RTLIB_ANT_UTIL_INCLUDE_TYPE_IMPL_H

#include <stdbool.h>
#include <stdlib.h>

#include "util/type.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @brief Get type name
static const char* List_type_name(VALUE_LIST* list) {
  switch (LIST_TYPE(list)) {
    case UI32_TYPE:
      return "UI32";
    case I32_TYPE:
      return "I32";
    case I64_TYPE:
      return "I64";
    case UI64_TYPE:
      return "UI64";
    case DBL_TYPE:
      return "DBL";
    case DCMPLX_TYPE:
      return "DCMPLX";
    case BIGINT_TYPE:
      return "BIGINT";
    case PTR_TYPE:
      return "PTR";
    case VL_PTR_TYPE:
      return "VALUE_LIST";
    default:
      return "invalid";
  }
}

//! @brief Get unsigned int32 values from given value list
//! @param list value list
static inline uint32_t* Get_ui32_values(VALUE_LIST* list) {
  IS_TRUE(list && LIST_TYPE(list) == UI32_TYPE, "list is not UI32_TYPE");
  return UI32_VALUES(list);
}

//! @brief Get ui64 values from given value list
static inline uint64_t* Get_ui64_values(VALUE_LIST* list) {
  IS_TRUE(list && LIST_TYPE(list) == UI64_TYPE, "list is not UI64_TYPE");
  return UI64_VALUES(list);
}

//! @brief Set i64 value to VALUE_LIST at idx
static inline void Set_i64_value(VALUE_LIST* list, size_t idx, int64_t val) {
  IS_TRUE(list && LIST_TYPE(list) == I64_TYPE, "list is not I64_TYPE");
  IS_TRUE(idx < LIST_LEN(list), "idx outof bound");
  I64_VALUE_AT(list, idx) = val;
}

//! @brief Set ui64 value to VALUE_LIST at idx
static inline void Set_ui64_value(VALUE_LIST* list, size_t idx, uint64_t val) {
  IS_TRUE(list && LIST_TYPE(list) == UI64_TYPE, "list is not UI64_TYPE");
  IS_TRUE(idx < LIST_LEN(list), "idx outof bound");
  UI64_VALUE_AT(list, idx) = val;
}

//! @brief If given value list if PRIME_TYPE
//! @param vl value list
static inline bool Is_prim_type(VALUE_LIST* vl) {
  if (LIST_TYPE(vl) & PRIM_TYPE) {
    return true;
  } else {
    return false;
  }
}

//! @brief If given value list is valid
//! @param vl value list
static inline bool Is_valid(VALUE_LIST* vl) {
  if (vl == NULL) return false;
  switch (LIST_TYPE(vl)) {
    case UI32_TYPE:
    case I32_TYPE:
    case I64_TYPE:
    case UI64_TYPE:
    case I128_TYPE:
    case PTR_TYPE:
    case VL_PTR_TYPE:
    case DBL_TYPE:
    case DCMPLX_TYPE:
    case BIGINT_TYPE:
      return true;
    default:
      return false;
  }
  return false;
}

//! @brief If value list is empty
//! @param list value list
static inline bool Is_empty(VALUE_LIST* list) {
  return !(list && LIST_LEN(list) > 0 && (ANY_VALUES(list) != NULL));
}

//! @brief Initialize value list for ANY_VAL
static inline void Init_value_list(VALUE_LIST* list, VALUE_TYPE type,
                                   size_t len, ANY_VAL* init_vals) {
  IS_TRUE(list, "list is null");
  if (init_vals != NULL && list->_vals._a == init_vals) return;

  size_t mem_size = Value_mem_size(type) * len;
  if (list->_vals._a == NULL) {
    list->_vals._a = (ANY_VAL*)malloc(mem_size);
    list->_length  = len;
  } else {
    IS_TRUE(list->_type == type && list->_length >= len,
            "list already initialized with smaller type or length");
  }
  if (init_vals == NULL) {
    memset(list->_vals._a, 0, mem_size);
  } else {
    memcpy(list->_vals._a, init_vals, mem_size);
  }
  list->_type = type;
}

//! @brief Free elements of value list
static inline void Free_value_list_elems(VALUE_LIST* list) {
  if (list == NULL) return;
  if (list->_vals._a) {
    if (LIST_TYPE(list) == BIGINT_TYPE) {
      BIG_INT* bi = BIGINT_VALUES(list);
      for (size_t idx = 0; idx < LIST_LEN(list); idx++) {
        BI_FREES(bi[idx]);
      }
    } else if (LIST_TYPE(list) == VL_PTR_TYPE) {
      for (size_t idx = 0; idx < LIST_LEN(list); idx++) {
        VALUE_LIST* vl = VL_VALUE_AT(list, idx);
        Free_value_list(vl);
      }
    }
    free(list->_vals._a);
    list->_vals._a = NULL;
  }
}

//! @brief Print linke list
void Print_link_list(FILE* fp, LL* ll);

//! @brief Free link list node
static inline void Free_ll_node(LL_NODE* node) { free(node); }

//! @brief Print linke list
static inline void Print_ll(FILE* fp, LL* ll) {
  if (ll == NULL) return;
  FOR_ALL_LL_ELEM(ll, node) { fprintf(fp, "%d ", node->_val); }
}

//! @brief Decrease count of link list
static inline void Dec_link_list_cnt(LL* ll) {
  IS_TRUE(ll, "null link list");
  ll->_cnt--;
}

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_ANT_UTIL_INCLUDE_TYPE_IMPL_H
