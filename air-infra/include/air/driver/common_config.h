//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_COMMON_CONFIG_H
#define AIR_UTIL_COMMON_CONFIG_H

#include <ostream>

namespace air {

namespace util {

//! @brief Define common config items available for all passes. Include:
//!      <li> Enable or disable the pass </li>
//!      <li> Show or not show the progress </li>
//!      <li> Enable or disable trace for the pass </li>
//!      <li> Skip transformation on input units with given id </li>

struct COMMON_CONFIG {
public:
  //! @brief Construct a new common config object
  COMMON_CONFIG()
      : _help(false),
        _enable(true),
        _show(false),
        _rt_dump(false),
        _rt_timing(false),
        _rt_validate(false),
        _trace_stat(false),
        _trace_ir_before(false),
        _trace_ir_after(false),
        _trace_st_before(false),
        _trace_st_after(false),
        _trace_detail(0),
        _skip_before(-1),
        _skip_after(-1),
        _skip_equal(-1) {}

  //! @brief Check if the pass is enabled or disabled
  //! @return true, -help/-h option is provided from the command line(should
  //! call
  //!  OPTION_MGR's Print function to print option detail)
  //! @return false, -help/-h option is not provided
  bool Help() const { return _help; }

  //! @brief Check if the pass is enabled or disabled
  bool Enable() const { return _enable; }

  //! @brief Check if progress is shown or not
  bool Show() const { return _show; }

  //! @brief Check if runtime dump is enabled or not
  bool Rt_dump() const { return _rt_dump; }

  //! @brief Check if runtime timing is enabled or not
  bool Rt_timing() const { return _rt_timing; }

  //! @brief Check if runtime validate is enabled or not
  bool Rt_validate() const { return _rt_validate; }

  //! @brief Check if trace resource statistics for the pass is enabled or
  //   disabled
  bool Trace_stat() const { return _trace_stat; }

  //! @brief Check if trace IR before pass is enabled or disabled
  bool Trace_ir_before() const { return _trace_ir_before; }

  //! @brief Check if trace IR after pass is enabled or disabled
  bool Trace_ir_after() const { return _trace_ir_after; }

  //! @brief Check if trace Symtab before pass is enabled or disabled
  bool Trace_st_before() const { return _trace_st_before; }

  //! @brief Check if trace Symtab after pass is enabled or disabled
  bool Trace_st_after() const { return _trace_st_after; }

  //! @brief Check if trace on given flag is enabled or disabled
  bool Is_trace(int flag) const { return (_trace_detail & (1 << flag)) != 0; }

  //! @brief Check if units with given id is skipped or not
  bool Is_skipped(uint32_t id) const {
    return (_skip_before != -1 && id < _skip_before) ||
           (_skip_after != -1 && id > _skip_after) ||
           (_skip_equal != -1 && id == _skip_equal);
  }

  //! @brief Read ir from elf section
  std::string Read_ir() const { return _read_ir; }

  //! @brief Write ir to elf section
  std::string Write_ir() const { return _write_ir; }

  //! @brief Enable/disable the pass
  void Set_enable(bool ena) { _enable = ena; }

  //! @brief Print common config settings
  void Print(std::ostream& os) const;

