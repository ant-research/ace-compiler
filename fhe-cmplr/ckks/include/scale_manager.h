//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <string>

#include "air/base/analyze_ctx.h"
#include "air/base/container.h"
#include "air/base/opcode.h"
#include "air/base/st_decl.h"
#include "air/base/visitor.h"
#include "air/core/handler.h"
#include "air/core/null_handler.h"
#include "air/core/opcode.h"
#include "air/driver/pass.h"
#include "air/util/debug.h"
#include "air/util/messg.h"
#include "fhe/ckks/ckks_gen.h"
#include "fhe/ckks/ckks_handler.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/ckks/invalid_handler.h"
#include "fhe/core/lower_ctx.h"
#include "fhe/sihe/sihe_gen.h"

namespace fhe {
namespace ckks {
using namespace air::base;
using namespace fhe::core;

//! return value of scale manage handlers
class SCALE_INFO {
public:
  SCALE_INFO() : _scale(0), _node(NODE_PTR()) {}

  SCALE_INFO(NODE_PTR node) : _scale(0), _node(node) {}

  SCALE_INFO(uint32_t scale, NODE_PTR node) : _scale(scale), _node(node) {}

  ~SCALE_INFO() {}

  uint32_t Scale() const { return _scale; }
  NODE_PTR Node() const { return _node; }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  SCALE_INFO(const SCALE_INFO&);
  SCALE_INFO& operator=(const SCALE_INFO&);

  const uint32_t _scale;  // scale of handled node
  NODE_PTR       _node;
};

//! context of scale manage handlers.
class SCALE_MANAGE_CTX {
public:
  // key: id of addr_datum/preg; val: scale of addr_datum/preg
  using SCALE_MAP = std::map<uint32_t, uint32_t>;

  explicit SCALE_MANAGE_CTX(LOWER_CTX* ctx) : _lower_ctx(ctx) {}

  ~SCALE_MANAGE_CTX() {}

  inline uint32_t Get_scale_of_addr_datum(ADDR_DATUM_ID id) {
    SCALE_MAP::const_iterator iter = _scale_of_addr_datum.find(id.Value());
    if (iter == _scale_of_addr_datum.end()) {
      Templ_print(std::cout,
                  "ERROR: get scale of addr_datum before define it.");
      AIR_ASSERT(false);
    }
    return iter->second;
  }

  void Set_scale_of_addr_datum(ADDR_DATUM_ID id, uint32_t new_level) {
    _scale_of_addr_datum[id.Value()] = new_level;
  }

  inline uint32_t Get_scale_of_preg(PREG_ID id) {
    SCALE_MAP::const_iterator iter = _scale_of_preg.find(id.Value());
    if (iter == _scale_of_preg.end()) {
      Templ_print(std::cout, "ERROR: get scale of preg before define it.");
      AIR_ASSERT(false);
    }
    return iter->second;
  }

  void Set_scale_of_preg(PREG_ID id, uint32_t new_level) {
    _scale_of_preg[id.Value()] = new_level;
  }

  uint32_t Scale_factor() const {
    return _lower_ctx->Get_ctx_param().Get_scaling_factor_bit_num();
  }

  LOWER_CTX* Lower_ctx() const { return _lower_ctx; }
  uint32_t   Unfix_scale() const { return 0; }
  bool       Is_unfix_scale(uint32_t scale) const { return scale == 0; }
  bool Need_rescale(uint32_t scale) const { return scale > 2 * Scale_factor(); }

  //! set scale attr for node result
  void Set_node_scale(NODE_PTR node, uint32_t scale) {
    const char* attr_name = Lower_ctx()->Attr_name(core::FHE_ATTR_KIND::SCALE);
    node->Set_attr(attr_name, &scale, 1);
  }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  SCALE_MANAGE_CTX(void);
  SCALE_MANAGE_CTX(const SCALE_MANAGE_CTX&);
  SCALE_MANAGE_CTX& operator=(const SCALE_MANAGE_CTX&);

