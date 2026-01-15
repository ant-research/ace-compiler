//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================
#ifndef FHE_POLY_HPOLY_ATTR_PROP_H
#define FHE_POLY_HPOLY_ATTR_PROP_H

#include "air/base/analyze_ctx.h"
#include "air/base/visitor.h"
#include "air/core/default_handler.h"
#include "air/core/handler.h"
#include "air/driver/driver_ctx.h"
#include "air/opt/ssa_container.h"
#include "air/opt/ssa_node_list.h"
#include "fhe/ckks/ckks_handler.h"
#include "fhe/ckks/invalid_handler.h"
#include "fhe/core/lower_ctx.h"
#include "fhe/poly/config.h"
#include "fhe/poly/default_handler.h"
#include "fhe/poly/handler.h"
#include "fhe/poly/ir_gen.h"

namespace fhe {
namespace poly {

class ATTR_PROP_CTX;
class CORE_ATTR_PROP;
class CKKS_ATTR_PROP;
class HPOLY_ATTR_PROP;
using ATTR_VISITOR =
    air::base::VISITOR<ATTR_PROP_CTX, air::core::HANDLER<CORE_ATTR_PROP>,
                       fhe::ckks::HANDLER<CKKS_ATTR_PROP>,
                       fhe::poly::HANDLER<HPOLY_ATTR_PROP>>;

class ATTR_INFO {
public:
  static constexpr uint32_t NOT_SET    = (uint32_t)-1;
  static constexpr uint32_t DEF_NUM_P  = 0;
  static constexpr uint32_t DEF_SBASE  = 0;
  static constexpr uint32_t DEF_SF_DEG = 1;
  ATTR_INFO() {}
  ATTR_INFO(uint32_t num_q, uint32_t num_p = DEF_NUM_P,
            uint32_t sbase = DEF_SBASE, uint32_t sf_deg = DEF_SF_DEG)
      : _num_q(num_q), _num_p(num_p), _sbase(sbase), _sf_deg(sf_deg) {}

  ~ATTR_INFO() {}

  bool Is_valid() const { return (_num_q != NOT_SET); }

  //! @brief default rules of merging attribute
  void Join(const ATTR_INFO& other) {
    if (!Is_valid()) {
      _num_q  = other._num_q;
      _num_p  = other._num_p;
      _sbase  = other._sbase;
      _sf_deg = other._sf_deg;
      return;
    }
    AIR_ASSERT_MSG(_num_q == other._num_q, "need user to defined prop rule");
    AIR_ASSERT_MSG(_num_p == other._num_p, "need user to defined prop rule");
    AIR_ASSERT_MSG(_sbase == other._sbase, "need user to defined prop rule");
    AIR_ASSERT_MSG(_sf_deg == other._sf_deg, "need user to defined prop rule");
  }

  void Set_num_q(uint32_t num_q) { _num_q = num_q; }
  void Set_num_p(uint32_t num_p) { _num_p = num_p; }
  void Set_sbase(uint32_t sbase) { _sbase = sbase; }
  void Set_sf_deg(uint32_t sf) { _sf_deg = sf; }

  uint32_t Num_q(void) const { return _num_q; }
  uint32_t Num_p(void) const { return _num_p; }
  uint32_t Sbase(void) const { return _sbase; }
  uint32_t Sf_deg(void) const { return _sf_deg; }

private:
  uint32_t _num_q  = NOT_SET;     // number of q primes
  uint32_t _num_p  = DEF_NUM_P;   // number of p primes
  uint32_t _sbase  = DEF_SBASE;   // start RNS basis
  uint32_t _sf_deg = DEF_SF_DEG;  // scaling factor degree
};

class ATTR_PROP_CTX : public air::base::ANALYZE_CTX {
public:
  using SSA_ATTR_PAIR = std::pair<uint32_t, ATTR_INFO>;
  using SSA_ATTR_ALLOCATOR =
      air::util::CXX_MEM_ALLOCATOR<SSA_ATTR_PAIR, POLY_MEM_POOL>;
  using SSA_ATTR_MAP =
      std::map<uint32_t, ATTR_INFO, std::less<uint32_t>, SSA_ATTR_ALLOCATOR>;

