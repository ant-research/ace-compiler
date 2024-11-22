//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_INCLUDE_FHE_TYPES_H
#define RTLIB_INCLUDE_FHE_TYPES_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "common/error.h"
#include "uthash.h"
#include "util/fhe_bignumber.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846 /* pi */
#endif

// To ensure consistency in the naming convention for UT_hash_handle
#define HH hh

#ifndef AUXBITS
#define AUXBITS 60
#endif

#define TRUE  1
#define FALSE 0

#if __cplusplus
#include <complex>
typedef std::complex<double> DCMPLX;
#else
#include <complex.h>
typedef double complex DCMPLX;
#endif

typedef char                      ANY_VAL;
typedef char*                     PTR;
__extension__ typedef __int128    INT128_T;
__extension__ typedef __uint128_t UINT128_T;

/**
 * @brief type of VALUE_LIST
 *
 */
typedef enum {
  INVALID_TYPE = 0,
  DBL_TYPE     = 0x1,
  DCMPLX_TYPE  = 0x10,
  UI32_TYPE    = 0x100,
  I32_TYPE     = 0x200,
  I64_TYPE     = 0x400,
  UI64_TYPE    = 0x800,
  I128_TYPE    = 0x1000,
  BIGINT_TYPE  = 0x2000,
  PTR_TYPE     = 0x4000,
  VL_PTR_TYPE  = 0x8000,  // VALUE_LIST PTR TYPE
  PRIM_TYPE    = DBL_TYPE | DCMPLX_TYPE | UI32_TYPE | I32_TYPE | I64_TYPE |
              UI64_TYPE | I128_TYPE,
} VALUE_TYPE;

/**
 * @brief VALUE_LIST to store a list of int64/double/DCMPLX/BIG_INT values
 *
 */
typedef struct VALUE_LIST {
  VALUE_TYPE _type;
  size_t     _length;
  union {
    ANY_VAL*            _a;     // any values
    int32_t*            _i32;   // int32 values
    uint32_t*           _ui32;  // uint32 values
    int64_t*            _i;     // int64 values
    uint64_t*           _u;     // uint64 values
    INT128_T*           _i128;  // int128 values
    PTR*                _p;     // ptr values
    struct VALUE_LIST** _v;     // VALUE_LIST ptr values
    double*             _d;     // double values
    DCMPLX*             _c;     // double complex values
    BIG_INT*            _b;     // big integer values
  } _vals;
} VALUE_LIST;

#define I32_VALUE_AT(list, idx)    (list)->_vals._i32[idx]
#define UI32_VALUE_AT(list, idx)   (list)->_vals._ui32[idx]
#define I64_VALUE_AT(list, idx)    (list)->_vals._i[idx]
#define UI64_VALUE_AT(list, idx)   (list)->_vals._u[idx]
#define I128_VALUE_AT(list, idx)   (list)->_vals._i128[idx]
#define PTR_VALUE_AT(list, idx)    (list)->_vals._p[idx]
#define VL_VALUE_AT(list, idx)     (list)->_vals._v[idx]
#define DBL_VALUE_AT(list, idx)    (list)->_vals._d[idx]
#define DCMPLX_VALUE_AT(list, idx) (list)->_vals._c[idx]
#define BIGINT_VALUE_AT(list, idx) (list)->_vals._b[idx]
#define ANY_VALUES(list)           (list)->_vals._a
#define I64_VALUES(list)           (list)->_vals._i
#define UI64_VALUES(list)          (list)->_vals._u
#define I32_VALUES(list)           (list)->_vals._i32
#define UI32_VALUES(list)          (list)->_vals._ui32
#define DBL_VALUES(list)           (list)->_vals._d
#define DCMPLX_VALUES(list)        (list)->_vals._c
#define BIGINT_VALUES(list)        (list)->_vals._b
#define LIST_LEN(list)             (list)->_length
#define LIST_TYPE(list)            (list)->_type

