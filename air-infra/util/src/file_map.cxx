//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/util/file_map.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace air {

namespace util {

FILE_MAP::FILE_MAP(const char* name, bool op)
    : _file(name), _fd(0), _file_size(0), _map(0), _map_size(0), _op(op) {
  if (op) {
    Open(O_RDWR | O_CREAT | O_TRUNC);
    Write(PROT_READ | PROT_WRITE, MAP_SHARED);
  } else {
    Open(O_RDONLY);
    Read(PROT_READ, MAP_SHARED);
  }
}

void FILE_MAP::Open(uint32_t op) {
  _fd = open(Get_file_name(), op, (mode_t)0600);
  if (_fd < 0) {
    CMPLR_ASSERT(false, "Open elf file err: %s", Get_file_name());
    // TODO: to be completed err info
  }
}

void FILE_MAP::Close() { close(_fd); }

void FILE_MAP::Read(uint32_t prot, uint32_t flags) {
  struct stat file_info = {0};

  if (fstat(_fd, &file_info) == -1) {
    CMPLR_ASSERT(false, "Error getting the file size: %s", Get_file_name());
    // TODO: to be completed err info
  }

  if (file_info.st_size == 0) {
    CMPLR_ASSERT(false, "Error: File is empty, nothing to do: %s",
                 Get_file_name());
    // TODO: to be completed err info
  }

  AIR_DEBUG("File size is %ji\n", (intmax_t)file_info.st_size);

  Set_file_size(file_info.st_size);

  // TODO: To be completed remap ops
  if (file_info.st_size > MAPPED_SIZE) {
    Set_map_size(MAPPED_SIZE);
  } else {
    Set_map_size(file_info.st_size);
  }

  _map = (char*)mmap(NULL, Get_map_size(), prot, flags, Get_file_id(), 0);
  if (_map == MAP_FAILED) {
    close(_fd);
    CMPLR_ASSERT(false, "Map elf file failed: %s", Get_file_name());
    // TODO: to be completed err info
  }
}

void FILE_MAP::Write(uint32_t prot, uint32_t flags) {
  Set_map_size(MAPPED_SIZE);
  lseek(_fd, Get_map_size() - 1, SEEK_END);
  write(_fd, "", 1);

  _map = (char*)mmap(NULL, Get_map_size(), prot, flags, _fd, 0);
  if (_map == MAP_FAILED) {
    close(_fd);
    CMPLR_ASSERT(false, "Map elf file failed: %s", Get_file_name());
    // TODO: to be completed err info
  }
}

void FILE_MAP::Remap(uint32_t sz) {
  // remap file
  if (ftruncate(_fd, sz) == -1) {
    std::cerr << "Failed to truncate file" << std::endl;
    CMPLR_ASSERT(false, "Failed to truncate file");
    munmap(_map, Get_map_size());
    close(_fd);
  }

  // remap file
  void* new_data = mremap(_map, Get_map_size(), sz, MREMAP_MAYMOVE);
  if (new_data == MAP_FAILED) {
    CMPLR_ASSERT(false, "Failed to remap file");
    munmap(_map, Get_map_size());
    close(_fd);
  }
  _map = (char*)new_data;
}
void FILE_MAP::Unmap() {
  // Don't forget to free the mmapped memory
  if (munmap(_map, Get_map_size()) == -1) {
    close(_fd);
    CMPLR_ASSERT(false, "Error un-mmapping the file: %s", Get_file_name());
    return;  // TODO: to be completed err info
  }

  // Un-mmaping doesn't close the file, so we still need to do that.
  close(_fd);
}

}  // namespace util

}  // namespace air
