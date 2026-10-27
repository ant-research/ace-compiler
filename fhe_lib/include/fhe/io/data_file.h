//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief data_file.h
//! Data file I/O for FHE runtime backends.
//!
//! Provides DataFileReader and DataFileWriter for reading and writing
//! data files that follow the binary format defined in
//! fhe/core/rt_data_def.h (DATA_FILE_HDR + entries + DATA_LUT_ENTRY table).
//!
//! These classes are the C++ replacement for both the older C RT_DATA_FILE
//! API (compiler/rtlib) and the PhantomDataFileReader/Writer. They use
//! direct file I/O (ifstream/ofstream) and have no dependency on compiler
//! infrastructure headers.

#ifndef FHERT_DATA_FILE_IO_H
#define FHERT_DATA_FILE_IO_H

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

// DATA_FILE_HDR, DATA_LUT_ENTRY, DATA_ENTRY_TYPE, etc.
#include "fhe/core/rt_data_def.h"

namespace fhert
{

//=============================================================================
// DataFileReader — reads DE_MSG_F32 / DE_PLAINTEXT data files
//=============================================================================

class DataFileReader
{
public:
    DataFileReader() = default;

    //! Open and parse a data file.
    //! Reads the header and LUT. Returns true on success.
    bool open(const std::string& path);

    //! Read a single entry's raw data into buf.
    //! buf must be at least entry._size bytes.
    bool read_entry(const fhe::core::DATA_LUT_ENTRY& entry, char* buf);

    //! Close the file (called automatically by destructor).
    void close();

    ~DataFileReader() { close(); }

    // -- Accessors --

    const fhe::core::DATA_FILE_HDR& header() const { return _hdr; }
    const std::vector<fhe::core::DATA_LUT_ENTRY>& lut() const { return _lut; }

private:
    fhe::core::DATA_FILE_HDR _hdr{};
    std::vector<fhe::core::DATA_LUT_ENTRY> _lut;
    std::ifstream _ifs;
};

//=============================================================================
// DataFileWriter — writes DE_PLAINTEXT data files
//=============================================================================

class DataFileWriter
{
public:
    DataFileWriter() = default;

    //! Create a new data file for writing.
    //! @param path    Output file path
    //! @param model   Model name (copied into header, may be nullptr)
    //! @param uuid    UUID string (copied into header, may be nullptr)
    //! @param type    Entry type (default: DE_PLAINTEXT)
    //! @param align   Alignment exponent, 2^align bytes (default: 12 = 4096)
    bool open(const std::string& path, const char* model = nullptr, const char* uuid = nullptr,
              fhe::core::DATA_ENTRY_TYPE type = fhe::core::DE_PLAINTEXT, uint8_t align = 12);

    //! Finalize the file: write LUT, patch header, close.
    void close();

    ~DataFileWriter() { close(); }

    // Non-copyable
    DataFileWriter(const DataFileWriter&) = delete;
    DataFileWriter& operator=(const DataFileWriter&) = delete;

    //! Append a serialized data entry.
    //! @param name   Entry name (up to 15 chars)
    //! @param data   Pointer to the entry's serialized data
    //! @param size   Size of data in bytes
    //! @param scale  Encoding scale
    //! @param level  Encoding level
    //! @return Index of the new entry in the LUT
    uint64_t append_entry(const char* name, const char* data, uint32_t size, int scale, int level);

    //! Return the number of entries appended so far.
    uint64_t entry_count() const { return _lut.size(); }

private:
    fhe::core::DATA_FILE_HDR _hdr{};
    std::vector<fhe::core::DATA_LUT_ENTRY> _lut;
    std::ofstream _ofs;
};

}  // namespace fhert

#endif  // FHERT_DATA_FILE_IO_H
