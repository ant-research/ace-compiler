//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_DRIVER_PASS_H
#define AIR_DRIVER_PASS_H

#include "air/driver/common_config.h"
#include "air/util/option.h"

namespace air {

namespace driver {

//! @brief An empty pass as common base for all passes
//! @tparam PASS_CONFIG Pass config. Must inherit from COMMON_CONFIG
template <typename PASS_CONFIG>
class PASS {
public:
  //! @brief Construct a new PASS object, do nothing
  PASS() {}

  //! @brief Initialize the pass with pointer to driver. do nothing. In a
  //! real pass, initialization work should be done. Pass context can be
  //! retrieved from driver.
  //!
  //! @tparam DRIVER Type of the driver where the pass is initialized
  //! @param driver Pointer to the driver where the pass is initialized
  //! @return true if no error occurs during the initialization
  //! @return false if error occurs during the initialization
  template <typename DRIVER>
  bool Init(DRIVER* driver) {
    return true;
  }

  //! @brief Pre-run the pass. Extra preparation work can be
  //! done in Pre-run phase.
  //!
  //! @return true if no error occurs during pre-run phase
  //! @return false if error occurs during pre-run phase
  bool Pre_run() { return true; }

  //! @brief Run the pass. Major work is done in this phase.
  //!
  //! @return true if no error occurs during run phase
  //! @return false if error occurs during run phase
  bool Run() { return true; }

  //! @brief Post-run the pass. Extra clean-up or summary work
  //! can be done in Post-run phase.
  //! Post run phase shouldn't trigger any error.
  void Post_run() {}

  //! @brief Finalize the pass. All clean-up work is done in
  //! finalization phase.
  //! Finalize phase shouldn't trigger any error.
  void Fini() {}

  //! @brief Get common config for the pass.
  const PASS_CONFIG& Config() const { return _config; }

  //! @brief Declare APIs to access config items
  DECLARE_COMMON_CONFIG_ACCESS_API(_config)

  //! @brief Enable/disable the pass
  void Set_enable(bool ena) { _config.Set_enable(ena); }

protected:
  // Interface for derived class to update config
  PASS_CONFIG& Config() { return _config; }

  // Pass config items
  PASS_CONFIG _config;

};  // class PASS

}  // namespace driver

}  // namespace air

#endif  // AIR_DRIVER_PASS_H
