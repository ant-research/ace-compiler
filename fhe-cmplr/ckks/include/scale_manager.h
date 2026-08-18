//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

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

private:
  // REQUIRED UNDEFINED UNWANTED methods
  SCALE_INFO& operator=(const SCALE_INFO& other);
  uint16_t    _scale_deg;
  uint16_t    _rescale_level;
};

//! return value of scale manage handlers
class SCALE_MNG_RETV {
public:
  SCALE_MNG_RETV(const SCALE_INFO& scale_info, NODE_PTR node)
      : _scale_info(scale_info.Scale_deg(), scale_info.Rescale_level()),
        _node(node) {}

  SCALE_MNG_RETV() : SCALE_MNG_RETV(SCALE_INFO(0, 0), NODE_PTR()) {}

  explicit SCALE_MNG_RETV(NODE_PTR node)
      : SCALE_MNG_RETV(SCALE_INFO(0, 0), node) {}

  ~SCALE_MNG_RETV() {}

  uint32_t Scale() const { return _scale_info.Scale_deg(); }
  uint32_t Rescale_level() const { return _scale_info.Rescale_level(); }
  const SCALE_INFO& Scale_info() const { return _scale_info; }
  SCALE_INFO&       Scale_info() { return _scale_info; }
  NODE_PTR          Node() const { return _node; }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  SCALE_MNG_RETV(const SCALE_MNG_RETV&);
  SCALE_MNG_RETV& operator=(const SCALE_MNG_RETV&);

  SCALE_INFO _scale_info;  // scale and mul-depth of handled node
  NODE_PTR   _node;
};

//! context of scale manage handlers.
class SCALE_MNG_CTX : public ANALYZE_CTX {
public:
  // key: id of addr_datum/preg; val: scale of addr_datum/preg
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

  void Set_scale_info(air::opt::SSA_VER_ID ver, const SCALE_INFO& scale_depth) {
    SCALE_INFO& scale_info = _ssa_ver_scale_info[ver.Value()];
    scale_info.Set_scale_deg(scale_depth.Scale_deg());
    scale_info.Set_rescale_level(scale_depth.Rescale_level());
  }

  const SCALE_INFO& Get_scale_info(air::opt::SSA_VER_ID ver) const {
    SCALE_MAP::const_iterator iter = _ssa_ver_scale_info.find(ver.Value());
    if (iter == _ssa_ver_scale_info.end()) {
      AIR_ASSERT_MSG(false, "encounter use before define");
    }
    return iter->second;
  }

  SCALE_MAP::const_iterator Find_scale_info(air::opt::SSA_VER_ID ver) const {
    return _ssa_ver_scale_info.find(ver.Value());
  }

  SCALE_MAP::const_iterator End_scale_info() const {
    return _ssa_ver_scale_info.end();
  }

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
    const char* scale_attr_name =
        Lower_ctx()->Attr_name(core::FHE_ATTR_KIND::SCALE);
    node->Set_attr(scale_attr_name, &scale, 1);
    const char* rescale_level_attr_name =
        Lower_ctx()->Attr_name(core::FHE_ATTR_KIND::RESCALE_LEVEL);
    node->Set_attr(rescale_level_attr_name, &rescale_level, 1);
  }

  void Set_node_scale_info(NODE_PTR node, const SCALE_INFO& scale_info) {
    Set_node_scale_info(node, scale_info.Scale_deg(),
                        scale_info.Rescale_level());
  }

  const PHI_NODE_LIST& Phi_res_need_rescale() const { return _rescale_phi_res; }
  void                 Set_phi_res_need_resacle(air::opt::PHI_NODE_ID phi) {
    _rescale_phi_res.push_back(phi);
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

  DECLARE_CKKS_CONFIG_ACCESS_API((*_config))
  DECLARE_TRACE_DETAIL_API((*_config), _driver_ctx)
private:
  // REQUIRED UNDEFINED UNWANTED methods
  SCALE_MNG_CTX(void);
  SCALE_MNG_CTX(const SCALE_MNG_CTX&);
  SCALE_MNG_CTX& operator=(const SCALE_MNG_CTX&);

  LOWER_CTX* _lower_ctx;           // lower context records FHE types
  SCALE_MAP  _ssa_ver_scale_info;  // scale and rescale level of ssa_ver
  uint32_t   _formal_scale_deg;    // scale degree of formals
  air::opt::SSA_CONTAINER*       _ssa_cntr;
  const CKKS_CONFIG*             _config;  // config of CKKS
  const air::driver::DRIVER_CTX* _driver_ctx;
  PHI_NODE_LIST _rescale_phi_res;  // phi nodes need rescale result
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
  template <typename VISITOR>
  void Handle_phi_opnd(VISITOR* visitor, NODE_PTR do_loop, uint32_t opnd_id);
};

