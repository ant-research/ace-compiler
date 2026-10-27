//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief cufile_api.cpp
//! Runtime cuFile loading via dlopen + dlsym.
//!
//! On first call to probe_cufile(), attempts to dlopen the cuFile shared
//! library, trying multiple sonames in order:
//!   1. libcufile.so.1  (CUDA 12.x+)
//!   2. libcufile.so.0  (CUDA 11.x, some CUDA 12.x distributions)
//!   3. libcufile.so    (unversioned, dev installs)
//!
//! If successful, resolves all cuFile function pointers via dlsym.
//! If the library is not installed or any required symbol is missing,
//! CuFileApi::loaded remains false and all pointers are nullptr.
//!
//! This module has zero link-time dependency on libcufile.so — it only
//! links against libdl (-ldl) for dlopen/dlsym.

#include "fhe/io/cufile_api.h"

#include "fhe/util/logging.h"

#include <cstdlib>
#include <cstring>
#include <dlfcn.h>
#include <mutex>
#include <spdlog/spdlog.h>

namespace rtio
{

//=============================================================================
// Internal: dlsym helper
//=============================================================================

//! Load a symbol from the shared library. Returns nullptr on failure.
//! On failure, prints a warning and sets *ok = false.
static void* load_symbol(void* handle, const char* name, bool* ok)
{
    // Clear any stale error
    dlerror();

    void* sym = dlsym(handle, name);
    const char* err = dlerror();
    if (err != nullptr) {
        spdlog::warn("[cufile] dlsym({}) failed: {}", name, err);
        *ok = false;
        return nullptr;
    }
    return sym;
}

//=============================================================================
// probe_cufile
//=============================================================================

const CuFileApi& probe_cufile()
{
    static CuFileApi api;
    static std::once_flag flag;

    std::call_once(flag, []() {
        // Try multiple sonames — different CUDA distributions use different
        // version suffixes. KvikIO uses the same approach.
        static const char* sonames[] = {
            "libcufile.so.1",  // CUDA 12.x+
            "libcufile.so.0",  // CUDA 11.x, some CUDA 12.x
            "libcufile.so",    // unversioned (dev installs)
        };

        void* handle = nullptr;
        const char* loaded_soname = nullptr;
        for (const char* name : sonames) {
            dlerror();  // clear stale error
            handle = dlopen(name, RTLD_LAZY | RTLD_LOCAL);
            if (handle) {
                loaded_soname = name;
                break;
            }
        }

        if (!handle) {
            spdlog::info("[cufile] cuFile not available (tried libcufile.so.1,0,). "
                         "Using host-staged I/O fallback.");
            return;  // api.loaded stays false
        }

        // Resolve all required symbols
        // cuFile exports C-linkage symbols with "cuFile" prefix,
        // e.g. cuFileDriverOpen, cuFileRead, etc.
        bool ok = true;

#define FHE_CUFILE_SYMBOL(name)                                                                                        \
    api.name = reinterpret_cast<decltype(api.name)>(load_symbol(handle, "cuFile" #name, &ok));

        FHE_CUFILE_SYMBOL(DriverOpen)
        FHE_CUFILE_SYMBOL(DriverClose)
        FHE_CUFILE_SYMBOL(HandleRegister)
        FHE_CUFILE_SYMBOL(HandleDeregister)
        FHE_CUFILE_SYMBOL(BufRegister)
        FHE_CUFILE_SYMBOL(BufDeregister)
        FHE_CUFILE_SYMBOL(Read)
        FHE_CUFILE_SYMBOL(Write)

#undef FHE_CUFILE_SYMBOL

        if (!ok) {
            spdlog::warn("[cufile] Failed to resolve some cuFile symbols. "
                         "Using host-staged I/O fallback.");
            dlclose(handle);
            return;
        }

        api.loaded = true;
        spdlog::info("[cufile] cuFile library loaded successfully (dlopen: {})", loaded_soname);

        // NOTE: We do NOT dlclose here — the handle is kept alive for the
        // lifetime of the process. GdsDriver will manage driver init/fini.
        // The dlopen handle is stored in GdsDriver for cleanup on exit.
    });

    return api;
}

//=============================================================================
// GdsDriver singleton
//=============================================================================

GdsDriver& GdsDriver::instance()
{
    static GdsDriver driver;
    return driver;
}

GdsDriver::GdsDriver()
{
    _api = probe_cufile();

    if (!_api.loaded) {
        _driver_initialized = false;
        return;
    }

    // Initialize the cuFile driver
    CUfileError_t err = _api.DriverOpen();
    if (err == CUfileError_t::SUCCESS) {
        _driver_initialized = true;
        spdlog::info("[cufile] cuFile driver initialized successfully");
    }
    else {
        _driver_initialized = false;
        spdlog::warn("[cufile] cuFileDriverOpen failed (err={}). "
                     "Using host-staged I/O fallback.",
                     static_cast<int>(err));
    }
}

GdsDriver::~GdsDriver()
{
    if (_driver_initialized && _api.DriverClose) {
        CUfileError_t err = _api.DriverClose();
        if (err != CUfileError_t::SUCCESS) {
            spdlog::warn("[cufile] cuFileDriverClose failed (err={})", static_cast<int>(err));
        }
    }

    // Note: we don't dlclose the shared library handle here.
    // The cuFile library may have internal threads and at-exit handlers
    // that assume the library is still mapped. Let the OS clean up.
}

}  // namespace rtio
