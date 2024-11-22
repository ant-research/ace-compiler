//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_SIHE_INVALID_HANDLER_H
#define FHE_SIHE_INVALID_HANDLER_H

#include "air/base/container.h"
#include "air/base/opcode_gen.h"
#include "fhe/sihe/sihe_opcode.h"

namespace fhe {

namespace sihe {

//! @brief Invalid handler which assert any node by default
class INVALID_HANDLER {
public:
  // default handler implementation for each OPCODE
#define SIHE_OPCODE(NAME, name, cat, kid_num, fld_num, prop) \
  OPCODE_INVALID_HANDLER_GEN(NAME, name, cat, kid_num, fld_num, prop)
#include "opcode_def.inc"
#undef SIHE_OPCODE
};

}  // namespace sihe

}  // namespace fhe

#endif  // FHE_SIHE_INVALID_HANDLER_H
