//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief api.cu
//! dlsym entry points and backward-compat C API shims for the Phantom backend.
//!
//! The 5 symbols resolved via dlsym() by KERNEL_RUNNER are:
//!   Prepare_context, Finalize_context, Prepare_input, Handle_output, Main_graph
//!
//! All C API functions (Phantom_save_keys, etc.) are backward-compat shims
//! that delegate to FheContext<VendorTraits> methods.

#include "common/io_api.h"
#include "common/rt_api.h"
#include "common/rt_runtime_config.h"
#include "common/rtlib_timing.h"
#include "fhe/context.h"
#include "fhe/kms/key_serializer.h"
#include "fhe/kms/kms_api.h"
#include "fhe/kms/kms_runtime_api.h"
#include "fhe/need_bts.h"
#include "fhe/util/logging.h"
#include "fhe/vendor/lifecycle_api.h"
#include "fhe/vendor/phantom/rt_phantom.h"
#include "fhe/vendor/phantom/vendor_traits.h"

#include <spdlog/spdlog.h>

using FHertPhantomContext = fhert::FheContext<phantom::VendorTraits>;

//=============================================================================
// dlsym entry points (extern "C" — resolved by KERNEL_RUNNER)
//
// Prepare_context / Finalize_context / Prepare_input / Handle_output /
// Main_graph are defined once for all backends in
// fhe/vendor/lifecycle_api.h and instantiated here for phantom.
//=============================================================================
FHERT_DEFINE_LIFECYCLE_API(phantom::VendorTraits)

//=============================================================================
// KMS dlsym contract
//
// KMS save/load/query logic + the galois load timer are unified in fhert::
// (fhe/kms/kms_api.h), parameterized on VendorTraits. Each backend exports a
// PascalCase C symbol set (matching the Prepare_context/Handle_output dlsym
// convention) resolved by KERNEL_RUNNER — never accessed from Python ctypes.
// The legacy FHert_* / Phantom_* / Ace_* alias names are retired; Python
// callers reach these through _C / FHERuntime, not by dlopen-ing the .so.
// Contract declarations: fhe/kms/kms_runtime_api.h.
//=============================================================================

int Kms_save_keys(const char* key_dir, const uint32_t* galois_elts, size_t num_elts)
{
    return fhert::SaveKeys<phantom::VendorTraits>(key_dir, galois_elts, num_elts);
}
int Kms_load_keys(const char* key_dir)
{
    return fhert::LoadKeys<phantom::VendorTraits>(key_dir);
}
int Kms_save_eval_keys(const char* key_dir, const uint32_t* galois_elts, size_t num_elts)
{
    return fhert::SaveEvalKeys<phantom::VendorTraits>(key_dir, galois_elts, num_elts);
}
int Kms_load_eval_keys(const char* key_dir)
{
    return fhert::LoadEvalKeys<phantom::VendorTraits>(key_dir);
}
int Kms_keys_loaded_from_kms()
{
    return fhert::KeysLoadedFromKms<phantom::VendorTraits>();
}
double Kms_get_galois_load_time_seconds()
{
    return fhert::GetGaloisLoadTimeSeconds<phantom::VendorTraits>();
}
int Kms_create_session(const char* key_dir)
{
    return fhert::CreateSession<phantom::VendorTraits>(key_dir);
}
int Kms_switch_session(int idx)
{
    return fhert::SwitchSession<phantom::VendorTraits>(idx);
}

int Kms_activate_session(int idx)
{
    return fhert::ActivateSession<phantom::VendorTraits>(idx);
}

int Kms_num_sessions()
{
    return fhert::NumSessions<phantom::VendorTraits>();
}

int Kms_release_session(int idx)
{
    return fhert::ReleaseSession<phantom::VendorTraits>(idx);
}

long Kms_run_cipher(int session_idx, bool (*kernel_fn)(), const char* const* input_names, const char* const* input_bufs,
                    const size_t* input_sizes, size_t n_inputs, char* out_buf, size_t out_cap, const char* output_name)
{
    return fhert::RunCipher<phantom::VendorTraits>(session_idx, kernel_fn, input_names, input_bufs, input_sizes,
                                                   n_inputs, out_buf, out_cap, output_name);
}

//! Fill out[4] with the last RunCipher call's per-stage seconds on THIS thread:
//! out[0]=deser+sync, out[1]=launder+sync, out[2]=kernel(eval), out[3]=serialize.
//! Returns 0 on success. Lets the server echo per-stage timings to the client
//! (server/client are separate environments).
int Kms_last_runcipher_timing(double* out)
{
    if (out == nullptr)
        return -1;
    const auto& t = fhert::g_last_runcipher_timing;
    out[0] = t.deser_sync;
    out[1] = t.launder_sync;
    out[2] = t.kernel;
    out[3] = t.serialize;
    return 0;
}

long Kms_encrypt_input(int session_idx, const char* input_name, const double* plain, size_t n_elems, char* out_buf,
                       size_t out_cap)
{
    return fhert::EncryptInput<phantom::VendorTraits>(session_idx, input_name, plain, n_elems, out_buf, out_cap);
}

long Kms_decrypt_output(int session_idx, const char* output_name, const char* in_buf, size_t in_size,
                        double* out_doubles, size_t out_cap_doubles)
{
    return fhert::DecryptOutput<phantom::VendorTraits>(session_idx, output_name, in_buf, in_size, out_doubles,
                                                       out_cap_doubles);
}

long Kms_encrypt_decrypt_plain(int session_idx, const double* plain, size_t n_elems, double* out_doubles,
                               size_t out_cap_doubles)
{
    return fhert::EncryptDecryptPlain<phantom::VendorTraits>(session_idx, plain, n_elems, out_doubles, out_cap_doubles);
}

long Kms_serialize_fields_diag(int session_idx, const double* plain, size_t n_elems, double* out_fields, size_t out_cap)
{
    return fhert::SerializeFieldsDiag<phantom::VendorTraits>(session_idx, plain, n_elems, out_fields, out_cap);
}

long Kms_serialize_io_input(int session_idx, const char* name, char* out_buf, size_t out_cap)
{
    return fhert::SerializeIoInput<phantom::VendorTraits>(session_idx, name, out_buf, out_cap);
}