typedef VALUE_LIST VL_DBL;        // VALUE_LIST<double>
typedef VALUE_LIST VL_UI32;       // VALUE_LIST<uint32_t>
typedef VALUE_LIST VL_I32;        // VALUE_LIST<int32_t>
typedef VALUE_LIST VL_VL_I32;     // VALUE_LIST<VALUE_LIST<int32_t>>
typedef VALUE_LIST VL_PTR;        // VALUE_LIST<PTR>
typedef VALUE_LIST VL_VL_PTR;     // VALUE_LIST<VALUE_LIST<PTR>>>
typedef VALUE_LIST VL_DCMPLX;     // VALUE_LIST<DCMPLX>
typedef VALUE_LIST VL_VL_DCMPLX;  // VALUE_LIST<VALUE_LIST<DCMPLX>>
typedef VALUE_LIST
    VL_VL_VL_DCMPLX;  // VALUE_LIST<VALUE_LIST<VALUE_LIST<DCMPLX>>>

#define VL_L2_VALUE_AT(list, idx1, idx2) \
  VL_VALUE_AT(VL_VALUE_AT(list, idx1), idx2)
#define VL_L3_VALUE_AT(list, idx1, idx2, idx3) \
  VL_VALUE_AT(VL_L2_VALUE_AT(list, idx2, idx2), idx3)
#define VL_L4_VALUE_AT(list, idx1, idx2, idx3, idx4) \
  VL_VALUE_AT(VL_L3_VALUE_AT(list, idx1, idx2, idx3), idx4)
#define FOR_ALL_ELEM(list, idx) \
  for (size_t idx = 0; idx < LIST_LEN(list); idx++)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief get i32 values from given value list
 *
 * @param list value list
 * @return int32_t*
 */
static inline int32_t* Get_i32_values(VALUE_LIST* list) {
  IS_TRUE(list && LIST_TYPE(list) == I32_TYPE, "list is not I32_TYPE");
  return list->_vals._i32;
}

//! @brief get unsigned int32 values from given value list
//! @param list value list
//! @return uint32_t*
static inline uint32_t* Get_ui32_values(VALUE_LIST* list) {
  IS_TRUE(list && LIST_TYPE(list) == UI32_TYPE, "list is not UI32_TYPE");
  return list->_vals._ui32;
}

/**
 * @brief get i64 values from given value list
 *
 * @param list value list
 * @return int64_t*
 */
static inline int64_t* Get_i64_values(VALUE_LIST* list) {
  IS_TRUE(list && LIST_TYPE(list) == I64_TYPE, "list is not I64_TYPE");
  return list->_vals._i;
}

/**
 * @brief get ui64 values from given value list
 *
 * @param list value list
 * @return uint64_t*
 */
static inline uint64_t* Get_ui64_values(VALUE_LIST* list) {
  IS_TRUE(list && LIST_TYPE(list) == UI64_TYPE, "list is not UI64_TYPE");
  return list->_vals._u;
}

//! @brief get i128 values from given value list
//! @param list value list
//! @return INT128_T*
static inline INT128_T* Get_i128_values(VALUE_LIST* list) {
  IS_TRUE(list && LIST_TYPE(list) == I128_TYPE, "list is not I128_TYPE");
  return list->_vals._i128;
}

/**
 * @brief get ptr values from given value list
 *
 * @param list value list
 * @return PTR*
 */
static inline PTR* Get_ptr_values(VALUE_LIST* list) {
  IS_TRUE(list && LIST_TYPE(list) == PTR_TYPE, "list is not PTR_TYPE");
  return list->_vals._p;
}

/**
 * @brief get value list values from given value list
 *
 * @param list value list
 * @return VALUE_LIST*
 */
static inline VALUE_LIST** Get_vl_values(VALUE_LIST* list) {
  IS_TRUE(list && LIST_TYPE(list) == PTR_TYPE, "list is not PTR_TYPE");
  return list->_vals._v;
}

/**
 * @brief get double values from given value list
 *
 * @param list value list
 * @return double*
 */
static inline double* Get_dbl_values(VALUE_LIST* list) {
  IS_TRUE(list && LIST_TYPE(list) == DBL_TYPE, "list is not DBL_TYPE");
  return list->_vals._d;
}

/**
 * @brief get complex values from given value list
 *
 * @param list value list
 * @return DCMPLX*
 */
static inline DCMPLX* Get_dcmplx_values(VALUE_LIST* list) {
  IS_TRUE(list && LIST_TYPE(list) == DCMPLX_TYPE, "list is not DCMPLX_TYPE");
  return list->_vals._c;
}

