//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "common/rtlib.h"
#include "nn/util/imagenet_reader.h"

#define IMAGENET_CLASS_COUNT 1000

enum RESULT {
  FAIL  = 0,
  TOP_1 = 1,
  TOP_5 = 2
};

#define RES_K 10
#define TOP_K  5

RESULT Validate_output_data(double* result, int len, int label) {
  int    top10_idx[RES_K] = { 0 };
  double top10_val[RES_K] = { 0.0 };
  for (int i = 0; i < len; ++i) {
    int j = RES_K - 1;
    while ((j >= 0) && (result[i] > top10_val[j])) {
      -- j;
    }
    if (j == RES_K - 1) {
      continue;
    }

    for (int k = RES_K - 1; k > j + 1; --k) {
      top10_val[k] = top10_val[k - 1];
      top10_idx[k] = top10_val[k - 1];
    }
    top10_val[j + 1] = result[i];
    top10_idx[j + 1] = i;
  }

  printf("TOP %d: [", RES_K);
  for (int i = 0; i < RES_K; ++i) {
    if (i > 0) {
      printf(", ");
    }
    printf("%d:%.4f", top10_idx[i], top10_val[i]);
  }
  printf("], expected=%d.\n", label);
  if (top10_idx[0] == label) {
    return TOP_1;
  }
  for (int i = 1; i < TOP_K; ++i) {
    if (top10_idx[i] == label) {
      return TOP_5;
    }
  }
  return FAIL;
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    printf(
        "[INFO] Usage: %s <imagenet-tar-file> <imagenet-label-file> "
        "[start] [end]\n",
        argv[0]);
    return 0;
  }

  uint32_t start_idx = 0;
  uint32_t end_idx = 0;
  if (argc > 3) {
    start_idx = atoi(argv[3]);
  }

  if (argc > 4) {
    end_idx = atoi(argv[4]);
    if (end_idx < start_idx) {
      printf("[ERRPR] end image %d exceeds start index %d\n", end_idx, start_idx);
      return 1;
    }
  }
  printf("INFO: infer images [%d, %d] from %s, label in file %s.\n", start_idx, end_idx,
         argv[1], argv[2]);

  double                                    mean[]  = {0.485, 0.456, 0.406};
  double                                    stdev[] = {0.229, 0.224, 0.225};
  nn::util::IMAGENET_READER reader(argv[1], argv[2], start_idx,
                                   end_idx - start_idx + 1, mean, stdev);
  if (reader.Initialize() == false) {
    printf("[ERROR] Fail to initialize imagenet reader. please check file %s, "
           "label file %s, start index %d, or end index %d\n",
           argv[1], argv[2], start_idx, end_idx);
    return 1;
  }

  Prepare_context();

  uint32_t top1_cnt = 0;
  uint32_t top5_cnt = 0;
  uint32_t fail_cnt = 0;
#pragma omp parallel for reduction(+:top1_cnt) reduction(+:fail_cnt)
  for (uint32_t i = start_idx; i <= end_idx; ++i) {
    int label;
#pragma omp critical
    {
      TENSOR* input_data =
          Alloc_tensor(1, reader.Channel(), reader.Height(),
                       reader.Width(), NULL);
      label = reader.Load(i, input_data->_vals);
      AIR_ASSERT(label != -1);
      Prepare_input(input_data, "input");
      Free_tensor(input_data);
    }

    Run_main_graph();

#pragma omp critical
    {
      double* result = Handle_output("output");
      RESULT  res    = Validate_output_data(result, IMAGENET_CLASS_COUNT, label);
      free(result);
      if (res == TOP_1) {
        printf("[INFO] infer image %d top 1 success\n", i);
        ++top1_cnt;
        ++top5_cnt;
      } else if (res == TOP_5) {
        printf("[INFO] infer image %d top 5 success\n", i);
        ++top5_cnt;
      } else {
        printf("[INFO] infer image %d failed\n", i);
        ++fail_cnt;
      }
    }
  }
  printf("[RESULT] infer %d images, top1 %d %.3f, top5 %d %.3f fail %d %.3f.\n",
         end_idx - start_idx + 1, top1_cnt,
         (double)top1_cnt / (double)(end_idx - start_idx + 1), top5_cnt,
         (double)top5_cnt / (double)(end_idx - start_idx + 1), fail_cnt,
         (double)fail_cnt / (double)(end_idx - start_idx + 1));

  Finalize_context();

  return 0;
}
