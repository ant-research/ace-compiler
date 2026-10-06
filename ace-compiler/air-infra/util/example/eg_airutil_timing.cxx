//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <unistd.h>

#include "air/util/debug.h"
#include "air/util/timing.h"

// Interal instance print timing data to cout
void Test_time_taken() {
  TIME_START();

  // sleep(1);
  for (int i = 0; i < 100000000; ++i) i++;

  TIME_TAKEN("Test Point1");

  // sleep(2);
  for (int i = 0; i < 100000000; ++i) i++;

  TIME_TAKEN("Test Point2");
}

// External instance print timing data to cout
void Test_time_taken_ex() {
  air::util::TIMING tim;
  TIME_START_EX(tim);

  // sleep(1);
  for (int i = 0; i < 100000000; ++i) i++;

  TIME_TAKEN_EX(tim, "Test Point21");

  // sleep(2);
  for (int i = 0; i < 100000000; ++i) i++;

  TIME_TAKEN_EX(tim, "Test Point22");
}

// External instance print timing data to log file
void Test_time_taken_ex_log() {
  air::util::TIMING tim("timing.t");
  TIME_START_EX(tim);

  // usleep(10);
  for (int i = 0; i < 100000000; ++i) i++;

  TIME_TAKEN_EX(tim, "Test Point31");

  // usleep(10);
  for (int i = 0; i < 100000000; ++i) i++;

  TIME_TAKEN_EX(tim, "Test Point32");
}

int main(int argv, char** argc) {
  Test_time_taken();

  Test_time_taken_ex();

  Test_time_taken_ex_log();
}