/**
 * @brief get bigint values from given value list
 *
 * @param list value list
 * @return BIG_INT*
 */
static inline BIG_INT* Get_bint_values(VALUE_LIST* list) {
  IS_TRUE(list && LIST_TYPE(list) == BIGINT_TYPE, "list is not BIGINT_TYPE");
  return list->_vals._b;
}

/**
 * @brief get i32 value at idx from given value list
 *
 * @param list value list
 * @param idx
 * @return int32_t*
 */
static inline int32_t Get_i32_value_at(VALUE_LIST* list, size_t idx) {
  IS_TRUE(list && LIST_TYPE(list) == I32_TYPE, "list is not I32_TYPE");
  IS_TRUE(idx < LIST_LEN(list), "idx outof bound");
  return I32_VALUE_AT(list, idx);
}

/**
 * @brief get ui32 value at idx from given value list
 *
 * @param list value list
 * @param idx
 * @return uint32_t*
 */
static inline uint32_t Get_ui32_value_at(VALUE_LIST* list, size_t idx) {
  IS_TRUE(list && LIST_TYPE(list) == UI32_TYPE, "list is not I32_TYPE");
  IS_TRUE(idx < LIST_LEN(list), "idx outof bound");
  return UI32_VALUE_AT(list, idx);
}

/**
 * @brief get i64 value at idx from given value list
 *
 * @param list value list
 * @param idx
 * @return int64_t*
 */
static inline int64_t Get_i64_value_at(VALUE_LIST* list, size_t idx) {
  IS_TRUE(list && LIST_TYPE(list) == I64_TYPE, "list is not I64_TYPE");
  IS_TRUE(idx < LIST_LEN(list), "idx outof bound");
  return I64_VALUE_AT(list, idx);
}

/**
 * @brief get ui64 value at idx from given value list
 *
 * @param list value list
 * @param idx
 * @return uint64_t*
 */
static inline uint64_t Get_ui64_value_at(VALUE_LIST* list, size_t idx) {
  IS_TRUE(list && LIST_TYPE(list) == UI64_TYPE, "list is not UI64_TYPE");
  IS_TRUE(idx < LIST_LEN(list), "idx outof bound");
  return UI64_VALUE_AT(list, idx);
}

//! @brief get i128 value at idx from given value list
//! @param list value list
//! @param idx
//! @return INT128_T
static inline INT128_T Get_i128_value_at(VALUE_LIST* list, size_t idx) {
  IS_TRUE(list && LIST_TYPE(list) == I128_TYPE, "list is not I128_TYPE");
  IS_TRUE(idx < LIST_LEN(list), "idx outof bound");
  return I128_VALUE_AT(list, idx);
}

/**
 * @brief get ptr value at idx from given value list
 *
 * @param list value list
 * @param idx
 * @return PTR*
 */
static inline PTR Get_ptr_value_at(VALUE_LIST* list, size_t idx) {
  IS_TRUE(list && LIST_TYPE(list) == PTR_TYPE, "list is not PTR_TYPE");
  IS_TRUE(idx < LIST_LEN(list), "idx outof bound");
  return PTR_VALUE_AT(list, idx);
}

/**
 * @brief get value list value at idx from given value list
 *
 * @param list value list
 * @param idx
 * @return VALUE_LIST*
 */
static inline VALUE_LIST* Get_vl_value_at(VALUE_LIST* list, size_t idx) {
  IS_TRUE(list && LIST_TYPE(list) == VL_PTR_TYPE, "list is not PTR_TYPE");
  IS_TRUE(idx < LIST_LEN(list), "idx outof bound");
  return VL_VALUE_AT(list, idx);
}

/**
 * @brief get value list value at [idx1, idx2] from given value list
 *
 * @param list value list
 * @param idx1
 * @param idx2
 * @return VALUE_LIST*
 */
static inline VALUE_LIST* Get_vl_l2_value_at(VALUE_LIST* list, size_t idx1,
                                             size_t idx2) {
  IS_TRUE(list && LIST_TYPE(list) == VL_PTR_TYPE, "list is not PTR_TYPE");
  IS_TRUE(idx1 < LIST_LEN(list), "idx outof bound");
  return Get_vl_value_at(VL_VALUE_AT(list, idx1), idx2);
}

