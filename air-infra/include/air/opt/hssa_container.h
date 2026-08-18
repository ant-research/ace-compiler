//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_HSSA_CONTAINER_H
#define AIR_OPT_HSSA_CONTAINER_H

#include "air/base/opcode.h"
#include "air/base/st.h"
#include "air/opt/hssa_expr.h"
#include "air/opt/hssa_stmt.h"
#include "air/opt/node_list.h"
#include "air/opt/pre_decl.h"
#include "air/opt/ssa_decl.h"
#include "air/opt/ssa_node_list.h"

namespace air {

namespace opt {

enum {
  CR_HTABLE_SIZE = 227,
  DEF_CR_BITS    = 4,
};

class HSSA_CONTAINER;
class DU_INFO;

class HSSA_CONTAINER {
public:
  HSSA_CONTAINER(air::base::CONTAINER* cont, SSA_CONTAINER* ssa_cont,
                 uint32_t htable_size = CR_HTABLE_SIZE);

  air::base::CONTAINER* Air_cont() const { return _cont; }
  SSA_CONTAINER*        Ssa_cont() const { return _ssa_cont; }

  uint32_t Htable_size() const { return _htable_size; }

  HSTMT_ID Root_stmt_id(void) const { return _root_stmt; }

  HSTMT_PTR Root_stmt(void) const { return Stmt_ptr(_root_stmt); }

  void Set_root_stmt(HSTMT_PTR root_stmt) { _root_stmt = root_stmt->Id(); }

  // create a new version from var_cr
  HCR_PTR New_var_with_ver(HCR_PTR var_cr, uint32_t ver);

  HCR_PTR New_preg_cr(HCR_PTR cr);

  HCR_PTR Find_or_new_var_cr(SSA_VER_ID id);

  // create a new cr from symbol, for new created symbol without SSA
  HCR_PTR New_var_cr(air::base::ADDR_DATUM_PTR sym,
                     uint32_t                  sub_idx = SSA_SYM::NO_INDEX);

  HCR_PTR New_op_cr(OP_HCR_DATA_PTR op_cr);

  HCR_PTR New_cst_cr(uint64_t cst_val);
  HCR_PTR New_cst_cr(CST_HCR_DATA_PTR cst_cr);

  HCR_PTR New_cr(HCR_PTR cr);

  HCR_PTR Find_or_new_cr(HCR_PTR op_cr);

  HCR_PTR New_op_cr(air::base::OPCODE opcode, air::base::TYPE_ID rtype,
                    air::base::TYPE_ID dsctype, air::base::SPOS spos);

  HCR_PTR Find_or_new_cst_cr(CST_HCR_DATA_PTR cst_cr);

  HSTMT_PTR New_do_loop(air::base::NODE_PTR node, HSTMT_PTR init, HCR_PTR cond,
                        HSTMT_PTR body, HCR_PTR incr, HPHI_PTR hphi);

  HSTMT_PTR New_call(air::base::NODE_PTR node);

  HSTMT_PTR New_if(air::base::NODE_PTR node, HCR_PTR cond);

  HSTMT_PTR New_assign_stmt(air::base::NODE_PTR node);

  HSTMT_PTR New_assign_stmt(HCR_PTR var, HCR_PTR rhs);

  HSTMT_PTR New_op_stmt(air::base::NODE_PTR node);

  HSTMT_PTR New_entry_stmt(air::base::NODE_PTR node);

  HMU_PTR New_mu(HSTMT_PTR stmt);

  HCHI_PTR New_chi(HSTMT_PTR stmt);

  HPHI_PTR New_phi(BB_PTR, uint32_t num_opnd);

  uint32_t Hash_const(int64_t val);

  HCR_PTR Hash_expr(HCR* cr) {
    CMPLR_ASSERT(false, "TO IMPL Hash_expr");
    return air::base::Null_ptr;
  }

  HCR_PTR Hash_op(OP_HCR_DATA& cr);

  void Add_hcr(uint32_t hash_idx, HCR_PTR cr);

  void Set_ver_cr(SSA_VER_ID ver_id, HCR_PTR cr_ptr) {
    AIR_ASSERT(cr_ptr->Id().Value() < _hcr_tab->Size());
    (*_ver_cr_map)[ver_id.Value()] = cr_ptr->Id().Value();
  }

  HCR_ID Ver_cr(SSA_VER_ID ver_id) const {
    U32_U32_MAP::const_iterator it = _ver_cr_map->find(ver_id.Value());
    return (it != _ver_cr_map->end()) ? HCR_ID(it->second) : HCR_ID();
  }

