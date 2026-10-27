//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief io_policy.cpp
//! IoPolicy factory implementations and global policy management.
//!
//! IoPolicy is a pure transport-level selection (auto/gds/staged/mapped).
//! Business-layer presets (IoProfile in io_profile.h) combine IoPolicy + IoConfig
//! for common scenarios. The resolve step here translates IoPolicy + IoLoc + global
//! overrides into a concrete IoConfig for create_io_transport().

#include "fhe/io/io_policy.h"

// Internal IO implementation headers — not visible to business layers
#include "fhe/io/cufile_api.h"
#include "fhe/io/gds_io.h"
#include "fhe/io/gds_transport.h"
#include "fhe/io/mmap_transport.h"
#include "fhe/io/staged_transport.h"

#include <stdexcept>

namespace rtio
{

//=============================================================================
// Policy + location → IoConfig resolution (internal)
//=============================================================================

//! Resolve IoPolicy + IoLoc into an IoConfig for create_io_transport().
//!
//! Transport selection is now driven entirely by IoPolicy:
//!   - auto:   GDS if cuFile available, else staged
//!   - gds:    cuFile DMA (error if cuFile unavailable)
//!   - staged: fread + cudaMemcpy (always available)
//!   - mapped: mmap + cudaMemcpy (always available for Device)
//!
//! IoConfig tunes parameters (chunk_size, threads, etc.) but does NOT
//! select the transport path — that's IoPolicy's job.
//!
//! Global IoConfig overrides (set via set_global_io_config) take precedence
//! for num_io_threads, chunk_size, and disk_paths. The IoPolicy value
//! (transport path) is NOT overridden by IoConfig — it is the caller's
//! explicit choice.
static IoConfig resolve_io_config(IoPolicy policy, IoLoc loc)
{
    IoConfig cfg;

    // Default IoConfig values are sensible for all policies:
    //   chunk_size = 8 MiB (optimal for GDS DMA throughput)
    //   num_io_threads = 4
    //   fallback_on_error = true
    //   galois_merged_format = true
    // No per-policy switch needed — IoPolicy only selects transport path.

    // Host location: GDS is meaningless for CPU data, always use staged
    if (loc == IoLoc::Host) {
        // Force staged for host data — GDS is a GPU-only path
        // This is enforced in create_io_transport() too, but we set it
        // here for clarity in the resolved config.
    }

    // Merge with global IoConfig for expert overrides.
    // If the user explicitly set non-default values via IoConfig.apply(),
    // those take precedence over the defaults.
    IoConfig global_cfg = get_global_io_config();
    if (global_cfg.num_io_threads > 0) {
        cfg.num_io_threads = global_cfg.num_io_threads;
    }
    if (!global_cfg.disk_paths.empty()) {
        cfg.disk_paths = global_cfg.disk_paths;
    }
    if (global_cfg.chunk_size != cfg.chunk_size) {
        cfg.chunk_size = global_cfg.chunk_size;
    }
    if (global_cfg.fallback_on_error != cfg.fallback_on_error) {
        cfg.fallback_on_error = global_cfg.fallback_on_error;
    }
    if (global_cfg.galois_merged_format != cfg.galois_merged_format) {
        cfg.galois_merged_format = global_cfg.galois_merged_format;
    }

    return cfg;
}

//=============================================================================
// Business-layer factory functions
//=============================================================================

std::unique_ptr<IoTransport> open_reader(const std::string& path, IoLoc loc, IoPolicy policy)
{
    IoConfig cfg = resolve_io_config(policy, loc);
    return create_io_transport(path, "rb", loc, policy, cfg);
}

std::unique_ptr<IoTransport> open_writer(const std::string& path, IoLoc loc, IoPolicy policy)
{
    IoConfig cfg = resolve_io_config(policy, loc);
    return create_io_transport(path, "wb", loc, policy, cfg);
}

//=============================================================================
// Global policy configuration
//=============================================================================

namespace
{

IoConfig& global_io_config_instance()
{
    static IoConfig instance;
    return instance;
}

IoPolicy& global_io_policy_instance()
{
    static IoPolicy instance = IoPolicy::auto_;
    return instance;
}

}  // anonymous namespace

void set_global_io_policy(IoPolicy policy)
{
    global_io_policy_instance() = policy;
}

IoPolicy get_global_io_policy()
{
    return global_io_policy_instance();
}

}  // namespace rtio
