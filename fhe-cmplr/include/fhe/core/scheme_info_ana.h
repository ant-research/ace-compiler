//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CORE_SCHEME_INFO_ANA_H
#define FHE_CORE_SCHEME_INFO_ANA_H

#include "air/base/analyze_ctx.h"
#include "air/base/container_decl.h"
#include "air/base/visitor.h"
#include "air/core/default_handler.h"
#include "air/core/handler.h"
#include "air/driver/driver.h"
#include "air/util/debug.h"
#include "fhe/core/scheme_info.h"
#include "fhe/core/scheme_info_config.h"
#include "nn/core/default_handler.h"
#include "nn/core/handler.h"
#include "nn/core/opcode.h"

namespace fhe {

namespace core {

//! Information of tensor operation
//! which contains value of mul_level consumed by each tensor operation.
class TENSOR_OP_INFO {
public:
  TENSOR_OP_INFO(air::base::OPCODE op, uint32_t mul_level)
      : _opcode(op), _mul_level(mul_level) {}
  ~TENSOR_OP_INFO() {}

  air::base::OPCODE Opcode() const { return _opcode; }
  uint32_t          Mul_level() const { return _mul_level; }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  TENSOR_OP_INFO(void);
  TENSOR_OP_INFO(const TENSOR_OP_INFO&);
  TENSOR_OP_INFO operator=(const TENSOR_OP_INFO&);

  air::base::OPCODE _opcode;
  uint32_t          _mul_level;
};

//! Information of modulus at each security level.
//! _least_poly_deg_pow is the minimum value of log2(poly_deg),
//! under which has no valid modulus.
//! _mod_bit_num is an array composed of the max values of modulus bit number
//! at each poly_deg. Index of _mod_bit_num is (log2(poly_deg) -
//! _least_poly_deg_pow).
class MODULUS_INFO {
public:
  MODULUS_INFO(SECURITY_LEVEL sec_level, uint32_t least_deg_pow,
               std::vector<uint32_t> mod_bit_num)
      : _sec_level(sec_level),
        _least_poly_deg_pow(least_deg_pow),
        _mod_bit_num(mod_bit_num) {}
  ~MODULUS_INFO() {}

  SECURITY_LEVEL Sec_level() const { return _sec_level; }
  //! return max bit num of modulus at sec_lev and poly_deg.
  uint32_t Max_mod_bit_num(SECURITY_LEVEL sec_lev, uint32_t poly_deg) const {
    AIR_ASSERT_MSG(sec_lev == _sec_level, "security level inconsistent");
    AIR_ASSERT_MSG(poly_deg >= _least_poly_deg_pow, "poly deg is too small");
    AIR_ASSERT_MSG(poly_deg < _least_poly_deg_pow + _mod_bit_num.size(),
                   "poly deg is out of bound");
    return _mod_bit_num[poly_deg - _least_poly_deg_pow];
  }
  //! return min poly_deg at _sec_level and modulus bit number.
  uint64_t Min_poly_deg(uint32_t mod_bit_num) const {
    // check whether mod_bit_num is out of bound.
    AIR_ASSERT_MSG(mod_bit_num <= _mod_bit_num.back(),
                   "modulus bit number out of bound");

    uint64_t poly_deg = 1ULL << _least_poly_deg_pow;
    for (uint32_t id = 0; id < _mod_bit_num.size(); ++id) {
      if (mod_bit_num <= _mod_bit_num[id]) {
        break;
      }
      poly_deg = (poly_deg << 1U);
    }
    return poly_deg;
  }

  uint32_t Least_poly_deg_pow() const { return _least_poly_deg_pow; }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  MODULUS_INFO(void);
  MODULUS_INFO(const MODULUS_INFO&);
  MODULUS_INFO& operator=(const MODULUS_INFO&);

