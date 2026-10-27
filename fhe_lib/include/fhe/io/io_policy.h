//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief io_policy.h
//! Top-level IO policy — the ONLY IO header that business layers should include.
//!
//! Business layers (KeySerializer, KeyMgr, ACE/Phantom runtimes) include
//! this single header to get everything they need:
//!   - IoPolicy:   transport mode (auto, gds, staged, mapped)
//!   - IoLoc:      where is my data? (Device or Host)
//!   - IoExecMode: sync/async/pipeline execution mode
//!   - IoTransport: unified read/write + buffer registration capability
//!   - Factory functions: open_reader, open_writer
//!
//! IoPolicy describes HOW data is moved (the transport path).
//! IoConfig (in transport.h) tunes transport parameters (threads, chunk_size, etc.).
//! Business-layer presets (IoProfile in io_profile.h) combine IoPolicy + IoConfig
//! for common scenarios (galois key load, key persistence, checkpoint).
//!
//! What business layers do NOT see (intentionally hidden):
//!   - create_io_transport() with manual IoConfig
//!   - Any concrete transport class name
//!   - IoBulkReader (multi-file parallel read executor, for advanced use via io_bulk_reader.h)
//!
//! Usage:
//!   #include "fhe/io/io_policy.h"          // ONLY this
//!
//!   auto writer = rtio::open_writer(path,
//!       rtio::IoLoc::Device, rtio::IoPolicy::gds);
//!   writer->write_gpu(gpu_ptr, 0, size);
//!
//!   auto reader = rtio::open_reader(path,
//!       rtio::IoLoc::Device, rtio::IoPolicy::auto_);
//!   reader->register_buffer(gpu_ptr, size);
//!   reader->read_gpu(gpu_ptr, 0, size);

#ifndef FHE_IO_POLICY_H
#define FHE_IO_POLICY_H

#include <cstddef>
#include <memory>
#include <string>

namespace rtio
{

//=============================================================================
// IoLoc — data location hint (business-level concept)
//=============================================================================

//! Hint for where the bulk data resides — determines whether GDS is eligible.
//! This is a business-level decision: the Phantom backend uses Device,
//! the ACE backend uses Host. The IO implementation uses this hint
//! together with IoPolicy to select the concrete transport.
enum class IoLoc
{
    Device,  //!< Data in GPU memory (phantom backend) — GDS eligible
    Host,    //!< Data in host/CPU memory (antlib/seal/openfhe) — GDS never used
};

//=============================================================================
// Forward declarations — abstract interfaces, not exposed by value
//=============================================================================

class IoTransport;  // Defined in transport.h (implementation layer)

//=============================================================================
// IoPolicy — transport-level IO policy
//=============================================================================

//! Describes the IO transport path — how data is moved.
//!
//! This is a pure transport selection, independent of business logic.
//! Business-layer scenarios (galois key load, checkpoint, etc.) are
//! encoded in IoProfile (io_profile.h) which combines IoPolicy + IoConfig.
//!
//! | Policy  | Transport path                                       | GDS?  |
//! |---------|------------------------------------------------------|-------|
//! | auto    | GDS if cuFile available, else staged (default)      | auto  |
//! | gds     | cuFile DMA, fallback to staged on error             | yes   |
//! | staged  | Host staged: fread + cudaMemcpy (safest path)       | no    |
//! | mapped  | Host mapped: mmap + cudaMemcpy (one less copy)      | no    |
//!
//! IoLoc::Host always forces staged, regardless of IoPolicy — GDS is
//! meaningless for CPU data.
enum class IoPolicy
{
    auto_ = 0,   //!< Auto-detect: GDS if cuFile available, else staged
    gds = 1,     //!< GDS (cuFile DMA), fallback to staged on error
    staged = 2,  //!< Host staged: fread + cudaMemcpy (always available)
    mapped = 3,  //!< Host mapped: mmap + cudaMemcpy (one less copy)
};

//=============================================================================
// IoExecMode — execution mode for IO operations
//=============================================================================

//! Controls how IO operations are dispatched relative to computation.
//!
//! Currently only synchronous (blocking) mode is implemented.
//! Async and pipeline modes are reserved for future extensions where
//! IO and GPU computation can overlap.
//!
//! | Mode     | Description                                                  |
//! |----------|--------------------------------------------------------------|
//! | sync     | Blocking — call returns when IO completes (current default)  |
//! | async    | TODO: Non-blocking — returns future/stream for completion     |
//! | pipeline | TODO: Prefetch next batch while computing current batch       |
enum class IoExecMode
{
    sync = 0,  //!< Synchronous blocking IO (current default)
    // async    = 1,  //!< TODO: Non-blocking IO with completion notification
    // pipeline = 2,  //!< TODO: IO/compute pipeline with prefetch
};

//=============================================================================
// Business-layer factory functions
//=============================================================================

//! Open an IO reader (read mode) using IoPolicy.
//!
//! Returns a transport suitable for reading a single file.
//! For multi-file parallel reads, use IoBulkReader (io_bulk_reader.h).
//!
//! @param path    File path
//! @param loc     Data location (Device for GPU, Host for CPU)
//! @param policy  Transport path selection (default: auto)
//! @return IoTransport opened for reading
std::unique_ptr<IoTransport> open_reader(const std::string& path, IoLoc loc, IoPolicy policy = IoPolicy::auto_);

//! Open an IO writer (write mode) using IoPolicy.
//!
//! @param path    File path
//! @param loc     Data location (Device for GPU, Host for CPU)
//! @param policy  Transport path selection (default: auto)
//! @return IoTransport opened for writing
std::unique_ptr<IoTransport> open_writer(const std::string& path, IoLoc loc, IoPolicy policy = IoPolicy::auto_);

//=============================================================================
// Global policy configuration (optional expert override)
//=============================================================================

//! Set the global IO policy. Affects all future IO operations that use
//! auto or don't specify an explicit policy.
//! The default is IoPolicy::auto.
void set_global_io_policy(IoPolicy policy);

//! Get the current global IO policy.
IoPolicy get_global_io_policy();

}  // namespace rtio

#endif  // FHE_IO_POLICY_H
