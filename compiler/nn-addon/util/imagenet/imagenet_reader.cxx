//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

// command line:
// $ imagenet_reader imagenet-10-raw.tar imagenet-10-label.txt 1
// imagenet-10-raw.tar is plain tar ball of ImageNet JPEG files
// imagenet-10-label.txt is plain text label file with lines of numbers

#include "nn/util/imagenet_reader.h"

int main(int argc, char* argv[]) {
  if (argc < 3) {
    printf(
        "Usage: %s <imagenet-tar-file> <imagenet-label-file> "
        "[start] [count]\n",
        argv[0]);
    printf("  load images [start, start+count) from <imagenet-tar-file>,\n");
    printf("  convert to tensor and do normalization.\n");
    printf("  imagenet-tar-file and imagenet-label-file must be provided.\n");
    printf("  start is optional with default value 0.\n");
    printf("  count is optional with default value 1.\n");
    return 0;
  }

  int start = (argc > 3) ? atoi(argv[3]) : 0;
  if (start < 0 || start > 1000) {
    printf("Warn: adjust start %d to 0.\n", start);
    start = 0;
  }
  int count = (argc > 4) ? atoi(argv[4]) : 1;
  if (count <= 0 || count > 1000) {
    printf("Warn: adjust count %d to 1.\n", count);
    count = 1;
  }

  double                    mean[3]  = {0.485, 0.456, 0.406};
  double                    stdev[3] = {0.229, 0.224, 0.225};
  nn::util::IMAGENET_READER ir(argv[1], argv[2], start, count, mean, stdev);

  if (!ir.Initialize()) {
    printf("Error: initialize imagenet reader failed.\n");
    return 1;
  }

  int                 size   = ir.Channel() * ir.Height() * ir.Width();
  int                 stride = ir.Height() * ir.Width();
  std::vector<double> buf(size);
  for (int i = 0; i < ir.Count(); ++i) {
    int lab = ir.Load(start + i, buf.data());
    printf("%2d: %2d [[%5.4f %5.4f %5.4f %5.4f ... %5.4f %5.4f %5.4f %5.4f],\n",
           start + i, lab, buf[0], buf[1], buf[2], buf[3], buf[stride - 4],
           buf[stride - 3], buf[stride - 2], buf[stride - 1]);
    printf("         [%5.4f %5.4f %5.4f %5.4f ... %5.4f %5.4f %5.4f %5.4f],\n",
           buf[stride], buf[stride + 1], buf[stride + 2], buf[stride + 3],
           buf[stride * 2 - 4], buf[stride * 2 - 3], buf[stride * 2 - 2],
           buf[stride * 2 - 1]);
    printf("         [%5.4f %5.4f %5.4f %5.4f ... %5.4f %5.4f %5.4f %5.4f]]\n",
           buf[stride * 2], buf[stride * 2 + 1], buf[stride * 2 + 2],
           buf[stride * 2 + 3], buf[size - 4], buf[size - 3], buf[size - 2],
           buf[size - 1]);
  }

  return 0;
}
