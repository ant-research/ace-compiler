//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_PERF_H
#define AIR_UTIL_PERF_H

#include <time.h>

#include <ostream>

#include "air/util/debug.h"

namespace air {

namespace util {

class REPORT;  // forward declaration

//! @brief Tool for performance measure
class PERF {
public:
  //! @brief Add environment information and initialize
  PERF(TFILE& trace, TFILE& perf);

  //! @brief Store data to trace | perf file
  ~PERF(void);

  TFILE&  Get_tfile() { return _tfile; }
  TFILE&  Get_pfile() { return _pfile; }
  clock_t Get_clock_init() { return _init; }
  clock_t Get_clock_start() { return _start; }

  void Set_clock_start(clock_t time) { _start = time; }

  //! @brief for In order to calculate the time period accurately
  // It can also not call if you don't worry about precision
  void Start() { _start = clock(); }

  //! @todo Need to confirm which parameters to pass, driver/phase/pass
  void Taken(std::string driver, std::string phase, std::string pass);

  void Print(std::ostream& os, bool rot) const;

  //! @brief for Debug
  void Print() const;

private:
  TFILE&  _tfile;  // Trace file
  TFILE&  _pfile;  // Perf file
  REPORT* _data;   // perfmance data
  clock_t _init;   // measure time init
  clock_t _start;  // measure time start
};

}  // namespace util

}  // namespace air

#endif  // AIR_UTIL_PERF_H
