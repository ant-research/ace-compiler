//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <climits>
#include <list>
#include <string>

#include "air/base/analyze_ctx.h"
#include "air/base/container.h"
#include "air/base/container_decl.h"
#include "air/base/opcode.h"
#include "air/base/st_decl.h"
#include "air/base/visitor.h"
#include "air/core/handler.h"
#include "air/core/null_handler.h"
#include "air/core/opcode.h"
#include "air/driver/driver_ctx.h"
#include "air/driver/pass.h"
#include "air/opt/ssa_node_list.h"
#include "air/util/messg.h"
#include "fhe/ckks/ckks_gen.h"
#include "fhe/ckks/ckks_handler.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/ckks/config.h"
#include "fhe/ckks/invalid_handler.h"
#include "fhe/core/lower_ctx.h"
#include "fhe/sihe/sihe_gen.h"

namespace fhe {
namespace ckks {
using namespace air::base;
using namespace fhe::core;

class SCALE_INFO {
public:
  SCALE_INFO(uint16_t scale_deg, uint16_t rescale_level)
      : _scale_deg(scale_deg), _rescale_level(rescale_level) {
    AIR_ASSERT_MSG(scale_deg < UINT16_MAX, "scale degree out of range");
    AIR_ASSERT_MSG(rescale_level < UINT16_MAX, "rescale level out of range");
  }
  SCALE_INFO(const SCALE_INFO& other)
      : SCALE_INFO(other.Scale_deg(), other.Rescale_level()) {}
  SCALE_INFO() : SCALE_INFO(0, 0) {}

  ~SCALE_INFO() {}

  void Set_scale_deg(uint16_t val) {
    AIR_ASSERT_MSG(val < UINT16_MAX, "scale degree out of range");
    _scale_deg = val;
  }
  uint16_t Scale_deg() const { return _scale_deg; }
  void     Set_rescale_level(uint16_t val) {
    AIR_ASSERT_MSG(val < UINT16_MAX, "rescale level out of range");
    _rescale_level = val;
  }
  uint16_t Rescale_level() const { return _rescale_level; }

  bool operator==(const SCALE_INFO& rhs) const {
    return _scale_deg == rhs._scale_deg && _rescale_level == rhs._rescale_level;
  }

  void Print(std::ostream& os) const {
    os << "{rescale_level= " << _rescale_level << ", scale_deg= " << _scale_deg
       << "}" << std::endl;
  }
  void Print(void) const { Print(std::cout); }

private:
  uint16_t _scale_deg;
  uint16_t _rescale_level;
};

//! return value of scale manage handlers
class SCALE_MNG_RETV {
public:
  SCALE_MNG_RETV(const SCALE_INFO& scale_info, NODE_PTR node)
      : _scale_info(scale_info.Scale_deg(), scale_info.Rescale_level()),
        _node(node) {}

  SCALE_MNG_RETV(const SCALE_MNG_RETV& o)
      : SCALE_MNG_RETV(o.Scale_info(), o.Node()) {}

  SCALE_MNG_RETV() : SCALE_MNG_RETV(SCALE_INFO(0, 0), NODE_PTR()) {}

  explicit SCALE_MNG_RETV(NODE_PTR node)
      : SCALE_MNG_RETV(SCALE_INFO(0, 0), node) {}

  SCALE_MNG_RETV& operator=(const SCALE_MNG_RETV& o) {
    _scale_info = o.Scale_info();
    _node       = o.Node();
    return *this;
  }

  ~SCALE_MNG_RETV() {}

  uint32_t Scale() const { return _scale_info.Scale_deg(); }
  uint32_t Rescale_level() const { return _scale_info.Rescale_level(); }
  const SCALE_INFO& Scale_info() const { return _scale_info; }
  SCALE_INFO&       Scale_info() { return _scale_info; }
  void              Set_scale_info(const SCALE_INFO& si) { _scale_info = si; }
  NODE_PTR          Node() const { return _node; }

private:
  SCALE_INFO _scale_info;  // scale and mul-depth of handled node
  NODE_PTR   _node;
};

//! @biref Rescale information for an expression, including its parent node,
//! child ID in the parent node, its node, rescale count, and resulting
//! SCALE_INFO.
class EXPR_RESCALE_INFO {
public:
  using SET  = std::set<EXPR_RESCALE_INFO>;
  using ITER = SET::iterator;
  EXPR_RESCALE_INFO(NODE_PTR parent, NODE_PTR node, uint32_t rs_cnt,
                    const SCALE_INFO& res_scale)
      : _parent(parent), _node(node), _rs_cnt(rs_cnt), _scale_info(res_scale) {}
  EXPR_RESCALE_INFO(const EXPR_RESCALE_INFO& o)
      : EXPR_RESCALE_INFO(o._parent, o._node, o._rs_cnt, o._scale_info) {}

  ~EXPR_RESCALE_INFO() {}

  NODE_PTR Parent(void) const { return _parent; }
  NODE_PTR Node(void) const { return _node; }
  uint32_t Child_id(void) const {
    for (uint32_t id = 0; id < _parent->Num_child(); ++id) {
      if (_parent->Child(id) == _node) return id;
    }
    AIR_ASSERT(false);
    return UINT_MAX;
  }
  uint32_t          Rescale_cnt(void) const { return _rs_cnt; }
  void              Set_rescale_cnt(uint32_t val) { _rs_cnt = val; }
  const SCALE_INFO& Res_scale(void) const { return _scale_info; }
  SCALE_INFO&       Res_scale(void) { return _scale_info; }
  bool              operator<(const EXPR_RESCALE_INFO& o) const {
    return _node->Id() < o.Node()->Id();
  }

  void Print(std::ostream& os) const {
    os << "Require " << _rs_cnt << " rescaling on " << std::endl
       << _node->To_str() << "the " << Child_id() << "-th child of" << std::endl
       << _parent->To_str() << "to ";
    _scale_info.Print(os);
  }
  void Print(void) const { Print(std::cout); }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  EXPR_RESCALE_INFO(void);
  EXPR_RESCALE_INFO& operator=(const EXPR_RESCALE_INFO&);

  NODE_PTR   _parent;
  NODE_PTR   _node;
  uint32_t   _rs_cnt;      // number of required rescale.
  SCALE_INFO _scale_info;  // scale info after rescale.
};

//! context of scale manage handlers.
class SCALE_MNG_CTX : public ANALYZE_CTX {
public:
  // key: id of SSA_VER/NODE; val: SCALE_INFO of SSA_VER/NODE
  using SCALE_MAP     = std::map<uint32_t, SCALE_INFO>;
  using PHI_NODE_LIST = std::list<air::opt::PHI_NODE_ID>;

  explicit SCALE_MNG_CTX(LOWER_CTX* ctx, const CKKS_CONFIG* config,
                         const air::driver::DRIVER_CTX* driver_ctx,
                         uint32_t                       formal_scale_deg,
                         air::opt::SSA_CONTAINER*       ssa_cntr)
      : _lower_ctx(ctx),
        _config(config),
        _driver_ctx(driver_ctx),
        _formal_scale_deg(formal_scale_deg),
        _ssa_cntr(ssa_cntr) {}

  ~SCALE_MNG_CTX() {}

