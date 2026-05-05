//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef PY_AIRGEN_H
#define PY_AIRGEN_H

#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "air/base/container.h"
#include "air/base/meta_info.h"
#include "air/base/opcode.h"
#include "air/base/st.h"
#include "air/base/transform_ctx.h"
#include "air/util/debug.h"

using namespace air::util;
using namespace air::base;
namespace py = pybind11;

namespace air::dsl {

class PY_AIRGEN {
public:
  PY_AIRGEN();
  explicit PY_AIRGEN(FUNC_SCOPE& fs, NODE_PTR blk_node);

  GLOB_SCOPE* Glob_scope();

  FUNC_PTR Owning_func(FUNC_SCOPE& fs);
  NODE_PTR Get_blk_ctx();
  void     Set_blk_ctx(NODE_PTR blk_node);

  void Append(STMT_PTR node);

  SIGNATURE_TYPE_PTR New_sig_point();
  void Add_parm(std::string name, TYPE_PTR ptype, SIGNATURE_TYPE_PTR sig_type,
                const SPOS& spos);
  void Add_ret(TYPE_PTR ret_ty, SIGNATURE_TYPE_PTR ptype, const SPOS& spos);
  void Set_sig_complete(SIGNATURE_TYPE_PTR sig_type);

  STR_PTR  New_str(std::string name);
  FUNC_PTR New_func(std::string name, const SPOS& spos, bool with_scope = true);
  FUNC_SCOPE*    New_func_scope(FUNC_PTR f);
  ENTRY_PTR      New_entry_point(SIGNATURE_TYPE_PTR sig, FUNC_PTR f,
                                 const SPOS& spos);
  FUNC_SCOPE*    Get_cur_func_scope();
  void           Set_cur_func_scope(FUNC_SCOPE& fs);
  ADDR_DATUM_PTR New_var(std::string name, TYPE_PTR ty, const SPOS& spos);
  ADDR_DATUM_PTR Formal(int idx);
  ADDR_DATUM_PTR Ld_var(NODE_PTR node);

  NODE_PTR New_call(FUNC_SCOPE* fs, std::vector<NODE_PTR> args,
                    const SPOS& spos);
  NODE_PTR New_zero(TYPE_PTR ty, const SPOS& spos);
  NODE_PTR New_intconst(PRIMITIVE_TYPE type, uint64_t val, const SPOS& spos);
  NODE_PTR New_ld(ADDR_DATUM_PTR addr, const SPOS& spos);
  NODE_PTR New_ldc(CONSTANT_PTR cst, const SPOS& spos);
  NODE_PTR New_lt(NODE_PTR lhs, NODE_PTR rhs, TYPE_PTR ty, const SPOS& spos);
  NODE_PTR New_stmt_block(const SPOS& spos);
  STMT_PTR New_st(NODE_PTR val, ADDR_DATUM_PTR addr, const SPOS& spos);
  STMT_PTR New_retv(NODE_PTR val, const SPOS& spos);
  STMT_PTR New_do_loop(ADDR_DATUM_PTR iv, NODE_PTR init, NODE_PTR comp,
                       NODE_PTR incr, NODE_PTR body, const SPOS& spos);
  CONSTANT_PTR New_float_array_const(const std::string& name, int asize,
                                     CONST_TYPE_PTR       elem_type,
                                     std::vector<int64_t> shape,
                                     py::array_t<float> buf, const SPOS& spos);
  CONSTANT_PTR New_int_array_const(const std::string& name, int asize,
                                   TYPE_PTR             elem_type,
                                   std::vector<int64_t> shape,
                                   py::array_t<int32_t> buf, const SPOS& spos);

  void Block_append(NODE_PTR block, STMT_PTR stmt);

  // create types
  TYPE_PTR Get_prim_type(PRIMITIVE_TYPE type);
  TYPE_PTR Get_array_type(std::string name, TYPE_PTR etype,
                          const std::vector<int>& arb, const SPOS& spos);

  TYPE_PTR Get_rtype(NODE_PTR val);
  NODE_PTR Clone_exp(NODE_PTR val);
  NODE_PTR New_add(NODE_PTR a, NODE_PTR b, const SPOS& spos);
  NODE_PTR New_mul(NODE_PTR a, NODE_PTR b, const SPOS& spos);
  void     Print(NODE_PTR a);
  void     Print_glob();
  NODE_PTR New_array(CONSTANT_PTR cst, NODE_PTR& offset, const SPOS& spos);
  NODE_PTR New_array(ADDR_DATUM_PTR addr_datum, NODE_PTR& offset,
                     const SPOS& spos);
  STMT_PTR New_ist(NODE_PTR& ra_array, NODE_PTR& val, const SPOS& spos);
  NODE_PTR New_ild(NODE_PTR& ra_array, const SPOS& spos);
  void     Append_block(NODE_PTR body);

private:
  ARB_PTR Create_dims(const std::vector<int>& dims);

private:
  GLOB_SCOPE* _glob;
  FUNC_SCOPE* _fs;
  NODE_PTR    _blk_node;
};

}  // namespace air::dsl

#endif  // PY_AIRGEN_H
