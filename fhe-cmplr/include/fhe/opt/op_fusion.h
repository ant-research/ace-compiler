//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_OPT_OP_FUSION_H
#define FHE_OPT_OP_FUSION_H

#include "air/base/container.h"
#include "air/opt/hssa_expr.h"
#include "air/opt/hssa_func.h"
#include "air/opt/hssa_stmt.h"
#include "fhe/core/lower_ctx.h"
#include "fhe/poly/config.h"

namespace fhe {

namespace poly {
class OP_FUSION_RULE;
using OP_FUSION_MEM_POOL = air::util::MEM_POOL<4096>;
using OPLIST             = std::vector<air::base::OPCODE>;
using RULE_ALLOC =
    air::util::CXX_MEM_ALLOCATOR<OP_FUSION_RULE, OP_FUSION_MEM_POOL>;
using RULE_VEC = std::vector<OP_FUSION_RULE, RULE_ALLOC>;

class OP_FUSION_OPT {
public:
  OP_FUSION_OPT(POLY_CONFIG& config, air::driver::DRIVER_CTX* ctx,
                core::LOWER_CTX* lower_ctx)
      : _config(config),
        _driver_ctx(ctx),
        _lower_ctx(lower_ctx),
        _cur_func(nullptr),
        _rules(RULE_ALLOC(&_mpool)),
        _cand_idx(0),
        _opt_cnt(0) {}

  uint32_t             Cand_idx(void) const { return _cand_idx; }
  void                 Inc_cand_idx(void) { _cand_idx++; }
  uint32_t             Opt_cnt(void) const { return _opt_cnt; }
  void                 Inc_opt_cnt(void) { _opt_cnt++; }
  air::opt::HSSA_FUNC* Cur_func() const { return _cur_func; }
  void Set_cur_func(air::opt::HSSA_FUNC* func) { _cur_func = func; }
  const core::LOWER_CTX* Lower_ctx() const { return _lower_ctx; }

  air::opt::HCONTAINER* Hssa_cont() const { return Cur_func()->Hssa_cont(); }
  air::opt::CFG&        Cfg() { return Cur_func()->Cfg(); }

  DECLARE_TRACE_DETAIL_API(_config, _driver_ctx)

  void Run(air::opt::HSSA_FUNC* hssa_func);

  bool Fuse_rule(OP_FUSION_RULE& cand, air::opt::HEXPR_PTR expr,
                 const air::opt::NODE_INFO& parent);

  RULE_VEC& Rules(void) { return _rules; }
  void      Register_rules(uint32_t priority, air::base::OPCODE fused_op,
                           OPLIST& list);

  void Add_dead_stmt(air::opt::HSTMT_PTR stmt) {
    if (!stmt->Is_dead()) {
      _dead_stmts.push_back(stmt);
      stmt->Set_dead();
    }
  }

  void Remove_dead_stmts();

  uint64_t            Prec_key(air::opt::HEXPR_PTR encode_var);
  air::opt::HEXPR_PTR New_prec_var(air::opt::HEXPR_PTR encode_var);
  air::opt::HEXPR_PTR Prec_var(air::opt::HEXPR_PTR encode_var);
  air::opt::HEXPR_PTR Get_prec_var(air::opt::HEXPR_PTR encode_var);

private:
  OP_FUSION_MEM_POOL               _mpool;
  POLY_CONFIG&                     _config;
  air::driver::DRIVER_CTX*         _driver_ctx;
  air::opt::HSSA_FUNC*             _cur_func;
  core::LOWER_CTX*                 _lower_ctx;
  uint32_t                         _cand_idx;
  uint32_t                         _opt_cnt;
  RULE_VEC                         _rules;
  std::vector<air::opt::HSTMT_PTR> _dead_stmts;
  std::map<uint64_t, air::opt::HEXPR_PTR>
      _prec_sym_map;  // [HEXPR_ID_VALUE, HEXPR_PTR]
};

class OP_FUSION_RULE {
public:
  OP_FUSION_RULE(uint32_t priority, air::base::OPCODE fused_op, OPLIST& lists)
      : _priority(priority),
        _fused_op(fused_op),
        _op_seq(lists.begin(), lists.end()) {}

  air::base::OPCODE Start_op(void) {
    AIR_ASSERT(_op_seq.size() > 1);
    return _op_seq[0];
  }

  air::base::OPCODE Opcode(void) const { return _fused_op; }

  air::base::OPCODE Op(uint32_t idx) const {
    AIR_ASSERT(idx < Op_cnt());
    return _op_seq[idx];
  }

  uint32_t Op_cnt(void) const { return _op_seq.size(); }
  uint32_t Priority(void) const { return _priority; }

  static bool Less(const OP_FUSION_RULE& c1, const OP_FUSION_RULE& c2);

  void Print(std::ostream& os) const;

private:
  uint32_t          _priority;
  OPLIST            _op_seq;
  air::base::OPCODE _fused_op;
};

}  // namespace poly
}  // namespace fhe
#endif