  void Set_scale_info(air::opt::SSA_VER_ID ver, const SCALE_INFO& si) {
    _ssa_ver_scale_info[ver.Value()] = si;
  }

  const SCALE_INFO& Get_scale_info(air::opt::SSA_VER_ID ver) const {
    SCALE_MAP::const_iterator iter = _ssa_ver_scale_info.find(ver.Value());
    if (iter == _ssa_ver_scale_info.end()) {
      AIR_ASSERT_MSG(false, "encounter use before define");
    }
    return iter->second;
  }

  SCALE_MAP::iterator Find_scale_info(air::opt::SSA_VER_ID ver) {
    return _ssa_ver_scale_info.find(ver.Value());
  }

  SCALE_MAP::const_iterator End_scale_info() const {
    return _ssa_ver_scale_info.end();
  }

  void Set_scale_info(NODE_ID node, const SCALE_INFO& si) {
    _node_scale_info[node.Value()] = si;
  }

  const SCALE_INFO& Get_scale_info(NODE_ID node) const {
    SCALE_MAP::const_iterator iter = _node_scale_info.find(node.Value());
    if (iter == _node_scale_info.end()) {
      AIR_ASSERT_MSG(false, "encounter use before define");
    }
    return iter->second;
  }

  SCALE_MAP::iterator Node_scale_info_iter(NODE_ID node) {
    return _node_scale_info.find(node.Value());
  }

  const SCALE_MAP& Node_scale_info(void) const { return _node_scale_info; }
  SCALE_MAP&       Node_scale_info(void) { return _node_scale_info; }

  uint32_t Get_scale(air::opt::SSA_VER_ID ver) const {
    return Get_scale_info(ver).Scale_deg();
  }

  uint32_t Get_rescale_level(air::opt::SSA_VER_ID ver) const {
    return Get_scale_info(ver).Rescale_level();
  }

  uint32_t Scale_factor() const {
    return _lower_ctx->Get_ctx_param().Get_scaling_factor_bit_num();
  }

  LOWER_CTX* Lower_ctx() const { return _lower_ctx; }
  uint32_t   Unfix_scale() const { return 0; }
  bool       Is_unfix_scale(uint32_t scale) const { return scale == 0; }
  bool       Need_rescale(uint32_t scale) const { return scale > 2; }

  //! set scale attr for node result
  void Set_node_scale_info(NODE_PTR node, uint32_t scale,
                           uint32_t rescale_level) {
    Set_node_scale_info(node, SCALE_INFO(scale, rescale_level));
  }
  void Set_node_scale_info(NODE_PTR node, const SCALE_INFO& si) {
    uint32_t scale = si.Scale_deg();
    node->Set_attr(core::FHE_ATTR_KIND::SCALE, &scale, 1);

    uint32_t rs_level = si.Rescale_level();
    node->Set_attr(core::FHE_ATTR_KIND::RESCALE_LEVEL, &rs_level, 1);

    Set_scale_info(node->Id(), si);
  }

  const PHI_NODE_LIST& Phi_res_need_rescale() const { return _rescale_phi_res; }
  void                 Set_phi_res_need_rescale(air::opt::PHI_NODE_ID phi) {
    _rescale_phi_res.push_back(phi);
  }

  const EXPR_RESCALE_INFO::SET& Rescale_expr(void) const {
    return _rescale_expr;
  }

  //! @brief Record the expr that require rescaling. If the expr has already
  //! been recorded, update the rescaling info to one with lower resulting scale
  //! degree.
  void Add_expr_rescale_info(EXPR_RESCALE_INFO rs_info) {
    std::pair<EXPR_RESCALE_INFO::ITER, bool> res =
        _rescale_expr.insert(rs_info);
    if (!res.second) {
      EXPR_RESCALE_INFO::ITER iter           = res.first;
      uint32_t                prev_scale_deg = iter->Res_scale().Scale_deg();
      uint32_t                cur_scale_deg  = rs_info.Res_scale().Scale_deg();
      if (prev_scale_deg > cur_scale_deg) {
        uint32_t new_rs_cnt =
            rs_info.Rescale_cnt() + (prev_scale_deg - cur_scale_deg);
        rs_info.Set_rescale_cnt(new_rs_cnt);
        _rescale_expr.erase(iter);
        _rescale_expr.insert(rs_info);
        Trace(TD_CKKS_SCALE_MGT, "Update rescale candidate:\n");
        Trace_obj(TD_CKKS_SCALE_MGT, &rs_info);
      }
      AIR_ASSERT(prev_scale_deg >= cur_scale_deg);
    } else {
      Trace(TD_CKKS_SCALE_MGT, _rescale_expr.size(), "-th candidate:\n");
      Trace_obj(TD_CKKS_SCALE_MGT, &rs_info);
    }
  }

  uint32_t Formal_scale_deg() const { return _formal_scale_deg; }
  air::opt::SSA_CONTAINER*       Ssa_cntr() const { return _ssa_cntr; }
  const CKKS_CONFIG*             Config() { return _config; }
  const air::driver::DRIVER_CTX* Driver_ctx() { return _driver_ctx; }

  //! @brief check if the node occurs in occ_stmt is rescale candiate
  bool Rescale_node(NODE_PTR node, STMT_PTR occ_stmt, uint32_t scale_deg);
  //! @brief check if the phi node is rescale candidate
  bool Rescale_phi_res(air::opt::PHI_NODE_PTR phi, uint32_t opnd_id,
                       uint32_t scale_deg);
  //! @brief check chi result and annotate scale and level
  void Process_chi_res(air::base::NODE_PTR node, uint32_t scale,
                       uint32_t level);

  //! @brief return true if the current function is the entry function;
  //! otherwise, return false.
  bool Is_entry_func(void) const {
    FUNC_PTR func = _ssa_cntr->Func_scope()->Owning_func();
    return func->Entry_point()->Is_program_entry();
  }

  //! @brief return true if the current function requires local scale
  //! management, meaning ReSBM is either disabled or only inserts
  //! bootstrapping. Otherwise return false. Since ReSBM, a global scale
  //! manager, is executed before local scale managers (such as EVA's waterline
  //! rescaling, PARS, and ACE_SM), the CKKS options for local scale managers
  //! are overridden by those of ReSBM.
  bool Req_local_scale_mng(void) const { return !Rgn_scl_bts_mng(); }

  //! @brief return true if the current funtion requires local scale
  //! management, EVA's waterline rescaling. Otherwise, return false.
  bool Req_eva(void) const {
    if (!Req_local_scale_mng()) return false;
    // Enable EVA's waterline rescaling in callee functions, as other local
    // scale managers cannot guarantee correctness in these functions.
    if (!Is_entry_func()) return true;
    return Eva_waterline();
  }
  //! @brief return true if the current funtion requires local scale
  //! management, PARS. Otherwise, return false.
  bool Req_pars(void) const {
    if (!Req_local_scale_mng()) return false;
    if (!Is_entry_func()) return false;
    return Pars_rsc();
  }

  //! @brief return true if the current funtion requires local ACE scale
  //! management. Otherwise, return false.
  bool Ace_sm(void) const {
    return !Eva_waterline() && !Pars_rsc() && !Rgn_scl_bts_mng();
  }

