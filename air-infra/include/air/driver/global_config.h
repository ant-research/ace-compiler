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
        _keep(false),
        _print_pass(false),
        _print_meta(false) {}

  bool        Help() const { return _help; }
  bool        Show() const { return _show; }
  bool        Trace() const { return _trace; }
  bool        Perf() const { return _perf; }
  bool        Keep() const { return _keep; }
  bool        Print_pass() const { return _print_pass; }
  bool        Print_meta() const { return _print_meta; }
  const char* Ofile() const { return _ofile.c_str(); }

  void Register_options(DRIVER_CTX* ctx);
  void Update_options(const char* ifile);
  void Print(std::ostream& os) const;

  bool        _help;        // -help
  bool        _show;        // -show
  bool        _trace;       // -trace   // .t
  bool        _perf;        // -perf    // .json
  bool        _keep;        // -keep
  bool        _print_pass;  // -print-pass
  bool        _print_meta;  // -print-meta
  std::string _ofile;       // -o <output c/c++ file>
};

}  // namespace driver

}  // namespace air

#endif  // AIR_DRIVER_GLOBAL_CONFIG_H
