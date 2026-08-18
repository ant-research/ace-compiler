//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_BOOTSTRAP_INSERTER_H
#define FHE_CKKS_BOOTSTRAP_INSERTER_H

#include <list>
#include <set>

#include "air/base/analyze_ctx.h"
#include "air/base/container.h"
#include "air/base/container_decl.h"
#include "air/base/ptr_wrapper.h"
#include "air/base/st.h"
#include "air/base/st_decl.h"
#include "air/base/st_enum.h"
#include "air/base/transform_ctx.h"
#include "air/core/default_handler.h"
#include "air/core/opcode.h"
#include "air/opt/dfg_data.h"
#include "air/opt/ssa_container.h"
#include "air/opt/ssa_decl.h"
#include "air/opt/ssa_node_list.h"
#include "air/util/debug.h"
#include "dfg_region.h"
#include "fhe/ckks/ckks_gen.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/ckks/default_handler.h"
#include "fhe/core/lower_ctx.h"
#include "resbm_ctx.h"
namespace fhe {
namespace ckks {

class INSERTER_RETV {
public:
  INSERTER_RETV(NODE_PTR node) : _node(node) {}
  INSERTER_RETV(void) : _node() {}
  ~INSERTER_RETV() {}

  NODE_PTR Node() const { return _node; }

private:
  NODE_PTR _node;
};

class BTS_INFO {
public:
  using SSA_VER_ID = air::opt::SSA_VER_ID;
  using SET        = std::set<BTS_INFO>;
  BTS_INFO(NODE_ID expr, uint32_t lev)
      : _id(expr), _bts_lev(lev), _kind(air::opt::DATA_NODE_EXPR) {}
  BTS_INFO(SSA_VER_ID ver, uint32_t lev)
      : _id(ver), _bts_lev(lev), _kind(air::opt::DATA_NODE_SSA_VER) {}
  BTS_INFO(const BTS_INFO& o)
      : _id(o._id._val), _bts_lev(o._bts_lev), _kind(o._kind) {}
  ~BTS_INFO() {}

  bool Is_expr(void) const { return _kind == air::opt::DATA_NODE_EXPR; }
  bool Is_ssa_ver(void) const { return _kind == air::opt::DATA_NODE_SSA_VER; }
  NODE_ID    Expr(void) const { return _id._expr; }
  SSA_VER_ID Ssa_ver(void) const { return _id._ver; }
  uint32_t   Bts_lev(void) const { return _bts_lev; }

  bool operator<(const BTS_INFO& o) const {
    if (_id._val < o._id._val) return true;
    if (_id._val > o._id._val) return false;
    if (_kind < o._kind) return true;
    if (_kind > o._kind) return false;
    return _bts_lev < o._bts_lev;
  }
  bool operator==(const BTS_INFO& o) const {
    return _id._val == o._id._val && _kind == o._kind && _bts_lev == o._bts_lev;
  }

private:
  union BTS_ID {
    uint32_t   _val;
    NODE_ID    _expr;
    SSA_VER_ID _ver;
    BTS_ID(NODE_ID expr) : _expr(expr) {}
    BTS_ID(SSA_VER_ID ver) : _ver(ver) {}
    BTS_ID(uint32_t id) : _val(id) {}
  } _id;
  uint32_t                _bts_lev;
  air::opt::DFG_NODE_KIND _kind;
};

class BTS_POINT {
public:
  using SSA_VER_ID = air::opt::SSA_VER_ID;

  explicit BTS_POINT(FUNC_ID func) : _func(func) {}
  ~BTS_POINT() {}