  //! @brief Return true if the current function requires local scale management
  //! using ACE_SM, a scale manager optimized based on EVA's waterline rescaling
  //! for ResNet models. Otherwise, return false.
  bool Req_ace_sm(void) const {
    if (!Req_local_scale_mng()) return false;
    if (!Is_entry_func()) return false;
    return Ace_sm();
  }

  DECLARE_CKKS_OPTION_CONFIG_ACCESS_API((*_config))
  DECLARE_TRACE_DETAIL_API((*_config), _driver_ctx)
private:
  // REQUIRED UNDEFINED UNWANTED methods
  SCALE_MNG_CTX(void);
  SCALE_MNG_CTX(const SCALE_MNG_CTX&);
  SCALE_MNG_CTX& operator=(const SCALE_MNG_CTX&);

  LOWER_CTX* _lower_ctx = nullptr;  // lower context records FHE types
  air::opt::SSA_CONTAINER*       _ssa_cntr   = nullptr;
  const CKKS_CONFIG*             _config     = nullptr;  // config of CKKS
  const air::driver::DRIVER_CTX* _driver_ctx = nullptr;
  SCALE_MAP     _ssa_ver_scale_info;     // scale and rescale level of ssa_ver
  SCALE_MAP     _node_scale_info;        // scale and rescale level of node
  PHI_NODE_LIST _rescale_phi_res;        // phi nodes need rescale result
  EXPR_RESCALE_INFO::SET _rescale_expr;  // expr need rescale
  uint32_t               _formal_scale_deg;  // scale degree of formals
};

//! @brief Implimentation of PARS scale management algorithm.
//! The original algorithm of PARS is presented in:
//! Lee Yongwoo,Heo Seonyeong,Cheon Seonyoung,Jeong Shinnung, Kim Changsu, Kim
//! Eunkyung, Lee Dongyoon, and Kim Hanjun. 2022. HECATE: Performance-Aware
//! Scale Optimization for Homomorphic Encryption Compiler. In 2022 IEEE/ACM
//! International Symposium on Code Generation and Optimization(CGO). 193-204.
class PARS {
public:
  using SCALE_MAP_ITER = SCALE_MNG_CTX::SCALE_MAP::iterator;
  PARS(SCALE_MNG_CTX* ctx) : _ctx(ctx) {}
  ~PARS() {}

  SCALE_INFO Handle(NODE_PTR node);

private:
  // REQUIRED UNDEFINED UNWANTED methods
  PARS(void);
  PARS(const PARS&);
  const PARS& operator=(const PARS&);

  //! @brief If arg0.scale > waterline * scale_factor: arg0 <- Rescale(arg0)
  void Rescale_ana(NODE_PTR node);
  //! @brief If opc in {CKKS.add, CKKS.mul}, arg0.level < arg1.level then:
  //! if arg0.scale == waterline: arg0 <- Modswitch(arg0)
  //! if arg0.scale > waterline:  arg0 <- Downscale(arg0)
  //! In current implementation, waterline is equal to scale_factor, only
  //! Modswitch is needed. Level match can be done by RTLIB automatically.
  void Level_match(NODE_PTR node);
  //! @brief If opc == CKKS.add and arg0.scale < arg1.scale then: arg1 <-
  //! rescale(arg1, arg0.scale).
  void Scale_match(NODE_PTR node);
  //! @brief If opc == CKKS.mul and arg0.scale * arg1.scale > waterline^2 *
  //! scale_factor then: arg0 <- Rescale(arg0); arg1 <- Rescale(arg1).
  SCALE_INFO     Downscale_analysis(NODE_PTR node);
  SCALE_MNG_CTX* Context(void) { return _ctx; }

  SCALE_MNG_CTX* _ctx;
};

//! @brief Implementation of ACE scale manager (ACE_SM), an optimization of
//! waterline rescaling. It aims to avoid inserting rescaling in loops and
//! insert rescaling before expensive operations like Rotate and Relinearize.
class ACE_SM {
public:
  explicit ACE_SM(SCALE_MNG_CTX* ctx) : _ctx(ctx) {}
  ~ACE_SM() {}

  SCALE_INFO Handle_mul(NODE_PTR node, SCALE_INFO si0, SCALE_INFO si1);
  SCALE_INFO Handle_add(NODE_PTR node, SCALE_INFO si0, SCALE_INFO si1);
  SCALE_INFO Handle_relin(NODE_PTR node, SCALE_INFO si);
  SCALE_INFO Handle_rotate(NODE_PTR node, SCALE_INFO si);

private:
  // REQUIRED UNDEFINED UNWANTED methods
  ACE_SM(void);
  ACE_SM(const ACE_SM&);
  const ACE_SM& operator=(const ACE_SM&);

  SCALE_INFO     Rescale_res(NODE_PTR node, const SCALE_INFO& si);
  SCALE_MNG_CTX* Context() { return _ctx; }
  SCALE_MNG_CTX* _ctx;
};

//! impl of CORE IR scale manager.
class CORE_SCALE_MANAGER : public air::core::NULL_HANDLER {
public:
  template <typename RETV, typename VISITOR>
  RETV Handle_do_loop(VISITOR* visitor, NODE_PTR do_loop);

  template <typename RETV, typename VISITOR>
  RETV Handle_func_entry(VISITOR* visitor, NODE_PTR entry_node);

  template <typename RETV, typename VISITOR>
  RETV Handle_idname(VISITOR* visitor, NODE_PTR idname);

  template <typename RETV, typename VISITOR>
  RETV Handle_ld(VISITOR* visitor, NODE_PTR ld_node);

  template <typename RETV, typename VISITOR>
  RETV Handle_ldp(VISITOR* visitor, NODE_PTR ldp_node);

