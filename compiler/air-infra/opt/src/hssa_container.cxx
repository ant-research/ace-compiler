//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/opt/hssa_container.h"

#include "air/opt/bb.h"

using namespace air::base;

namespace air {

namespace opt {

// --- EXPR_HTABLE implementation ---

uint32_t EXPR_HTABLE::Bucket_idx(HEXPR_PTR expr) const {
  return expr->Hash_idx() % _size;
}

HEXPR_PTR EXPR_HTABLE::Find(HEXPR_PTR expr) const {
  uint32_t   idx         = Bucket_idx(expr);
  HEXPR_ID   bucket_head = _buckets[idx];
  HEXPR_LIST expr_list(_cont, bucket_head);
  return expr_list.Find(expr);
}

void EXPR_HTABLE::Insert(HEXPR_PTR expr) {
  uint32_t idx         = Bucket_idx(expr);
  HEXPR_ID bucket_head = _buckets[idx];
  if (bucket_head == HEXPR_ID()) {
    _buckets[idx] = expr->Id();
  } else {
    HEXPR_LIST expr_list(_cont, bucket_head);
    expr_list.Prepend(expr->Id());
    _buckets[idx] = expr->Id();
  }
}

HEXPR_PTR EXPR_HTABLE::Find_or_insert(HEXPR_PTR expr) {
  HEXPR_PTR ret = Find(expr);
  if (ret->Is_null()) {
    ret = _cont->New_expr(expr);
    Insert(ret);
  }
  return ret;
}

void EXPR_HTABLE::Print(std::ostream& os) const {
  os << "HTABLE: " << std::endl;
  for (uint32_t idx = 0; idx < _size; idx++) {
    HEXPR_ID head = _buckets[idx];
    if (head != HEXPR_ID()) {
      os << "[" << idx << "]";
      HEXPR_LIST expr_list(_cont, head);
      expr_list.Print_id(os);
      os << std::endl;
    }
  }
}

// --- HCONTAINER implementation ---

HCONTAINER::HCONTAINER(CONTAINER* cont, SSA_CONTAINER* ssa_cont,
                       uint32_t htable_size)
    : _cont(cont), _ssa_cont(ssa_cont) {
  _root_stmt = air::base::Null_id;

  HEXPR_ALLOC           hexpr_alloc(&_mpool);
  HSTMT_ALLOC           hstmt_alloc(&_mpool);
  HPHI_ALLOC            hphi_alloc(&_mpool);
  HMU_ALLOC             hmu_alloc(&_mpool);
  HCHI_ALLOC            hchi_alloc(&_mpool);
  U32_U32_MAP_ALLOCATOR u32map_alloc(&_mpool);

  _hexpr_tab = hexpr_alloc.Allocate(&_mpool, HEXPR_TAB_KIND, "hexpr_tab", true);
  _hstmt_tab = hstmt_alloc.Allocate(&_mpool, HSTMT_TAB_KIND, "hstmt_tab", true);
  _hphi_tab  = hphi_alloc.Allocate(&_mpool, HPHI_TAB_KIND, "hphi_tab", true);
  _hmu_tab   = hmu_alloc.Allocate(&_mpool, HMU_TAB_KIND, "hmu_tab", true);
  _hchi_tab  = hchi_alloc.Allocate(&_mpool, HCHI_TAB_KIND, "hchi_tab", true);
  _ver_expr_map = u32map_alloc.Allocate(13, std::hash<uint32_t>(),
                                        std::equal_to<uint32_t>(),
                                        U32_U32_PAIR_ALLOCATOR(&_mpool));
  HEXPR_ID* htable_ptr =
      (HEXPR_ID*)_mpool.Allocate(sizeof(HEXPR_ID) * htable_size);
  htable_ptr = new (htable_ptr) HEXPR_ID[htable_size]();
  _htable.Init(htable_ptr, htable_size, this);
}

HEXPR_PTR HCONTAINER::Find_or_new_var_expr(SSA_VER_ID id) {
  HEXPR_ID expr_id = Ver_expr(id);
  if (expr_id != Null_id) {
    return Expr_ptr(expr_id);
  } else {
    VAR_DATA_PTR var_data = _hexpr_tab->Allocate<VAR_DATA>();
    new (var_data) VAR_DATA(Ssa_cont()->Ver_sym(id));
    HEXPR_PTR expr = HEXPR_PTR(HEXPR(this, var_data));
    Set_ver_expr(id, expr);
    return expr;
  }
}

HEXPR_PTR HCONTAINER::New_var_expr(ADDR_DATUM_PTR datum, uint32_t sub_idx) {
  VAR_DATA_PTR var_data = _hexpr_tab->Allocate<VAR_DATA>();
  new (var_data) VAR_DATA(datum, sub_idx);
  return HEXPR_PTR(HEXPR(this, var_data));
}

HEXPR_PTR HCONTAINER::New_op_expr(OP_DATA_PTR op_expr) {
  OP_DATA_PTR op_ptr = air::base::Static_cast<OP_DATA_PTR>(
      _hexpr_tab->Malloc(OP_DATA::Size(op_expr->Kid_cnt())));
  new (op_ptr) OP_DATA(op_expr);
  return HEXPR_PTR(HEXPR(this, op_ptr));
}

HEXPR_PTR HCONTAINER::New_cst_expr(uint64_t cst_val) {
  TYPE_ID id = Air_cont()
                   ->Glob_scope()
                   ->Prim_type(air::base::PRIMITIVE_TYPE::INT_U64)
                   ->Id();
  CST_DATA       cst_data(cst_val, id);
  HEXPR_DATA_PTR cst_ptr(&cst_data, HEXPR_ID());
  HEXPR_PTR      ret = Find_or_new_expr(HEXPR_PTR(HEXPR(this, cst_ptr)));
  return ret;
}

HEXPR_PTR HCONTAINER::New_cst_expr(CST_DATA_PTR cst_expr) {
  CST_DATA_PTR cst_ptr = _hexpr_tab->Allocate<CST_DATA>();
  new (cst_ptr) CST_DATA(cst_expr);
  return HEXPR_PTR(HEXPR(this, cst_ptr));
}

HEXPR_PTR HCONTAINER::New_expr(HEXPR_PTR expr) {
  switch (expr->Kind()) {
    case EK_VAR:
      CMPLR_ASSERT(false, "New_expr for EK_VAR not yet implemented");
      break;
    case EK_OP: {
      OP_DATA_PTR op_data = expr->Cast_to_op_expr();
      return New_op_expr(op_data);
    }
    case EK_CONST: {
      CST_DATA_PTR cst_data = expr->Cast_to_cst_expr();
      return New_cst_expr(cst_data);
    }
    default:
      CMPLR_ASSERT(false, "New_expr: unsupported expression kind");
  }
  return Null_ptr;
}

HSTMT_PTR HCONTAINER::New_assign_stmt(air::base::NODE_PTR node) {
  ASSIGN_DATA_PTR stmt_ptr = _hstmt_tab->Allocate<ASSIGN_DATA>();
  new (stmt_ptr) ASSIGN_DATA(node);
  return HSTMT_PTR(HSTMT(this, stmt_ptr));
}

HSTMT_PTR HCONTAINER::New_assign_stmt(HEXPR_PTR var, HEXPR_PTR rhs) {
  ASSIGN_DATA_PTR stmt_ptr = _hstmt_tab->Allocate<ASSIGN_DATA>();
  new (stmt_ptr)
      ASSIGN_DATA(OPCODE(air::core::CORE, air::core::ST), var->Id(), rhs->Id());
  HSTMT_PTR ret = HSTMT_PTR(HSTMT(this, stmt_ptr));
  var->Cast_to_var_expr()->Set_def_stmt(ret->Id());
  return ret;
}

HSTMT_PTR HCONTAINER::New_op_stmt(air::base::NODE_PTR node) {
  NARY_DATA_PTR op_ptr = air::base::Static_cast<NARY_DATA_PTR>(
      _hstmt_tab->Malloc(NARY_DATA::Size(node->Num_child())));
  new (op_ptr) NARY_DATA(node);
  return HSTMT_PTR(HSTMT(this, op_ptr));
}

HSTMT_PTR HCONTAINER::New_entry_stmt(air::base::NODE_PTR node) {
  NARY_DATA_PTR op_ptr = air::base::Static_cast<NARY_DATA_PTR>(
      _hstmt_tab->Malloc(NARY_DATA::Size(node->Num_child() - 1)));
  new (op_ptr) NARY_DATA(node);
  return HSTMT_PTR(HSTMT(this, op_ptr));
}

HSTMT_PTR HCONTAINER::New_do_loop(air::base::NODE_PTR node, HSTMT_PTR init,
                                  HEXPR_PTR cond, HSTMT_PTR body,
                                  HEXPR_PTR incr, HPHI_PTR hphi) {
  DO_LOOP_DATA_PTR stmt_ptr = _hstmt_tab->Allocate<DO_LOOP_DATA>();
  new (stmt_ptr)
      DO_LOOP_DATA(node, init->Id(), cond->Id(), body->Id(), incr->Id());
  return HSTMT_PTR(HSTMT(this, stmt_ptr));
}

HSTMT_PTR HCONTAINER::New_call(air::base::NODE_PTR node) {
  CALL_DATA_PTR call_ptr = air::base::Static_cast<CALL_DATA_PTR>(
      _hstmt_tab->Malloc(CALL_DATA::Size(node->Num_arg())));
  new (call_ptr) CALL_DATA(node);
  return HSTMT_PTR(HSTMT(this, call_ptr));
}

HSTMT_PTR HCONTAINER::New_if(air::base::NODE_PTR node, HEXPR_PTR cond) {
  IF_DATA_PTR stmt_ptr = _hstmt_tab->Allocate<IF_DATA>();
  new (stmt_ptr) IF_DATA(OPCODE(air::core::CORE, air::core::IF), cond);
  return HSTMT_PTR(HSTMT(this, stmt_ptr));
}

HMU_PTR HCONTAINER::New_mu(HSTMT_PTR stmt) {
  HMU_DATA_PTR mu_data = _hmu_tab->Allocate<HMU_DATA>();
  mu_data->Init();
  return HMU_PTR(HMU(this, mu_data));
}

HCHI_PTR HCONTAINER::New_chi(HSTMT_PTR stmt) {
  HCHI_DATA_PTR chi_data = _hchi_tab->Allocate<HCHI_DATA>();
  new (chi_data) HCHI_DATA(stmt->Id());
  return HCHI_PTR(HCHI(this, chi_data));
}

HPHI_PTR HCONTAINER::New_phi(BB_PTR bb, uint32_t num_opnd) {
  AIR_ASSERT(bb->Kind() == BB_IF_PHI || bb->Kind() == BB_LOOP_PHI);
  uint32_t mem_size = (sizeof(HPHI_DATA) / _hphi_tab->Unit_size()) + num_opnd;
  HPHI_DATA_PTR phi_ptr =
      air::base::Static_cast<HPHI_DATA_PTR>(_hphi_tab->Malloc(mem_size));
  new (phi_ptr) HPHI_DATA(bb->Id(), num_opnd);
  return HPHI_PTR(HPHI(this, phi_ptr));
}

HEXPR_PTR HCONTAINER::New_var_with_ver(HEXPR_PTR var_expr, uint32_t ver) {
  VAR_DATA_PTR var_data = var_expr->Cast_to_var_expr();
  VAR_DATA_PTR new_var  = _hexpr_tab->Allocate<VAR_DATA>();
  new (new_var) VAR_DATA(var_data);
  new_var->Set_ver(ver);
  return HEXPR_PTR(HEXPR(this, new_var));
}

HEXPR_PTR HCONTAINER::New_preg_expr(HEXPR_PTR expr) {
  CONTAINER*  cont = Air_cont();
  FUNC_SCOPE* fs   = cont->Parent_func_scope();
  PREG_PTR    preg = fs->New_preg(expr->Rtype());

  VAR_DATA_PTR new_var = _hexpr_tab->Allocate<VAR_DATA>();
  new (new_var) VAR_DATA(preg);
  new_var->Set_ver(0);
  return HEXPR_PTR(HEXPR(this, new_var));
}

HEXPR_PTR HCONTAINER::Find_or_new_expr(HEXPR_PTR expr) {
  return _htable.Find_or_insert(expr);
}

HEXPR_PTR HCONTAINER::New_op_expr(OPCODE opcode, TYPE_ID rtype, TYPE_ID dsctype,
                                  SPOS spos) {
  uint32_t num_child = META_INFO::Op_num_child(opcode);
  OP_DATA* op_data   = OP_DATA::Alloc(num_child);
  new (op_data) OP_DATA(opcode, num_child, rtype, dsctype, spos);
  HEXPR_PTR new_op = New_op_expr(OP_DATA_PTR(op_data, OP_HEXPR_ID()));
  free(op_data);
  return new_op;
}

void HCONTAINER::Print(std::ostream& os) const { Print_stmt_list(os); }

void HCONTAINER::Print_stmt_list(std::ostream& os) const {
  HSTMT_LIST stmt_list(this, _root_stmt);
  stmt_list.Print(os);
}

}  // namespace opt
}  // namespace air