  // const std::set<NODE_ID>&    Bts_expr(void) const { return _bts_expr; }
  // std::set<NODE_ID>&          Bts_expr(void) { return _bts_expr; }
  // const std::set<SSA_VER_ID>& Bts_ver(void) const { return _bts_ver; }
  // std::set<SSA_VER_ID>&       Bts_ver(void) { return _bts_ver; }
  const BTS_INFO::SET& Bts_info(void) const { return _bts_info; }
  BTS_INFO::SET&       Bts_info(void) { return _bts_info; }
  FUNC_ID              Func(void) const { return _func; }
  void                 Add_expr(NODE_ID expr, uint32_t lev) {
    _bts_info.insert(BTS_INFO(expr, lev));
  }
  void Add_ver(SSA_VER_ID ver, uint32_t lev) {
    _bts_info.insert(BTS_INFO(ver, lev));
  }
  bool operator==(const BTS_POINT& o) {
    return _func == o.Func() && _bts_info == o.Bts_info();
  }
  bool operator<(const BTS_POINT& o) const {
    if (_func < o.Func()) return true;
    if (_func > o.Func()) return false;
    return _bts_info < o._bts_info;
  }

private:
  FUNC_ID       _func;
  BTS_INFO::SET _bts_info;
};

class BTS_INSERTER_CTX : public air::base::TRANSFORM_CTX {
public:
  using NODE_ID       = air::base::NODE_ID;
  using NODE_PTR      = air::base::NODE_PTR;
  using STMT_PTR      = air::base::STMT_PTR;
  using SSA_CONTAINER = air::opt::SSA_CONTAINER;
  using SSA_VER_ID    = air::opt::SSA_VER_ID;
  using SSA_VER_PTR   = air::opt::SSA_VER_PTR;
  using SSA_SYM_PTR   = air::opt::SSA_SYM_PTR;
  using GLOB_SCOPE    = air::base::GLOB_SCOPE;

  BTS_INSERTER_CTX(BTS_POINT bts_point, const SSA_CONTAINER* ssa_cntr,
                   CONTAINER* new_cntr, core::LOWER_CTX* lower_ctx,
                   RESBM_CTX* ctx)
      : air::base::TRANSFORM_CTX(new_cntr),
        _bts_point(bts_point),
        _ssa_cntr(ssa_cntr),
        _new_cntr(new_cntr),
        _lower_ctx(lower_ctx),
        _ctx(ctx) {}

  BTS_INFO::SET::const_iterator Bootstrap_info(NODE_ID node) const {
    for (BTS_INFO::SET::const_iterator iter = _bts_point.Bts_info().begin();
         iter != _bts_point.Bts_info().end(); ++iter) {
      if (iter->Is_expr() && iter->Expr() == node) return iter;
    }
    return _bts_point.Bts_info().end();
  }
  BTS_INFO::SET::const_iterator Bootstrap_info(SSA_VER_ID ver) const {
    for (BTS_INFO::SET::const_iterator iter = _bts_point.Bts_info().begin();
         iter != _bts_point.Bts_info().end(); ++iter) {
      if (iter->Is_ssa_ver() && iter->Ssa_ver() == ver) return iter;
    }
    return _bts_point.Bts_info().end();
  }
  const BTS_INFO::SET& Bootstrap_info(void) const {
    return _bts_point.Bts_info();
  }

  void Erase_bts_point(BTS_INFO::SET::const_iterator iter) {
    _bts_point.Bts_info().erase(iter);
  }

  NODE_PTR Bootstrap_node(NODE_PTR node, uint32_t bts_lev) {
    CKKS_GEN ckks_gen(_new_cntr, _lower_ctx);

    _ctx->Trace("Bootstrap node: ", node->To_str());
    NODE_PTR    bts_node = ckks_gen.Gen_bootstrap(node, node->Spos());
    const char* lev_attr_name =
        Lower_ctx()->Attr_name(core::FHE_ATTR_KIND::LEVEL);
    bts_lev += 1;
    bts_node->Set_attr(lev_attr_name, &bts_lev, 1);
    return bts_node;
  }