  bool        _help;             //!< Show the help info
  bool        _enable;           //!< Enable or disable the pass
  bool        _show;             //!< Show or not show the progress
  bool        _rt_dump;          //!< Runtime dump
  bool        _rt_timing;        //!< Runtime timing
  bool        _rt_validate;      //!< Runtime validate
  bool        _trace_stat;       //!< Trace resource statistics
  bool        _trace_ir_before;  //!< Trace IR before pass
  bool        _trace_ir_after;   //!< Trace IR after pass
  bool        _trace_st_before;  //!< Trace Symtab before pass
  bool        _trace_st_after;   //!< Trace Symtab after pass
  uint64_t    _trace_detail;     //!< Trace pass execution details
  int64_t     _skip_before;  //!< Skip units with smaller id than _skip_before
  int64_t     _skip_after;   //!< Skip units with greater id than _skip_after
  int64_t     _skip_equal;   //!< Skip units with same id as _skip_equal
  std::string _read_ir;      //!< Read IR from binary file before pass
  std::string _write_ir;     //!< Write IR to binary file after pass
};                           // COMMON_CONFIG

//! @brief Macro to define API to access common config
#define DECLARE_COMMON_CONFIG_ACCESS_API(cfg)                              \
  bool        Enable() const { return cfg.Enable(); }                      \
  bool        Show() const { return cfg.Show(); }                          \
  bool        Rt_timing() const { return cfg.Rt_timing(); }                \
  bool        Rt_validate() const { return cfg.Rt_validate(); }            \
  bool        Trace_stat() const { return cfg.Trace_stat(); }              \
  bool        Trace_ir_before() const { return cfg.Trace_ir_before(); }    \
  bool        Trace_ir_after() const { return cfg.Trace_ir_after(); }      \
  bool        Trace_st_before() const { return cfg.Trace_st_before(); }    \
  bool        Trace_st_after() const { return cfg.Trace_st_after(); }      \
  bool        Is_trace(int flag) const { return cfg.Is_trace(flag); }      \
  bool        Is_skipped(uint32_t id) const { return cfg.Is_skipped(id); } \
  std::string Read_ir() const { return cfg.Read_ir(); }                    \
  std::string Write_ir() const { return cfg.Write_ir(); }

//! @brief Macro to append common config description so that
//!  this can be easily added to pass's whole option description.
#define DECLARE_COMMON_CONFIG(name, config)                            \
  {"enable",          "e", "Enable the pass " #name, &config._enable,  \
   air::util::K_NONE, 0,   air::util::V_NONE},                         \
      {"show",                                                         \
       "s",                                                            \
       "Show the progress of " #name,                                  \
       &config._show,                                                  \
       air::util::K_NONE,                                              \
       0,                                                              \
       air::util::V_NONE},                                             \
      {"help",                                                         \
       "h",                                                            \
       "Show the option detial of " #name,                             \
       &config._help,                                                  \
       air::util::K_NONE,                                              \
       0,                                                              \
       air::util::V_NONE},                                             \
      {"rt_dump",                                                      \
       "rtd",                                                          \
       "Enable runtime dump in " #name,                                \
       &config._rt_dump,                                               \
       air::util::K_NONE,                                              \
       0,                                                              \
       air::util::V_NONE},                                             \
      {"rt_timing",                                                    \
       "rtt",                                                          \
       "Enable runtime timing in " #name,                              \
       &config._rt_timing,                                             \
       air::util::K_NONE,                                              \
       0,                                                              \
       air::util::V_NONE},                                             \
      {"rt_validate",                                                  \
       "rtv",                                                          \
       "Enable runtime validation in " #name,                          \
       &config._rt_validate,                                           \
       air::util::K_NONE,                                              \
       0,                                                              \
       air::util::V_NONE},                                             \
      {"read_ir",                                                      \
       "b2ir",                                                         \
       "Enable read ir from elf file before " #name,                   \
       &config._read_ir,                                               \
       air::util::K_STR,                                               \
       0,                                                              \
       air::util::V_EQUAL},                                            \
      {"write_ir",                                                     \
       "ir2b",                                                         \
       "Enable Write ir to elf file after " #name,                     \
       &config._write_ir,                                              \
       air::util::K_STR,                                               \
       0,                                                              \
       air::util::V_EQUAL},                                            \
      {"trace_stat",                                                   \
       "ts",                                                           \
       "Enable trace resource statistics in " #name,                   \
       &config._trace_stat,                                            \
       air::util::K_NONE,                                              \
       0,                                                              \
       air::util::V_NONE},                                             \
      {"trace_ir_before",                                              \
       "tib",                                                          \
       "Enable trace IR before " #name,                                \
       &config._trace_ir_before,                                       \
       air::util::K_NONE,                                              \
       0,                                                              \
       air::util::V_NONE},                                             \
      {"trace_ir_after",                                               \
       "tia",                                                          \
       "Enable trace IR after " #name,                                 \
       &config._trace_ir_after,                                        \
       air::util::K_NONE,                                              \
       0,                                                              \
       air::util::V_NONE},                                             \
      {"trace_st_before",                                              \
       "tsb",                                                          \
       "Enable trace Symtab before " #name,                            \
       &config._trace_st_before,                                       \
       air::util::K_NONE,                                              \
       0,                                                              \
       air::util::V_NONE},                                             \
      {"trace_st_after",                                               \
       "tsa",                                                          \
       "Enable trace Symtab after " #name,                             \
       &config._trace_st_after,                                        \
       air::util::K_NONE,                                              \
       0,                                                              \
       air::util::V_NONE},                                             \
      {"trace_detail",                                                 \
       "td",                                                           \
       "Enable trace details in " #name,                               \
       &config._trace_detail,                                          \
       air::util::K_INT64,                                             \
       0,                                                              \
       air::util::V_EQUAL},                                            \
      {"skip_before",                                                  \
       "",                                                             \
       "Skip item before given id in " #name,                          \
       &config._skip_before,                                           \
       air::util::K_INT64,                                             \
       0,                                                              \
       air::util::V_EQUAL},                                            \
      {"skip_after",                                                   \
       "",                                                             \
       "Skip processing after given id in " #name,                     \
       &config._skip_after,                                            \
       air::util::K_INT64,                                             \
       0,                                                              \
       air::util::V_EQUAL},                                            \
  {                                                                    \
    "skip_equal", "", "Skip processing with id in " #name,             \
        &config._skip_equal, air::util::K_INT64, 0, air::util::V_EQUAL \
  }

}  // namespace util

}  // namespace air

#endif  // AIR_UTIL_COMMON_CONFIG_H
