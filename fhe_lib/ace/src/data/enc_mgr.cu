//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief enc_mgr.cu
//! EncMgr<VendorTraits> backend-specific method implementations.
//!
//! Load_encode: reads float data from DE_MSG_F32, encodes via FheContext.
//! Load_plaintext: loads pre-encoded DE_PLAINTEXT via EncIo.
//! Load_encode_validate: encodes with element-wise validation.

#include "common/error.h"
#include "common/rt_data_file.h"
#include "fhe/context.h"
#include "fhe/enc/enc_io.h"
#include "fhe/enc/enc_mgr.h"
#include "fhe/vendor/ace/vendor_traits.h"

#include <cmath>
#include <cstdio>
#include <unistd.h>
#include <vector>

namespace fhert
{

using Traits = ace::VendorTraits;
using FheCtx = FheContext<Traits>;

//=============================================================================
// Load_encode — DE_MSG_F32 path
//=============================================================================

template <> void* EncMgr<Traits>::Load_encode(Plaintext& pt, const EncodeParam& params, ContextT& ctx)
{
    (void)ctx;
    float* data = Msg_ptr(params._index, params._offset, params._len);
    Dump_msg_preview("Load_encode", params, data);

    // ACE encodes with cst_lvl (logical int level), converting via Runtime_level
    FheCtx::Current().Encode_float_cst_lvl(pt, data, params._len, params._scale, static_cast<int>(params._level));
    return &pt;
}

//=============================================================================
// Load_encode_validate — DE_MSG_F32 with validation
//=============================================================================

template <>
void EncMgr<Traits>::Load_encode_validate(Plaintext& pt, std::vector<float>& buf, const EncodeParam& params,
                                          ContextT& ctx)
{
    (void)ctx;
    float* data = Msg_ptr(params._index, params._offset, params._len);
    Dump_msg_preview("Load_encode_validate", params, data);

    for (uint32_t i = 0; i < params._len; ++i) {
        FMT_ASSERT(fabs(buf[i] - data[i]) < 0.000001f, "Load_encode_validate failed. index=%u, i=%u: %f != %f.",
                   params._index, i, buf[i], data[i]);
    }

    FheCtx::Current().Encode_float_cst_lvl(pt, data, params._len, params._scale, static_cast<int>(params._level));
}

}  // namespace fhert