  LOWER_CTX* _lower_ctx;            // lower context records FHE types
  SCALE_MAP  _scale_of_addr_datum;  // scale of each cipher type addr_datum
  SCALE_MAP  _scale_of_preg;        // scale of each cipher type preg
};

//! impl of CORE IR scale manager.
class CORE_SCALE_MANAGER : public air::core::NULL_HANDLER {
public:
  explicit CORE_SCALE_MANAGER(CONTAINER* cntr, SCALE_MANAGE_CTX* ctx)
      : _ana_ctx(ctx), _ckks_gen(cntr, ctx->Lower_ctx()) {}

  ~CORE_SCALE_MANAGER() {}

  template <typename RETV, typename VISITOR>
  RETV Handle_do_loop(VISITOR* visitor, NODE_PTR do_loop);

  template <typename RETV, typename VISITOR>
  RETV Handle_func_entry(VISITOR* visitor, NODE_PTR entry_node);

  template <typename RETV, typename VISITOR>
  RETV Handle_ld(VISITOR* visitor, NODE_PTR ld_node);

  template <typename RETV, typename VISITOR>
  RETV Handle_ldp(VISITOR* visitor, NODE_PTR ldp_node);

  template <typename RETV, typename VISITOR>
  RETV Handle_ldc(VISITOR* visitor, NODE_PTR ldc_node) {
    // runtime will set scale of const as sf.
    return RETV{_ana_ctx->Scale_factor(), ldc_node};
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ild(VISITOR* visitor, NODE_PTR ild_node);

  template <typename RETV, typename VISITOR>
  RETV Handle_zero(VISITOR* visitor, NODE_PTR zero) {
    // scale of CORE.zero is unfixed.
    return RETV{_ana_ctx->Unfix_scale(), zero};
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_one(VISITOR* visitor, NODE_PTR one) {
    // scale of CORE.one is unfixed.
    return RETV{_ana_ctx->Unfix_scale(), one};
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
  // REQUIRED UNDEFINED UNWANTED methods
  CORE_SCALE_MANAGER(void);
  CORE_SCALE_MANAGER(const CORE_SCALE_MANAGER&);
  CORE_SCALE_MANAGER& operator=(const CORE_SCALE_MANAGER&);

  SCALE_MANAGE_CTX* _ana_ctx;  // context of scale manager.
  CKKS_GEN          _ckks_gen;
};

template <typename RETV, typename VISITOR>
RETV CORE_SCALE_MANAGER::Handle_do_loop(VISITOR* visitor, NODE_PTR do_loop) {
  for (uint32_t child_id = 0; child_id < do_loop->Num_child(); ++child_id) {
    NODE_PTR child = do_loop->Child(child_id);
    (void)visitor->template Visit<RETV>(child);
  }
  return RETV(_ana_ctx->Unfix_scale(), do_loop);
}

template <typename RETV, typename VISITOR>
RETV CORE_SCALE_MANAGER::Handle_func_entry(VISITOR* visitor, NODE_PTR node) {
  ANALYZE_CTX::GUARD guard(visitor->Context(), node);
  return visitor->template Visit<RETV>(node->Last_child());
}

template <typename RETV, typename VISITOR>
RETV CORE_SCALE_MANAGER::Handle_ld(VISITOR* visitor, NODE_PTR load_node) {
  ADDR_DATUM_PTR addr_datum = load_node->Addr_datum();
  TYPE_ID        type_id    = addr_datum->Type_id();

  const LOWER_CTX* ctx = _ana_ctx->Lower_ctx();
  if (!ctx->Is_cipher_type(type_id) && !ctx->Is_cipher3_type(type_id)) {
    // return unfix_scale(0) for value stored in non-cipher var
    return RETV(_ana_ctx->Unfix_scale(), load_node);
  }

  uint32_t scale = _ana_ctx->Get_scale_of_addr_datum(addr_datum->Id());
  return RETV{scale, load_node};
}

template <typename RETV, typename VISITOR>
RETV CORE_SCALE_MANAGER::Handle_ldp(VISITOR* visitor, NODE_PTR ldp_node) {
  PREG_PTR preg    = ldp_node->Preg();
  TYPE_ID  type_id = preg->Type_id();

  const LOWER_CTX* ctx = _ana_ctx->Lower_ctx();
  if (!ctx->Is_cipher_type(type_id) && !ctx->Is_cipher3_type(type_id)) {
    // return unfix_scale(0) for value stored in non-cipher preg
    return RETV(_ana_ctx->Unfix_scale(), ldp_node);
  }

  uint32_t scale = _ana_ctx->Get_scale_of_preg(preg->Id());
  return RETV{scale, ldp_node};
}

template <typename RETV, typename VISITOR>
RETV CORE_SCALE_MANAGER::Handle_ild(VISITOR* visitor, NODE_PTR ild_node) {
  TYPE_ID          access_type_id = ild_node->Access_type_id();
  const LOWER_CTX* ctx            = _ana_ctx->Lower_ctx();
  if (!ctx->Is_cipher_type(access_type_id) &&
      !ctx->Is_cipher3_type(access_type_id)) {
    // return unfix_scale(0) for value stored in non-cipher array
    return RETV(_ana_ctx->Unfix_scale(), ild_node);
  }

  NODE_PTR addr_child = ild_node->Child(0);
  AIR_ASSERT(addr_child->Opcode() == air::core::OPC_ARRAY);
  NODE_PTR lda_child = addr_child->Array_base();
  AIR_ASSERT(lda_child->Opcode() == air::core::OPC_LDA);
  ADDR_DATUM_PTR array_sym = lda_child->Addr_datum();
  AIR_ASSERT(array_sym->Type()->Is_array());
  uint32_t scale = _ana_ctx->Get_scale_of_addr_datum(array_sym->Id());
  return RETV{scale, ild_node};
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
  ADDR_DATUM_ID id = st_node->Addr_datum_id();
  (void)_ana_ctx->Set_scale_of_addr_datum(id, res.Scale());
  return RETV{res.Scale(), st_node};
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
  PREG_ID preg = stp_node->Preg_id();
  (void)_ana_ctx->Set_scale_of_preg(preg, res.Scale());
  return RETV{res.Scale(), stp_node};
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
  (void)_ana_ctx->Set_scale_of_addr_datum(array_sym->Id(), res.Scale());
  return RETV{res.Scale(), ist_node};
}

template <typename RETV, typename VISITOR>
RETV CORE_SCALE_MANAGER::Handle_call(VISITOR* visitor, NODE_PTR call_node) {
  const LOWER_CTX* ctx = _ana_ctx->Lower_ctx();
  uint32_t         sf  = _ana_ctx->Scale_factor();

  // 1. handle child nodes: reset scale of each child to scale_factor
  for (uint32_t id = 0; id < call_node->Num_child(); ++id) {
    NODE_PTR child         = call_node->Child(id);
    TYPE_ID  child_type_id = child->Rtype_id();
    RETV     res           = visitor->template Visit<RETV>(child);
    uint32_t scale         = res.Scale();
    NODE_PTR new_child     = res.Node();

    // not support CIPHER3 type parameter.
    AIR_ASSERT(!ctx->Is_cipher3_type(child_type_id));
    if (!ctx->Is_cipher_type(child_type_id)) {
      AIR_ASSERT(new_child == child);
      continue;
    }

    while (scale > sf) {
      new_child = _ckks_gen.Gen_rescale(new_child);
      scale -= sf;
    }
    AIR_ASSERT(scale == sf);
    call_node->Set_child(id, new_child);
  }

  // 2. handle ret value: set scale of ret value as scale factor.
  PREG_PTR ret_preg = call_node->Ret_preg();
  TYPE_ID  type_id  = ret_preg->Type_id();
  // not support return CIPHER3 type value.
  AIR_ASSERT(!ctx->Is_cipher3_type(type_id));
  if (!ctx->Is_cipher_type(type_id)) {
    // return unfix_scale(0) for value stored in non-cipher preg
    return RETV(_ana_ctx->Unfix_scale(), call_node);
  }

  (void)_ana_ctx->Set_scale_of_preg(ret_preg->Id(), sf);
  return RETV{_ana_ctx->Scale_factor(), call_node};
}

template <typename RETV, typename VISITOR>
RETV CORE_SCALE_MANAGER::Handle_retv(VISITOR* visitor, NODE_PTR retv_node) {
  // 1. handle child of retv
  NODE_PTR child         = retv_node->Child(0);
  TYPE_ID  child_type_id = child->Rtype_id();
  AIR_ASSERT(_ana_ctx->Lower_ctx()->Is_cipher_type(child_type_id));

  RETV     res       = visitor->template Visit<RETV>(child);
  NODE_PTR new_child = res.Node();
  uint32_t scale     = res.Scale();

  // 2. remain retv of Main_graph unchanged.
  uint32_t sf          = _ana_ctx->Scale_factor();
  FUNC_PTR parent_func = retv_node->Func_scope()->Owning_func();
  if (parent_func->Entry_point()->Is_program_entry()) {
    AIR_ASSERT_MSG(scale == (sf * (scale / sf)),
                   "only support scale is an integer multiple of sf");
    _ana_ctx->Set_node_scale(retv_node->Child(0), scale / sf);
    return RETV{sf, retv_node};
  }

  // 3. rescale child of retv to scale factor.
  // opnd of retv must be: load/ldid sym
  if (scale == sf && new_child->Has_sym() && new_child->Is_ld()) {
    _ana_ctx->Set_node_scale(retv_node->Child(0), 1);
    return RETV{sf, retv_node};
  }
  while (scale > sf) {
    new_child = _ckks_gen.Gen_rescale(new_child);
    scale -= sf;
  }
  AIR_ASSERT(scale == sf);
  const SPOS& spos       = new_child->Spos();
  CONTAINER*  cntr       = retv_node->Container();
  FUNC_SCOPE* func_scope = retv_node->Func_scope();
  std::string tmp_name("_rescale_tmp_" + std::to_string(child->Id().Value()));
  ADDR_DATUM_PTR tmp_var =
      func_scope->New_var(new_child->Rtype(), tmp_name.c_str(), spos);
  STMT_PTR st = cntr->New_st(new_child, tmp_var, spos);
  cntr->Stmt_list().Prepend(retv_node->Stmt(), st);

  NODE_PTR ld_tmp = cntr->New_ld(tmp_var, spos);
  retv_node->Set_child(0, ld_tmp);
  _ana_ctx->Set_node_scale(retv_node->Child(0), 1);
  return {sf, retv_node};
}

//! impl of CKKS IR scale manager
class CKKS_SCALE_MANAGER : public fhe::ckks::INVALID_HANDLER {
public:
  CKKS_SCALE_MANAGER(CONTAINER* cntr, SCALE_MANAGE_CTX* ctx)
      : _mng_ctx(ctx), _cntr(cntr), _ckks_gen(cntr, ctx->Lower_ctx()) {}

  ~CKKS_SCALE_MANAGER() {}

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
  // REQUIRED UNDEFINED UNWANTED methods
  CKKS_SCALE_MANAGER(void);
  CKKS_SCALE_MANAGER(const CKKS_SCALE_MANAGER&);
  CKKS_SCALE_MANAGER& operator=(const CKKS_SCALE_MANAGER&);

  //! gen CKKS.rescale(node) to dec scale of node.
  //! scale of CKKS.rescale(node) is (node.scale - sf).
  template <typename RETV, typename VISITOR>
  RETV Rescale_node(VISITOR* visitor, NODE_PTR node);

  //! handle encode as 2nd child of CKKS.mul/add/sub
  void Handle_encode_in_bin_arith_node(STMT_PTR parent_stmt, NODE_PTR bin_node,
                                       uint32_t child0_scale);

  SCALE_MANAGE_CTX* Get_ana_ctx() const { return _mng_ctx; }

  SCALE_MANAGE_CTX* _mng_ctx;
  CONTAINER*        _cntr;      // CONTAINER of current function
  CKKS_GEN          _ckks_gen;  // CKKS_GEN of current function
};

template <typename RETV, typename VISITOR>
RETV CKKS_SCALE_MANAGER::Rescale_node(VISITOR* visitor, NODE_PTR node) {
  RETV     retv     = visitor->template Visit<RETV>(node);
  NODE_PTR new_node = retv.Node();
  uint32_t scale    = retv.Scale();
  uint32_t sf       = _mng_ctx->Scale_factor();
  if (scale == sf) {
    return RETV{scale, new_node};
  }

  AIR_ASSERT(scale > sf);
  SPOS     spos         = new_node->Spos();
  NODE_PTR rescale_node = _ckks_gen.Gen_rescale(new_node);
  scale -= sf;
  AIR_ASSERT(scale == sf);
  return RETV{scale, rescale_node};
}

template <typename RETV, typename VISITOR>
RETV CKKS_SCALE_MANAGER::Handle_mul(VISITOR* visitor, NODE_PTR mul_node) {
  // handle child0: dec scale of child0 to sf
  NODE_PTR child0 = mul_node->Child(0);
  AIR_ASSERT(_mng_ctx->Lower_ctx()->Is_cipher_type(child0->Rtype_id()));
  RETV     retv0      = Rescale_node<RETV>(visitor, child0);
  NODE_PTR new_child0 = retv0.Node();
  mul_node->Set_child(0, new_child0);

  // handle child1: dec scale of child 1 to sf
  NODE_PTR child1         = mul_node->Child(1);
  TYPE_ID  child1_type_id = child1->Rtype_id();
  if (_mng_ctx->Lower_ctx()->Is_cipher_type(child1_type_id)) {
    RETV retv1 = Rescale_node<RETV>(visitor, child1);
    mul_node->Set_child(1, retv1.Node());
  } else if (_mng_ctx->Lower_ctx()->Is_plain_type(child1_type_id)) {
    OPCODE encode_op(CKKS_DOMAIN::ID, CKKS_OPERATOR::ENCODE);
    if (child1->Opcode() == encode_op) {
      Handle_encode_in_bin_arith_node(visitor->Parent_stmt(), mul_node,
                                      retv0.Scale());
    } else {
      Templ_print(std::cout, "TODO: handle plaintext expr or var");
    }
  } else if (child1->Rtype()->Is_scalar()) {
    // scale of scalar is set as sf in runtime. no need to handle it in cmplr.
  } else {
    Templ_print(std::cout, "ERROR: not supported rtype of child1 of CKKS::MUL");
    AIR_ASSERT(false);
  }
  uint32_t sf = _mng_ctx->Scale_factor();
  return RETV{2 * sf, mul_node};
}

template <typename RETV, typename VISITOR>
RETV CKKS_SCALE_MANAGER::Handle_add(VISITOR* visitor, NODE_PTR add_node) {
  // handle child0
  NODE_PTR child0 = add_node->Child(0);
  RETV     retv0  = visitor->template Visit<RETV>(child0);
  add_node->Set_child(0, retv0.Node());
  uint32_t scale0 = retv0.Scale();

  // handle child1
  uint32_t   scale1       = scale0;
  NODE_PTR   child1       = add_node->Child(1);
  TYPE_ID    rtype_child1 = child1->Rtype_id();
  LOWER_CTX* lower_ctx    = _mng_ctx->Lower_ctx();
  OPCODE     opc_child1   = child1->Opcode();
  if (opc_child1 == OPC_ENCODE) {
    Handle_encode_in_bin_arith_node(visitor->Parent_stmt(), add_node, scale0);
  } else if (lower_ctx->Is_cipher_type(rtype_child1) ||
             lower_ctx->Is_cipher3_type(rtype_child1)) {
    RETV retv1 = visitor->template Visit<RETV>(child1);
    add_node->Set_child(1, retv1.Node());
    scale1 = retv1.Scale();
  } else {
    AIR_ASSERT_MSG(child1->Rtype()->Is_prim(), "not supported type of child1");
    AIR_ASSERT_MSG(opc_child1 == air::core::OPC_ONE ||
                       opc_child1 == air::core::OPC_ZERO ||
                       opc_child1 == air::core::OPC_LDC,
                   "not supported opcode of child1");
  }

  if (_mng_ctx->Is_unfix_scale(scale0)) {
    return RETV{scale1, add_node};
  } else if (_mng_ctx->Is_unfix_scale(scale1)) {
    return RETV{scale0, add_node};
  } else if (scale0 == scale1) {
    return RETV{scale0, add_node};
  } else if (scale0 > scale1) {
    AIR_ASSERT(scale0 == (scale1 + _mng_ctx->Scale_factor()));
    NODE_PTR rescale = Rescale_node<RETV>(visitor, add_node->Child(0)).Node();
    add_node->Set_child(0, rescale);
    return RETV{scale1, add_node};
  } else {
    AIR_ASSERT(scale1 == (scale0 + _mng_ctx->Scale_factor()));
    NODE_PTR rescale = Rescale_node<RETV>(visitor, add_node->Child(1)).Node();
    add_node->Set_child(1, rescale);
    return RETV{scale0, add_node};
  }
}

template <typename RETV, typename VISITOR>
RETV CKKS_SCALE_MANAGER::Handle_rotate(VISITOR* visitor, NODE_PTR rot_node) {
  RETV retv0 = visitor->template Visit<RETV>(rot_node->Child(0));
  rot_node->Set_child(0, retv0.Node());
  uint32_t scale = retv0.Scale();

  // handle child1
  (void)visitor->template Visit<RETV>(rot_node->Child(1));
  return RETV{scale, rot_node};
}

template <typename RETV, typename VISITOR>
RETV CKKS_SCALE_MANAGER::Handle_relin(VISITOR* visitor, NODE_PTR relin_node) {
  NODE_PTR child0 = relin_node->Child(0);
  AIR_ASSERT(_mng_ctx->Lower_ctx()->Is_cipher3_type(child0->Rtype_id()));

  uint32_t scale = visitor->template Visit<RETV>(child0).Scale();
  AIR_ASSERT(!_mng_ctx->Need_rescale(scale));
  return RETV{scale, relin_node};
}

template <typename RETV, typename VISITOR>
RETV CKKS_SCALE_MANAGER::Handle_encode(VISITOR* visitor, NODE_PTR encode_node) {
  NODE_PTR scale_node = encode_node->Child(2);
  if (scale_node->Domain() == air::core::CORE &&
      scale_node->Operator() == air::core::OPCODE::INTCONST) {
    uint32_t scale = scale_node->Intconst();
    scale          = (scale != 0) ? scale : _mng_ctx->Scale_factor();
    return RETV{scale, encode_node};
  }

  AIR_ASSERT(scale_node->Operator() == CKKS_OPERATOR::SCALE &&
             scale_node->Domain() == CKKS_DOMAIN::ID);
  NODE_PTR cipher_node = scale_node->Child(0);
  uint32_t scale       = visitor->template Visit<RETV>(cipher_node).Scale();
  return RETV{scale, encode_node};
}

template <typename RETV, typename VISITOR>
RETV CKKS_SCALE_MANAGER::Handle_bootstrap(VISITOR* visitor,
                                          NODE_PTR bootstrap) {
  // handle child: reset scale of child to scale factor.
  NODE_PTR child     = bootstrap->Child(0);
  RETV     res       = visitor->template Visit<RETV>(child);
  uint32_t scale     = res.Scale();
  NODE_PTR new_child = res.Node();
  uint32_t sf        = Get_ana_ctx()->Scale_factor();
  while (scale > sf) {
    new_child = _ckks_gen.Gen_rescale(new_child);
    scale -= sf;
  }
  AIR_ASSERT(scale == sf);
  bootstrap->Set_child(0, new_child);
  return RETV{sf, bootstrap};
}

//! insert rescale nodes to fulfill scale constraints:
//! 1. CKKS.add/sub: child0.scale == child1.scale
//! 2. CKKS.mul: (child0.scale + child1.scale) <= (sf + _scale_of_formal)
class SCALE_MANAGER {
public:
  using CORE_HANDLER = air::core::HANDLER<CORE_SCALE_MANAGER>;
  using CKKS_HANDLER = HANDLER<CKKS_SCALE_MANAGER>;
  using INSERT_VISITOR =
      air::base::VISITOR<air::base::ANALYZE_CTX, CORE_HANDLER, CKKS_HANDLER>;

  SCALE_MANAGER(FUNC_SCOPE* func_scope, LOWER_CTX* ctx,
                uint32_t scale_of_formal)
      : _func_scope(func_scope),
        _mng_ctx(ctx),
        _scale_of_formal(scale_of_formal) {}

  SCALE_MANAGER(FUNC_SCOPE* func_scope, LOWER_CTX* ctx)
      : _func_scope(func_scope), _mng_ctx(ctx) {
    _scale_of_formal = _mng_ctx.Scale_factor();
  }

  ~SCALE_MANAGER() {}

  //! insert rescale nodes to fulfill scale constraints
  void Run() {
    // 1. init scale of formals
    Handle_formal();

    // 2. handle function body
    CORE_SCALE_MANAGER     core_impl(&Func_scope()->Container(), &Mng_ctx());
    CORE_HANDLER           core_handler(&core_impl);
    CKKS_SCALE_MANAGER     ckks_impl(&Func_scope()->Container(), &Mng_ctx());
    CKKS_HANDLER           ckks_handler(&ckks_impl);
    air::base::ANALYZE_CTX trav_ctx;
    INSERT_VISITOR         visitor(trav_ctx, {core_handler, ckks_handler});
    NODE_PTR               func_body = Func_scope()->Container().Entry_node();
    visitor.template Visit<SCALE_INFO>(func_body);
  }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  SCALE_MANAGER(void);
  SCALE_MANAGER(const SCALE_MANAGER&);
  SCALE_MANAGER& operator=(const SCALE_MANAGER&);

  //! init scale of formals
  void Handle_formal() {
    FORMAL_ITER iter     = _func_scope->Begin_formal();
    FORMAL_ITER end_iter = _func_scope->End_formal();
    for (; iter != end_iter; ++iter) {
      ADDR_DATUM_ID id = (*iter)->Id();
      _mng_ctx.Set_scale_of_addr_datum(id, Scale_of_formal());
    }
  }

  FUNC_SCOPE*       Func_scope() const { return _func_scope; }
  SCALE_MANAGE_CTX& Mng_ctx() { return _mng_ctx; }
  LOWER_CTX*        Lower_ctx() const { return _mng_ctx.Lower_ctx(); }
  uint32_t          Scale_of_formal() const { return _scale_of_formal; }

  FUNC_SCOPE*      _func_scope;  // function scope to fulfill scale constraints
  SCALE_MANAGE_CTX _mng_ctx;     // context for scale manage handlers
  uint32_t         _scale_of_formal;  // scale of formals
};
}  // namespace ckks
}  // namespace fhe