template <typename VISITOR>
void CORE_SCALE_MANAGER::Handle_phi_opnd(VISITOR* visitor, NODE_PTR do_loop,
                                         uint32_t opnd_id) {
  SCALE_MNG_CTX*           ana_ctx  = &visitor->Context();
  air::opt::SSA_CONTAINER* ssa_cntr = ana_ctx->Ssa_cntr();
  air::opt::PHI_NODE_ID    phi_id   = ssa_cntr->Node_phi(do_loop->Id());
  air::opt::PHI_LIST       phi_list(ssa_cntr, phi_id);

  auto handle_phi_opnd = [](air::opt::PHI_NODE_PTR phi, uint32_t opnd_id,
                            SCALE_MNG_CTX& ctx) {
    air::opt::SSA_VER_ID                     ver_id = phi->Opnd_id(opnd_id);
    air::opt::SSA_VER_PTR                    ver = ctx.Ssa_cntr()->Ver(ver_id);
    SCALE_MNG_CTX::SCALE_MAP::const_iterator scale_info_iter =
        ctx.Find_scale_info(ver_id);
    if (scale_info_iter == ctx.End_scale_info()) return;

    SCALE_INFO opnd_scale_info(scale_info_iter->second);
    if (ctx.Rescale_phi_res(phi, opnd_id, opnd_scale_info.Scale_deg())) {
      opnd_scale_info.Set_scale_deg(opnd_scale_info.Scale_deg() - 1);
      opnd_scale_info.Set_rescale_level(opnd_scale_info.Rescale_level() + 1);

      ctx.Set_phi_res_need_resacle(phi->Id());
    }
    ctx.Set_scale_info(phi->Result_id(), opnd_scale_info);
  };
  phi_list.For_each(handle_phi_opnd, opnd_id, visitor->Context());
}

template <typename RETV, typename VISITOR>
RETV CORE_SCALE_MANAGER::Handle_do_loop(VISITOR* visitor, NODE_PTR do_loop) {
  // 1. handle phi opnds from preheader
  Handle_phi_opnd(visitor, do_loop, air::opt::PREHEADER_PHI_OPND_ID);

  // 2. handle stmt list
  for (uint32_t child_id = 0; child_id < do_loop->Num_child(); ++child_id) {
    NODE_PTR child = do_loop->Child(child_id);
    (void)visitor->template Visit<RETV>(child);
  }

  // 3. handle phi opnds from backedge
  Handle_phi_opnd(visitor, do_loop, air::opt::BACK_EDGE_PHI_OPND_ID);
  uint32_t unfix_scale_deg = visitor->Context().Unfix_scale();
  return RETV(SCALE_INFO(unfix_scale_deg, 0), do_loop);
}

template <typename RETV, typename VISITOR>
RETV CORE_SCALE_MANAGER::Handle_func_entry(VISITOR* visitor, NODE_PTR node) {
  ANALYZE_CTX::GUARD guard(visitor->Context(), node);
  uint32_t           child_num = node->Num_child();
  AIR_ASSERT(child_num > 1);
  for (uint32_t child_id = 0; (child_id + 1) < child_num; ++child_id) {
    (void)visitor->template Visit<RETV>(node->Child(child_id));
  }
  return visitor->template Visit<RETV>(node->Last_child());
}

template <typename RETV, typename VISITOR>
RETV CORE_SCALE_MANAGER::Handle_idname(VISITOR* visitor, NODE_PTR idname) {
  SCALE_MNG_CTX*        ana_ctx              = &visitor->Context();
  uint32_t              formal_scale_deg     = ana_ctx->Formal_scale_deg();
  uint32_t              formal_rescale_level = 0;
  air::opt::SSA_VER_PTR ssa_ver = ana_ctx->Ssa_cntr()->Node_ver(idname->Id());
  ana_ctx->Set_scale_info(ssa_ver->Id(),
                          SCALE_INFO(formal_scale_deg, formal_rescale_level));
  return RETV(SCALE_INFO(formal_scale_deg, formal_rescale_level), idname);
}

