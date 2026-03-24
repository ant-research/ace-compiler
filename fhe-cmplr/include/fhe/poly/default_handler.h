//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_POLY_DEFAULT_HANDLER_H
#define FHE_POLY_DEFAULT_HANDLER_H

#include "air/base/container.h"
#include "air/base/opcode_gen.h"
#include "fhe/poly/opcode.h"

namespace fhe {
namespace poly {

//! @brief Default handler which always call visitor Context's Handle_node
//! and Handle_block to handle nodes
class DEFAULT_HANDLER {
public:
  // null handler implementation for each OPCODE
#define DEF_OPCODE(NAME, name, kid_num, fld_num, prop) \
  OPCODE_DEFAULT_HANDLER_GEN(NAME, name, kid_num, fld_num, prop)
#include "opcode_def.inc"
#undef DEF_OPCODE
};

}  // namespace poly
}  // namespace fhe

#endif  // FHE_POLY_DEFAULT_HANDLER_H
