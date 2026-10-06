//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_SMO_BOOTSTRAP_INSERTER_H
#define FHE_CKKS_SMO_BOOTSTRAP_INSERTER_H

#include "air/base/container.h"
#include "air/base/node.h"
#include "air/base/transform_ctx.h"
#include "air/core/default_handler.h"
#include "air/core/opcode.h"
#include "air/opt/ssa_container.h"
#include "air/opt/ssa_node_list.h"
#include "air/util/debug.h"
#include "dfg_region.h"
#include "fhe/ckks/ckks_gen.h"
#include "fhe/ckks/ckks_handler.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/ckks/config.h"
#include "fhe/ckks/default_handler.h"
#include "fhe/core/lower_ctx.h"
#include "resbm_ctx.h"
namespace fhe {
namespace ckks {

//! @brief Return value of bootstrap/rescale inserter.
class INSERTER_RETV {
public:
  INSERTER_RETV(NODE_PTR node) : _node(node) {}
  INSERTER_RETV(void) : _node() {}
  INSERTER_RETV(const INSERTER_RETV& o) : _node(o._node) {}
  INSERTER_RETV& operator=(const INSERTER_RETV& o) {
    _node = o._node;
    return *this;
  }
  ~INSERTER_RETV() {}

  NODE_PTR Node() const { return _node; }

private:
  NODE_PTR _node;
};

//! @brief NODE_INFO contains NODE_ID/SSA_VER_ID that require
//! rescaling/bootstrap.
class NODE_INFO {
public:
  using NODE_ID    = air::base::NODE_ID;
  using SSA_VER_ID = air::opt::SSA_VER_ID;
  using SET        = std::set<NODE_INFO>;
  NODE_INFO(NODE_ID node) : _id(node), _kind(air::opt::DATA_NODE_EXPR) {}
  NODE_INFO(SSA_VER_ID ver) : _id(ver), _kind(air::opt::DATA_NODE_SSA_VER) {}
  NODE_INFO(const NODE_INFO& o) : _id(o._id._val), _kind(o._kind) {}
  ~NODE_INFO() {}

  bool Is_expr(void) const { return _kind == air::opt::DATA_NODE_EXPR; }
  bool Is_ssa_ver(void) const { return _kind == air::opt::DATA_NODE_SSA_VER; }
  NODE_ID    Expr(void) const { return _id._expr; }
  SSA_VER_ID Ssa_ver(void) const { return _id._ver; }
  bool       operator==(const NODE_INFO& o) const {
    return _id._val == o._id._val && _kind == o._kind;
  }
  bool operator<(const NODE_INFO& o) const {
    if (_id._val < o._id._val) return true;
    if (_id._val > o._id._val) return false;
    return _kind < o._kind;
  }
  std::string To_str() const {
    std::string str =
        (Is_expr() ? "NODE_" : "SSA_VAR_") + std::to_string(_id._val);
    return str;
  }
  void Print(std::ostream& os) const { os << To_str(); }
  void Print(void) const { Print(std::cout); }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  NODE_INFO(void);
  NODE_INFO& operator=(const NODE_INFO&);

  union RS_ID {
    uint32_t   _val;
    NODE_ID    _expr;
    SSA_VER_ID _ver;
    RS_ID(NODE_ID expr) : _expr(expr) {}
    RS_ID(SSA_VER_ID ver) : _ver(ver) {}
    RS_ID(uint32_t id) : _val(id) {}
  } _id;
  air::opt::DFG_NODE_KIND _kind;
};

//! @brief BTS_INFO contains NODE_INFO that require bootstrapping,
//! and the resulting level of bootstrap.
class BTS_INFO {
public:
  using NODE_ID    = air::base::NODE_ID;
  using SSA_VER_ID = air::opt::SSA_VER_ID;
  using SET        = std::set<BTS_INFO>;
  using SET_ITER   = SET::const_iterator;
  BTS_INFO(NODE_ID expr, uint32_t lev) : _node(expr), _bts_lev(lev) {}
  BTS_INFO(SSA_VER_ID ver, uint32_t lev) : _node(ver), _bts_lev(lev) {}
  BTS_INFO(const BTS_INFO& o) : _node(o._node), _bts_lev(o._bts_lev) {}
  ~BTS_INFO() {}