template <typename RETV, typename VISITOR>
RETV CORE_SCALE_MANAGER::Handle_ld(VISITOR* visitor, NODE_PTR load_node) {
  SCALE_MNG_CTX*   ana_ctx = &visitor->Context();
  TYPE_ID          type_id = load_node->Rtype_id();
  const LOWER_CTX* ctx     = ana_ctx->Lower_ctx();
  if (!ctx->Is_cipher_type(type_id) && !ctx->Is_cipher3_type(type_id)) {
    // return unfix_scale(0) for value stored in non-cipher var
    return RETV(SCALE_INFO(ana_ctx->Unfix_scale(), 0), load_node);
  }

  air::opt::SSA_VER_PTR ssa_ver =
      ana_ctx->Ssa_cntr()->Node_ver(load_node->Id());
  const SCALE_INFO& scale_info = ana_ctx->Get_scale_info(ssa_ver->Id());
  ana_ctx->Set_node_scale_info(load_node, scale_info.Scale_deg(),
                               scale_info.Rescale_level());
  return RETV{scale_info, load_node};
}

template <typename RETV, typename VISITOR>
RETV CORE_SCALE_MANAGER::Handle_ldp(VISITOR* visitor, NODE_PTR ldp_node) {
  SCALE_MNG_CTX*   ana_ctx = &visitor->Context();
  TYPE_ID          type_id = ldp_node->Rtype_id();
  const LOWER_CTX* ctx     = ana_ctx->Lower_ctx();
  if (!ctx->Is_cipher_type(type_id) && !ctx->Is_cipher3_type(type_id)) {
    // return unfix_scale(0) for value stored in non-cipher preg
    return RETV(SCALE_INFO(ana_ctx->Unfix_scale(), 0), ldp_node);
  }

  air::opt::SSA_VER_PTR ssa_ver = ana_ctx->Ssa_cntr()->Node_ver(ldp_node->Id());
  const SCALE_INFO&     scale_info = ana_ctx->Get_scale_info(ssa_ver->Id());
  return RETV{scale_info, ldp_node};
}

template <typename RETV, typename VISITOR>
RETV CORE_SCALE_MANAGER::Handle_ild(VISITOR* visitor, NODE_PTR ild_node) {
  SCALE_MNG_CTX*   ana_ctx        = &visitor->Context();
  TYPE_ID          access_type_id = ild_node->Access_type_id();
  const LOWER_CTX* ctx            = ana_ctx->Lower_ctx();
  if (!ctx->Is_cipher_type(access_type_id) &&
      !ctx->Is_cipher3_type(access_type_id)) {
    // return unfix_scale(0) for value stored in non-cipher array
    return RETV(SCALE_INFO(ana_ctx->Unfix_scale(), 0), ild_node);
  }

  NODE_PTR addr_child = ild_node->Child(0);
  AIR_ASSERT(addr_child->Opcode() == air::core::OPC_ARRAY);
  NODE_PTR lda_child = addr_child->Array_base();
  AIR_ASSERT(lda_child->Opcode() == air::core::OPC_LDA);
  ADDR_DATUM_PTR array_sym = lda_child->Addr_datum();
  AIR_ASSERT(array_sym->Type()->Is_array());

  air::opt::SSA_CONTAINER* ssa_cntr   = ana_ctx->Ssa_cntr();
  air::opt::MU_NODE_ID     mu         = ssa_cntr->Node_mu(ild_node->Id());
  air::opt::SSA_VER_ID     ssa_ver    = ssa_cntr->Mu_node(mu)->Opnd_id();
  const SCALE_INFO&        scale_info = ana_ctx->Get_scale_info(ssa_ver);
  return RETV{scale_info, ild_node};
}

template <typename RETV, typename VISITOR>
RETV CORE_SCALE_MANAGER::Handle_st(VISITOR* visitor, NODE_PTR st_node) {
  // 1. handle rhs node
  NODE_PTR child = st_node->Child(0);
  RETV     res   = visitor->template Visit<RETV>(child);
  if (res.Node() != NODE_PTR() && res.Node() != child) {
    st_node->Set_child(0, res.Node());
  }

  // 2. update scale of stored addr_datum
  SCALE_MNG_CTX*           ana_ctx  = &visitor->Context();
  air::opt::SSA_CONTAINER* ssa_cntr = ana_ctx->Ssa_cntr();
  air::opt::SSA_VER_ID     ssa_ver  = ssa_cntr->Node_ver_id(st_node->Id());
  ana_ctx->Set_scale_info(ssa_ver, res.Scale_info());
  ana_ctx->Set_node_scale_info(st_node, res.Scale(), res.Rescale_level());
  return RETV{res.Scale_info(), st_node};
}

