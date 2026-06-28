//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_DRIVER_PASS_MANAGER_H
#define AIR_DRIVER_PASS_MANAGER_H

#include <cstring>
#include <string>

#include "air/util/binary/elf_info.h"
#include "air/util/option.h"

namespace air {
namespace driver {

/**
 * @brief A pass manager contains the pipeline or data objects shared between
 * passes in the pipeline.
 *
 * @tparam PASSES All passes in the pipeline managed by the manager
 */
template <typename... PASSES>
class PASS_MANAGER {
public:
  /**
   * @brief Construct a new pass manager object. do nothing
   *
   */
  PASS_MANAGER() {}

  /**
   * @brief Construct a new pass manager object with external pass objects
   *
   * @param passes External pass objects
   */
  PASS_MANAGER(PASSES&&... passes) : _passes(passes...) {}

  /**
   * @brief Construct a new pass manager object with external pass tuple
   *
   * @param passes External pass tuple
   */
  PASS_MANAGER(const std::tuple<PASSES...>&& passes) : _passes(passes) {}

  /**
   * @brief Initialize the pipeline with pointer to driver
   *
   * @tparam DRIVER Type of the driver where the pass manager is initialized
   * @param driver Pointer to the driver which runs the pass manager/pipeline
   * @return R_CODE returned by initialization
   */
  template <typename DRIVER>
  R_CODE Init(DRIVER* driver) {
    return Forward<0>(
        [](auto&& pass, auto&& arg) -> R_CODE { return pass.Init(arg); },
        driver);
  }

  /**
   * @brief Pre-run each pass in the pipeline. Extra preparation work can be
   * done in Pre-run phase.
   *
   * @return R_CODE returned by pre-run phase
   */
  template <typename DRIVER>
  R_CODE Pre_run(DRIVER* driver) {
    return Forward<0>([](auto&& pass) -> R_CODE { return pass.Pre_run(); });
  }