  bool       Is_expr(void) const { return _node.Is_expr(); }
  bool       Is_ssa_ver(void) const { return _node.Is_ssa_ver(); }
  NODE_ID    Expr(void) const { return _node.Expr(); }
  SSA_VER_ID Ssa_ver(void) const { return _node.Ssa_ver(); }
  uint32_t   Bts_lev(void) const { return _bts_lev; }

  bool operator<(const BTS_INFO& o) const {
    if (_node < o._node) return true;
    if (_node == o._node) return _bts_lev < o._bts_lev;
    return false;
  }
  bool operator==(const BTS_INFO& o) const {
    return _node == o._node && _bts_lev == o._bts_lev;
  }
  std::string To_str() const {
    std::string str = _node.To_str();
    str += " bts to lev= " + std::to_string(_bts_lev);
    return str;
  }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  BTS_INFO(void);
  BTS_INFO& operator=(const BTS_INFO&);

  NODE_INFO _node;     // node require bootstrap
  uint32_t  _bts_lev;  // bootstrap resulting level
};

using RS_SET  = NODE_INFO::SET;
using BTS_SET = BTS_INFO::SET;

//! @brief Context of the bootstrap/rescale inserter, containing sets for
//! bootstrap info (_bts_set) and rescale info (_rs_set). It provides access and
//! erase APIs for both sets and includes APIs to create rescale and bootstrap
//! nodes for SSA_VER and NODE.
class INSERTER_CTX : public air::base::TRANSFORM_CTX {
public:
  using NODE_ID       = air::base::NODE_ID;
  using NODE_PTR      = air::base::NODE_PTR;
  using STMT_PTR      = air::base::STMT_PTR;
  using SSA_CONTAINER = air::opt::SSA_CONTAINER;
  using SSA_VER_ID    = air::opt::SSA_VER_ID;
  using SSA_VER_PTR   = air::opt::SSA_VER_PTR;
  using SSA_SYM_PTR   = air::opt::SSA_SYM_PTR;
  using GLOB_SCOPE    = air::base::GLOB_SCOPE;

  INSERTER_CTX(const BTS_SET& bts_set, const RS_SET& rs_set,
               const SSA_CONTAINER* ssa_cntr, CONTAINER* new_cntr,
               core::LOWER_CTX* lower_ctx, RESBM_CTX* ctx)
      : air::base::TRANSFORM_CTX(new_cntr),
        _bts_set(bts_set),
        _rs_set(rs_set),
        _ssa_cntr(ssa_cntr),
        _new_cntr(new_cntr),
        _lower_ctx(lower_ctx),
        _ctx(ctx) {}
  INSERTER_CTX(const BTS_SET& bts_set, const SSA_CONTAINER* ssa_cntr,
               CONTAINER* new_cntr, core::LOWER_CTX* lower_ctx, RESBM_CTX* ctx)
      : INSERTER_CTX(bts_set, RS_SET(), ssa_cntr, new_cntr, lower_ctx, ctx) {}
  INSERTER_CTX(const RS_SET& rs_set, const SSA_CONTAINER* ssa_cntr,
               CONTAINER* new_cntr, core::LOWER_CTX* lower_ctx, RESBM_CTX* ctx)
      : INSERTER_CTX(BTS_SET(), rs_set, ssa_cntr, new_cntr, lower_ctx, ctx) {}
  ~INSERTER_CTX() {}

  //! @brief Return an iterator to the bootstrap info of the node.
  BTS_SET::const_iterator Bootstrap_info(NODE_ID node) const {
    for (BTS_SET::const_iterator iter = _bts_set.begin();
         iter != _bts_set.end(); ++iter) {
      if (iter->Is_expr() && iter->Expr() == node) return iter;
    }
    return _bts_set.end();
  }
  //! @brief Return an iterator to the bootstrap info of the ver.
  BTS_SET::const_iterator Bootstrap_info(SSA_VER_ID ver) const {
    for (BTS_SET::const_iterator iter = _bts_set.begin();
         iter != _bts_set.end(); ++iter) {
      if (iter->Is_ssa_ver() && iter->Ssa_ver() == ver) return iter;
    }
    return _bts_set.end();
  }
  //! @brief Return true if NODE/SSA_VER ID requires bootstrap; otherwise,
  //! return false.
  template <typename ID>
  bool Need_bootstrap(ID id) const {
    return Bootstrap_info(id) != _bts_set.end();
  }

