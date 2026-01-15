//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CORE_CTX_PARAM_ANA_H
#define FHE_CORE_CTX_PARAM_ANA_H

#include <algorithm>
#include <list>
#include <ostream>

#include "air/base/analyze_ctx.h"
#include "air/base/container.h"
#include "air/base/container_decl.h"
#include "air/base/id_wrapper.h"
#include "air/base/st.h"
#include "air/base/st_decl.h"
#include "air/base/visitor.h"
#include "air/core/handler.h"
#include "air/core/null_handler.h"
#include "air/driver/driver_ctx.h"
#include "air/opt/ssa_container.h"
#include "air/opt/ssa_decl.h"
#include "air/opt/ssa_node_list.h"
#include "air/util/debug.h"
#include "air/util/error.h"
#include "fhe/ckks/ckks_gen.h"
#include "fhe/ckks/ckks_handler.h"
#include "fhe/ckks/config.h"
#include "fhe/ckks/null_handler.h"
#include "fhe/core/lower_ctx.h"
#include "nn/core/attr.h"

namespace fhe {
namespace core {

using namespace air::base;

// TODO: mv IV_INFO and related APIs into air_infra
class IV_INFO {
public:
  IV_INFO(SYM_ID sym_id, int64_t init, int64_t stride, int64_t itr_cnt)
      : _iv_sym_id(sym_id),
        _init_val(init),
        _stride(stride),
        _itr_cnt(itr_cnt) {}

  SYM_ID  Get_iv_sym_id() const { return _iv_sym_id; }
  int64_t Get_init_val() const { return _init_val; }
  int64_t Get_stride() const { return _stride; }
  int64_t Get_itr_cnt() const { return _itr_cnt; }

private:
  /**
   * @brief SYM_ID of iv
   *
   */
  SYM_ID _iv_sym_id;

  /**
   * @brief init value of iv
   *
   */
  int64_t _init_val;

  /**
   * @brief stride of iv
   *
   */
  int64_t _stride;

  /**
   * @brief iteration number of loop
   *
   */
  int64_t _itr_cnt;
};

//! @brief return value of handler APIs of CTX_PARAM analyzer
class ANA_RETV {
public:
  ANA_RETV(void) : _mul_level_inc(false), _mul_level(0) {}
  ANA_RETV(bool level_inc, uint32_t mul_level)
      : _mul_level_inc(level_inc), _mul_level(mul_level) {}
  ~ANA_RETV() {}

  void     Set_mul_level_inc(bool val) { _mul_level_inc = val; }
  bool     Mul_level_inc() const { return _mul_level_inc; }
  uint32_t Mul_level() const { return _mul_level; }

private:
  bool _mul_level_inc;  // true if mul_level of node increased, otherwise false.
  uint32_t _mul_level;  // mul_level of handled node
};

//! @brief context of CTX_PARAM analyzer.
class CTX_PARAM_ANA_CTX : public ANALYZE_CTX {
public:
  using ROTATE_IDX_SET = std::set<int32_t>;
  // key: id of addr_datum/preg; val: mul_level of addr_datum/preg
  using MUL_LEVEL_MAP = std::map<uint32_t, uint32_t>;
  using IV_INFO_STACK = std::list<IV_INFO>;
  using MUL_LEV_STACK = std::list<uint32_t>;
  using SSA_CONTAINER = air::opt::SSA_CONTAINER;
  using SSA_VER_ID    = air::opt::SSA_VER_ID;
  using VER_SET       = std::set<SSA_VER_ID>;

  CTX_PARAM_ANA_CTX(const air::driver::DRIVER_CTX* driver_ctx,
                    const ckks::CKKS_CONFIG* config, LOWER_CTX* ctx,
                    FUNC_SCOPE* func_scope, SSA_CONTAINER* ssa_cntr,
                    uint32_t formal_mul_lev)
      : _lower_ctx(ctx),
        _func_scope(func_scope),
        _ssa_cntr(ssa_cntr),
        _driver_ctx(driver_ctx),
        _config(config) {}
  ~CTX_PARAM_ANA_CTX() {}

  uint32_t Get_mul_level_of_ssa_ver(SSA_VER_ID id);
  bool     Update_mul_level_of_ssa_ver(SSA_VER_ID id, uint32_t new_level);

  void                 Push_iv_info(IV_INFO iv) { _iv_info.push_back(iv); }
  void                 Pop_iv_info() { _iv_info.pop_back(); }
  const IV_INFO_STACK& Get_iv_info() const { return _iv_info; }

  void     Push_mul_level(uint32_t val) { _mul_lev_stack.push_back(val); }
  void     Pop_mul_level() { _mul_lev_stack.pop_back(); }
  uint32_t Top_mul_level() { return _mul_lev_stack.back(); }
  const MUL_LEV_STACK& Get_mul_lev_stack() const { return _mul_lev_stack; }

  void Update_mul_level(uint32_t val) {
    _func_mul_level = std::max(_func_mul_level, val);
  }

  uint32_t              Get_mul_level() const { return _func_mul_level; }
  void                  Add_rotate_index(int32_t idx) { _rot_idx.insert(idx); }
  const ROTATE_IDX_SET& Get_rotate_index() const { return _rot_idx; }
  //! set mul_level attr for node result
  void Set_node_mul_level(NODE_PTR node, uint32_t mul_level) const {
    node->Set_attr(core::FHE_ATTR_KIND::LEVEL, &mul_level, 1);
  }

  uint32_t Input_level() {
    if (Func_scope()->Owning_func()->Entry_point()->Is_program_entry()) {
      uint32_t input_lev_config = _config->Input_cipher_lvl();
      return input_lev_config != 0 ? input_lev_config : Get_mul_level();
    }
    return 0;
  }

  //! @brief Set mul_level of SSA_VER ver is undetermined.
  void           Record_undeterm(SSA_VER_ID ver) { _undeterm_ver.insert(ver); }
  const VER_SET& Undeterm_ssa_ver(void) const { return _undeterm_ver; }

  //! @brief Clear recorded SSA_VERs with undetermined mul_level.
  void Clear_undeterm_ssa_ver(void) { _undeterm_ver.clear(); }
  //! @brief Check if the loop needs be traversed again. Return true if any
  //! SSA_VER has an undetermined mul_level; otherwise, return false.
  bool Trav_loop_again(void) const { return !_undeterm_ver.empty(); }

  STMT_PTR Gen_modswitch(air::opt::SSA_SYM_PTR sym, uint32_t mul_lev,
                         uint32_t in_lev, SPOS spos);

  LOWER_CTX*     Lower_ctx() const { return _lower_ctx; }
  FUNC_SCOPE*    Func_scope() const { return _func_scope; }
  SSA_CONTAINER* Ssa_cntr() const { return _ssa_cntr; }
  void           Print(std::ostream& out) const;

  DECLARE_TRACE_DETAIL_API((*_config), _driver_ctx)
  DECLARE_CKKS_OPTION_CONFIG_ACCESS_API((*_config))

  constexpr static uint32_t INVALID_LVL = 0;

private:
  // REQUIRED UNDEFINED UNWANTED methods
  CTX_PARAM_ANA_CTX(void);
  CTX_PARAM_ANA_CTX(const CTX_PARAM_ANA_CTX&);
  CTX_PARAM_ANA_CTX& operator=(const CTX_PARAM_ANA_CTX&);

