//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

// eg_airdriver_01.cxx
//
// An simple example to describe the usage of PASS, PIPELINE, PASS_MANAGER and
// DRIVER. We declare SHARED_DATA to contain shared data between passes. The
// SHARED_DATA instance is maintained by EXAMPLE_DRIVER. Two passes 'PASS_A'
// and 'PASS_B' are introduced, which put/get data from EXAMPLE_DRIVER. On top
// of PASS_A and PASS_B, a pipeline was constructed. Then the pipeline is used
// to create the PASS_MANAGER which is instantiated by EXAMPLE_DRIVER.
//
// The ownership of all objects are listed below:
// EXAMPLE_DRIVER driver
//  +-- SHARED_DATA _shared_data
//  +-- EXAMPLE_PASS_MANAGER _pass_mgr
//       +-- PASS_A std::tuple<0>(_passes)
//       +-- PASS_B std::tuple<1>(_passes)
//
// The calling sequence are listed below:
// main()
//  +-- driver.Init()
//       +-- _pass_mgr.Init()
//            +-- PASS_A std::tuple<0>(_passes).Init()
//            +-- PASS_B std::tuple<1>(_passes).Init()
//  +-- driver.Pre_run()
//       +-- _pass_mgr.Pre_run()
//            +-- PASS_A std::tuple<0>(_passes).Pre_run()
//            +-- PASS_B std::tuple<1>(_passes).Pre_run()
//  +-- driver.Run()
//       +-- _pass_mgr.Run()
//            +-- PASS_A std::tuple<0>(_passes).Run()
//            +-- PASS_B std::tuple<1>(_passes).Run()
//  +-- driver.Post_run()
//       +-- _pass_mgr.Post_run()
//            +-- PASS_B std::tuple<1>(_passes).Post_run()
//            +-- PASS_A std::tuple<0>(_passes).Post_run()
//  +-- driver.Fini()
//       +-- _pass_mgr.Fini()
//            +-- PASS_B std::tuple<1>(_passes).Fini()
//            +-- PASS_A std::tuple<0>(_passes).Fini()
//

#include <iostream>

#include "air/driver/common_config.h"
#include "air/driver/driver.h"

using namespace std;
using namespace air::driver;

namespace {

// define a class to contain shared data between two passes
class SHARED_DATA {
public:
  SHARED_DATA() : _value("(empty)") {}

  void Put(const char* producer, const char* value) {
    cout << "[SHARED]: " << producer << " puts " << value << endl;
    _value = value;
  }

  const char* Get(const char* consumer) const {
    cout << "[SHARED]: " << consumer << " gets " << _value << endl;
    return _value;
  }

private:
  const char* _value;
};  // SHARED_DATA

// define a template class to simulate the PASS behavior
// here we pass DRIVER as template argument
template <typename DRIVER, typename SELF>
class PASS_TMPL : public PASS<air::util::COMMON_CONFIG> {
public:
  R_CODE Init(DRIVER* driver) {
    cout << SELF::ID << "::Init" << endl;
    _driver = driver;
    _driver->Get(SELF::ID);
    _driver->Put(SELF::ID, "Init");
    return R_CODE::NORMAL;
  }

  R_CODE Pre_run() {
    cout << SELF::ID << "::Pre_run" << endl;
    _driver->Get(SELF::ID);
    _driver->Put(SELF::ID, "Pre_run");
    return R_CODE::NORMAL;
  }

  R_CODE Run() {
    cout << SELF::ID << "::Run" << endl;
    _driver->Get(SELF::ID);
    _driver->Put(SELF::ID, "Run");
    return R_CODE::NORMAL;
  }

  void Post_run() {
    cout << SELF::ID << "::Post_run" << endl;
    _driver->Get(SELF::ID);
    _driver->Put(SELF::ID, "Post_run");
  }

  void Fini() {
    cout << SELF::ID << "::Fini" << endl;
    _driver->Get(SELF::ID);
    _driver->Put(SELF::ID, "Fini");
  }

  const char* Name() { return SELF::ID; }

private:
  DRIVER* _driver;

};  // PASS_TMPL

// identifier for PASS_A
struct PASS_A_ID {
  static constexpr const char* ID = "PASS_A";
};

// identifier for PASS_B
struct PASS_B_ID {
  static constexpr const char* ID = "PASS_B";
};

// the example driver
class EXAMPLE_DRIVER : public DRIVER {
public:
  EXAMPLE_DRIVER() : DRIVER(true) {}

  R_CODE Init(int argc, char** argv) {
    DRIVER::Init(argc, argv);
    return _pass_mgr.Init(this);
  }

  R_CODE Pre_run() { return _pass_mgr.Pre_run(this); }

  R_CODE Run() { return _pass_mgr.Run(this); }

  void Post_run() { _pass_mgr.Post_run(this); }

  void Fini() { _pass_mgr.Fini(this); }

  void Put(const char* producer, const char* value) {
    _shared_data.Put(producer, value);
  }

  const char* Get(const char* consumer) const {
    return _shared_data.Get(consumer);
  }

private:
  typedef PASS_TMPL<EXAMPLE_DRIVER, PASS_A_ID> PASS_A;
  typedef PASS_TMPL<EXAMPLE_DRIVER, PASS_B_ID> PASS_B;
  typedef PASS_MANAGER<PASS_A, PASS_B>         EXAMPLE_PASS_MANAGER;

  EXAMPLE_PASS_MANAGER _pass_mgr;
  SHARED_DATA          _shared_data;

};  // EXAMPLE_DRIVER

}  // anonymous namespace

// main
int main() {
  int            argc   = 2;
  const char*    argv[] = {"driver", "/dev/null"};
  EXAMPLE_DRIVER driver;
  driver.Init(argc, (char**)argv);
  driver.Pre_run();
  driver.Run();
  driver.Post_run();
  driver.Fini();
  return 0;
}  // main
