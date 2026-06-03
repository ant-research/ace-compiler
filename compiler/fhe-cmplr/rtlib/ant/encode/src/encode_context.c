//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <stdint.h>

#include "ckks/encoder.h"
#include "ckks/param.h"
#include "common/cmplr_api.h"
#include "common/rtlib_timing.h"
#include "context/ckks_context.h"
#include "fhe/core/rt_encode_api.h"

void Prepare_encode_context(uint32_t degree, uint32_t sec_level, uint32_t depth,
                            uint32_t input_lev, uint32_t first_mod_size,
                            uint32_t scaling_mod_size, uint32_t num_q_parts,
                            uint32_t hamming_weight) {
  Prepare_context_for_cmplr(degree, sec_level, depth, input_lev, first_mod_size,
                            scaling_mod_size, num_q_parts, hamming_weight);
}

void Finalize_encode_context() { Finalize_context_for_cmplr(); }
