//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CORE_LOWER_CTX_H
#define FHE_CORE_LOWER_CTX_H

#include <sys/types.h>

#include <cstdint>
#include <functional>
#include <set>
#include <unordered_map>

#include "air/base/container.h"
#include "air/base/st.h"
#include "air/base/transform_ctx.h"
#include "fhe/core/lower_cfg.h"
#include "fhe/core/rt_context.h"
#include "fhe/core/scheme_info.h"
#include "nn/core/data_scheme.h"

namespace fhe {
namespace core {

enum FHE_TYPE_KIND : uint8_t {
  INVALID  = 0,
  RNS_POLY = 1,  // a group of polynomials composed by different RNS base
  CIPHER   = 2,
  CIPHER3  = 3,  // ciphertext contains 3 RNS_POLY
  PLAIN    = 4,
  LEVEL_T  = 5,  // level of ciphertext or plaintext
  SCALE_T  = 6,  // scale of ciphertext or plaintext for CKKS
  POLY     = 7,  // polynomial
  END,
};

enum FHE_FUNC : uint32_t {
  APPROX_RELU    = 0,
  ROTATE         = 1,
  RELIN          = 2,
  RNS_ADD        = 3,
  RNS_SUB        = 4,
  RNS_MUL        = 5,
  RNS_ROTATE     = 6,
  RNS_ADD_EXT    = 7,
  RNS_SUB_EXT    = 8,
  RNS_MUL_EXT    = 9,
  RNS_ROTATE_EXT = 10,
  FHE_FUNC_END   = 11,
};

#define INVALID_MUL_DEPTH -1
//! Mul_depth indicates the count of FHE::mul operations executed since program
//! entry.
#define INIT_MUL_DEPTH 0
//! Same with HECATE, rescale_level indicates the number of consumed
//! scale_factor.
#define INIT_RESCALE_LEVEL 0
//! Mul_level matches rtlib, indicating the q prime count in a ciphertext.
#define INIT_MUL_LEVEL 1

//! Attribute names available in FHE phase
class FHE_ATTR_KIND {
public:
  static constexpr const char* SCALE         = "scale";
  static constexpr const char* LEVEL         = "level";
  static constexpr const char* MUL_DEPTH     = "mul_depth";
  static constexpr const char* RESCALE_LEVEL = "rescale_level";
  // bool, annotate if contains p primes
  static constexpr const char* EXTENDED = "extended";
  // bool, annotate for ciphertext polynomial multiply
  static constexpr const char* MUL_CIPH = "mul_ciph";
  // bool, annotate if contains precomputed value
  static constexpr const char* PRECOMPUTE = "precompute";
  // integer, the RNS base start index
  static constexpr const char* S_BASE = "s_base";
  // integer, the number of p primes
  static constexpr const char* NUM_P = "num_p";
  // bool, annotate if polynomial under coefficient mode
  static constexpr const char* COEFF_MODE = "coeff_mode";
};

//! info of function gen in FHE phase
class FHE_FUNC_INFO {
public:
  FHE_FUNC_INFO(void) {}
  FHE_FUNC_INFO(air::base::FUNC_ID func_id, uint32_t mul_depth)
      : _func_id(func_id), _mul_depth(mul_depth) {}
  ~FHE_FUNC_INFO() {}

  void Set_func_id(air::base::FUNC_ID func_id) { _func_id = func_id; }
  air::base::FUNC_ID Get_func_id() const { return _func_id; }
  void               Set_mul_depth(uint32_t mul_depth) {
    // mul_depth of function cannot be changed
    if (_mul_depth != INVALID_MUL_DEPTH) {
      AIR_ASSERT(_mul_depth == mul_depth);
    }
    _mul_depth = mul_depth;
  }
  uint32_t Get_mul_depth() const {
    // cannot get mul_depth of fhe func before set it
    AIR_ASSERT(_mul_depth != INVALID_MUL_DEPTH);
    return _mul_depth;
  }
  air::base::FUNC_SCOPE* Get_func_scope(air::base::GLOB_SCOPE* glob_scope) {
    if (_func_id == air::base::FUNC_ID()) {
      return nullptr;
    }
    return &glob_scope->Open_func_scope(_func_id);
  }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  FHE_FUNC_INFO(const FHE_FUNC_INFO&);
  FHE_FUNC_INFO& operator=(const FHE_FUNC_INFO&);

  air::base::FUNC_ID _func_id   = air::base::FUNC_ID();
  int32_t            _mul_depth = INVALID_MUL_DEPTH;
};

//! @brief CRT_CST holds compile time generated CRT constant data
class CRT_CST {
public:
  CRT_CST() = default;
  ~CRT_CST() {}

