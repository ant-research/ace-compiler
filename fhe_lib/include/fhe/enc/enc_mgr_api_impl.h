//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief enc_mgr_api_impl.h
//! Common implementation for EncMgr C-linkage wrappers.
//!
//! Include this file once per backend, with the following macros defined:
//!   ENC_MGR_CTX_TYPE  — FheContext specialization, e.g. FheContext<VendorTraits>
//!   ENC_MGR_PT_TYPE   — Plaintext type, e.g. PhantomPlaintext
//!
//! Generates all 11 extern "C" functions required by the rtlib C API.

// Verify macros are defined
#ifndef ENC_MGR_CTX_TYPE
#error "ENC_MGR_CTX_TYPE must be defined before including enc_mgr_api_impl.h"
#endif
#ifndef ENC_MGR_PT_TYPE
#error "ENC_MGR_PT_TYPE must be defined before including enc_mgr_api_impl.h"
#endif

#include "common/rt_api.h"  // Get_enc_policy() weak declaration

extern "C"
{

    bool Pt_mgr_init(const char* fname)
    {
        return ENC_MGR_CTX_TYPE::Current().enc_mgr().Init(std::string(fname),
                                                          ENC_MGR_CTX_TYPE::Current().crypto_context(),
                                                          static_cast<fhert::EncPolicy>(Get_enc_policy()));
    }

    void Pt_mgr_fini()
    {
        ENC_MGR_CTX_TYPE::Current().enc_mgr().Fini();
    }

    void Pt_prefetch(uint32_t index)
    {
        (void)index;
    }

    void* Pt_get(uint32_t index, size_t len, uint32_t scale, uint32_t level)
    {
        (void)index;
        (void)len;
        (void)scale;
        (void)level;
        return nullptr;
    }

    void* Pt_get_validate(float* buf, uint32_t index, size_t len, uint32_t scale, uint32_t level)
    {
        (void)buf;
        (void)index;
        (void)len;
        (void)scale;
        (void)level;
        return nullptr;
    }

    void Pt_free(uint32_t index)
    {
        (void)index;
    }

    void Free_data(void* poly)
    {
        (void)poly;
    }

    // The C-linkage signature is aligned with rtlib/common/pt_mgr.h (adds the
    // extended-basis flag is_ext emitted by the mdown-sinking pass). The ace
    // backend's Load_encode -> CKKSEncoder::encode chain does not yet accept
    // is_ext, so it is accepted here for ABI parity and intentionally unused
    // until the ace backend gains an mdown-sinking encode path; it does NOT
    // reach Get_or_encode in this revision.
    void* Pt_from_msg(void* pt, uint32_t index, size_t len, uint32_t scale, uint32_t level, bool is_ext)
    {
        (void)is_ext;
        auto& enc_mgr = ENC_MGR_CTX_TYPE::Current().enc_mgr();
        auto* plain = static_cast<ENC_MGR_PT_TYPE*>(pt);
        return enc_mgr.Get_or_encode(*plain, index, len, scale, level, 0,
                                     *ENC_MGR_CTX_TYPE::Current().crypto_context());
    }

    void* Pt_from_msg_ofst(void* pt, uint32_t index, size_t ofst, size_t len, uint32_t scale, uint32_t level,
                           bool is_ext)
    {
        (void)is_ext;
        auto& enc_mgr = ENC_MGR_CTX_TYPE::Current().enc_mgr();
        auto* plain = static_cast<ENC_MGR_PT_TYPE*>(pt);
        return enc_mgr.Get_or_encode(*plain, index, len, scale, level, ofst,
                                     *ENC_MGR_CTX_TYPE::Current().crypto_context());
    }

    void Pt_from_msg_validate(void* pt, float* buf, uint32_t index, size_t len, uint32_t scale, uint32_t level,
                              bool is_ext)
    {
        (void)is_ext;
        auto& enc_mgr = ENC_MGR_CTX_TYPE::Current().enc_mgr();
        auto* plain = static_cast<ENC_MGR_PT_TYPE*>(pt);
        enc_mgr.Get_or_encode_validate(*plain, buf, index, len, scale, level, 0,
                                       *ENC_MGR_CTX_TYPE::Current().crypto_context());
    }

    void Pt_from_msg_ofst_validate(void* pt, float* buf, uint32_t index, size_t offset, size_t len, uint32_t scale,
                                   uint32_t level, bool is_ext)
    {
        (void)is_ext;
        auto& enc_mgr = ENC_MGR_CTX_TYPE::Current().enc_mgr();
        auto* plain = static_cast<ENC_MGR_PT_TYPE*>(pt);
        enc_mgr.Get_or_encode_validate(*plain, buf, index, len, scale, level, offset,
                                       *ENC_MGR_CTX_TYPE::Current().crypto_context());
    }

}  // extern "C"

#undef ENC_MGR_CTX_TYPE
#undef ENC_MGR_PT_TYPE
