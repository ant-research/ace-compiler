//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

// Standalone bootstrap smoke for the C-side OpenFHE backend (Stage 4).
// Mirrors eg_rtopenfhe_add.cxx / eg_rtphantom_boot.cxx: the .inc at the bottom
// supplies Need_bts() (true), Get_context_params(), and Main_graph() which
// depletes the input cipher, bootstraps it, and sets the output cipher while
// printing the pre/post level info. This main encrypts the input, runs the
// graph, decrypts the output, prints max_abs, and exits non-zero on tolerance
// failure.

#include <math.h>

#include <cstdio>
#include <cstdlib>
#include <vector>

#include "common/rtlib.h"

double Expected_data[] = {0.25, 0.5, 0.75, 1.0, -0.5, -0.25, -0.75, -1.0};
int    Expected_len    = 8;

// set by Main_graph() in the .inc (refresh verdict); OR'd into final verdict.
extern int g_boot_level_ok;

TENSOR* Generate_input_data(size_t n, size_t c, size_t h, size_t w,
                            double* data) {
  return Alloc_tensor(n, c, h, w, data);
}

int main(int argc, char* argv[]) {
  Prepare_context();

  double input_val[] = {0.25, 0.5, 0.75, 1.0, -0.5, -0.25, -0.75, -1.0};
  TENSOR* input_data = Generate_input_data(1, 1, 2, 4, input_val);
  printf("input");
  Print_tensor(stdout, input_data);
  Prepare_input(input_data, "input");
  Free_tensor(input_data);

  Run_main_graph();

  double* result = Handle_output("output");
  Finalize_context();

  // absolute error check
  // the bootstrap smoke passes if (1) the level-refresh delta was positive
  // (printed in Main_graph) and (2) max_abs is below the tolerance (start
  // strict 1e-3; relax to 1e-2 if toy FIXEDMANUAL precision requires it). Both
  // conditions must hold; on failure we print FAILED and exit non-zero.
  double error     = 1e-3;
  double max_abs   = 0.0;
  printf("decoded: ");
  for (int i = 0; i < Expected_len; ++i) {
    double e = fabs(result[i] - Expected_data[i]);
    if (e > max_abs) max_abs = e;
    printf("%.4f ", result[i]);
  }
  printf("\n");
  free(result);

  bool level_ok = (g_boot_level_ok != 0);

  printf("max_abs=%.6g (tolerance=%.0e) level_refresh=%s\n", max_abs, error,
         level_ok ? "yes" : "no");
  if (level_ok && max_abs < error) {
    printf("SUCCESS!\n");
    return 0;
  }
  printf("FAILED!\n");
  return 1;
}

#include "eg_rtopenfhe_boot.inc"
