//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef ACE_RUNTIME_CONFIG_MANAGER_H
#define ACE_RUNTIME_CONFIG_MANAGER_H

#include <vector>
#include <mutex>
#include <stdexcept>

#include <sstream>
#include <iostream>

#include "common/rtlib.h"

namespace ace {
namespace runtime {

/**
 * @brief Runtime configuration compiled from a model.
 *
 * This structure holds all configuration data needed for FHE kernel
 * execution, including input/output counts, CKKS parameters, and
 * encoding/decoding schemes.
 */
struct RUNTIME_CONFIG {
  int                       _input_cnt;     //!< Number of inputs
  int                       _output_cnt;    //!< Number of outputs
  uintptr_t                 _ctx_param;     //!< Parameters to create CKKS Context
  std::vector<DATA_SCHEME>  _encode_sch;    //!< Describe scheme for encode
  std::vector<DATA_SCHEME>  _decode_sch;    //!< Describe scheme for decode
  RT_DATA_INFO              _weight_info;   //!< Weight data information
};

/**
 * @brief Get runtime configuration for debugging purposes.
 * @return Pointer to the global runtime configuration
 */
const RUNTIME_CONFIG* Get_runtime_config();

/**
 * @brief Singleton manager for runtime configuration.
 *
 * This class provides thread-safe access to the global FHE runtime
 * configuration. Configuration must be set before kernel execution.
 */
class CONFIG_MANAGER {
  public:
      /**
       * @brief Get the singleton instance.
       * @return Reference to the singleton instance
       */
      static CONFIG_MANAGER& Instance() {
          static CONFIG_MANAGER inst;
          return inst;
      }

      /**
       * @brief Set the runtime configuration.
       * @param config Configuration to set
       */
      void Set_config(const RUNTIME_CONFIG& config) {
          std::lock_guard<std::mutex> lock(_mutex);
          _config = config;
          _is_configured = true;
      }

      /**
       * @brief Get the current runtime configuration.
       * @return Const reference to the configuration
       * @throws std::runtime_error if configuration not set
       */
      const RUNTIME_CONFIG& Get_config() const {
          std::lock_guard<std::mutex> lock(_mutex);
          if (!_is_configured) {
              throw std::runtime_error("Configuration not set! Call set_fhe_config() first.");
          }
          return _config;
      }

      /**
       * @brief Check if configuration has been set.
       * @return true if configured, false otherwise
       */
      bool Is_configured() const {
          std::lock_guard<std::mutex> lock(_mutex);
          return _is_configured;
      }

  private:
      CONFIG_MANAGER() = default;
      mutable std::mutex _mutex;          //!< Mutex for thread safety
      RUNTIME_CONFIG _config;             //!< Stored configuration
      bool _is_configured = false;        //!< Configuration status flag
};

/**
 * @brief Validator for runtime configuration.
 *
 * Provides static methods to validate configuration integrity,
 * including input/output counts, CKKS parameters, and encoding schemes.
 */
class CONFIG_VALIDATOR {
public:
    /**
     * @brief Validate configuration and return error messages.
     * @param config Configuration to validate
     * @return String containing error messages (empty if valid)
     */
    static std::string Validate(const RUNTIME_CONFIG& config);

    /**
     * @brief Check if configuration is valid.
     * @param config Configuration to validate
     * @return true if valid, false otherwise
     */
    static bool Is_valid(const RUNTIME_CONFIG& config) {
        return Validate(config).empty();
    }
};

/**
 * @brief Printer for runtime configuration.
 *
 * Provides static methods to print configuration details for
 * debugging and logging purposes.
 */
class CONFIG_PRINTER {
public:
    /**
     * @brief Print configuration to output stream.
     * @param config Configuration to print
     * @param os Output stream (default: std::cout)
     */
    static void Print(const RUNTIME_CONFIG& config, std::ostream& os);

    /**
     * @brief Convert configuration to string representation.
     * @param config Configuration to convert
     * @return String representation of the configuration
     */
    static std::string To_string(const RUNTIME_CONFIG& config);
};

} // namespace runtime
} // namespace ace

#endif // ACE_RUNTIME_CONFIG_MANAGER_H