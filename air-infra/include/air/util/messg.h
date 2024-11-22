//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_MESSG_H
#define AIR_UTIL_MESSG_H

#include <stdarg.h>
#include <string.h>

#include <fstream>
#include <iostream>
#include <ostream>
#include <utility>

#include "air/util/error.h"
#include "air/util/srcdbg.h"
#include "err_msg.inc.h"

namespace air {

namespace util {

/**
 * @brief trace file handles
 *
 */
class TFILE {
public:
  /**
   * @brief Construct a new TFILE object, trace on cout
   *
   */
  TFILE(void) { _tfbuf = _tfile.basic_ios<char>::rdbuf(std::cout.rdbuf()); }

  /**
   * @brief Construct a new TFILE object
   *
   * @param file type cosnt char*
   */
  TFILE(const char* file) {
    if (file == NULL) {
      _tfbuf = _tfile.basic_ios<char>::rdbuf(std::cout.rdbuf());
      return;
    }

    _tfile.open(file, std::ios::out);
    if (!_tfile) {
      // fail to open trace file
      // print error message
      // bail out the compiler session
      std::cout << " fail to open trace file " << file << std::endl;
    }
  };

  /**
   * @brief Construct a new TFILE object
   *
   * @param file type const std::string
   */
  TFILE(const std::string& file) {
    if (file.empty()) {
      _tfbuf = _tfile.basic_ios<char>::rdbuf(std::cout.rdbuf());
      return;
    }

    _tfile.open(file, std::ios::out);
    if (!_tfile) {
      // fail to open trace file
      // print error message
      // bail out the compiler session
      std::cout << " fail to open trace file " << file << std::endl;
    }
  }

  /**
   * @brief Open a new file for tracing
   *
   * @param file trace file name
   */
  void Open(const char* file) {
    if (_tfbuf) {
      _tfile.basic_ios<char>::rdbuf(_tfbuf);
    }
    _tfile.open(file, std::ios::out);
    if (!_tfile) {
      std::cout << " fail to open trace file " << file << std::endl;
    }
  }

  /**
   * @brief Destroy the TFILE object; close Trace_file()
   *
   */
  ~TFILE(void) {
    if (_tfile.is_open()) {
      _tfile.close();
    }
  };

  /**
   * @brief Check if trace file is open
   *
   * @param void
   */
  bool Is_open(void) { return _tfile.is_open(); }

  /**
   * @brief Access private member _tfile
   *
   * @return ofstream& ofstream handle
   */
  std::ofstream& Tfile(void) { return _tfile; }

private:
  // TFILE(void);                   // REQUIRED UNDEFINED UNWANTED methods
  TFILE(const TFILE&);             // REQUIRED UNDEFINED UNWANTED methods
  TFILE& operator=(const TFILE&);  // REQUIRED UNDEFINED UNWANTED methods

  std::ofstream               _tfile;
  std::basic_streambuf<char>* _tfbuf;
};

}  // namespace util

}  // namespace air

// Predefined
// Used in the generated error description table (for the format statement in
// err msg) poorman's way of meta description of number of params and their type
// in error msg

enum class ERRT_DESC {
  NONE    = 0,
  INT32   = 1,
  INT64   = 2,
  FLOAT   = 3,
  DOUBLE  = 4,
  POINTER = 5,
  STRING  = 6
};

// Predefined
#define MAX_PARMS   5
#define MSG_MAXSIZE 512  // 512 bytes max for any output message

typedef struct {
  U_CODE
  _err_code;  // used to index into the actual fmt statement, do not reorder
  SEVL        _severity;
  const char* _fmt;
  ERRT_DESC   _actual[MAX_PARMS];
} MSG_DESC;

/**
 * @brief Print Handle
 *
 * @tparam outstream Stream type of the message
 * @tparam T Type of Print data
 * @param out Indicate the Print device of message
 * @param s Ouptut data of Print
 */
template <class outstream, typename T>
void Templ_print(outstream& out, const T& s) {
  out << s << std::endl;
}

/**
 * @brief Print Handle
 *
 * @tparam outstream Stream type of the message
 * @tparam T
 * @tparam Args
 * @param out
 * @param format
 * @param value
 * @param args
 */
template <class outstream, typename T, typename... Args>
void Templ_print(outstream& out, const char* format, T value, Args... args) {
  while (*format) {
    if (*format == '%' && *(++format) != '%') {
      out << value;
      Templ_print(out, ++format, args...);
      return;
    }
    out << *format++;
  }
  exit(1);
}

/**
 * @brief Message handle
 *
 * @tparam Stream Stream type of the message
 * @param out Indicate the Output device of the message
 * @param file The location of the message file name
 * @param line The location of the message line
 * @param sv Type of the message
 */
template <class Stream>
void Msg_hdr(Stream& out, const char* file, int line, const SEVL sv) {
  out << file << ':' << line << ": " << Sv_to_string(sv) << ": ";
}

/**
 * @brief Message handle
 *
 * @tparam Stream Stream type of the message
 * @param out Indicate the Output device of the message
 * @param sv Type of the message
 */
template <class Stream>
void Msg_hdr(Stream& out, const SEVL sv) {
  out << Sv_to_string(sv) << ": ";
}

/**
 * @brief Assert handle
 *
 * @tparam Stream Type of the assert message
 * @param out Indicate the Output device of the assert message
 * @param file The location of the assert file name
 * @param line The location of the assert line
 */
template <class Stream>
void Assert_hdr(Stream& out, const char* file, int line) {
  out << file << ':' << line << ": "
      << "Assertion Failure"
      << ": ";
}

#endif  // AIR_UTIL_MESSG_H