  const MUL_LEVEL_MAP& Get_mul_level_of_ssa_ver() const {
    return _mul_level_of_ssa_ver;
  }
  MUL_LEVEL_MAP& Get_mul_level_of_ssa_ver() { return _mul_level_of_ssa_ver; }

  LOWER_CTX*     _lower_ctx;
  MUL_LEVEL_MAP  _mul_level_of_ssa_ver;
  IV_INFO_STACK  _iv_info;  // iv info of nested loops. top is iv of inner loop
  MUL_LEV_STACK  _mul_lev_stack;  // stack records mul_level of parent node
  ROTATE_IDX_SET _rot_idx;        // rotate index of current function
  VER_SET        _undeterm_ver;   // SSA_VERs with undetermined mul_level
  FUNC_SCOPE*    _func_scope;     // current function scope
  SSA_CONTAINER* _ssa_cntr;
  const ckks::CKKS_CONFIG*       _config;
  const air::driver::DRIVER_CTX* _driver_ctx;
  uint32_t _func_mul_level = 0;  // mul_level of current function
};

//! @brief impl of CORE IR handler
class CORE_ANA_IMPL : public air::core::NULL_HANDLER {
public:
  using SSA_CONTAINER = air::opt::SSA_CONTAINER;
  using SSA_VER_ID    = air::opt::SSA_VER_ID;
  using PHI_NODE_PTR  = air::opt::PHI_NODE_PTR;
  using CHI_NODE_PTR  = air::opt::CHI_NODE_PTR;
  using MU_NODE_PTR   = air::opt::MU_NODE_PTR;

  CORE_ANA_IMPL(void) {}
  ~CORE_ANA_IMPL() {}

  template <typename RETV, typename VISITOR>
  RETV Handle_func_entry(VISITOR* visitor, NODE_PTR entry_node);
  template <typename RETV, typename VISITOR>
  RETV Handle_block(VISITOR* visitor, NODE_PTR block_node);
  template <typename RETV, typename VISITOR>
  RETV Handle_do_loop(VISITOR* visitor, NODE_PTR do_loop);
  template <typename RETV, typename VISITOR>
  RETV Handle_idname(VISITOR* visitor, air::base::NODE_PTR node);
  template <typename RETV, typename VISITOR>
  RETV Handle_ld(VISITOR* visitor, NODE_PTR ld_node);
  template <typename RETV, typename VISITOR>
  RETV Handle_ild(VISITOR* visitor, NODE_PTR ild_node);
  template <typename RETV, typename VISITOR>
  RETV Handle_ldp(VISITOR* visitor, NODE_PTR ldp_node);
  template <typename RETV, typename VISITOR>
  RETV Handle_stp(VISITOR* visitor, NODE_PTR stp_node);
  template <typename RETV, typename VISITOR>
  RETV Handle_st(VISITOR* visitor, NODE_PTR st_node);
  template <typename RETV, typename VISITOR>
  RETV Handle_ist(VISITOR* visitor, NODE_PTR ist_node);
  template <typename RETV, typename VISITOR>
  RETV Handle_call(VISITOR* visitor, NODE_PTR call_node);
  template <typename RETV, typename VISITOR>
  RETV Handle_retv(VISITOR* visitor, NODE_PTR retv_node);
  template <typename RETV, typename VISITOR>
  RETV Handle_phi_list(VISITOR* visitor, NODE_PTR node, uint32_t opnd_id);

private:
  // REQUIRED UNDEFINED UNWANTED methods
  CORE_ANA_IMPL(const CORE_ANA_IMPL&);
  CORE_ANA_IMPL& operator=(const CORE_ANA_IMPL&);

  int64_t Get_init_val_of_iv(NODE_PTR do_loop);
  int64_t Get_strid_of_iv(NODE_PTR do_loop);
  int64_t Get_bound_of_iv(NODE_PTR do_loop);
  int64_t Get_itr_cnt(air::base::OPCODE cmp_op, int64_t init, int64_t stride,
                      int64_t bound);
  IV_INFO Get_loop_iv_info(NODE_PTR loop_node);