  /**
   * @brief Run each pass in the pipeline. Major work is done in this phase.
   *
   * @return R_CODE returned by run phase
   */
  template <typename DRIVER>
  R_CODE Run(DRIVER* driver) {
    // Get resume info from config (already analyzed in Parse_options)
    const char* resume_phase = driver->Resume_phase();
    bool        is_resume    = driver->Is_resume();
    std::string ifile_str;
    if (driver->Ifile() != nullptr) {
      ifile_str = std::string(driver->Ifile());
    }

    return Forward<0>([driver, resume_phase, is_resume,
                       &ifile_str](auto&& pass) -> R_CODE {
      // Handle IR resume: skip passes based on metadata phase
      if (is_resume && Should_skip_pass(pass.Name(), resume_phase)) {
        CMPLR_WARN_MSG(driver->Tfile(),
                       "%s PASS is skipped for IR resume (phase: %s).",
                       pass.Name(), resume_phase);
        return R_CODE::NORMAL;
      }

      // Load IR at the first pass after the resume point
      if (is_resume && Should_load_ir(pass.Name(), resume_phase)) {
        driver->Read_ir(ifile_str);
      }

      // Read AIR from ELF before phase runs (explicit -b2ir option)
      if (!pass.Read_ir().empty()) {
        driver->Read_ir(pass.Read_ir());
      }

      if (pass.Enable() == false) {
        CMPLR_WARN_MSG(driver->Tfile(), "%s PASS is disabled.", pass.Name());
        return R_CODE::NORMAL;
      }
      if (driver->Trace() || pass.Trace_st_before()) {
        driver->Tstream() << "#### SymTab trace before " << pass.Name()
                          << std::endl;
        driver->Trace_st();
      }
      if (driver->Trace() || pass.Trace_ir_before()) {
        driver->Tstream() << "#### IR trace before " << pass.Name()
                          << std::endl;
        driver->Trace_ir();
      }

      // Verify AIR before each phase
      if (driver->Verify() || pass.Verify_ir()) {
        driver->Verify_ir();
      }
      R_CODE ret_code = pass.Run();
      if (driver->Trace() || pass.Trace_st_after()) {
        driver->Tstream() << "#### SymTab trace after " << pass.Name()
                          << std::endl;
        driver->Trace_st();
      }
      if (driver->Trace() || pass.Trace_ir_after()) {
        driver->Tstream() << "#### IR trace after " << pass.Name() << std::endl;
        driver->Trace_ir();
      }
      if (driver->Trace_mp() || pass.Trace_mp()) {
        driver->Tstream() << "#### Mempool after " << pass.Name() << std::endl;
        driver->Trace_mp_info();
      }

      // Write AIR to ELF with Keep
      if (driver->Keep()) {
        std::string lower = pass.Name();
        std::transform(lower.begin(), lower.end(), lower.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        std::string kfile = driver->Ifile();
        kfile.append(".");
        kfile.append(lower);
        driver->Write_ir(kfile, pass.Name());
      }

      // Write AIR to ELF after phase
      if (!pass.Write_ir().empty()) {
        driver->Write_ir(pass.Write_ir(), pass.Name());
      }

      // Dump IR if --dump matches current phase
      // Generate dump file in current working directory (like GCC -save-temps)
      if (driver->Dump_enabled() &&
          Match_dump_phase(pass.Name(), driver->Dump())) {
        std::string dump_file = driver->Ifile_basename() + ".dump.";
        std::string lower     = pass.Name();
        std::transform(lower.begin(), lower.end(), lower.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        dump_file.append(lower);
        dump_file.append(BFILE_SUFFIX);
        driver->Write_ir(dump_file, pass.Name());
      }

      // Verify AIR after each phase
      if (driver->Verify() || pass.Verify_ir()) {
        driver->Verify_ir();
      }

      return ret_code;
    });
  }

  /**
   * @brief Post-run each pass in the pipeline. Extra clean-up or summary work
   * can be done in Post-run phase.
   *
   */
  template <typename DRIVER>
  void Post_run(DRIVER* driver) {
    return Backward<sizeof...(PASSES) - 1>(
        [](auto&& pass) { pass.Post_run(); });
  }

  /**
   * @brief Finalize each pass in the pipeline. All clean-up work is done in
   * finalization phase.
   *
   */
  template <typename DRIVER>
  void Fini(DRIVER* driver) {
    return Backward<sizeof...(PASSES) - 1>([](auto&& pass) { pass.Fini(); });
  }

  //! @brief Enable/disable given pass
  template <int PASS_ID>
  void Set_pass_enable(bool ena) {
    std::get<PASS_ID>(_passes).Set_enable(ena);
  }

  //! @brief Check if given pass is enabled
  template <int PASS_ID>
  bool Pass_enable() const {
    return std::get<PASS_ID>(_passes).Enable();
  }

  template <typename PASS, int PASS_ID>
  PASS& Get_pass() {
    return std::get<PASS_ID>(_passes);
  }

  template <typename PASS, int PASS_ID>
  const PASS& Get_pass() const {
    return std::get<PASS_ID>(_passes);
  }

private:
  // forward visit all passes in the pipeline:
  // pass 0 --> pass 1 --> ... --> pass n-1 --> pass n
  template <int I, typename Func, typename... Args>
  R_CODE Forward(Func f, Args&&... args) {
    R_CODE r_code = f(std::get<I>(_passes), args...);
    // when return code is abnormal, return immediately
    if (r_code != R_CODE::NORMAL) {
      return r_code;
    }

    if constexpr (I + 1 < sizeof...(PASSES)) {
      return Forward<I + 1, Func, Args...>(f, args...);
    }
    return R_CODE::NORMAL;
  }

  // backward visit all passes in the pipeline:
  // pass n --> pass n-1 --> ... --> pass 1 --> pass 0
  template <int I, typename Func, typename... Args>
  void Backward(Func f, Args&&... args) {
    f(std::get<I>(_passes), args...);
    if constexpr (I > 0) {
      Backward<I - 1, Func, Args...>(f, args...);
    }
  }

  // all passes managed by this pass manager
  std::tuple<PASSES...> _passes;

  //! @brief Check if a pass should be skipped based on IR resume phase
  //! @param pass_name Name of the current pass
  //! @param ir_phase Phase abbreviation stored in IR file (e.g., "O2A")
  //! @return true if pass should be skipped
  static bool Should_skip_pass(const char*        pass_name,
                               const std::string& ir_phase) {
    int ir_phase_idx = util::Get_phase_index(ir_phase.c_str());
    int pass_idx     = util::Get_phase_index(pass_name);

    // Skip if pass is before or at the IR phase
    return (ir_phase_idx >= 0 && pass_idx >= 0 && pass_idx <= ir_phase_idx);
  }

  //! @brief Check if IR should be loaded before this pass
  //! @param pass_name Name of the current pass
  //! @param ir_phase Phase abbreviation stored in IR file
  //! @return true if IR should be loaded before this pass
  static bool Should_load_ir(const char*        pass_name,
                             const std::string& ir_phase) {
    int ir_phase_idx = util::Get_phase_index(ir_phase.c_str());
    int pass_idx     = util::Get_phase_index(pass_name);

    // Load IR at the pass immediately after the IR phase
    return (ir_phase_idx >= 0 && pass_idx == ir_phase_idx + 1);
  }

  //! @brief Check if current pass matches the dump phase
  //! @param pass_name Name of the current pass (e.g., "ONNX2AIR")
  //! @param dump_phase User-specified dump phase (pass name)
  //! @return true if pass matches dump phase
  static bool Match_dump_phase(const char* pass_name, const char* dump_phase) {
    return strcmp(pass_name, dump_phase) == 0;
  }
};

}  // namespace driver

}  // namespace air

#endif  // AIR_DRIVER_PASS_MANAGER_H
