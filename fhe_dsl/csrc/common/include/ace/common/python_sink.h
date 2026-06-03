//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef ACE_COMMON_PYTHON_SINK_H
#define ACE_COMMON_PYTHON_SINK_H

#include <spdlog/spdlog.h>
#include <spdlog/sinks/base_sink.h>
#include <mutex>
#include <string>

#include <pybind11/pybind11.h>

namespace ace {
namespace common {

/**
 * @brief spdlog sink that forwards log messages to Python's logging module.
 *
 * All C++ log messages are routed through Python's `logging.getLogger()`,
 * giving users full control over log level, format, and handlers from Python.
 *
 * Thread safety: inherits base_sink's mutex. GIL is acquired before calling Python.
 */
class PythonSink : public spdlog::sinks::base_sink<std::mutex> {
protected:
    void sink_it_(const spdlog::details::log_msg& msg) override {
        pybind11::gil_scoped_acquire gil;
        try {
            auto logging = pybind11::module_::import("logging");
            auto logger = logging.attr("getLogger")(logger_name_);
            std::string text(msg.payload.data(), msg.payload.size());
            switch (msg.level) {
                case spdlog::level::trace:
                case spdlog::level::debug:
                    logger.attr("debug")(text);
                    break;
                case spdlog::level::info:
                    logger.attr("info")(text);
                    break;
                case spdlog::level::warn:
                    logger.attr("warning")(text);
                    break;
                case spdlog::level::err:
                    logger.attr("error")(text);
                    break;
                case spdlog::level::critical:
                    logger.attr("critical")(text);
                    break;
                default:
                    logger.attr("info")(text);
                    break;
            }
        } catch (...) {
            // If Python logging fails (e.g., during shutdown), fall back to stderr
            fmt::print(stderr, "[{}] {}\n", logger_name_, msg.payload);
        }
    }

    void flush_() override {}

public:
    explicit PythonSink(const std::string& logger_name)
        : logger_name_(logger_name) {}

private:
    std::string logger_name_;
};

} // namespace common
} // namespace ace

#endif // ACE_COMMON_PYTHON_SINK_H