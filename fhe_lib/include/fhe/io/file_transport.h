//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief file_transport.h
//! Base class for file-based I/O transports with shared CRC32 and host I/O.
//!
//! StagedTransport and MmapTransport share the same host read/write
//! implementation (fseek + fread/fwrite) and the same CRC32 accumulation
//! logic. This base class eliminates the duplication.

#ifndef FHE_FILE_TRANSPORT_H
#define FHE_FILE_TRANSPORT_H

#include "fhe/io/transport.h"

#include <cstdio>
#include <string>
#include <vector>

namespace rtio
{

//=============================================================================
// FileTransport — shared base for StagedTransport and MmapTransport
//=============================================================================

//! Base class providing shared file I/O and CRC32 accumulation for
//! transports that use POSIX FILE* for host data.
//!
//! Subclasses must implement write_gpu() and read_gpu() with their
//! respective GPU transfer strategies (fread+cudaMemcpy vs mmap+cudaMemcpy).
class FileTransport : public IoTransport
{
public:
    //! Close the file and release resources.
    ~FileTransport() override;

    // Non-copyable, non-movable
    FileTransport(const FileTransport&) = delete;
    FileTransport& operator=(const FileTransport&) = delete;

    //-------------------------------------------------------------------------
    // Host data I/O (shared implementation)
    //-------------------------------------------------------------------------

    //! Write host data to file at the given offset using fseek + fwrite.
    bool write(const void* data, size_t offset, size_t size) override;

    //! Read host data from file at the given offset using fseek + fread.
    //! Accumulates CRC if active.
    bool read(void* buf, size_t offset, size_t size) override;

    //-------------------------------------------------------------------------
    // CRC32 accumulation
    //-------------------------------------------------------------------------

    void reset_crc() override;
    uint32_t crc32() const override;

    //-------------------------------------------------------------------------
    // Info
    //-------------------------------------------------------------------------

    const std::string& path() const override { return _path; }
    size_t file_size() const override { return _file_size; }

protected:
    //! Open a file. Subclasses call this from their constructors.
    //! @param path       File path
    //! @param mode       "wb" (write) or "rb" (read)
    //! @param chunk_size Host buffer size for GPU streaming
    //! @throws std::runtime_error if the file cannot be opened
    FileTransport(const std::string& path, const char* mode, size_t chunk_size);

    //! Update CRC32 accumulator with the given host data.
    void update_crc(const void* data, size_t size);

    std::string _path;
    FILE* _fd = nullptr;          //!< File handle
    size_t _file_size = 0;        //!< Current file size
    size_t _chunk_size;           //!< Chunk buffer size
    std::vector<uint8_t> _chunk;  //!< Host buffer for GPU streaming I/O

    // CRC32 accumulation (read path)
    bool _crc_active = false;    //!< Whether CRC accumulation is active
    uint32_t _crc_state = 0;     //!< Running CRC state (unfinalized)
    bool _crc_finalized = true;  //!< Whether crc32() was called since reset

    // CRC32 table (shared across all instances, thread-safe via call_once)
    static uint32_t crc32_table_[256];
    static bool _crc32_table_init;
    static void init_crc32_table();
};

}  // namespace rtio

#endif  // FHE_FILE_TRANSPORT_H
