//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @file util/logging.h
//! Lightweight logging facade over spdlog for the FHE runtime library.
//!
//! Provides a pure-C++ logging infrastructure (no Python dependency).
//! The default logger writes to stderr with colors.  When used from
//! pybind11 extension modules, `ace::common::Init_logging()` in
//! csrc/common/ replaces the default sink with a PythonSink so that
//! C++ log messages flow into Python's `logging` module.
//!
//! Usage from fhe_lib C++ code:
//!   #include "util/logging.h"
//!   spdlog::info("[io] File opened: {}", path);
//!   spdlog::debug("[gds] read_gpu: {:.3} ms", elapsed_ms);
//!   spdlog::error("[kms] Key index parse failed: {}", reason);
//!
//! Tag convention: bracket the subsystem name at the start of the
//! message to make grepping easy:
//!   [io]   — IO transport, bulk reader
//!   [gds]  — GDS/cuFile transport
//!   [kms]  — Key management, serialization
//!   [data] — Plaintext data management
//!   [perf] — Performance statistics

#ifndef FHERT_UTIL_LOGGING_H
#define FHERT_UTIL_LOGGING_H

#include <spdlog/spdlog.h>
#include <string>

namespace fhert
{
namespace util
{
namespace logging
{

/// Initialize the default spdlog logger with a stderr_color sink.
/// Called automatically on first log output, but can be called
/// explicitly to set a custom logger name.
/// @param logger_name  Name for the spdlog logger (default: "ace")
void init(const std::string& logger_name = "ace");

/// Set the global log level.
/// @param level  One of: TRACE, DEBUG, INFO, WARN, ERROR, CRITICAL, OFF
/// @throws std::invalid_argument if level string is invalid
void set_level(const std::string& level);

/// Set the global log level using spdlog's native enum.
void set_level(spdlog::level::level_enum level);

}  // namespace logging
}  // namespace util
}  // namespace fhert

#endif  // FHERT_UTIL_LOGGING_H