  HSTMT_PTR Node(HSTMT_ID sr_id) const { return Stmt_ptr(sr_id); }
  HCR_PTR   Node(HCR_ID sr_id) const { return Cr_ptr(sr_id); }
  HMU_PTR   Node(HMU_ID hmu_id) const { return Mu_ptr(hmu_id); }
  HCHI_PTR  Node(HCHI_ID hchi_id) const { return Chi_ptr(hchi_id); }
  HPHI_PTR  Node(HPHI_ID hphi_id) const { return Phi_ptr(hphi_id); }

  HSTMT_PTR Stmt_ptr(HSTMT_ID sr_id) const {
    return HSTMT_PTR(HSTMT(this, _hstmt_tab->Find(sr_id)));
  }
  HCR_PTR Cr_ptr(HCR_ID cr_id) const {
    return HCR_PTR(HCR(this, _hcr_tab->Find(cr_id)));
  }
  HMU_PTR Mu_ptr(HMU_ID hmu_id) const {
    return HMU_PTR(HMU(this, _hchi_tab->Find(hmu_id)));
  }
  HCHI_PTR Chi_ptr(HCHI_ID hchi_id) const {
    return HCHI_PTR(HCHI(this, _hchi_tab->Find(hchi_id)));
  }
  HPHI_PTR Phi_ptr(HPHI_ID hphi_id) const {
    return HPHI_PTR(HPHI(this, _hphi_tab->Find(hphi_id)));
  }

  void Build_du_info(DU_INFO* du_info) {
    //CMPLR_ASSERT(false, "ALARM: TO IMPL");
  }

  void Print(std::ostream& os) const;

  void Print_stmt_list(std::ostream& os) const;

  void Print_htable(std::ostream& os) const;

private:
  typedef air::util::MEM_POOL<4096> MEM_POOL;

  static constexpr uint32_t HCR_TAB_KIND   = 0x20001;
  static constexpr uint32_t HSTMT_TAB_KIND = 0x20002;
  static constexpr uint32_t HPHI_TAB_KIND  = 0x20003;
  static constexpr uint32_t HMU_TAB_KIND   = 0x20004;
  static constexpr uint32_t HCHI_TAB_KIND  = 0x20005;

  typedef std::pair<uint32_t, uint32_t> U32_U32_PAIR;
  typedef air::util::CXX_MEM_ALLOCATOR<U32_U32_PAIR, MEM_POOL>
      U32_U32_PAIR_ALLOCATOR;
  typedef std::unordered_map<uint32_t, uint32_t, std::hash<uint32_t>,
                             std::equal_to<uint32_t>, U32_U32_PAIR_ALLOCATOR>
      U32_U32_MAP;

  typedef air::util::CXX_MEM_ALLOCATOR<U32_U32_MAP, MEM_POOL>
      U32_U32_MAP_ALLOCATOR;

  typedef air::base::ARENA<4, 4, false>                  HCR_TAB;
  typedef air::base::ARENA<sizeof(HSTMT_DATA), 4, false> HSTMT_TAB;
  typedef air::base::ARENA<4, 4, false>                  HPHI_TAB;
  typedef air::base::ARENA<sizeof(HCHI_DATA), 4, false>  HCHI_TAB;
  typedef air::base::ARENA<sizeof(HMU_DATA), 4, false>   HMU_TAB;

  typedef air::util::CXX_MEM_ALLOCATOR<HCR_TAB, MEM_POOL>   HCR_ALLOC;
  typedef air::util::CXX_MEM_ALLOCATOR<HSTMT_TAB, MEM_POOL> HSTMT_ALLOC;
  typedef air::util::CXX_MEM_ALLOCATOR<HPHI_TAB, MEM_POOL>  HPHI_ALLOC;
  typedef air::util::CXX_MEM_ALLOCATOR<HCHI_TAB, MEM_POOL>  HCHI_ALLOC;
  typedef air::util::CXX_MEM_ALLOCATOR<HMU_TAB, MEM_POOL>   HMU_ALLOC;

  air::base::CONTAINER* _cont;
  SSA_CONTAINER*        _ssa_cont;
  MEM_POOL              _mpool;  //!< Mempool for all HSSA tables
  HSTMT_ID              _root_stmt;
  HSTMT_TAB*            _hstmt_tab;
  HCR_TAB*              _hcr_tab;
  HPHI_TAB*             _hphi_tab;
  HCHI_TAB*             _hchi_tab;
  HMU_TAB*              _hmu_tab;
  U32_U32_MAP*          _ver_cr_map;
  uint32_t              _htable_size;
  HCR_ID*               _htable;
};

}  // namespace opt

}  // namespace air
#endif