  STMT_PTR Bootstrap_ssa_ver(SSA_VER_PTR ssa_ver, uint32_t bts_lev) {
    SSA_SYM_PTR sym  = Ssa_cntr()->Sym(ssa_ver->Sym_id());
    SPOS        spos = Glob_scope()->Unknown_simple_spos();
    if (sym->Is_addr_datum()) {
      ADDR_DATUM_PTR addr_datum =
          Cntr()->Parent_func_scope()->Addr_datum(ADDR_DATUM_ID(sym->Var_id()));
      if (Lower_ctx()->Is_cipher_type(sym->Type_id())) {
        NODE_PTR ld       = Cntr()->New_ld(addr_datum, spos);
        NODE_PTR bts_node = Bootstrap_node(ld, bts_lev);
        STMT_PTR st       = Cntr()->New_st(bts_node, addr_datum, spos);
        return st;
      } else {
        TYPE_PTR type = Glob_scope()->Type(sym->Type_id());
        AIR_ASSERT(type->Is_array());
        ARRAY_TYPE_PTR arr_type = type->Cast_to_arr();
        AIR_ASSERT(arr_type->Elem_count() == 1 && arr_type->Dim() == 1);
        NODE_PTR lda = Cntr()->New_lda(addr_datum, POINTER_KIND::FLAT32, spos);
        NODE_PTR array = Cntr()->New_array(lda, 1, spos);
        array->Set_child(
            1, Cntr()->New_intconst(PRIMITIVE_TYPE::INT_U32, 0, spos));
        NODE_PTR ild = Cntr()->New_ild(array, spos);
        NODE_PTR bts = Bootstrap_node(ild, bts_lev);
        STMT_PTR ist =
            Cntr()->New_ist(Cntr()->Clone_node_tree(array), bts, spos);
        return ist;
      }
    } else if (sym->Is_preg()) {
      AIR_ASSERT(Lower_ctx()->Is_cipher_type(sym->Type_id()));
      PREG_PTR preg = Cntr()->Parent_func_scope()->Preg(PREG_ID(sym->Var_id()));
      NODE_PTR ldp  = Cntr()->New_ldp(preg, spos);
      NODE_PTR bts_node = Bootstrap_node(ldp, bts_lev);
      STMT_PTR stp      = Cntr()->New_stp(bts_node, preg, spos);
      return stp;
    } else {
      AIR_ASSERT_MSG(false, "not supported ssa symbol kind");
      return STMT_PTR();
    }
  }

  STMT_PTR Bts_tm_start() {
    const char*  msg = "Bootstrap_start";
    CONSTANT_PTR msg_cst =
        Glob_scope()->New_const(CONSTANT_KIND::STR_ARRAY, msg, strlen(msg));
    STMT_PTR tm_start =
        Cntr()->New_tm_start(msg_cst, Glob_scope()->Unknown_simple_spos());
    return tm_start;
  }

  STMT_PTR Bts_tm_taken() {
    const char*  msg = "Bootstrap_end";
    CONSTANT_PTR msg_cst =
        Glob_scope()->New_const(CONSTANT_KIND::STR_ARRAY, msg, strlen(msg));
    STMT_PTR tm_taken =
        Cntr()->New_tm_taken(msg_cst, Glob_scope()->Unknown_simple_spos());
    return tm_taken;
  }
  bool     Have_bootstraped_all(void) { return _bts_point.Bts_info().empty(); }
  CKKS_GEN Ckks_gen(void) const { return CKKS_GEN(Cntr(), Lower_ctx()); }
  CONTAINER*           Cntr(void) const { return _new_cntr; }
  const SSA_CONTAINER* Ssa_cntr(void) const { return _ssa_cntr; }
  GLOB_SCOPE*      Glob_scope(void) const { return _ssa_cntr->Glob_scope(); }
  core::LOWER_CTX* Lower_ctx(void) const { return _lower_ctx; }

private:
  const SSA_CONTAINER* _ssa_cntr  = nullptr;
  core::LOWER_CTX*     _lower_ctx = nullptr;
  CONTAINER*           _new_cntr  = nullptr;
  RESBM_CTX*           _ctx       = nullptr;
  BTS_POINT            _bts_point;
};

class CORE_BTS_INSERTER_IMPL : public air::core::DEFAULT_HANDLER {
public:
  using NODE_PTR      = air::base::NODE_PTR;
  using STMT_PTR      = air::base::STMT_PTR;
  using SSA_VER_ID    = air::opt::SSA_VER_ID;
  using SSA_VER_PTR   = air::opt::SSA_VER_PTR;
  using SSA_CONTAINER = air::opt::SSA_CONTAINER;

