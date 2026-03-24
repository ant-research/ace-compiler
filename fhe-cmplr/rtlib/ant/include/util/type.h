//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_ANT_INCLUDE_UTIL_TYPE_H
#define RTLIB_ANT_INCLUDE_UTIL_TYPE_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "common/error.h"
#include "util/bignumber.h"

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

//! @brief Type of VALUE_LIST
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

//! @brief VALUE_LIST to store a list of int64/double/DCMPLX/BIG_INT values
typedef struct VALUE_LIST {
  VALUE_TYPE _type;
  size_t     _length;
  union {
    ANY_VAL*            _a;     //!< any values
    int32_t*            _i32;   //!< int32 values
    uint32_t*           _ui32;  //!< uint32 values
    int64_t*            _i;     //!< int64 values
    uint64_t*           _u;     //!< uint64 values
    INT128_T*           _i128;  //!< int128 values
    PTR*                _p;     //!< ptr values
    struct VALUE_LIST** _v;     //!< VALUE_LIST ptr values
    double*             _d;     //!< double values
    DCMPLX*             _c;     //!< double complex values
    BIG_INT*            _b;     //!< big integer values
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
#define I128_VALUES(list)          (list)->_vals._i128
#define DBL_VALUES(list)           (list)->_vals._d
#define DCMPLX_VALUES(list)        (list)->_vals._c
#define BIGINT_VALUES(list)        (list)->_vals._b
#define PTR_VALUES(list)           (list)->_vals._p
#define LIST_LEN(list)             (list)->_length
#define LIST_TYPE(list)            (list)->_type

typedef VALUE_LIST VL_DBL;        //!< VALUE_LIST<double>
typedef VALUE_LIST VL_UI32;       //!< VALUE_LIST<uint32_t>
typedef VALUE_LIST VL_I32;        //!< VALUE_LIST<int32_t>
typedef VALUE_LIST VL_VL_I32;     //!< VALUE_LIST<VALUE_LIST<int32_t>>
typedef VALUE_LIST VL_PTR;        //!< VALUE_LIST<PTR>
typedef VALUE_LIST VL_VL_PTR;     //!< VALUE_LIST<VALUE_LIST<PTR>>>
typedef VALUE_LIST VL_DCMPLX;     //!< VALUE_LIST<DCMPLX>
typedef VALUE_LIST VL_VL_DCMPLX;  //!< VALUE_LIST<VALUE_LIST<DCMPLX>>
typedef VALUE_LIST
    VL_VL_VL_DCMPLX;  //!< VALUE_LIST<VALUE_LIST<VALUE_LIST<DCMPLX>>>

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

//! @brief Get i32 values from given value list
//! @param list value list
//! @return int32_t*
static inline int32_t* Get_i32_values(VALUE_LIST* list) {
  IS_TRUE(list && LIST_TYPE(list) == I32_TYPE, "list is not I32_TYPE");
  return I32_VALUES(list);
}

//! @brief Get i64 values from given value list
//! @param list value list
//! @return int64_t*
static inline int64_t* Get_i64_values(VALUE_LIST* list) {
  IS_TRUE(list && LIST_TYPE(list) == I64_TYPE, "list is not I64_TYPE");
  return I64_VALUES(list);
}

//! @brief Get i128 values from given value list
//! @param list value list
//! @return INT128_T*
static inline INT128_T* Get_i128_values(VALUE_LIST* list) {
  IS_TRUE(list && LIST_TYPE(list) == I128_TYPE, "list is not I128_TYPE");
  return I128_VALUES(list);
}

//! @brief Get ptr values from given value list
//! @param list value list
//! @return PTR*
static inline PTR* Get_ptr_values(VALUE_LIST* list) {
  IS_TRUE(list && LIST_TYPE(list) == PTR_TYPE, "list is not PTR_TYPE");
  return PTR_VALUES(list);
}

//! @brief Get double values from given value list
//! @param list value list
//! @return double*
static inline double* Get_dbl_values(VALUE_LIST* list) {
  IS_TRUE(list && LIST_TYPE(list) == DBL_TYPE, "list is not DBL_TYPE");
  return DBL_VALUES(list);
}

//! @brief Get complex values from given value list
//! @param list value list
//! @return DCMPLX*
static inline DCMPLX* Get_dcmplx_values(VALUE_LIST* list) {
  IS_TRUE(list && LIST_TYPE(list) == DCMPLX_TYPE, "list is not DCMPLX_TYPE");
  return DCMPLX_VALUES(list);
}

//! @brief Get bigint values from given value list
//! @param list value list
//! @return BIG_INT*
static inline BIG_INT* Get_bint_values(VALUE_LIST* list) {
  IS_TRUE(list && LIST_TYPE(list) == BIGINT_TYPE, "list is not BIGINT_TYPE");
  return BIGINT_VALUES(list);
}

//! @brief Get i32 value at idx from given value list
//! @param list value list
//! @return int32_t
static inline int32_t Get_i32_value_at(VALUE_LIST* list, size_t idx) {
  IS_TRUE(list && LIST_TYPE(list) == I32_TYPE, "list is not I32_TYPE");
  IS_TRUE(idx < LIST_LEN(list), "idx outof bound");
  return I32_VALUE_AT(list, idx);
}

//! @brief Get ui32 value at idx from given value list
//! @param list value list
//! @return uint32_t
static inline uint32_t Get_ui32_value_at(VALUE_LIST* list, size_t idx) {
  IS_TRUE(list && LIST_TYPE(list) == UI32_TYPE, "list is not I32_TYPE");
  IS_TRUE(idx < LIST_LEN(list), "idx outof bound");
  return UI32_VALUE_AT(list, idx);
}

//! @brief Get i64 value at idx from given value list
//! @param list value list
//! @return int64_t
static inline int64_t Get_i64_value_at(VALUE_LIST* list, size_t idx) {
  IS_TRUE(list && LIST_TYPE(list) == I64_TYPE, "list is not I64_TYPE");
  IS_TRUE(idx < LIST_LEN(list), "idx outof bound");
  return I64_VALUE_AT(list, idx);
}

//! @brief Get ptr value at idx from given value list
//! @param list value list
//! @return PTR
static inline PTR Get_ptr_value_at(VALUE_LIST* list, size_t idx) {
  IS_TRUE(list && LIST_TYPE(list) == PTR_TYPE, "list is not PTR_TYPE");
  IS_TRUE(idx < LIST_LEN(list), "idx outof bound");
  return PTR_VALUE_AT(list, idx);
}

//! @brief Get value list value at idx from given value list
//! @param list value list
//! @return VALUE_LIST*
static inline VALUE_LIST* Get_vl_value_at(VALUE_LIST* list, size_t idx) {
  IS_TRUE(list && LIST_TYPE(list) == VL_PTR_TYPE, "list is not VL_PTR_TYPE");
  IS_TRUE(idx < LIST_LEN(list), "idx outof bound");
  return VL_VALUE_AT(list, idx);
}

//! @brief Get value list value at [idx1, idx2] from given value list
//! @param list value list
//! @return VALUE_LIST*
static inline VALUE_LIST* Get_vl_l2_value_at(VALUE_LIST* list, size_t idx1,
                                             size_t idx2) {
  IS_TRUE(list && LIST_TYPE(list) == VL_PTR_TYPE, "list is not VL_PTR_TYPE");
  IS_TRUE(idx1 < LIST_LEN(list), "idx outof bound");
  return Get_vl_value_at(VL_VALUE_AT(list, idx1), idx2);
}

//! @brief Get value list value at [idx1][idx2][idx3] from a multi-dimensional
//! array with more than three dimensions
//! @param list value list
//! @return VALUE_LIST*
static inline VALUE_LIST* Get_vl_l3_value_at(VALUE_LIST* list, size_t idx1,
                                             size_t idx2, size_t idx3) {
  IS_TRUE(list && LIST_TYPE(list) == VL_PTR_TYPE, "list is not VL_PTR_TYPE");
  IS_TRUE(idx1 < LIST_LEN(list), "idx outof bound");
  return Get_vl_value_at(Get_vl_l2_value_at(list, idx1, idx2), idx3);
}

//! @brief Get size of given value list
//! @param list value list
//! @return size_t
static inline size_t Get_dim1_size(VALUE_LIST* list) {
  IS_TRUE(list && LIST_TYPE(list) == VL_PTR_TYPE, "null value VL_PTR_TYPE");
  return LIST_LEN(list);
}

//! @brief Get size of given value list list[0]
//! @param list value list
//! @return size_t
static inline size_t Get_dim2_size(VALUE_LIST* list) {
  IS_TRUE(list && LIST_TYPE(list) == VL_PTR_TYPE, "list is not VL_PTR_TYPE");
  return LIST_LEN(Get_vl_value_at(list, 0));
}

//! @brief Get size of given value list list[list[0]]
//! @param list value list
//! @return size_t
static inline size_t Get_dim3_size(VALUE_LIST* list) {
  IS_TRUE(list && LIST_TYPE(list) == VL_PTR_TYPE, "list is not VL_PTR_TYPE");
  return LIST_LEN(Get_vl_l2_value_at(list, 0, 0));
}

//! @brief Get double value at idx from given value list
//! @param list value list
//! @return double
static inline double Get_dbl_value_at(VALUE_LIST* list, size_t idx) {
  IS_TRUE(list && LIST_TYPE(list) == DBL_TYPE, "list is not DBL_TYPE");
  IS_TRUE(idx < LIST_LEN(list), "idx outof bound");
  return DBL_VALUE_AT(list, idx);
}

//! @brief Get complex value at idx from given value list
//! @param list value list
//! @return DCMPLX
static inline DCMPLX Get_dcmplx_value_at(VALUE_LIST* list, size_t idx) {
  IS_TRUE(list && LIST_TYPE(list) == DCMPLX_TYPE, "list is not DCMPLX_TYPE");
  IS_TRUE(idx < LIST_LEN(list), "idx outof bound");
  return DCMPLX_VALUE_AT(list, idx);
}

//! @brief Get bigint value at idx from given value list
//! NOTE: Big Integer is actully an array, cannot directly return it
//! return the address instead
//! @param list value list
//! @return BIG_INT*
static inline BIG_INT* Get_bint_value_at(VALUE_LIST* list, size_t idx) {
  IS_TRUE(list && LIST_TYPE(list) == BIGINT_TYPE, "list is not BIGINT_TYPE");
  IS_TRUE(idx < LIST_LEN(list), "idx outof bound");
  return &(BIGINT_VALUE_AT(list, idx));
}

//! @brief Set ui32 value to VALUE_LIST at idx
//! @param list value list
//! @param idx idx of value list
//! @param val ui32 value
static inline void Set_ui32_value(VALUE_LIST* list, size_t idx, uint32_t val) {
  IS_TRUE(list && LIST_TYPE(list) == UI32_TYPE, "list is not UI32_TYPE");
  IS_TRUE(idx < LIST_LEN(list), "idx outof bound");
  UI32_VALUE_AT(list, idx) = val;
}

//! @brief Set i128 value to VALUE_LIST at idx
//! @param list value list
//! @param idx idx of value list
//! @param val i128 value
static inline void Set_i128_value(VALUE_LIST* list, size_t idx, INT128_T val) {
  IS_TRUE(list && LIST_TYPE(list) == I128_TYPE, "list is not I128_TYPE");
  IS_TRUE(idx < LIST_LEN(list), "idx outof bound");
  I128_VALUE_AT(list, idx) = val;
}

//! @brief Set double value to VALUE_LIST at idx
//! @param list value list
//! @param idx idx of value list
//! @param val double value
static inline void Set_dbl_value(VALUE_LIST* list, size_t idx, double val) {
  IS_TRUE(list && LIST_TYPE(list) == DBL_TYPE, "list is not DBL_TYPE");
  IS_TRUE(idx < LIST_LEN(list), "idx outof bound");
  DBL_VALUE_AT(list, idx) = val;
}

//! @brief Set complex value to VALUE_LIST at idx
//! @param list value list
//! @param idx idx of value list
//! @param val complex value
static inline void Set_dcmplx_value(VALUE_LIST* list, size_t idx, DCMPLX val) {
  IS_TRUE(list && LIST_TYPE(list) == DCMPLX_TYPE, "list is not DCMPLX_TYPE");
  IS_TRUE(idx < LIST_LEN(list), "idx outof bound");
  DCMPLX_VALUE_AT(list, idx) = val;
}

//! @brief Set bigint value to VALUE_LIST at idx
//! @param list value list
//! @param idx idx of value list
//! @param val bigint value
static inline void Set_bint_value(VALUE_LIST* list, size_t idx, BIG_INT val) {
  IS_TRUE(list && LIST_TYPE(list) == BIGINT_TYPE, "list is not BIGINT_TYPE");
  IS_TRUE(idx < LIST_LEN(list), "idx outof bound");
  BI_ASSIGN(BIGINT_VALUE_AT(list, idx), val);
}

//! @brief Set ptr value to VALUE_LIST at idx
//! @param list value list
//! @param idx idx of value list
//! @param val ptr value
static inline void Set_ptr_value(VALUE_LIST* list, size_t idx, PTR val) {
  IS_TRUE(list && LIST_TYPE(list) == PTR_TYPE, "list is not BIGINT_TYPE");
  IS_TRUE(idx < LIST_LEN(list), "idx outof bound");
  PTR_VALUE_AT(list, idx) = val;
}

//! @brief Get memory size of given VALUE_TYPE
//! @param type type of VALUE_LIST
//! @return memory size
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

//! @brief Alloc value list from given type & length
//! @param type type of value list
//! @param len length of value list
//! @return VALUE_LIST*
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

//! @brief Alloc 2-D value list from given type & length
//! @param type type of value list
//! @param dim1 length of the first level
//! @param dim2 length of the second level
//! @return VALUE_LIST*
static inline VALUE_LIST* Alloc_2d_value_list(VALUE_TYPE type, size_t dim1,
                                              size_t dim2) {
  VALUE_LIST* vl = Alloc_value_list(VL_PTR_TYPE, dim1);
  FOR_ALL_ELEM(vl, idx) {
    VALUE_LIST* vl2      = Alloc_value_list(type, dim2);
    VL_VALUE_AT(vl, idx) = vl2;
  }
  return vl;
}

//! @brief Alloc 3-D value list from given type & length
//! @param type type of value list
//! @param dim1 length of the first level
//! @param dim2 length of the second level
//! @param dim3 length of the third level
//! @return VALUE_LIST*
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

//! @brief Resize value list of double
//! @param vl value list
//! @param len new length of value list
//! @param value new value
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

//! @brief Initialize value list for i32
//! @param list value list
//! @param len length of value list
//! @param init_vals init value of value list
void Init_i32_value_list(VALUE_LIST* list, size_t len, int32_t* init_vals);

//! @brief Initialize value list for i64
//! @param list value list
//! @param len length of value list
//! @param init_vals init value of value list
void Init_i64_value_list(VALUE_LIST* list, size_t len, int64_t* init_vals);

//! @brief Initialize value list for double
//! @param list value list
//! @param len length of value list
//! @param init_vals init value of value list
void Init_dbl_value_list(VALUE_LIST* list, size_t len, double* init_vals);

//! @brief Initialize value list for DCMPLX
//! @param list value list
//! @param len length of value list
//! @param init_vals init value of value list
void Init_dcmplx_value_list(VALUE_LIST* list, size_t len, DCMPLX* init_vals);

//! @brief Initialize value list for i64 but not copy values
//! @param list value list
//! @param len length of value list
//! @param init_vals init value of value list
static inline void Init_i64_value_list_no_copy(VALUE_LIST* list, size_t len,
                                               int64_t* init_vals) {
  list->_type    = I64_TYPE;
  list->_length  = len;
  list->_vals._i = init_vals;
}

//! @brief Initialize value list for double but not copy values
//! @param list value list
//! @param len length of value list
//! @param init_vals init value of value list
static inline void Init_dbl_value_list_no_copy(VALUE_LIST* list, size_t len,
                                               double* init_vals) {
  list->_type    = DBL_TYPE;
  list->_length  = len;
  list->_vals._d = init_vals;
}

//! @brief Copy value list from src to tgt
//! @param tgt target value list
//! @param src source value list
void Copy_value_list(VALUE_LIST* tgt, VALUE_LIST* src);

//! @brief Extract value list cnt number of values from source from start index
//! @param tgt target value list
//! @param src source value list
//! @param start extract start index
//! @param cnt number of extracted values
void Extract_value_list(VALUE_LIST* tgt, VALUE_LIST* src, size_t start,
                        size_t cnt);

//! @brief Alloc value list from other
//! @param alloc_len memory size
//! @param other other value list
//! @return VALUE_LIST*
VALUE_LIST* Alloc_copy_value_list(size_t alloc_len, VALUE_LIST* other);

//! @brief Free value list
void Free_value_list(VALUE_LIST* list);

//! @brief Print function of value list
void Print_value_list(FILE* fp, VALUE_LIST* list);

//! @brief link_list_node
typedef struct LL_NODE_LIST {
  int32_t              _val;
  struct LL_NODE_LIST* _prev;
  struct LL_NODE_LIST* _next;
} LL_NODE;

//! @brief Alloc link list node
static inline LL_NODE* Alloc_ll_node(int32_t val) {
  LL_NODE* node = (LL_NODE*)malloc(sizeof(LL_NODE));
  node->_val    = val;
  node->_prev   = NULL;
  node->_next   = NULL;
  return node;
}

//! @brief link_list
typedef struct {
  LL_NODE* _head;
  size_t   _cnt;
} LL;

#define FOR_ALL_LL_ELEM(ll, node) \
  for (LL_NODE* node = ll->_head; node != NULL; node = node->_next)

//! @brief Alloc link list
//! @return LL*
static inline LL* Alloc_link_list(void) {
  LL* ll    = (LL*)malloc(sizeof(LL));
  ll->_head = NULL;
  ll->_cnt  = 0;
  return ll;
}

//! @brief Free link list
//! @param ll link list
void Free_link_list(LL* ll);

//! @brief Get count of link list
//! @return size_t
static inline size_t Get_link_list_cnt(LL* ll) {
  IS_TRUE(ll, "null link list");
  return ll->_cnt;
}

//! @brief Increase count of link list
static inline void Inc_link_list_cnt(LL* ll) {
  IS_TRUE(ll, "null link list");
  ll->_cnt++;
}

//! @brief Insert value into link list with unique
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

//! @brief Delete link list node from link list
void Delete_node(LL* ll, int32_t val);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_ANT_INCLUDE_UTIL_TYPE_H
