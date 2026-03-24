#include "py_airgen.h"

#include <ostream>

#include "air/core/opcode.h"
#include "fhe/ckks/ckks_opcode.h"
#include "nn/core/opcode.h"
#include "nn/vector/vector_opcode.h"
#include "nn/vector/vector_utils.h"

// #include "air/base/container.h"

namespace air::dsl {

PY_AIRGEN::PY_AIRGEN() {
  _glob = new GLOB_SCOPE(0, true);
  META_INFO::Remove_all();
  air::core::Register_core();
  nn::core::Register_nn();
  nn::vector::Register_vector_domain();
}

PY_AIRGEN::PY_AIRGEN(FUNC_SCOPE& fs, NODE_PTR blk_node)
    : _fs(&fs), _blk_node(blk_node) {
  _glob = nullptr;
}

FUNC_PTR PY_AIRGEN::Owning_func(FUNC_SCOPE& fs) { return fs.Owning_func(); }

NODE_PTR PY_AIRGEN::Get_blk_ctx() { return _blk_node; }

void PY_AIRGEN::Set_blk_ctx(NODE_PTR n) { _blk_node = n; }

void PY_AIRGEN::Set_cur_func_scope(FUNC_SCOPE& fs) { _fs = &fs; }

GLOB_SCOPE* PY_AIRGEN::Glob_scope() {
  if (_glob != nullptr) {
    return _glob;
  } else if (_fs == nullptr) {
    AIR_ASSERT_MSG(false, "No function scope");
  }
  return &_fs->Glob_scope();
}

void PY_AIRGEN::Append(STMT_PTR node) {
  STMT_LIST sl(_blk_node);
  sl.Append(node);
}

TYPE_PTR PY_AIRGEN::Get_prim_type(PRIMITIVE_TYPE type) {
  // N
  return Glob_scope()->Prim_type(type);
}

TYPE_PTR PY_AIRGEN::Get_array_type(std::string name, TYPE_PTR etype,
                                   const std::vector<int>& dims,
                                   const SPOS&             spos) {
  ARB_PTR        arb = Create_dims(dims);
  ARRAY_TYPE_PTR ret =
      Glob_scope()->New_arr_type(name.c_str(), etype, arb, spos);
  return ret;
}

ARB_PTR PY_AIRGEN::Create_dims(const std::vector<int>& dims) {
  ARB_PTR arb_tail, arb_head;
  for (int i = 0; i < dims.size(); ++i) {
    ARB_PTR arb = Glob_scope()->New_arb(i + 1, 0, dims[i], 1);
    if (i != 0) {
      arb_tail->Set_next(arb->Id());
    } else {
      arb_head = arb;
    }
    arb_tail = arb;
  }
  return arb_head;
}

STR_PTR PY_AIRGEN::New_str(std::string name) {
  STR_PTR ret = Glob_scope()->New_str(name.c_str());
  return ret;
}

void PY_AIRGEN::Add_parm(std::string name, TYPE_PTR ptype,
                         SIGNATURE_TYPE_PTR sig_type, const SPOS& spos) {
  Glob_scope()->New_param(name.c_str(), ptype, sig_type, spos);
}

void PY_AIRGEN::Add_ret(TYPE_PTR ret_ty, SIGNATURE_TYPE_PTR ptype,
                        const SPOS& spos) {
  Glob_scope()->New_ret_param(ret_ty, ptype);
}

SIGNATURE_TYPE_PTR PY_AIRGEN::New_sig_point() {
  SIGNATURE_TYPE_PTR sig = Glob_scope()->New_sig_type();
  return sig;
}

void PY_AIRGEN::Set_sig_complete(SIGNATURE_TYPE_PTR sig_type) {
  sig_type->Set_complete();
}

FUNC_PTR PY_AIRGEN::New_func(std::string name, const SPOS& spos,
                             bool with_scope) {
  FUNC_PTR ret = Glob_scope()->New_func(New_str(name), spos);
  ret->Set_parent(Glob_scope()->Comp_env_id());
  if (with_scope) {
    _fs = New_func_scope(ret);
  }
  return ret;
}

FUNC_SCOPE* PY_AIRGEN::New_func_scope(FUNC_PTR f) {
  FUNC_SCOPE* ret = &Glob_scope()->New_func_scope(f);
  return ret;
}

ENTRY_PTR PY_AIRGEN::New_entry_point(SIGNATURE_TYPE_PTR sig, FUNC_PTR f,
                                     const SPOS& spos) {
  ENTRY_PTR ret = Glob_scope()->New_entry_point(sig, f, f->Name(), spos);
  AIR_ASSERT(sig->Is_complete());
  _fs->Container().New_func_entry(spos);
  return ret;
}

ADDR_DATUM_PTR PY_AIRGEN::New_var(std::string name, TYPE_PTR ty,
                                  const SPOS& spos) {
  ADDR_DATUM_PTR ret = _fs->New_var(ty, name.c_str(), spos);
  return ret;
}

ADDR_DATUM_PTR PY_AIRGEN::Formal(int idx) {
  ADDR_DATUM_PTR ret = _fs->Formal(idx);
  return ret;
}

ADDR_DATUM_PTR PY_AIRGEN::Ld_var(NODE_PTR node) {
  AIR_ASSERT_MSG(node->Is_ld(), "node is not load");
  return node->Addr_datum();
}

NODE_PTR PY_AIRGEN::New_call(FUNC_SCOPE* fs, std::vector<NODE_PTR> args,
                             const SPOS& spos) {
  SIGNATURE_TYPE_PTR func_sig =
      fs->Owning_func()->Entry_point()->Type()->Cast_to_sig();
  TYPE_PTR rtype = func_sig->Ret_param()->Type();
  AIR_ASSERT_MSG(rtype != Null_ptr, "not support non-return call");
  PREG_PTR retv   = _fs->New_preg(rtype);
  STMT_PTR s_call = _fs->Container().New_call(fs->Owning_func()->Entry_point(),
                                              retv, args.size(), spos);
  for (size_t i = 0; i < args.size(); i++) {
    _fs->Container().New_arg(s_call, i, args[i]);
  }
  Append(s_call);
  return _fs->Container().New_ldp(retv, spos);
}

NODE_PTR PY_AIRGEN::New_zero(TYPE_PTR ty, const SPOS& spos) {
  NODE_PTR ret = _fs->Container().New_zero(ty, spos);
  return ret;
}

NODE_PTR PY_AIRGEN::New_intconst(PRIMITIVE_TYPE type, uint64_t val,
                                 const SPOS& spos) {
  NODE_PTR ret = _fs->Container().New_intconst(type, val, spos);
  return ret;
}

NODE_PTR PY_AIRGEN::New_ld(ADDR_DATUM_PTR addr, const SPOS& spos) {
  NODE_PTR ret = _fs->Container().New_ld(addr, spos);
  return ret;
}

NODE_PTR PY_AIRGEN::New_ldc(CONSTANT_PTR cst, const SPOS& spos) {
  NODE_PTR ret = _fs->Container().New_ldc(cst, spos);
  return ret;
}

NODE_PTR PY_AIRGEN::New_lt(NODE_PTR lhs, NODE_PTR rhs, TYPE_PTR ty,
                           const SPOS& spos) {
  NODE_PTR ret = _fs->Container().New_bin_arith(
      OPCODE(air::core::CORE, air::core::OPCODE::LT), ty, lhs, rhs, spos);
  return ret;
}

NODE_PTR PY_AIRGEN::New_stmt_block(const SPOS& spos) {
  NODE_PTR ret = _fs->Container().New_stmt_block(spos);
  return ret;
}

STMT_PTR PY_AIRGEN::New_st(NODE_PTR val, ADDR_DATUM_PTR addr,
                           const SPOS& spos) {
  STMT_PTR ret = _fs->Container().New_st(val, addr, spos);
  return ret;
}

STMT_PTR PY_AIRGEN::New_retv(NODE_PTR val, const SPOS& spos) {
  STMT_PTR ret = _fs->Container().New_retv(val, spos);
  return ret;
}

STMT_PTR PY_AIRGEN::New_do_loop(ADDR_DATUM_PTR iv, NODE_PTR init, NODE_PTR comp,
                                NODE_PTR incr, NODE_PTR body,
                                const SPOS& spos) {
  STMT_PTR ret = _fs->Container().New_do_loop(iv, init, comp, incr, body, spos);
  return ret;
}

CONSTANT_PTR PY_AIRGEN::New_float_array_const(
    const std::string& name, int asize, CONST_TYPE_PTR elem_type,
    std::vector<int64_t> shape, py::array_t<float> buf, const SPOS& spos) {
  py::buffer_info buf_info = buf.request();
  CONSTANT_PTR    ret      = nn::vector::New_array_const(
      Glob_scope(), name, asize, elem_type, shape, buf_info.ptr, spos);
  return ret;
}

CONSTANT_PTR PY_AIRGEN::New_int_array_const(const std::string& name, int asize,
                                            TYPE_PTR             elem_type,
                                            std::vector<int64_t> shape,
                                            py::array_t<int>     buf,
                                            const SPOS&          spos) {
  py::buffer_info buf_info = buf.request();
  CONSTANT_PTR    ret      = nn::vector::New_array_const(
      Glob_scope(), name, asize, elem_type, shape, buf_info.ptr, spos);
  return ret;
}

void PY_AIRGEN::Block_append(NODE_PTR block, STMT_PTR stmt) {
  STMT_LIST(block).Append(stmt);
}

FUNC_SCOPE* PY_AIRGEN::Get_cur_func_scope() { return _fs; }

TYPE_PTR PY_AIRGEN::Get_rtype(NODE_PTR val) { return val->Rtype(); }

NODE_PTR PY_AIRGEN::Clone_exp(NODE_PTR val) {
  return _fs->Container().Clone_node_tree(val);
}

NODE_PTR PY_AIRGEN::New_add(NODE_PTR a, NODE_PTR b, const SPOS& spos) {
  NODE_PTR node = _fs->Container().New_bin_arith(
      air::base::OPCODE(air::core::CORE, air::core::OPCODE::ADD), a->Rtype(), a,
      b, spos);
  return node;
}

NODE_PTR PY_AIRGEN::New_mul(NODE_PTR a, NODE_PTR b, const SPOS& spos) {
  NODE_PTR node = _fs->Container().New_bin_arith(
      air::base::OPCODE(air::core::CORE, air::core::OPCODE::MUL), a->Rtype(), a,
      b, spos);
  return node;
}

NODE_PTR PY_AIRGEN::New_array(CONSTANT_PTR ra_const, NODE_PTR& offset,
                              const SPOS& spos) {
  NODE_PTR ra_array = _fs->Container().New_array(
      _fs->Container().New_ldca(ra_const, POINTER_KIND::FLAT32, spos), 1, spos);
  _fs->Container().Set_array_idx(ra_array, 0, offset);
  return ra_array;
}

NODE_PTR PY_AIRGEN::New_array(ADDR_DATUM_PTR addr_datum, NODE_PTR& offset,
                              const SPOS& spos) {
  NODE_PTR ra_array = _fs->Container().New_array(
      _fs->Container().New_lda(addr_datum, POINTER_KIND::FLAT32, spos), 1,
      spos);
  _fs->Container().Set_array_idx(ra_array, 0, offset);
  return ra_array;
}

STMT_PTR PY_AIRGEN::New_ist(NODE_PTR& ra_array, NODE_PTR& val,
                            const SPOS& spos) {
  return _fs->Container().New_ist(ra_array, val, spos);
}

NODE_PTR PY_AIRGEN::New_ild(NODE_PTR& ra_array, const SPOS& spos) {
  return _fs->Container().New_ild(ra_array, spos);
}

void PY_AIRGEN::Print(NODE_PTR a) { a->Print(); }

void PY_AIRGEN::Print_glob() { Glob_scope()->Print(); }

void PY_AIRGEN::Append_block(NODE_PTR body) {
  STMT_LIST sl = _fs->Container().Stmt_list();
  for (STMT_PTR stmt = body->Begin_stmt(); stmt != body->End_stmt();) {
    STMT_PTR next_stmt = stmt->Next();
    sl.Append(stmt);
    stmt = next_stmt;
  }
}
}  // namespace air::dsl
