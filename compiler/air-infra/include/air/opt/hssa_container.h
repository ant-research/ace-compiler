//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_HCONTAINER_H
#define AIR_OPT_HCONTAINER_H

#include "air/base/opcode.h"
#include "air/base/st.h"
#include "air/opt/hssa_expr.h"
#include "air/opt/hssa_stmt.h"
#include "air/opt/node_list.h"
#include "air/opt/ssa_decl.h"
#include "air/opt/ssa_node_list.h"

namespace air {

namespace opt {

enum {
  EXPR_HTABLE_SIZE = 227,
  DEF_EXPR_BITS    = 4,
};

class HCONTAINER;
class DU_INFO;

//! @brief Hash table for HEXPR deduplication (chained hashing)
//! Owns the bucket array and provides Find/Insert/Print operations.
//! Chaining uses HEXPR::Next_id() as the linked-list pointer.
class EXPR_HTABLE {
public:
  EXPR_HTABLE() : _buckets(nullptr), _size(0), _cont(nullptr) {}

  void Init(HEXPR_ID* buckets, uint32_t size, HCONTAINER* cont) {
    _buckets = buckets;
    _size    = size;
    _cont    = cont;
  }

  uint32_t Size() const { return _size; }

  //! @brief Find expr in hash table by content match
  //! @return matching HEXPR_PTR, or null if not found
  HEXPR_PTR Find(HEXPR_PTR expr) const;

  //! @brief Insert expr into hash table (assumes not already present)
  void Insert(HEXPR_PTR expr);

  //! @brief Find existing or insert new expression
  //! @return existing match or newly inserted copy
  HEXPR_PTR Find_or_insert(HEXPR_PTR expr);

  void Print(std::ostream& os) const;

private:
  uint32_t    Bucket_idx(HEXPR_PTR expr) const;
  HEXPR_ID*   _buckets;  //!< bucket array (allocated from HCONTAINER's mempool)
  uint32_t    _size;     //!< number of buckets
  HCONTAINER* _cont;     //!< owning container (for Expr_ptr/New_expr)
};

class HCONTAINER {
public:
  HCONTAINER(air::base::CONTAINER* cont, SSA_CONTAINER* ssa_cont,
             uint32_t htable_size = EXPR_HTABLE_SIZE);

  air::base::CONTAINER* Air_cont() const { return _cont; }
  SSA_CONTAINER*        Ssa_cont() const { return _ssa_cont; }

  EXPR_HTABLE&       Htable() { return _htable; }
  const EXPR_HTABLE& Htable() const { return _htable; }

  HSTMT_ID Root_stmt_id(void) const { return _root_stmt; }

  HSTMT_PTR Root_stmt(void) const { return Stmt_ptr(_root_stmt); }

  void Set_root_stmt(HSTMT_PTR root_stmt) { _root_stmt = root_stmt->Id(); }

  //! @brief create a new version from var expression
  HEXPR_PTR New_var_with_ver(HEXPR_PTR var_expr, uint32_t ver);

  HEXPR_PTR New_preg_expr(HEXPR_PTR expr);

  HEXPR_PTR Find_or_new_var_expr(SSA_VER_ID id);

  //! @brief create a new expression from symbol, for new created symbol
  //!        without SSA
  HEXPR_PTR New_var_expr(air::base::ADDR_DATUM_PTR sym,
                         uint32_t                  sub_idx = SSA_SYM::NO_INDEX);

  HEXPR_PTR New_op_expr(OP_DATA_PTR op_expr);

  HEXPR_PTR New_cst_expr(uint64_t cst_val);
  HEXPR_PTR New_cst_expr(CST_DATA_PTR cst_expr);

  HEXPR_PTR New_expr(HEXPR_PTR expr);

  //! @brief Find existing or create new expression via hash table
  HEXPR_PTR Find_or_new_expr(HEXPR_PTR expr);

  HEXPR_PTR New_op_expr(air::base::OPCODE opcode, air::base::TYPE_ID rtype,
                        air::base::TYPE_ID dsctype, air::base::SPOS spos);

  HEXPR_PTR Find_or_new_cst_expr(CST_DATA_PTR cst_expr);

  HSTMT_PTR New_do_loop(air::base::NODE_PTR node, HSTMT_PTR init,
                        HEXPR_PTR cond, HSTMT_PTR body, HEXPR_PTR incr,
                        HPHI_PTR hphi);

  HSTMT_PTR New_call(air::base::NODE_PTR node);