  SECURITY_LEVEL        _sec_level;
  uint32_t              _least_poly_deg_pow;
  std::vector<uint32_t> _mod_bit_num;
};

//! Context of SCHEME_INFO_ANA visitor.
class SCHEME_INFO_ANA_CTX : public air::base::ANALYZE_CTX {
public:
  using MUL_LEVEL_MAP = std::unordered_map<uint32_t, uint32_t>;
  SCHEME_INFO_ANA_CTX(CTX_PARAM*                   ctx_param,
                      const air::base::FUNC_SCOPE* func_scope,
                      uint32_t                     formal_mul_level)
      : _ctx_param(ctx_param),
        _func_scope(func_scope),
        _formal_mul_level(formal_mul_level) {}
  ~SCHEME_INFO_ANA_CTX(){};

  void Set_addr_datum_mul_level(air::base::ADDR_DATUM_ID addr_datum_id,
                                uint32_t                 level) {
    uint32_t id               = addr_datum_id.Index();
    _addr_datum_mul_level[id] = level;
    _max_mul_level            = std::max(_max_mul_level, level);
  }

  uint32_t Get_addr_datum_mul_level(air::base::ADDR_DATUM_ID addr_datum_id) {
    uint32_t                id   = addr_datum_id.Index();
    MUL_LEVEL_MAP::iterator iter = _addr_datum_mul_level.find(id);
    AIR_ASSERT(iter != _addr_datum_mul_level.end());
    return iter->second;
  }

  void Set_preg_mul_level(air::base::PREG_ID preg_id, uint32_t level) {
    uint32_t id         = preg_id.Index();
    _preg_mul_level[id] = level;
    _max_mul_level      = std::max(_max_mul_level, level);
  }

  uint32_t Get_preg_mul_level(air::base::PREG_ID preg_id) {
    uint32_t                id   = preg_id.Index();
    MUL_LEVEL_MAP::iterator iter = _preg_mul_level.find(id);
    AIR_ASSERT(iter != _preg_mul_level.end());
    return iter->second;
  }

  void Update_msg_len(uint32_t val) {
    _max_msg_len = (_max_msg_len > val ? _max_msg_len : val);
  }
  uint32_t         Formal_mul_level() const { return _formal_mul_level; }
  uint32_t         Get_max_msg_len() { return _max_msg_len; }
  uint32_t         Get_max_mul_level() const { return _max_mul_level; }
  CTX_PARAM*       Get_ctx_param() { return _ctx_param; }
  const CTX_PARAM* Get_ctx_param() const { return _ctx_param; }
  const air::base::FUNC_SCOPE* Func_scope() const { return _func_scope; }
  void                         Print(std::ostream& out);

private:
  // REQUIRED UNDEFINED UNWANTED methods
  SCHEME_INFO_ANA_CTX(void);
  SCHEME_INFO_ANA_CTX(const SCHEME_INFO_ANA_CTX&);
  SCHEME_INFO_ANA_CTX& operator=(const SCHEME_INFO_ANA_CTX&);

  const uint32_t _formal_mul_level = 0;  // mul_level of formal
  uint32_t       _max_mul_level    = 0;  // max mul_level in analyzed function
  uint32_t       _max_msg_len      = 0;  // max msg length in analyzed function
  MUL_LEVEL_MAP  _preg_mul_level;        // mul_level of each preg
  MUL_LEVEL_MAP  _addr_datum_mul_level;  // mul_level of each addr_datum
  CTX_PARAM*     _ctx_param = nullptr;   // ckks context paramter
  const air::base::FUNC_SCOPE* _func_scope =
      nullptr;  // analyzed function scope
};

//! Return value of SCHEME_INFO_ANA handlers.
class SCHEME_INFO_ANA_RETV {
public:
  SCHEME_INFO_ANA_RETV(void) : _mul_level(0) {}
  SCHEME_INFO_ANA_RETV(uint32_t level) : _mul_level(level) {}
  ~SCHEME_INFO_ANA_RETV() {}

