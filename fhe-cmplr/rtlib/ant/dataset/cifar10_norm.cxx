//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <math.h>

#include "common/rtlib.h"
#include "nn/util/cifar_reader.h"

#define CIFAR_CLASS_COUNT 10
typedef nn::util::CIFAR_READER<CIFAR_CLASS_COUNT> CIFAR_READER;

int main(int argc, char* argv[]) {
  if (argc == 1) {
    printf(
        "[INFO] Usage: %s <path to test_batch.bin in cifar-10 batches-bin> "
        "[start] [end]\n",
        argv[0]);
    return 0;
  }

  double       ref_mean[]  = {0.0, 0.0, 0.0};
  double       ref_stdev[] = {1.0, 1.0, 1.0};
  CIFAR_READER cifar10_reader(argv[1], ref_mean, ref_stdev);
  if (cifar10_reader.Initialize() == false) {
    printf("[ERROR] Fail to initialize cifar-10 reader. please check file %s\n",
           argv[1]);
    return 1;
  }

  uint32_t start_idx = 0;
  uint32_t end_idx   = cifar10_reader.Count() - 1;
  if (argc > 2) {
    start_idx = atoi(argv[2]);
    if (start_idx >= cifar10_reader.Count()) {
      printf("[ERRPR] start image %d exceeds the limit %d\n", start_idx,
             cifar10_reader.Count());
      return 1;
    }
    end_idx = start_idx;
  }

  if (argc > 3) {
    end_idx = atoi(argv[3]);
    if (end_idx < start_idx || end_idx >= cifar10_reader.Count()) {
      printf("[ERRPR] end image %d exceeds the range [%d, %d]\n", end_idx,
             start_idx, cifar10_reader.Count());
      return 1;
    }
  }
  printf("INFO: normalize images [%d, %d] from cifar-10 %s.\n", start_idx,
         end_idx, argv[1]);
  uint32_t total   = end_idx - start_idx + 1;
  uint32_t channel = cifar10_reader.Channel();
  uint32_t height  = cifar10_reader.Height();
  uint32_t width   = cifar10_reader.Width();

  double               mean[3]  = {0.0, 0.0, 0.0};
  double               stdev[3] = {0.0, 0.0, 0.0};
  std::vector<TENSOR*> images(total);
  for (uint32_t i = start_idx; i <= end_idx; ++i) {
    TENSOR* input_data = Alloc_tensor(1, channel, height, width, NULL);
    int     label      = cifar10_reader.Load(i, input_data->_vals);
    AIR_ASSERT(label != -1);
    images[i - start_idx] = input_data;
  }
  for (uint32_t i = 0; i < total; ++i) {
    TENSOR* input_data = images[i];
    for (uint32_t j = 0; j < channel; ++j) {
      for (uint32_t k = 0; k < height; ++k) {
        for (uint32_t l = 0; l < width; ++l) {
          mean[j] += TENSOR_ELEM(input_data, 0, j, k, l);
        }
      }
    }
  }
  for (uint32_t i = 0; i < channel; ++i) {
    mean[i] /= (double)(height * width * total);
  }
  for (uint32_t i = 0; i < total; ++i) {
    TENSOR* input_data = images[i];
    for (uint32_t j = 0; j < channel; ++j) {
      for (uint32_t k = 0; k < height; ++k) {
        for (uint32_t l = 0; l < width; ++l) {
          double val = TENSOR_ELEM(input_data, 0, j, k, l);
          stdev[j] += (val - mean[j]) * (val - mean[j]);
        }
      }
    }
    Free_tensor(input_data);
  }
  images.clear();
  for (uint32_t i = 0; i < channel; ++i) {
    stdev[i] = sqrt(stdev[i] / (height * width * total - 1));
  }

  printf("[INFO] normalize %d images, mean=[%f, %f, %f], stdev=[%f, %f, %f].\n",
         end_idx - start_idx + 1, mean[0], mean[1], mean[2], stdev[0], stdev[1],
         stdev[2]);

  return 0;
}
