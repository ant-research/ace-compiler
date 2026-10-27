//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/util/logging.h"

#include <spdlog/sinks/stdout_sinks.h>
#include <stdexcept>

namespace fhert
{
namespace util
{
namespace logging
{

void init(const std::string& logger_name)
{
    // Avoid re-creating if a logger with this name already exists
    if (spdlog::get(logger_name)) {
        return;
    }

    auto sink = std::make_shared<spdlog::sinks::stderr_sink_mt>();
    auto logger = std::make_shared<spdlog::logger>(logger_name, sink);

    // Pattern: [time] [logger] [level] message
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] %v");

    // Set as the default logger so all spdlog::info/warn/... calls use it
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::info);
}

void set_level(const std::string& level)
{
    if (level == "TRACE" || level == "trace") {
        spdlog::set_level(spdlog::level::trace);
    }
    else if (level == "DEBUG" || level == "debug") {
        spdlog::set_level(spdlog::level::debug);
    }
    else if (level == "INFO" || level == "info") {
        spdlog::set_level(spdlog::level::info);
    }
    else if (level == "WARN" || level == "warn" || level == "WARNING" || level == "warning") {
        spdlog::set_level(spdlog::level::warn);
    }
    else if (level == "ERROR" || level == "error") {
        spdlog::set_level(spdlog::level::err);
    }
    else if (level == "CRITICAL" || level == "critical") {
        spdlog::set_level(spdlog::level::critical);
    }
    else if (level == "OFF" || level == "off") {
        spdlog::set_level(spdlog::level::off);
    }
    else {
        throw std::invalid_argument("Invalid log level: " + level);
    }
}

void set_level(spdlog::level::level_enum level)
{
    spdlog::set_level(level);
}

}  // namespace logging
}  // namespace util
}  // namespace fhert