  HSTMT_PTR New_if(air::base::NODE_PTR node, HEXPR_PTR cond);

  HSTMT_PTR New_assign_stmt(air::base::NODE_PTR node);

  HSTMT_PTR New_assign_stmt(HEXPR_PTR var, HEXPR_PTR rhs);

  HSTMT_PTR New_op_stmt(air::base::NODE_PTR node);

  HSTMT_PTR New_entry_stmt(air::base::NODE_PTR node);

  HMU_PTR New_mu(HSTMT_PTR stmt);

  HCHI_PTR New_chi(HSTMT_PTR stmt);

  HPHI_PTR New_phi(BB_PTR, uint32_t num_opnd);

  void Set_ver_expr(SSA_VER_ID ver_id, HEXPR_PTR expr_ptr) {
    AIR_ASSERT(expr_ptr->Id().Value() < _hexpr_tab->Size());
    (*_ver_expr_map)[ver_id.Value()] = expr_ptr->Id().Value();
  }

  HEXPR_ID Ver_expr(SSA_VER_ID ver_id) const {
    U32_U32_MAP::const_iterator it = _ver_expr_map->find(ver_id.Value());
    return (it != _ver_expr_map->end()) ? HEXPR_ID(it->second) : HEXPR_ID();
  }

  HSTMT_PTR Node(HSTMT_ID sr_id) const { return Stmt_ptr(sr_id); }
  HEXPR_PTR Node(HEXPR_ID expr_id) const { return Expr_ptr(expr_id); }
  HMU_PTR   Node(HMU_ID hmu_id) const { return Mu_ptr(hmu_id); }
  HCHI_PTR  Node(HCHI_ID hchi_id) const { return Chi_ptr(hchi_id); }
  HPHI_PTR  Node(HPHI_ID hphi_id) const { return Phi_ptr(hphi_id); }

  HSTMT_PTR Stmt_ptr(HSTMT_ID sr_id) const {
    return HSTMT_PTR(HSTMT(this, _hstmt_tab->Find(sr_id)));
  }
  HEXPR_PTR Expr_ptr(HEXPR_ID expr_id) const {
    return HEXPR_PTR(HEXPR(this, _hexpr_tab->Find(expr_id)));
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
    CMPLR_ASSERT(false, "Build_du_info not yet implemented");
  }

  void Print(std::ostream& os) const;

  void Print_stmt_list(std::ostream& os) const;

  void Print_htable(std::ostream& os) const { _htable.Print(os); }

private:
  typedef air::util::MEM_POOL<4096> MEM_POOL;

  static constexpr uint32_t HEXPR_TAB_KIND = 0x20001;
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

  typedef air::base::ARENA<4, 4, false>                  HEXPR_TAB;
  typedef air::base::ARENA<sizeof(HSTMT_DATA), 4, false> HSTMT_TAB;
  typedef air::base::ARENA<4, 4, false>                  HPHI_TAB;
  typedef air::base::ARENA<sizeof(HCHI_DATA), 4, false>  HCHI_TAB;
  typedef air::base::ARENA<sizeof(HMU_DATA), 4, false>   HMU_TAB;

  typedef air::util::CXX_MEM_ALLOCATOR<HEXPR_TAB, MEM_POOL> HEXPR_ALLOC;
  typedef air::util::CXX_MEM_ALLOCATOR<HSTMT_TAB, MEM_POOL> HSTMT_ALLOC;
  typedef air::util::CXX_MEM_ALLOCATOR<HPHI_TAB, MEM_POOL>  HPHI_ALLOC;
  typedef air::util::CXX_MEM_ALLOCATOR<HCHI_TAB, MEM_POOL>  HCHI_ALLOC;
  typedef air::util::CXX_MEM_ALLOCATOR<HMU_TAB, MEM_POOL>   HMU_ALLOC;

  air::base::CONTAINER* _cont;
  SSA_CONTAINER*        _ssa_cont;
  MEM_POOL              _mpool;  //!< Mempool for all HSSA tables
  HSTMT_ID              _root_stmt;
  HSTMT_TAB*            _hstmt_tab;
  HEXPR_TAB*            _hexpr_tab;
  HPHI_TAB*             _hphi_tab;
  HCHI_TAB*             _hchi_tab;
  HMU_TAB*              _hmu_tab;
  U32_U32_MAP*          _ver_expr_map;
  EXPR_HTABLE           _htable;
};

}  // namespace opt

}  // namespace air
#endif