  void Fixup_phi_res(CTX_PARAM_ANA_CTX& ctx, NODE_PTR node);
};

template <typename RETV, typename VISITOR>
RETV CORE_ANA_IMPL::Handle_func_entry(VISITOR* visitor, NODE_PTR node) {
  ANALYZE_CTX::GUARD guard(visitor->Context(), node);

  AIR_ASSERT(node->Num_child() > 0);
  visitor->template Visit<RETV>(node->Last_child());

  CTX_PARAM_ANA_CTX& ana_ctx = visitor->Context();

  ana_ctx.Trace(ckks::TRACE_DETAIL::TD_CKKS_LEVEL_MGT,
                "func_entry: l=", ana_ctx.Get_mul_level(),
                " input=", ana_ctx.Input_level(), "\n");

  air::opt::CHI_NODE_ID chi = ana_ctx.Ssa_cntr()->Node_chi(node->Id());
  if (chi != air::base::Null_id) {
    auto visit = [](air::opt::CHI_NODE_PTR chi, CTX_PARAM_ANA_CTX& ctx,
                    ckks::CKKS_GEN& ckks_gen, SPOS spos) {
      air::opt::SSA_VER_ID ver_id    = chi->Result_id();
      uint32_t             mul_level = ctx.Get_mul_level_of_ssa_ver(ver_id);
      uint32_t             opnd_lev  = ctx.Input_level();

      ctx.Trace(ckks::TRACE_DETAIL::TD_CKKS_LEVEL_MGT,
                "func_entry: ", ctx.Ssa_cntr()->Ver(ver_id)->To_str(),
                " l=", mul_level, "\n");
      bool is_entry_func = ctx.Ssa_cntr()
                               ->Func_scope()
                               ->Owning_func()
                               ->Entry_point()
                               ->Is_program_entry();
      if (!is_entry_func) return;
      if (opnd_lev > 0 && mul_level > opnd_lev) {
        AIR_ASSERT_MSG(false, "Input cipher level must be at least %u\n",
                       mul_level);
      }

      ctx.Lower_ctx()->Get_ctx_param().Set_input_level(mul_level);
      ctx.Trace(ckks::TD_CKKS_LEVEL_MGT, "Set input cipher level as ",
                mul_level);
    };
    ckks::CKKS_GEN     ckks_gen(node->Container(), ana_ctx.Lower_ctx());
    air::opt::CHI_LIST list(ana_ctx.Ssa_cntr(), chi);
    list.For_each(visit, ana_ctx, ckks_gen, node->Spos());
  }
  // traverse idname after function body
  for (uint32_t id = 0; id < node->Num_child() - 1; ++id) {
    air::base::NODE_PTR child = node->Child(id);
    visitor->template Visit<RETV>(child);
  }

  return RETV(false, ana_ctx.Get_mul_level());
}

template <typename RETV, typename VISITOR>
RETV CORE_ANA_IMPL::Handle_phi_list(VISITOR* visitor, NODE_PTR node,
                                    uint32_t opnd_id) {
  CTX_PARAM_ANA_CTX& ana_ctx  = visitor->Context();
  SSA_CONTAINER*     ssa_cntr = ana_ctx.Ssa_cntr();
  if (!ssa_cntr->Has_phi(node)) return RETV(false, 0);

  ana_ctx.Trace(ckks::TRACE_DETAIL::TD_CKKS_LEVEL_MGT,
                "phi: l=", ana_ctx.Get_mul_level(),
                opnd_id == air::opt::BACK_EDGE_PHI_OPND_ID ? "(back)\n"
                                                           : "(preheader)\n");

  air::opt::PHI_NODE_ID phi_id = ssa_cntr->Node_phi(node->Id());
  air::opt::PHI_LIST    phi_list(ssa_cntr, phi_id);
  auto phi_handler = [](PHI_NODE_PTR phi, CTX_PARAM_ANA_CTX& ana_ctx,
                        uint32_t opnd_id, RETV& res) {
    // 1. get mul_level of phi result
    SSA_VER_ID res_ver_id  = phi->Result_id();
    uint32_t   res_mul_lev = ana_ctx.Get_mul_level_of_ssa_ver(res_ver_id);

    // 2. update mul_level of phi opnd
    SSA_VER_ID opnd_ver_id = phi->Opnd_id(opnd_id);
    bool       mul_level_changed =
        ana_ctx.Update_mul_level_of_ssa_ver(opnd_ver_id, res_mul_lev);
    if (mul_level_changed) {
      res.Set_mul_level_inc(true);
    }

    ana_ctx.Trace(ckks::TRACE_DETAIL::TD_CKKS_LEVEL_MGT, "  ", phi->To_str(),
                  " ", ana_ctx.Ssa_cntr()->Ver(opnd_ver_id)->To_str(),
                  " l=", res_mul_lev, mul_level_changed ? " changed\n" : "\n");
  };

  RETV res(false, 0);
  phi_list.For_each(phi_handler, ana_ctx, opnd_id, res);
  return res;
}

//! Handle_block traverse stmts in reverse order.
template <typename RETV, typename VISITOR>
RETV CORE_ANA_IMPL::Handle_block(VISITOR* visitor, NODE_PTR block_node) {
  std::list<STMT_PTR> reverse_stmt_list;
  for (air::base::STMT_PTR stmt             = block_node->Begin_stmt();
       stmt != block_node->End_stmt(); stmt = stmt->Next()) {
    reverse_stmt_list.push_front(stmt);
  }

  bool mul_level_inc = false;
  for (STMT_PTR stmt : reverse_stmt_list) {
    mul_level_inc |=
        visitor->template Visit<RETV>(stmt->Node()).Mul_level_inc();
  }
  return RETV(mul_level_inc, 0);
}

template <typename RETV, typename VISITOR>
RETV CORE_ANA_IMPL::Handle_do_loop(VISITOR* visitor, NODE_PTR do_loop) {
  CTX_PARAM_ANA_CTX& ana_ctx = visitor->Context();
  // get iv info of do_loop, and push onto iv info stack of ANA_CTX
  const IV_INFO& cur_loop_iv_info = Get_loop_iv_info(do_loop);
  ana_ctx.Push_iv_info(cur_loop_iv_info);

  // fixup phi result level for whole array and array element before checking
  // operands
  Fixup_phi_res(ana_ctx, do_loop);

  uint32_t undeterm_ssa_ver_cnt = UINT32_MAX;
  if (ana_ctx.Loop_level() == 1) {
    ana_ctx.Clear_undeterm_ssa_ver();
  }
  bool trav_loop = true;
  while (trav_loop) {
    //  1. handle phi opnd from back edge
    Handle_phi_list<RETV>(visitor, do_loop, air::opt::BACK_EDGE_PHI_OPND_ID);

    // 2. handle loop body
    constexpr uint32_t LOOP_BODY_ID = 3;
    NODE_PTR           loop_body    = do_loop->Child(LOOP_BODY_ID);
    AIR_ASSERT(loop_body->Is_block());
    Handle_block<RETV>(visitor, loop_body);

    // 3. handle phi opnd defined before do_loop
    Handle_phi_list<RETV>(visitor, do_loop, air::opt::PREHEADER_PHI_OPND_ID);
    if ((ana_ctx.Loop_level() == 1) && ana_ctx.Trav_loop_again()) {
      trav_loop = true;
      // trace SSA_VERs with undetermined mul_level.
      const CTX_PARAM_ANA_CTX::VER_SET& undeterm_ssa_ver =
          ana_ctx.Undeterm_ssa_ver();
      AIR_ASSERT_MSG(undeterm_ssa_ver.size() < undeterm_ssa_ver_cnt,
                     "traverse the loop body without updating the mul_level of "
                     "any SSA_VER.");
      undeterm_ssa_ver_cnt = undeterm_ssa_ver.size();
      ana_ctx.Trace(ckks::TRACE_DETAIL::TD_CKKS_LEVEL_MGT, undeterm_ssa_ver_cnt,
                    " SSA_VERs need update mul_level:\n");
      for (SSA_VER_ID ver_id : undeterm_ssa_ver) {
        ana_ctx.Trace(ckks::TRACE_DETAIL::TD_CKKS_LEVEL_MGT, "    ",
                      ana_ctx.Ssa_cntr()->Ver(ver_id)->To_str(), "\n");
      }
      // clear SSA_VERs with undetermined mul_level for next iteration.
      ana_ctx.Clear_undeterm_ssa_ver();
    } else {
      trav_loop = false;
    }
  }

  ana_ctx.Pop_iv_info();
  return RETV();
}

template <typename RETV, typename VISITOR>
RETV CORE_ANA_IMPL::Handle_idname(VISITOR*            visitor,
                                  air::base::NODE_PTR formal) {
  CTX_PARAM_ANA_CTX& ana_ctx  = visitor->Context();
  SSA_CONTAINER*     ssa_cntr = ana_ctx.Ssa_cntr();
  SSA_VER_ID         ver_id   = ssa_cntr->Node_ver_id(formal->Id());

  LOWER_CTX* lower_ctx = ana_ctx.Lower_ctx();
  TYPE_ID    type = formal->Addr_datum()->Type_id();
  if (!lower_ctx->Is_cipher3_type(type) &&
      !lower_ctx->Is_cipher_type(type) &&
      !lower_ctx->Is_plain_type(type)) {
    ana_ctx.Trace(ckks::TRACE_DETAIL::TD_CKKS_LEVEL_MGT,
          std::string(ana_ctx.Indent(), '+'),
          "skip non-CIPHER/PLAIN idname: ", ssa_cntr->Ver(ver_id)->To_str(), "\n");
    return RETV{false, 0};
  }

  uint32_t mul_level     = ana_ctx.Get_mul_level_of_ssa_ver(ver_id);
  uint32_t opnd_lev      = ana_ctx.Input_level();
  bool     is_entry_func = ana_ctx.Ssa_cntr()
                           ->Func_scope()
                           ->Owning_func()
                           ->Entry_point()
                           ->Is_program_entry();
  if (is_entry_func && ana_ctx.Input_cipher_lvl() > 0 && mul_level > opnd_lev) {
    AIR_ASSERT_MSG(false, "Input cipher level must be at least %u\n",
                   mul_level);
  }

  ana_ctx.Trace(ckks::TRACE_DETAIL::TD_CKKS_LEVEL_MGT,
                std::string(ana_ctx.Indent(), '+'),
                "idname: ", ssa_cntr->Ver(ver_id)->To_str(), " l=", mul_level,
                " input=", opnd_lev, "\n");

  ana_ctx.Set_node_mul_level(formal, mul_level);
  if (mul_level == opnd_lev || opnd_lev == 1) {
    return RETV{false, mul_level};
  }
  return RETV{false, mul_level};
}

template <typename RETV, typename VISITOR>
RETV CORE_ANA_IMPL::Handle_ld(VISITOR* visitor, NODE_PTR ld_node) {
  // return zero if loaded symbol is not cipher
  TYPE_ID            preg_type_id = ld_node->Rtype_id();
  CTX_PARAM_ANA_CTX& ana_ctx      = visitor->Context();
  const LOWER_CTX*   lower_ctx    = ana_ctx.Lower_ctx();
  if (!lower_ctx->Is_cipher_type(preg_type_id) &&
      !lower_ctx->Is_cipher3_type(preg_type_id)) {
    return RETV{false, 0};
  }

  air::opt::SSA_VER_PTR ver = ana_ctx.Ssa_cntr()->Node_ver(ld_node->Id());
  uint32_t              ver_mul_level = ana_ctx.Top_mul_level();
  if (ver_mul_level == CTX_PARAM_ANA_CTX::INVALID_LVL) {
    ana_ctx.Record_undeterm(ver->Id());
    return RETV();
  }
  bool mul_level_changed =
      ana_ctx.Update_mul_level_of_ssa_ver(ver->Id(), ver_mul_level);
  ana_ctx.Set_node_mul_level(ld_node, ver_mul_level);

  ana_ctx.Trace(ckks::TRACE_DETAIL::TD_CKKS_LEVEL_MGT,
                std::string(ana_ctx.Indent(), '+'), "ld: ", ver->To_str(),
                " l=", ver_mul_level, mul_level_changed ? " changed\n" : "\n");

  return RETV{mul_level_changed, ver_mul_level};
}

template <typename RETV, typename VISITOR>
RETV CORE_ANA_IMPL::Handle_ldp(VISITOR* visitor, NODE_PTR ldp_node) {
  // return zero if loaded preg is not cipher
  TYPE_ID            preg_type_id = ldp_node->Rtype_id();
  CTX_PARAM_ANA_CTX& ana_ctx      = visitor->Context();
  const LOWER_CTX*   lower_ctx    = ana_ctx.Lower_ctx();
  if (!lower_ctx->Is_cipher_type(preg_type_id) &&
      !lower_ctx->Is_cipher3_type(preg_type_id)) {
    return RETV{false, 0};
  }

  air::opt::SSA_VER_PTR ver = ana_ctx.Ssa_cntr()->Node_ver(ldp_node->Id());
  uint32_t              ver_mul_level = ana_ctx.Top_mul_level();
  if (ver_mul_level == CTX_PARAM_ANA_CTX::INVALID_LVL) {
    ana_ctx.Record_undeterm(ver->Id());
    return RETV();
  }
  bool mul_level_changed =
      ana_ctx.Update_mul_level_of_ssa_ver(ver->Id(), ver_mul_level);
  ana_ctx.Set_node_mul_level(ldp_node, ver_mul_level);

  ana_ctx.Trace(ckks::TRACE_DETAIL::TD_CKKS_LEVEL_MGT,
                std::string(ana_ctx.Indent(), '+'), "ldp: ", ver->To_str(),
                " l=", ver_mul_level, mul_level_changed ? " changed\n" : "\n");

  return RETV{mul_level_changed, ver_mul_level};
}

template <typename RETV, typename VISITOR>
RETV CORE_ANA_IMPL::Handle_ild(VISITOR* visitor, NODE_PTR ild_node) {
  // return zero if iloaded value is not cipher
  TYPE_ID            access_type_id = ild_node->Access_type_id();
  CTX_PARAM_ANA_CTX& ana_ctx        = visitor->Context();
  const LOWER_CTX*   lower_ctx      = ana_ctx.Lower_ctx();
  if (!lower_ctx->Is_cipher_type(access_type_id) &&
      !lower_ctx->Is_cipher3_type(access_type_id)) {
    return RETV{false, 0};
  }

  // currently, only support: ild(array (lda, index))
  NODE_PTR addr_child = ild_node->Child(0);
  AIR_ASSERT(addr_child->Opcode() == air::core::OPC_ARRAY);
  AIR_ASSERT(addr_child->Array_base()->Opcode() == air::core::OPC_LDA);
  AIR_ASSERT(addr_child->Array_base()->Addr_datum()->Type()->Is_array());

  air::opt::SSA_VER_PTR ver = ana_ctx.Ssa_cntr()->Node_ver(ild_node->Id());
  uint32_t              ver_mul_level = ana_ctx.Top_mul_level();
  if (ver_mul_level == CTX_PARAM_ANA_CTX::INVALID_LVL) {
    ana_ctx.Record_undeterm(ver->Id());
    return RETV();
  }
  bool mul_level_changed =
      ana_ctx.Update_mul_level_of_ssa_ver(ver->Id(), ver_mul_level);
  ana_ctx.Set_node_mul_level(ild_node, ver_mul_level);

  ana_ctx.Trace(ckks::TRACE_DETAIL::TD_CKKS_LEVEL_MGT,
                std::string(ana_ctx.Indent(), '+'), "ldp: ", ver->To_str(),
                " l=", ver_mul_level, mul_level_changed ? " changed\n" : "\n");

  // return mul_level of accessed ciphertext
  return RETV{mul_level_changed, ver_mul_level};
}

template <typename RETV, typename VISITOR>
RETV CORE_ANA_IMPL::Handle_st(VISITOR* visitor, NODE_PTR st_node) {
  // 1. get mul_level of stored addr_datum version
  CTX_PARAM_ANA_CTX& ana_ctx = visitor->Context();
  SSA_VER_ID         ver_id  = ana_ctx.Ssa_cntr()->Node_ver_id(st_node->Id());
  uint32_t           ver_mul_level = ana_ctx.Get_mul_level_of_ssa_ver(ver_id);

  TYPE_ID          type_id   = st_node->Addr_datum()->Type_id();
  const LOWER_CTX* lower_ctx = ana_ctx.Lower_ctx();
  if (lower_ctx->Is_cipher_type(type_id) ||
      lower_ctx->Is_cipher3_type(type_id)) {
    if (ver_mul_level == CTX_PARAM_ANA_CTX::INVALID_LVL) {
      ana_ctx.Record_undeterm(ver_id);
    } else {
      ana_ctx.Set_node_mul_level(st_node, ver_mul_level);
    }
  }

  ana_ctx.Trace(ckks::TRACE_DETAIL::TD_CKKS_LEVEL_MGT,
                std::string(ana_ctx.Indent(), ' '),
                "st: ", ana_ctx.Ssa_cntr()->Ver(ver_id)->To_str(),
                " l=", ver_mul_level, "\n");

  air::opt::CHI_NODE_ID chi = ana_ctx.Ssa_cntr()->Node_chi(st_node->Id());
  if (chi != air::base::Null_id) {
    auto visit = [](air::opt::CHI_NODE_PTR chi, CTX_PARAM_ANA_CTX& ctx,
                    uint32_t level) {
      ctx.Update_mul_level_of_ssa_ver(chi->Result_id(), level);
      ctx.Update_mul_level_of_ssa_ver(chi->Opnd_id(), level);

      ctx.Trace(
          ckks::TRACE_DETAIL::TD_CKKS_LEVEL_MGT, std::string(ctx.Indent(), ' '),
          "  CHI: (", ctx.Ssa_cntr()->Ver(chi->Result_id())->To_str(), ", ",
          ctx.Ssa_cntr()->Ver(chi->Opnd_id())->To_str(), ") l=", level, "\n");
    };
    air::opt::CHI_LIST list(ana_ctx.Ssa_cntr(), chi);
    list.For_each(visit, ana_ctx, ver_mul_level);
  }

  // 2. handle rhs node
  AIR_ASSERT(ana_ctx.Get_mul_lev_stack().empty());
  ana_ctx.Push_mul_level(ver_mul_level);
  NODE_PTR child = st_node->Child(0);
  RETV     res   = visitor->template Visit<RETV>(child);
  ana_ctx.Pop_mul_level();
  AIR_ASSERT(ana_ctx.Get_mul_lev_stack().empty());
  return res;
}

template <typename RETV, typename VISITOR>
RETV CORE_ANA_IMPL::Handle_stp(VISITOR* visitor, NODE_PTR stp_node) {
  // 1. get mul_level of stored preg version
  CTX_PARAM_ANA_CTX& ana_ctx = visitor->Context();
  SSA_VER_ID         ver_id  = ana_ctx.Ssa_cntr()->Node_ver_id(stp_node->Id());
  uint32_t           ver_mul_level = ana_ctx.Get_mul_level_of_ssa_ver(ver_id);

  TYPE_ID          type_id   = stp_node->Preg()->Type_id();
  const LOWER_CTX* lower_ctx = ana_ctx.Lower_ctx();
  if (lower_ctx->Is_cipher_type(type_id) ||
      lower_ctx->Is_cipher3_type(type_id)) {
    if (ver_mul_level == CTX_PARAM_ANA_CTX::INVALID_LVL) {
      ana_ctx.Record_undeterm(ver_id);
    } else {
      ana_ctx.Set_node_mul_level(stp_node, ver_mul_level);
    }
  }

  ana_ctx.Trace(ckks::TRACE_DETAIL::TD_CKKS_LEVEL_MGT,
                std::string(ana_ctx.Indent(), ' '),
                "stp: ", ana_ctx.Ssa_cntr()->Ver(ver_id)->To_str(),
                " l=", ver_mul_level, "\n");

  // 2. handle rhs node
  // mul_level_stack is a stmt level info
  AIR_ASSERT(ana_ctx.Get_mul_lev_stack().empty());
  ana_ctx.Push_mul_level(ver_mul_level);
  NODE_PTR child = stp_node->Child(0);
  RETV     res   = visitor->template Visit<RETV>(child);
  ana_ctx.Pop_mul_level();
  AIR_ASSERT(ana_ctx.Get_mul_lev_stack().empty());
  return res;
}

template <typename RETV, typename VISITOR>
RETV CORE_ANA_IMPL::Handle_ist(VISITOR* visitor, NODE_PTR ist_node) {
  // 1. get mul_level of istored ciphertext
  CTX_PARAM_ANA_CTX& ana_ctx = visitor->Context();
  AIR_ASSERT(ana_ctx.Ssa_cntr()->Has_chi(ist_node));
  // check type of address node.
  // currently, only support: ist (array(lda, id), ...)
  NODE_PTR addr_child = ist_node->Child(0);
  AIR_ASSERT(addr_child->Opcode() == air::core::OPC_ARRAY);
  AIR_ASSERT(addr_child->Array_base()->Opcode() == air::core::OPC_LDA);
  AIR_ASSERT(addr_child->Array_base()->Addr_datum()->Type()->Is_array());

  SSA_VER_ID res_ver_id    = ana_ctx.Ssa_cntr()->Node_ver_id(ist_node->Id());
  uint32_t   res_mul_level = ana_ctx.Get_mul_level_of_ssa_ver(res_ver_id);
  ana_ctx.Set_node_mul_level(ist_node, res_mul_level);

  ana_ctx.Trace(ckks::TRACE_DETAIL::TD_CKKS_LEVEL_MGT,
                std::string(ana_ctx.Indent(), ' '),
                "ist: ", ana_ctx.Ssa_cntr()->Ver(res_ver_id)->To_str(),
                " l=", res_mul_level, "\n");

  // 2. handle rhs node
  TYPE_ID access_type_id = ist_node->Access_type_id();
  // return zero if istored value is not ciphertext
  const LOWER_CTX* lower_ctx = ana_ctx.Lower_ctx();
  if (!lower_ctx->Is_cipher_type(access_type_id) &&
      !lower_ctx->Is_cipher3_type(access_type_id)) {
    return RETV{false, 0};
  }
  if (res_mul_level == CTX_PARAM_ANA_CTX::INVALID_LVL) {
    ana_ctx.Record_undeterm(res_ver_id);
    return RETV{false, 0};
  }

  AIR_ASSERT(ana_ctx.Get_mul_lev_stack().empty());
  ana_ctx.Push_mul_level(res_mul_level);
  NODE_PTR rhs_child = ist_node->Child(1);
  RETV     res       = visitor->template Visit<RETV>(rhs_child);
  ana_ctx.Pop_mul_level();
  AIR_ASSERT(ana_ctx.Get_mul_lev_stack().empty());
  return res;
}

template <typename RETV, typename VISITOR>
RETV CORE_ANA_IMPL::Handle_call(VISITOR* visitor, NODE_PTR call_node) {
  CTX_PARAM_ANA_CTX& ana_ctx = visitor->Context();
  // check retv type: only support return ciphertext.
  PREG_PTR retv_preg = call_node->Ret_preg();
  AIR_ASSERT_MSG(ana_ctx.Lower_ctx()->Is_cipher_type(retv_preg->Type_id()),
                 "only support function call return ciphertext");

  // 1. get mul_level of retv
  SSA_VER_ID retv_ver_id     = ana_ctx.Ssa_cntr()->Node_ver_id(call_node->Id());
  uint32_t   retv_mul_level  = ana_ctx.Get_mul_level_of_ssa_ver(retv_ver_id);
  TYPE_ID    type_id         = retv_preg->Type_id();
  const LOWER_CTX* lower_ctx = ana_ctx.Lower_ctx();
  if (lower_ctx->Is_cipher_type(type_id) ||
      lower_ctx->Is_cipher3_type(type_id)) {
    if (retv_mul_level == CTX_PARAM_ANA_CTX::INVALID_LVL) {
      ana_ctx.Record_undeterm(retv_ver_id);
    } else {
      ana_ctx.Set_node_mul_level(call_node, retv_mul_level);
    }
  }

  // 2. get mul_level consumed by called function from attribute.
  const uint32_t* mul_depth_ptr =
      call_node->Attr<uint32_t>(FHE_ATTR_KIND::MUL_DEPTH);
  AIR_ASSERT(mul_depth_ptr != nullptr);
  uint32_t func_mul_depth = *mul_depth_ptr;

  // 3. handle actual parameters.
  RETV     ret_res{false, 0};
  uint32_t param_mul_level = retv_mul_level + func_mul_depth;
  AIR_ASSERT(ana_ctx.Get_mul_lev_stack().empty());
  ana_ctx.Push_mul_level(param_mul_level);
  ana_ctx.Update_mul_level(param_mul_level);

  uint32_t child_num = call_node->Num_child();
  AIR_ASSERT(child_num >= 1);
  for (uint32_t id = 0; id < child_num; ++id) {
    NODE_PTR child = call_node->Child(id);
    if (!ana_ctx.Lower_ctx()->Is_cipher_type(child->Rtype_id())) continue;

    // handle actual parameter
    NODE_PTR param = child;
    while (param->Opcode() == ckks::OPC_RESCALE) {
      param = param->Child(0);
    }
    const uint32_t* param_level_ptr =
        param->Attr<uint32_t>(FHE_ATTR_KIND::LEVEL);
    if (param_level_ptr != nullptr) {
      ana_ctx.Push_mul_level(*param_level_ptr);
    }
    RETV res = visitor->template Visit<RETV>(child);
    if (param_level_ptr != nullptr) {
      ana_ctx.Pop_mul_level();
    }

    if (res.Mul_level_inc()) ret_res.Set_mul_level_inc(true);
  }

  ana_ctx.Pop_mul_level();
  AIR_ASSERT(ana_ctx.Get_mul_lev_stack().empty());
  return ret_res;
}

template <typename RETV, typename VISITOR>
RETV CORE_ANA_IMPL::Handle_retv(VISITOR* visitor, NODE_PTR retv_node) {
  NODE_PTR child = retv_node->Child(0);
  AIR_ASSERT(child->Is_ld());
  CTX_PARAM_ANA_CTX& ana_ctx = visitor->Context();
  SSA_VER_ID         ver_id  = ana_ctx.Ssa_cntr()->Node_ver_id(child->Id());
  AIR_ASSERT(ver_id != Null_id);

  // to decrypt correctly, mul_level of retv_node must be <= scale
  const uint32_t* scale_ptr     = child->Attr<uint32_t>(FHE_ATTR_KIND::SCALE);
  uint32_t        level_of_retv = (scale_ptr != nullptr) ? *scale_ptr : 1;
  ana_ctx.Update_mul_level_of_ssa_ver(ver_id, level_of_retv);
  ana_ctx.Update_mul_level(level_of_retv);
  ana_ctx.Set_node_mul_level(child, level_of_retv);

  ana_ctx.Trace(ckks::TRACE_DETAIL::TD_CKKS_LEVEL_MGT,
                std::string(ana_ctx.Indent(), '+'),
                "retv: ", ana_ctx.Ssa_cntr()->Ver(ver_id)->To_str(),
                " l=", level_of_retv, "\n");

  air::opt::MU_NODE_ID mu = ana_ctx.Ssa_cntr()->Node_mu(retv_node->Id());
  if (mu != air::base::Null_id) {
    auto visit = [](air::opt::MU_NODE_PTR mu, CTX_PARAM_ANA_CTX& ctx,
                    uint32_t level) {
      air::opt::SSA_VER_ID ver = mu->Opnd_id();
      ctx.Update_mul_level_of_ssa_ver(ver, level);
      ctx.Trace(
          ckks::TRACE_DETAIL::TD_CKKS_LEVEL_MGT, std::string(ctx.Indent(), ' '),
          "  MU: ", ctx.Ssa_cntr()->Ver(ver)->To_str(), " l=", level, "\n");
    };
    air::opt::MU_LIST list(ana_ctx.Ssa_cntr(), mu);
    list.For_each(visit, ana_ctx, level_of_retv);
  }

  return {false, level_of_retv};
}

//! @brief impl of CKKS IR handler
class CKKS_ANA_IMPL : public fhe::ckks::NULL_HANDLER {
public:
  template <typename RETV, typename VISITOR>
  RETV Handle_mul(VISITOR* visitor, NODE_PTR mul_node);
  template <typename RETV, typename VISITOR>
  RETV Handle_add(VISITOR* visitor, NODE_PTR add_node);
  template <typename RETV, typename VISITOR>
  RETV Handle_rotate(VISITOR* visitor, NODE_PTR rot_node);
  template <typename RETV, typename VISITOR>
  RETV Handle_relin(VISITOR* visitor, NODE_PTR relin_node);
  template <typename RETV, typename VISITOR>
  RETV Handle_modswitch(VISITOR* visitor, NODE_PTR mod_switch);
  template <typename RETV, typename VISITOR>
  RETV Handle_rescale(VISITOR* visitor, NODE_PTR rescale);
  template <typename RETV, typename VISITOR>
  RETV Handle_bootstrap(VISITOR* visitor, NODE_PTR bootstrap);
  template <typename RETV, typename VISITOR>
  RETV Handle_encode(VISITOR* visitor, NODE_PTR encode);
};

template <typename RETV, typename VISITOR>
RETV CKKS_ANA_IMPL::Handle_mul(VISITOR* visitor, NODE_PTR mul_node) {
  CTX_PARAM_ANA_CTX& ana_ctx   = visitor->Context();
  LOWER_CTX*         lower_ctx = ana_ctx.Lower_ctx();
  TYPE_ID            rtype_id  = mul_node->Rtype_id();
  if (!lower_ctx->Is_cipher3_type(rtype_id) &&
      !lower_ctx->Is_cipher_type(rtype_id)) {
    return RETV{false, 0};
  }

  // 1. get mul_level of mul_node result
  uint32_t mul_level = ana_ctx.Top_mul_level();
  // set mul_level attr for mul result
  ana_ctx.Set_node_mul_level(mul_node, mul_level);

  ana_ctx.Trace(ckks::TRACE_DETAIL::TD_CKKS_LEVEL_MGT,
                std::string(ana_ctx.Indent(), '+'), "mul: l=", mul_level,
                "+\n");

  // 2. handle children nodes
  ana_ctx.Push_mul_level(mul_level);
  RETV child0_res = visitor->template Visit<RETV>(mul_node->Child(0));
  RETV child1_res = visitor->template Visit<RETV>(mul_node->Child(1));

  AIR_ASSERT_MSG(mul_level == ana_ctx.Top_mul_level(),
                 "mul level inconsistent");
  ana_ctx.Pop_mul_level();

  return RETV{child0_res.Mul_level_inc() || child1_res.Mul_level_inc(),
              mul_level};
}

template <typename RETV, typename VISITOR>
RETV CKKS_ANA_IMPL::Handle_add(VISITOR* visitor, NODE_PTR add_node) {
  CTX_PARAM_ANA_CTX& ana_ctx   = visitor->Context();
  LOWER_CTX*         lower_ctx = ana_ctx.Lower_ctx();
  TYPE_ID            rtype_id  = add_node->Rtype_id();
  if (!lower_ctx->Is_cipher3_type(rtype_id) &&
      !lower_ctx->Is_cipher_type(rtype_id)) {
    return RETV{false, 0};
  }

  // 1. get mul_level of add_node result
  uint32_t mul_level = ana_ctx.Top_mul_level();
  // set mul_level attr for add result
  ana_ctx.Set_node_mul_level(add_node, mul_level);

  ana_ctx.Trace(ckks::TRACE_DETAIL::TD_CKKS_LEVEL_MGT,
                std::string(ana_ctx.Indent(), '+'), "add: l=", mul_level, "\n");

  // 2. handle children nodes
  ana_ctx.Push_mul_level(mul_level);
  RETV child0_res = visitor->template Visit<RETV>(add_node->Child(0));
  RETV child1_res = visitor->template Visit<RETV>(add_node->Child(1));

  AIR_ASSERT_MSG(mul_level == ana_ctx.Top_mul_level(),
                 "mul level inconsistent");
  ana_ctx.Pop_mul_level();

  return RETV{child0_res.Mul_level_inc() || child1_res.Mul_level_inc(),
              mul_level};
}

template <typename RETV, typename VISITOR>
RETV CKKS_ANA_IMPL::Handle_rotate(VISITOR* visitor, NODE_PTR rot_node) {
  // 1. get mul_level of rot_node result
  CTX_PARAM_ANA_CTX& ana_ctx   = visitor->Context();
  uint32_t           mul_level = ana_ctx.Top_mul_level();
  // set mul_level attr for rotate result
  ana_ctx.Set_node_mul_level(rot_node, mul_level);

  ana_ctx.Trace(ckks::TRACE_DETAIL::TD_CKKS_LEVEL_MGT,
                std::string(ana_ctx.Indent(), '+'), "rotate: l=", mul_level,
                "\n");

  // 2. handle cipher type child0
  ana_ctx.Push_mul_level(mul_level);
  RETV child0_res = visitor->template Visit<RETV>(rot_node->Child(0));

  AIR_ASSERT_MSG(mul_level == ana_ctx.Top_mul_level(),
                 "mul level inconsistent");
  ana_ctx.Pop_mul_level();

  // get rotate index from attr
  const char* rot_idx_key   = nn::core::ATTR::RNUM;
  uint32_t    rot_idx_count = 0;
  const int*  rot_idx       = rot_node->Attr<int>(rot_idx_key, &rot_idx_count);
  AIR_ASSERT(rot_idx != nullptr && rot_idx_count > 0);
  for (uint32_t i = 0; i < rot_idx_count; ++i) {
    visitor->Context().Add_rotate_index(rot_idx[i]);
  }

  return child0_res;
}

template <typename RETV, typename VISITOR>
RETV CKKS_ANA_IMPL::Handle_relin(VISITOR* visitor, NODE_PTR relin_node) {
  // 1. get mul_level of relin result
  CTX_PARAM_ANA_CTX& ana_ctx   = visitor->Context();
  uint32_t           mul_level = ana_ctx.Top_mul_level();
  // set mul_level attr for relin result
  ana_ctx.Set_node_mul_level(relin_node, mul_level);

  ana_ctx.Trace(ckks::TRACE_DETAIL::TD_CKKS_LEVEL_MGT,
                std::string(ana_ctx.Indent(), '+'), "relin: l=", mul_level,
                "\n");

  return visitor->template Visit<RETV>(relin_node->Child(0));
}

template <typename RETV, typename VISITOR>
RETV CKKS_ANA_IMPL::Handle_modswitch(VISITOR* visitor, NODE_PTR modswitch) {
  // 1. get mul_level of mod_switch result
  CTX_PARAM_ANA_CTX& ana_ctx   = visitor->Context();
  uint32_t           mul_level = ana_ctx.Top_mul_level();
  // set mul_level attr for mod_switch result
  ana_ctx.Set_node_mul_level(modswitch, mul_level);

  ana_ctx.Trace(ckks::TRACE_DETAIL::TD_CKKS_LEVEL_MGT,
                std::string(ana_ctx.Indent(), '+'), "modswitch: l=", mul_level,
                "-\n");

  // 2. handle child node
  // mod_switch inc mul_level by 1
  mul_level += 1;
  ana_ctx.Push_mul_level(mul_level);
  ana_ctx.Update_mul_level(mul_level);
  RETV res = visitor->template Visit<RETV>(modswitch->Child(0));

  AIR_ASSERT_MSG(mul_level == ana_ctx.Top_mul_level(),
                 "mul level inconsistent");
  ana_ctx.Pop_mul_level();

  return res;
}

template <typename RETV, typename VISITOR>
RETV CKKS_ANA_IMPL::Handle_rescale(VISITOR* visitor, NODE_PTR rescale) {
  // 1. get mul_level of rescale result
  CTX_PARAM_ANA_CTX& ana_ctx   = visitor->Context();
  uint32_t           mul_level = ana_ctx.Top_mul_level();
  // set mul_level attr for rescale result
  ana_ctx.Set_node_mul_level(rescale, mul_level);

  ana_ctx.Trace(ckks::TRACE_DETAIL::TD_CKKS_LEVEL_MGT,
                std::string(ana_ctx.Indent(), '+'), "rescale: l=", mul_level,
                "-\n");

  // 2. handle child node
  // rescale inc mul_level by 1
  mul_level += 1;
  ana_ctx.Push_mul_level(mul_level);
  ana_ctx.Update_mul_level(mul_level);

  RETV res = visitor->template Visit<RETV>(rescale->Child(0));

  AIR_ASSERT_MSG(mul_level == ana_ctx.Top_mul_level(),
                 "mul level inconsistent");
  ana_ctx.Pop_mul_level();
  return res;
}

template <typename RETV, typename VISITOR>
RETV CKKS_ANA_IMPL::Handle_bootstrap(VISITOR* visitor, NODE_PTR bootstrap) {
  // 1. get mul_level of bootstrap result
  CTX_PARAM_ANA_CTX& ana_ctx   = visitor->Context();
  uint32_t           mul_level = ana_ctx.Top_mul_level();

  // 2. handle redundant bootstrap
  if (!ana_ctx.Rgn_scl_bts_mng() && !ana_ctx.Rgn_bts_mng()) {
    NODE_PTR        child = bootstrap->Child(0);
    const uint32_t* rescale_lev_attr =
        child->Attr<uint32_t>(FHE_ATTR_KIND::RESCALE_LEVEL);
    AIR_ASSERT(rescale_lev_attr != nullptr);
    uint32_t rescale_lev = *rescale_lev_attr;
    if (rescale_lev + mul_level <= ana_ctx.Input_level()) {
      NODE_PTR parent_node = ana_ctx.Parent(1);
      for (uint32_t id = 0; id < parent_node->Num_child(); ++id) {
        if (parent_node->Child(id) != bootstrap) continue;
        ana_ctx.Trace(ckks::TD_CKKS_LEVEL_MGT, "Delete redundant bootstrap");
        ana_ctx.Trace_obj(ckks::TD_CKKS_LEVEL_MGT, ana_ctx.Parent_stmt());
        parent_node->Set_child(id, child);
        ana_ctx.Trace_obj(ckks::TD_CKKS_LEVEL_MGT, ana_ctx.Parent_stmt());

        ana_ctx.Push_mul_level(mul_level);
        (void)visitor->template Visit<RETV>(bootstrap->Child(0));
        AIR_ASSERT_MSG(mul_level == ana_ctx.Top_mul_level(),
                       "mul level inconsistent");
        ana_ctx.Pop_mul_level();
        return RETV(false, mul_level);
      }
    }
  } else {
    const uint32_t* lev_attr = bootstrap->Attr<uint32_t>(FHE_ATTR_KIND::LEVEL);
    AIR_ASSERT(lev_attr != nullptr);
    mul_level = *lev_attr;
    if (!ana_ctx.Opt_bts_res_lvl()) {
      // 1. update bootstrap resulting level to max bootstrap level.
      uint32_t mbl = ana_ctx.Max_bts_lvl() + 1;
      bootstrap->Set_attr<uint32_t>(FHE_ATTR_KIND::LEVEL, &mbl, 1);

      // 2. modswitch bootstrap result to required level.
      NODE_PTR parent = ana_ctx.Parent(1);
      OPCODE   opc    = parent->Opcode();
      AIR_ASSERT(opc == air::core::OPC_ST || opc == air::core::OPC_STP);
      air::opt::SSA_VER_PTR ssa_ver =
          ana_ctx.Ssa_cntr()->Node_ver(parent->Id());
      STMT_PTR st = ana_ctx.Gen_modswitch(ssa_ver->Sym(), mul_level, mbl,
                                          bootstrap->Spos());
      STMT_LIST(ana_ctx.Parent_block()).Append(parent->Stmt(), st);
      mul_level = mbl;
    }
  }

  // set mul_depth attr for App_relu
  if (ana_ctx.Opt_bts_res_lvl() && !ana_ctx.Rgn_bts_mng() &&
      !ana_ctx.Rgn_scl_bts_mng()) {
    ana_ctx.Set_node_mul_level(bootstrap, mul_level);
  }

  STMT_PTR parent_stmt = ana_ctx.Parent_stmt();
  ana_ctx.Trace_obj(ckks::TD_CKKS_LEVEL_MGT, parent_stmt);
  ana_ctx.Trace(ckks::TD_CKKS_LEVEL_MGT,
                " output mul_level: ", std::to_string(mul_level), "\n");

  // 3. update function mul_level
  uint32_t bootstrap_mul_depth =
      ana_ctx.Lower_ctx()->Get_ctx_param().Mul_depth_of_bootstrap();
  uint32_t tot_mul_level = bootstrap_mul_depth + mul_level;
  ana_ctx.Update_mul_level(tot_mul_level);

  // 4. handle child bootstrap child node
  const uint32_t child_mul_level = 1;
  ana_ctx.Push_mul_level(child_mul_level);

  (void)visitor->template Visit<RETV>(bootstrap->Child(0));

  AIR_ASSERT_MSG(child_mul_level == ana_ctx.Top_mul_level(),
                 "mul level inconsistent");
  ana_ctx.Pop_mul_level();
  return RETV(false, bootstrap_mul_depth);
}

template <typename RETV, typename VISITOR>
RETV CKKS_ANA_IMPL::Handle_encode(VISITOR* visitor, NODE_PTR encode) {
  // 1. get mul_level of bootstrap result
  CTX_PARAM_ANA_CTX& ana_ctx   = visitor->Context();
  uint32_t           mul_level = ana_ctx.Top_mul_level();
  AIR_ASSERT_MSG(mul_level > 0, "target level of encode must >= 1");

  ana_ctx.Trace(ckks::TRACE_DETAIL::TD_CKKS_LEVEL_MGT,
                std::string(ana_ctx.Indent(), '+'), "encode: l=", mul_level,
                "\n");

  bool is_entry_func =
      ana_ctx.Func_scope()->Owning_func()->Entry_point()->Is_program_entry();
  if (!ana_ctx.Enc_lvl_cst() || !is_entry_func) {
    return RETV{false, mul_level};
  }
  CONTAINER*  cntr     = encode->Container();
  GLOB_SCOPE* glob     = cntr->Glob_scope();
  TYPE_PTR    u32_type = glob->Prim_type(PRIMITIVE_TYPE::INT_U32);
  NODE_PTR    int_cst = cntr->New_intconst(u32_type, mul_level, encode->Spos());
  const uint32_t level_child_id = 3;
  encode->Set_child(level_child_id, int_cst);
  encode->Set_attr(core::FHE_ATTR_KIND::LEVEL, &mul_level, 1);
  return RETV(false, mul_level);
}

//! @brief CTX_PARAM analyzer.
//! In current impl, mul_level of ciphertext is determined on demand.
//! In other words, the mul_level of a ciphertext is set as the minimum value
//! that fulfill the demands on mul_level at all use sites.
//! Generally, there are data dependence between ciphertext variables.
//! To collect demands on a variable, we need process all the variables that
//! data depends on it first. Therefore, function boby and all nested blocks
//! are traversed in reverse order. For each node, we use the mul_level of the
//! result to deduce mul_level of the opnds. For leaf nodes(ld/ldp/ild),
//! mul_level demand on the variable is updated with the mul_level of the leaf
//! node.
class CTX_PARAM_ANA {
public:
  using CORE_HANDLER = air::core::HANDLER<CORE_ANA_IMPL>;
  using CKKS_HANDLER = fhe::ckks::HANDLER<CKKS_ANA_IMPL>;
  using ANA_VISITOR =
      air::base::VISITOR<CTX_PARAM_ANA_CTX, CORE_HANDLER, CKKS_HANDLER>;
  using SSA_CONTAINER = air::opt::SSA_CONTAINER;
  using DRIVER_CTX    = air::driver::DRIVER_CTX;
  using CKKS_CONFIG   = ckks::CKKS_CONFIG;
  CTX_PARAM_ANA(FUNC_SCOPE* func_scope, LOWER_CTX* ctx,
                const DRIVER_CTX* driver_ctx, const CKKS_CONFIG* config)
      : _func_scope(func_scope),
        _lower_ctx(ctx),
        _driver_ctx(driver_ctx),
        _config(config),
        _ssa_cntr(&func_scope->Container()),
        _ana_ctx(driver_ctx, config, ctx, func_scope, &_ssa_cntr, 0) {}

