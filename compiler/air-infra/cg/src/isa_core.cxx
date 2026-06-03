//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/cg/isa_core.h"

#include "air/cg/targ_info.h"

namespace air {

namespace cg {

static constexpr struct INST_META CORE_INST_META_INVALID = {
    "invalid", 0, 0, 0, {}};

static constexpr struct INST_META CORE_INST_META_CHI = {
    "chi", CORE_CHI, 1, 1, {}};

static constexpr struct INST_META CORE_INST_META_PHI = {
    "phi", CORE_PHI, 1, 0, {}};

static constexpr struct INST_META const* CORE_INST_META[] = {
    &CORE_INST_META_INVALID,
    &CORE_INST_META_CHI,
    &CORE_INST_META_PHI,
};

static constexpr struct ISA_INFO_META CORE_ISA_INFO = {
    "isa-core",
    ISA_CORE,
    sizeof(CORE_INST_META) / sizeof(CORE_INST_META[0]),
    0,
    CORE_INST_META,
    nullptr};

void Register_core_isa() {
  const static bool ret = TARG_INFO_MGR::Register_isa_info(&CORE_ISA_INFO);
  AIR_ASSERT(ret == true);
}

}  // namespace cg

}  // namespace air
