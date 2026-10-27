//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief data_file.cpp
//! DataFileReader / DataFileWriter implementation.

#include "fhe/io/data_file.h"

#include "fhe/util/logging.h"

#include <cstring>
#include <ctime>
#include <spdlog/spdlog.h>
#include <vector>

namespace fhert
{

//=============================================================================
// DataFileReader
//=============================================================================

bool DataFileReader::open(const std::string& path)
{
    _ifs.open(path, std::ios::binary);
    if (!_ifs)
        return false;

    if (!_ifs.read(reinterpret_cast<char*>(&_hdr), sizeof(_hdr)))
        return false;

    if (std::memcmp(_hdr._magic, DATA_FILE_MAGIC, sizeof(_hdr._magic)) != 0) {
        spdlog::error("[io] Invalid magic in data file: {}", path);
        return false;
    }

    if (_hdr._ent_type != fhe::core::DE_MSG_F32 && _hdr._ent_type != fhe::core::DE_PLAINTEXT) {
        spdlog::error("[io] Unknown entry type {} in {}", _hdr._ent_type, path);
        return false;
    }

    _lut.resize(_hdr._ent_count);
    if (!_ifs.seekg(_hdr._lut_ofst))
        return false;
    if (!_ifs.read(reinterpret_cast<char*>(_lut.data()), _hdr._ent_count * sizeof(fhe::core::DATA_LUT_ENTRY)))
        return false;

    return true;
}

bool DataFileReader::read_entry(const fhe::core::DATA_LUT_ENTRY& entry, char* buf)
{
    if (!_ifs.seekg(entry._ent_ofst))
        return false;
    if (!_ifs.read(buf, entry._size))
        return false;
    return true;
}

void DataFileReader::close()
{
    if (_ifs.is_open()) {
        _ifs.close();
    }
    _lut.clear();
}

//=============================================================================
// DataFileWriter
//=============================================================================

bool DataFileWriter::open(const std::string& path, const char* model, const char* uuid, fhe::core::DATA_ENTRY_TYPE type,
                          uint8_t align)
{
    std::memset(&_hdr, 0, sizeof(_hdr));
    std::memcpy(_hdr._magic, DATA_FILE_MAGIC, sizeof(_hdr._magic));
    _hdr._ent_type = static_cast<uint8_t>(type);
    _hdr._ent_align = align;
    if (model) {
        std::strncpy(_hdr._model, model, sizeof(_hdr._model));
    }
    if (uuid) {
        std::strncpy(_hdr._uuid, uuid, sizeof(_hdr._uuid));
    }

    _ofs.open(path, std::ios::binary);
    if (!_ofs)
        return false;

    // Skip the 4K header page — we write it in close()
    _ofs.seekp(DATA_FILE_PAGE_SIZE);
    return true;
}

void DataFileWriter::close()
{
    if (!_ofs.is_open())
        return;

    // Write LUT at current position
    _hdr._ent_count = static_cast<uint64_t>(_lut.size());
    _hdr._lut_ofst = static_cast<uint64_t>(_ofs.tellp());
    if (!_lut.empty()) {
        _ofs.write(reinterpret_cast<const char*>(_lut.data()), _lut.size() * sizeof(fhe::core::DATA_LUT_ENTRY));
    }

    // Write header at beginning
    _hdr._rt_ver = 0;  // TODO: use actual RT version
    _hdr._flag = 0;
    timespec_get(&_hdr._ctime, TIME_UTC);
    _ofs.seekp(0);
    _ofs.write(reinterpret_cast<const char*>(&_hdr), sizeof(_hdr));
    _ofs.flush();
    _ofs.close();
}

uint64_t DataFileWriter::append_entry(const char* name, const char* data, uint32_t size, int scale, int level)
{
    uint64_t idx = _lut.size();
    uint64_t ofst = static_cast<uint64_t>(_ofs.tellp());

    // Align to 2^hdr._ent_align
    uint64_t alignment = (1ULL << _hdr._ent_align);
    uint64_t aligned_ofst = (ofst + alignment - 1) & ~(alignment - 1);
    if (aligned_ofst != ofst) {
        _ofs.seekp(aligned_ofst);
    }
    ofst = aligned_ofst;

    // Write data
    _ofs.write(data, size);

    // Pad to alignment
    uint32_t rem = size % static_cast<uint32_t>(alignment);
    if (rem > 0) {
        uint32_t padding = static_cast<uint32_t>(alignment) - rem;
        std::vector<char> zeros(padding, 0);
        _ofs.write(zeros.data(), padding);
    }

    // Create LUT entry
    _lut.emplace_back();
    fhe::core::DATA_LUT_ENTRY& entry = _lut.back();
    std::memset(&entry, 0, sizeof(entry));
    std::strncpy(entry._name, name, sizeof(entry._name) - 1);
    entry._index = static_cast<uint32_t>(idx);
    entry._size = size;
    entry._scale = scale;
    entry._level = level;
    entry._ent_ofst = ofst;

    return idx;
}

}  // namespace fhert