/**
 * @brief get value list value at [idx1, idx2, idx3] from given value list
 *
 * @param list value list
 * @param idx1
 * @param idx2
 * @param idx3
 * @return VALUE_LIST*
 */
static inline VALUE_LIST* Get_vl_l3_value_at(VALUE_LIST* list, size_t idx1,
                                             size_t idx2, size_t idx3) {
  IS_TRUE(list && LIST_TYPE(list) == VL_PTR_TYPE, "list is not PTR_TYPE");
  IS_TRUE(idx1 < LIST_LEN(list), "idx outof bound");
  return Get_vl_l2_value_at(VL_VALUE_AT(list, idx1), idx2, idx3);
}

/**
 * @brief get size of given value list
 *
 * @param list value list
 * @return size_t
 */
static inline size_t Get_dim1_size(VALUE_LIST* list) {
  IS_TRUE(list && LIST_TYPE(list) == VL_PTR_TYPE, "null value VL_PTR");
  return LIST_LEN(list);
}

/**
 * @brief get size of given value list list[0]
 *
 * @param list value list
 * @return size_t
 */
static inline size_t Get_dim2_size(VALUE_LIST* list) {
  IS_TRUE(list && LIST_TYPE(list) == VL_PTR_TYPE, "list is not VL_PTR type");
  return LIST_LEN(Get_vl_value_at(list, 0));
}

/**
 * @brief get size of given value list list[list[0]]
 *
 * @param list value list
 * @return size_t
 */
static inline size_t Get_dim3_size(VALUE_LIST* list) {
  IS_TRUE(list && LIST_TYPE(list) == VL_PTR_TYPE, "list is not VL_PTR type");
  return LIST_LEN(Get_vl_l2_value_at(list, 0, 0));
}

/**
 * @brief get double value at idx from given value list
 *
 * @param list value list
 * @param idx
 * @return double
 */
static inline double Get_dbl_value_at(VALUE_LIST* list, size_t idx) {
  IS_TRUE(list && LIST_TYPE(list) == DBL_TYPE, "list is not DBL_TYPE");
  IS_TRUE(idx < LIST_LEN(list), "idx outof bound");
  return DBL_VALUE_AT(list, idx);
}

/**
 * @brief get complex value at idx from given value list
 *
 * @param list value list
 * @param idx
 * @return DCMPLX
 */
static inline DCMPLX Get_dcmplx_value_at(VALUE_LIST* list, size_t idx) {
  IS_TRUE(list && LIST_TYPE(list) == DCMPLX_TYPE, "list is not DCMPLX_TYPE");
  IS_TRUE(idx < LIST_LEN(list), "idx outof bound");
  return DCMPLX_VALUE_AT(list, idx);
}

/**
 * @brief get bigint value at idx from given value list
 * NOTE: Big Integer is actully an array, cannot directly return it
 * return the address instead
 *
 * @param list value list
 * @param idx
 * @return BIG_INT*
 */
static inline BIG_INT* Get_bint_value_at(VALUE_LIST* list, size_t idx) {
  IS_TRUE(list && LIST_TYPE(list) == BIGINT_TYPE, "list is not BIGINT_TYPE");
  IS_TRUE(idx < LIST_LEN(list), "idx outof bound");
  return &(list->_vals._b[idx]);
}

/**
 * @brief set ui32 value to VALUE_LIST at idx
 *
 * @param list value list
 * @param idx idx of value list
 * @param val ui32 value
 */
static inline void Set_ui32_value(VALUE_LIST* list, size_t idx, uint32_t val) {
  IS_TRUE(list && LIST_TYPE(list) == UI32_TYPE, "list is not UI32_TYPE");
  IS_TRUE(idx < LIST_LEN(list), "idx outof bound");
  UI32_VALUE_AT(list, idx) = val;
}

/**
 * @brief set i64 value to VALUE_LIST at idx
 *
 * @param list value list
 * @param idx idx of value list
 * @param val i64 value
 */
