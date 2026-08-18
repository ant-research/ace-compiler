//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/opt/hssa_container.h"

#include "air/opt/bb.h"
#include "air/opt/occ.h"

using namespace air::base;

namespace air {

namespace opt {

HSSA_CONTAINER::HSSA_CONTAINER(CONTAINER* cont, SSA_CONTAINER* ssa_cont,
                               uint32_t htable_size)
    : _cont(cont), _ssa_cont(ssa_cont), _htable_size(htable_size) {
  _root_stmt = air::base::Null_id;

  HCR_ALLOC             hcr_alloc(&_mpool);
  HSTMT_ALLOC           hstmt_alloc(&_mpool);
  HPHI_ALLOC            hphi_alloc(&_mpool);
  HMU_ALLOC             hmu_alloc(&_mpool);
  HCHI_ALLOC            hchi_alloc(&_mpool);
  U32_U32_MAP_ALLOCATOR u32map_alloc(&_mpool);

  _hcr_tab   = hcr_alloc.Allocate(&_mpool, HCR_TAB_KIND, "hcr_tab", true);
  _hstmt_tab = hstmt_alloc.Allocate(&_mpool, HSTMT_TAB_KIND, "hstmt_tab", true);
  _hphi_tab  = hphi_alloc.Allocate(&_mpool, HPHI_TAB_KIND, "hphi_tab", true);
  _hmu_tab   = hmu_alloc.Allocate(&_mpool, HMU_TAB_KIND, "hmu_tab", true);
  _hchi_tab  = hchi_alloc.Allocate(&_mpool, HCHI_TAB_KIND, "hchi_tab", true);
  _ver_cr_map        = u32map_alloc.Allocate(13, std::hash<uint32_t>(),
                                             std::equal_to<uint32_t>(),
                                             U32_U32_PAIR_ALLOCATOR(&_mpool));
  HCR_ID* htable_ptr = (HCR_ID*)_mpool.Allocate(sizeof(HCR_ID) * htable_size);
  _htable            = new (htable_ptr) HCR_ID[htable_size]();
}

void HSSA_CONTAINER::Add_hcr(uint32_t hash_idx, HCR_PTR cr) {
  HCR_ID bucket_head = _htable[hash_idx];
  if (bucket_head == HCR_ID()) {
    _htable[hash_idx] = cr->Id();
  } else {
    HCR_LIST cr_list(this, bucket_head);
    cr_list.Prepend(cr->Id());
    _htable[hash_idx] = cr->Id();
  }
}

HCR_PTR HSSA_CONTAINER::Find_or_new_var_cr(SSA_VER_ID id) {
  HCR_ID cr_id = Ver_cr(id);
  if (cr_id != Null_id) {
    return Cr_ptr(cr_id);
  } else {
    VAR_HCR_DATA_PTR var_cr = _hcr_tab->Allocate<VAR_HCR_DATA>();
    new (var_cr) VAR_HCR_DATA(Ssa_cont()->Ver_sym(id));
    HCR_PTR cr = HCR_PTR(HCR(this, var_cr));
    Set_ver_cr(id, cr);
    return cr;
  }
}

HCR_PTR HSSA_CONTAINER::New_var_cr(ADDR_DATUM_PTR datum, uint32_t sub_idx) {
  VAR_HCR_DATA_PTR var_cr = _hcr_tab->Allocate<VAR_HCR_DATA>();
  new (var_cr) VAR_HCR_DATA(datum, sub_idx);
  HCR_PTR cr = HCR_PTR(HCR(this, var_cr));
  return cr;
}

HCR_PTR HSSA_CONTAINER::New_op_cr(OP_HCR_DATA_PTR op_cr) {
  OP_HCR_DATA_PTR op_ptr = air::base::Static_cast<OP_HCR_DATA_PTR>(
      _hcr_tab->Malloc(OP_HCR_DATA::Size(op_cr->Kid_cnt())));
  new (op_ptr) OP_HCR_DATA(op_cr);
  return HCR_PTR(HCR(this, op_ptr));
}

HCR_PTR HSSA_CONTAINER::New_cst_cr(uint64_t cst_val) {
  TYPE_ID id = Air_cont()
                   ->Glob_scope()
                   ->Prim_type(air::base::PRIMITIVE_TYPE::INT_U64)
                   ->Id();
  CST_HCR_DATA cst_data(cst_val, id);
  HCR_DATA_PTR cst_ptr(&cst_data, HCR_ID());
  HCR_PTR      ret = Find_or_new_cr(HCR_PTR(HCR(this, cst_ptr)));
  return ret;
}

HCR_PTR HSSA_CONTAINER::New_cst_cr(CST_HCR_DATA_PTR cst_cr) {
  CST_HCR_DATA_PTR cr_ptr = _hcr_tab->Allocate<CST_HCR_DATA>();
  new (cr_ptr) CST_HCR_DATA(cst_cr);
  return HCR_PTR(HCR(this, cr_ptr));
}

HCR_PTR HSSA_CONTAINER::New_cr(HCR_PTR cr) {
  switch (cr->Kind()) {
    case CK_VAR:
      CMPLR_ASSERT(false, "TO IMPL");
      break;
    case CK_OP: {
      OP_HCR_DATA_PTR op_cr = cr->Cast_to_op_cr();
      return New_op_cr(op_cr);
    }
    case CK_CONST: {
      CST_HCR_DATA_PTR cst_cr = cr->Cast_to_cst_cr();
      return New_cst_cr(cst_cr);
    }
    default:
      CMPLR_ASSERT(false, "TO IMPL");
  }
  return Null_ptr;
}

HSTMT_PTR HSSA_CONTAINER::New_assign_stmt(air::base::NODE_PTR node) {
  ASSIGN_HSTMT_DATA_PTR stmt_ptr = _hstmt_tab->Allocate<ASSIGN_HSTMT_DATA>();
  new (stmt_ptr) ASSIGN_HSTMT_DATA(node);
  return HSTMT_PTR(HSTMT(this, stmt_ptr));
}

HSTMT_PTR HSSA_CONTAINER::New_assign_stmt(HCR_PTR var, HCR_PTR rhs) {
  ASSIGN_HSTMT_DATA_PTR stmt_ptr = _hstmt_tab->Allocate<ASSIGN_HSTMT_DATA>();
  new (stmt_ptr) ASSIGN_HSTMT_DATA(OPCODE(air::core::CORE, air::core::ST),
                                   var->Id(), rhs->Id());
  HSTMT_PTR ret = HSTMT_PTR(HSTMT(this, stmt_ptr));
  var->Cast_to_var_cr()->Set_def_stmt(ret->Id());
  return ret;
}

HSTMT_PTR HSSA_CONTAINER::New_op_stmt(air::base::NODE_PTR node) {
  OP_HSTMT_DATA_PTR op_ptr = air::base::Static_cast<OP_HSTMT_DATA_PTR>(
      _hstmt_tab->Malloc(OP_HSTMT_DATA::Size(node->Num_child())));
  new (op_ptr) OP_HSTMT_DATA(node);
  return HSTMT_PTR(HSTMT(this, op_ptr));
}

HSTMT_PTR HSSA_CONTAINER::New_entry_stmt(air::base::NODE_PTR node) {
  OP_HSTMT_DATA_PTR op_ptr = air::base::Static_cast<OP_HSTMT_DATA_PTR>(
      _hstmt_tab->Malloc(OP_HSTMT_DATA::Size(node->Num_child() - 1)));
  new (op_ptr) OP_HSTMT_DATA(node);
  return HSTMT_PTR(HSTMT(this, op_ptr));
}

HSTMT_PTR HSSA_CONTAINER::New_do_loop(air::base::NODE_PTR node, HSTMT_PTR init,
                                      HCR_PTR cond, HSTMT_PTR body,
                                      HCR_PTR incr, HPHI_PTR hphi) {
  DO_LOOP_HSTMT_DATA_PTR stmt_ptr = _hstmt_tab->Allocate<DO_LOOP_HSTMT_DATA>();
  new (stmt_ptr)
      DO_LOOP_HSTMT_DATA(node, init->Id(), cond->Id(), body->Id(), incr->Id());
  return HSTMT_PTR(HSTMT(this, stmt_ptr));
}

HSTMT_PTR HSSA_CONTAINER::New_call(air::base::NODE_PTR node) {
  CALL_HSTMT_DATA_PTR call_ptr = air::base::Static_cast<CALL_HSTMT_DATA_PTR>(
      _hstmt_tab->Malloc(OP_HSTMT_DATA::Size(node->Num_arg())));
  new (call_ptr) CALL_HSTMT_DATA(node);
  return HSTMT_PTR(HSTMT(this, call_ptr));
}

HSTMT_PTR HSSA_CONTAINER::New_if(air::base::NODE_PTR node, HCR_PTR cond) {
  IF_HSTMT_DATA_PTR stmt_ptr = _hstmt_tab->Allocate<IF_HSTMT_DATA>();
  new (stmt_ptr) IF_HSTMT_DATA(OPCODE(air::core::CORE, air::core::IF), cond);
  return HSTMT_PTR(HSTMT(this, stmt_ptr));
}

HMU_PTR HSSA_CONTAINER::New_mu(HSTMT_PTR stmt) {
  HMU_DATA_PTR mu_data = _hmu_tab->Allocate<HMU_DATA>();
  mu_data->Init();
  return HMU_PTR(HMU(this, mu_data));
}

HCHI_PTR HSSA_CONTAINER::New_chi(HSTMT_PTR stmt) {
  HCHI_DATA_PTR chi_data = _hchi_tab->Allocate<HCHI_DATA>();
  new (chi_data) HCHI_DATA(stmt->Id());
  return HCHI_PTR(HCHI(this, chi_data));
}

HPHI_PTR HSSA_CONTAINER::New_phi(BB_PTR bb, uint32_t num_opnd) {
  AIR_ASSERT(bb->Kind() == BB_IF_PHI || bb->Kind() == BB_LOOP_PHI);
  uint32_t mem_size = (sizeof(HPHI_DATA) / _hphi_tab->Unit_size()) + num_opnd;
  HPHI_DATA_PTR phi_ptr =
      air::base::Static_cast<HPHI_DATA_PTR>(_hphi_tab->Malloc(mem_size));
  new (phi_ptr) HPHI_DATA(bb->Id(), num_opnd);
  return HPHI_PTR(HPHI(this, phi_ptr));
}

HCR_PTR HSSA_CONTAINER::New_var_with_ver(HCR_PTR cr, uint32_t ver) {
  VAR_HCR_DATA_PTR var_cr  = cr->Cast_to_var_cr();
  VAR_HCR_DATA_PTR new_var = _hcr_tab->Allocate<VAR_HCR_DATA>();
  new (new_var) VAR_HCR_DATA(var_cr);
  new_var->Set_ver(ver);
  return HCR_PTR(HCR(this, new_var));
}

HCR_PTR HSSA_CONTAINER::New_preg_cr(HCR_PTR cr) {
  CONTAINER*  cont = Air_cont();
  FUNC_SCOPE* fs   = cont->Parent_func_scope();
  PREG_PTR    preg = fs->New_preg(cr->Rtype());

  VAR_HCR_DATA_PTR new_var = _hcr_tab->Allocate<VAR_HCR_DATA>();
  new (new_var) VAR_HCR_DATA(preg);
  new_var->Set_ver(0);
  return HCR_PTR(HCR(this, new_var));
}

HCR_PTR HSSA_CONTAINER::Find_or_new_cr(HCR_PTR cr) {
  uint32_t hash_idx    = cr->Hash_idx();
  uint32_t bucket_idx  = hash_idx % Htable_size();
  HCR_ID   bucket_head = _htable[bucket_idx];

  HCR_LIST cr_list(this, bucket_head);
  HCR_PTR  ret = cr_list.Find(cr);
  if (ret->Is_null()) {
    ret = New_cr(cr);
    Add_hcr(bucket_idx, ret);
  }
  return ret;
}

HCR_PTR HSSA_CONTAINER::New_op_cr(OPCODE opcode, TYPE_ID rtype, TYPE_ID dsctype,
                                  SPOS spos) {
  uint32_t     num_child = META_INFO::Op_num_child(opcode);
  OP_HCR_DATA* op_cr     = OP_HCR_DATA::Alloc(num_child);
  new (op_cr) OP_HCR_DATA(opcode, num_child, rtype, dsctype, spos);
  HCR_PTR new_op = New_op_cr(OP_HCR_DATA_PTR(op_cr, OP_HCR_ID()));
  free(op_cr);
  return new_op;
}

void HSSA_CONTAINER::Print(std::ostream& os) const { Print_stmt_list(os); }

void HSSA_CONTAINER::Print_stmt_list(std::ostream& os) const {
  HSTMT_LIST stmt_list(this, _root_stmt);
  stmt_list.Print(os);
}

void HSSA_CONTAINER::Print_htable(std::ostream& os) const {
  os << "HTABLE: " << std::endl;
  for (uint32_t idx = 0; idx < _htable_size; idx++) {
    HCR_ID head = _htable[idx];
    if (head != HCR_ID()) {
      os << "[" << idx << "]";
      HCR_LIST cr_list(this, head);
      cr_list.Print_id(os);
      os << std::endl;
    }
  }
}

}  // namespace opt
}  // namespace air