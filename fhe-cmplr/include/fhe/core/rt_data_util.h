//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CORE_RT_DATA_UTIL_H
#define FHE_CORE_RT_DATA_UTIL_H

#include <iostream>
#include <vector>

#include "air/core/ir2c_ctx.h"
#include "fhe/core/lower_ctx.h"
#include "fhe/core/rt_data_def.h"
#include "fhe/core/rt_version.h"
#include "fhe/poly/poly2c_config.h"

namespace fhe {

namespace core {

//! @brief Binary data file reader
class RT_DATA_READER {
public:
  RT_DATA_READER(std::istream& is)
      : _is(is), _ent_count(0), _lut_ofst(0), _ent_align(0) {}

  bool Read_hdr(DATA_FILE_HDR& hdr) {
    AIR_ASSERT(_is.tellg() == 0);
    if (!_is.read((char*)&hdr, sizeof(hdr))) {
      AIR_ASSERT_MSG(false, "Failed to read header.\n");
      return false;
    }
    if (memcmp(hdr._magic, DATA_FILE_MAGIC, sizeof(hdr._magic)) != 0) {
      AIR_ASSERT_MSG(false, "Not a correct rt data file.\n");
      return false;
    }
    if (hdr._rt_ver != RT_VERSION_FULL) {
      AIR_ASSERT_MSG(false, "RT version mismatch.\n");
      return false;
    }
    _ent_align = (1 << hdr._ent_align);
    if ((hdr._lut_ofst % _ent_align) != 0) {
      AIR_ASSERT_MSG(false, "LUT not aligned.\n");
      return false;
    }
    _ent_count = hdr._ent_count;
    _lut_ofst  = hdr._lut_ofst;
    return true;
  }

  bool Read_lut(std::vector<DATA_LUT_ENTRY>& lut) {
    if (!_is.seekg(_lut_ofst)) {
      AIR_ASSERT_MSG(false, "Failed to seek lut table.\n");
      return false;
    }
    lut.resize(_ent_count);
    if (!_is.read((char*)lut.data(), _ent_count * sizeof(DATA_LUT_ENTRY))) {
      AIR_ASSERT_MSG(false, "Failed to read lookup table.\n");
      return false;
    }
    AIR_ASSERT_MSG(Validate_lut(lut), "Validate lut failed.\n");
    return true;
  }

  bool Read_entry(const DATA_LUT_ENTRY& ent, char* buf) {
    if (!_is.seekg(ent._ent_ofst)) {
      AIR_ASSERT_MSG(false, "Failed to seek to entry offset.\n");
      return false;
    }
    if (!_is.read(buf, ent._size)) {
      AIR_ASSERT_MSG(false, "Failed to read entry.\n");
      return false;
    }
    return true;
  }

private:
  bool Validate_lut(const std::vector<DATA_LUT_ENTRY>& lut) {
    if (lut.size() != _ent_count) {
      AIR_ASSERT_MSG(false, "LUT entry count mismatch.\n");
      return false;
    }
    for (uint64_t i = 0; i < _ent_count; ++i) {
      // make sure index is unique
      if (lut[i]._index != i) {
        AIR_ASSERT_MSG(false, "LUT entry index mismatch.\n");
        return false;
      }
      // make sure each entry is aligned
      if ((lut[i]._ent_ofst % _ent_align) != 0) {
        AIR_ASSERT_MSG(false, "LUT entry not aligned.\n");
        return false;
      }
      // make sure entry offset is valid
      if (_lut_ofst > DATA_FILE_PAGE_SIZE && lut[i]._ent_ofst >= _lut_ofst) {
        AIR_ASSERT_MSG(false, "LUT entry offset out of range.\n");
        return false;
      }
    }
    return true;
  }

  std::istream& _is;
  uint64_t      _ent_count;
  uint64_t      _lut_ofst;
  uint32_t      _ent_align;
};

//! @brief Binary data file dumper
class RT_DATA_DUMPER {
public:
  RT_DATA_DUMPER(std::ostream& os) : _os(os), _ent_type(0) {}

  bool Dump(const char* fname) {
    std::ifstream ifs(fname, std::ios::binary);
    if (!ifs) {
      return false;
    }
    return Dump(ifs);
  }

  bool Dump(std::istream& is) {
    RT_DATA_READER reader(is);
    DATA_FILE_HDR  hdr;
    if (!reader.Read_hdr(hdr)) {
      return false;
    }
    _ent_type = hdr._ent_type;
    std::vector<DATA_LUT_ENTRY> lut;
    if (!reader.Read_lut(lut)) {
      return false;
    }
    Dump_hdr(hdr);
    Dump_lut(lut);
    for (auto it = lut.begin(); it != lut.end(); ++it) {
      const DATA_LUT_ENTRY& entry = *it;
      std::vector<char>     cnt(entry._size);
      if (!reader.Read_entry(entry, cnt.data())) {
        return false;
      }
      Dump_ent(entry, cnt.data());
    }
    return true;
  }

private:
  void Dump_hdr(const DATA_FILE_HDR& hdr);
  void Dump_lut(const std::vector<DATA_LUT_ENTRY>& lut);
  void Dump_ent(const DATA_LUT_ENTRY& entry, const char* buf);

  std::ostream& _os;
  uint8_t       _ent_type;

};  // RT_DATA_DUMPER

}  // namespace core

}  // namespace fhe

#endif  // FHE_CORE_RT_DATA_UTIL_H