  //! @brief Erase bootstrap info of iterator iter.
  void Erase_bts_info(BTS_SET::const_iterator iter) { _bts_set.erase(iter); }
  const BTS_SET& Bootstrap_info(void) const { return _bts_set; }

  //! @brief Return the bootstrap node for node with the resulting level set to
  //! bts_lev, namely CKKS.bootstrap(node, bts_lev).
  NODE_PTR Bootstrap_expr(NODE_PTR node, uint32_t bts_lev, bool need_rescale);
  //! @brief Return the bootstrap stmt for ssa_ver with the resulting level set
  //! to bts_lev. Create a bootstrap node for the symbol of ssa_ver and store
  //! it in the symbol. Return the store statement.
  //! TODO: for ssa_ver defined by STMT, bootstrap rhs of define stmt instead of
  //! create new store node.
  STMT_PTR Bootstrap_ssa_ver(SSA_VER_PTR ssa_ver, uint32_t bts_lev,
                             bool need_rescale);

  //! @brief Append BTS stmt to context and timing it if RTM is on
  void Append_bts(STMT_PTR bts_stmt);

  //! @brief Trace time of bootstrap operation.
  void Trace_bts_tm(STMT_PTR bts_stmt);

  //! @brief Return an iterator to the rescale info of the node.
  RS_SET::const_iterator Rs_point(NODE_ID node) const {
    return _rs_set.find(NODE_INFO(node));
  }
  //! @brief Return an iterator to the rescale info of the ver.
  RS_SET::const_iterator Rs_info(SSA_VER_ID ver) const {
    return _rs_set.find(NODE_INFO(ver));
  }
  //! @brief Return true if NODE/SSA_VER ID requires rescaling; otherwise,
  //! return false.
  template <typename ID>
  bool Need_rescale(ID id) const {
    return _rs_set.find(NODE_INFO(id)) != _rs_set.end();
  }
  //! @brief Erase rescale info of NODE/SSA_VER ID.
  template <typename ID>
  void Erase_rs_info(ID id) {
    _rs_set.erase(NODE_INFO(id));
  }
  //! @brief Erase rescale info of iterator iter.
  void Erase_rs_info(RS_SET::const_iterator iter) { _rs_set.erase(iter); }
  const RS_SET& Rescale_set(void) const { return _rs_set; }
  //! @brief Return the rescale node for node, namely CKKS.rescale(node).
  NODE_PTR Rescale_expr(NODE_PTR node);
  //! @brief Return the rescale statement for ssa_ver.
  //! For SSA_VER defined by STMT, rescale the RHS of the defining STMT using
  //! Rescale_expr, and replace the RHS with the rescale node. Return a null
  //! statement.
  //! For SSA_VER defined by CHI and PHI, create a rescale node for the symbol
  //! of ssa_ver and store it in the symbol. Return the store statement.
  STMT_PTR Rescale_ssa_ver(SSA_VER_PTR ssa_ver);

  void     Handle_ssa_ver(SSA_VER_PTR ver);
  NODE_PTR Handle_expr(NODE_ID orig_id, NODE_PTR new_node);

  //! @brief Return true if all bootstraps are processed; otherwise, return
  //! false.
  bool Bootstraped_all(void) { return _bts_set.empty(); }
  //! @brief Return true if all rescalings are processed; otherwise, return
  //! false.
  bool Rescaled_all(void) { return _rs_set.empty(); }

  CKKS_GEN   Ckks_gen(void) const { return CKKS_GEN(Cntr(), Lower_ctx()); }
  CONTAINER* Cntr(void) const { return _new_cntr; }
  const SSA_CONTAINER* Ssa_cntr(void) const { return _ssa_cntr; }
  GLOB_SCOPE*      Glob_scope(void) const { return _ssa_cntr->Glob_scope(); }
  core::LOWER_CTX* Lower_ctx(void) const { return _lower_ctx; }
  RESBM_CTX*       Resbm_ctx(void) const { return _ctx; }

private:
  const SSA_CONTAINER* _ssa_cntr  = nullptr;
  core::LOWER_CTX*     _lower_ctx = nullptr;
  CONTAINER*           _new_cntr  = nullptr;
  RESBM_CTX*           _ctx       = nullptr;
  BTS_SET              _bts_set;      // set of bootstrap info
  RS_SET               _rs_set;       // set of rescale info
  uint32_t             _bts_cnt = 0;  // count of inserted bootstrap
};

//! @brief Handler of CORE domain operation
class CORE_BTS_INSERTER_IMPL : public air::core::DEFAULT_HANDLER {
public:
  using NODE_PTR      = air::base::NODE_PTR;
  using STMT_PTR      = air::base::STMT_PTR;
  using SSA_VER_ID    = air::opt::SSA_VER_ID;
  using SSA_VER_PTR   = air::opt::SSA_VER_PTR;
  using SSA_CONTAINER = air::opt::SSA_CONTAINER;

