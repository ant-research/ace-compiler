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
#include "fhe/core/scheme_info.h"

namespace fhe {
namespace core {
using namespace air::base;

enum FHE_TYPE_KIND : uint8_t {
  INVALID = 0,
  POLY    = 1,
  CIPHER  = 2,
  CIPHER3 = 3,  // ciphertext contains 3 polynomials
  PLAIN   = 4,
  END     = 5,
};

enum FHE_ATTR_KIND : uint32_t {
  SCALE     = 0,
  LEVEL     = 1,
  MUL_DEPTH = 2,
  LAST      = 3,
};

enum FHE_FUNC : uint32_t {
  APPROX_RELU  = 0,
  FHE_FUNC_END = 1,
};

#define INVALID_MUL_DEPTH -1

//! info of function gen in FHE phase
class FHE_FUNC_INFO {
public:
  FHE_FUNC_INFO(void) : _func_id(FUNC_ID()), _mul_depth(INVALID_MUL_DEPTH) {}
  FHE_FUNC_INFO(FUNC_ID func_id, uint32_t mul_depth)
      : _func_id(func_id), _mul_depth(mul_depth) {}
  ~FHE_FUNC_INFO() {}

  void    Set_func_id(FUNC_ID func_id) { _func_id = func_id; }
  FUNC_ID Get_func_id() const { return _func_id; }
  void    Set_mul_depth(uint32_t mul_depth) {
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
  FUNC_SCOPE* Get_func_scope(GLOB_SCOPE* glob_scope) {
    if (_func_id == FUNC_ID()) {
      return nullptr;
    }
    return &glob_scope->Open_func_scope(_func_id);
  }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  FHE_FUNC_INFO(const FHE_FUNC_INFO&);
  FHE_FUNC_INFO& operator=(const FHE_FUNC_INFO&);

  FUNC_ID _func_id   = FUNC_ID();
  int32_t _mul_depth = INVALID_MUL_DEPTH;
};

class LOWER_CTX {
public:
  LOWER_CTX() = default;
  ~LOWER_CTX() {}

  const CTX_PARAM& Get_ctx_param() const { return _ctx_param; }

  CTX_PARAM& Get_ctx_param() { return _ctx_param; }

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

  void Set_poly_type_id(air::base::TYPE_ID ploy_type_id) {
    _fhe_type[FHE_TYPE_KIND::POLY] = ploy_type_id;
  }

  air::base::TYPE_ID Get_poly_type_id() const {
    air::base::TYPE_ID poly_type_id = _fhe_type[FHE_TYPE_KIND::POLY];
    CMPLR_ASSERT(poly_type_id != air::base::TYPE_ID(),
                 "use poly type before create it");
    return poly_type_id;
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

  FHE_FUNC_INFO& Get_approx_relu_func_info() {
    return _fhe_func_info[static_cast<uint32_t>(FHE_FUNC::APPROX_RELU)];
  }

  const FHE_FUNC_INFO& Get_approx_relu_func_info() const {
    return _fhe_func_info[static_cast<uint32_t>(FHE_FUNC::APPROX_RELU)];
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

  bool Is_poly_type(air::base::TYPE_ID ty_id) const {
    return ty_id == Get_poly_type_id();
  }

  const char* Attr_name(FHE_ATTR_KIND attr) const;

private:
  // REQUIRED UNDEFINED UNWANTED methods
  LOWER_CTX(const LOWER_CTX&);
  LOWER_CTX& operator=(const LOWER_CTX&);

  CTX_PARAM _ctx_param;                     // cipher crypto context paramters
  TYPE_ID   _fhe_type[FHE_TYPE_KIND::END];  // type id gen in fhe
  FHE_FUNC_INFO
  _fhe_func_info[FHE_FUNC::FHE_FUNC_END];  // info of func gen in fhe
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
  bool Is_poly_type(air::base::TYPE_ID ty) const {
    return _fhe_ctx.Is_poly_type(ty);
  }

private:
  LOWER_CTX& _fhe_ctx;
};

}  // namespace core
}  // namespace fhe

#endif  // FHE_CORE_LOWER_CTX_H
