//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief enc_mgr.cu
//! EncMgr<VendorTraits> explicit instantiation.
//!
//! Backend-specific methods for weight loading: Load_encode (DE_MSG_F32),
//! Load_plaintext (DE_PLAINTEXT), and Load_encode_validate.
//! Backend-agnostic methods (Init, Fini, Msg_ptr, Dump_*) are in the
//! template header (enc_mgr.h) and shared across all backends.

#include "fhe/context.h"
#include "fhe/enc/enc_io.h"
#include "fhe/enc/enc_mgr.h"
#include "fhe/vendor/phantom/vendor_traits.h"

#include <cmath>
#include <spdlog/spdlog.h>
#include <unistd.h>

namespace fhert
{

//=============================================================================
// EncMgr<VendorTraits> — Load_encode (DE_MSG_F32 path)
//=============================================================================

template <> void* EncMgr<phantom::VendorTraits>::Load_encode(Plaintext& pt, const EncodeParam& params, ContextT& ctx)
{

    float* data = Msg_ptr(params._index, params._offset, params._len);
    Dump_msg_preview("Load_encode", params, data);

    // Encode using the FheContext's Encode_float method
    using FheCtx = fhert::FheContext<phantom::VendorTraits>;
    auto& fhe_ctx = FheCtx::Current();
    fhe_ctx.Encode_float(pt, data, params._len, params._scale, params._level);

    return &pt;
}

//=============================================================================
// EncMgr<VendorTraits> — Load_encode_validate
//=============================================================================

template <>
void EncMgr<phantom::VendorTraits>::Load_encode_validate(Plaintext& pt, std::vector<float>& buf,
                                                         const EncodeParam& params, ContextT& ctx)
{

    float* data = Msg_ptr(params._index, params._offset, params._len);
    Dump_msg_preview("Load_encode_validate", params, data);
    for (uint32_t i = 0; i < params._len; ++i) {
        FMT_ASSERT(fabs(buf[i] - data[i]) < 0.000001, "Load_encode_validate failed. index=%u, i=%u: %f != %f.",
                   params._index, i, buf[i], data[i]);
    }

    using FheCtx = fhert::FheContext<phantom::VendorTraits>;
    auto& fhe_ctx = FheCtx::Current();
    fhe_ctx.Encode_float(pt, data, params._len, params._scale, params._level);
}

}  // namespace fhert
