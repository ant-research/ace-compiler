//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================
#ifndef AIR_OPT_SSAPRE_H
#define AIR_OPT_SSAPRE_H

#include <set>
#include <stack>

#include "air/driver/driver_ctx.h"
#include "air/opt/pre_container.h"
#include "air/util/mem_pool.h"

namespace air {

namespace opt {
enum PREKIND { EPRE_K, LPRE_K, SPRE_K };
class HCONTAINER;

typedef air::util::MEM_POOL<4096>                      MEM_POOL;
typedef air::util::CXX_MEM_ALLOCATOR<OCC_ID, MEM_POOL> ALL_OCC_ALLOC;
typedef std::vector<OCC_ID, ALL_OCC_ALLOC>             ALL_OCC_LIST;
typedef std::set<OCC_PTR, OCC::LESS>                   OCC_PTR_SET;

enum SSAPRE_TRACE_DETAIL {
  TRACE_IR_BEFORE_PRE = 0,
  TRACE_IR_AFTER_PRE  = 1,
  TRACE_PRE_FLOW      = 2,
};

//! @brief Config for building SSA
class SSAPRE_CONFIG {
public:
  SSAPRE_CONFIG(uint32_t td) : _trace_detail(td) {}
  ~SSAPRE_CONFIG() {}

  void Set_trace_ir_before_pre(bool val) {
    _trace_detail |= (((uint32_t)val) << TRACE_IR_BEFORE_PRE);
  }
  void Set_trace_ir_after_pre(bool val) {
    _trace_detail |= (((uint32_t)val) << TRACE_IR_AFTER_PRE);
  }
  void Set_trace_pre_flow(bool val) {
    _trace_detail |= (((uint32_t)val) << TRACE_PRE_FLOW);
  }
  bool Is_trace(uint32_t flag) const {
    return (_trace_detail & (1U << flag)) != 0;
  }
  void Add_cand_op(air::base::OPCODE opcode) { _cand_ops.insert(opcode); }
  bool Is_cand_op(air::base::OPCODE opcode) {
    if (_cand_ops.find(opcode) != _cand_ops.end()) return true;
    return false;
  }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  SSAPRE_CONFIG(void);
  SSAPRE_CONFIG(const SSAPRE_CONFIG&);
  SSAPRE_CONFIG operator=(const SSAPRE_CONFIG&);

  uint32_t                    _trace_detail = 0;
  std::set<air::base::OPCODE> _cand_ops;
};

class SSAPRE {
public:
  SSAPRE(PREKIND k, CFG& cfg, const driver::DRIVER_CTX* driver_ctx)
      : _kind(k),
        _pre_cont(cfg),
        _pre_cand(air::base::Null_ptr),
        _cur_tmp(air::base::Null_ptr),
        _cur_ver(0),
        _cur_all_occs(ALL_OCC_ALLOC(&_mpool)),
        _var_phi_set(std::less<BB_ID>(), BBID_ALLOC(&_mpool)),
        _df_phi_set(std::less<BB_ID>(), BBID_ALLOC(&_mpool)),
        _driver_ctx(driver_ctx),
        _config(0) {}

  void Run();

  SSAPRE_CONFIG& Pre_config(void) { return _config; }

  DECLARE_TRACE_DETAIL_API(_config, _driver_ctx)

private:
  HCONTAINER&    Hssa_cont() const { return _pre_cont.Hssa_cont(); }
  CFG&           Cfg() const { return _pre_cont.Cfg(); }
  DOM_INFO&      Dom_info() const { return Cfg().Dom_info(); }
  PRE_CONTAINER& Pre_cont() { return _pre_cont; }
  PRE_CAND_PTR   Cur_cand() const { return _pre_cand; }
  uint32_t       Cur_ver() const { return _cur_ver; }
  HEXPR_PTR      Cur_tmp() const { return _cur_tmp; }
  void           Set_cur_cand(PRE_CAND_PTR cand) { _pre_cand = cand; }
  void           Set_cur_tmp(HEXPR_PTR tmp) { _cur_tmp = tmp; }
  void           Inc_cur_ver(void) { _cur_ver++; }
  void           Init_cur_ver(void) { _cur_ver = 0; }
  BBID_CSET&  Var_phi_set() { return _var_phi_set; }
  BBID_CSET&  Df_phi_set() { return _df_phi_set; }
  ALL_OCC_LIST&  Cur_all_occs() { return _cur_all_occs; }
  void Append_all_occ(OCC_PTR occ) { _cur_all_occs.push_back(occ->Id()); }

