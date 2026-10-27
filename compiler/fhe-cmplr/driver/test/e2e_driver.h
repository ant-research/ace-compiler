//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_TEST_E2E_DRIVER_H
#define FHE_TEST_E2E_DRIVER_H

#include "fhe/driver/fhe_cmplr.h"

namespace fhe {

namespace test {

//<! @brief end-to-end test util
class E2E_DRIVER : public air::driver::DRIVER {
public:
  E2E_DRIVER() : air::driver::DRIVER(true), _cmplr(false) {}

  R_CODE Init(int argc, char* argv[]) {
    Init_opcode();
    _cmplr.Init(this);
    return air::driver::DRIVER::Init(argc, argv);
  }

  R_CODE Pre_run() { return _cmplr.Pre_run(); }

  R_CODE Run() {
    // create vector IR for e2e test
    air::base::GLOB_SCOPE* glob = Glob_scope();
    AIR_ASSERT(glob != nullptr);
    Build_vector_ir(glob);

    // run the rest pipeline
    return _cmplr.Run();
  }

  void Post_run() { _cmplr.Post_run(); }

  void Fini() { _cmplr.Fini(); }

private:
  void Init_opcode();
  void Build_vector_ir(air::base::GLOB_SCOPE* glob);

  fhe::driver::FHE_COMPILER _cmplr;

};  // E2E_DRIVER

}  // namespace test

}  // namespace fhe

#endif  // FHE_TEST_E2E_DRIVER_H