static inline void Set_i64_value(VALUE_LIST* list, size_t idx, int64_t val) {
  IS_TRUE(list && LIST_TYPE(list) == I64_TYPE, "list is not I64_TYPE");
  IS_TRUE(idx < LIST_LEN(list), "idx outof bound");
  I64_VALUE_AT(list, idx) = val;
}

/**
 * @brief set ui64 value to VALUE_LIST at idx
 *
 * @param list value list
 * @param idx idx of value list
 * @param val ui64 value
 */
static inline void Set_ui64_value(VALUE_LIST* list, size_t idx, uint64_t val) {
  IS_TRUE(list && LIST_TYPE(list) == UI64_TYPE, "list is not UI64_TYPE");
  IS_TRUE(idx < LIST_LEN(list), "idx outof bound");
  UI64_VALUE_AT(list, idx) = val;
}

//! @brief set ui64 value to VALUE_LIST at idx
//! @param list value list
//! @param idx idx of value list
//! @param val ui64 value
static inline void Set_i128_value(VALUE_LIST* list, size_t idx, INT128_T val) {
  IS_TRUE(list && LIST_TYPE(list) == I128_TYPE, "list is not I128_TYPE");
  IS_TRUE(idx < LIST_LEN(list), "idx outof bound");
  I128_VALUE_AT(list, idx) = val;
}

/**
 * @brief set double value to VALUE_LIST at idx
 *
 * @param list value list
 * @param idx idx of value list
 * @param val double value
 */
static inline void Set_dbl_value(VALUE_LIST* list, size_t idx, double val) {
  IS_TRUE(list && LIST_TYPE(list) == DBL_TYPE, "list is not DBL_TYPE");
  IS_TRUE(idx < LIST_LEN(list), "idx outof bound");
  DBL_VALUE_AT(list, idx) = val;
}

/**
 * @brief set complex value to VALUE_LIST at idx
 *
 * @param list value list
 * @param idx idx of value list
 * @param val complex value
 */
static inline void Set_dcmplx_value(VALUE_LIST* list, size_t idx, DCMPLX val) {
  IS_TRUE(list && LIST_TYPE(list) == DCMPLX_TYPE, "list is not DCMPLX_TYPE");
  IS_TRUE(idx < LIST_LEN(list), "idx outof bound");
  DCMPLX_VALUE_AT(list, idx) = val;
}

/**
 * @brief set bigint value to VALUE_LIST at idx
 *
 * @param list value list
 * @param idx idx of value list
 * @param val bigint value
 */
static inline void Set_bint_value(VALUE_LIST* list, size_t idx, BIG_INT val) {
  IS_TRUE(list && LIST_TYPE(list) == BIGINT_TYPE, "list is not BIGINT_TYPE");
  IS_TRUE(idx < LIST_LEN(list), "idx outof bound");
  BI_ASSIGN(BIGINT_VALUE_AT(list, idx), val);
}

/**
 * @brief set ptr value to VALUE_LIST at idx
 *
 * @param list value list
 * @param idx idx of value list
 * @param val ptr value
 */
static inline void Set_ptr_value(VALUE_LIST* list, size_t idx, PTR val) {
  IS_TRUE(list && LIST_TYPE(list) == PTR_TYPE, "list is not BIGINT_TYPE");
  IS_TRUE(idx < LIST_LEN(list), "idx outof bound");
  PTR_VALUE_AT(list, idx) = val;
}

/**
 * @brief get memory size of given VALUE_TYPE
 *
 * @param type type of VALUE_LIST
 * @return memory size
 */
static inline size_t Value_mem_size(VALUE_TYPE type) {
  switch (type) {
    case UI32_TYPE:
      return sizeof(uint32_t);
    case I32_TYPE:
      return sizeof(int32_t);
    case I64_TYPE:
      return sizeof(int64_t);
    case UI64_TYPE:
      return sizeof(uint64_t);
    case I128_TYPE:
      return sizeof(INT128_T);
    case PTR_TYPE:
      return sizeof(char*);
    case VL_PTR_TYPE:
      return sizeof(VALUE_LIST*);
    case DBL_TYPE:
      return sizeof(double);
    case DCMPLX_TYPE:
      return sizeof(DCMPLX);
    case BIGINT_TYPE:
      return sizeof(BIG_INT);
    default:
      IS_TRUE(FALSE, "unsupported");
  }
  return 0;
}