  ATTR_PROP_CTX(const POLY_CONFIG& config, air::driver::DRIVER_CTX* driver_ctx,
                fhe::core::LOWER_CTX* lower_ctx, air::base::CONTAINER* cntr,
                air::opt::SSA_CONTAINER* ssa_cntr)
      : _driver_ctx(driver_ctx),
        _lower_ctx(lower_ctx),
        _config(config),
        _irgen(cntr, lower_ctx, &_pool),
        _ssa_cntr(ssa_cntr),
        _ssa_attr_map(SSA_ATTR_ALLOCATOR(&_pool)) {
    _pool.Push();
  }
  ~ATTR_PROP_CTX(void) { _pool.Pop(); }

  //! @brief Get lower context
  fhe::core::LOWER_CTX* Lower_ctx() const { return _lower_ctx; }

  air::driver::DRIVER_CTX* Driver_ctx() const { return _driver_ctx; }

  const POLY_CONFIG& Config() const { return _config; }

  fhe::poly::IR_GEN& Irgen() { return _irgen; }

  air::opt::SSA_CONTAINER* Ssa_cntr() const { return _ssa_cntr; };

  //! @brief Visit kid and prop attribute for child if needed
  template <typename RETV, typename VISITOR>
  RETV Handle_node(VISITOR* visitor, air::base::NODE_PTR node) {
    // push current node to ANALYZE_CTX's stack
    ANALYZE_CTX::Push(node);

    ATTR_INFO          retv;
    air::base::TYPE_ID tid =
        node->Has_rtype() ? node->Rtype_id()
                          : (node->Has_access_type() ? node->Access_type_id()
                                                     : air::base::TYPE_ID());
    for (uint32_t i = 0; i < node->Num_child(); ++i) {
      ATTR_INFO attr_child = visitor->template Visit<RETV>(node->Child(i));
      if (_irgen.Is_fhe_type(node->Child(i)->Rtype_id())) {
        retv.Join(attr_child);
      }
    }
    if (retv.Is_valid() && !tid.Is_null() && _irgen.Is_fhe_type(tid)) {
      Set_attr_info(node, retv);
    }
    // pop current node from ANALYZE_CTX's stack
    ANALYZE_CTX::Pop(node);
    return retv;
  }

  //! @brief default action for unknown domain
  template <typename RETV, typename VISITOR>
  RETV Handle_unknown_domain(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(!node->Is_block());
    return Handle_node<RETV>(visitor, node);
  }

  void Set_attr_info(air::base::NODE_PTR node, const ATTR_INFO& attr_info) {
    AIR_ASSERT(attr_info.Is_valid());
    _irgen.Set_num_q(node, attr_info.Num_q());

    if (attr_info.Num_p() > 0) {
      _irgen.Set_num_p(node, attr_info.Num_p());
    } else if (_irgen.Get_num_p(node) != 0) {
      // set num_p to zero if already have attribute
      _irgen.Set_num_p(node, attr_info.Num_p());
    }
    if (attr_info.Sbase() > 0) {
      _irgen.Set_sbase(node, attr_info.Sbase());
    }
    if (attr_info.Sf_deg() >= 1) {
      _irgen.Set_sf_deg(node, attr_info.Sf_deg());
    }
  }

  void Set_attr_info(air::opt::SSA_VER_ID ver, const ATTR_INFO& attr_info) {
    _ssa_attr_map[ver.Value()] = attr_info;
  }

  bool Has_attr_info(air::opt::SSA_VER_ID ver) const {
    SSA_ATTR_MAP::const_iterator iter = _ssa_attr_map.find(ver.Value());
    return iter != _ssa_attr_map.end();
  }

  const ATTR_INFO& Attr_info(air::opt::SSA_VER_ID ver) const {
    SSA_ATTR_MAP::const_iterator iter = _ssa_attr_map.find(ver.Value());
    AIR_ASSERT_MSG(iter != _ssa_attr_map.end(), "ssa attr info not set");
    return iter->second;
  }