template <typename RETV, typename VISITOR>
RETV CORE_SCALE_MANAGER::Handle_stp(VISITOR* visitor, NODE_PTR stp_node) {
  // 1. handle rhs node
  NODE_PTR child = stp_node->Child(0);
  RETV     res   = visitor->template Visit<RETV>(child);
  if (res.Node() != NODE_PTR() && res.Node() != child) {
    stp_node->Set_child(0, res.Node());
  }

  // 2. update scale of stored preg
  SCALE_MNG_CTX*           ana_ctx  = &visitor->Context();
  air::opt::SSA_CONTAINER* ssa_cntr = ana_ctx->Ssa_cntr();
  air::opt::SSA_VER_ID     ssa_ver  = ssa_cntr->Node_ver_id(stp_node->Id());
  ana_ctx->Set_scale_info(ssa_ver, res.Scale_info());
  return RETV{res.Scale_info(), stp_node};
}

template <typename RETV, typename VISITOR>
RETV CORE_SCALE_MANAGER::Handle_ist(VISITOR* visitor, NODE_PTR ist_node) {
  // 1. handle rhs node
  NODE_PTR rhs_child = ist_node->Child(1);
  RETV     res       = visitor->template Visit<RETV>(rhs_child);
  if (res.Node() != NODE_PTR() && res.Node() != rhs_child) {
    ist_node->Set_child(1, res.Node());
  }

  // 2. update scale of istored cipher
  NODE_PTR addr_child = ist_node->Child(0);
  AIR_ASSERT(addr_child->Opcode() == air::core::OPC_ARRAY);
  NODE_PTR lda_child = addr_child->Array_base();
  AIR_ASSERT(lda_child->Opcode() == air::core::OPC_LDA);
  ADDR_DATUM_PTR array_sym = lda_child->Addr_datum();
  AIR_ASSERT(array_sym->Type()->Is_array());

  SCALE_MNG_CTX*           ana_ctx  = &visitor->Context();
  air::opt::SSA_CONTAINER* ssa_cntr = ana_ctx->Ssa_cntr();
  air::opt::CHI_NODE_ID    chi      = ssa_cntr->Node_chi(ist_node->Id());
  air::opt::SSA_VER_ID     ssa_ver  = ssa_cntr->Chi_node(chi)->Result_id();
  ana_ctx->Set_scale_info(ssa_ver, res.Scale_info());
  return RETV{res.Scale_info(), ist_node};
}

template <typename RETV, typename VISITOR>
RETV CORE_SCALE_MANAGER::Handle_call(VISITOR* visitor, NODE_PTR call_node) {
  SCALE_MNG_CTX* ana_ctx = &visitor->Context();
  LOWER_CTX*     ctx     = ana_ctx->Lower_ctx();
  CKKS_GEN       ckks_gen(call_node->Container(), ctx);
  // 1. handle child nodes: reset scale of each child to scale_factor
  uint32_t max_rescale_level = 0;
  for (uint32_t id = 0; id < call_node->Num_child(); ++id) {
    NODE_PTR child         = call_node->Child(id);
    TYPE_ID  child_type_id = child->Rtype_id();
    RETV     res           = visitor->template Visit<RETV>(child);
    uint32_t scale_deg     = res.Scale();
    uint32_t rescale_level = res.Rescale_level();
    NODE_PTR new_child     = res.Node();

    // not support CIPHER3 type parameter.
    AIR_ASSERT(!ctx->Is_cipher3_type(child_type_id));
    if (!ctx->Is_cipher_type(child_type_id)) {
      AIR_ASSERT(new_child == child);
      continue;
    }

    while (scale_deg > 1) {
      new_child = ckks_gen.Gen_rescale(new_child);
      scale_deg -= 1;
      ++rescale_level;
    }
    AIR_ASSERT(scale_deg == 1);
    call_node->Set_child(id, new_child);
    max_rescale_level = std::max(max_rescale_level, rescale_level);
  }

  // 2. handle ret value: set scale of ret value as scale factor.
  PREG_PTR ret_preg = call_node->Ret_preg();
  TYPE_ID  type_id  = ret_preg->Type_id();
  // not support return CIPHER3 type value.
  AIR_ASSERT(!ctx->Is_cipher3_type(type_id));
  if (!ctx->Is_cipher_type(type_id)) {
    // return unfix_scale(0) for value stored in non-cipher preg
    return RETV(SCALE_INFO(ana_ctx->Unfix_scale(), 0), call_node);
  }

  const char* mul_depth_attr_name = ctx->Attr_name(FHE_ATTR_KIND::MUL_DEPTH);
  const uint32_t* mul_depth_ptr =
      call_node->Attr<uint32_t>(mul_depth_attr_name);
  AIR_ASSERT(mul_depth_ptr != nullptr);
  uint32_t retv_rescale_level = *mul_depth_ptr + max_rescale_level;

  air::opt::SSA_CONTAINER* ssa_cntr = ana_ctx->Ssa_cntr();
  air::opt::SSA_VER_ID     retv     = ssa_cntr->Node_ver_id(call_node->Id());
  SCALE_INFO               retv_scale_info(1, retv_rescale_level);
  ana_ctx->Set_scale_info(retv, retv_scale_info);
  return RETV{retv_scale_info, call_node};
}

