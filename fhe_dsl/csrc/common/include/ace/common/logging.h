//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef ACE_COMMON_LOGGING_H
#define ACE_COMMON_LOGGING_H

#include <string>

namespace ace {
namespace common {

/**
 * @brief Initialize spdlog with a Python sink for the given module.
 *
 * Creates a spdlog logger that forwards all C++ log messages to
 * Python's `logging.getLogger(logger_name)`. Should be called once
 * per extension module in its PYBIND11_MODULE block.
 *
 * @param logger_name Python logger name (e.g., "ace.runtime", "ace.frontend")
 */
void Init_logging(const std::string& logger_name);

/**
 * @brief Set the global spdlog log level.
 *
 * @param level Level string: TRACE, DEBUG, INFO, WARN, ERROR, CRITICAL, OFF
 * @throws std::invalid_argument if level string is invalid
 */
void Set_log_level(const std::string& level);

} // namespace common
} // namespace ace

#endif // ACE_COMMON_LOGGING_H