//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "log.h"

namespace ace {
namespace runtime {

LOG_LEVEL LOGGER::Parse_level(const std::string& level) {
    if (level == "DEBUG") return LOG_LEVEL::DEBUG;
    if (level == "INFO") return LOG_LEVEL::INFO;
    if (level == "WARNING") return LOG_LEVEL::WARN;
    if (level == "ERROR") return LOG_LEVEL::ERROR;
    if (level == "CRITICAL") return LOG_LEVEL::CRITICAL;
    throw std::invalid_argument("Invalid log level");
}

std::string LOGGER::Level_to_str(LOG_LEVEL level) const {
    switch (level) {
        case LOG_LEVEL::ERROR: return "ERROR";
        case LOG_LEVEL::WARN: return "WARN";
        case LOG_LEVEL::INFO: return "INFO";
        case LOG_LEVEL::DEBUG: return "DEBUG";
        case LOG_LEVEL::CRITICAL: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

std::string LOGGER::Get_cur_time() const {
    std::time_t now = std::time(nullptr);
    char buf[20];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return buf;
}

} // namespace runtime
} // namespace ace