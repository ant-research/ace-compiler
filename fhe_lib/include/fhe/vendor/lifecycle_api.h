//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @file lifecycle_api.h
//! @brief Backend-parameterized dlsym lifecycle entry points.
//!
//! `KernelRunner` resolves five C-linkage symbols from each backend's runtime
//! — `Prepare_context`, `Finalize_context`, `Prepare_input`, `Handle_output`,
//! and `Main_graph` (`weak`) — via `dlsym` on the loaded kernel .so (which
//! links the backend .so as a NEEDED dep). The bodies were previously
//! duplicated verbatim (modulo the `FheContext<Backend>` typedef) in each
//! backend's `api.cu`. This header defines them once, parameterized on
//! `VendorTraits`, via a macro that each backend `api.cu` invokes once.
//!
//! A macro is used (rather than the template-helper + extern "C" shim split
//! used for the KMS API in fhe/kms/kms_api.h) because the lifecycle symbol
//! bodies share file-scope owned-context state (`g_owned_instance`) that is
//! inherently per-translation-unit and cannot be threaded through a template
//! instance; the macro keeps that state local to each backend .so.
//!
//! Architectural placement: `fhe/vendor/`, the same layer as `VendorTraits` —
//! the bodies depend only on `FheContext<Backend>` plus common-layer helpers
//! (`Io_init`/`Io_fini`, `Get_key_mode`/`Get_enc_policy`, `Init_rtlib_timing`)
//! that every backend already includes. The common helpers themselves are NOT
//! duplicated here.
//!
//! Unified semantics across backends:
//! - `Finalize_context` always calls `cudaDeviceSynchronize()` before
//!   destroying the context, so no backend relies on the Python side to
//!   synchronize first. (Historically phantom did this and ACE relied on a
//!   Python-side synchronize; the unified version adopts the safer rule.)

#ifndef FHE_VENDOR_LIFECYCLE_API_H
#define FHE_VENDOR_LIFECYCLE_API_H

#include "common/io_api.h"
#include "common/rt_api.h"
#include "common/rt_runtime_config.h"
#include "common/rtlib_timing.h"
#include "fhe/context.h"

#include <cstdio>
#include <cuda_runtime.h>
#include <spdlog/spdlog.h>
#include <utility>

//! Define the five dlsym lifecycle symbols for the backend `Backend`
//! (a `VendorTraits` type). Invoke exactly once per backend `api.cu`,
//! at file scope. Emits a file-scope owned `FheContext<Backend>` pointer
//! plus the five `extern "C"` definitions; the owning pointer must NOT be
//! redeclared by the caller.
#define FHERT_DEFINE_LIFECYCLE_API(Backend)                                                                            \
    namespace                                                                                                          \
    {                                                                                                                  \
    using FHERT_Lifecycle_Context = fhert::FheContext<Backend>;                                                        \
    FHERT_Lifecycle_Context* g_owned_instance = nullptr;                                                               \
    } /* anonymous namespace */                                                                                        \
                                                                                                                       \
    extern "C"                                                                                                         \
    {                                                                                                                  \
                                                                                                                       \
        void Prepare_context()                                                                                         \
        {                                                                                                              \
            Init_rtlib_timing();                                                                                       \
            Io_init();                                                                                                 \
                                                                                                                       \
            /* KeyMode / EncPolicy read from global C state (set by Python via */                                      \
            /* Set_key_mode / Set_enc_policy). */                                                                      \
            auto key_mode = static_cast<fhert::KeyMode>(Get_key_mode());                                               \
            auto enc_policy = static_cast<fhert::EncPolicy>(Get_enc_policy());                                         \
                                                                                                                       \
            /* Create the FheContext and make it the thread-local current context. */                                  \
            auto ctx = FHERT_Lifecycle_Context::Create(key_mode, enc_policy);                                          \
            g_owned_instance = ctx.release();                                                                          \
            g_owned_instance->MakeCurrent();                                                                           \
                                                                                                                       \
            /* LOAD_ENCODED: pre-encode raw (DE_MSG_F32) weights into the in-memory */                                 \
            /* cache at init time so inference pays zero encoding cost. Must run */                                    \
            /* after MakeCurrent() because Preload_encode() goes through */                                            \
            /* FheContext::Current(). No-op for already-encoded (DE_PLAINTEXT). */                                     \
            if (enc_policy == fhert::EncPolicy::LOAD_ENCODED && g_owned_instance->enc_mgr().Is_initialized() &&        \
                !g_owned_instance->enc_mgr().is_plaintext()) {                                                         \
                g_owned_instance->enc_mgr().Preload_encode();                                                          \
            }                                                                                                          \
        }                                                                                                              \
                                                                                                                       \
        void Finalize_context()                                                                                        \
        {                                                                                                              \
            /* Synchronize so no async CUDA op (cudaFreeAsync etc.) is in flight */                                    \
            /* when FheContext's destructor runs. Required on every backend — do */                                  \
            /* not rely on the Python side to call cudaDeviceSynchronize(). */                                         \
            cudaDeviceSynchronize();                                                                                   \
            FHERT_Lifecycle_Context::ClearCurrent();                                                                   \
            if (g_owned_instance) {                                                                                    \
                delete g_owned_instance;                                                                               \
                g_owned_instance = nullptr;                                                                            \
            }                                                                                                          \
            Io_fini();                                                                                                 \
        }                                                                                                              \
                                                                                                                       \
        void Prepare_input(TENSOR* input, const char* name)                                                            \
        {                                                                                                              \
            FHERT_Lifecycle_Context::Current().Prepare_input(input, name);                                             \
        }                                                                                                              \
                                                                                                                       \
        /* Mode B-P: make the single globally-owned FheContext the calling thread's */                                 \
        /* current context. Worker pthreads start with a null thread-local current ctx, so */                          \
        /* FHE op dispatch (which goes FheContext::Current()) and Activate_session_for_this_thread */                  \
        /* (which uses Current()) would see null on a worker unless this is called first. Returns 0/-1. */             \
        int Kms_make_current_on_this_thread()                                                                          \
        {                                                                                                              \
            if (g_owned_instance) {                                                                                    \
                g_owned_instance->MakeCurrent();                                                                       \
                return 0;                                                                                              \
            }                                                                                                          \
            return -1;                                                                                                 \
        }                                                                                                              \
                                                                                                                       \
        double* Handle_output(const char* name)                                                                        \
        {                                                                                                              \
            return FHERT_Lifecycle_Context::Current().Handle_output(name, 0);                                          \
        }                                                                                                              \
                                                                                                                       \
        bool Main_graph() __attribute__((weak));                                                                       \
        bool Main_graph()                                                                                              \
        {                                                                                                              \
            /* Typically supplied by the compiled kernel .so; placeholder here. */                                     \
            return true;                                                                                               \
        }                                                                                                              \
                                                                                                                       \
    } /* extern "C" */

#endif  // FHE_VENDOR_LIFECYCLE_API_H