  uint32_t Mul_level() const { return _mul_level; }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  SCHEME_INFO_ANA_RETV(const SCHEME_INFO_ANA_RETV&);
  SCHEME_INFO_ANA_RETV& operator=(const SCHEME_INFO_ANA_RETV&);

  uint32_t _mul_level = 0;  // mul_level of the result of handled node
};

//! Handler of core IR which analyzes mul_level and msg_len of
//! ciphertext type FORMAL/air::base::ADDR_DATUM/PREG.
class CORE_ANA_HANDLER : public air::core::DEFAULT_HANDLER {
public:
  template <typename RETV, typename VISITOR>
  RETV Handle_func_entry(VISITOR* visitor, air::base::NODE_PTR entry);
  template <typename RETV, typename VISITOR>
  RETV Handle_st(VISITOR* visitor, air::base::NODE_PTR st);
  template <typename RETV, typename VISITOR>
  RETV Handle_stp(VISITOR* visitor, air::base::NODE_PTR stp);
  template <typename RETV, typename VISITOR>
  RETV Handle_idname(VISITOR* visitor, air::base::NODE_PTR idname);

  template <typename RETV, typename VISITOR>
  RETV Handle_ld(VISITOR* visitor, air::base::NODE_PTR ld);
  template <typename RETV, typename VISITOR>
  RETV Handle_ldp(VISITOR* visitor, air::base::NODE_PTR ldp);

