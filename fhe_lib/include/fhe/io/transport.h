//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief transport.h
//! Abstract I/O transport interface for FHE key/data serialization.
//!
//! IoTransport provides a unified interface for reading/writing files with
//! both host and GPU data. Concrete implementations choose the I/O strategy:
//! - StagedTransport: Host Staged — fread + cudaMemcpy, always available
//! - GdsTransport: GDS (Direct) — cuFile DMA with automatic fallback
//!
//! Transport selection is handled by the create_io_transport() factory, which
//! considers IoPolicy (transport path), IoConfig (tuning parameters),
//! IoLoc (data location), and cuFile driver availability.
//!
//! NOTE: Business layers should include io_policy.h instead of this header.
//! This header exposes IoConfig which is an expert-level tuning interface.

#ifndef FHE_IO_TRANSPORT_H
#define FHE_IO_TRANSPORT_H

#include "fhe/io/io_policy.h"  // IoLoc, IoPolicy, IoExecMode

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace rtio
{

//=============================================================================
// IoConfig — I/O transport tuning parameters
//=============================================================================

//! Configuration for I/O transport behavior.
//!
//! IoPolicy (in io_policy.h) selects the transport path (auto/gds/staged/mapped).
//! IoConfig tunes the parameters for that transport (chunk size, threads, etc.).
//! Business-layer presets (IoProfile in io_profile.h) combine both.
//!
//! NOTE: IoConfig no longer contains a data_path field. Transport selection
//! is controlled by IoPolicy, not IoConfig. This separation ensures that
//! business code specifies transport intent via IoPolicy, while expert code
//! can tune parameters via IoConfig.
struct IoConfig
{

    //! If GDS fails and this is true, silently fall back to StagedTransport.
    //! If false, a GDS failure is propagated as an error.
    //! Only meaningful when IoPolicy is auto or gds.
    bool fallback_on_error = true;

    //! Chunk size for I/O operations (bytes).
    //! For GdsTransport: 8 MiB (8*1024*1024) is optimal for GDS DMA throughput.
    //! Smaller chunks (e.g. 512 KB) cause excessive cuFileRead dispatch overhead.
    //! Larger chunks (>16 MB) exceed cuFile max_direct_io_size_kb and degrade.
    //! For StagedTransport: 8 MiB still works well and avoids OOM on large key files.
    size_t chunk_size = 8UL * 1024 * 1024;

    //! Number of I/O threads for parallel GDS reads.
    //! Default 4 — reaches ~80% of single-disk peak throughput.
    //! 1 = single-threaded (backward compatible, but 3x slower than 4T).
    //! For multi-disk, use 4 threads per disk for best results.
    //! Values > 1 enable multi-threaded cuFileRead with chunk_size-sized
    //! I/O requests issued concurrently via atomic chunk dispenser.
    int num_io_threads = 4;

    //! Multi-disk paths for per-key galois key distribution.
    //! When empty (default), all galois key files are written to / read from
    //! the key_dir/model/ directory (single-disk mode).
    //! When non-empty, key i is placed on disk_paths[i % disk_paths.size()],
    //! enabling multi-NVMe parallel bandwidth for per-key galois key I/O.
    //! The galois_key.index.json is always written to disk_paths[0]/model/.
    std::vector<std::string> disk_paths = {};

    //! Whether to save galois keys in merged format (one large file with
    //! offset-based indexing) instead of per-key format (many small files).
    //! Default true — merged format eliminates per-file cuFileHandle registration
    //! overhead, matching the gdsio benchmark pattern for maximum GDS throughput.
    //! The load path auto-detects the format from index.json version, so
    //! per-key keys saved by older versions are still readable.
    bool galois_merged_format = true;

    //! Execution mode for IO operations.
    //! Controls how IO is dispatched relative to computation.
    //! Currently only sync (blocking) mode is implemented.
    //! Reserved for future async/pipeline support.
    IoExecMode exec_mode = IoExecMode::sync;
};

//=============================================================================
// IoTransport — abstract I/O transport
//=============================================================================

//! Abstract interface for file I/O with both host and GPU data paths.
//!
//! Usage (RAII — no manual open/close):
//!   auto io = create_io_transport("key.bin", "wb", IoLoc::Device, IoPolicy::staged);
//!   io->write(header, 0, 64);
//!   io->write_gpu(gpu_ptr, 64, data_size);
//!   // destructor closes file
//!
//! Host data (headers, metadata) uses write()/read().
//! GPU data (key payloads) uses write_gpu()/read_gpu().
//!
//! CRC32 accumulation:
//!   StagedTransport automatically computes CRC32 during read operations
//!   (zero-cost — computed while chunking through host memory). Call
//!   reset_crc() before the data section, then crc32() after to validate.
//!   GdsTransport does not compute CRC (returns 0) since the hardware
//!   ECC chain protects the NVMe→PCIe→HBM path.
class IoTransport
{
public:
    virtual ~IoTransport();

    //-------------------------------------------------------------------------
    // Host data I/O (headers, metadata — always small)
    //-------------------------------------------------------------------------

    //! Write host data to file at the given offset.
    //! @param data   Source buffer (host memory)
    //! @param offset Byte offset in file
    //! @param size   Number of bytes to write
    //! @return true on success
    virtual bool write(const void* data, size_t offset, size_t size) = 0;

    //! Read host data from file at the given offset.
    //! @param buf    Destination buffer (host memory)
    //! @param offset Byte offset in file
    //! @param size   Number of bytes to read
    //! @return true on success
    virtual bool read(void* buf, size_t offset, size_t size) = 0;

    //-------------------------------------------------------------------------
    // GPU data I/O (key payload — potentially huge)
    //-------------------------------------------------------------------------

    //! Write GPU data to file at the given offset.
    //! @param gpu_ptr Source GPU memory pointer (cudaMalloc'd)
    //! @param offset  Byte offset in file
    //! @param size    Number of bytes to write
    //! @return true on success
    virtual bool write_gpu(const void* gpu_ptr, size_t offset, size_t size) = 0;

    //! Read from file directly into GPU memory.
    //! @param gpu_ptr Destination GPU memory pointer (cudaMalloc'd)
    //! @param offset  Byte offset in file
    //! @param size    Number of bytes to read
    //! @return true on success
    virtual bool read_gpu(void* gpu_ptr, size_t offset, size_t size) = 0;

    //! Read from file into GPU memory using a specific thread context.
    //! For GdsTransport, thread_id is ignored (multi-threading is handled
    //! internally via gds_parallel_read). For other transports, ignored.
    //! @param thread_id Per-thread resource index (0-based)
    //! @param gpu_ptr   Destination GPU memory pointer
    //! @param offset    Byte offset in file
    //! @param size      Number of bytes to read
    //! @return true on success
    virtual bool read_gpu(int thread_id, void* gpu_ptr, size_t offset, size_t size);

    //-------------------------------------------------------------------------
    // CRC32 accumulation (read path only)
    //-------------------------------------------------------------------------

    //! Reset the CRC32 accumulator.
    //! Call before reading the data section (after header) so that CRC
    //! covers exactly the data bytes.
    virtual void reset_crc() = 0;

    //! Get CRC32 of all data read since the last reset_crc().
    //! StagedTransport: returns the computed CRC (non-zero).
    //! GdsTransport: returns 0 (CRC not computed — hardware ECC protects path).
    virtual uint32_t crc32() const = 0;

    //-------------------------------------------------------------------------
    // Info
    //-------------------------------------------------------------------------

    //! Get the file path.
    virtual const std::string& path() const = 0;

    //! Get the current file size (valid after open for read; updated after write).
    virtual size_t file_size() const = 0;

    //! Whether cuFile/GDS was actually used for I/O.
    //! StagedTransport: always false.
    //! GdsTransport: true if cuFile was used, false if fallback occurred.
    virtual bool used_gds() const = 0;

    //! Human-readable transport name ("staged", "mapped", or "gds").
    virtual const char* transport_name() const = 0;

    //-------------------------------------------------------------------------
    // GPU buffer registration (meaningful for GDS; no-op for others)
    //-------------------------------------------------------------------------

    //! Register a GPU buffer for GDS I/O.
    //! Must be called after cudaMalloc and before read_gpu/write_gpu for
    //! GDS transport. For Staged/Mapped transport this is a no-op.
    //! @param gpu_ptr  GPU memory pointer (cudaMalloc'd)
    //! @param size     Buffer size in bytes
    //! @return true on success (always true for non-GDS transports)
    virtual bool register_buffer(void* gpu_ptr, size_t size);

    //! Deregister all previously registered GPU buffers.
    //! Should be called before cudaFree. No-op for non-GDS transports.
    virtual void deregister_all();
};

//=============================================================================
// Factory function
//=============================================================================

//! Create an I/O transport for the given file.
//!
//! Transport selection logic (in order of priority):
//!   1. IoPolicy::staged  → always StagedTransport
//!   2. IoPolicy::mapped  → MmapTransport (mmap + cudaMemcpy)
//!   3. IoPolicy::gds     → GdsTransport; throws if cuFile unavailable
//!   4. IoPolicy::auto    → GdsTransport if cuFile available,
//!                          StagedTransport otherwise
//!   5. IoLoc::Host       → always StagedTransport (GDS meaningless for CPU data)
//!
//! Additionally, the ACE_GDS_ENABLED env var provides a process-level override
//! (0 = force HostStaged, 1 = force Gds, unset = normal policy). This is mainly
//! useful for debugging or containerized environments where IoPolicy cannot be
//! set programmatically.
//!
//! Global IoConfig overrides (set via set_global_io_config) take precedence
//! for num_io_threads, chunk_size, and disk_paths. The IoPolicy value
//! (transport path) is NOT overridden by IoConfig — it is the caller's
//! explicit choice.
//!
//! The returned transport is open and ready for I/O. RAII: closes on destruction.
//!
//! @param path    File path
//! @param mode    "wb" for write, "rb" for read
//! @param policy  Transport path selection (auto/gds/staged/mapped)
//! @param loc     Data location hint (default: IoLoc::Device)
//! @param config  I/O tuning parameters (threads, chunk_size, etc.)
//! @return Open transport; throws std::runtime_error on Gds + cuFile unavailable
std::unique_ptr<IoTransport> create_io_transport(const std::string& path, const char* mode, IoLoc loc, IoPolicy policy,
                                                 const IoConfig& config = {});

//=============================================================================
// Global I/O configuration (process-wide singleton)
//=============================================================================

//! Set the global I/O configuration.
//! This is the configuration used by default when create_io_transport() is
//! called without an explicit IoConfig argument, and by KEY_MANAGER when
//! saving/loading keys.
void set_global_io_config(const IoConfig& config);

//! Get the current global I/O configuration.
IoConfig get_global_io_config();

}  // namespace rtio

#endif  // FHE_IO_TRANSPORT_H