  template <typename RETV, typename VISITOR>
  RETV Handle_func_entry(VISITOR* visitor, NODE_PTR node) {
    INSERTER_CTX* ctx = &visitor->Context();

    // 1. process function body
    uint32_t child_cnt = node->Num_child();
    AIR_ASSERT(child_cnt > 1);
    uint32_t func_body_id = child_cnt - 1;
    NODE_PTR old_body     = node->Child(func_body_id);
    AIR_ASSERT(old_body->Is_block());
    NODE_PTR new_body       = visitor->template Visit<RETV>(old_body).Node();
    NODE_PTR new_entry_node = ctx->Cntr()->Entry_node();
    new_entry_node->Set_child(func_body_id, new_body);
    new_body->Set_parent_stmt(new_entry_node->Stmt());

    // 2. process idname
    for (uint32_t id = 0; id < func_body_id; ++id) {
      NODE_PTR new_node = visitor->template Visit<RETV>(node->Child(id)).Node();
      new_entry_node->Set_child(id, new_node);
    }
    return RETV(new_entry_node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_idname(VISITOR* visitor, air::base::NODE_PTR node) {
    INSERTER_CTX* ctx = &visitor->Context();
    SSA_VER_PTR   ver = ctx->Ssa_cntr()->Node_ver(node->Id());

    RS_SET::const_iterator rs_iter = ctx->Rs_info(ver->Id());
    if (rs_iter != ctx->Rescale_set().end()) {
      STMT_PTR rs_stmt = ctx->Rescale_ssa_ver(ver);
      if (rs_stmt != STMT_PTR()) ctx->Cntr()->Stmt_list().Prepend(rs_stmt);
      ctx->Erase_rs_info(rs_iter);
    }
    BTS_INFO::SET::const_iterator iter = ctx->Bootstrap_info(ver->Id());
    if (iter != ctx->Bootstrap_info().end()) {
      STMT_PTR bts_stmt = ctx->Bootstrap_ssa_ver(ver, iter->Bts_lev(), false);
      ctx->Cntr()->Stmt_list().Prepend(bts_stmt);
      ctx->Trace_bts_tm(bts_stmt);
      ctx->Erase_bts_info(iter);
    }

    NODE_PTR new_idname = ctx->Cntr()->Clone_node(node);
    return RETV(new_idname);
  }

  template <typename RETV, typename VISITOR>
  NODE_PTR Handle_store_node(VISITOR* visitor, NODE_PTR node) {
    OPCODE opc = node->Opcode();
    AIR_ASSERT_MSG(opc == air::core::OPC_ST || opc == air::core::OPC_STP ||
                       opc == air::core::OPC_IST,
                   "Not supported store operation");

    uint32_t rhs_id  = (opc == air::core::OPC_IST) ? 1 : 0;
    NODE_PTR rhs     = node->Child(rhs_id);
    NODE_PTR new_rhs = visitor->template Visit<RETV>(rhs).Node();

    INSERTER_CTX* ctx = &visitor->Context();
    SSA_VER_PTR   ver = ctx->Ssa_cntr()->Node_ver(node->Id());
    if (ctx->Need_rescale(ver->Id())) {
      new_rhs = ctx->Rescale_expr(new_rhs);
      ctx->Erase_rs_info(ver->Id());
    }

    BTS_SET::const_iterator iter = ctx->Bootstrap_info(ver->Id());
    if (iter != ctx->Bootstrap_info().end()) {
      uint32_t bts_lev = iter->Bts_lev();
      new_rhs          = ctx->Bootstrap_expr(new_rhs, bts_lev, false);
      ctx->Erase_bts_info(iter);
    }
    return new_rhs;
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_st(VISITOR* visitor, air::base::NODE_PTR node) {
    NODE_PTR new_rhs = Handle_store_node<RETV>(visitor, node);

    INSERTER_CTX*  ctx  = &visitor->Context();
    CONTAINER*     cntr = ctx->Cntr();
    ADDR_DATUM_PTR new_addr =
        cntr->Parent_func_scope()->Addr_datum(node->Addr_datum_id());
    STMT_PTR new_st = ctx->Cntr()->New_st(new_rhs, new_addr, node->Spos());
    return RETV(new_st->Node());
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_stp(VISITOR* visitor, air::base::NODE_PTR node) {
    NODE_PTR new_rhs = Handle_store_node<RETV>(visitor, node);

    INSERTER_CTX* ctx      = &visitor->Context();
    CONTAINER*    cntr     = ctx->Cntr();
    PREG_PTR      new_preg = cntr->Parent_func_scope()->Preg(node->Preg_id());
    STMT_PTR new_stp = ctx->Cntr()->New_stp(new_rhs, new_preg, node->Spos());
    return RETV(new_stp->Node());
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ist(VISITOR* visitor, air::base::NODE_PTR node) {
    NODE_PTR new_rhs = Handle_store_node<RETV>(visitor, node);

    INSERTER_CTX* ctx       = &visitor->Context();
    CONTAINER*    cntr      = ctx->Cntr();
    NODE_PTR      addr_node = cntr->Clone_node(node->Child(0));
    STMT_PTR new_ist = ctx->Cntr()->New_ist(addr_node, new_rhs, node->Spos());
    return RETV(new_ist->Node());
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_do_loop(VISITOR* visitor, NODE_PTR node) {
    INSERTER_CTX* ctx = &visitor->Context();
    // 1. handle do_loop
    NODE_PTR new_do_loop = ctx->Handle_node<RETV>(visitor, node).Node();

    // 2. handle phi_list. insert bootstrap for phi result.
    const SSA_CONTAINER*  ssa_cntr = ctx->Ssa_cntr();
    air::opt::PHI_NODE_ID phi      = ssa_cntr->Node_phi(node->Id());
    air::opt::PHI_LIST    phi_list(ssa_cntr, phi);

    auto handle_phi = [](air::opt::PHI_NODE_PTR phi, INSERTER_CTX* ctx) {
      air::opt::SSA_VER_PTR ver = phi->Result();
      ctx->Handle_ssa_ver(ver);
    };
    phi_list.For_each(handle_phi, ctx);

    return RETV(new_do_loop);
  }
};

//! @brief Handler of CKKS domain operation
class CKKS_BTS_INSERTER_IMPL : public DEFAULT_HANDLER {
public:
  using NODE_PTR      = air::base::NODE_PTR;
  using STMT_PTR      = air::base::STMT_PTR;
  using SSA_CONTAINER = air::opt::SSA_CONTAINER;

  template <typename RETV, typename VISITOR>
  RETV Handle_add(VISITOR* visitor, NODE_PTR node) {
    return Handle_bin_arith_node<RETV>(visitor, node);
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_mul(VISITOR* visitor, NODE_PTR node) {
    return Handle_bin_arith_node<RETV>(visitor, node);
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_sub(VISITOR* visitor, NODE_PTR node) {
    return Handle_bin_arith_node<RETV>(visitor, node);
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_rotate(VISITOR* visitor, NODE_PTR node) {
    return Handle_bin_arith_node<RETV>(visitor, node);
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_relin(VISITOR* visitor, NODE_PTR node) {
    return Handle_bin_arith_node<RETV>(visitor, node);
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_bin_arith_node(VISITOR* visitor, NODE_PTR node) {
    INSERTER_CTX* ctx = &visitor->Context();
    NODE_PTR      new_node =
        ctx->Cntr()->New_cust_node(node->Opcode(), node->Rtype(), node->Spos());
    for (uint32_t id = 0; id < node->Num_child(); ++id) {
      NODE_PTR new_child =
          visitor->template Visit<RETV>(node->Child(id)).Node();
      // store bootstrap node in a preg
      if (new_child->Opcode() == OPC_BOOTSTRAP) {
        PREG_PTR preg =
            ctx->Cntr()->Parent_func_scope()->New_preg(new_child->Rtype());
        STMT_PTR stp = ctx->Cntr()->New_stp(new_child, preg, node->Spos());
        new_child    = ctx->Cntr()->New_ldp(preg, node->Spos());
        ctx->Prepend(stp);

        ctx->Trace_bts_tm(stp);
        ctx->Resbm_ctx()->Trace(TD_RESBM_BTS_INSERT, "Insert bootstrap:\n");
        ctx->Resbm_ctx()->Trace_obj(TD_RESBM_BTS_INSERT, stp);
      }
      new_node->Set_child(id, new_child);
    }
    new_node = ctx->Handle_expr(node->Id(), new_node);
    return RETV(new_node);
  }
};

//! @brief SMO_BTS_INSERTER inserts rescale/bootstrap to manage scale and level
//! of ciphertexts. SMO_BTS_INSERTER involves 2 steps:
//! 1. Collect bootstrap/rescale points for each callsite.
//! 2. For the entry function, insert bootstrap/rescale in the original function
//! body.
//!    For called functions, clone each function for each bootstrap/rescale plan
//!    and replace the original function with the cloned version.
class SMO_BTS_INSERTER {
public:
  using FUNC_ID          = air::base::FUNC_ID;
  using FUNC_SCOPE       = air::base::FUNC_SCOPE;
  using GLOB_SCOPE       = air::base::GLOB_SCOPE;
  using NODE_ID          = air::base::NODE_ID;
  using SSA_VER_PTR      = air::opt::SSA_VER_PTR;
  using DFG_NODE_ID      = air::opt::DFG_NODE_ID;
  using DFG_NODE_PTR     = air::opt::DFG_NODE_PTR;
  using FORMAL_SCALE_LEV = std::map<CALLSITE_INFO, VAR_SCALE_LEV::VEC>;
  using CALLSITE_BTS     = std::map<CALLSITE_INFO, BTS_SET>;
  using CALLSITE_RS      = std::map<CALLSITE_INFO, RS_SET>;
  using SMO_BTS_PLAN     = std::pair<BTS_SET, RS_SET>;
  using PLAN2FUNC        = std::map<SMO_BTS_PLAN, FUNC_ID>;
  using CORE_HANDLER     = air::core::HANDLER<CORE_BTS_INSERTER_IMPL>;
  using CKKS_HANDLER     = HANDLER<CKKS_BTS_INSERTER_IMPL>;
  using VISITOR = air::base::VISITOR<INSERTER_CTX, CORE_HANDLER, CKKS_HANDLER>;

  SMO_BTS_INSERTER(RESBM_CTX* resbm_ctx, GLOB_SCOPE* glob_scope,
                   core::LOWER_CTX* lower_ctx)
      : _resbm_ctx(resbm_ctx), _glob_scope(glob_scope), _lower_ctx(lower_ctx) {}
  ~SMO_BTS_INSERTER() {}

  //! @brief Perform bootstrap/rescale inserter.
  void Perform();

private:
  // REQUIRED UNDEFINED UNWANTED methods
  SMO_BTS_INSERTER(void);
  SMO_BTS_INSERTER(const SMO_BTS_INSERTER&);
  SMO_BTS_INSERTER& operator=(const SMO_BTS_INSERTER&);

  //! @brief Collect scale/level of formals for each callsite.
  void Collect_formal_scale_level(const MIN_LATENCY_PLAN* plan);
  //! @brief Collect bootstrap point and resulting level of region element
  void Collect_bootstrap_point(REGION_ELEM_ID elem, uint32_t bts_lev,
                               const ELEM_BTS_INFO::SET& redund_bts);
  //! @brief Collect bootstrap points in plan.
  void Collect_bootstrap_point(const MIN_LATENCY_PLAN* plan,
                               ELEM_BTS_INFO::SET&     redund_bts);
  //! @brief Collect rescale points in plan.
  void Collect_rescale_point(const MIN_LATENCY_PLAN* plan);
  //! @brief Traverse each scale/bootstrap management plan, and collect all the
  //! bootstrap and rescale points. Store bootstrap points in _bts_set,
  //! rescale points in _rs_set, and formal scale/level in
  void Collect_rescale_bts_point();

  //! @brief Insert bootstrap for a SSA_VER. Create a bootstrap node,
  //! and store the bootstrap result in the SSA_VER's symbol.
  void Bootstrap_ssa_ver(INSERTER_CTX& ctx, SSA_VER_PTR ver, uint32_t bts_lev,
                         bool need_rescale);
  //! @brief Insert bootstrap for an EXPR. Bootstrap the result of expr,
  //! and replace expr with the new bootstrap node.
  void Bootstrap_expr(INSERTER_CTX& ctx, DFG_NODE_PTR dfg_node,
                      uint32_t bts_lev, bool need_rescale);
  //! @brief Insert bootstrap for entry function.
  void Bootstrap_entry_func(FUNC_ID func, const RS_SET& rs_set,
                            const BTS_SET& bts_set);

  //! @brief Insert rescale for a SSA_VER. Create a rescale node,
  //! and store the rescale result in the SSA_VER's symbol.
  void Rescale_ssa_ver(INSERTER_CTX& ctx, SSA_VER_PTR ver);
  //! @brief Insert rescale for an EXPR. Create a rescale node, and
  //! replace the expr with the new rescale node.
  void Rescale_expr(INSERTER_CTX& ctx, DFG_NODE_PTR dfg_node);
  //! @brief Insert rescale for entry function.
  void Rescale_entry_func(FUNC_ID func, const RS_SET& rs_set,
                          const BTS_SET& bts_set);

  //! @brief Insert bootstrap in cloned function, and return FUNC_PTR.
  FUNC_PTR Planed_func(FUNC_ID func, const BTS_SET& bts_set,
                       const RS_SET& rs_set);
  //! @brief Set scale and level for parameters at callsite.
  void Set_param_scale_lev(const CALLSITE_INFO& callsite, NODE_PTR param,
                           uint32_t formal_id);
  //! @brief Insert rescale and bootstrap in entry function.
  void Handle_entry_func(const BTS_SET& bts_set, const RS_SET& rs_set);
  //! @brief Insert rescale and bootstrap in called function.
  void Handle_called_func(const CALLSITE_INFO& callsite, const BTS_SET& bts_set,
                          const RS_SET& rs_set);
  void Insert_smo_bootstrap();

  FUNC_ID Plan_func(const BTS_SET& bts_set, const RS_SET& rs_set) {
    return _plan_func[{bts_set, rs_set}];
  }
  void Set_plan_func(const BTS_SET& bts_set, const RS_SET& rs_set,
                     FUNC_ID func) {
    AIR_ASSERT(!func.Is_null());
    _plan_func[{bts_set, rs_set}] = func;
  }
  const CALLSITE_BTS& Callsite_bts(void) const { return _callsite_bts; }
  CALLSITE_BTS&       Callsite_bts(void) { return _callsite_bts; }
  BTS_SET&            Bts_set(const CALLSITE_INFO& callsite) {
    std::pair<CALLSITE_BTS::iterator, bool> res =
        _callsite_bts.emplace(callsite, BTS_SET());
    return res.first->second;
  }

  const CALLSITE_RS& Callsite_rs(void) const { return _callsite_rs; }
  CALLSITE_RS&       Callsite_rs(void) { return _callsite_rs; }
  RS_SET&            Rs_set(const CALLSITE_INFO& callsite) {
    return _callsite_rs[callsite];
  }

  FORMAL_SCALE_LEV&   Formal_scale_info(void) { return _formal_scale_lev; }
  VAR_SCALE_LEV::VEC& Formal_scale_info(const CALLSITE_INFO& callsite) {
    return _formal_scale_lev[callsite];
  }
  RESBM_CTX*       Resbm_ctx() const { return _resbm_ctx; }
  GLOB_SCOPE*      Glob_scope() { return _glob_scope; }
  core::LOWER_CTX* Lower_ctx() { return _lower_ctx; }

  RESBM_CTX*       _resbm_ctx  = nullptr;
  core::LOWER_CTX* _lower_ctx  = nullptr;
  GLOB_SCOPE*      _glob_scope = nullptr;
  CALLSITE_BTS     _callsite_bts;
  CALLSITE_RS      _callsite_rs;
  FORMAL_SCALE_LEV _formal_scale_lev;  // scale and level of formal
  PLAN2FUNC        _plan_func;         // function cloned for rescale/bts plan
};

}  // namespace ckks
}  // namespace fhe

#endif  // FHE_CKKS_SMO_BOOTSTRAP_INSERTER_H
