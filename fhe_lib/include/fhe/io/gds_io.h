//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief gds_io.h
//! GPU Direct Storage (GDS / cuFile) I/O internal declarations.
//!
//! This header declares the GDS driver check and environment variable
//! compatibility function used by the IO transport layer (GdsTransport).
//!
//! cuFile is loaded at runtime via dlopen (see cufile_api.h). If the
//! cuFile library is not installed, GDS operations silently fall back
//! to host-staged I/O.
//!
//! NOTE: This header is part of the IO implementation layer. Business
//! layers should include io_policy.h instead.

#ifndef FHE_GDS_IO_H
#define FHE_GDS_IO_H

#include "fhe/io/cufile_api.h"

#include <string>

namespace rtio
{

//=============================================================================
// Environment variable control (legacy compatibility)
//=============================================================================

//! Read the ACE_GDS_ENABLED environment variable.
//! @return  1 = force-enable, 0 = force-disable, -1 = not set (auto-detect)
int gds_env_enabled();

//! Check if the GDS (cuFile) driver is currently initialized and available.
//! Equivalent to GdsDriver::instance().available().
bool gds_driver_available();

//=============================================================================
// Internal: parallel cuFile read engine (used by GdsTransport)
//=============================================================================

//! Perform a parallel chunked cuFile read using pre-registered handles.
//!
//! Each handle (and its underlying fd) submits I/O to the NVMe independently.
//! An atomic chunk dispenser distributes work across handles, so N handles
//! approximate N× the submission queue depth.
//!
//! Single-handle path (num_handles==1) uses no thread overhead.
//! Multi-handle path spawns (num_handles-1) threads + runs worker 0 on current thread.
//!
//! @param api          cuFile API function pointers
//! @param handles      Array of registered cuFile handles (void* = CUfileHandle_t)
//! @param num_handles  Number of handles in the array
//! @param gpu_ptr      Destination GPU buffer (must be cudaMalloc'd + BufRegistered)
//! @param offset       Byte offset in the file to start reading
//! @param size         Number of bytes to read
//! @param chunk_size   Chunk size for each cuFileRead request (default 512 KB)
//! @return true if read succeeded, false on error
bool gds_parallel_read(const CuFileApi& api, void** handles, int num_handles, void* gpu_ptr, size_t offset, size_t size,
                       size_t chunk_size);

}  // namespace rtio

#endif  // FHE_GDS_IO_H