template <typename RETV, typename VISITOR>
RETV CORE_SCALE_MANAGER::Handle_retv(VISITOR* visitor, NODE_PTR retv_node) {
  SCALE_MNG_CTX* ana_ctx = &visitor->Context();
  // 1. handle child of retv
  NODE_PTR child         = retv_node->Child(0);
  TYPE_ID  child_type_id = child->Rtype_id();
  AIR_ASSERT(ana_ctx->Lower_ctx()->Is_cipher_type(child_type_id));

  RETV     res           = visitor->template Visit<RETV>(child);
  NODE_PTR new_child     = res.Node();
  uint32_t scale_deg     = res.Scale();
  uint32_t rescale_level = res.Rescale_level();

  // 2. remain retv of Main_graph unchanged.
  FUNC_PTR parent_func = retv_node->Func_scope()->Owning_func();
  if (parent_func->Entry_point()->Is_program_entry()) {
    ana_ctx->Set_node_scale_info(retv_node->Child(0), scale_deg, rescale_level);
    return RETV{res.Scale_info(), retv_node};
  }

  // 3. rescale child of retv to scale factor.
  // opnd of retv must be: load/ldid sym
  if (scale_deg == 1 && new_child->Has_sym() && new_child->Is_ld()) {
    ana_ctx->Set_node_scale_info(retv_node->Child(0), 1, rescale_level);
    return RETV{res.Scale_info(), retv_node};
  }
  CKKS_GEN ckks_gen(retv_node->Container(), ana_ctx->Lower_ctx());
  while (scale_deg > 1) {
    new_child = ckks_gen.Gen_rescale(new_child);
    scale_deg -= 1;
    ++rescale_level;
  }
  AIR_ASSERT(scale_deg == 1);
  const SPOS& spos       = new_child->Spos();
  CONTAINER*  cntr       = retv_node->Container();
  FUNC_SCOPE* func_scope = retv_node->Func_scope();

  PREG_PTR preg = func_scope->New_preg(new_child->Rtype());
  STMT_PTR stp  = cntr->New_stp(new_child, preg, spos);
  cntr->Stmt_list().Prepend(retv_node->Stmt(), stp);

  NODE_PTR ld_tmp = cntr->New_ldp(preg, spos);
  retv_node->Set_child(0, ld_tmp);
  ana_ctx->Set_node_scale_info(retv_node->Child(0), 1, rescale_level);
  return RETV{SCALE_INFO(1, rescale_level), retv_node};
}

//! impl of CKKS IR scale manager
class CKKS_SCALE_MANAGER : public fhe::ckks::INVALID_HANDLER {
public:
  template <typename RETV, typename VISITOR>
  RETV Handle_mul(VISITOR* visitor, NODE_PTR mul_node);

  template <typename RETV, typename VISITOR>
  RETV Handle_add(VISITOR* visitor, NODE_PTR add_node);

  // template <typename RETV, typename VISITOR>
  // RETV Handle_sub(VISITOR* visitor, NODE_PTR add_node);

