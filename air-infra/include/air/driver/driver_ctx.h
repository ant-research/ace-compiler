//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_DRIVER_DRIVER_CTX_H
#define AIR_DRIVER_DRIVER_CTX_H

#include <filesystem>
#include <unordered_map>

#include "air/base/st.h"
#include "air/driver/global_config.h"
#include "air/util/error.h"
#include "air/util/messg.h"
#include "air/util/option.h"

namespace air {

namespace driver {

//! @brief ID for default trace file
static constexpr uint32_t DEFAULT_TFILE = 0;

//! @brief Driver context which can be shared among all drivers and passes
class DRIVER_CTX {
public:
  //! @brief Construct a new DRIVER_CTX
  DRIVER_CTX() {
    Register_tfile(DEFAULT_TFILE, nullptr);
    _glob = new air::base::GLOB_SCOPE(0, true);
  }

  ~DRIVER_CTX() {
    if (_glob != nullptr) {
      delete _glob;
    }
    Destroy_tfile();
  }

  //! @brief Parse command line options
  R_CODE Parse_options(int argc, char** argv) {
    _config.Register_options(this);
    R_CODE ret_code = _option_mgr.Parse_options(argc, argv);
    _config.Update_options(Ifile());

    // Analyze input file for IR resume detection (after Update_options)
    Analyze_input_file();

    // print option has higher priority than no input file
    Handle_global_options();
    if (!Ifile()) {
      // will abort the program
      CMPLR_USR_MSG(U_CODE::No_Input_File, Exe_name());
      return R_CODE::USER;
    }
    if (ret_code == R_CODE::NORMAL) {
      // redirect default trace to file named by input file
      Tfile(DEFAULT_TFILE).Open(_option_mgr.Tfile("").c_str());
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

  //! @brief Register trace file to emit tracing information
  //! @param id Id of the trace file
  //! @param suffix Suffix of the trace file name
  //! @return TFILE created for the id
  air::util::TFILE& Register_tfile(uint32_t id, const char* suffix);

  //! @brief Get input file name
  const char* Ifile() const { return _option_mgr.Ifile(); }

  //! @brief Get input file basename (filename without path)
  std::string Ifile_basename() const {
    return Ifile() == nullptr
               ? DFILE_PREFIX
               : std::filesystem::path(Ifile()).filename().string();
  }

  //! @brief Get extra output file name
  std::string Ext_ofile(const char* ext) const {
    return _option_mgr.Ofile(ext);
  }

  //! @brief Get default assembly file name
  std::string Def_sfile() const { return Ifile_basename() + SFILE_SUFFIX; }

  //! @brief Get default c file name
  std::string Def_cfile() const { return Ifile_basename() + CFILE_SUFFIX; }

  //! @brief Get configure file name
  std::string Def_cfg_file() const {
    return Ifile_basename() + CFG_FILE_SUFFIX;
  }

  //! @brief Get phase name from IR file (reads ELF header only)
  //! @param ifile IR file path
  //! @return Phase name string, empty string if not a valid IR file
  std::string Get_ir_phase(const std::string& ifile);

  //! @brief Access global config items
  DECLARE_GLOBAL_CONFIG_ACCESS_API(_config)

  //! @brief Get global scope
  air::base::GLOB_SCOPE* Glob_scope() const { return _glob; }

  //! @brief Update global scope
  void Update_glob_scope(air::base::GLOB_SCOPE* glob) { _glob = glob; }

  //! @brief Get trace file object
  air::util::TFILE& Tfile(uint32_t id = DEFAULT_TFILE) const {
    auto it = _tfiles.find(id);
    AIR_ASSERT_MSG(it != _tfiles.end(), "tfile not registered");
    return *(it->second);
  }

  //! @brief Get ofstream for trace
  std::ostream& Tstream(uint32_t id = 0) const { return Tfile(id).Tfile(); }

  //! @brief get executable program name
  const char* Exe_name() const { return _option_mgr.Exe_name(); }

  //! @brief Terminate compilation process early
  void Teardown(R_CODE rc);

private:
  // handle global options
  void Handle_global_options();

  // analyze input file for IR resume detection
  void Analyze_input_file();

  // destroy tfiles
  void Destroy_tfile();

  // Option manager
  air::util::OPTION_MGR _option_mgr;

  // Global common config
  air::driver::GLOBAL_CONFIG _config;

  // Trace files
  std::unordered_map<uint32_t, air::util::TFILE*> _tfiles;

  // Global scope
  air::base::GLOB_SCOPE* _glob;
};  // DRIVER_CTX

//! @brief Macro to define API for tracing
//!  There are three kinds of trace APIs:
//!   - Trace(flag, ...): Print ... into trace file if flag is on
//!   - Trace_cmd(flag, f, ...): Call f(os, ...) to write trace file
//!   - Trace_obj(flag, obj): Call obj->Print(os) to write trace file

#define DECLARE_TRACE_DETAIL_API(cfg, ctx_ptr)           \
  template <typename... Args>                            \
  void Trace(uint64_t flag, Args&&... args) {            \
    if (cfg.Is_trace(flag)) {                            \
      std::ostream& os = ctx_ptr->Tstream();             \
      (os << ... << args);                               \
    }                                                    \
  }                                                      \
  template <typename F, typename... Args>                \
  void Trace_cmd(uint64_t flag, F&& f, Args&&... args) { \
    if (cfg.Is_trace(flag)) {                            \
      std::ostream& os = ctx_ptr->Tstream();             \
      f(os, args...);                                    \
    }                                                    \
  }                                                      \
  template <typename OBJ>                                \
  void Trace_obj(uint64_t flag, const OBJ& obj) {        \
    if (cfg.Is_trace(flag)) {                            \
      std::ostream& os = ctx_ptr->Tstream();             \
      obj->Print(os);                                    \
    }                                                    \
  }

}  // namespace driver

}  // namespace air

#endif  // AIR_DRIVER_DRIVER_CTX_H