  static constexpr const char* QMUK_ARR_NAME  = "QMUK_ARR";
  static constexpr uint32_t    QMUK_ELEM_CNT  = 3;  // Q/MU/K
  static constexpr uint32_t    QMUK_ELEM_SIZE = QMUK_ELEM_CNT * sizeof(int64_t);
  static constexpr const char* QMUK_NAME      = "QMUK";

  static constexpr const char* QPRIMES_NAME    = "QPRIMES";
  static constexpr const char* PPRIMES_NAME    = "PPRIMES";
  static constexpr const char* QLHINVMODQ_NAME = "QLHINVMODQ";
  static constexpr const char* QLHMODP_NAME    = "QLHMODP";
  static constexpr const char* QLINVMODQ_NAME  = "QLINVMODQ";
  static constexpr const char* QLHALFMODQ_NAME = "QLHALFMODQ";
  static constexpr const char* Q0HALFMODQ_NAME = "Q0HALFMODQ";
  static constexpr const char* PHMODQ_NAME     = "PHMODQ";
  static constexpr const char* PHINVMODP_NAME  = "PHINVMODP";
  static constexpr const char* PINVMODQ_NAME   = "PINVMODQ";

  air::base::CONSTANT_ID Get_qmuk() const { return _qmuk; }
  void                   Set_qmuk(air::base::CONSTANT_PTR cst) {
    AIR_ASSERT_MSG(_qmuk == air::base::CONSTANT_ID(),
                                     "QMUK constant already set");
    _qmuk = cst->Id();
  }

  //! @brief Get or create constant for Q primes
  air::base::CONSTANT_PTR Get_q(air::base::GLOB_SCOPE* gs);
  //! @brief Get constant for Q primes
  air::base::CONSTANT_PTR Get_q(air::base::GLOB_SCOPE* gs) const {
    AIR_ASSERT(_q != air::base::CONSTANT_ID())
    return gs->Constant(_q);
  }

  //! @brief Get or create constant for P primes
  air::base::CONSTANT_PTR Get_p(air::base::GLOB_SCOPE* gs);
  //! @brief Get constant for P primes
  air::base::CONSTANT_PTR Get_p(air::base::GLOB_SCOPE* gs) const {
    AIR_ASSERT(_p != air::base::CONSTANT_ID())
    return gs->Constant(_p);
  }

  //! @brief Get prime value at specified index, the primes are
  //! composed by Q_PRIMES + P_PRIMES, while idx greater than Q_PRIMES's
  //! length, P prime will be returned
  uint64_t Get_prime(air::base::GLOB_SCOPE* gs, uint32_t idx);

  //! @brief Get prime value at specified index, assert if primes not yet setup
  uint64_t Get_prime(air::base::GLOB_SCOPE* gs, uint32_t idx) const;

  //! @brief Get or create constant for qlhinvmodq used in HPOLY.precomp
  air::base::CONSTANT_PTR Get_qlhinvmodq(air::base::GLOB_SCOPE* gs,
                                         uint32_t               part_idx,
                                         uint32_t               part_size_idx);

  //! @brief Get or create constant for qlhmodp used in HPOLY.bconv(precomp)
  air::base::CONSTANT_PTR Get_qlhmodp(air::base::GLOB_SCOPE* gs, uint32_t q_idx,
                                      uint32_t part_idx, uint32_t start_idx,
                                      uint32_t len);

  //! @brief Get or create constant for qlinvmodq used in HPOLY.rescale
  air::base::CONSTANT_PTR Get_qlinvmodq(air::base::GLOB_SCOPE* gs,
                                        uint32_t               idx);

  //! @brief Get or create constant for phmodq used in HPOLY.bconv(mod_down)
  air::base::CONSTANT_PTR Get_phmodq(air::base::GLOB_SCOPE* gs);

  //! @brief Get or create constant for qlhalfmodq used in
  //! HPOLY.bswitch(rescale)
  air::base::CONSTANT_PTR Get_qlhalfmodq(air::base::GLOB_SCOPE* gs,
                                         uint32_t               idx);

  //! @brief Get or create constant for q0halfmodq used in
  //! HPOLY.bswitch(raise_mod)
  air::base::CONSTANT_PTR Get_q0halfmodq(air::base::GLOB_SCOPE* gs);

  //! @brief Get or create constant for qhinvmodp used in HPOLY.mod_down
  air::base::CONSTANT_PTR Get_phinvmodp(air::base::GLOB_SCOPE* gs);

