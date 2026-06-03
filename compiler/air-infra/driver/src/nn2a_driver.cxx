//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "nn2a_driver.h"

#include <iostream>

using namespace std;

int nn::driver::NN2A_DRIVER::Run() {
  cout << "Run" << endl;
  cout << "show option: " << Show() << endl;
  cout << "trace option: " << Trace_enabled() << endl;
  cout << "skip before: " << Skip_before() << endl;
  cout << "alias option: " << Alias() << endl;
  cout << "analysis enable option: " << Analysis_enable() << endl;
  cout << "o option: " << Output_file() << endl;
  cout << "include file: " << Include_file() << endl;
  return 0;
}

int nn::driver::NN2A_DRIVER::Fini() {
  cout << "Fini" << endl;
  return 0;
}