/**
 * @brief alloc value list from given type & length
 *
 * @param type type of value list
 * @param len length of value list
 * @return VALUE_LIST*
 */
static inline VALUE_LIST* Alloc_value_list(VALUE_TYPE type, size_t len) {
  VALUE_LIST* ret = (VALUE_LIST*)malloc(sizeof(VALUE_LIST));
  ret->_type      = type;
  ret->_length    = len;
  if (len > 0) {
    size_t mem_size = Value_mem_size(type) * len;
    ret->_vals._a   = (ANY_VAL*)malloc(mem_size);
    if (type == BIGINT_TYPE) {
      for (size_t idx = 0; idx < len; idx++) {
        BI_INIT(ret->_vals._b[idx]);
      }
    } else {
      memset(ret->_vals._a, 0, mem_size);
    }
  } else {
    ret->_vals._a = NULL;
  }
  return ret;
}

/**
 * @brief alloc 2-D value list from given type & length
 *
 * @param type type of value list
 * @param dim1 length of the first level
 * @param dim2 length of the second level
 * @return VALUE_LIST*
 */
static inline VALUE_LIST* Alloc_2d_value_list(VALUE_TYPE type, size_t dim1,
                                              size_t dim2) {
  VALUE_LIST* vl = Alloc_value_list(VL_PTR_TYPE, dim1);
  FOR_ALL_ELEM(vl, idx) {
    VALUE_LIST* vl2      = Alloc_value_list(type, dim2);
    VL_VALUE_AT(vl, idx) = vl2;
  }
  return vl;
}

/**
 * @brief alloc 3-D value list from given type & length
 *
 * @param type type of value list
 * @param dim1 length of the first level
 * @param dim2 length of the second level
 * @param dim3 length of the third level
 * @return VALUE_LIST*
 */
static inline VALUE_LIST* Alloc_3d_value_list(VALUE_TYPE type, size_t dim1,
                                              size_t dim2, size_t dim3) {
  VALUE_LIST* vl = Alloc_value_list(VL_PTR_TYPE, dim1);
  FOR_ALL_ELEM(vl, idx) {
    VALUE_LIST* vl2      = Alloc_value_list(VL_PTR_TYPE, dim2);
    VL_VALUE_AT(vl, idx) = vl2;
    FOR_ALL_ELEM(vl2, idx2) {
      VALUE_LIST* vl3        = Alloc_value_list(type, dim3);
      VL_VALUE_AT(vl2, idx2) = vl3;
    }
  }
  return vl;
}

/**
 * @brief if given value list if PRIME_TYPE
 *
 * @param vl value list
 * @return true
 * @return false
 */
static inline bool Is_prim_type(VALUE_LIST* vl) {
  if (LIST_TYPE(vl) & PRIM_TYPE) {
    return true;
  } else {
    return false;
  }
}

/**
 * @brief if given value list is valid
 *
 * @param vl value list
 * @return true
 * @return false
 */
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

/**
 * @brief resize value list of double
 *
 * @param vl value list
 * @param len new length of value list
 * @param value new value
 */
static inline void Resize_dbl_value_list(VALUE_LIST* vl, size_t len,
                                         double value) {
  IS_TRUE(vl, "null value list");
  IS_TRUE(vl->_type == DBL_TYPE, "not double type");
  size_t old_len = LIST_LEN(vl);
  if (old_len < len) {
    double* old_values = Get_dbl_values(vl);
    double* new_values = (double*)malloc(len * sizeof(double));
    if (old_values) {
      memcpy(new_values, old_values, old_len * sizeof(double));
      free(old_values);
    }
    for (size_t i = old_len; i < len; i++) {
      *(new_values + i) = value;
    }
    vl->_vals._d = new_values;
  }
  vl->_length = len;
}

/**
 * @brief resize value list of PTR
 *
 * @param vl value list
 * @param len new length of value list
 * @param value new value
 */
