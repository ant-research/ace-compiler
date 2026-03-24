// -*-c++-*- ===================================================================
//
// Part of the AVH_Compiler Project, under the Apache License v2.0.
// See http://avhc.org/LICENSE/txt for license information.
//
// =============================================================================

#ifndef NN_UTIL_CIFAR_READER_H
#define NN_UTIL_CIFAR_READER_H

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <memory>
#include <vector>

#include "air/util/debug.h"

namespace nn {

namespace util {

//! CIFAR_READER, read image data and do preprocess from cifar binary data file
//  CATEGORY_COUNT can be 10 for cifar-10 and 100 for cifar100.
//  CIFAR-10:  http://www.cs.toronto.edu/~kriz/cifar-10-binary.tar.gz
//  CIFAR-100: http://www.cs.toronto.edu/~kriz/cifar-100-binary.tar.gz
//
//  Use pread() to perform file I/O to enable reading file in parallel in
//  multi-threading environment.
//
//  The format of cifar-10 binary data file is a flat file with multiple lines.
//  Each line contains the label and pixel RGB value in byte. Total length is
//  3073 bytes.
//  |0|1 ... 1024|1025 ... 2048|2049 ... 3072|
//  +-+----------+-------------+-------------+
//  |L|R ... RRRR|GGGG ... GGGG|BBBB ... BBBB|
//  L is in range of [0, 9] to indicate the label. RGB is in range of [0, 255].
//
//  The format of cifar-100 is similar to cifar-10 but with 2 bytes for coarse
//  and file label respectively. Total length is 3074 bytes.
//  |0|1|2 ... 1025|1026 ... 2049|2050 ... 3073|
//  +-+-+----------+-------------+-------------+
//  |L|l|R ... RRRR|GGGG ... GGGG|BBBB ... BBBB|
//  L is in range of [0, 19] for coarse label. l is in range of [0, 99] for fine
//  label. Only fine label is used.

template <uint32_t CATEGORY_COUNT>
class CIFAR_READER {
private:
  AIR_STATIC_ASSERT(CATEGORY_COUNT == 10 || CATEGORY_COUNT == 100);
  static constexpr uint32_t WIDTH      = 32;
  static constexpr uint32_t HEIGHT     = 32;
  static constexpr uint32_t CHANNEL    = 3;
  static constexpr uint32_t LABEL_SIZE = (CATEGORY_COUNT == 10) ? 1 : 2;
  static constexpr uint32_t PIXEL_SIZE = WIDTH * HEIGHT * CHANNEL;
  static constexpr uint32_t ROW_SIZE   = LABEL_SIZE + PIXEL_SIZE;

public:
  //! Construct cifar reader with file name. mean and stdev for each channel
  CIFAR_READER(const char* file, double mean[CHANNEL], double stdev[CHANNEL])
      : _count(0) {
    memcpy(_mean, mean, sizeof(_mean));
    memcpy(_stdev, stdev, sizeof(_stdev));
    _fd = open(file, O_RDONLY);
  }

  //! Destruct cifar reader and close file
  ~CIFAR_READER() {
    if (_fd) {
      close(_fd);
    }
  }

  //! Initialize cifar reader to get file size and row count
  bool Initialize() {
    if (_fd == -1) {
      return false;
    }
    struct stat sbuf;
    int         ret = fstat(_fd, &sbuf);
    if (ret == -1 || sbuf.st_size % ROW_SIZE != 0) {
      return false;
    }
    _count = sbuf.st_size / ROW_SIZE;
    return true;
  }

  constexpr uint32_t Channel() const { return CHANNEL; }
  constexpr uint32_t Height() const { return HEIGHT; }
  constexpr uint32_t Width() const { return WIDTH; }

  uint32_t Count() const { return _count; }

  //! Load image data, adjust with mean and stdev, store to data array
  template <typename T>
  int Load(uint32_t index, T* data) {
    AIR_ASSERT(_fd != -1);
    AIR_ASSERT(index < _count);
    uint8_t buf[ROW_SIZE];
    int     ret = pread(_fd, buf, ROW_SIZE, index * ROW_SIZE);
    if (ret != ROW_SIZE) {
      return -1;
    }
    uint32_t px = 0;
    for (uint32_t ch = 0; ch < CHANNEL; ++ch) {
      for (uint32_t wh = 0; wh < WIDTH * HEIGHT; ++wh) {
        data[px] =
            ((double)buf[px + LABEL_SIZE] / 255.0 - _mean[ch]) / _stdev[ch];
        px++;
      }
    }
    return (CATEGORY_COUNT == 10) ? buf[0] : buf[1];
  }

private:
  int      _fd;              // file descriptor
  uint32_t _count;           // number of rows (images) in the file
  double   _mean[CHANNEL];   // pre-defined mean for each channel
  double   _stdev[CHANNEL];  // pre-defined stdev for each channel
};                           // CIFAR_READER

}  // namespace util

}  // namespace nn

#endif  // NN_UTIL_CIFAR_READER_H
