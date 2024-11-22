//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/core/rt_data_util.h"

namespace fhe {

namespace core {

void RT_DATA_DUMPER::Dump_hdr(const DATA_FILE_HDR& hdr) {
  _os << "Header:" << std::endl;
  _os << "  Magic:    " << hdr._magic << std::endl;
  _os << "  Version:  " << hdr._rt_ver << std::endl;
  _os << "  Flag:     " << hdr._flag << std::endl;
  _os << "  Type:     " << Data_entry_name((DATA_ENTRY_TYPE)hdr._ent_type)
      << std::endl;
  _os << "  Align:    " << (uint32_t)hdr._ent_align << std::endl;
  _os << "  Count:    " << hdr._ent_count << std::endl;
  _os << "  LUT ofst: " << hdr._lut_ofst << std::endl;
  //_os << "  Created:  " << hdr._ctime << std::endl;
  _os << "  Model:    " << hdr._model << std::endl;
  _os << "  UUID:     " << hdr._uuid << std::endl;
}

void RT_DATA_DUMPER::Dump_lut(const std::vector<DATA_LUT_ENTRY>& lut) {
  _os << "Lookup Table:" << std::endl;
  for (auto it = lut.cbegin(); it != lut.cend(); ++it) {
    _os << "  " << it->_name << " | " << it->_index << ": ";
    _os << "size=" << it->_size << " ofst=" << it->_ent_ofst << std::endl;
  }
}

void RT_DATA_DUMPER::Dump_ent(const DATA_LUT_ENTRY& entry, const char* buf) {
  _os << entry._index << " :";
  auto printer = [](std::ostream& os, uint64_t count, auto val) {
    if (count > 0) os << " " << val[0];
    if (count > 1) os << " " << val[1];
    if (count > 2) os << " " << val[2];
    if (count > 3) os << " " << val[3];
    if (count > 8) os << " ...";
    if (count > 7) os << " " << val[count - 4];
    if (count > 6) os << " " << val[count - 3];
    if (count > 5) os << " " << val[count - 2];
    if (count > 4) os << " " << val[count - 1];
  };
  if (_ent_type == DE_MSG_F32) {
    uint64_t count = entry._size / sizeof(float);
    AIR_ASSERT((entry._size % sizeof(float)) == 0);
    printer(_os, count, (const float*)buf);
  } else if (_ent_type == DE_MSG_F64) {
    uint64_t count = entry._size / sizeof(double);
    AIR_ASSERT((entry._size % sizeof(double)) == 0);
    printer(_os, count, (const double*)buf);
  } else if (_ent_type == DE_PLAINTEXT) {
    _os << " Plaintext";
  }
  _os << std::endl;
}

}  // namespace core

}  // namespace fhe
