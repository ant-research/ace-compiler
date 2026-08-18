#include "dsl.hpp"
#include "air/core/opcode.h"
#include <ostream>

// #include "air/base/container.h"

namespace air::dsl {

DSL::DSL() {
  _glob = new GLOB_SCOPE(0, true);
  META_INFO::Remove_all();
  air::core::Register_core();
}

// define for operator
int DSL::add(int a, int b) {
  // TODO: Call air OP
  return a + b;
}

// for debug
int DSL::sub(int a, int b) {
  // TODO: Call air OP
  return a - b;
}
// for debug
int DSL::mul(int a, int b) {
  // TODO: Call air OP
  return a * b;
}

// or define for operatores
void DSL::BinOp(int op) {
  // New_bin_arith, ...
}

// need redefine API
void DSL::Assign() {
  // N
}

// need redefine API
void DSL::Constant() {
  // N
}

GLOB_SCOPE *DSL::getGlobalScope() {
  if (_glob == nullptr) {
    _glob = new GLOB_SCOPE(0, true);
  }
  return _glob;
}

TYPE_PTR DSL::getPrimType(PRIMITIVE_TYPE type) {
  // N
  return _glob->Prim_type(type);
}

TYPE_PTR DSL::getArrayType(std::string name, TYPE_PTR etype,
                           const std::vector<int> &dims, const SPOS &spos) {
  ARB_PTR arb = createDims(dims);
  ARRAY_TYPE_PTR ret = _glob->New_arr_type(name.c_str(), etype, arb, spos);
  return ret;
}

ARB_PTR DSL::createDims(const std::vector<int> &dims) {
  ARB_PTR arb_tail, arb_head;
  for (int i = 0; i < dims.size(); ++i) {
    ARB_PTR arb = _glob->New_arb(i + 1, 0, dims[i], 1);
    if (i != 0) {
      arb_tail->Set_next(arb->Id());
    } else {
      arb_head = arb;
    }
    arb_tail = arb;
  }
  return arb_head;
}

STR_PTR DSL::newStr(std::string name) {
  STR_PTR ret = _glob->New_str(name.c_str());
  return ret;
}

void DSL::addParm(std::string name, TYPE_PTR ptype, SIGNATURE_TYPE_PTR sig_type,
                  const SPOS &spos) {
  _glob->New_param(name.c_str(), ptype, sig_type, spos);
}

void DSL::addRet(TYPE_PTR ret_ty, SIGNATURE_TYPE_PTR ptype, const SPOS &spos) {
  _glob->New_ret_param(ret_ty, ptype);
}

SIGNATURE_TYPE_PTR DSL::newSigType() {
  SIGNATURE_TYPE_PTR sig = _glob->New_sig_type();
  return sig;
}

void DSL::setSigComplete(SIGNATURE_TYPE_PTR sig_type) {
  sig_type->Set_complete();
}

FUNC_PTR DSL::newFunc(std::string name, const SPOS &spos, bool with_scope) {
  FUNC_PTR ret = _glob->New_func(newStr(name), spos);
  ret->Set_parent(_glob->Comp_env_id());
  if (with_scope) {
    _fs = newFuncScope(ret);
  }
  return ret;
}

FUNC_SCOPE *DSL::newFuncScope(FUNC_PTR f) {
  FUNC_SCOPE *ret = &_glob->New_func_scope(f);
  return ret;
}

ENTRY_PTR DSL::newEntryPoint(SIGNATURE_TYPE_PTR sig, FUNC_PTR f,
                             const SPOS &spos) {
  ENTRY_PTR ret = _glob->New_entry_point(sig, f, f->Name(), spos);
  AIR_ASSERT(sig->Is_complete());
  _fs->Container().New_func_entry(spos);
  return ret;
}

ADDR_DATUM_PTR DSL::newVar(std::string name, TYPE_PTR ty, const SPOS &spos) {
  ADDR_DATUM_PTR ret = _fs->New_var(ty, name.c_str(), spos);
  return ret;
}

ADDR_DATUM_PTR DSL::Formal(int idx) {
  ADDR_DATUM_PTR ret = _fs->Formal(idx);
  return ret;
}

NODE_PTR DSL::newLd(ADDR_DATUM_PTR addr, const SPOS &spos) {
  NODE_PTR ret = _fs->Container().New_ld(addr, spos);
  return ret;
}

STMT_PTR DSL::newSt(NODE_PTR val, ADDR_DATUM_PTR addr, const SPOS &spos) {
  STMT_PTR ret = _fs->Container().New_st(val, addr, spos);
  _fs->Container().Stmt_list().Append(ret);
  return ret;
}

STMT_PTR DSL::newRETV(NODE_PTR val, const SPOS &spos) {
  STMT_PTR ret = _fs->Container().New_retv(val, spos);
  _fs->Container().Stmt_list().Append(ret);
  return ret;
}

FUNC_SCOPE *DSL::getCurFuncScope() { return _fs; }

} // namespace air::dsl