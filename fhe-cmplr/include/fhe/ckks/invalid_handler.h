//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_INVALID_HANDLER_H
#define FHE_CKKS_INVALID_HANDLER_H

#include "air/base/container.h"
#include "air/base/container_decl.h"
#include "fhe/ckks/ckks_opcode.h"

namespace fhe {

namespace ckks {
class INVALID_HANDLER {
public:
#define CKKS_OPCODE(NAME, name, category, kid_num, fld_num, property) \
  template <typename RETV, typename VISITOR>                          \
  RETV Handle_##name(VISITOR* visitor, air::base::NODE_PTR node) {    \
    CMPLR_ASSERT(false, "Unexpected operator: " #name "");            \
    return RETV();                                                    \
  }
#include "fhe/ckks/opcode_def.inc"
};

}  // namespace ckks

}  // namespace fhe

#endif