  template <typename RETV, typename VISITOR>
  RETV Handle_func_entry(VISITOR* visitor, NODE_PTR entry) {
    BTS_INSERTER_CTX* ctx = &visitor->Context();

    // 1. process function body
    uint32_t child_cnt = entry->Num_child();
    AIR_ASSERT(child_cnt > 1);
    uint32_t func_body_id = child_cnt - 1;
    NODE_PTR old_body     = entry->Child(func_body_id);
    AIR_ASSERT(old_body->Is_block());
    NODE_PTR new_body       = visitor->template Visit<RETV>(old_body).Node();
    NODE_PTR new_entry_node = ctx->Cntr()->Entry_node();
    new_entry_node->Set_child(func_body_id, new_body);

    // 2. process idname
    for (uint32_t id = 0; id < func_body_id; ++id) {
      NODE_PTR new_node =
          visitor->template Visit<RETV>(entry->Child(id)).Node();
      new_entry_node->Set_child(id, new_node);
    }
    return RETV(new_entry_node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_idname(VISITOR* visitor, air::base::NODE_PTR idname) {
    BTS_INSERTER_CTX* ctx = &visitor->Context();
    SSA_VER_PTR       ver = ctx->Ssa_cntr()->Node_ver(idname->Id());

    BTS_INFO::SET::const_iterator iter = ctx->Bootstrap_info(ver->Id());
    if (iter != ctx->Bootstrap_info().end()) {
      STMT_PTR bts_stmt = ctx->Bootstrap_ssa_ver(ver, iter->Bts_lev());
      ctx->Cntr()->Stmt_list().Prepend(bts_stmt);
      ctx->Erase_bts_point(iter);
    }

    NODE_PTR new_idname = ctx->Cntr()->Clone_node(idname);
    return RETV(new_idname);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_st(VISITOR* visitor, air::base::NODE_PTR st) {
    NODE_PTR new_rhs = visitor->template Visit<RETV>(st->Child(0)).Node();

    BTS_INSERTER_CTX*             ctx  = &visitor->Context();
    SSA_VER_PTR                   ver  = ctx->Ssa_cntr()->Node_ver(st->Id());
    BTS_INFO::SET::const_iterator iter = ctx->Bootstrap_info(ver->Id());
    if (iter != ctx->Bootstrap_info().end()) {
      STMT_PTR bts_stmt = ctx->Bootstrap_ssa_ver(ver, iter->Bts_lev());
      ctx->Append(bts_stmt);
      ctx->Erase_bts_point(iter);
    }

    CONTAINER*     cntr = ctx->Cntr();
    ADDR_DATUM_PTR new_addr =
        cntr->Parent_func_scope()->Addr_datum(st->Addr_datum_id());
    STMT_PTR new_st = ctx->Cntr()->New_st(new_rhs, new_addr, st->Spos());
    return RETV(new_st->Node());
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_stp(VISITOR* visitor, air::base::NODE_PTR stp) {
    NODE_PTR new_rhs = visitor->template Visit<RETV>(stp->Child(0)).Node();

    BTS_INSERTER_CTX*             ctx  = &visitor->Context();
    SSA_VER_PTR                   ver  = ctx->Ssa_cntr()->Node_ver(stp->Id());
    BTS_INFO::SET::const_iterator iter = ctx->Bootstrap_info(ver->Id());
    if (iter != ctx->Bootstrap_info().end()) {
      STMT_PTR bts_stmt = ctx->Bootstrap_ssa_ver(ver, iter->Bts_lev());
      ctx->Append(bts_stmt);
      ctx->Erase_bts_point(iter);
    }

    CONTAINER* cntr     = ctx->Cntr();
    PREG_PTR   new_preg = cntr->Parent_func_scope()->Preg(stp->Preg_id());
    STMT_PTR   new_stp  = ctx->Cntr()->New_stp(new_rhs, new_preg, stp->Spos());
    return RETV(new_stp->Node());
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ist(VISITOR* visitor, air::base::NODE_PTR ist) {
    NODE_PTR new_rhs = visitor->template Visit<RETV>(ist->Child(1)).Node();

    BTS_INSERTER_CTX*             ctx      = &visitor->Context();
    const SSA_CONTAINER*          ssa_cntr = ctx->Ssa_cntr();
    air::opt::CHI_NODE_ID         chi      = ssa_cntr->Node_chi(ist->Id());
    air::opt::SSA_VER_PTR         ver      = ssa_cntr->Chi_node(chi)->Result();
    BTS_INFO::SET::const_iterator iter     = ctx->Bootstrap_info(ver->Id());
    if (iter != ctx->Bootstrap_info().end()) {
      STMT_PTR bts_stmt = ctx->Bootstrap_ssa_ver(ver, iter->Bts_lev());
      ctx->Append(bts_stmt);
      ctx->Erase_bts_point(iter);
    }

    CONTAINER* cntr      = ctx->Cntr();
    NODE_PTR   addr_node = cntr->Clone_node(ist->Child(0));
    STMT_PTR   new_ist = ctx->Cntr()->New_ist(addr_node, new_rhs, ist->Spos());
    return RETV(new_ist->Node());
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_do_loop(VISITOR* visitor, NODE_PTR do_loop) {
    BTS_INSERTER_CTX* ctx = &visitor->Context();
    // 1. handle do_loop
    NODE_PTR new_do_loop = ctx->Handle_node<RETV>(visitor, do_loop).Node();

    // 2. handle phi_list. insert bootstrap for phi result.
    const SSA_CONTAINER*  ssa_cntr = ctx->Ssa_cntr();
    air::opt::PHI_NODE_ID phi      = ssa_cntr->Node_phi(do_loop->Id());
    air::opt::PHI_LIST    phi_list(ssa_cntr, phi);

    auto handle_phi = [](air::opt::PHI_NODE_PTR phi, BTS_INSERTER_CTX* ctx) {
      air::opt::SSA_VER_PTR         ver  = phi->Result();
      BTS_INFO::SET::const_iterator iter = ctx->Bootstrap_info(ver->Id());
      if (iter == ctx->Bootstrap_info().end()) return;
      STMT_PTR bts_stmt = ctx->Bootstrap_ssa_ver(ver, iter->Bts_lev());
      ctx->Append(bts_stmt);
      ctx->Erase_bts_point(iter);
    };
    phi_list.For_each(handle_phi, ctx);

    return RETV(new_do_loop);
  }
};

class CKKS_BTS_INSERTER_IMPL : public DEFAULT_HANDLER {
public:
  using NODE_PTR      = air::base::NODE_PTR;
  using STMT_PTR      = air::base::STMT_PTR;
  using SSA_CONTAINER = air::opt::SSA_CONTAINER;

  template <typename RETV, typename VISITOR>
  RETV Handle_add(VISITOR* visitor, NODE_PTR add) {
    return Handle_bin_arith_node<RETV>(visitor, add);
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_mul(VISITOR* visitor, NODE_PTR mul) {
    return Handle_bin_arith_node<RETV>(visitor, mul);
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_sub(VISITOR* visitor, NODE_PTR sub) {
    return Handle_bin_arith_node<RETV>(visitor, sub);
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_rotate(VISITOR* visitor, NODE_PTR rotate) {
    return Handle_bin_arith_node<RETV>(visitor, rotate);
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_relin(VISITOR* visitor, NODE_PTR relin) {
    return Handle_bin_arith_node<RETV>(visitor, relin);
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_bin_arith_node(VISITOR* visitor, NODE_PTR ckks_node) {
    BTS_INSERTER_CTX* ctx      = &visitor->Context();
    NODE_PTR          new_node = ctx->Cntr()->New_cust_node(
        ckks_node->Opcode(), ckks_node->Rtype(), ckks_node->Spos());
    for (uint32_t id = 0; id < ckks_node->Num_child(); ++id) {
      NODE_PTR new_child =
          visitor->template Visit<RETV>(ckks_node->Child(id)).Node();
      new_node->Set_child(id, new_child);
    }

    BTS_INFO::SET::const_iterator iter = ctx->Bootstrap_info(ckks_node->Id());
    if (iter != ctx->Bootstrap_info().end()) {
      NODE_PTR parent_node = ctx->Parent(1);
      if (parent_node->Opcode() == air::core::OPC_ST ||
          parent_node->Opcode() == air::core::OPC_STP) {
        air::opt::SSA_VER_PTR ver =
            ctx->Ssa_cntr()->Node_ver(parent_node->Id());
        STMT_PTR bts_stmt = ctx->Bootstrap_ssa_ver(ver, iter->Bts_lev());
        ctx->Append(bts_stmt);
      } else {
        new_node = ctx->Bootstrap_node(new_node, iter->Bts_lev());
      }
      ctx->Erase_bts_point(iter);
    }
    return RETV(new_node);
  }
};

class BOOTSTRAP_INSERTER {
public:
  using FUNC_ID           = air::base::FUNC_ID;
  using FUNC_SCOPE        = air::base::FUNC_SCOPE;
  using GLOB_SCOPE        = air::base::GLOB_SCOPE;
  using NODE_ID           = air::base::NODE_ID;
  using SSA_VER_PTR       = air::opt::SSA_VER_PTR;
  using DFG_NODE_ID       = air::opt::DFG_NODE_ID;
  using DFG_NODE_PTR      = air::opt::DFG_NODE_PTR;
  using VAR_SCALE_INFO    = MIN_LATENCY_PLAN::VAR_SCALE_INFO;
  using FORMAL_SCALE_INFO = MIN_LATENCY_PLAN::FORMAL_SCALE_INFO;

  BOOTSTRAP_INSERTER(RESBM_CTX* resbm_ctx, GLOB_SCOPE* glob_scope,
                     core::LOWER_CTX* lower_ctx)
      : _resbm_ctx(resbm_ctx), _glob_scope(glob_scope), _lower_ctx(lower_ctx) {}

  void Perform();

private:
  FORMAL_SCALE_INFO& Formal_scale_info(void) { return _formal_scale_info; }
  void               Collect_bootstrap_points();

  //! @brief insert bootstrap for a SSA_VER.
  void Insert_bootstrap_ver(BTS_INSERTER_CTX& ctx, SSA_VER_PTR ver,
                            uint32_t bts_lev);
  //! @brief insert bootstrap for an EXPR.
  void Insert_bootstrap_expr(BTS_INSERTER_CTX& ctx, DFG_NODE_PTR dfg_node,
                             uint32_t bts_lev);
  //! @brief insert bootstrap for a function.
  void Insert_bootstrap_orig_func(FUNC_ID func, const BTS_POINT& bts_point);
  //! @brief insert bootstrap in cloned function.
  FUNC_PTR Insert_bootstrap_clone_func(FUNC_ID          func,
                                       const BTS_POINT& bts_point);
  void     Set_param_scale_info(const CALLSITE_INFO& call_site, NODE_PTR param,
                                uint32_t formal_id);
  void     Insert_bootstrap_points();
  FUNC_SCOPE* Gen_new_func(FUNC_ID func, const char* suffix);
  void       Add_bootstrap_point(FUNC_ID func, NODE_ID call_node, NODE_ID node);
  RESBM_CTX* Resbm_ctx() const { return _resbm_ctx; }
  GLOB_SCOPE*      Glob_scope() { return _glob_scope; }
  core::LOWER_CTX* Lower_ctx() { return _lower_ctx; }

  RESBM_CTX*                                            _resbm_ctx;
  core::LOWER_CTX*                                      _lower_ctx;
  GLOB_SCOPE*                                           _glob_scope;
  std::map<FUNC_ID, std::map<CALLSITE_INFO, BTS_POINT>> _bts_point;
  // std::map<CALLSITE_INFO, BTS_POINT>                    _bts_point;
  std::map<CALLSITE_INFO, std::list<VAR_SCALE_INFO>> _formal_scale_info;
  std::map<BTS_POINT, FUNC_ID>                       _processed_bts_point;
};

}  // namespace ckks
}  // namespace fhe

#endif  // FHE_CKKS_BOOTSTRAP_INSERTER_H