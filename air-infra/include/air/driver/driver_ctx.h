//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_DRIVER_DRIVER_CTX_H
#define AIR_DRIVER_DRIVER_CTX_H

#include "air/base/st.h"
#include "air/driver/global_config.h"
#include "air/util/error.h"
#include "air/util/messg.h"
#include "air/util/option.h"
#include "air/util/perf.h"

namespace air {

namespace driver {

//! @brief Driver context which can be shared among all drivers and passes
class DRIVER_CTX {
public:
  //! @brief Construct a new DRIVER_CTX
  DRIVER_CTX()
      : _glob(new air::base::GLOB_SCOPE(0, true)),
        _tfile(nullptr),
        _pfile(nullptr) {
    _perf = new air::util::PERF(_tfile, _pfile);
  }

  ~DRIVER_CTX() {
    AIR_ASSERT(_perf != nullptr);
    delete _perf;
  }

  //! @brief Parse command line options
  R_CODE Parse_options(int argc, char** argv) {
    _config.Register_options(this);
    R_CODE ret_code = _option_mgr.Parse_options(argc, argv);
    _config.Update_options(Ifile());
    // print option has higher priority than no input file
    Handle_global_options();
    if (!Ifile()) {
      CMPLR_USR_MSG(U_CODE::No_Input_File,
                    Exe_name());  // will abort the program
      return R_CODE::USER;
    }
    if (ret_code == R_CODE::NORMAL) {
      _tfile.Open(_option_mgr.Tfile());
      _pfile.Open(_option_mgr.Pfile());
    }
    return ret_code;
  }

  //! @brief Register top level options which doesn't belong any group
  //! @param desc_handle Description for top level options
  void Register_top_level_option(air::util::OPTION_DESC_HANDLE* desc_handle) {
    _option_mgr.Register_top_level_option(desc_handle);
  }

  //! @brief Register options belong to a group
  //! @param grp Description for the group options
  void Register_option_group(air::util::OPTION_GRP* grp) {
    _option_mgr.Register_option_group(grp);
  }

  //! @brief Get input file name
  const char* Ifile() { return _option_mgr.Ifile(); }

  //! @brief Get output file name
  const char* Ofile() { return _config.Ofile(); }

  //! @brief Get glob keep status
  bool Keep() { return _config.Keep(); }

  //! @brief Get global scope
  air::base::GLOB_SCOPE* Glob_scope() { return _glob; }

  //! @brief Update global scope
  void Update_glob_scope(air::base::GLOB_SCOPE* glob) {
    if (_glob != nullptr) {
      delete _glob;
    }
    _glob = glob;
  }

  //! @brief Get trace file object
  air::util::TFILE& Tfile() const { return _tfile; }

  //! @brief Get ofstream for trace
  std::ostream& Trace() const { return _tfile.Tfile(); }

  //! @brief Get perf file object
  air::util::TFILE& Pfile() const { return _pfile; }

  //! @brief Get ofstream for perf
  std::ostream& Perf() const { return _pfile.Tfile(); }

  //! @brief get executable program name
  const char* Exe_name() const { return _option_mgr.Exe_name(); }

  //! @brief Reset measure position
  void Perf_start() {
    AIR_ASSERT(_perf != nullptr);
    _perf->Start();
  }

  //! @brief Call measure position
  void Perf_taken(std::string driver, std::string phase, std::string pass) {
    AIR_ASSERT(_perf != nullptr);
    _perf->Taken(driver, phase, pass);
  }

  //! @brief Terminate compilation process early
  void Teardown(R_CODE rc);

private:
  // handle global options
  void Handle_global_options();

  // Option manager
  air::util::OPTION_MGR _option_mgr;

  // Global common config
  air::driver::GLOBAL_CONFIG _config;

  // Trace file
  mutable air::util::TFILE _tfile;

  // Perf file
  mutable air::util::TFILE _pfile;

  // Global scope
  air::base::GLOB_SCOPE* _glob;

  // pointer to unique perf context
  air::util::PERF* _perf;
};  // DRIVER_CTX

//! @brief Macro to define API for tracing
//!  There are three kinds of trace APIs:
//!   - Trace(flag, ...): Print ... into trace file if flag is on
//!   - Trace_cmd(flag, f, ...): Call f(os, ...) to write trace file
//!   - Trace_obj(flag, obj): Call obj->Print(os) to write trace file

#define DECLARE_TRACE_DETAIL_API(cfg, ctx_ptr)          \
  template <typename... Args>                           \
  void Trace(int32_t flag, Args&&... args) {            \
    if (cfg.Is_trace(flag)) {                           \
      std::ostream& os = ctx_ptr->Trace();              \
      (os << ... << args);                              \
    }                                                   \
  }                                                     \
  template <typename F, typename... Args>               \
  void Trace_cmd(int32_t flag, F&& f, Args&&... args) { \
    if (cfg.Is_trace(flag)) {                           \
      std::ostream& os = ctx_ptr->Trace();              \
      f(os, args...);                                   \
    }                                                   \
  }                                                     \
  template <typename OBJ>                               \
  void Trace_obj(int32_t flag, const OBJ& obj) {        \
    if (cfg.Is_trace(flag)) {                           \
      std::ostream& os = ctx_ptr->Trace();              \
      obj->Print(os);                                   \
    }                                                   \
  }

}  // namespace driver

}  // namespace air

#endif  // AIR_DRIVER_DRIVER_CTX_H
