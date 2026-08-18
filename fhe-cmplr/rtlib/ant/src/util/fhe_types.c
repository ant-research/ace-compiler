//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "util/fhe_types.h"

#include <stdlib.h>

#include "common/trace.h"
#include "util/fhe_utils.h"

void Init_value_list(VALUE_LIST* list, VALUE_TYPE type, size_t len,
                     ANY_VAL* init_vals) {
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

void Init_i32_value_list(VALUE_LIST* list, size_t len, int32_t* init_vals) {
  IS_TRUE(list, "list is null");
  Init_value_list(list, I32_TYPE, len, (ANY_VAL*)init_vals);
}

void Init_i64_value_list(VALUE_LIST* list, size_t len, int64_t* init_vals) {
  IS_TRUE(list, "list is null");
  Init_value_list(list, I64_TYPE, len, (ANY_VAL*)init_vals);
}

void Init_dbl_value_list(VALUE_LIST* list, size_t len, double* init_vals) {
  IS_TRUE(list, "list is null");
  Init_value_list(list, DBL_TYPE, len, (ANY_VAL*)init_vals);
}

void Init_dcmplx_value_list(VALUE_LIST* list, size_t len, DCMPLX* init_vals) {
  IS_TRUE(list, "list is null");
  Init_value_list(list, DCMPLX_TYPE, len, (ANY_VAL*)init_vals);
}

void Init_bigint_value_list(VALUE_LIST* list, size_t len, BIG_INT* init_vals) {
  IS_TRUE(list, "list is null");

  size_t   mem_size = sizeof(BIG_INT) * len;
  BIG_INT* bi       = BIGINT_VALUES(list);

  if (bi == NULL) {
    BIGINT_VALUES(list) = (BIG_INT*)malloc(mem_size);
    bi                  = BIGINT_VALUES(list);
  } else {
    IS_TRUE(list->_length == len,
            "list already initialized with different length");
  }
  if (init_vals == NULL) {
    for (size_t idx = 0; idx < len; idx++) {
      BI_INIT(bi[idx]);
    }
  } else {
    for (size_t idx = 0; idx < len; idx++) {
      BI_INIT_ASSIGN(bi[idx], init_vals[idx]);
    }
  }
  list->_length = len;
  list->_type   = BIGINT_TYPE;
}

void Init_bigint_value_list_with_si(VALUE_LIST* res, size_t len,
                                    int64_t* init_vals) {
  IS_TRUE(res, "list is null");

  size_t   mem_size = sizeof(BIG_INT) * len;
  BIG_INT* bi       = BIGINT_VALUES(res);

  if (bi == NULL) {
    BIGINT_VALUES(res) = (BIG_INT*)malloc(mem_size);
    bi                 = BIGINT_VALUES(res);
  } else {
    IS_TRUE(res->_length == len,
            "list already initialized with different length");
  }
  if (init_vals == NULL) {
    for (size_t idx = 0; idx < len; idx++) {
      BI_INIT(bi[idx]);
    }
  } else {
    for (size_t idx = 0; idx < len; idx++) {
      BI_INIT_ASSIGN_SI(bi[idx], init_vals[idx]);
    }
  }
  res->_length = len;
  res->_type   = BIGINT_TYPE;
}

void Copy_value_list(VALUE_LIST* tgt, VALUE_LIST* src) {
  if (tgt == src) return;
  IS_TRUE(tgt && src, "null value list");
  IS_TRUE(LIST_LEN(tgt) >= LIST_LEN(src), "not enough space");
  IS_TRUE(LIST_TYPE(tgt) == LIST_TYPE(src), "unmatched type");
  IS_TRUE(Is_prim_type(tgt), "only primitive type supported");
  Init_value_list(tgt, LIST_TYPE(tgt), LIST_LEN(src), ANY_VALUES(src));
}

void Extract_value_list(VALUE_LIST* tgt, VALUE_LIST* src, size_t start,
                        size_t cnt) {
  IS_TRUE(src && tgt && start < LIST_LEN(src) && start + cnt <= LIST_LEN(src),
          "invalid range");
  LIST_TYPE(tgt)  = LIST_TYPE(src);
  LIST_LEN(tgt)   = cnt;
  ANY_VALUES(tgt) = ANY_VALUES(src) + start * sizeof(PTR);
}

VALUE_LIST* Alloc_copy_value_list(size_t Alloc_len, VALUE_LIST* other) {
  VALUE_LIST* vl = Alloc_value_list(LIST_TYPE(other), Alloc_len);
  Copy_value_list(vl, other);
  return vl;
}

void Free_value_list_elems(VALUE_LIST* list) {
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

void Free_value_list(VALUE_LIST* list) {
  if (list == NULL) return;
  assert(Is_valid(list));

  Free_value_list_elems(list);
  free(list);
}

const char* List_type_name(VALUE_LIST* list) {
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

void Print_value_list(FILE* fp, VALUE_LIST* list) {
  if (list == NULL) {
    fprintf(fp, "{}\n");
    return;
  }
  fprintf(fp, "%s VALUES @[%p][%ld] = {", List_type_name(list), list,
          LIST_LEN(list));
  switch (LIST_TYPE(list)) {
    case I32_TYPE:
      for (size_t idx = 0; idx < LIST_LEN(list); idx++) {
        fprintf(fp, " %d", I32_VALUE_AT(list, idx));
      }
      fprintf(fp, " }\n");
      break;
    case UI32_TYPE:
      for (size_t idx = 0; idx < LIST_LEN(list); idx++) {
        fprintf(fp, " %d", UI32_VALUE_AT(list, idx));
      }
      fprintf(fp, " }\n");
      break;
    case I64_TYPE:
      for (size_t idx = 0; idx < LIST_LEN(list); idx++) {
        fprintf(fp, " %ld", I64_VALUE_AT(list, idx));
      }
      fprintf(fp, " }\n");
      break;
    case UI64_TYPE:
      for (size_t idx = 0; idx < LIST_LEN(list); idx++) {
        fprintf(fp, " %ld", UI64_VALUE_AT(list, idx));
      }
      fprintf(fp, " }\n");
      break;
    case I128_TYPE:
      for (size_t idx = 0; idx < LIST_LEN(list); idx++) {
        fprintf(fp, " 0x%lx%lx",
                (uint64_t)((UINT128_T)I128_VALUE_AT(list, idx) >> 64),
                (uint64_t)(I128_VALUE_AT(list, idx)));
      }
      fprintf(fp, " }\n");
      break;
    case PTR_TYPE:
      for (size_t idx = 0; idx < LIST_LEN(list); idx++) {
        fprintf(fp, " %p", PTR_VALUE_AT(list, idx));
      }
      fprintf(fp, " }\n");
      break;
    case DBL_TYPE:
      for (size_t idx = 0; idx < LIST_LEN(list); idx++) {
        fprintf(fp, " %e", DBL_VALUE_AT(list, idx));
      }
      fprintf(fp, " }\n");
      break;
    case DCMPLX_TYPE:
      for (size_t idx = 0; idx < LIST_LEN(list); idx++) {
        DCMPLX val = DCMPLX_VALUE_AT(list, idx);
        fprintf(fp, " (%.17f, %.17f)", creal(val), cimag(val));
      }
      fprintf(fp, " }\n");
      break;
    case BIGINT_TYPE:
      for (size_t idx = 0; idx < LIST_LEN(list); idx++) {
        fprintf(fp, " ");
        Print_bi(fp, BIGINT_VALUE_AT(list, idx));
      }
      fprintf(fp, " }\n");
      break;
    case VL_PTR_TYPE:
      fprintf(fp, "\n");
      for (size_t idx = 0; idx < LIST_LEN(list); idx++) {
        fprintf(fp, "    ");
        Print_value_list(fp, VL_VALUE_AT(list, idx));
      }
      fprintf(fp, "    }\n");
      break;
    default:
      IS_TRUE(FALSE, "invalid type");
  }
}

void Print_link_list(FILE* fp, LL* ll) {
  if (ll == NULL) return;
  FOR_ALL_LL_ELEM(ll, node) { fprintf(fp, "%d ", node->_val); }
}
