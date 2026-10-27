//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief file_transport.cpp
//! Shared base class implementation for file-based I/O transports.

#include "fhe/io/file_transport.h"

#include <cerrno>
#include <cstring>
#include <mutex>
#include <stdexcept>

namespace rtio
{

//=============================================================================
// CRC32 table (shared across all FileTransport instances)
//=============================================================================

uint32_t FileTransport::crc32_table_[256] = {};
bool FileTransport::_crc32_table_init = false;

void FileTransport::init_crc32_table()
{
    // Double-checked locking — table contents are identical regardless
    // of which thread initializes first, so a rare race is benign.
    if (_crc32_table_init)
        return;
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t crc = i;
        for (int j = 0; j < 8; j++) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;
            else
                crc >>= 1;
        }
        crc32_table_[i] = crc;
    }
    _crc32_table_init = true;
}

//=============================================================================
// Constructor / Destructor
//=============================================================================

FileTransport::FileTransport(const std::string& path, const char* mode, size_t chunk_size)
    : _path(path), _chunk_size(chunk_size), _chunk(chunk_size)
{
    init_crc32_table();

    // Open file
    const char* fmode = nullptr;
    if (mode[0] == 'w' || mode[0] == 'W') {
        fmode = "w+b";  // write + read (need to read back for CRC)
    }
    else {
        fmode = "rb";  // read-only is sufficient for read path
    }

    _fd = std::fopen(path.c_str(), fmode);
    if (!_fd) {
        throw std::runtime_error(std::string("[FileTransport] Failed to open file: ") + path +
                                 " (errno=" + std::to_string(errno) + ")");
    }

    // Get file size
    if (std::fseek(_fd, 0, SEEK_END) == 0) {
        _file_size = static_cast<size_t>(std::ftell(_fd));
    }
}

FileTransport::~FileTransport()
{
    if (_fd) {
        std::fflush(_fd);
        std::fclose(_fd);
        _fd = nullptr;
    }
}

//=============================================================================
// Host data I/O
//=============================================================================

bool FileTransport::write(const void* data, size_t offset, size_t size)
{
    if (!_fd || size == 0)
        return size == 0;  // 0-byte write is OK

    if (std::fseek(_fd, static_cast<long>(offset), SEEK_SET) != 0) {
        return false;
    }

    size_t written = std::fwrite(data, 1, size, _fd);
    if (written != size)
        return false;

    std::fflush(_fd);

    // Update file size
    size_t end = offset + size;
    if (end > _file_size)
        _file_size = end;

    return true;
}

bool FileTransport::read(void* buf, size_t offset, size_t size)
{
    if (!_fd || size == 0)
        return size == 0;  // 0-byte read is OK

    if (std::fseek(_fd, static_cast<long>(offset), SEEK_SET) != 0) {
        return false;
    }

    size_t nread = std::fread(buf, 1, size, _fd);
    if (nread != size)
        return false;

    // Accumulate CRC if active
    if (_crc_active) {
        update_crc(buf, size);
    }

    return true;
}

//=============================================================================
// CRC32 accumulation
//=============================================================================

void FileTransport::reset_crc()
{
    _crc_active = true;
    _crc_state = 0xFFFFFFFF;
    _crc_finalized = false;
}

uint32_t FileTransport::crc32() const
{
    if (!_crc_active)
        return 0;
    // Finalize: XOR with 0xFFFFFFFF
    return _crc_state ^ 0xFFFFFFFF;
}

void FileTransport::update_crc(const void* data, size_t size)
{
    const uint8_t* bytes = static_cast<const uint8_t*>(data);
    uint32_t crc = _crc_state;
    for (size_t i = 0; i < size; i++) {
        crc = (crc >> 8) ^ crc32_table_[(crc ^ bytes[i]) & 0xFF];
    }
    _crc_state = crc;
}

}  // namespace rtio