  template <typename RETV, typename VISITOR>
  RETV Handle_do_loop(VISITOR* visitor, air::base::NODE_PTR loop) {
    AIR_ASSERT_MSG(false, "handler of do_loop not implemented");
    return RETV(0);
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_ist(VISITOR* visitor, air::base::NODE_PTR ist) {
    AIR_ASSERT_MSG(false, "handler of istore not implemented");
    return RETV(0);
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_ild(VISITOR* visitor, air::base::NODE_PTR ild) {
    AIR_ASSERT_MSG(false, "handler of ild not implemented");
    return RETV(0);
  }
};

template <typename RETV, typename VISITOR>
RETV CORE_ANA_HANDLER::Handle_func_entry(VISITOR*            visitor,
                                         air::base::NODE_PTR entry) {
  for (uint32_t id = 0; id < entry->Num_child(); ++id) {
    air::base::NODE_PTR child = entry->Child(id);
    visitor->template Visit<RETV>(child);
  }
  uint32_t mul_level_of_func = visitor->Context().Get_max_mul_level();
  return RETV(mul_level_of_func);
}

template <typename RETV, typename VISITOR>
RETV CORE_ANA_HANDLER::Handle_st(VISITOR* visitor, air::base::NODE_PTR st) {
  // 1. handle rhs node
  air::base::NODE_PTR rhs_child = st->Child(0);
  RETV                res       = visitor->template Visit<RETV>(rhs_child);

  // 2. record mul_level of lhs addr_datum
  air::base::ADDR_DATUM_ID addr_datum_id = st->Addr_datum_id();
  visitor->Context().Set_addr_datum_mul_level(addr_datum_id, res.Mul_level());
  return RETV(res.Mul_level());
}

template <typename RETV, typename VISITOR>
RETV CORE_ANA_HANDLER::Handle_stp(VISITOR* visitor, air::base::NODE_PTR stp) {
  // 1. handle rhs node
  air::base::NODE_PTR rhs_child = stp->Child(0);
  RETV                res       = visitor->template Visit<RETV>(rhs_child);

  // 2. record mul_level of lhs preg
  air::base::PREG_ID preg_id = stp->Preg_id();
  visitor->Context().Set_preg_mul_level(preg_id, res.Mul_level());
  return RETV(res.Mul_level());
}

template <typename RETV, typename VISITOR>
RETV CORE_ANA_HANDLER::Handle_idname(VISITOR*            visitor,
                                     air::base::NODE_PTR idname) {
  air::base::ADDR_DATUM_PTR formal = idname->Addr_datum();

  // 1. update msg length in current function
  air::base::TYPE_PTR type = formal->Type();
  AIR_ASSERT(type->Is_array());
  uint32_t             msg_len = type->Cast_to_arr()->Elem_count();
  SCHEME_INFO_ANA_CTX& ctx     = visitor->Context();
  ctx.Update_msg_len(msg_len);

  // 2. set mul_level of formals as zero
  const uint32_t           formal_mul_level     = ctx.Formal_mul_level();
  air::base::ADDR_DATUM_ID formal_addr_datum_id = formal->Id();
  ctx.Set_addr_datum_mul_level(formal_addr_datum_id, formal_mul_level);
  return RETV(formal_mul_level);
}

template <typename RETV, typename VISITOR>
RETV CORE_ANA_HANDLER::Handle_ld(VISITOR* visitor, air::base::NODE_PTR ld) {
  air::base::ADDR_DATUM_ID addr_datum_id = ld->Addr_datum_id();
  uint32_t                 mul_level =
      visitor->Context().Get_addr_datum_mul_level(addr_datum_id);
  return RETV(mul_level);
}

template <typename RETV, typename VISITOR>
RETV CORE_ANA_HANDLER::Handle_ldp(VISITOR* visitor, air::base::NODE_PTR ldp) {
  air::base::PREG_ID preg_id   = ldp->Preg_id();
  uint32_t           mul_level = visitor->Context().Get_preg_mul_level(preg_id);
  return RETV(mul_level);
}

//! Handler of tensor IR which analyzes and updates mul_level and msg_len of
//! tensor operation results.
class TENSOR_ANA_HANDLER : public nn::core::DEFAULT_HANDLER {
public:
  template <typename RETV, typename VISITOR>
  RETV Handle_add(VISITOR* visitor, air::base::NODE_PTR add);
  template <typename RETV, typename VISITOR>
  RETV Handle_average_pool(VISITOR* visitor, air::base::NODE_PTR avg_pool);
  template <typename RETV, typename VISITOR>
  RETV Handle_conv(VISITOR* visitor, air::base::NODE_PTR conv);
  template <typename RETV, typename VISITOR>
  RETV Handle_flatten(VISITOR* visitor, air::base::NODE_PTR flatten);
  template <typename RETV, typename VISITOR>
  RETV Handle_gemm(VISITOR* visitor, air::base::NODE_PTR gemm);
  template <typename RETV, typename VISITOR>
  RETV Handle_global_average_pool(VISITOR*            visitor,
                                  air::base::NODE_PTR glob_avg_pool);
  template <typename RETV, typename VISITOR>
  RETV Handle_max_pool(VISITOR* visitor, air::base::NODE_PTR max_pool);
  template <typename RETV, typename VISITOR>
  RETV Handle_mul(VISITOR* visitor, air::base::NODE_PTR mul);
  template <typename RETV, typename VISITOR>
  RETV Handle_relu(VISITOR* visitor, air::base::NODE_PTR relu);
  template <typename RETV, typename VISITOR>
  RETV Handle_reshape(VISITOR* visitor, air::base::NODE_PTR reshape);
  template <typename RETV, typename VISITOR>
  RETV Handle_strided_slice(VISITOR*            visitor,
                            air::base::NODE_PTR strided_slice);
  template <typename RETV, typename VISITOR>
  RETV Handle_sub(VISITOR* visitor, air::base::NODE_PTR sub);

private:
  //! return mul_level consumed by tensor operation.
  uint32_t Mul_level_of_tensor_op(air::base::OPCODE opc);
  //! return element number of the result of the node. rtype of node must be
  //! array. msg_len is elem_count of the array type.
  uint64_t Msg_len(air::base::NODE_PTR node) {
    air::base::TYPE_PTR rtype = node->Rtype();
    AIR_ASSERT(rtype->Is_array());
    uint64_t msg_len = rtype->Cast_to_arr()->Elem_count();
    return msg_len;
  }
};

template <typename RETV, typename VISITOR>
RETV TENSOR_ANA_HANDLER::Handle_add(VISITOR* visitor, air::base::NODE_PTR add) {
  // 1. cal mul_level of child nodes
  air::base::NODE_PTR child0 = add->Child(0);
  uint32_t mul_level0 = visitor->template Visit<RETV>(child0).Mul_level();
  air::base::NODE_PTR child1 = add->Child(1);
  uint32_t mul_level1 = visitor->template Visit<RETV>(child1).Mul_level();

  // 2. add not inc mul_level.
  // mul_level of add result is the bigger one of mul_level0 and mul_level1
  uint32_t mul_level = std::max(mul_level0, mul_level1);
  return RETV(mul_level);
}

template <typename RETV, typename VISITOR>
RETV TENSOR_ANA_HANDLER::Handle_average_pool(VISITOR*            visitor,
                                             air::base::NODE_PTR avg_pool) {
  // 1. update msg length of current function
  visitor->Context().Update_msg_len(Msg_len(avg_pool));

  // 2. cal and return mul_level of avg_pool result
  air::base::NODE_PTR data_child = avg_pool->Child(0);
  uint32_t input_level = visitor->template Visit<RETV>(data_child).Mul_level();
  uint32_t mul_level_of_avg_pool =
      input_level + Mul_level_of_tensor_op(nn::core::OPC_AVERAGE_POOL);
  return RETV(mul_level_of_avg_pool);
}

template <typename RETV, typename VISITOR>
RETV TENSOR_ANA_HANDLER::Handle_conv(VISITOR*            visitor,
                                     air::base::NODE_PTR conv) {
  // 1. update msg length of current function
  visitor->Context().Update_msg_len(Msg_len(conv));

  // 2. cal and return mul_level of conv result
  air::base::NODE_PTR data_child = conv->Child(0);
  uint32_t input_level = visitor->template Visit<RETV>(data_child).Mul_level();
  uint32_t mul_level_of_conv =
      input_level + Mul_level_of_tensor_op(nn::core::OPC_CONV);

  return RETV(mul_level_of_conv);
}

template <typename RETV, typename VISITOR>
RETV TENSOR_ANA_HANDLER::Handle_flatten(VISITOR*            visitor,
                                        air::base::NODE_PTR flatten) {
  // 1. update msg length of current function
  visitor->Context().Update_msg_len(Msg_len(flatten));

  // 2. cal and return mul_level of flatten result
  air::base::NODE_PTR data_child = flatten->Child(0);
  uint32_t input_level = visitor->template Visit<RETV>(data_child).Mul_level();

  uint32_t mul_level_of_flatten =
      input_level + Mul_level_of_tensor_op(nn::core::OPC_FLATTEN);
  return RETV(mul_level_of_flatten);
}

template <typename RETV, typename VISITOR>
RETV TENSOR_ANA_HANDLER::Handle_gemm(VISITOR*            visitor,
                                     air::base::NODE_PTR gemm) {
  // 1. update msg length of current function
  visitor->Context().Update_msg_len(Msg_len(gemm));

  // 2. cal and return mul_level of gemm result
  air::base::NODE_PTR data_child = gemm->Child(0);
  uint32_t input_level = visitor->template Visit<RETV>(data_child).Mul_level();
  uint32_t mul_level_of_gemm =
      input_level + Mul_level_of_tensor_op(nn::core::OPC_GEMM);
  return RETV(mul_level_of_gemm);
}

template <typename RETV, typename VISITOR>
RETV TENSOR_ANA_HANDLER::Handle_global_average_pool(
    VISITOR* visitor, air::base::NODE_PTR glob_avg_pool) {
  // 1. update msg length of current function
  visitor->Context().Update_msg_len(Msg_len(glob_avg_pool));

  // 2. cal and return mul_level of glob_avg_pool result
  air::base::NODE_PTR data_child = glob_avg_pool->Child(0);
  uint32_t input_level = visitor->template Visit<RETV>(data_child).Mul_level();
  uint32_t mul_level_of_glob_avg_pool =
      input_level + Mul_level_of_tensor_op(nn::core::OPC_GLOBAL_AVERAGE_POOL);
  return RETV(mul_level_of_glob_avg_pool);
}

template <typename RETV, typename VISITOR>
RETV TENSOR_ANA_HANDLER::Handle_max_pool(VISITOR*            visitor,
                                         air::base::NODE_PTR max_pool) {
  // 1. update msg length of current function
  visitor->Context().Update_msg_len(Msg_len(max_pool));

  // 2. cal and return mul_level of max_pool result
  air::base::NODE_PTR data_child = max_pool->Child(0);
  uint32_t input_level = visitor->template Visit<RETV>(data_child).Mul_level();
  uint32_t mul_level_of_max_pool =
      input_level + Mul_level_of_tensor_op(nn::core::OPC_MAX_POOL);
  return RETV(mul_level_of_max_pool);
}

template <typename RETV, typename VISITOR>
RETV TENSOR_ANA_HANDLER::Handle_mul(VISITOR* visitor, air::base::NODE_PTR mul) {
  // 1. update msg length of current function
  visitor->Context().Update_msg_len(Msg_len(mul));

  // 2. cal and return mul_level of mul result
  // 2.1 handle child0
  air::base::NODE_PTR child0 = mul->Child(0);
  uint32_t mul_level0 = visitor->template Visit<RETV>(child0).Mul_level();

  // 2.2 handle child1
  air::base::NODE_PTR child1 = mul->Child(1);
  uint32_t mul_level1 = visitor->template Visit<RETV>(child1).Mul_level();

  // 2.3 mul inc mul_level by 1
  uint32_t mul_level = std::max(mul_level0, mul_level1) +
                       Mul_level_of_tensor_op(nn::core::OPC_MUL);
  return RETV(mul_level);
}

template <typename RETV, typename VISITOR>
RETV TENSOR_ANA_HANDLER::Handle_relu(VISITOR*            visitor,
                                     air::base::NODE_PTR relu) {
  // 1. update msg length of current function
  SCHEME_INFO_ANA_CTX& ctx = visitor->Context();
  ctx.Update_msg_len(Msg_len(relu));

  // 2. cal and return mul_level of relu result.
  // Current impl inserts bootstrapping before each relu.
  // Therefore, input of relu is result of bootstrapping.
  uint32_t bts_mul_level = ctx.Get_ctx_param()->Mul_depth_of_bootstrap();
  uint32_t mul_level_of_relu =
      bts_mul_level + Mul_level_of_tensor_op(nn::core::OPC_RELU);
  return RETV(mul_level_of_relu);
}

template <typename RETV, typename VISITOR>
RETV TENSOR_ANA_HANDLER::Handle_reshape(VISITOR*            visitor,
                                        air::base::NODE_PTR reshape) {
  // 1. update msg length of current function
  visitor->Context().Update_msg_len(Msg_len(reshape));

  // 2. cal and return mul_level of reshape result
  air::base::NODE_PTR data_child = reshape->Child(0);
  uint32_t input_level = visitor->template Visit<RETV>(data_child).Mul_level();
  uint32_t mul_level_of_reshape =
      input_level + Mul_level_of_tensor_op(nn::core::OPC_RESHAPE);
  return RETV(mul_level_of_reshape);
}

template <typename RETV, typename VISITOR>
RETV TENSOR_ANA_HANDLER::Handle_strided_slice(
    VISITOR* visitor, air::base::NODE_PTR strided_slice) {
  // 1. update msg length of current function
  visitor->Context().Update_msg_len(Msg_len(strided_slice));

  // 2. cal and return mul_level of strided_slice result
  air::base::NODE_PTR data_child = strided_slice->Child(0);
  uint32_t input_level = visitor->template Visit<RETV>(data_child).Mul_level();
  uint32_t mul_level_of_strided_slice =
      input_level + Mul_level_of_tensor_op(nn::core::OPC_STRIDED_SLICE);
  return RETV(mul_level_of_strided_slice);
}

template <typename RETV, typename VISITOR>
RETV TENSOR_ANA_HANDLER::Handle_sub(VISITOR* visitor, air::base::NODE_PTR sub) {
  // 1. cal mul_level of child nodes
  air::base::NODE_PTR child0 = sub->Child(0);
  uint32_t mul_level0 = visitor->template Visit<RETV>(child0).Mul_level();
  air::base::NODE_PTR child1 = sub->Child(1);
  uint32_t mul_level1 = visitor->template Visit<RETV>(child1).Mul_level();

  // 2. sub not inc mul_level.
  // mul_level of sub result is the bigger one of mul_level0 and mul_level1
  uint32_t mul_level = std::max(mul_level0, mul_level1);
  return RETV(mul_level);
}

//! Analyzer of scheme info, of which input is TENSOR IR.
//! SCHEME_INFO_ANA analyze and update scheme info that should be used
//! in encrypting current program.
class SCHEME_INFO_ANA {
public:
  SCHEME_INFO_ANA(air::base::GLOB_SCOPE*   glob_scope,
                  air::driver::DRIVER_CTX* driver_ctx,
                  SCHEME_INFO_CONFIG*      config)
      : _glob_scope(glob_scope), _driver_ctx(driver_ctx), _config(config) {}
  ~SCHEME_INFO_ANA() {}

  using CORE_HANDLER   = air::core::HANDLER<CORE_ANA_HANDLER>;
  using TENSOR_HANDLER = nn::core::HANDLER<TENSOR_ANA_HANDLER>;
  using ANA_VISITOR =
      air::base::VISITOR<SCHEME_INFO_ANA_CTX, CORE_HANDLER, TENSOR_HANDLER>;

  R_CODE Run();

private:
  // REQUIRED UNDEFINED UNWANTED methods
  SCHEME_INFO_ANA(void);
  SCHEME_INFO_ANA(const SCHEME_INFO_ANA&);
  SCHEME_INFO_ANA& operator=(const SCHEME_INFO_ANA&);

  //! traverse function, cal max value of mul_level and msg_len in current
  //! function.
  R_CODE Ana_func(const air::base::FUNC_SCOPE& func_scope);
  //! update scheme_info with config
  void Update_scheme_info_with_config();
  //! update scheme_info with analyzing results
  void Update_scheme_info_with_ana_res();

  void Update_msg_len(uint32_t msg_len) {
    _max_msg_len = (_max_msg_len > msg_len ? _max_msg_len : msg_len);
  }
  void Update_mul_lev(uint32_t mul_lev) {
    _max_mul_lev = (_max_mul_lev > mul_lev ? _max_mul_lev : mul_lev);
  }
  air::base::GLOB_SCOPE*   Glob_scope() const { return _glob_scope; }
  air::driver::DRIVER_CTX* Driver_ctx() const { return _driver_ctx; }
  uint32_t                 Max_msg_len() const { return _max_msg_len; }
  uint32_t                 Max_mul_lev() const { return _max_mul_lev; }
  CTX_PARAM&               Ctx_param() { return _ctx_param; }
  SCHEME_INFO_CONFIG*      Config() const { return _config; }

  DECLARE_TRACE_DETAIL_API((*_config), _driver_ctx)

  air::driver::DRIVER_CTX* _driver_ctx;
  SCHEME_INFO_CONFIG*      _config;
  air::base::GLOB_SCOPE*   _glob_scope;
  uint32_t  _max_msg_len = 0;  // max msg_len in current glob_scope
  uint32_t  _max_mul_lev = 0;  // max mul_lev in current glob_scope
  CTX_PARAM _ctx_param;        // TODO: store ctx_param in TARGET_INFO
};

}  // namespace core
}  // namespace fhe
#endif
