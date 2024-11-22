//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_DRIVER_PASS_MANAGER_H
#define AIR_DRIVER_PASS_MANAGER_H

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
    return Forward<0>([driver](auto&& pass) -> R_CODE {
      if (driver->Keep() || pass.Trace_ir_before()) {
        driver->Trace() << "#### IR trace before " << pass.Name() << std::endl;
        driver->Trace_ir();
      }
      if (pass.Trace_stat()) driver->Perf_start();
      R_CODE ret_code = pass.Run();
      if (driver->Keep() || pass.Trace_ir_after()) {
        driver->Trace() << "#### IR trace after " << pass.Name() << std::endl;
        driver->Trace_ir();
      }
      if (pass.Trace_stat()) {
        driver->Perf_taken(driver->Exe_name(), pass.Name(), "--");
      }

      // Write AIR to ELF after phase
      if (!pass.Write_ir().empty()) {
        driver->Write_ir(pass.Write_ir());
      }

      // Read AIR from ELF before phase
      if (!pass.Read_ir().empty()) {
        driver->Read_ir(pass.Read_ir());
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
};

}  // namespace driver

}  // namespace air

#endif  // AIR_DRIVER_PASS_MANAGER_H
