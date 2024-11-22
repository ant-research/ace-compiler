//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CORE_RT_DATA_WRITER_H
#define FHE_CORE_RT_DATA_WRITER_H

#include <iostream>
#include <vector>

#include "air/core/ir2c_ctx.h"
#include "fhe/core/lower_ctx.h"
#include "fhe/core/rt_data_def.h"
#include "fhe/core/rt_version.h"
#include "fhe/poly/poly2c_config.h"

namespace fhe {

namespace core {

// makesure DATA_FILE_HDR is less than 4K
AIR_STATIC_ASSERT(sizeof(DATA_FILE_HDR) < DATA_FILE_PAGE_SIZE);

//! @brief Binary data file to contain weight data in model
class RT_DATA_WRITER {
public:
  RT_DATA_WRITER(const char* fname, DATA_ENTRY_TYPE type, const char* model,
                 const char* uuid) {
    AIR_ASSERT(model != nullptr);
    AIR_ASSERT(uuid != nullptr);
    _hdr._ent_type  = type;
    _hdr._ent_align = (type == DE_PLAINTEXT) ? 12 : 5;
    strncpy(_hdr._model, model, sizeof(_hdr._model));
    strncpy(_hdr._uuid, uuid, sizeof(_hdr._uuid));
    _os.open(fname, std::ios::binary);
    if (_os) {
      _os.seekp(DATA_FILE_PAGE_SIZE);
    }
  }

  ~RT_DATA_WRITER() {
    memcpy(_hdr._magic, DATA_FILE_MAGIC, sizeof(_hdr._magic));
    _hdr._rt_ver    = RT_VERSION_FULL;
    _hdr._flag      = 0;
    _hdr._ent_count = _lut.size();
    _hdr._lut_ofst  = _os.tellp();
    timespec_get(&_hdr._ctime, TIME_UTC);
    if (_os) {
      _os.write((char*)_lut.data(), _lut.size() * sizeof(DATA_LUT_ENTRY));
      _os.seekp(0);
      _os.write((char*)&_hdr, sizeof(_hdr));
      _os.flush();
      _os.close();
    }
  }

  template <typename T>
  uint64_t Append(const char* name, const T* data, uint32_t count) {
    AIR_ASSERT((sizeof(T) == 4 && _hdr._ent_type == DE_MSG_F32) ||
               (sizeof(T) == 8 && _hdr._ent_type == DE_MSG_F64));
    AIR_ASSERT(count > 0);
    uint32_t size = count * sizeof(T);
    return Write_data(name, (const char*)data, size);
  }

  uint64_t Append_pt(const char* name, const char* data, uint32_t size) {
    AIR_ASSERT(_hdr._ent_type == DE_PLAINTEXT);
    AIR_ASSERT(size > 0);
    return Write_data(name, (const char*)data, size);
  }

  uint64_t Cur_idx() const { return _lut.size(); }

private:
  uint64_t Write_data(const char* name, const char* data, uint32_t size) {
    uint64_t idx  = _lut.size();
    uint64_t ofst = _os.tellp();
    AIR_ASSERT((ofst % (1 << _hdr._ent_align)) == 0);
    _os.write(data, size);
    uint32_t rem = size % (1 << _hdr._ent_align);
    if (rem > 0) {
      uint32_t padding = (1 << _hdr._ent_align) - rem;
      _os.seekp(padding, std::ios_base::cur);
    }
    _lut.emplace_back(name, idx, size, ofst);
    return (!_os) ? (uint64_t)(-1) : idx;
  }

  struct CXX_DATA_LUT_ENTRY : public DATA_LUT_ENTRY {
  public:
    CXX_DATA_LUT_ENTRY(const char* name, uint32_t index, uint32_t size,
                       uint64_t ofst) {
      AIR_ASSERT(strlen(name) < sizeof(_name));
      strncpy(_name, name, sizeof(_name));
      _index    = index;
      _size     = size;
      _ent_ofst = ofst;
    }
  };

  std::ofstream                   _os;
  DATA_FILE_HDR                   _hdr;
  std::vector<CXX_DATA_LUT_ENTRY> _lut;

};  // RT_DATA_WRITER

}  // namespace core

}  // namespace fhe

#endif  // FHE_CORE_RT_DATA_WRITER_H
