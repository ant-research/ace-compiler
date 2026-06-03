//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "ace/common/logging.h"
#include "ace/common/python_sink.h"

#include <spdlog/spdlog.h>
#include <stdexcept>

namespace ace {
namespace common {

void Init_logging(const std::string& logger_name) {
    auto python_sink = std::make_shared<PythonSink>(logger_name);
    auto logger = std::make_shared<spdlog::logger>(logger_name, python_sink);
    logger->set_pattern("%v");  // Let Python logging handle formatting
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::info);
}

void Set_log_level(const std::string& level) {
    if (level == "TRACE" || level == "trace") {
        spdlog::set_level(spdlog::level::trace);
    } else if (level == "DEBUG" || level == "debug") {
        spdlog::set_level(spdlog::level::debug);
    } else if (level == "INFO" || level == "info") {
        spdlog::set_level(spdlog::level::info);
    } else if (level == "WARN" || level == "warn" || level == "WARNING" || level == "warning") {
        spdlog::set_level(spdlog::level::warn);
    } else if (level == "ERROR" || level == "error") {
        spdlog::set_level(spdlog::level::err);
    } else if (level == "CRITICAL" || level == "critical") {
        spdlog::set_level(spdlog::level::critical);
    } else if (level == "OFF" || level == "off") {
        spdlog::set_level(spdlog::level::off);
    } else {
        throw std::invalid_argument("Invalid log level: " + level);
    }
}

} // namespace common
} // namespace ace