  template <typename RETV, typename VISITOR>
  RETV Handle_rotate(VISITOR* visitor, NODE_PTR rot_node);

  template <typename RETV, typename VISITOR>
  RETV Handle_relin(VISITOR* visitor, NODE_PTR relin_node);

  template <typename RETV, typename VISITOR>
  RETV Handle_encode(VISITOR* visitor, NODE_PTR encode_node);

  template <typename RETV, typename VISITOR>
  RETV Handle_bootstrap(VISITOR* visitor, NODE_PTR bootstrap);

private:
  //! gen CKKS.rescale(node) to dec scale of node.
  //! scale of CKKS.rescale(node) is (node.scale - sf).
  template <typename RETV, typename VISITOR>
  RETV Rescale_node(VISITOR* visitor, NODE_PTR node);

  //! handle encode as 2nd child of CKKS.mul/add/sub
  void Handle_encode_in_bin_arith_node(SCALE_MNG_CTX* ana_ctx,
                                       NODE_PTR       bin_node,
                                       uint32_t       child0_scale);
};

template <typename RETV, typename VISITOR>
RETV CKKS_SCALE_MANAGER::Rescale_node(VISITOR* visitor, NODE_PTR node) {
  RETV     retv      = visitor->template Visit<RETV>(node);
  NODE_PTR new_node  = retv.Node();
  uint32_t scale_deg = retv.Scale();
  if (scale_deg == 1) {
    return RETV{retv.Scale_info(), new_node};
  }

  SCALE_MNG_CTX* ana_ctx = &visitor->Context();
  CKKS_GEN       ckks_gen(node->Container(), ana_ctx->Lower_ctx());
  AIR_ASSERT(scale_deg > 1);
  SPOS     spos         = new_node->Spos();
  NODE_PTR rescale_node = ckks_gen.Gen_rescale(new_node);
  scale_deg -= 1;
  uint32_t rescale_level = retv.Rescale_level() + 1;
  AIR_ASSERT(scale_deg == 1);
  ana_ctx->Set_node_scale_info(rescale_node, scale_deg, rescale_level);
  return RETV{SCALE_INFO(scale_deg, rescale_level), rescale_node};
}

template <typename RETV, typename VISITOR>
RETV CKKS_SCALE_MANAGER::Handle_mul(VISITOR* visitor, NODE_PTR mul_node) {
  SCALE_MNG_CTX* ana_ctx   = &visitor->Context();
  LOWER_CTX*     lower_ctx = ana_ctx->Lower_ctx();
  // handle child0: dec scale of child0 to sf
  NODE_PTR child0 = mul_node->Child(0);
  AIR_ASSERT(lower_ctx->Is_cipher_type(child0->Rtype_id()));
  RETV     retv0         = Rescale_node<RETV>(visitor, child0);
  uint32_t rescale_level = retv0.Rescale_level();
  NODE_PTR new_child0    = retv0.Node();
  mul_node->Set_child(0, new_child0);

  // handle child1: dec scale of child 1 to sf
  NODE_PTR child1         = mul_node->Child(1);
  TYPE_ID  child1_type_id = child1->Rtype_id();
  if (lower_ctx->Is_cipher_type(child1_type_id)) {
    RETV retv1 = Rescale_node<RETV>(visitor, child1);
    mul_node->Set_child(1, retv1.Node());
    rescale_level = std::max(rescale_level, retv1.Rescale_level());
  } else if (lower_ctx->Is_plain_type(child1_type_id)) {
    OPCODE encode_op(CKKS_DOMAIN::ID, CKKS_OPERATOR::ENCODE);
    if (child1->Opcode() == encode_op) {
      Handle_encode_in_bin_arith_node(ana_ctx, mul_node, 1);
    } else {
      Templ_print(std::cout, "TODO: handle plaintext expr or var");
    }
  } else if (child1->Rtype()->Is_scalar()) {
    // scale of scalar is set as sf in runtime. no need to handle it in cmplr.
  } else {
    Templ_print(std::cout, "ERROR: not supported rtype of child1 of CKKS::MUL");
    AIR_ASSERT(false);
  }
  ana_ctx->Set_node_scale_info(mul_node, 2, rescale_level);
  if (ana_ctx->Rescale_node(mul_node, ana_ctx->Parent_stmt(), 2)) {
    CKKS_GEN ckks_gen(mul_node->Container(), lower_ctx);
    NODE_PTR rescale_node = ckks_gen.Gen_rescale(mul_node);
    ana_ctx->Set_node_scale_info(rescale_node, 1, rescale_level + 1);

    ana_ctx->Trace(TRACE_CKKS_TRAN_RES,
                   "\nCKKS_SCALE_MANAGER::Handle_mul gen rescale node:\n");
    ana_ctx->Trace_obj(TRACE_CKKS_TRAN_RES, rescale_node);
    return RETV{SCALE_INFO(1, rescale_level + 1), rescale_node};
  } else {
    return RETV{SCALE_INFO(2, rescale_level), mul_node};
  }
}

