// example.hpp
#pragma once

#include "air/base/container.h"
#include "air/base/meta_info.h"
#include "air/base/opcode.h"
#include "air/base/st.h"
#include "air/util/debug.h"
#include <pybind11/stl.h>

using namespace air::util;
using namespace air::base;

namespace air::dsl {

class DSL {
public:
  DSL();

  // TODO : The operator API should be defined, according to operator or merged
  // one API
  int add(int a, int b);
  int sub(int a, int b);
  int mul(int a, int b);

  // operator: Add, Sub, ...
  void BinOp(int op);

  // Assign, Constant, Variable
  void Assign();
  void Constant();

  GLOB_SCOPE *getGlobalScope();

  SIGNATURE_TYPE_PTR newSigType();
  void addParm(std::string name, TYPE_PTR ptype, SIGNATURE_TYPE_PTR sig_type,
               const SPOS &spos);
  void addRet(TYPE_PTR ret_ty, SIGNATURE_TYPE_PTR ptype, const SPOS &spos);
  void setSigComplete(SIGNATURE_TYPE_PTR sig_type);

  STR_PTR newStr(std::string name);
  FUNC_PTR newFunc(std::string name, const SPOS &spos, bool with_scope = true);
  FUNC_SCOPE *newFuncScope(FUNC_PTR f);
  ENTRY_PTR newEntryPoint(SIGNATURE_TYPE_PTR sig, FUNC_PTR f, const SPOS &spos);
  FUNC_SCOPE *getCurFuncScope();
  ADDR_DATUM_PTR newVar(std::string name, TYPE_PTR ty, const SPOS &spos);
  ADDR_DATUM_PTR Formal(int idx);

  NODE_PTR newLd(ADDR_DATUM_PTR addr, const SPOS &spos);
  STMT_PTR newSt(NODE_PTR val, ADDR_DATUM_PTR addr, const SPOS &spos);
  STMT_PTR newRETV(NODE_PTR val, const SPOS &spos);

  // create types
  TYPE_PTR getPrimType(PRIMITIVE_TYPE type);
  TYPE_PTR getArrayType(std::string name, TYPE_PTR etype,
                        const std::vector<int> &arb, const SPOS &spos);

private:
  ARB_PTR createDims(const std::vector<int> &dims);

private:
  GLOB_SCOPE *_glob;
  FUNC_SCOPE *_fs;
};

} // namespace air::dsl