  uint32_t Num_q_from_rns_range(uint32_t s_idx, uint32_t e_idx) const {
    AIR_ASSERT(s_idx <= e_idx);
    uint32_t tot_num_q = _lower_ctx->Get_ctx_param().Get_mul_level();
    if (s_idx < tot_num_q) {
      if (e_idx < tot_num_q) {
        return e_idx - s_idx + 1;
      } else {
        return tot_num_q - s_idx;
      }
    } else {
      return 0;
    }
  }

  uint32_t Num_p_from_rns_range(uint32_t s_idx, uint32_t e_idx) const {
    AIR_ASSERT(s_idx <= e_idx);
    // num_p either 0 or full p prime number
    if (e_idx == _lower_ctx->Get_ctx_param().Get_tot_prime_num() - 1) {
      return _lower_ctx->Get_ctx_param().Get_p_prime_num();
    } else {
      // num_p is zero, then there should be q primes
      AIR_ASSERT(s_idx < _lower_ctx->Get_ctx_param().Get_mul_level());
      AIR_ASSERT(e_idx < _lower_ctx->Get_ctx_param().Get_mul_level());
      return 0;
    }
  }

  void Prop_chi_list(air::base::NODE_PTR node, const ATTR_INFO& info) {
    air::opt::CHI_NODE_ID chi = Ssa_cntr()->Node_chi(node->Id());
    if (chi != air::base::Null_id) {
      auto visit = [](air::opt::CHI_NODE_PTR chi, ATTR_PROP_CTX& ctx,
                      const ATTR_INFO& attr_info) {
        air::opt::SSA_CONTAINER* ssa_cntr = ctx.Ssa_cntr();
        air::opt::SSA_VER_PTR    ver      = chi->Result();
        air::opt::SSA_SYM_PTR    sym      = ver->Sym();
        air::base::TYPE_ID       ty_id    = sym->Type_id();
        if (ctx.Lower_ctx()->Is_rns_poly_type(ty_id)) {
          ctx.Set_attr_info(ver->Id(), attr_info);
        }
      };
      air::opt::CHI_LIST list(Ssa_cntr(), chi);
      list.For_each(visit, *this, info);
    }
  }

  // add trace related api
  DECLARE_TRACE_DETAIL_API(_config, _driver_ctx)
private:
  POLY_MEM_POOL            _pool;
  fhe::poly::IR_GEN        _irgen;
  fhe::core::LOWER_CTX*    _lower_ctx;
  air::driver::DRIVER_CTX* _driver_ctx;
  const POLY_CONFIG&       _config;
  air::opt::SSA_CONTAINER* _ssa_cntr;
  SSA_ATTR_MAP             _ssa_attr_map;
};

class CKKS_ATTR_PROP : public fhe::ckks::INVALID_HANDLER {
public:
  //! @brief Construct a new CKKS_ATTR_PROP object
  CKKS_ATTR_PROP() {}

  //! @brief Prop attribute from encode node
  //! Note: CKKS.encode/CKKS.bootstrap are the only op that not lowered to
  //! POLY(shall we lower it?) let CKKS_ATTR_PROP inherit from INVALID_HANDLER
  //! to capture any unresolved IR
  template <typename RETV, typename VISITOR>
  RETV Handle_encode(VISITOR* visitor, air::base::NODE_PTR node) {
    IR_GEN&            irgen = visitor->Context().Irgen();
    air::base::TYPE_ID ty    = node->Rtype_id();
    AIR_ASSERT(irgen.Is_fhe_type(ty));
    // treat encode as leaf node, assume CKKS layer already annoate attribute
    ATTR_INFO ret(irgen.Get_num_q(node), irgen.Get_num_p(node),
                  irgen.Get_sbase(node));
    return ret;
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_bootstrap(VISITOR* visitor, air::base::NODE_PTR node) {
    IR_GEN&            irgen = visitor->Context().Irgen();
    air::base::TYPE_ID ty    = node->Rtype_id();
    AIR_ASSERT(irgen.Is_fhe_type(ty));
    // treat bootstrap as leaf node, assume CKKS layer already annoate attribute
    ATTR_INFO ret(irgen.Get_num_q(node), irgen.Get_num_p(node),
                  irgen.Get_sbase(node));
    return ret;
  }
};

class HPOLY_ATTR_PROP : public fhe::poly::DEFAULT_HANDLER {
public:
  //! @brief Construct a new HPOLY_ATTR_PROP object
  HPOLY_ATTR_PROP() {}