template <typename RETV, typename VISITOR>
RETV CKKS_SCALE_MANAGER::Handle_add(VISITOR* visitor, NODE_PTR add_node) {
  SCALE_MNG_CTX* ana_ctx = &visitor->Context();
  // handle child0
  NODE_PTR child0 = add_node->Child(0);
  RETV     retv0  = visitor->template Visit<RETV>(child0);
  add_node->Set_child(0, retv0.Node());
  uint32_t scale_deg0     = retv0.Scale();
  uint32_t rescale_level0 = retv0.Rescale_level();

  // handle child1
  uint32_t   scale_deg1     = scale_deg0;
  uint32_t   rescale_level1 = rescale_level0;
  NODE_PTR   child1         = add_node->Child(1);
  TYPE_ID    rtype_child1   = child1->Rtype_id();
  LOWER_CTX* lower_ctx      = ana_ctx->Lower_ctx();
  OPCODE     opc_child1     = child1->Opcode();
  if (opc_child1 == OPC_ENCODE) {
    Handle_encode_in_bin_arith_node(ana_ctx, add_node, scale_deg0);
  } else if (lower_ctx->Is_cipher_type(rtype_child1) ||
             lower_ctx->Is_cipher3_type(rtype_child1)) {
    RETV retv1 = visitor->template Visit<RETV>(child1);
    add_node->Set_child(1, retv1.Node());
    scale_deg1     = retv1.Scale();
    rescale_level1 = retv1.Rescale_level();
  } else {
    AIR_ASSERT_MSG(child1->Rtype()->Is_prim(), "not supported type of child1");
    AIR_ASSERT_MSG(opc_child1 == air::core::OPC_ONE ||
                       opc_child1 == air::core::OPC_ZERO ||
                       opc_child1 == air::core::OPC_LDC,
                   "not supported opcode of child1");
  }

  uint32_t rescale_level;
  if (scale_deg0 == scale_deg1 || ana_ctx->Is_unfix_scale(scale_deg0)) {
    rescale_level = std::max(rescale_level0, rescale_level1);
    return RETV{SCALE_INFO(scale_deg1, rescale_level), add_node};
  } else if (ana_ctx->Is_unfix_scale(scale_deg1)) {
    return RETV{SCALE_INFO(scale_deg0, rescale_level1), add_node};
  } else if (scale_deg0 > scale_deg1) {
    AIR_ASSERT(scale_deg0 == (scale_deg1 + 1));
    NODE_PTR rescale = Rescale_node<RETV>(visitor, add_node->Child(0)).Node();
    add_node->Set_child(0, rescale);
    uint32_t rescale_level = std::max(rescale_level0 + 1, rescale_level1);
    return RETV{SCALE_INFO(scale_deg1, rescale_level), add_node};
  } else {
    AIR_ASSERT(scale_deg1 == (scale_deg0 + 1));
    NODE_PTR rescale = Rescale_node<RETV>(visitor, add_node->Child(1)).Node();
    add_node->Set_child(1, rescale);
    uint32_t rescale_level = std::max(rescale_level0, rescale_level1 + 1);
    return RETV{SCALE_INFO(scale_deg0, rescale_level), add_node};
  }
}