  ~CTX_PARAM_ANA() {}

  //! @brief calculate mul_level and rotate_index of function.
  //! update CTX_PARAM in LOWER_CTX.
  R_CODE                   Run();
  const CTX_PARAM_ANA_CTX& Get_ana_ctx() const { return _ana_ctx; }
  CTX_PARAM_ANA_CTX&       Get_ana_ctx() { return _ana_ctx; }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  CTX_PARAM_ANA(void);
  CTX_PARAM_ANA(const CTX_PARAM_ANA&);
  CTX_PARAM_ANA& operator=(const CTX_PARAM_ANA&);

  void                    Build_ssa();
  SSA_CONTAINER&          Ssa_cntr() { return _ssa_cntr; };
  R_CODE                  Update_ctx_param_with_config();
  FUNC_SCOPE*             Func_scope() const { return _func_scope; }
  LOWER_CTX*              Lower_ctx() const { return _lower_ctx; }
  const DRIVER_CTX*       Driver_ctx() const { return _driver_ctx; }
  const CKKS_CONFIG*      Config() const { return _config; }
  FUNC_SCOPE*             _func_scope;
  LOWER_CTX*              _lower_ctx;
  const DRIVER_CTX*       _driver_ctx;
  const CKKS_CONFIG*      _config;
  air::opt::SSA_CONTAINER _ssa_cntr;
  CTX_PARAM_ANA_CTX       _ana_ctx;
};

}  // namespace core
}  // namespace fhe
#endif
