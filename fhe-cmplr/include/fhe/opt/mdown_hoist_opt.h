//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_OPT_MDOWN_HOIST_OPT_H
#define FHE_OPT_MDOWN_HOIST_OPT_H

#include <map>

#include "air/base/container.h"
#include "air/opt/du_info.h"
#include "air/opt/hssa_expr.h"
#include "air/opt/hssa_func.h"
#include "air/opt/hssa_stmt.h"
#include "fhe/core/lower_ctx.h"
#include "fhe/poly/config.h"

namespace fhe {
namespace poly {

class MDOWN_CAND;
typedef air::util::MEM_POOL<4096> MDOWN_OPT_MEM_POOL;
typedef air::util::CXX_MEM_ALLOCATOR<MDOWN_CAND*, MDOWN_OPT_MEM_POOL>
                                                   MDOWN_CAND_ALLOC;
typedef std::vector<MDOWN_CAND*, MDOWN_CAND_ALLOC> MDOWN_CAND_LIST;

#define MAX_ITERATION 3

enum MDOWN_FLAG {
  NONE          = 0x0,
  CANNOT_EXTEND = 0x1,
  IS_MDOWN      = 0x2,
  KID_HAS_MDOWN = 0x4,
  IS_CAND       = 0x8,
};

class MDOWN_HOIST_OPT {
public:
  MDOWN_HOIST_OPT(POLY_CONFIG& config, air::driver::DRIVER_CTX* ctx,
                  core::LOWER_CTX* lower_ctx)
      : _config(config),
        _driver_ctx(ctx),
        _lower_ctx(lower_ctx),
        _cur_func(nullptr),
        _iteration(0),
        _factor_cnt(0),
        _sink_cnt(0),
        _worklist(MDOWN_CAND_ALLOC(&_mpool)) {}

  air::opt::CFG& Cfg() { return Cur_func()->Cfg(); }

  const MDOWN_CAND_LIST& Worklist() const { return _worklist; }

  void Run(air::opt::HSSA_FUNC* hssa_func);

  uint32_t Iteration(void) const { return _iteration; }
  bool     Is_encode(air::opt::HEXPR_PTR expr);
  bool     Is_mdown(air::opt::HEXPR_PTR expr);
  bool     Is_mdown_check_point(air::opt::HEXPR_PTR expr);
  bool     Is_modswitch(air::opt::HEXPR_PTR expr);

  void Add_cand(MDOWN_CAND& cand);

  void Add_cand(air::opt::HEXPR_PTR expr);

  void Add_cand_parent(const air::opt::NODE_INFO& node_info);

  MDOWN_FLAG Get_expr_flag(air::opt::HEXPR_PTR expr);
  void       Set_expr_flag(air::opt::HEXPR_PTR expr, MDOWN_FLAG f);
  void       Clear_expr_flag(air::opt::HEXPR_PTR expr, MDOWN_FLAG f);
  bool       Has_expr_flag(air::opt::HEXPR_PTR expr, MDOWN_FLAG f);
  void       Prop_flag(air::opt::HEXPR_PTR expr1, air::opt::HEXPR_PTR expr2);

  bool Is_expr_used_in_loop(air::opt::HEXPR_PTR     expr,
                            air::opt::LOOP_INFO_PTR loop_info);

  void Print_worklist(std::ostream& os) const;

  DECLARE_TRACE_DETAIL_API(_config, _driver_ctx)
public:
  POLY_CONFIG&             Config() const { return _config; }
  air::driver::DRIVER_CTX* Driver_ctx() const { return _driver_ctx; }
  air::opt::HSSA_FUNC*     Cur_func() const { return _cur_func; }
  air::opt::HCONTAINER*    Hssa_cont() const { return Cur_func()->Hssa_cont(); }
  air::opt::DU_INFO&       Du_info() { return _du_info; }
  void Set_cur_func(air::opt::HSSA_FUNC* func) { _cur_func = func; }

  void Create_worklist();

  void Mdown_commutation(MDOWN_CAND* cand);
  void Mdown_commutation(air::opt::HEXPR_PTR cand);

  void Mark_extend(air::opt::HEXPR_PTR op);
  void Extend_op(air::opt::HEXPR_PTR op);
  bool Need_extend(air::opt::HEXPR_PTR op);
  void Commute_op(air::opt::HEXPR_PTR op1, air::opt::HEXPR_PTR op2,
                  air::opt::HEXPR_PTR op1_pexpr);
  void Commute_op(air::opt::HEXPR_PTR op1, air::opt::HEXPR_PTR op2,
                  air::opt::HSTMT_PTR op1_pstmt);

