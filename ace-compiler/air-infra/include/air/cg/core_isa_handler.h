//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_CG_CORE_ISA_HANDLER_H
#define AIR_CG_CORE_ISA_HANDLER_H

#include "air/cg/cgir_node.h"
#include "air/cg/isa_core.h"
#include "air/util/debug.h"

namespace air {
namespace cg {
template <typename IMPL>
class CORE_ISA_HANDLER {
public:
  template <typename VISITOR>
  void Handle(VISITOR* visitor, INST_PTR inst) {
    switch (inst->Opcode()) {
      case CORE_PHI: {
        IMPL().Handle_phi(visitor, inst);
        break;
      }
      case CORE_CHI: {
        IMPL().Handle_chi(visitor, inst);
        break;
      }
      default: {
        AIR_ASSERT_MSG(false, "Not supported core ISA opcode");
        break;
      }
    }
  }

  static constexpr uint8_t ID = ISA_CORE;
};
}  // namespace cg
}  // namespace air
#endif  // AIR_CG_CORE_ISA_HANDLER_H