  //! @brief Get or create constant for pinvmodq used in HPOLY.mod_down
  air::base::CONSTANT_PTR Get_pinvmodq(air::base::GLOB_SCOPE* gs);

private:
  void Set_q(air::base::CONSTANT_PTR cst) {
    AIR_ASSERT_MSG(_q == air::base::CONSTANT_ID(), "Q constant already set");
    _q = cst->Id();
  }

  void Set_p(air::base::CONSTANT_PTR cst) {
    AIR_ASSERT_MSG(_p == air::base::CONSTANT_ID(), "P constant already set");
    _p = cst->Id();
  }

  void Set_phmodq(air::base::CONSTANT_PTR cst) {
    AIR_ASSERT_MSG(_phmodq == air::base::CONSTANT_ID(),
                   "phmodq constant already set");
    _phmodq = cst->Id();
  }

  void Set_phinvmodp(air::base::CONSTANT_PTR cst) {
    AIR_ASSERT_MSG(_phinvmodp == air::base::CONSTANT_ID(),
                   "phinvmodp constant already set");
    _phinvmodp = cst->Id();
  }

  void Set_pinvmodq(air::base::CONSTANT_PTR cst) {
    AIR_ASSERT_MSG(_pinvmodq == air::base::CONSTANT_ID(),
                   "pinvmodq constant already set");
    _pinvmodq = cst->Id();
  }

  void Add_qlhinvmodq(uint64_t key, air::base::CONSTANT_PTR cst) {
    _qlhinvmodq_map[key] = cst->Id();
  }
  void Add_qlinvmodq(uint32_t key, air::base::CONSTANT_PTR cst) {
    _qlinvmodq_map[key] = cst->Id();
  }

  void Add_qlhmodp(uint32_t key, air::base::CONSTANT_PTR cst) {
    _qlhmodp_map[key] = cst->Id();
  }

  void Add_qlhalfmodq_map(uint32_t key, air::base::CONSTANT_PTR cst) {
    _qlhalfmodq_map[key] = cst->Id();
  }

  air::base::CONSTANT_PTR Create_array_cst(air::base::GLOB_SCOPE* gs,
                                           std::string            name_str,
                                           const uint64_t* data, size_t len);

  air::base::CONSTANT_PTR Create_2d_array_cst(
      air::base::GLOB_SCOPE* gs, std::string name_str,
      std::vector<fhe::rt_context::VL>& vec2d, bool transpose = false);

  // constant ID for q primes
  air::base::CONSTANT_ID _q = air::base::CONSTANT_ID();
  // constant ID for p primes
  air::base::CONSTANT_ID _p = air::base::CONSTANT_ID();
  // constant ID for $phat \mod q$
  air::base::CONSTANT_ID _phmodq = air::base::CONSTANT_ID();
  // constant ID for $phat^{-1} \mod p$
  air::base::CONSTANT_ID _phinvmodp = air::base::CONSTANT_ID();
  // constant ID for $p^{-1} \mod q$
  air::base::CONSTANT_ID _pinvmodq = air::base::CONSTANT_ID();
  // constant ID for modulus q/mu/k
  air::base::CONSTANT_ID _qmuk = air::base::CONSTANT_ID();
  // constant mapping for $q_lhat^{-1} \mod q$
  std::map<uint64_t, air::base::CONSTANT_ID> _qlhinvmodq_map;
  // constant mapping for $q_l^{-1} \mod q$
  std::map<uint32_t, air::base::CONSTANT_ID> _qlinvmodq_map;
  // constant mapping for $q_lhat \mod p$
  std::map<uint64_t, air::base::CONSTANT_ID> _qlhmodp_map;
  // constant mapping for $q_l/2 \mod q$
  std::map<uint32_t, air::base::CONSTANT_ID> _qlhalfmodq_map;
};

class LOWER_CTX {
public:
  LOWER_CTX() = default;
  ~LOWER_CTX() { Release_rt_context(); }

  const CTX_PARAM& Get_ctx_param() const { return _ctx_param; }

  CTX_PARAM& Get_ctx_param() { return _ctx_param; }

  const CRT_CST& Get_crt_cst() const { return _crt_cst; }
  CRT_CST&       Get_crt_cst() { return _crt_cst; }

  void Set_cipher_type_id(air::base::TYPE_ID cipher_type_id) {
    _fhe_type[FHE_TYPE_KIND::CIPHER] = cipher_type_id;
  }

  air::base::TYPE_ID Get_cipher_type_id() const {
    air::base::TYPE_ID cipher_type_id = _fhe_type[FHE_TYPE_KIND::CIPHER];
    CMPLR_ASSERT(cipher_type_id != air::base::TYPE_ID(),
                 "use cipher type before create it");
    return cipher_type_id;
  }