  void Mdown_factor(MDOWN_CAND* cand);
  void Mdown_factor(air::opt::HEXPR_PTR cand, air::opt::NODE_INFO& cand_parent);
  void Mdown_sinking(air::opt::HEXPR_PTR  cand,
                     air::opt::NODE_INFO& cand_parent);

  bool Has_ext_attr(air::opt::HEXPR_PTR expr);
  bool Has_ext_var(air::opt::HEXPR_PTR var_expr);
  //! Get or create extended variable
  air::opt::HEXPR_PTR Get_ext_var(air::opt::HEXPR_PTR var_expr);
  air::opt::HEXPR_PTR New_ext_var(air::opt::HEXPR_PTR var_expr);
  //! Get or create versioned extended variable
  air::opt::HEXPR_PTR Get_ext_var_with_ver(air::opt::HEXPR_PTR expr);

  air::opt::HPHI_PTR Find_ext_phi(air::opt::HPHI_PTR phi);
  air::opt::HPHI_PTR New_ext_phi(air::opt::HPHI_PTR phi);

  void Set_flag_changed(bool v) { _flag_changed = v; }
  bool Is_flag_changed(void) const { return _flag_changed; }

public:
  MDOWN_OPT_MEM_POOL       _mpool;
  POLY_CONFIG&             _config;
  air::driver::DRIVER_CTX* _driver_ctx;
  air::opt::HSSA_FUNC*     _cur_func;
  core::LOWER_CTX*         _lower_ctx;
  std::map<uint64_t, air::opt::HEXPR_PTR>
      _ext_sym_map;  // [SYM_ID_VALUE, HEXPR_PTR]
  std::map<uint32_t, air::opt::HEXPR_PTR>
      _ext_expr_map;  // [HEXPR_ID_VALUE, HEXPR_PTR]
  std::map<uint32_t, air::opt::HPHI_PTR> _ext_phi_map;
  MDOWN_CAND_LIST                        _worklist;
  std::map<uint32_t, MDOWN_FLAG>         _expr_flags;
  bool                                   _flag_changed;
  std::vector<air::opt::HEXPR_PTR>       _worklist2;
  uint32_t                               _iteration;
  std::vector<air::opt::NODE_INFO>       _cand_parent;
  air::opt::DU_INFO                      _du_info;
  uint32_t                               _factor_cnt;
  uint32_t                               _sink_cnt;
};

using NODE_STACK = std::vector<air::opt::HEXPR_PTR>;

class MDOWN_CAND {
public:
  enum KID_DEF_STATUS {
    DEF_BY_UNKNOWN,  // kid is defined by unkown
    DEF_BY_RECUR,    // kid is defined by recursion
    DEF_BY_MDOWN,    // kid is defined by mod down
  };
  MDOWN_CAND(air::opt::HEXPR_PTR expr, air::opt::NODE_INFO parent) {
    uint32_t kid_cnt = expr->Kid_cnt();
    _root            = expr;
    _root_parent     = parent;
    _kid_sts.resize(kid_cnt, DEF_BY_UNKNOWN);
    _ud_chain.resize(kid_cnt);
  }

  MDOWN_CAND(MDOWN_CAND& cand) {
    _root        = cand.Root();
    _root_parent = cand.Root_parent();
    _ud_chain    = cand.Ud_chain();
    _kid_sts     = cand.Status();
  }

  void Add_ud_chain(uint32_t kid_idx, NODE_STACK& stack) {
    NODE_STACK& kid_ud = _ud_chain[kid_idx];
    kid_ud.assign(stack.begin(), stack.end());
  }

  void Set_status(uint32_t idx, KID_DEF_STATUS sts) {
    AIR_ASSERT(idx < _kid_sts.size());
    _kid_sts[idx] = sts;
  }
  KID_DEF_STATUS Status(uint32_t idx) {
    AIR_ASSERT(idx < _kid_sts.size());
    return _kid_sts[idx];
  }

  std::vector<KID_DEF_STATUS>& Status(void) { return _kid_sts; }

  const air::opt::NODE_INFO& Root_parent(void) const { return _root_parent; }
  air::opt::HEXPR_PTR        Root(void) const { return _root; }

  const std::vector<NODE_STACK>& Ud_chain(void) const { return _ud_chain; }

  bool Is_valid();

  void Print(std::ostream& os) const;

private:
  air::opt::HEXPR_PTR         _root;
  air::opt::NODE_INFO         _root_parent;
  std::vector<NODE_STACK>     _ud_chain;
  std::vector<KID_DEF_STATUS> _kid_sts;
};

}  // namespace poly
}  // namespace fhe

#endif  // FHE_OPT_MDOWN_HOIST_OPT_H
