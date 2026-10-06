//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "util/type.h"

#include "type_impl.h"
#include "util/bignumber.h"

void Free_value_list(VALUE_LIST* list) {
  if (list == NULL) return;
  assert(Is_valid(list));

  Free_value_list_elems(list);
  free(list);
}

void Copy_value_list(VALUE_LIST* tgt, VALUE_LIST* src) {
  if (tgt == src) return;
  IS_TRUE(tgt && src, "null value list");
  IS_TRUE(LIST_LEN(tgt) >= LIST_LEN(src), "not enough space");
  IS_TRUE(LIST_TYPE(tgt) == LIST_TYPE(src), "unmatched type");
  IS_TRUE(Is_prim_type(tgt), "only primitive type supported");
  Init_value_list(tgt, LIST_TYPE(tgt), LIST_LEN(src), ANY_VALUES(src));
}

VALUE_LIST* Alloc_copy_value_list(size_t Alloc_len, VALUE_LIST* other) {
  VALUE_LIST* vl = Alloc_value_list(LIST_TYPE(other), Alloc_len);
  Copy_value_list(vl, other);
  return vl;
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

void Extract_value_list(VALUE_LIST* tgt, VALUE_LIST* src, size_t start,
                        size_t cnt) {
  IS_TRUE(src && tgt && start < LIST_LEN(src) && start + cnt <= LIST_LEN(src),
          "invalid range");
  LIST_TYPE(tgt)  = LIST_TYPE(src);
  LIST_LEN(tgt)   = cnt;
  ANY_VALUES(tgt) = ANY_VALUES(src) + start * sizeof(PTR);
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

void Free_link_list(LL* ll) {
  LL_NODE* cur_node = ll->_head;
  while (cur_node) {
    LL_NODE* next = cur_node->_next;
    Free_ll_node(cur_node);
    cur_node = next;
  }
  free(ll);
}

void Delete_node(LL* ll, int32_t val) {
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