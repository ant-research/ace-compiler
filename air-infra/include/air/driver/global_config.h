//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_DRIVER_GLOBAL_CONFIG_H
#define AIR_DRIVER_GLOBAL_CONFIG_H

#include <string>

#include "air/util/debug.h"

namespace air {

namespace driver {

class DRIVER_CTX;

//! @brief global common config
class GLOBAL_CONFIG {
public:
  GLOBAL_CONFIG()
      : _help(false),
        _show(false),
        _trace(false),
        _trace_mp(false),
        _keep(false),
        _verify(AIR_DEBUG_ON),  // turn on in debug version
        _print_pass(false),
        _print_meta(false),
        _opt_level(0) {}

  bool        Help() const { return _help; }
  bool        Show() const { return _show; }
  bool        Trace() const { return _trace; }
  bool        Trace_mp() const { return _trace_mp; }
  bool        Perf() const { return _perf; }
  bool        Keep() const { return _keep; }
  bool        Verify() const { return _verify; }
  bool        Print_pass() const { return _print_pass; }
  bool        Print_meta() const { return _print_meta; }
  uint64_t    Opt_level() const { return _opt_level; }
  const char* Ofile() const { return _ofile.c_str(); }

  void Register_options(DRIVER_CTX* ctx);
  void Update_options(const char* ifile);
  void Print(std::ostream& os) const;

  bool        _help;        // -help
  bool        _show;        // -show
  bool        _trace;       // -trace   // .t
  bool        _trace_mp;    // -trace_mp // .t
  bool        _perf;        // -perf    // .json
  bool        _keep;        // -keep
  bool        _verify;      // -verify
  bool        _print_pass;  // -print-pass
  bool        _print_meta;  // -print-meta
  uint64_t    _opt_level;   // -O0, -O1, -O2, -O3
  std::string _ofile;       // -o <output c/c++ file>
};

//! @brief Macro to define API to access global config
#define DECLARE_GLOBAL_CONFIG_ACCESS_API(cfg)               \
  bool        Help() const { return cfg.Help(); }           \
  bool        Show() const { return cfg.Show(); }           \
  bool        Trace() const { return cfg.Trace(); }         \
  bool        Trace_mp() const { return cfg.Trace_mp(); }   \
  bool        Perf() const { return cfg.Perf(); }           \
  bool        Keep() const { return cfg.Keep(); }           \
  bool        Verify() const { return cfg.Verify(); }       \
  uint64_t    Opt_level() const { return cfg.Opt_level(); } \
  const char* Ofile() const { return cfg.Ofile(); }

}  // namespace driver

}  // namespace air

#endif  // AIR_DRIVER_GLOBAL_CONFIG_H