  template <typename RETV, typename VISITOR>
  RETV Handle_rescale(VISITOR* visitor, air::base::NODE_PTR node) {
    air::base::NODE_PTR n_ch0 = node->Child(0);
    ATTR_INFO           info  = visitor->template Visit<RETV>(n_ch0);
    AIR_ASSERT(info.Num_q() > 1);
    ATTR_INFO ret(info.Num_q() - 1, info.Num_p(), info.Sbase());
    visitor->Context().Set_attr_info(node, ret);
    return ret;
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_add(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_arith<RETV>(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_add_ext(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_arith<RETV>(visitor, node, true);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_sub(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_arith<RETV>(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_sub_ext(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_arith<RETV>(visitor, node, true);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_mul(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_arith<RETV>(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_mul_ext(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_arith<RETV>(visitor, node, true);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_mac(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_arith<RETV>(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_mac_ext(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_arith<RETV>(visitor, node, true);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_arith(VISITOR* visitor, air::base::NODE_PTR node,
                    bool is_ext = false) {
    ATTR_INFO      ret;
    ATTR_PROP_CTX& ctx = visitor->Context();
    for (uint32_t id = 0; id < node->Num_child(); ++id) {
      air::base::NODE_PTR child      = node->Child(id);
      ATTR_INFO           child_info = visitor->template Visit<RETV>(child);
      if (!ctx.Irgen().Is_fhe_type(child->Rtype_id())) {
        continue;
      }
      AIR_ASSERT(child_info.Is_valid());
      if (!ret.Is_valid()) {
        ret.Join(child_info);
      }
      AIR_ASSERT(ret.Sbase() == child_info.Sbase());
      uint32_t num_q =
          ret.Num_q() > child_info.Num_q() ? child_info.Num_q() : ret.Num_q();
      uint32_t num_p = is_ext ? child_info.Num_p() : 0;
      if (ret.Num_q() != num_q) {
        CMPLR_WARN_MSG(ctx.Driver_ctx()->Tfile(),
                       "Unmatched level in arithmetic, result will pick the "
                       "smaller level");
      }
      AIR_ASSERT_MSG(!is_ext || num_p != 0, "Extended op with no p primes");
      ret.Set_num_q(num_q);
      ret.Set_num_p(num_p);
    }
    ctx.Set_attr_info(node, ret);
    return ret;
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_precomp(VISITOR* visitor, air::base::NODE_PTR node) {
    IR_GEN&             irgen = visitor->Context().Irgen();
    air::base::NODE_PTR n_ch0 = node->Child(0);
    ATTR_INFO           info1 = visitor->template Visit<RETV>(n_ch0);
    ATTR_INFO           ret(info1.Num_q(),
                            irgen.Lower_ctx()->Get_ctx_param().Get_p_prime_num(), 0);
    visitor->Context().Set_attr_info(node, ret);
    return ret;
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_mod_down(VISITOR* visitor, air::base::NODE_PTR node) {
    IR_GEN&             irgen = visitor->Context().Irgen();
    air::base::NODE_PTR n_ch0 = node->Child(0);
    ATTR_INFO           info  = visitor->template Visit<RETV>(n_ch0);
    ATTR_INFO           ret(info.Num_q(), 0, 0);
    visitor->Context().Set_attr_info(node, ret);
    return ret;
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_concat(VISITOR* visitor, air::base::NODE_PTR node) {
    air::base::NODE_PTR n_ch0 = node->Child(0);
    air::base::NODE_PTR n_ch1 = node->Child(1);
    ATTR_INFO           info1 = visitor->template Visit<RETV>(n_ch0);
    ATTR_INFO           info2 = visitor->template Visit<RETV>(n_ch1);
    AIR_ASSERT(info1.Is_valid() && info2.Is_valid());
    AIR_ASSERT(info1.Num_p() == 0 || info2.Num_p() == 0);
    // if greater, there are gaps between Q and P
    AIR_ASSERT(info2.Sbase() >= info1.Sbase() + info1.Num_q());
    uint32_t  num_p = info1.Num_p() == 0 ? info2.Num_p() : info1.Num_p();
    ATTR_INFO retv(info1.Num_q() + info2.Num_q(), num_p, info1.Sbase());
    visitor->Context().Set_attr_info(node, retv);
    return retv;
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_extract(VISITOR* visitor, air::base::NODE_PTR node) {
    ATTR_PROP_CTX&      ctx   = visitor->Context();
    IR_GEN&             irgen = ctx.Irgen();
    air::base::NODE_PTR n_ch0 = node->Child(0);
    air::base::NODE_PTR n_ch1 = node->Child(1);
    air::base::NODE_PTR n_ch2 = node->Child(2);
    ATTR_INFO           info  = visitor->template Visit<RETV>(n_ch0);
    AIR_ASSERT(n_ch1->Opcode() == air::core::OPC_INTCONST);
    AIR_ASSERT(n_ch2->Opcode() == air::core::OPC_INTCONST);
    // Please note: s_idx and e_idx are indices for RNS_POLY
    // iterations, not for the RNS basis.
    uint32_t s_idx = n_ch1->Intconst();
    uint32_t e_idx = n_ch2->Intconst();

    AIR_ASSERT(info.Is_valid());
    uint32_t num_q     = info.Num_q();
    uint32_t num_p     = info.Num_p();
    uint32_t ret_num_q = 0;
    uint32_t ret_num_p = 0;
    if (e_idx >= num_q) {
      AIR_ASSERT_MSG(e_idx == num_q + num_p - 1, "p either zero or full");
      ret_num_p = num_p;
      ret_num_q = e_idx - s_idx + 1 - num_p;
    } else {
      AIR_ASSERT_MSG(e_idx <= num_q - 1, "p either zero or full");
      ret_num_p = 0;
      ret_num_q = e_idx - s_idx + 1;
    }
    ATTR_INFO retv(ret_num_q, ret_num_p, info.Sbase() + s_idx);
    ctx.Set_attr_info(node, retv);
    return retv;
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_bconv(VISITOR* visitor, air::base::NODE_PTR node) {
    ATTR_PROP_CTX&      ctx   = visitor->Context();
    air::base::NODE_PTR n_ch0 = node->Child(0);
    air::base::NODE_PTR n_ch2 = node->Child(2);
    air::base::NODE_PTR n_ch3 = node->Child(3);
    AIR_ASSERT(n_ch2->Opcode() == air::core::OPC_INTCONST);
    AIR_ASSERT(n_ch3->Opcode() == air::core::OPC_INTCONST);

    visitor->template Visit<RETV>(n_ch0);

    uint32_t  s_idx = n_ch2->Intconst();
    uint32_t  e_idx = n_ch3->Intconst();
    ATTR_INFO retv(ctx.Num_q_from_rns_range(s_idx, e_idx),
                   ctx.Num_p_from_rns_range(s_idx, e_idx), s_idx);
    visitor->Context().Set_attr_info(node, retv);
    return retv;
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_bswitch(VISITOR* visitor, air::base::NODE_PTR node) {
    ATTR_PROP_CTX&      ctx   = visitor->Context();
    air::base::NODE_PTR n_ch0 = node->Child(0);
    air::base::NODE_PTR n_ch2 = node->Child(2);
    air::base::NODE_PTR n_ch3 = node->Child(3);
    AIR_ASSERT(n_ch2->Opcode() == air::core::OPC_INTCONST);
    AIR_ASSERT(n_ch3->Opcode() == air::core::OPC_INTCONST);

    visitor->template Visit<RETV>(n_ch0);

    uint32_t  s_idx = n_ch2->Intconst();
    uint32_t  e_idx = n_ch3->Intconst();
    ATTR_INFO retv(ctx.Num_q_from_rns_range(s_idx, e_idx),
                   ctx.Num_p_from_rns_range(s_idx, e_idx), s_idx);
    visitor->Context().Set_attr_info(node, retv);
    return retv;
  }
};

class CORE_ATTR_PROP : public air::core::DEFAULT_HANDLER {
public:
  CORE_ATTR_PROP() {}

  template <typename RETV, typename VISITOR>
  RETV Handle_func_entry(VISITOR* visitor, air::base::NODE_PTR node) {
    ATTR_PROP_CTX&           ctx      = visitor->Context();
    air::opt::SSA_CONTAINER* ssa_cntr = ctx.Ssa_cntr();
    AIR_ASSERT(node->Num_child() > 0);
    for (uint32_t id = 0; id < node->Num_child(); ++id) {
      air::base::NODE_PTR child = node->Child(id);
      visitor->template Visit<RETV>(child);
    }
    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_idname(VISITOR* visitor, air::base::NODE_PTR formal) {
    ATTR_PROP_CTX& ctx   = visitor->Context();
    IR_GEN&        irgen = ctx.Irgen();
    // early return if not fhe type
    if (!irgen.Is_fhe_type(formal->Rtype_id())) {
      return RETV();
    }

    air::opt::SSA_CONTAINER* ssa_cntr = ctx.Ssa_cntr();
    air::opt::SSA_VER_PTR    ver_ptr  = ssa_cntr->Node_ver(formal->Id());
    // set ssa attribute from formal node's attribute
    ATTR_INFO info(irgen.Get_num_q(formal), irgen.Get_num_p(formal),
                   irgen.Get_sbase(formal), irgen.Get_sf_deg(formal));
    ctx.Set_attr_info(ver_ptr->Id(), info);

    air::base::NODE_PTR parent = visitor->Parent(1);
    AIR_ASSERT(parent->Opcode() == air::core::OPC_FUNC_ENTRY);
    air::opt::CHI_NODE_ID chi = ssa_cntr->Node_chi(parent->Id());

    // propogate attribute from IDNAME node to its field by iterate the entry
    // chi list
    if (chi != air::base::Null_id) {
      auto visit = [](air::opt::CHI_NODE_PTR chi, ATTR_PROP_CTX& ctx,
                      air::opt::SSA_VER_PTR par_ver) {
        air::opt::SSA_CONTAINER* ssa_cntr   = ctx.Ssa_cntr();
        air::opt::SSA_VER_PTR    ver        = chi->Result();
        air::opt::SSA_SYM_PTR    sym        = ver->Sym();
        air::base::TYPE_ID       ty_id      = sym->Type_id();
        air::opt::SSA_SYM_ID     par_sym_id = sym->Parent_id();
        if (ctx.Lower_ctx()->Is_rns_poly_type(ty_id) &&
            par_sym_id == par_ver->Sym_id()) {
          const ATTR_INFO& par_info = ctx.Attr_info(par_ver->Id());
          ctx.Set_attr_info(ver->Id(), par_info);
        }
      };
      air::opt::CHI_LIST list(ssa_cntr, chi);
      list.For_each(visit, ctx, ver_ptr);
    }
    return info;
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_do_loop(VISITOR* visitor, air::base::NODE_PTR node) {
    // 1. handle phi opnds from preheader
    Handle_phi_opnd(visitor->Context(), node, air::opt::PREHEADER_PHI_OPND_ID);

    // 2. handle stmt list
    for (uint32_t id = 0; id < node->Num_child(); ++id) {
      air::base::NODE_PTR child = node->Child(id);
      visitor->template Visit<RETV>(child);
    }

    // 3. handle phi opnds from backedge
    Handle_phi_opnd(visitor->Context(), node, air::opt::BACK_EDGE_PHI_OPND_ID);
    return RETV();
  }

  void Handle_phi_opnd(ATTR_PROP_CTX& ctx, air::base::NODE_PTR node,
                       uint32_t opnd_id) {
    air::opt::SSA_CONTAINER* ssa_cntr = ctx.Ssa_cntr();
    air::opt::PHI_NODE_ID    phi_id   = ssa_cntr->Node_phi(node->Id());
    air::opt::PHI_LIST       phi_list(ssa_cntr, phi_id);

    auto handle_opnd = [](air::opt::PHI_NODE_PTR phi, ATTR_PROP_CTX& ctx,
                          uint32_t opnd_id) {
      air::opt::SSA_VER_ID opnd_ver = phi->Opnd_id(opnd_id);
      air::opt::SSA_VER_ID res_ver  = phi->Result_id();
      // early return if no attribute info for operand
      if (!ctx.Has_attr_info(opnd_ver)) {
        return;
      }
      const ATTR_INFO& info = ctx.Attr_info(opnd_ver);
      if (ctx.Has_attr_info(res_ver)) {
        // merge the attribute
        const ATTR_INFO& res_info = ctx.Attr_info(res_ver);
        ATTR_INFO        join_info(res_info.Num_q(), res_info.Num_p(),
                                   res_info.Sbase());
        join_info.Join(info);
        ctx.Set_attr_info(res_ver, join_info);
      } else {
        ctx.Set_attr_info(res_ver, info);
      }
    };

    phi_list.For_each(handle_opnd, ctx, opnd_id);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_validate(VISITOR* visitor, air::base::NODE_PTR node) {
    // skip validate
    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_call(VISITOR* visitor, air::base::NODE_PTR node) {
    ATTR_PROP_CTX&           ctx      = visitor->Context();
    IR_GEN&                  irgen    = ctx.Irgen();
    air::opt::SSA_CONTAINER* ssa_cntr = ctx.Ssa_cntr();

    // 1. process return preg
    if (!irgen.Is_fhe_type(node->Ret_preg()->Type_id())) {
      return RETV();
    }
    // treat call as a leaf node, assume CKKS already annotate the attributes
    air::opt::SSA_VER_PTR ret_ver = ssa_cntr->Node_ver(node->Id());
    ATTR_INFO             ret_info(irgen.Get_num_q(node), irgen.Get_num_p(node),
                                   irgen.Get_sbase(node));
    ctx.Set_attr_info(ret_ver->Id(), ret_info);

    // 2. propagate to chi list
    ctx.Prop_chi_list(node, ret_info);
    return ret_info;
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ld(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_ld_var(visitor->Context(), node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ldf(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_ld_var(visitor->Context(), node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ldp(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_ld_var(visitor->Context(), node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ldpf(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_ld_var(visitor->Context(), node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ild(VISITOR* visitor, air::base::NODE_PTR node) {
    CMPLR_ASSERT(false, "TO IMPL");
    IR_GEN& irgen = visitor->Context().Irgen();
    if (irgen.Is_fhe_type(node->Rtype_id())) {
      ATTR_INFO info(irgen.Lower_ctx()->Get_ctx_param().Get_mul_level(),
                     irgen.Lower_ctx()->Get_ctx_param().Get_p_prime_num(), 0);
      visitor->Context().Set_attr_info(node, info);
      return info;
    }
    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_st(VISITOR* visitor, air::base::NODE_PTR node) {
    RETV rhs = visitor->template Visit<RETV>(node->Child(0));
    return Handle_st_var(visitor->Context(), node, rhs);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_stf(VISITOR* visitor, air::base::NODE_PTR node) {
    RETV rhs = visitor->template Visit<RETV>(node->Child(0));
    return Handle_st_var(visitor->Context(), node, rhs);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_stp(VISITOR* visitor, air::base::NODE_PTR node) {
    RETV rhs = visitor->template Visit<RETV>(node->Child(0));
    return Handle_st_var(visitor->Context(), node, rhs);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_stpf(VISITOR* visitor, air::base::NODE_PTR node) {
    RETV rhs = visitor->template Visit<RETV>(node->Child(0));
    return Handle_st_var(visitor->Context(), node, rhs);
  }

  ATTR_INFO Handle_ld_var(ATTR_PROP_CTX& ctx, air::base::NODE_PTR node) {
    IR_GEN& irgen = ctx.Irgen();
    if (irgen.Is_fhe_type(node->Rtype_id())) {
      air::opt::SSA_VER_PTR ssa_ver   = ctx.Ssa_cntr()->Node_ver(node->Id());
      const ATTR_INFO&      attr_info = ctx.Attr_info(ssa_ver->Id());
      ctx.Set_attr_info(node, attr_info);
      return attr_info;
    }
    return ATTR_INFO();
  }

  ATTR_INFO Handle_st_var(ATTR_PROP_CTX& ctx, air::base::NODE_PTR node,
                          ATTR_INFO& rhs_attr) {
    IR_GEN&            irgen = ctx.Irgen();
    air::base::TYPE_ID ty =
        node->Has_fld() ? node->Field()->Type_id() : node->Access_type_id();
    if (irgen.Is_fhe_type(ty)) {
      air::opt::SSA_VER_PTR ssa_ver = ctx.Ssa_cntr()->Node_ver(node->Id());
      ctx.Set_attr_info(ssa_ver->Id(), rhs_attr);
      ctx.Set_attr_info(node, rhs_attr);

      // propagate to chi list
      ctx.Prop_chi_list(node, rhs_attr);
    }
    return ATTR_INFO();
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_retv(VISITOR* visitor, air::base::NODE_PTR retv_node) {
    ATTR_PROP_CTX&           ctx      = visitor->Context();
    IR_GEN&                  irgen    = ctx.Irgen();
    air::opt::SSA_CONTAINER* ssa_cntr = ctx.Ssa_cntr();
    air::base::NODE_PTR      child    = retv_node->Child(0);
    AIR_ASSERT(child->Is_ld());
    air::opt::SSA_VER_ID ver_id = ssa_cntr->Node_ver_id(child->Id());
    // set ssa attribute from formal node's attribute
    ATTR_INFO info(irgen.Get_num_q(child), irgen.Get_num_p(child),
                   irgen.Get_sbase(child));
    ctx.Set_attr_info(ver_id, info);

    // prop to mu list
    air::opt::MU_NODE_ID mu = ssa_cntr->Node_mu(retv_node->Id());
    if (mu != air::base::Null_id) {
      auto visit = [](air::opt::MU_NODE_PTR mu, ATTR_PROP_CTX& ctx,
                      const ATTR_INFO& info) {
        air::opt::SSA_VER_ID ver = mu->Opnd_id();
        ctx.Set_attr_info(ver, info);
      };
      air::opt::MU_LIST list(ssa_cntr, mu);
      list.For_each(visit, ctx, info);
    }
    return info;
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_zero(VISITOR* visitor, air::base::NODE_PTR node) {
    IR_GEN&            irgen = visitor->Context().Irgen();
    air::base::TYPE_ID ty    = node->Rtype_id();
    if (irgen.Is_fhe_type(ty)) {
      ATTR_INFO info(irgen.Get_num_q(node), irgen.Get_num_p(node),
                     irgen.Get_sbase(node));
      return info;
    }
    return RETV();
  }
};

//! @brief ATTR_PGTR - attribute propagator, provide function scope
//! attribute propagation for MUL_LEVEL/NUM_P/SBASE at HPOLY phase
class ATTR_PGTR {
public:
  ATTR_PGTR(const POLY_CONFIG& config, air::driver::DRIVER_CTX* driver_ctx,
            core::LOWER_CTX* lower_ctx, air::base::FUNC_SCOPE* fscope)
      : _fscope(fscope),
        _ssa_cntr(&fscope->Container()),
        _ctx(config, driver_ctx, lower_ctx, &(fscope->Container()),
             &_ssa_cntr) {}
  ~ATTR_PGTR() {}

  void Run();

private:
  // REQUIRED UNDEFINED UNWANTED methods
  ATTR_PGTR(void);
  ATTR_PGTR(const ATTR_PGTR&);
  ATTR_PGTR& operator=(const ATTR_PGTR&);

  void                     Build_ssa();
  air::base::FUNC_SCOPE*   Func_scope() const { return _fscope; }
  ATTR_PROP_CTX&           Ctx() { return _ctx; }
  air::opt::SSA_CONTAINER* Ssa_cntr() { return &_ssa_cntr; }

  air::base::FUNC_SCOPE*  _fscope;
  ATTR_PROP_CTX           _ctx;
  air::opt::SSA_CONTAINER _ssa_cntr;
};
}  // namespace poly
}  // namespace fhe
#endif