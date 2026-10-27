//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_POLY_POLY2C_CONFIG_H
#define FHE_POLY_POLY2C_CONFIG_H

#include <string>

#include "air/driver/common_config.h"
#include "air/driver/driver_ctx.h"
#include "air/util/debug.h"
#include "air/util/option.h"
#include "fhe/core/lib_provider.h"

namespace fhe {
namespace poly {

//! @brief Galois-key free strategy emitted by Handle_rotate.
//!   - OFF:        never emit Free_galois_key (slot resident for whole session;
//!                 release_session reclaims at session boundary).
//!   - USE_AND_FREE: legacy F1 — emit Free_galois_key(step) right after EACH
//!                 rotate that uses a non-constant step. Safe but causes per-frame
//!                 reload when the same step recurs (multi-frame / multi-user).
//!   - LAST_USE:   program-level liveness via RNUM — Load each distinct step once
//!                 before its first use, Free once after its last use; all
//!                 intermediate rotates hit the resident early-return. Default.
enum class Gk_free_mode { OFF, USE_AND_FREE, LAST_USE };

struct POLY2C_CONFIG : public air::util::COMMON_CONFIG {
public:
  POLY2C_CONFIG(void)
      : _prov_str("ant"),
        _ct_encode(false),
        _free_poly(false),
        _free_galois_key(false),
        _gk_free_mode(Gk_free_mode::OFF),
        _bts_explicit_slot(false),
        _provider(fhe::core::PROVIDER::ANT),
        _ifile(nullptr) {}

  void Register_options(air::driver::DRIVER_CTX* ctx);
  void Update_options();
  void Set_ifile(const char* ifile) { _ifile = ifile; }
  void Set_config(const char* config) { _config = config; }

  void Print(std::ostream& os) const;

  const char*    Prov_str() const { return _prov_str.c_str(); }
  core::PROVIDER Provider() const { return _provider; }
  const char*    Data_file() const { return _data_file.c_str(); }
  const char*    Ifile() const { return _ifile; }

  //! @brief Get config file name
  //! @return Config file name. Priority:
  //!         1. If _config is set, returns _config
  //!         2. If _ifile is valid, derives "<basename>.conf" from _ifile
  //!         3. Otherwise returns "default.conf"
  const char* Config() const {
    if (!_config.empty()) {
      return _config.c_str();
    }
    if (_ifile != nullptr && _ifile[0] != '\0') {
      _derived_cfg = Derive_cfg_name(_ifile);
      return _derived_cfg.c_str();
    }
    return "default" CFG_FILE_SUFFIX;
  }

  bool Emit_data_file() const { return !_data_file.empty(); }
  bool Ct_encode() const { return _ct_encode; }
  bool Free_poly() const { return _free_poly; }
  bool Free_galois_key() const { return _free_galois_key; }
  bool Bts_explicit_slot() const { return _bts_explicit_slot; }
  //! @brief Galois-key free strategy. -P2C:fgk true/false maps to LAST_USE/OFF
  //!         for backward compat; -P2C:gkfm off|use_and_free|last_use sets it
  //!         explicitly. Resolve_gk_free_mode() fuses the two in Update_options.
  Gk_free_mode Get_gk_free_mode() const { return _gk_free_mode; }

  // leave this member public so that OPTION_DESC can access it
  std::string _prov_str;
  std::string _data_file;   // place data in a separated file
  bool        _ct_encode;   // encode constants to plaintext at compile time
  bool        _free_poly;   // insert free_poly
  bool        _free_galois_key;  // legacy bool; mapped into _gk_free_mode
  Gk_free_mode _gk_free_mode;    // authoritative galois-key free strategy
  std::string _gk_fm_str;        // -P2C:gkfm = off|use_and_free|last_use
  bool _bts_explicit_slot;  // emit Eval_bootstrap_ciph with explicit slot

  fhe::core::PROVIDER _provider;  // parsed from _prov_str
  const char*         _ifile;     // set ifile if data_file is set
  std::string         _config;    // output config file name

private:
  //! @brief Derive config file name from input file path
  //! @param ifile Input file path
  //! @return Derived config file name in format "<basename>.conf"
  static std::string Derive_cfg_name(const char* ifile) {
    std::string result(ifile);
    // Extract basename (remove directory path)
    size_t pos = result.find_last_of("/\\");
    if (pos != std::string::npos) {
      result = result.substr(pos + 1);
    }
    // Remove extension
    pos = result.rfind('.');
    if (pos != std::string::npos) {
      result = result.substr(0, pos);
    }
    result += CFG_FILE_SUFFIX;
    return result;
  }

  // Mutable cache for derived config name (thread-safe in C++11+)
  mutable std::string _derived_cfg;
};

//! @brief Macro to define API to access POLY2C config
#define DECLARE_POLY2C_CONFIG_ACCESS_API(cfg)                                  \
  core::PROVIDER Provider() const { return cfg.Provider(); }                   \
  const char*    Data_file() const { return cfg.Data_file(); }                 \
  const char*    Config() const { return cfg.Config(); }                       \
  bool           Emit_data_file() const { return cfg.Emit_data_file(); }       \
  bool           Ct_encode() const { return cfg.Ct_encode(); }                 \
  bool           Free_poly() const { return cfg.Free_poly(); }                 \
  bool           Free_galois_key() const { return cfg.Free_galois_key(); }     \
  fhe::poly::Gk_free_mode Gk_free_mode() const { return cfg.Get_gk_free_mode(); }  \
  bool           Bts_explicit_slot() const { return cfg.Bts_explicit_slot(); } \
  DECLARE_COMMON_CONFIG_ACCESS_API(cfg)

}  // namespace poly
}  // namespace fhe

#endif  // FHE_POLY_POLY2C_CONFIG_H