  air::base::TYPE_PTR Get_cipher_type(air::base::GLOB_SCOPE* glob_scope) const {
    return glob_scope->Type(Get_cipher_type_id());
  }

  void Set_plain_type_id(air::base::TYPE_ID plain_type_id) {
    _fhe_type[FHE_TYPE_KIND::PLAIN] = plain_type_id;
  }

  air::base::TYPE_ID Get_plain_type_id() const {
    air::base::TYPE_ID plain_type_id = _fhe_type[FHE_TYPE_KIND::PLAIN];
    CMPLR_ASSERT(plain_type_id != air::base::TYPE_ID(),
                 "use plain type before create it");
    return plain_type_id;
  }

  air::base::TYPE_PTR Get_plain_type(air::base::GLOB_SCOPE* glob_scope) const {
    return glob_scope->Type(Get_plain_type_id());
  }

  air::base::TYPE_PTR Get_level_type(air::base::GLOB_SCOPE* glob_scope) {
    return Find_or_create_type(FHE_TYPE_KIND::LEVEL_T, "LEVEL_T", glob_scope);
  }

  air::base::TYPE_PTR Get_scale_type(air::base::GLOB_SCOPE* glob_scope) {
    return Find_or_create_type(FHE_TYPE_KIND::SCALE_T, "SCALE_T", glob_scope);
  }

  void Set_rns_poly_type_id(air::base::TYPE_ID ty_id) {
    _fhe_type[FHE_TYPE_KIND::RNS_POLY] = ty_id;
  }

  air::base::TYPE_ID Get_rns_poly_type_id() const {
    air::base::TYPE_ID type_id = _fhe_type[FHE_TYPE_KIND::RNS_POLY];
    AIR_ASSERT_MSG(type_id != air::base::TYPE_ID(),
                   "use RNS_POLY type before create it");
    return type_id;
  }

  air::base::TYPE_PTR Get_rns_poly_type(
      air::base::GLOB_SCOPE* glob_scope) const {
    return glob_scope->Type(Get_rns_poly_type_id());
  }

  void Set_poly_type_id(air::base::TYPE_ID ty_id) {
    AIR_ASSERT_MSG(_fhe_type[FHE_TYPE_KIND::POLY] == air::base::TYPE_ID(),
                   "POLY type already set");
    _fhe_type[FHE_TYPE_KIND::POLY] = ty_id;
  }

  air::base::TYPE_ID Get_poly_type_id() const {
    air::base::TYPE_ID lpoly_type_id = _fhe_type[FHE_TYPE_KIND::POLY];
    AIR_ASSERT_MSG(lpoly_type_id != air::base::TYPE_ID(),
                   "use POLY type before create it");
    return lpoly_type_id;
  }

  air::base::TYPE_PTR Get_poly_type(air::base::GLOB_SCOPE* glob_scope) const {
    return glob_scope->Type(Get_poly_type_id());
  }

  void Set_cipher3_type_id(air::base::TYPE_ID cipher3_type_id) {
    _fhe_type[FHE_TYPE_KIND::CIPHER3] = cipher3_type_id;
  }

  air::base::TYPE_ID Get_cipher3_type_id() const {
    air::base::TYPE_ID cipher3_type_id = _fhe_type[FHE_TYPE_KIND::CIPHER3];
    CMPLR_ASSERT(cipher3_type_id != air::base::TYPE_ID(),
                 "use cipher3 type before create it");
    return cipher3_type_id;
  }

  air::base::TYPE_PTR Get_cipher3_type(
      air::base::GLOB_SCOPE* glob_scope) const {
    return glob_scope->Type(Get_cipher3_type_id());
  }

  air::base::TYPE_PTR Convert_to_cipher_type(
      air::base::GLOB_SCOPE* glob_scope, air::base::TYPE_PTR vec_type) const {
    // 1. non-array type: return the type itself
    if (!vec_type->Is_array()) {
      return vec_type;
    }

    // 2. array type: return cipher or vector of cipher
    air::base::TYPE_PTR       ciph_type  = Get_cipher_type(glob_scope);
    air::base::ARRAY_TYPE_PTR array_type = vec_type->Cast_to_arr();
    air::base::TYPE_PTR       elem_type  = array_type->Elem_type();

    // 2.1 for vector of float, return cipher
    if (elem_type->Is_float()) {
      return ciph_type;
    }

    // 2.2 for vector of vector type, return vector of cipher
    AIR_ASSERT(elem_type->Is_array());
    air::base::ARRAY_TYPE_PTR elem_array_type = elem_type->Cast_to_arr();
    AIR_ASSERT(elem_array_type->Elem_type()->Is_float());
    // AIR_ASSERT_MSG(elem_array_type->Dim() == 1,
    //                "not support multi-dim ciphertext array");
    // gen array type name
    uint32_t    ciph_cnt = array_type->Elem_count();
    std::string array_name("cipher_" + std::to_string(ciph_cnt));
    return glob_scope->New_arr_type(array_name.c_str(), ciph_type, {ciph_cnt},
                                    glob_scope->Unknown_simple_spos());
  }

