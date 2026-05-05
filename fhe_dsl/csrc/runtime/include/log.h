//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef ACE_RUNTIME_LOG_H
#define ACE_RUNTIME_LOG_H

#include <iostream>
#include <string>
#include <sstream>
#include <ctime>
#include <mutex>

namespace ace {
namespace runtime {

/**
 * @brief Log level enumeration.
 */
enum class LOG_LEVEL {
    CRITICAL,   //!< Critical errors that cause immediate termination
    ERROR,      //!< Errors that may be recoverable
    WARN,       //!< Warning messages for potential issues
    INFO,       //!< Informational messages
    DEBUG       //!< Debug messages for development
};

/**
 * @brief Thread-safe logger singleton.
 *
 * Provides formatted logging with configurable log levels and
 * timestamp prefixes. Supports variadic template arguments for
 * flexible message formatting.
 */
class LOGGER {
public:
    /**
     * @brief Get the singleton logger instance.
     * @param os Output stream (default: std::cout)
     * @param level Minimum log level to output (default: "INFO")
     * @return Reference to the singleton instance
     */
    static LOGGER& Instance(std::ostream& os = std::cout, const std::string& level = "INFO") {
        static LOGGER log(os, level);
        return log;
    }

    LOGGER(const LOGGER&) = delete;
    LOGGER& operator=(const LOGGER&) = delete;

    /**
     * @brief Set the minimum log level.
     * @param level Log level string ("DEBUG", "INFO", "WARN", "ERROR", "CRITICAL")
     */
    void Set_level(const std::string& level) {
        _level = Parse_level(level);
    }

    /**
     * @brief Write a log message at the specified level.
     * @param level Log level for this message
     * @param msg Message format string
     * @param args Additional arguments to append
     */
    template<typename... Args>
    void Write(LOG_LEVEL level, const std::string& msg, Args... args) const {
        if (level <= _level) {
            _os << "[" << Get_cur_time() << "] " << Level_to_str(level) << ": " << Format(msg, args...) << std::endl;
        }
    }

    /**
     * @brief Log an error message.
     * @param msg Message string
     * @param args Additional arguments to append
     */
    template<typename... Args>
    void Error(const std::string& msg, Args... args) const {
        Write(LOG_LEVEL::ERROR, msg, args...);
    }

    /**
     * @brief Log a warning message.
     * @param msg Message string
     * @param args Additional arguments to append
     */
    template<typename... Args>
    void Warn(const std::string& msg, Args... args) const {
        Write(LOG_LEVEL::WARN, msg, args...);
    }

    /**
     * @brief Log an informational message.
     * @param msg Message string
     * @param args Additional arguments to append
     */
    template<typename... Args>
    void Info(const std::string& msg, Args... args) const {
        Write(LOG_LEVEL::INFO, msg, args...);
    }

    /**
     * @brief Log a debug message.
     * @param msg Message string
     * @param args Additional arguments to append
     */
    template<typename... Args>
    void Debug(const std::string& msg, Args... args) const {
        Write(LOG_LEVEL::DEBUG, msg, args...);
    }

private:
    std::ostream&       _os;        //!< Output stream
    LOG_LEVEL           _level;     //!< Current minimum log level

    LOGGER(std::ostream& os, const std::string& level) : _os(os), _level(Parse_level(level)) {}

    LOG_LEVEL Parse_level(const std::string& level);
    std::string Level_to_str(LOG_LEVEL level) const;
    std::string Get_cur_time() const;

    template<typename... Args>
    std::string Format(const std::string& msg, Args... args) const {
        std::ostringstream os;
        os << msg;
        (void)(os << ... << args);
        return os.str();
    }
};

} // namespace runtime
} // namespace ace

/** @brief Write log message at specified level */
#define LOG_WRITE(level, msg, ...)  ace::runtime::LOGGER::Instance().Write(level, msg, ##__VA_ARGS__)

/** @brief Log error message */
#define LOG_ERROR(msg, ...)         ace::runtime::LOGGER::Instance().Error(msg, ##__VA_ARGS__)

/** @brief Log warning message */
#define LOG_WARN(msg, ...)          ace::runtime::LOGGER::Instance().Warn(msg, ##__VA_ARGS__)

/** @brief Log informational message */
#define LOG_INFO(msg, ...)          ace::runtime::LOGGER::Instance().Info(msg, ##__VA_ARGS__)

/** @brief Log debug message */
#define LOG_DEBUG(msg, ...)         ace::runtime::LOGGER::Instance().Debug(msg, ##__VA_ARGS__)

#endif // ACE_RUNTIME_LOG_H