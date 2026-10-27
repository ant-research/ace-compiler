//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief transport.cpp
//! Factory function for I/O transport creation.
//!
//! Transport selection is driven by IoPolicy (the transport path selector).
//! IoConfig tunes parameters (chunk_size, threads, etc.) but does NOT
//! select the transport — that's IoPolicy's job.

#include "fhe/io/transport.h"

#include "fhe/io/cufile_api.h"
#include "fhe/io/gds_io.h"
#include "fhe/io/gds_transport.h"
#include "fhe/io/mmap_transport.h"
#include "fhe/io/staged_transport.h"

#include <stdexcept>

namespace rtio
{

//=============================================================================
// IoTransport — explicit destructor (ensures unique_ptr<IoTransport> works
// when only a forward declaration is visible, as in io_policy.h consumers)
//=============================================================================

IoTransport::~IoTransport() = default;

bool IoTransport::register_buffer(void* /*gpu_ptr*/, size_t /*size*/)
{
    return true;  // no-op default (Staged/Mapped)
}

void IoTransport::deregister_all()
{
    // no-op default (Staged/Mapped)
}

bool IoTransport::read_gpu(int /*thread_id*/, void* gpu_ptr, size_t offset, size_t size)
{
    // Default: ignore thread_id, delegate to single-threaded read_gpu
    return read_gpu(gpu_ptr, offset, size);
}

std::unique_ptr<IoTransport> create_io_transport(const std::string& path, const char* mode, IoLoc loc, IoPolicy policy,
                                                 const IoConfig& config)
{
    // IoPolicy::staged → always StagedTransport
    if (policy == IoPolicy::staged) {
        return std::make_unique<StagedTransport>(path, mode, config.chunk_size);
    }

    // IoPolicy::mapped → MmapTransport (mmap + cudaMemcpy)
    // Host Mapped uses mmap + cudaMemcpy, one less copy than Host Staged.
    if (policy == IoPolicy::mapped) {
        return std::make_unique<MmapTransport>(path, mode, config.chunk_size);
    }

    // IoLoc::Host → GDS is not applicable for CPU data
    if (loc == IoLoc::Host) {
        return std::make_unique<StagedTransport>(path, mode, config.chunk_size);
    }

    // From here: loc == Device, policy is gds or auto

    // Legacy ACE_GDS_ENABLED env var (overrides IoPolicy)
    int env = gds_env_enabled();  // 1=force-on, 0=force-disable, -1=not set
    if (env == 0) {
        // Env var force-disable: always StagedTransport
        return std::make_unique<StagedTransport>(path, mode, config.chunk_size);
    }

    bool force_gds = (policy == IoPolicy::gds) || (env == 1);

    // Check cuFile availability
    if (GdsDriver::instance().available()) {
        try {
            auto transport = std::make_unique<GdsTransport>(path, mode, config.fallback_on_error, config.chunk_size,
                                                            config.num_io_threads);
            return transport;
        }
        catch (const std::exception& e) {
            // GdsTransport construction failed (e.g., file open error with GDS)
            if (force_gds) {
                // Gds mode: re-throw (user explicitly asked for GDS)
                throw;
            }
            // Auto mode with fallback: silently fall back
            return std::make_unique<StagedTransport>(path, mode, config.chunk_size);
        }
    }

    // cuFile not available
    if (force_gds) {
        throw std::runtime_error("[IoTransport] GDS policy requested (IoPolicy::gds or ACE_GDS_ENABLED=1) "
                                 "but cuFile is not available. Install nvidia-gds or use IoPolicy::auto_.");
    }

    // Auto mode: fall back to StagedTransport
    return std::make_unique<StagedTransport>(path, mode, config.chunk_size);
}

//=============================================================================
// Global I/O configuration (process-wide singleton)
//=============================================================================

// Since FHErt_io is built as a SHARED library, there is exactly one copy of
// this translation unit in the process.  The dual-singleton problem (where
// a static library was copied into each downstream .so, creating separate
// IoConfig instances) is eliminated at the library level — no extern "C"
// or ELF vague linkage tricks are needed.

namespace
{

IoConfig& global_io_config_instance()
{
    static IoConfig instance;
    return instance;
}

}  // anonymous namespace

void set_global_io_config(const IoConfig& config)
{
    global_io_config_instance() = config;
}

IoConfig get_global_io_config()
{
    return global_io_config_instance();
}

}  // namespace rtio