  void Create_worklist();
  void Insert_phis();
  void Rename();
  void Compute_down_safty();
  void Compute_canbe_avail();
  void Compute_later();
  void Finalize();

  void Init_cand(PRE_CAND_PTR cand);
  bool Is_skip_cand(PRE_CAND_PTR cand);

  // Step 2: Insert phi APIs
  void Get_domfrontier(BB_PTR bb, BBID_CSET& bb_set);
  void Collect_phi_sets();
  void Create_phi_occs();
  void Create_phi_opnd_occs();
  void Gen_var_phi_list(HEXPR_PTR expr);
  void Build_all_occs();

  // Step 3: Rename APIs
  void      Rename_for_real_occ(OCC_PTR& occ, std::stack<OCC_PTR>& occ_stack,
                                OCC_PTR_SET& pending_occs);
  void      Rename_for_phi_opnd(OCC_PTR& occ, OCC_PTR real_occ,
                                OCC_PTR_SET& pending_occs, uint32_t phi_idx);
  bool      Is_same_ver_for_real_def(OCC_PTR occ1, OCC_PTR occ2);
  bool      Is_same_ver_for_phi_def(OCC_PTR def, OCC_PTR use);
  bool      Is_expr_modify_phi_res(HEXPR_PTR expr, BB_PTR phi_bb);
  bool      Need_fixing_phi(OCC_PTR def, OCC_PTR use);
  void      Create_new_version(OCC_PTR& occ, std::stack<OCC_PTR>& occ_stack);
  HEXPR_PTR Phi_opnd_with_cur_ver(OCC_PTR occ, uint32_t phi_idx);
  HEXPR_PTR Get_cur_ver(HEXPR_PTR expr, OCC_PTR phi_occ, uint32_t phi_idx);

  // Step 5: Finalize APIs
  bool      Need_insert(PHI_OPND_OCC_DATA_PTR phi_opnd);
  void      Compute_save_reload_inserts();
  bool      Replace_occurs(OCC_PTR occ, HEXPR_ID saved_expr);
  HEXPR_PTR Get_or_new_temp_var_exp(HEXPR_PTR expr);
  void      Gen_reload_for_realocc(OCC_PTR occ);
  void      Gen_save_for_occ(OCC_PTR occ);
  void      Gen_save_reload();

  static void Print_all_occs(std::ostream& os, PRE_CONTAINER& cont,
                             ALL_OCC_LIST& occ_list);
  typedef air::util::MEM_POOL<4096> MEM_POOL;

  PREKIND       _kind;
  PRE_CONTAINER _pre_cont;
  MEM_POOL      _mpool;     //!< Mempool for all SSAPRE
  PRE_CAND_PTR  _pre_cand;  // current candidate
  HEXPR_PTR     _cur_tmp;   // current tmp var expr, init for each pre candidate
  uint32_t      _cur_ver;   // current tmp version, init for each pre candidate
  ALL_OCC_LIST  _cur_all_occs;  // current all occs
  BBID_CSET  _var_phi_set;   // phi inserted by var UD
  BBID_CSET  _df_phi_set;    // phi  inserted by dominance frontiers
  const driver::DRIVER_CTX* _driver_ctx;
  SSAPRE_CONFIG             _config;
};

}  // namespace opt
}  // namespace air
#endif
