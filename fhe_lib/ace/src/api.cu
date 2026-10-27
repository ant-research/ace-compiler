//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "common/common.h"
#include "common/io_api.h"
#include "common/rt_api.h"
#include "common/rt_runtime_config.h"
#include "common/rtlib_timing.h"
#include "common/tensor.h"
#include "common/trace.h"
#include "fhe/context.h"
#include "fhe/kms/kms_api.h"
#include "fhe/kms/kms_runtime_api.h"
#include "fhe/need_bts.h"
#include "fhe/util/logging.h"
#include "fhe/vendor/ace/ace_config.h"
#include "fhe/vendor/ace/vendor_traits.h"
#include "fhe/vendor/lifecycle_api.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <spdlog/spdlog.h>
#include <vector>

using FHertAceContext = fhert::FheContext<ace::VendorTraits>;

namespace
{

using Ciphertext = acelib::DeviceCipher;
using Plaintext = acelib::DevicePlain;

#ifndef ACE_LIBRARY_DEBUG
#define ACE_LIBRARY_DEBUG 0
#endif

constexpr bool kAceLibraryDebug = ACE_LIBRARY_DEBUG != 0;

FILE* Ace_dump_file()
{
    return Is_trace_on() ? Get_trace_file() : stdout;
}

FILE* Ace_debug_file()
{
    return Is_trace_on() ? Get_trace_file() : stderr;
}

}  // namespace

//=============================================================================
// Lifecycle C API
//
// Prepare_context / Finalize_context / Prepare_input / Handle_output /
// Main_graph are defined once for all backends in fhe/vendor/lifecycle_api.h
// and instantiated here for ACE.
//=============================================================================
FHERT_DEFINE_LIFECYCLE_API(ace::VendorTraits)

//=============================================================================
// Pt MGR accessor
//=============================================================================

//=============================================================================
// KMS dlsym contract
//
// KMS save/load/query logic is unified in fhert:: (fhe/kms/kms_api.h),
// parameterized on VendorTraits. Each backend exports a PascalCase C symbol
// set (matching the Prepare_context/Handle_output dlsym convention) resolved
// by KERNEL_RUNNER — never accessed from Python ctypes. The legacy FHert_* /
// Ace_* / Phantom_* alias names are retired; Python callers reach these
// through _C / FHERuntime. Contract declarations: fhe/kms/kms_runtime_api.h.
//=============================================================================

int Kms_save_keys(const char* key_dir, const uint32_t* galois_elts, size_t num_elts)
{
    return fhert::SaveKeys<ace::VendorTraits>(key_dir, galois_elts, num_elts);
}
int Kms_load_keys(const char* key_dir)
{
    return fhert::LoadKeys<ace::VendorTraits>(key_dir);
}
int Kms_save_eval_keys(const char* key_dir, const uint32_t* galois_elts, size_t num_elts)
{
    return fhert::SaveEvalKeys<ace::VendorTraits>(key_dir, galois_elts, num_elts);
}
int Kms_load_eval_keys(const char* key_dir)
{
    return fhert::LoadEvalKeys<ace::VendorTraits>(key_dir);
}
int Kms_keys_loaded_from_kms()
{
    return fhert::KeysLoadedFromKms<ace::VendorTraits>();
}
double Kms_get_galois_load_time_seconds()
{
    return fhert::GetGaloisLoadTimeSeconds<ace::VendorTraits>();
}
int Kms_create_session(const char* key_dir)
{
    return fhert::CreateSession<ace::VendorTraits>(key_dir);
}
int Kms_switch_session(int idx)
{
    return fhert::SwitchSession<ace::VendorTraits>(idx);
}

int Kms_activate_session(int idx)
{
    return fhert::ActivateSession<ace::VendorTraits>(idx);
}

int Kms_num_sessions()
{
    return fhert::NumSessions<ace::VendorTraits>();
}

//=============================================================================
// Debug / dump utilities
//=============================================================================

void Dump_ciph(CIPHER ct, size_t start, size_t len)
{
    std::vector<double> vec;
    (void)ct;
    (void)start;
    (void)len;
    spdlog::debug("[Dump_ciph: not yet implemented in refactored ACE]");
}

void Dump_plain(PLAIN pt, size_t start, size_t len)
{
    std::vector<double> vec;
    FHertAceContext::Current().Decode(*pt, vec);
    size_t max = std::min(vec.size(), start + len);
    FILE* out = Ace_dump_file();
    for (size_t i = start; i < max; ++i) {
        fprintf(out, "%f ", vec[i]);
    }
    fprintf(out, "\n");
}

void Dump_cipher_msg(const char* name, CIPHER ct, uint32_t len)
{
    fprintf(Ace_dump_file(), "[%s]: ", name);
    Dump_ciph(ct, 16, len);
}

void Dump_plain_msg(const char* name, PLAIN pt, uint32_t len)
{
    fprintf(Ace_dump_file(), "[%s]: ", name);
    Dump_plain(pt, 16, len);
}

double* Get_msg(CIPHER ct)
{
    (void)ct;
    return nullptr;
}

double* Get_msg_from_plain(PLAIN pt)
{
    std::vector<double> vec;
    FHertAceContext::Current().Decode(*pt, vec);
    double* msg = static_cast<double*>(malloc(sizeof(double) * vec.size()));
    memcpy(msg, vec.data(), sizeof(double) * vec.size());
    return msg;
}

bool Within_value_range(CIPHER ciph, double* msg, uint32_t len)
{
    (void)ciph;
    (void)msg;
    (void)len;
    return true;
}
