//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_BINARY_TAR_READER_H
#define AIR_UTIL_BINARY_TAR_READER_H

#include <stdio.h>

#include "air/util/binary/tar_hdr.h"
#include "air/util/debug.h"

namespace air {

namespace util {

//! @brief TAR_ENTRY: a tar entry
class TAR_ENTRY {
  friend class TAR_READER;

public:
  //! @brief file content
  const char* Content() const { return _content; }

  //! @brief content length
  size_t Length() const { return _length; }

  //! @brief index in tar file
  uint32_t Index() const { return _index; }

  //! @brief construct an empty entry
  TAR_ENTRY() : _content(nullptr), _length(0), _max_len(0), _index(0) {}

  //! @brief destructor, free content if necessary
  ~TAR_ENTRY() {
    if (_content != nullptr) {
      delete[] _content;
    }
  }

private:
  char* Alloc(size_t sz) {
    _length = sz;
    if (_max_len >= sz) {
      AIR_ASSERT(_content != nullptr);
      _content[sz - 1] = 0;
      return _content;
    }
    if (_content != nullptr) {
      delete[] _content;
    }
    _content = new char[sz];
    AIR_ASSERT(_content != nullptr);
    _max_len = sz;
    return _content;
  }

  void Set_length(size_t len) { _length = len; }
  void Set_index(uint32_t idx) { _index = idx; }

  char*    _content;  // content buffer
  size_t   _length;   // content length
  size_t   _max_len;  // max length of content buffer
  uint32_t _index;    // index in tar file
};

//! @brief TAR file reader
class TAR_READER {
public:
  //! @brief construct TAR_READER object
  TAR_READER(const char* file) : _tar_fname(file), _fp(nullptr), _index(0) {}

  //! @brief destructor, close file if it's not closed
  ~TAR_READER() {
    if (_fp) {
      fclose(_fp);
    }
  }

  //! @brief open file and seek to first entry
  //! @return true if first entry is located correctly
  //!         false error occurs during open file and locate first entry
  bool Init() {
    AIR_ASSERT(_fp == nullptr);
    _fp = fopen(_tar_fname, "rb");
    if (_fp == nullptr) {
      return false;
    }
    fseek(_fp, 0, SEEK_SET);
    _index = 0;
    return true;
  }

  //! @brief copy current entry info to entry and move to next entry
  //! @return true if the current entry is available
  //!         false if there is no more entries
  bool Next(TAR_ENTRY& entry, bool read_content) {
    AIR_ASSERT(_fp != nullptr);
    TAR_HDR hdr;
    size_t  sz = fread(&hdr, sizeof(hdr), 1, _fp);
    if (sz != 1) {
      // not a complete tar header
      AIR_ASSERT_MSG(false, "Not a tar file entry.\n");
      return false;
    }
    if (Is_zero(&hdr)) {
      // reach end of tar file
      return false;
    }
    if (!hdr.Validate()) {
      // not a valid tar header
      AIR_ASSERT_MSG(false, "Not a valid tar file entry.\n");
      return false;
    }
    AIR_ASSERT_MSG(hdr.Is_ustar(), "Not a ustar entry.\n");
    AIR_ASSERT_MSG(hdr.Is_file(), "Not a normal file entry.\n");
    size_t length  = hdr.File_size();
    size_t rem     = length % TAR_BLOCK_SIZE;
    size_t padding = (rem != 0) ? TAR_BLOCK_SIZE - rem : 0;
    if (read_content) {
      char* buf = entry.Alloc(length);
      sz        = fread(buf, sizeof(char), length, _fp);
      if (sz != length) {
        // read content failed
        AIR_ASSERT_MSG(false, "Read file content failed.\n");
        return false;
      }
    } else {
      padding += length;
    }
    if (padding != 0) {
      int ret = fseek(_fp, padding, SEEK_CUR);
      AIR_ASSERT_MSG(ret == 0, "Failed to seek fp for padding.\n");
    }

    entry.Set_length(length);
    entry.Set_index(_index++);
    return true;
  }

  //! @brief finalize file
  void Fini() {
    AIR_ASSERT(_fp != nullptr);
    fclose(_fp);
    _fp = nullptr;
  }

private:
  bool Is_zero(const TAR_HDR* hdr) const {
    uint64_t* buf = (uint64_t*)hdr;
    for (int i = 0; i < sizeof(TAR_HDR) / sizeof(uint64_t); ++i) {
      if (buf[i] != 0) {
        return false;
      }
    }
    return true;
  }

  const char* _tar_fname;
  FILE*       _fp;
  uint32_t    _index;
};  // class TAR_READER

}  // namespace util

}  // namespace air

#endif  // AIR_UTIL_BINARY_TAR_READER_H
