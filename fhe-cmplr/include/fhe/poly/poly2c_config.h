//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_POLY_POLY2C_CONFIG_H
#define FHE_POLY_POLY2C_CONFIG_H

#include "air/driver/common_config.h"
#include "air/driver/driver_ctx.h"
#include "fhe/core/lib_provider.h"

namespace fhe {
namespace poly {

struct POLY2C_CONFIG : public air::util::COMMON_CONFIG {
public:
  POLY2C_CONFIG(void)
      : _prov_str("ant"),
        _ct_encode(false),
        _free_poly(false),
        _provider(fhe::core::PROVIDER::ANT),
        _ifile(nullptr) {}

  void Register_options(air::driver::DRIVER_CTX* ctx);
  void Update_options();
  void Set_ifile(const char* ifile) { _ifile = ifile; }

  void Print(std::ostream& os) const;

  const char*    Prov_str() const { return _prov_str.c_str(); }
  core::PROVIDER Provider() const { return _provider; }
  const char*    Data_file() const { return _data_file.c_str(); }
  const char*    Ifile() const { return _ifile; }
  bool           Emit_data_file() const { return !_data_file.empty(); }
  bool           Ct_encode() const { return _ct_encode; }
  bool           Free_poly() const { return _free_poly; }

  // leave this member public so that OPTION_DESC can access it
  std::string _prov_str;
  std::string _data_file;  // place data in a seperated file
  bool        _ct_encode;  // encode constant at compile-time
  bool        _free_poly;  // insert free_poly

  fhe::core::PROVIDER _provider;  // parsed from _prov_str
  const char*         _ifile;     // set ifile if data_file is set
};

//! @brief Macro to define API to access POLY2C config
#define DECLARE_POLY2C_CONFIG_ACCESS_API(cfg)                            \
  core::PROVIDER Provider() const { return cfg.Provider(); }             \
  const char*    Data_file() const { return cfg.Data_file(); }           \
  bool           Emit_data_file() const { return cfg.Emit_data_file(); } \
  bool           Ct_encode() const { return cfg.Ct_encode(); }           \
  bool           Free_poly() const { return cfg.Free_poly(); }           \
  DECLARE_COMMON_CONFIG_ACCESS_API(cfg)

}  // namespace poly
}  // namespace fhe

#endif  // FHE_POLY_POLY2C_CONFIG_H
