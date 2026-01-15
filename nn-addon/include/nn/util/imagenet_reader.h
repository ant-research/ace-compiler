//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_UTIL_IMAGENET_READER_H
#define NN_UTIL_IMAGENET_READER_H

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <memory>
#include <opencv2/opencv.hpp>
#include <vector>

#include "air/util/binary/tar_reader.h"

namespace nn {

namespace util {

//! IMAGENET_READER, read and preprocess image from imagenet tar file with
//  OpenCV
//  IMAGENET: https://image-net.org/
//  ILSVRC2012_img_val.tar contains 10,000 images for validation
//  ILSVRC2012_validation_ground_truth.txt contains labels for each image
//
//  IMAGENET_READER firstly load image in JPEG format from tar file, then
//  call OpenCV to convert it into NCHW Tensor with nomalization
class IMAGENET_READER {
private:
  static constexpr uint32_t WIDTH      = 224;
  static constexpr uint32_t HEIGHT     = 224;
  static constexpr uint32_t CHANNEL    = 3;
  static constexpr uint32_t PIXEL_SIZE = WIDTH * HEIGHT * CHANNEL;

public:
  //! Construct imagenet reader with tar file name, label file name, number of
  //  image to read, mean and stdev for each channel for nomalization
  IMAGENET_READER(const char* tar_file, const char* leb_file, uint32_t start,
                  uint32_t count, double mean[CHANNEL], double stdev[CHANNEL])
      : _tar_file(tar_file), _leb_file(leb_file), _start(start), _count(count) {
    _raw_data.resize(count);
    _leb_data.resize(count);
    memcpy(_mean, mean, sizeof(_mean));
    memcpy(_stdev, stdev, sizeof(_stdev));
  }

  //! Destruct imagenet reader
  ~IMAGENET_READER() {}

  //! Initialize imagenet reader to label and raw file content
  bool Initialize() {
    int idx = 0;
    // load label data
    std::ifstream ifs(_leb_file);
    // skip entries before start
    int val;
    while (ifs && idx < _start) {
      ifs >> val;
      ++idx;
    }
    if (idx != _start) {
      return false;
    }
    idx = 0;
    while (ifs && idx < _count) {
      ifs >> _leb_data[idx++];
    }
    if (idx != _count) {
      return false;
    }

    // load raw image data
    air::util::TAR_READER tr(_tar_file);
    if (!tr.Init()) {
      return false;
    }
    idx = 0;
    // skip entries before start
    air::util::TAR_ENTRY hdr;
    while (idx < _start && tr.Next(hdr, false)) {
      ++idx;
    }
    if (idx != _start) {
      return false;
    }
    idx = 0;
    while (idx < _count && tr.Next(_raw_data[idx], true)) {
      AIR_ASSERT(_raw_data[idx].Index() == idx + _start);
      ++idx;
    }
    if (idx != _count) {
      return false;
    }
    return true;
  }

  //! @brief get image channel
  constexpr uint32_t Channel() const { return CHANNEL; }

  //! @brief get image height
  constexpr uint32_t Height() const { return HEIGHT; }

  //! @brief get image width
  constexpr uint32_t Width() const { return WIDTH; }

  //! @brief get number of images
  uint32_t Count() const { return _count; }

  //! @brief load image data, adjust with mean and stdev, store to data array
  //! @return label of the image
  template <typename T>
  int Load(uint32_t index, T* data) {
    AIR_ASSERT(index >= _start && index < _start + _count);
    const char* cnt = _raw_data[index - _start].Content();
    size_t      len = _raw_data[index - _start].Length();
    AIR_ASSERT(_raw_data[index - _start].Index() == index);
    cv::_InputArray in(cnt, len);
    cv::Mat         image = cv::imdecode(in, cv::IMREAD_COLOR);
    AIR_ASSERT_MSG(!image.empty(), "failed to decode image.\n");
    AIR_ASSERT_MSG(image.channels() == Channel(), "channels mismatch\n");

    // convert to 8UC3
    image.convertTo(image, CV_8UC3);
    AIR_ASSERT_MSG(!image.empty(), "failed to convert to CV_8UC3.\n");

    // resize to 256 * 256
    constexpr int BW = 256;
    constexpr int BH = 256;
    cv::resize(image, image, cv::Size(BW, BH));
    AIR_ASSERT_MSG(!image.empty(), "failed to resize image.\n");
    AIR_ASSERT_MSG(image.total() == BW * BH, "height * width mismatch\n");

    // center chop 224 * 224
    constexpr int X = (BW - WIDTH) / 2;
    constexpr int Y = (BH - HEIGHT) / 2;
    cv::Mat       roi(image, cv::Rect(X, Y, WIDTH, HEIGHT));
    AIR_ASSERT_MSG(roi.total() == WIDTH * HEIGHT, "height * width mismatch\n");

    // writet to output and convert NHWC to NHCW, BGR to RGB
    T* data_r = data;
    T* data_g = data_r + WIDTH * HEIGHT;
    T* data_b = data_g + WIDTH * HEIGHT;
    for (uint32_t wh = 0; wh < WIDTH * HEIGHT; ++wh) {
      cv::Vec3b px = roi.at<cv::Vec3b>(wh);
      data_r[wh]   = ((T)px[2] / 255.0 - _mean[0]) / _stdev[0];
      data_g[wh]   = ((T)px[1] / 255.0 - _mean[1]) / _stdev[1];
      data_b[wh]   = ((T)px[0] / 255.0 - _mean[2]) / _stdev[2];
    }

    return _leb_data[index - _start];
  }

private:
  void Dump_mat(const cv::Mat& mat, const char* fname, bool bgr) {
    FILE* fp = fopen(fname, "w");
    for (int x = 0; x < 3; ++x) {
      for (int r = 0; r < mat.rows; ++r) {
        for (int c = 0; c < mat.cols; ++c) {
          cv::Vec3b px      = mat.at<cv::Vec3b>(r, c);
          int       channel = bgr ? x : 2 - x;
          fprintf(fp, "%5.4f ", (double)px[channel] / 255.0);
        }
      }
    }
    fclose(fp);
  }

  void Dump_vec(const double* vec, size_t len, const char* fname) {
    FILE* fp = fopen(fname, "w");
    for (int i = 0; i < len; ++i) {
      fprintf(fp, "%5.4f ", vec[i]);
    }
    fclose(fp);
  }

  std::vector<air::util::TAR_ENTRY> _raw_data;  // raw image data
  std::vector<int>                  _leb_data;  // image label
  const char*                       _tar_file;  // imagenet tar file name
  const char*                       _leb_file;  // imagenet label file name
  uint32_t                          _start;     // start index of images to read
  uint32_t                          _count;     // number of rows/images to read
  double _mean[CHANNEL];   // pre-defined mean for each channel
  double _stdev[CHANNEL];  // pre-defined stdev for each channel
};                         // IMAGENET_READER

}  // namespace util

}  // namespace nn

#endif  // NN_UTIL_IMAGENET_READER_H
