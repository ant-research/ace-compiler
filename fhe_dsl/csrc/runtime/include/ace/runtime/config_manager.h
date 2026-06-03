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
  int                       _input_cnt = 0;     //!< Number of inputs
  int                       _output_cnt = 0;    //!< Number of outputs
  uintptr_t                 _ctx_param = 0;     //!< Parameters to create CKKS Context
  std::vector<DATA_SCHEME>  _encode_sch;        //!< Describe scheme for encode
  std::vector<DATA_SCHEME>  _decode_sch;        //!< Describe scheme for decode
  RT_DATA_INFO              _weight_info = {};  //!< Weight data information
  std::string               _weight_file_name;  //!< Owns storage for _weight_info._file_name
  std::string               _weight_file_uuid;  //!< Owns storage for _weight_info._file_uuid

  std::string To_string() const;
  std::string Validate() const;
  bool Is_valid() const { return Validate().empty(); }

  //! Free heap-allocated memory in DATA_SCHEME entries and CKKS_PARAMS
  void Cleanup();
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
       *
       * Can only be called before Freeze_config(). Once frozen,
       * further calls will throw std::runtime_error.
       *
       * @param config Configuration to set
       * @throws std::runtime_error if configuration is frozen
       */
      void Set_config(const RUNTIME_CONFIG& config) {
          std::lock_guard<std::mutex> lock(_mutex);
          if (_frozen) {
              throw std::runtime_error("Configuration is frozen and cannot be modified");
          }
          _config = config;
          // Fix self-referential pointers: after copy, _weight_info pointers
          // still point to the source's strings. Re-point to our own copies.
          if (!_config._weight_file_name.empty()) {
              _config._weight_info._file_name = _config._weight_file_name.c_str();
          }
          if (!_config._weight_file_uuid.empty()) {
              _config._weight_info._file_uuid = _config._weight_file_uuid.c_str();
          }
          _is_configured = true;
      }

      /**
       * @brief Freeze the configuration, preventing further modifications.
       *
       * Should be called after initial setup is complete (e.g., after
       * Parse_and_set_config). This formalizes the contract that config
       * is immutable during inference, making Get_config_ref() safe.
       */
      void Freeze_config() {
          std::lock_guard<std::mutex> lock(_mutex);
          _frozen = true;
      }

      /**
       * @brief Unfreeze the configuration, allowing reconfiguration.
       *
       * Should only be called when no inference is running, as unfreezing
       * invalidates pointers returned by Get_config_ref(). Used when
       * creating a new FHERuntime with different configuration.
       */
      void Unfreeze_config() {
          std::lock_guard<std::mutex> lock(_mutex);
          _frozen = false;
      }

      /**
       * @brief Reset configuration: clean up heap memory, unfreeze, mark unconfigured.
       *
       * Safe way to prepare for reconfiguration. Cleans up the stored config
       * directly (not a copy), avoiding dangling pointers.
       */
      void Reset_config() {
          std::lock_guard<std::mutex> lock(_mutex);
          _config.Cleanup();
          _frozen = false;
          _is_configured = false;
      }

      /**
       * @brief Check if the configuration is frozen.
       * @return true if frozen, false otherwise
       */
      bool Is_frozen() const {
          std::lock_guard<std::mutex> lock(_mutex);
          return _frozen;
      }

      /**
       * @brief Get a copy of the current runtime configuration.
       * @return Copy of the configuration (thread-safe)
       * @throws std::runtime_error if configuration not set
       */
      RUNTIME_CONFIG Get_config() const {
          std::lock_guard<std::mutex> lock(_mutex);
          if (!_is_configured) {
              throw std::runtime_error("Configuration not set! Call set_fhe_config() first.");
          }
          return _config;
      }

      /**
       * @brief Get a const reference to the current runtime configuration.
       *
       * Intended for C callback functions that need stable pointers into
       * config fields (e.g., DATA_SCHEME pointers into _encode_sch vector).
       * Safe only when config is not modified concurrently, which is the
       * case after initial setup during inference.
       *
       * @return Const reference to the configuration
       * @throws std::runtime_error if configuration not set
       */
      const RUNTIME_CONFIG& Get_config_ref() const {
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
      bool _frozen = false;              //!< Whether config is frozen (immutable)
};

} // namespace runtime
} // namespace ace

#endif // ACE_RUNTIME_CONFIG_MANAGER_H