static inline void Resize_ptr_value_list(VALUE_LIST* vl, size_t len,
                                         PTR value) {
  IS_TRUE(vl, "null value list");
  IS_TRUE(vl->_type == PTR_TYPE, "not double type");
  size_t old_len = LIST_LEN(vl);
  if (old_len < len) {
    PTR* old_values = Get_ptr_values(vl);
    PTR* new_values = (PTR*)malloc(len * sizeof(PTR));
    if (old_values) {
      memcpy(new_values, old_values, old_len * sizeof(PTR));
    }
    for (size_t i = old_len; i < len; i++) {
      *(new_values + old_len + i) = value;
    }
    vl->_vals._p = new_values;
    free(old_values);
  }
  vl->_length = len;
}

/**
 * @brief if value list is empty
 *
 * @param list value list
 * @return true
 * @return false
 */
static inline bool Is_empty(VALUE_LIST* list) {
  return !(list && LIST_LEN(list) > 0 && (ANY_VALUES(list) != NULL));
}

/**
 * @brief initialize value list for i32
 *
 * @param list value list
 * @param len length of value list
 * @param init_vals init value of value list
 */
void Init_i32_value_list(VALUE_LIST* list, size_t len, int32_t* init_vals);

/**
 * @brief initialize value list for i64
 *
 * @param list value list
 * @param len length of value list
 * @param init_vals init value of value list
 */
void Init_i64_value_list(VALUE_LIST* list, size_t len, int64_t* init_vals);

/**
 * @brief initialize value list for double
 *
 * @param list value list
 * @param len length of value list
 * @param init_vals init value of value list
 */
void Init_dbl_value_list(VALUE_LIST* list, size_t len, double* init_vals);

/**
 * @brief initialize value list for DCMPLX
 *
 * @param list value list
 * @param len length of value list
 * @param init_vals init value of value list
 */
void Init_dcmplx_value_list(VALUE_LIST* list, size_t len, DCMPLX* init_vals);

/**
 * @brief initialize value list for bigint
 *
 * @param list value list
 * @param len length of value list
 * @param init_vals init value of value list
 */
void Init_bigint_value_list(VALUE_LIST* list, size_t len, BIG_INT* init_vals);

/**
 * @brief initialize value list for bigint with int64
 *
 * @param list value list
 * @param len length of value list
 * @param init_vals init value of value list
 */
void Init_bigint_value_list_with_si(VALUE_LIST* res, size_t len,
                                    int64_t* init_vals);

/**
 * @brief initialize value list for i64 but not copy values
 *
 * @param list value list
 * @param len length of value list
 * @param init_vals init value of value list
 */
static inline void Init_i64_value_list_no_copy(VALUE_LIST* list, size_t len,
                                               int64_t* init_vals) {
  list->_type    = I64_TYPE;
  list->_length  = len;
  list->_vals._i = init_vals;
}

/**
 * @brief initialize value list for double but not copy values
 *
 * @param list value list
 * @param len length of value list
 * @param init_vals init value of value list
 */
static inline void Init_dbl_value_list_no_copy(VALUE_LIST* list, size_t len,
                                               double* init_vals) {
  list->_type    = DBL_TYPE;
  list->_length  = len;
  list->_vals._d = init_vals;
}

/**
 * @brief copy value list from src to tgt
 *
 * @param tgt target value list
 * @param src source value list
 */
void Copy_value_list(VALUE_LIST* tgt, VALUE_LIST* src);

//! @brief Extract value list cnt number of values from source from start index
//! @param tgt target value list
//! @param src source value list
//! @param start extract start index
//! @param cnt number of extracted values
void Extract_value_list(VALUE_LIST* tgt, VALUE_LIST* src, size_t start,
                        size_t cnt);

/**
 * @brief alloc value list from other
 *
 * @param alloc_len memory size
 * @param other other value list
 * @return VALUE_LIST*
 */
VALUE_LIST* Alloc_copy_value_list(size_t alloc_len, VALUE_LIST* other);

/**
 * @brief free elements of value list
 *
 * @param list value list
 */
void Free_value_list_elems(VALUE_LIST* list);

/**
 * @brief free value list
 *
 * @param list value list
 */
void Free_value_list(VALUE_LIST* list);

/**
 * @brief print function of value list
 *
 * @param fp
 * @param list
 */
void Print_value_list(FILE* fp, VALUE_LIST* list);

/**
 * @brief link_list_node
 *
 */
typedef struct LL_NODE_LIST {
  int32_t              _val;
  struct LL_NODE_LIST* _prev;
  struct LL_NODE_LIST* _next;
} LL_NODE;

