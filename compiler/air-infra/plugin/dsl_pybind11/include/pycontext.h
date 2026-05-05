//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_PYCONTEXT_H
#define AIR_PYCONTEXT_H

#include "air/base/node.h"
#include "air/base/st.h"

using namespace air::base;

namespace air::dsl {
class PYCONTEXT {
public:
  typedef std::tuple<int64_t, int64_t, int64_t, int64_t> NCHW;
  PYCONTEXT(NODE_PTR node);

  // -------- Type relevant APIs --------
  std::vector<int64_t> Rtype(NODE_PTR node);
  TYPE_PTR             Get_elem_type(NODE_PTR node);
  CONST_TYPE_PTR       Get_elem_const_type(NODE_PTR node);

  NCHW Get_array_nchw(TYPE_PTR type);

  // -------- Constant relevant APIs --------

  std::vector<float> Get_float_const_array(NODE_PTR node);

  // -------- Attribute relevant APIs --------
  std::vector<int> Strides(NODE_PTR node);
  std::vector<int> Pads(NODE_PTR node);
  std::vector<int> Group(NODE_PTR node);

  NODE_PTR Get_cur_node();

private:
  NODE_PTR _cur_node;
};
}  // namespace air::dsl

#endif  // AIR_PYCONTEXT_H
