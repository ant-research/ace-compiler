//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_TIMING_H
#define AIR_UTIL_TIMING_H

#include <time.h>

#include <ostream>

#include "air/util/debug.h"

namespace air {

namespace util {

/**
 * @brief Tool for Timing measure
 *
 */
class TIMING {
public:
  /**
   * @brief Construct a new TIMING object
   *
   */
  TIMING()
      : _trace(nullptr), _start(0), _prev(0), _curr(0), _taken(0), _total(0) {
    Tab_head();
  }

  /**
   * @brief Construct a new TIMING object
   *
   * @param stream trace to file
   */
  TIMING(const char* stream)
      : _trace(stream), _start(0), _prev(0), _curr(0), _taken(0), _total(0) {
    Tab_head();
  }

  /**
   * @brief Get the measurement start time and reset variable
   *
   * @return double start time
   */
  clock_t Start_time(const int line, const char* func, const char* remark) {
    _start = clock();
    _prev  = 0;
    _curr  = _start;
    _taken = 0;
    _total = 0;

    Trace(line, func, remark);

    return _start;
  }

  /**
   * @brief Obtain start time
   *
   * @return double start time
   */
  clock_t Get_start() { return _start; }

  /**
   * @brief Obtain last time for calc the time between routines
   *
   * @return double last time
   */
  clock_t Get_prev() { return _prev; }

  /**
   * @brief Obtain current time and trace time tab data
   *
   * @param line measured location
   * @param func measured function
   * @param remark Note the meaning of the measurement
   * @return double current time
   */
  clock_t Taken_time(const int line, const char* func, const char* remark) {
    _curr = clock();
    if (_prev == 0) {
      _taken = ((double)(_curr - _start)) / CLOCKS_PER_SEC;
    } else {
      _taken = ((double)(_curr - _prev)) / CLOCKS_PER_SEC;
    }
    _total = ((double)(_curr - _start)) / CLOCKS_PER_SEC;

    Trace(line, func, remark);

    _prev = _curr;

    return _curr;
  }

private:
  TFILE   _trace;
  clock_t _start;
  clock_t _prev;
  clock_t _curr;
  double  _taken;
  double  _total;

  const char* _tab_head = "%s\t%s\t\t%s\t\t\t%s";
  const char* _tab_data = "%d\t%s\t\t%fs / %fs\t\t%s";

  /**
   * @brief Trace timing tab header
   *
   */
  void Tab_head() {
    AIR_TRACE(_trace, _tab_head, "LINE", "FUNCTION", "TIMING", "REMARK");
  }

  /**
   * @brief Trace timing tab data
   *
   * @param line measured location
   * @param func measured function
   * @param remark Note the meaning of the measurement
   */
  void Trace(const int line, const char* func, const char* remark) {
    AIR_TRACE(_trace, _tab_data, line, func, _taken, _total, remark);
  }
};

}  // namespace util

}  // namespace air

extern air::util::TIMING Tim;

#define TIME_START()     Tim.Start_time(__LINE__, __FUNCTION__, "Start ...")
#define TIME_START_EX(x) x.Start_time(__LINE__, __FUNCTION__, "Start ...")

#define TIME_TAKEN(x)       Tim.Taken_time(__LINE__, __FUNCTION__, x)
#define TIME_TAKEN_EX(x, y) x.Taken_time(__LINE__, __FUNCTION__, y)

#endif  // AIR_UTIL_TIMING_H
