//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <json/json.h>
#include <unistd.h>

#include <iostream>

#include "air/util/debug.h"
#include "air/util/perf.h"

/*
// layout perf timing data for driver/phase/pass

// trace file on the bottom
[driver0][phase0][pass0] : phase_time = 0.24621799 / 0.246252 (ms) mem=
[driver0][phase0][pass1] : phase_time = 0.24870 / 0.494972(ms)
[driver0][phase1][pass0] : phase_time = 0.24870 / 0.738281 (ms)
[driver0][phase1][pass1] : phase_time = 0.252170 / 0.990466 (ms)

// json file to cti
{
   "context" : {
      "num_cpus" : 16
      "parallel" : “false”
      "... ..." :                         // TODO: add env info
      "library_version" : "0.0.1"
      "library_build_date" : "2023-12-03"
      "library_build_type" : "release"
   },
   "performace" : [
      {
         "cpu_time" : 0.246252,
         "driver_name" : "driver_0",
         "index" : 0,
         "pass_name" : "pass_0",
         "phase_name" : "phase_0",
         "phase_time" : 0.24621799999999999,
         "time_unit" : "us"
         "mem_size" : 123456              // TODO: add mem used
      },
      {
         "cpu_time" : 0.49497200000000002,
         "driver_name" : "driver_1",
         "index" : 1,
         "pass_name" : "pass_1",
         "phase_name" : "phase_1",
         "phase_time" : 0.24870500000000001,
         "time_unit" : "us"
      },
      {
         "cpu_time" : 0.73828199999999999,
         "driver_name" : "driver_2",
         "index" : 2,
         "pass_name" : "pass_2",
         "phase_name" : "phase_2",
         "phase_time" : 0.243279,
         "time_unit" : "us"
      },
      {
         "cpu_time" : 0.99046699999999999,
         "driver_name" : "driver_3",
         "index" : 3,
         "pass_name" : "pass_3",
         "phase_name" : "phase_3",
         "phase_time" : 0.25217000000000001,
         "time_unit" : "us"
      }
   ]
}
*/

// TODO : integration call in COMMON_CONFIG
// file object is trace | perf

int main(int argv, char** argc) {
  air::util::TFILE trace("test.t");
  air::util::TFILE perf("test.json");
  air::util::PERF  tim(trace, perf);

  for (int i = 0; i < 4; i++) {
    std::string driver = "driver_" + std::to_string(i);
    std::string phase  = "phase_" + std::to_string(i);
    std::string pass   = "pass_" + std::to_string(i);

    tim.Start();

    for (int i = 0; i < 100000000; ++i) i++;

    tim.Taken(driver, phase, pass);
  }

  tim.Print();
}

// TODO : integration call use MACRO
#if 0
// use MICRO, define in COMMON_CONFIG
air::util::TFILE Tfile("trace.t");
air::util::TFILE Pfile("perf.json");
PERF         Perf(Tfile, Pfile);

#define PERF_START()        Perf.Start()
#define PERF_TAKEN(x, y, z) Perf.Taken(x, y, z)
#define PERF_PRINT()        Perf.Print()

// for micro
int Func_call() {
  for (int i = 0; i < 4; i++) {
    std::string driver = "driver_" + std::to_string(i);
    std::string phase  = "phase_" + std::to_string(i);
    std::string pass   = "pass_" + std::to_string(i);

    PERF_START();

    // sleep(2) for test
    for (int i = 0; i < 100000000; ++i) i++;

    // Get node data
    PERF_TAKEN(driver, phase, pass);
  }

  // dump report
  PERF_PRINT();
}
#endif