/**
 * @brief link_list
 *
 */
typedef struct {
  LL_NODE* _head;
  size_t   _cnt;
} LL;

#define FOR_ALL_LL_ELEM(ll, node) \
  for (LL_NODE* node = ll->_head; node != NULL; node = node->_next)

/**
 * @brief alloc link list node
 *
 * @param val
 * @return LL_NODE*
 */
static inline LL_NODE* Alloc_ll_node(int32_t val) {
  LL_NODE* node = (LL_NODE*)malloc(sizeof(LL_NODE));
  node->_val    = val;
  node->_prev   = NULL;
  node->_next   = NULL;
  return node;
}

/**
 * @brief free link list node
 *
 * @param node
 */
static inline void Free_ll_node(LL_NODE* node) { free(node); }

/**
 * @brief alloc link list
 *
 * @return LL*
 */
static inline LL* Alloc_link_list(void) {
  LL* ll    = (LL*)malloc(sizeof(LL));
  ll->_head = NULL;
  ll->_cnt  = 0;
  return ll;
}

/**
 * @brief free link list
 *
 * @param ll link list
 */
static inline void Free_link_list(LL* ll) {
  LL_NODE* cur_node = ll->_head;
  while (cur_node) {
    LL_NODE* next = cur_node->_next;
    Free_ll_node(cur_node);
    cur_node = next;
  }
  free(ll);
}

/**
 * @brief print linke list
 *
 * @param fp
 * @param ll
 */
void Print_link_list(FILE* fp, LL* ll);

/**
 * @brief get count of link list
 *
 * @param ll
 * @return size_t
 */
static inline size_t Get_link_list_cnt(LL* ll) {
  IS_TRUE(ll, "null link list");
  return ll->_cnt;
}

/**
 * @brief increase count of link list
 *
 * @param ll
 */
static inline void Inc_link_list_cnt(LL* ll) {
  IS_TRUE(ll, "null link list");
  ll->_cnt++;
}

/**
 * @brief decrease count of link list
 *
 * @param ll
 */
static inline void Dec_link_list_cnt(LL* ll) {
  IS_TRUE(ll, "null link list");
  ll->_cnt--;
}

/**
 * @brief insert value into link list with unique
 *
 * @param ll
 * @param val
 */
static inline void Insert_sort_uniq(LL* ll, int32_t val) {
  LL_NODE* head = ll->_head;
  if (head == NULL) {
    LL_NODE* new_node = Alloc_ll_node(val);
    ll->_head         = new_node;
    Inc_link_list_cnt(ll);
    return;
  }
  FOR_ALL_LL_ELEM(ll, node) {
    if (val == node->_val) {
      break;
    } else if (val < node->_val) {
      LL_NODE* new_node = Alloc_ll_node(val);
      Inc_link_list_cnt(ll);
      if (node == head) {
        new_node->_next = node;
        node->_prev     = new_node;
        ll->_head       = new_node;
      } else {
        node->_prev->_next = new_node;
        new_node->_prev    = node->_prev;
        node->_prev        = new_node;
        new_node->_next    = node;
      }
      break;
    } else {
      if (node->_next == NULL) {
        // insert to the end
        LL_NODE* new_node = Alloc_ll_node(val);
        Inc_link_list_cnt(ll);
        node->_next     = new_node;
        new_node->_prev = node;
        break;
      }
    }
  }
}

/**
 * @brief delete link list node from link list
 *
 * @param ll
 * @param val
 */
static inline void Delete_node(LL* ll, int32_t val) {
  LL_NODE* head = ll->_head;
  FOR_ALL_LL_ELEM(ll, node) {
    if (node->_val == val) {
      if (node == head) {
        // remove item is head
        LL_NODE* new_head = node->_next;
        ll->_head         = new_head;
        Free_ll_node(node);
        Dec_link_list_cnt(ll);
      } else {
        node->_prev->_next = node->_next;
        if (node->_next) {
          node->_next->_prev = node->_prev;
        }
        Free_ll_node(node);
        Dec_link_list_cnt(ll);
      }
      break;
    }
  }
}

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_INCLUDE_FHE_TYPES_H
