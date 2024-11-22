//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_POLY_INVALID_HANDLER_H
#define FHE_POLY_INVALID_HANDLER_H

#include "air/base/container.h"
#include "fhe/poly/opcode.h"

namespace fhe {
namespace poly {

class INVALID_HANDLER {
public:
  // invalid handler implementation for each OPCODE
#define DEF_OPCODE(NAME, name, cat, kid_num, fld_num, prop) \
  OPCODE_INVALID_HANDLER_GEN(NAME, name, cat, kid_num, fld_num, prop)
#include "opcode_def.inc"
#undef DEF_OPCODE
};

}  // namespace poly
}  // namespace fhe

#endif  // FHE_POLY_INVALID_HANDLER_H
