//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "common/rtlib.h"
#include "nn/util/cifar_reader.h"

bool Validate_output_data(double* result, int len, int label) {
  printf("Result: [");
  for (int i = 0; i < len; ++i) {
    if (i > 0) {
      printf(", ");
    }
    printf("%.4f", result[i]);
  }
  printf("], expected=%d.\n", label);
  double max_val = result[label];
  for (int i = 0; i < label; ++i) {
    if (result[i] > max_val) {
      return false;
    }
  }
  for (int i = label + 1; i < len; ++i) {
    if (result[i] > max_val) {
      return false;
    }
  }
  return true;
}

int main(int argc, char* argv[]) {
  if (argc == 1) {
    printf(
        "[INFO] Usage: %s <path to test_batch.bin in cifar-%d batches-bin> "
        "[start] [end]\n",
        argv[0], CIFAR_CLASS_COUNT);
    return 0;
  }

  double                                    mean[]  = {0.485, 0.456, 0.406};
  double                                    stdev[] = {0.229, 0.224, 0.225};
  nn::util::CIFAR_READER<CIFAR_CLASS_COUNT> cifar_reader(argv[1], mean,
                                                           stdev);
  if (cifar_reader.Initialize() == false) {
    printf("[ERROR] Fail to initialize cifar-%d reader. please check file %s\n",
           CIFAR_CLASS_COUNT, argv[1]);
    return 1;
  }

  uint32_t start_idx = 0;
  uint32_t end_idx   = cifar_reader.Count() - 1;
  if (argc > 2) {
    start_idx = atoi(argv[2]);
    if (start_idx >= cifar_reader.Count()) {
      printf("[ERRPR] start image %d exceeds the limit %d\n", start_idx,
             cifar_reader.Count());
      return 1;
    }
    end_idx = start_idx;
  }

  if (argc > 3) {
    end_idx = atoi(argv[3]);
    if (end_idx < start_idx || end_idx >= cifar_reader.Count()) {
      printf("[ERRPR] end image %d exceeds the range [%d, %d]\n", end_idx,
             start_idx, cifar_reader.Count());
      return 1;
    }
  }
  printf("INFO: infer images [%d, %d] from cifar-%d %s.\n", start_idx, end_idx,
         CIFAR_CLASS_COUNT, argv[1]);

  Prepare_context();

  uint32_t pass_cnt = 0;
  uint32_t fail_cnt = 0;
#pragma omp parallel for reduction(+:pass_cnt) reduction(+:fail_cnt)
  for (uint32_t i = start_idx; i <= end_idx; ++i) {
    int label;
#pragma omp critical
    {
      TENSOR* input_data =
          Alloc_tensor(1, cifar_reader.Channel(), cifar_reader.Height(),
                       cifar_reader.Width(), NULL);
      label = cifar_reader.Load(i, input_data->_vals);
      AIR_ASSERT(label != -1);
      Prepare_input(input_data, "input");
      Free_tensor(input_data);
    }

    Run_main_graph();

#pragma omp critical
    {
      double* result = Handle_output("output");
      bool    res    = Validate_output_data(result, CIFAR_CLASS_COUNT, label);
      free(result);
      if (res) {
        printf("[INFO] infer image %d success\n", i);
        ++pass_cnt;
      } else {
        printf("[INFO] infer image %d failed\n", i);
        ++fail_cnt;
      }
    }
  }
  printf("[RESULT] infer %d images, pass %d %.3f, fail %d %.3f.\n",
         end_idx - start_idx + 1, pass_cnt,
         (double)pass_cnt / (double)(end_idx - start_idx + 1), fail_cnt,
         (double)fail_cnt / (double)(end_idx - start_idx + 1));

  Finalize_context();

  return 0;
}
