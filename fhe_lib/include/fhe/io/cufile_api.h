//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief cufile_api.h
//! Runtime-loadable cuFile API — dlopen + dlsym function pointer table.
//!
//! This header provides ABI-compatible type definitions for cuFile symbols
//! and declares the CuFileApi function pointer table + GdsDriver RAII singleton.
//! No #include <cufile.h> is needed — all types are defined locally to match
//! the cuFile ABI, so compilation works even without the cuFile SDK installed.
//!
//! At runtime, GdsDriver::instance() dlopen("libcufile.so.1") and populates
//! the function pointers. If dlopen fails (cuFile not installed), all pointers
//! remain nullptr and GDS is silently unavailable.

#ifndef FHE_CUFILE_API_H
#define FHE_CUFILE_API_H

#include <cstddef>
#include <cstdint>
#include <sys/types.h>  // ssize_t, off_t

namespace rtio
{

//=============================================================================
// cuFile ABI-compatible type definitions
//=============================================================================
// These match the NVIDIA cuFile SDK headers (cufile.h) exactly.
// Defined locally so we never need #include <cufile.h> at compile time.

//! Opaque handle for a cuFile file descriptor.
using CUfileHandle_t = void*;

//! cuFile error codes — matches CUfileError_t ABI.
enum class CUfileError_t : int
{
    SUCCESS = 0,
    NOT_INITIALIZED = 1,
    INVALID_VALUE = 2,
    DRIVER_NOT_LOADED = 3,
    DRIVER_VERSION_MISMATCH = 4,
    IO_ERROR = 5,
    NOT_SUPPORTED = 6,
    DEVICE_NOT_FOUND = 7,
    PERMISSION_DENIED = 8,
    OUT_OF_RESOURCES = 9,
};

//! File descriptor type for cuFile handle registration.
//! Matches CU_FILE_HANDLE_TYPE_OPAQUE_FD = 1.
enum CUfileHandleType : int
{
    CU_FILE_HANDLE_TYPE_OPAQUE_FD = 1,
};

//! File descriptor structure for cuFile handle registration.
//! Matches CUfileDescr_t ABI.
struct CUfileDescr_t
{
    int type;  //!< CUfileHandleType
    union
    {
        int fd;        //!< POSIX file descriptor
        void* handle;  //!< Reserved
    } handle;
    int reserved[4];  //!< Reserved for ABI stability
};

//=============================================================================
// CuFileApi — function pointer table populated by dlopen/dlsym
//=============================================================================

//! Function pointer table for cuFile API symbols.
//! Populated by probe_cufile() via dlopen("libcufile.so.1") + dlsym.
//! If cuFile is not available, all pointers remain nullptr and loaded=false.
struct CuFileApi
{
    bool loaded = false;  //!< Whether libcufile.so.1 was successfully loaded

    //--- Driver lifecycle ---------------------------------------------------

    //! cuFileDriverOpen — initialize the cuFile driver.
    CUfileError_t (*DriverOpen)() = nullptr;

    //! cuFileDriverClose — shut down the cuFile driver.
    CUfileError_t (*DriverClose)() = nullptr;

    //--- Handle management --------------------------------------------------

    //! cuFileHandleRegister — register a file with cuFile.
    CUfileError_t (*HandleRegister)(CUfileHandle_t*, CUfileDescr_t*) = nullptr;

    //! cuFileHandleDeregister — deregister a file from cuFile.
    void (*HandleDeregister)(CUfileHandle_t) = nullptr;

    //--- Buffer registration ------------------------------------------------

    //! cuFileBufRegister — register a GPU buffer for cuFile I/O.
    CUfileError_t (*BufRegister)(void*, size_t, int) = nullptr;

    //! cuFileBufDeregister — deregister a GPU buffer.
    CUfileError_t (*BufDeregister)(void*) = nullptr;

    //--- I/O ----------------------------------------------------------------

    //! cuFileRead — read from file directly into GPU memory.
    //! @param fh     Registered file handle
    //! @param buf    GPU buffer pointer (must be cudaMalloc'd + registered)
    //! @param size   Number of bytes to read
    //! @param offset File offset (bytes)
    //! @param dev_offset Offset into GPU buffer (bytes, usually 0)
    //! @return Bytes read on success, negative CUfileError_t on failure
    ssize_t (*Read)(CUfileHandle_t fh, void* buf, size_t size, off_t offset, off_t dev_offset) = nullptr;

    //! cuFileWrite — write from GPU memory directly to file.
    //! @param fh     Registered file handle
    //! @param buf    GPU buffer pointer (must be cudaMalloc'd + registered)
    //! @param size   Number of bytes to write
    //! @param offset File offset (bytes)
    //! @param dev_offset Offset into GPU buffer (bytes, usually 0)
    //! @return Bytes written on success, negative CUfileError_t on failure
    ssize_t (*Write)(CUfileHandle_t fh, const void* buf, size_t size, off_t offset, off_t dev_offset) = nullptr;
};

//=============================================================================
// probe_cufile — dlopen + dlsym, thread-safe, call_once
//=============================================================================

//! Probe for libcufile.so.1 at runtime.
//! Thread-safe (uses std::call_once). Returns a reference to a static
//! CuFileApi instance. If dlopen or any dlsym fails, loaded=false and all
//! function pointers are nullptr.
const CuFileApi& probe_cufile();

//=============================================================================
// GdsDriver — RAII singleton managing cuFile driver lifecycle
//=============================================================================

//! RAII singleton for cuFile driver initialization.
//!
//! Usage:
//!   if (GdsDriver::instance().available()) {
//!       // cuFile is usable
//!       auto &api = GdsDriver::instance().api();
//!       api->Read(fh, gpu_ptr, size, file_offset, 0);
//!   }
//!
//! On first access, calls probe_cufile() and, if loaded, cuFileDriverOpen().
//! On process exit (static destructor), calls cuFileDriverClose() + dlclose().
//! Thread-safe initialization via std::call_once.
class GdsDriver
{
public:
    //! Get the singleton instance.
    static GdsDriver& instance();

    //! Whether cuFile is available and the driver was initialized.
    bool available() const { return _driver_initialized; }

    //! Access the cuFile API function pointers.
    //! Only valid when available() == true.
    const CuFileApi& api() const { return _api; }

    // Non-copyable, non-movable
    GdsDriver(const GdsDriver&) = delete;
    GdsDriver& operator=(const GdsDriver&) = delete;

private:
    GdsDriver();
    ~GdsDriver();

    CuFileApi _api;
    bool _driver_initialized = false;

    // dlopen handle for cleanup
    void* _dl_handle = nullptr;
};

}  // namespace rtio

#endif  // FHE_CUFILE_API_H