template <typename RETV, typename VISITOR>
RETV CKKS_SCALE_MANAGER::Handle_rotate(VISITOR* visitor, NODE_PTR rot_node) {
  // 1. handle child0
  RETV retv0 = visitor->template Visit<RETV>(rot_node->Child(0));
  rot_node->Set_child(0, retv0.Node());

  // rescale child0 if current rotation occurs out of loop
  SCALE_MNG_CTX& ctx = visitor->Context();
  if (ctx.Rescale_node(rot_node, ctx.Parent_stmt(), retv0.Scale())) {
    AIR_ASSERT(retv0.Scale() == 2);
    RETV new_retv0 = Rescale_node<RETV>(visitor, rot_node->Child(0));
    rot_node->Set_child(0, new_retv0.Node());
    retv0.Scale_info().Set_scale_deg(retv0.Scale() - 1);

    ctx.Trace(TRACE_CKKS_TRAN_RES,
              "CKKS_SCALE_MANAGER::Handle_rotate rescale opnd of rotate: ");
    ctx.Trace_obj(TRACE_CKKS_TRAN_RES, rot_node);
  }

  // handle child1
  (void)visitor->template Visit<RETV>(rot_node->Child(1));
  return RETV{retv0.Scale_info(), rot_node};
}

template <typename RETV, typename VISITOR>
RETV CKKS_SCALE_MANAGER::Handle_relin(VISITOR* visitor, NODE_PTR relin_node) {
  SCALE_MNG_CTX* ana_ctx = &visitor->Context();
  NODE_PTR       child0  = relin_node->Child(0);
  AIR_ASSERT(ana_ctx->Lower_ctx()->Is_cipher3_type(child0->Rtype_id()));

  RETV retv = visitor->template Visit<RETV>(child0);
  AIR_ASSERT(!ana_ctx->Need_rescale(retv.Scale()));
  if (retv.Node() != NODE_PTR() && retv.Node() != child0) {
    relin_node->Set_child(0, retv.Node());
  }
  return RETV{retv.Scale_info(), relin_node};
}

template <typename RETV, typename VISITOR>
RETV CKKS_SCALE_MANAGER::Handle_encode(VISITOR* visitor, NODE_PTR encode_node) {
  const uint32_t scale_child_id = 2;
  NODE_PTR       scale_node     = encode_node->Child(scale_child_id);
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
  return RETV{SCALE_INFO(scale_deg, rescale_lev), encode_node};
}

template <typename RETV, typename VISITOR>
RETV CKKS_SCALE_MANAGER::Handle_bootstrap(VISITOR* visitor,
                                          NODE_PTR bootstrap) {
  SCALE_MNG_CTX* ana_ctx = &visitor->Context();
  // handle child: reset scale of child to scale factor.
  NODE_PTR   child       = bootstrap->Child(0);
  RETV       res         = visitor->template Visit<RETV>(child);
  uint32_t   scale_deg   = res.Scale();
  uint32_t   rescale_lev = res.Rescale_level();
  NODE_PTR   new_child   = res.Node();
  LOWER_CTX* lower_ctx   = ana_ctx->Lower_ctx();
  CKKS_GEN   ckks_gen(bootstrap->Container(), lower_ctx);
  while (scale_deg > 1) {
    new_child = ckks_gen.Gen_rescale(new_child);
    scale_deg -= 1;
    ++rescale_lev;
  }
  AIR_ASSERT(scale_deg == 1);

  bootstrap->Set_child(0, new_child);
  ana_ctx->Set_node_scale_info(new_child, 1, rescale_lev);

  return RETV{SCALE_INFO(1, rescale_lev), bootstrap};
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
    Build_ssa();

    air::base::ANALYZE_CTX trav_ctx;
    INSERT_VISITOR         visitor(Mng_ctx(), {CORE_HANDLER(), CKKS_HANDLER()});
    NODE_PTR               func_body = Func_scope()->Container().Entry_node();
    visitor.template Visit<SCALE_MNG_RETV>(func_body);

    // rescale result of phi
    Rescale_phi_res();
  }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  SCALE_MANAGER(void);
  SCALE_MANAGER(const SCALE_MANAGER&);
  SCALE_MANAGER&           operator=(const SCALE_MANAGER&);
  void                     Build_ssa();
  void                     Rescale_phi_res();
  FUNC_SCOPE*              Func_scope() const { return _func_scope; }
  SCALE_MNG_CTX&           Mng_ctx() { return _mng_ctx; }
  LOWER_CTX*               Lower_ctx() const { return _mng_ctx.Lower_ctx(); }
  air::opt::SSA_CONTAINER* Ssa_cntr() { return &_ssa_cntr; }

  FUNC_SCOPE*   _func_scope;  // function scope to fulfill scale constraints
  SCALE_MNG_CTX _mng_ctx;     // context for scale manage handlers
  air::opt::SSA_CONTAINER _ssa_cntr;
};
}  // namespace ckks
}  // namespace fhe