  template <typename RETV, typename VISITOR>
  RETV Handle_ldc(VISITOR* visitor, NODE_PTR ldc_node) {
    // runtime will set scale of const as sf.
    return RETV{SCALE_INFO(1, 0), ldc_node};
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ild(VISITOR* visitor, NODE_PTR ild_node);

  template <typename RETV, typename VISITOR>
  RETV Handle_zero(VISITOR* visitor, NODE_PTR zero) {
    // scale of CORE.zero is unfixed.
    uint32_t unfix_scale_deg = visitor->Context().Unfix_scale();
    return RETV{SCALE_INFO(unfix_scale_deg, 0), zero};
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_one(VISITOR* visitor, NODE_PTR one) {
    // scale of CORE.one is unfixed.
    uint32_t unfix_scale_deg = visitor->Context().Unfix_scale();
    return RETV{SCALE_INFO(unfix_scale_deg, 0), one};
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_intconst(VISITOR* visitor, NODE_PTR intcst) {
    // scale of CORE.intconst is unfixed.
    uint32_t unfix_scale_deg = visitor->Context().Unfix_scale();
    return RETV{SCALE_INFO(unfix_scale_deg, 0), intcst};
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_st(VISITOR* visitor, NODE_PTR st_node);

  template <typename RETV, typename VISITOR>
  RETV Handle_stp(VISITOR* visitor, NODE_PTR stp_node);

  template <typename RETV, typename VISITOR>
  RETV Handle_ist(VISITOR* visitor, NODE_PTR ist_node);

  template <typename RETV, typename VISITOR>
  RETV Handle_call(VISITOR* visitor, NODE_PTR call_node);

  template <typename RETV, typename VISITOR>
  RETV Handle_retv(VISITOR* visitor, NODE_PTR retv_node);

private:
  SCALE_INFO Rescale_prehead_opnd(SCALE_MNG_CTX& ctx, SCALE_INFO si,
                                  air::opt::SSA_SYM_PTR ver);
  template <typename VISITOR>
  void Handle_phi_opnd(VISITOR* visitor, NODE_PTR do_loop, uint32_t opnd_id);
};

template <typename VISITOR>
void CORE_SCALE_MANAGER::Handle_phi_opnd(VISITOR* visitor, NODE_PTR do_loop,
                                         uint32_t opnd_id) {
  air::opt::SSA_CONTAINER* ssa_cntr = visitor->Context().Ssa_cntr();
  air::opt::PHI_NODE_ID    phi_id   = ssa_cntr->Node_phi(do_loop->Id());
  air::opt::PHI_LIST       phi_list(ssa_cntr, phi_id);

  auto handle_phi_opnd = [=](air::opt::PHI_NODE_PTR phi, uint32_t opnd_id,
                             SCALE_MNG_CTX& ctx) {
    air::opt::SSA_VER_ID               ver_id = phi->Opnd_id(opnd_id);
    air::opt::SSA_VER_PTR              ver    = ctx.Ssa_cntr()->Ver(ver_id);
    SCALE_MNG_CTX::SCALE_MAP::iterator scale_info_iter =
        ctx.Find_scale_info(ver_id);
    if (scale_info_iter == ctx.End_scale_info()) return;

    SCALE_INFO opnd_scale_info(scale_info_iter->second);
    if (opnd_id == air::opt::PREHEADER_PHI_OPND_ID) {
      opnd_scale_info = Rescale_prehead_opnd(ctx, opnd_scale_info, ver->Sym());
    }
    if (ctx.Rescale_phi_res(phi, opnd_id, opnd_scale_info.Scale_deg())) {
      opnd_scale_info.Set_scale_deg(opnd_scale_info.Scale_deg() - 1);
      opnd_scale_info.Set_rescale_level(opnd_scale_info.Rescale_level() + 1);

      ctx.Trace(TD_CKKS_SCALE_MGT, "  phi: ", phi->To_str(),
                std::string(ctx.Indent(), ' '),
                " opnd s=", opnd_scale_info.Scale_deg() - 1,
                " l=", opnd_scale_info.Rescale_level() + 1, "\n");

      ctx.Set_phi_res_need_rescale(phi->Id());
    }

    ctx.Trace(TD_CKKS_SCALE_MGT, std::string(ctx.Indent(), ' '),
              "  phi: ", phi->To_str(), " res s=", opnd_scale_info.Scale_deg(),
              " l=", opnd_scale_info.Rescale_level(), "\n");

    ctx.Set_scale_info(phi->Result_id(), opnd_scale_info);
  };
  phi_list.For_each(handle_phi_opnd, opnd_id, visitor->Context());
}

template <typename RETV, typename VISITOR>
RETV CORE_SCALE_MANAGER::Handle_do_loop(VISITOR* visitor, NODE_PTR node) {
  SCALE_MNG_CTX& ctx = visitor->Context();
  // 1. handle phi opnds from preheader
  Handle_phi_opnd(visitor, node, air::opt::PREHEADER_PHI_OPND_ID);

  // 2. handle stmt list
  for (uint32_t child_id = 0; child_id < node->Num_child(); ++child_id) {
    NODE_PTR child = node->Child(child_id);
    (void)visitor->template Visit<RETV>(child);
  }

  // 3. handle phi opnds from backedge
  Handle_phi_opnd(visitor, node, air::opt::BACK_EDGE_PHI_OPND_ID);
  uint32_t unfix_scale_deg = visitor->Context().Unfix_scale();
  return RETV(SCALE_INFO(unfix_scale_deg, 0), node);
}

template <typename RETV, typename VISITOR>
RETV CORE_SCALE_MANAGER::Handle_func_entry(VISITOR* visitor, NODE_PTR node) {
  ANALYZE_CTX::GUARD guard(visitor->Context(), node);

  SCALE_MNG_CTX& ctx = visitor->Context();
  ctx.Trace(TD_CKKS_SCALE_MGT, std::string(ctx.Indent(), '+'),
            "func_entry: s=", ctx.Formal_scale_deg(), " l=", INIT_RESCALE_LEVEL,
            "\n");

  ctx.Process_chi_res(node, ctx.Formal_scale_deg(), INIT_RESCALE_LEVEL);

  uint32_t child_num = node->Num_child();
  AIR_ASSERT(child_num > 1);
  for (uint32_t child_id = 0; (child_id + 1) < child_num; ++child_id) {
    (void)visitor->template Visit<RETV>(node->Child(child_id));
  }
  return visitor->template Visit<RETV>(node->Last_child());
}

template <typename RETV, typename VISITOR>
RETV CORE_SCALE_MANAGER::Handle_idname(VISITOR* visitor, NODE_PTR node) {
  SCALE_MNG_CTX&        ctx                  = visitor->Context();
  uint32_t              formal_scale_deg     = ctx.Formal_scale_deg();
  uint32_t              formal_rescale_level = 0;
  air::opt::SSA_VER_PTR ssa_ver = ctx.Ssa_cntr()->Node_ver(node->Id());
  SCALE_INFO            si(formal_scale_deg, formal_rescale_level);
  ctx.Set_scale_info(ssa_ver->Id(), si);

  ctx.Trace(TD_CKKS_SCALE_MGT, std::string(ctx.Indent(), '+'),
            "idname: ", ssa_ver->To_str(), " s=", formal_scale_deg,
            " l=", formal_rescale_level, "\n");

  return RETV(si, node);
}

template <typename RETV, typename VISITOR>
RETV CORE_SCALE_MANAGER::Handle_ld(VISITOR* visitor, NODE_PTR node) {
  SCALE_MNG_CTX& ctx    = visitor->Context();
  TYPE_PTR       ty_ptr = node->Rtype();
  TYPE_ID        type_id =
      ty_ptr->Is_array() ? ty_ptr->Cast_to_arr()->Elem_type_id() : ty_ptr->Id();
  const LOWER_CTX* lower_ctx = ctx.Lower_ctx();
  if (!lower_ctx->Is_cipher_type(type_id) &&
      !lower_ctx->Is_cipher3_type(type_id)) {
    // return unfix_scale(0) for value stored in non-cipher var
    return RETV(SCALE_INFO(ctx.Unfix_scale(), 0), node);
  }

  air::opt::SSA_VER_PTR ssa_ver = ctx.Ssa_cntr()->Node_ver(node->Id());

  if (ssa_ver->Kind() == air::opt::VER_DEF_KIND::CHI) {
    // scan chi list to find out the scale/level from elements/fields
    NODE_PTR              def_node = ssa_ver->Def_chi()->Def_stmt()->Node();
    air::opt::CHI_NODE_ID chi      = ctx.Ssa_cntr()->Node_chi(def_node->Id());
    air::opt::CHI_LIST    list(ctx.Ssa_cntr(), chi);
    auto visit = [](air::opt::CHI_NODE_PTR chi, SCALE_MNG_CTX* ctx,
                    air::opt::SSA_VER_PTR ver) {
      if (chi->Sym_id() == ver->Sym_id()) {
        return;
      }
      // TODO: check all elements/fields. so far only 1 element/field is checked
      const SCALE_INFO& scale_info = ctx->Get_scale_info(chi->Result_id());
      ctx->Set_scale_info(ver->Id(), scale_info);

      ctx->Trace(TD_CKKS_SCALE_MGT, std::string(ctx->Indent(), ' '),
                 " ld: ", chi->To_str(), " (res) s=", scale_info.Scale_deg(),
                 " l=", scale_info.Rescale_level(), "\n");
    };
    list.For_each(visit, &ctx, ssa_ver);
  }

  const SCALE_INFO& scale_info = ctx.Get_scale_info(ssa_ver->Id());
  ctx.Set_node_scale_info(node, scale_info);

  ctx.Trace(TD_CKKS_SCALE_MGT, std::string(ctx.Indent(), ' '),
            "ld: ", ssa_ver->To_str(), " s=", scale_info.Scale_deg(),
            " l=", scale_info.Rescale_level(), "\n");

  return RETV{scale_info, node};
}

template <typename RETV, typename VISITOR>
RETV CORE_SCALE_MANAGER::Handle_ldp(VISITOR* visitor, NODE_PTR node) {
  SCALE_MNG_CTX&   ctx       = visitor->Context();
  TYPE_ID          type_id   = node->Rtype_id();
  const LOWER_CTX* lower_ctx = ctx.Lower_ctx();
  if (!lower_ctx->Is_cipher_type(type_id) &&
      !lower_ctx->Is_cipher3_type(type_id)) {
    // return unfix_scale(0) for value stored in non-cipher preg
    return RETV(SCALE_INFO(ctx.Unfix_scale(), 0), node);
  }

  air::opt::SSA_VER_PTR ssa_ver    = ctx.Ssa_cntr()->Node_ver(node->Id());
  const SCALE_INFO&     scale_info = ctx.Get_scale_info(ssa_ver->Id());
  ctx.Set_node_scale_info(node, scale_info);

  ctx.Trace(TD_CKKS_SCALE_MGT, std::string(ctx.Indent(), ' '),
            "ldp: ", ssa_ver->To_str(), " s=", scale_info.Scale_deg(),
            " l=", scale_info.Rescale_level(), "\n");

  return RETV{scale_info, node};
}

template <typename RETV, typename VISITOR>
RETV CORE_SCALE_MANAGER::Handle_ild(VISITOR* visitor, NODE_PTR node) {
  SCALE_MNG_CTX&   ctx            = visitor->Context();
  TYPE_ID          access_type_id = node->Access_type_id();
  const LOWER_CTX* lower_ctx      = ctx.Lower_ctx();
  if (!lower_ctx->Is_cipher_type(access_type_id) &&
      !lower_ctx->Is_cipher3_type(access_type_id)) {
    // return unfix_scale(0) for value stored in non-cipher array
    return RETV(SCALE_INFO(ctx.Unfix_scale(), 0), node);
  }

  NODE_PTR addr_child = node->Child(0);
  AIR_ASSERT(addr_child->Opcode() == air::core::OPC_ARRAY);
  NODE_PTR lda_child = addr_child->Array_base();
  AIR_ASSERT(lda_child->Opcode() == air::core::OPC_LDA);
  ADDR_DATUM_PTR array_sym = lda_child->Addr_datum();
  AIR_ASSERT(array_sym->Type()->Is_array());

  air::opt::SSA_CONTAINER* ssa_cntr   = ctx.Ssa_cntr();
  air::opt::SSA_VER_PTR    ssa_ver    = ssa_cntr->Node_ver(node->Id());
  const SCALE_INFO&        scale_info = ctx.Get_scale_info(ssa_ver->Id());
  ctx.Set_node_scale_info(node, scale_info);

  ctx.Trace(TD_CKKS_SCALE_MGT, std::string(ctx.Indent(), ' '),
            "ild: ", ssa_ver->To_str(), " s=", scale_info.Scale_deg(),
            " l=", scale_info.Rescale_level(), "\n");

  return RETV{scale_info, node};
}

template <typename RETV, typename VISITOR>
RETV CORE_SCALE_MANAGER::Handle_st(VISITOR* visitor, NODE_PTR node) {
  // 1. handle rhs node
  SCALE_MNG_CTX& ctx   = visitor->Context();
  NODE_PTR       child = node->Child(0);
  RETV           res   = visitor->template Visit<RETV>(child);

  // 2. update scale of stored addr_datum
  air::opt::SSA_CONTAINER* ssa_cntr = ctx.Ssa_cntr();
  air::opt::SSA_VER_PTR    ssa_ver  = ssa_cntr->Node_ver(node->Id());
  ctx.Set_scale_info(ssa_ver->Id(), res.Scale_info());
  ctx.Set_node_scale_info(node, res.Scale_info());

  ctx.Trace(TD_CKKS_SCALE_MGT, std::string(ctx.Indent(), ' '),
            "st: ", ssa_ver->To_str(), " s=", res.Scale(),
            " l=", res.Rescale_level(), "\n");

  // 3. update chi result
  ctx.Process_chi_res(node, res.Scale(), res.Rescale_level());

  return RETV{res.Scale_info(), node};
}

template <typename RETV, typename VISITOR>
RETV CORE_SCALE_MANAGER::Handle_stp(VISITOR* visitor, NODE_PTR node) {
  // 1. handle rhs node
  SCALE_MNG_CTX& ctx   = visitor->Context();
  NODE_PTR       child = node->Child(0);
  RETV           res   = visitor->template Visit<RETV>(child);

  // 2. update scale of stored preg
  air::opt::SSA_CONTAINER* ssa_cntr = ctx.Ssa_cntr();
  air::opt::SSA_VER_PTR    ssa_ver  = ssa_cntr->Node_ver(node->Id());
  ctx.Set_scale_info(ssa_ver->Id(), res.Scale_info());
  ctx.Set_node_scale_info(node, res.Scale_info());

  ctx.Trace(TD_CKKS_SCALE_MGT, std::string(ctx.Indent(), ' '),
            "stp: ", ssa_ver->To_str(), " s=", res.Scale(),
            " l=", res.Rescale_level(), "\n");

  return RETV{res.Scale_info(), node};
}

template <typename RETV, typename VISITOR>
RETV CORE_SCALE_MANAGER::Handle_ist(VISITOR* visitor, NODE_PTR node) {
  // 1. handle rhs node
  SCALE_MNG_CTX& ctx = visitor->Context();
  NODE_PTR       rhs = node->Child(1);
  RETV           res = visitor->template Visit<RETV>(rhs);

  // 2. update scale of istored cipher
  NODE_PTR addr_child = node->Child(0);
  AIR_ASSERT(addr_child->Opcode() == air::core::OPC_ARRAY);
  NODE_PTR lda_child = addr_child->Array_base();
  AIR_ASSERT(lda_child->Opcode() == air::core::OPC_LDA);
  ADDR_DATUM_PTR array_sym = lda_child->Addr_datum();
  AIR_ASSERT(array_sym->Type()->Is_array());

  air::opt::SSA_CONTAINER* ssa_cntr = ctx.Ssa_cntr();
  air::opt::SSA_VER_PTR    ssa_ver  = ssa_cntr->Node_ver(node->Id());
  ctx.Set_scale_info(ssa_ver->Id(), res.Scale_info());

  ctx.Trace(TD_CKKS_SCALE_MGT, std::string(ctx.Indent(), ' '),
            "ist: ", ssa_ver->To_str(), " s=", res.Scale(),
            " l=", res.Rescale_level(), "\n");

  ctx.Process_chi_res(node, res.Scale(), res.Rescale_level());

  return RETV{res.Scale_info(), node};
}

template <typename RETV, typename VISITOR>
RETV CORE_SCALE_MANAGER::Handle_call(VISITOR* visitor, NODE_PTR node) {
  SCALE_MNG_CTX& ctx       = visitor->Context();
  LOWER_CTX*     lower_ctx = ctx.Lower_ctx();
  CKKS_GEN       ckks_gen(node->Container(), lower_ctx);
  // 1. handle child nodes: reset scale of each child to scale_factor
  uint32_t max_rescale_level = 0;
  for (uint32_t child_id = 0; child_id < node->Num_child(); ++child_id) {
    NODE_PTR child = node->Child(child_id);
    RETV     res   = visitor->template Visit<RETV>(child);

    TYPE_ID rtype = child->Rtype_id();
    // not support CIPHER3 type parameter.
    AIR_ASSERT(!lower_ctx->Is_cipher3_type(rtype));
    if (!lower_ctx->Is_cipher_type(rtype)) {
      continue;
    }

    uint32_t scale_deg     = res.Scale();
    uint32_t rescale_level = res.Rescale_level();
    ctx.Set_node_scale_info(child, res.Scale_info());
    // As a calling convention, reduce scale_deg of call operands to 1.
    if (scale_deg > 1) {
      uint32_t rs_cnt = scale_deg - 1;
      rescale_level += rs_cnt;
      EXPR_RESCALE_INFO rs_info(node, child, rs_cnt,
                                SCALE_INFO(1, rescale_level));
      ctx.Add_expr_rescale_info(rs_info);
    }
    max_rescale_level = std::max(max_rescale_level, rescale_level);
  }

  // 2. handle ret value: set scale of ret value as scale factor.
  PREG_PTR ret_preg = node->Ret_preg();
  TYPE_ID  type_id  = ret_preg->Type_id();
  // not support return CIPHER3 type value.
  AIR_ASSERT(!lower_ctx->Is_cipher3_type(type_id));
  if (!lower_ctx->Is_cipher_type(type_id)) {
    // return unfix_scale(0) for value stored in non-cipher preg
    return RETV(SCALE_INFO(ctx.Unfix_scale(), 0), node);
  }

  const uint32_t* mul_depth_ptr =
      node->Attr<uint32_t>(FHE_ATTR_KIND::MUL_DEPTH);
  AIR_ASSERT(mul_depth_ptr != nullptr);
  uint32_t retv_rescale_level = *mul_depth_ptr + max_rescale_level;

  air::opt::SSA_CONTAINER* ssa_cntr = ctx.Ssa_cntr();
  air::opt::SSA_VER_PTR    retv     = ssa_cntr->Node_ver(node->Id());
  SCALE_INFO               retv_scale_info(1, retv_rescale_level);
  ctx.Set_scale_info(retv->Id(), retv_scale_info);

  ctx.Trace(TD_CKKS_SCALE_MGT, std::string(ctx.Indent(), ' '),
            "call: ", retv->To_str(), " (ret) s=", retv_scale_info.Scale_deg(),
            " l=", retv_scale_info.Rescale_level(), "\n");

  return RETV{retv_scale_info, node};
}

template <typename RETV, typename VISITOR>
RETV CORE_SCALE_MANAGER::Handle_retv(VISITOR* visitor, NODE_PTR node) {
  SCALE_MNG_CTX& ctx = visitor->Context();
  // 1. handle child of retv
  NODE_PTR child         = node->Child(0);
  TYPE_PTR child_type    = child->Rtype();
  TYPE_ID  child_type_id = child_type->Is_array()
                               ? child_type->Cast_to_arr()->Elem_type_id()
                               : child_type->Id();
  AIR_ASSERT(ctx.Lower_ctx()->Is_cipher_type(child_type_id));

  RETV     res           = visitor->template Visit<RETV>(child);
  uint32_t scale_deg     = res.Scale();
  uint32_t rescale_level = res.Rescale_level();

  ctx.Set_node_scale_info(child, res.Scale_info());
  ctx.Trace(TD_CKKS_SCALE_MGT, std::string(ctx.Indent(), ' '),
            "retv: (main) s=", scale_deg, " l=", rescale_level, "\n");
  // 2. remain retv of Main_graph unchanged.
  FUNC_PTR parent_func = node->Func_scope()->Owning_func();
  if (parent_func->Entry_point()->Is_program_entry()) {
    return RETV{res.Scale_info(), node};
  }

  // 3. rescale child of retv to scale factor.
  // opnd of retv must be: load/ldid sym
  if (scale_deg == 1 && child->Has_sym() && child->Is_ld()) {
    return RETV{res.Scale_info(), node};
  }

  const SPOS& spos       = child->Spos();
  CONTAINER*  cntr       = node->Container();
  FUNC_SCOPE* func_scope = node->Func_scope();

  PREG_PTR preg = func_scope->New_preg(child->Rtype());
  STMT_PTR stp  = cntr->New_stp(child, preg, spos);
  cntr->Stmt_list().Prepend(node->Stmt(), stp);

  // Reduce scale degree of returned value to 1.
  if (!ctx.Rgn_scl_bts_mng() && scale_deg > 1) {
    uint32_t rs_cnt = scale_deg - 1;
    rescale_level += rs_cnt;
    EXPR_RESCALE_INFO rs_info(stp->Node(), child, rs_cnt,
                              SCALE_INFO(1, rescale_level));
    ctx.Add_expr_rescale_info(rs_info);
  }

  NODE_PTR ld_tmp = cntr->New_ldp(preg, spos);
  node->Set_child(0, ld_tmp);
  SCALE_INFO si(1, rescale_level);
  ctx.Set_node_scale_info(ld_tmp, si);

  return RETV{si, node};
}

//! impl of CKKS IR scale manager
class CKKS_SCALE_MANAGER : public fhe::ckks::INVALID_HANDLER {
public:
  template <typename RETV, typename VISITOR>
  RETV Handle_mul(VISITOR* visitor, NODE_PTR node);

  template <typename RETV, typename VISITOR>
  RETV Handle_add(VISITOR* visitor, NODE_PTR node);

  // template <typename RETV, typename VISITOR>
  // RETV Handle_sub(VISITOR* visitor, NODE_PTR node);

  template <typename RETV, typename VISITOR>
  RETV Handle_rotate(VISITOR* visitor, NODE_PTR node);

  template <typename RETV, typename VISITOR>
  RETV Handle_relin(VISITOR* visitor, NODE_PTR node);

  template <typename RETV, typename VISITOR>
  RETV Handle_encode(VISITOR* visitor, NODE_PTR node);

  template <typename RETV, typename VISITOR>
  RETV Handle_bootstrap(VISITOR* visitor, NODE_PTR node);

  template <typename RETV, typename VISITOR>
  RETV Handle_rescale(VISITOR* visitor, NODE_PTR node);

private:
  //! gen CKKS.rescale(node) to dec scale of node.
  //! scale of CKKS.rescale(node) is (node.scale - sf).
  SCALE_INFO Rescale_res(SCALE_MNG_CTX* ctx, NODE_PTR node,
                         const SCALE_INFO& si);

  //! handle encode as 2nd child of CKKS.mul/add/sub
  void Handle_encode_in_bin_arith_node(SCALE_MNG_CTX* ctx, NODE_PTR bin_node,
                                       uint32_t child0_scale);
};

template <typename RETV, typename VISITOR>
RETV CKKS_SCALE_MANAGER::Handle_mul(VISITOR* visitor, NODE_PTR node) {
  SCALE_MNG_CTX& ctx       = visitor->Context();
  LOWER_CTX*     lower_ctx = ctx.Lower_ctx();

  // handle child0: dec scale of child0 to sf
  NODE_PTR child0 = node->Child(0);
  AIR_ASSERT(lower_ctx->Is_cipher_type(child0->Rtype_id()));
  RETV       retv0 = visitor->template Visit<RETV>(child0);
  SCALE_INFO si0   = retv0.Scale_info();

  // handle child1: dec scale of child 1 to sf
  NODE_PTR   child1       = node->Child(1);
  TYPE_ID    rtype_child1 = child1->Rtype_id();
  SCALE_INFO si1          = si0;
  if (lower_ctx->Is_cipher_type(rtype_child1)) {
    RETV retv1 = visitor->template Visit<RETV>(child1);
    si1        = retv1.Scale_info();
  } else if (lower_ctx->Is_plain_type(rtype_child1)) {
    if (child1->Opcode() == OPC_ENCODE) {
      Handle_encode_in_bin_arith_node(&ctx, node, 1);
    } else {
      Templ_print(std::cout, "TODO: handle plaintext expr or var");
    }
    si1.Set_scale_deg(1);
  } else if (child1->Rtype()->Is_scalar()) {
    // scale of scalar is set as sf in runtime. no need to handle it in cmplr.
    si1.Set_scale_deg(1);
  } else {
    Templ_print(std::cout, "ERROR: not supported rtype of child1 of CKKS::MUL");
    AIR_ASSERT(false);
  }

  uint16_t   scale_deg     = si0.Scale_deg() + si1.Scale_deg();
  uint16_t   rescale_level = std::max(si0.Rescale_level(), si1.Rescale_level());
  SCALE_INFO scale_info(scale_deg, rescale_level);
  ctx.Set_node_scale_info(node, scale_info);

  if (ctx.Req_eva()) {
    AIR_ASSERT(scale_deg == 2);
    NODE_PTR parent = ctx.Parent(1);
    scale_info      = SCALE_INFO(1, rescale_level + 1);
    EXPR_RESCALE_INFO rs_info(parent, node, scale_deg - 1, scale_info);
    ctx.Add_expr_rescale_info(rs_info);
  } else if (ctx.Req_pars()) {
    scale_info = PARS(&ctx).Handle(node);
  } else if (ctx.Req_ace_sm()) {
    scale_info = ACE_SM(&ctx).Handle_mul(node, si0, si1);
  }

  ctx.Trace(TD_CKKS_SCALE_MGT, std::string(ctx.Indent(), ' '),
            "mul: s=", scale_deg, " l=", rescale_level, "\n");
  return RETV{scale_info, node};
}

template <typename RETV, typename VISITOR>
RETV CKKS_SCALE_MANAGER::Handle_add(VISITOR* visitor, NODE_PTR node) {
  SCALE_MNG_CTX& ctx = visitor->Context();

  // handle child0
  NODE_PTR   child0 = node->Child(0);
  RETV       retv0  = visitor->template Visit<RETV>(child0);
  SCALE_INFO si0    = retv0.Scale_info();

  // handle child1
  SCALE_INFO si1          = si0;
  NODE_PTR   child1       = node->Child(1);
  TYPE_ID    rtype_child1 = child1->Rtype_id();
  LOWER_CTX* lower_ctx    = ctx.Lower_ctx();
  OPCODE     opc_child1   = child1->Opcode();
  if (lower_ctx->Is_cipher_type(rtype_child1) ||
      lower_ctx->Is_cipher3_type(rtype_child1)) {
    RETV retv1 = visitor->template Visit<RETV>(child1);
    // Currently, CKKS.add supports at most one operand with an unfixed scale.
    // This occurs when accumulating ciphertexts, with the sum ciphertext
    // initialized to ZERO.
    AIR_ASSERT_MSG(
        !ctx.Is_unfix_scale(si0.Scale_deg()) ||
            !ctx.Is_unfix_scale(retv1.Scale()),
        "Unsupported case: At least one operand's scale must be fixed.");
    if (!ctx.Is_unfix_scale(retv1.Scale())) si1 = retv1.Scale_info();
  } else if (lower_ctx->Is_plain_type(rtype_child1)) {
    AIR_ASSERT(opc_child1 == OPC_ENCODE);
  } else {
    AIR_ASSERT_MSG(child1->Rtype()->Is_prim(), "not supported type of child1");
    AIR_ASSERT_MSG(opc_child1 == air::core::OPC_ONE ||
                       opc_child1 == air::core::OPC_ZERO ||
                       opc_child1 == air::core::OPC_LDC,
                   "not supported opcode of child1");
  }

  SCALE_INFO si = si1;
  if (ctx.Req_pars()) {
    si = PARS(&ctx).Handle(node);
  } else if (ctx.Req_ace_sm()) {
    si = ACE_SM(&ctx).Handle_add(node, si0, si1);
  }
  if (opc_child1 == OPC_ENCODE) {
    AIR_ASSERT(!ctx.Is_unfix_scale(si.Scale_deg()));
    Handle_encode_in_bin_arith_node(&ctx, node, si.Scale_deg());
  }
  ctx.Set_node_scale_info(node, si);
  ctx.Trace(TD_CKKS_SCALE_MGT, std::string(ctx.Indent(), ' '),
            "add: s=", si.Scale_deg(), " l=", si.Rescale_level(), "\n");
  return RETV{si, node};
}

template <typename RETV, typename VISITOR>
RETV CKKS_SCALE_MANAGER::Handle_rotate(VISITOR* visitor, NODE_PTR node) {
  // 1. handle child0
  SCALE_MNG_CTX& ctx   = visitor->Context();
  NODE_PTR       child = node->Child(0);
  RETV           retv0 = visitor->template Visit<RETV>(child);
  SCALE_INFO     si    = retv0.Scale_info();
  // handle child1
  (void)visitor->template Visit<RETV>(node->Child(1));

  if (ctx.Req_pars()) {
    si = PARS(&ctx).Handle(node);
  } else if (ctx.Req_ace_sm()) {
    si = ACE_SM(&ctx).Handle_rotate(node, si);
  }
  ctx.Set_node_scale_info(node, si);
  return RETV{si, node};
}

template <typename RETV, typename VISITOR>
RETV CKKS_SCALE_MANAGER::Handle_relin(VISITOR* visitor, NODE_PTR node) {
  SCALE_MNG_CTX& ctx   = visitor->Context();
  NODE_PTR       child = node->Child(0);
  AIR_ASSERT(ctx.Lower_ctx()->Is_cipher3_type(child->Rtype_id()));
  RETV       retv = visitor->template Visit<RETV>(child);
  SCALE_INFO si   = retv.Scale_info();
  if (ctx.Req_pars()) {
    si = PARS(&ctx).Handle(node);
  } else if (ctx.Req_ace_sm()) {
    si = ACE_SM(&ctx).Handle_relin(node, si);
  }
  ctx.Set_node_scale_info(node, si);
  return RETV{si, node};
}

template <typename RETV, typename VISITOR>
RETV CKKS_SCALE_MANAGER::Handle_encode(VISITOR* visitor, NODE_PTR node) {
  const uint32_t scale_child_id = 2;
  NODE_PTR       scale_node     = node->Child(scale_child_id);
  uint32_t       scale_deg      = 0;
  if (scale_node->Opcode() == air::core::OPC_INTCONST) {
    scale_deg = scale_node->Intconst();
    scale_deg = (scale_deg != 0) ? scale_deg : 1;
  } else {
    AIR_ASSERT(scale_node->Opcode() == OPC_SCALE);
    NODE_PTR cipher_node = scale_node->Child(0);
    scale_deg            = visitor->template Visit<RETV>(cipher_node).Scale();
  }
  uint32_t rescale_lev = 0;
  return RETV{SCALE_INFO(scale_deg, rescale_lev), node};
}

template <typename RETV, typename VISITOR>
RETV CKKS_SCALE_MANAGER::Handle_bootstrap(VISITOR* visitor, NODE_PTR node) {
  SCALE_MNG_CTX& ctx = visitor->Context();
  // handle child: reset scale of child to scale factor.
  NODE_PTR child       = node->Child(0);
  RETV     res         = visitor->template Visit<RETV>(child);
  uint32_t scale_deg   = res.Scale();
  uint32_t rescale_lev = res.Rescale_level();

  // As a calling convention, reduce scale_deg of bootstrapping operands to 1.
  if (scale_deg > 1) {
    uint32_t rs_cnt = scale_deg - 1;
    rescale_lev += rs_cnt;
    EXPR_RESCALE_INFO rs_info(node, child, rs_cnt, SCALE_INFO(1, rescale_lev));
    ctx.Add_expr_rescale_info(rs_info);
  }

  return RETV{SCALE_INFO(1, rescale_lev), node};
}

template <typename RETV, typename VISITOR>
RETV CKKS_SCALE_MANAGER::Handle_rescale(VISITOR* visitor, NODE_PTR node) {
  SCALE_MNG_CTX& ctx = visitor->Context();
  // handle child: reset scale of child to scale factor.
  NODE_PTR child     = node->Child(0);
  RETV     res       = visitor->template Visit<RETV>(child);
  uint32_t scale_deg = res.Scale();
  AIR_ASSERT_MSG(scale_deg > 1,
                 "Scale degree of rescale operand must be larger than 1");
  uint32_t   rescale_lev = res.Rescale_level();
  SCALE_INFO si(scale_deg - 1, rescale_lev + 1);
  ctx.Set_node_scale_info(node, si);
  return RETV{si, node};
}

//! insert rescale nodes to fulfill scale constraints:
//! 1. CKKS.add/sub: child0.scale == child1.scale
//! 2. CKKS.mul: (child0.scale + child1.scale) <= (sf + _scale_of_formal)
class SCALE_MANAGER {
public:
  using CORE_HANDLER = air::core::HANDLER<CORE_SCALE_MANAGER>;
  using CKKS_HANDLER = HANDLER<CKKS_SCALE_MANAGER>;
  using INSERT_VISITOR =
      air::base::VISITOR<SCALE_MNG_CTX, CORE_HANDLER, CKKS_HANDLER>;

  SCALE_MANAGER(const air::driver::DRIVER_CTX* driver_ctx,
                const CKKS_CONFIG* config, FUNC_SCOPE* func_scope,
                LOWER_CTX* ctx, uint32_t scale_deg_of_formal)
      : _func_scope(func_scope),
        _ssa_cntr(&func_scope->Container()),
        _mng_ctx(ctx, config, driver_ctx, scale_deg_of_formal, &_ssa_cntr) {}

  SCALE_MANAGER(const air::driver::DRIVER_CTX* driver_ctx,
                const CKKS_CONFIG* config, FUNC_SCOPE* func_scope,
                LOWER_CTX* ctx)
      : _func_scope(func_scope),
        _ssa_cntr(&func_scope->Container()),
        _mng_ctx(ctx, config, driver_ctx, 1, &_ssa_cntr) {}

  ~SCALE_MANAGER() {}

  //! insert rescale nodes to fulfill scale constraints
  void Run() {
    Mng_ctx().Trace(TD_CKKS_SCALE_MGT, ">>> CKKS SCALE MANAGEMENT:\n\n");
    // 1. build SSA
    Build_ssa();

    Mng_ctx().Trace(TD_CKKS_SCALE_MGT,
                    ">>> SSA dump before scale management:\n\n");
    Mng_ctx().Trace_obj(TD_CKKS_SCALE_MGT, Mng_ctx().Ssa_cntr());

    // 2. visit each node to collect expressions and phi results that require
    // rescaling.
    air::base::ANALYZE_CTX trav_ctx;
    INSERT_VISITOR         visitor(Mng_ctx(), {CORE_HANDLER(), CKKS_HANDLER()});
    NODE_PTR               func_body = Func_scope()->Container().Entry_node();
    visitor.template Visit<SCALE_MNG_RETV>(func_body);

    // 3.1 rescale result of phi
    Rescale_phi_res();
    // 3.2 rescale expressions
    Rescale_expr();
  }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  SCALE_MANAGER(void);
  SCALE_MANAGER(const SCALE_MANAGER&);
  SCALE_MANAGER& operator=(const SCALE_MANAGER&);
  void           Build_ssa();
  STMT_PTR       Gen_rescale(air::opt::SSA_SYM_PTR sym, const SCALE_INFO& si,
                             SPOS spos);
  void           Rescale_phi_res();
  void           Rescale_expr();
  FUNC_SCOPE*    Func_scope() const { return _func_scope; }
  SCALE_MNG_CTX& Mng_ctx() { return _mng_ctx; }
  LOWER_CTX*     Lower_ctx() const { return _mng_ctx.Lower_ctx(); }
  air::opt::SSA_CONTAINER* Ssa_cntr() { return &_ssa_cntr; }

  FUNC_SCOPE*   _func_scope;  // function scope to fulfill scale constraints
  SCALE_MNG_CTX _mng_ctx;     // context for scale manage handlers
  air::opt::SSA_CONTAINER _ssa_cntr;
};
}  // namespace ckks
}  // namespace fhe
