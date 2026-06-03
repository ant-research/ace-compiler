//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_SSA_VERIFY_H
#define AIR_OPT_SSA_VERIFY_H

#include "air/opt/ssa_container.h"

//! @brief Verify SSA
//! Verify if SSA is constructed correctly:
//!   - Verify SSA container STATE
//!   - Verify if symbol in the same group has same AIR symbol or preg
//!   - Verify if symbol has PHI on all outer SCFs if it's defined in side SCF
//!   - Verify symbol on a node matches with the symbol in the version
//!   - Verify version of def matches with version on def-stmt
//!   - Verify version of use matches with top version during renaming

namespace air {

namespace opt {

//! @brief SSA_VERIFIER
class SSA_VERIFIER {
public:
  SSA_VERIFIER(SSA_CONTAINER* cont) : _cont(cont) {}

  void Perform() { Verify_simple(); }

private:
  void Verify_simple();

  SSA_CONTAINER* _cont;
};

}  // namespace opt

}  // namespace air

#endif  // AIR_OPT_SSA_VERIFY_H