  FHE_FUNC_INFO& Get_func_info(FHE_FUNC fhe_func) {
    return _fhe_func_info[static_cast<uint32_t>(fhe_func)];
  }

  const FHE_FUNC_INFO& Get_func_info(FHE_FUNC fhe_func) const {
    return _fhe_func_info[static_cast<uint32_t>(fhe_func)];
  }

  void Set_func_info(FHE_FUNC fhe_func, air::base::FUNC_ID id,
                     uint32_t depth = 0) {
    FHE_FUNC_INFO& func_info = Get_func_info(fhe_func);
    func_info.Set_func_id(id);
    func_info.Set_mul_depth(depth);
  }

  bool Is_cipher_type(air::base::TYPE_ID ty_id) const {
    return ty_id == Get_cipher_type_id();
  }

  bool Is_cipher3_type(air::base::TYPE_ID ty_id) const {
    return ty_id == Get_cipher3_type_id();
  }

  bool Is_plain_type(air::base::TYPE_ID ty_id) const {
    return ty_id == Get_plain_type_id();
  }

  bool Is_rns_poly_type(air::base::TYPE_ID ty_id) const {
    return ty_id == Get_rns_poly_type_id();
  }
  bool Is_poly_type(air::base::TYPE_ID ty_id) const {
    return ty_id == Get_poly_type_id();
  }

  //! @brief Register runtime context
  //! invoke Register_rt_context on demand to minimize compile time
  void Register_rt_context() const;

  //! @brief Release runtime context
  //! invoke Release_rt_context when destroying LOWER_CTX to prevent multiple
  //! calls to Register_rt_context
  void Release_rt_context() const;

  void Emit_cfg(air::base::FUNC_SCOPE* func_scope, std::ostream& ofile) {
    _cfg.Emit_cfg(func_scope, Get_ctx_param());
    _cfg.Dump(ofile);
  }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  LOWER_CTX(const LOWER_CTX&);
  LOWER_CTX& operator=(const LOWER_CTX&);

  air::base::TYPE_PTR Find_or_create_type(FHE_TYPE_KIND id, const char* name,
                                          air::base::GLOB_SCOPE* glob) {
    if (_fhe_type[id] != air::base::Null_ptr) {
      return glob->Type(_fhe_type[id]);
    }
    air::base::RECORD_TYPE_PTR type = glob->New_rec_type(
        air::base::RECORD_KIND::STRUCT, name, glob->Unknown_simple_spos());
    _fhe_type[id] = type->Id();
    return type;
  }

  CTX_PARAM          _ctx_param;  // cipher crypto context paramters
  CRT_CST            _crt_cst;    // compile time CRT constant data
  air::base::TYPE_ID _fhe_type[FHE_TYPE_KIND::END];  // type id gen in fhe
  FHE_FUNC_INFO
  _fhe_func_info[FHE_FUNC::FHE_FUNC_END];  // info of func gen in fhe
  LOWER_CFG _cfg;
};

//! @brief Context for FHE lowering phases
class FHE_LOWER_CTX : public air::base::TRANSFORM_CTX {
public:
  FHE_LOWER_CTX(air::base::CONTAINER* cntr, LOWER_CTX& ctx)
      : air::base::TRANSFORM_CTX(cntr), _fhe_ctx(ctx) {}

  bool Is_cipher_type(air::base::TYPE_ID ty_id) const {
    return _fhe_ctx.Is_cipher_type(ty_id);
  }
  bool Is_cipher3_type(air::base::TYPE_ID ty_id) const {
    return _fhe_ctx.Is_cipher3_type(ty_id);
  }
  bool Is_plain_type(air::base::TYPE_ID ty_id) const {
    return _fhe_ctx.Is_plain_type(ty_id);
  }
  bool Is_rns_poly_type(air::base::TYPE_ID ty) const {
    return _fhe_ctx.Is_rns_poly_type(ty);
  }

private:
  LOWER_CTX& _fhe_ctx;
};

}  // namespace core
}  // namespace fhe

#endif  // FHE_CORE_LOWER_CTX_H
