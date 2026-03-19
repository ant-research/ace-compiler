//-*-c-*-
//=============================================================================
// Test harness for generated bootstrap_full kernel.
// Compile with examples/output/bootstrap_full.c (which includes the wrapper
// providing Main_graph, encode/decode schemes) and link with ANT runtime.
// Run with CWD = examples/output so bootstrap_full_data.msg is found.
//=============================================================================

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/rtlib.h"

#define NUM_SLOTS 8

static double Input_p0[] = {0.1, -0.2, 0.3, -0.4, 0.5, -0.6, 0.7, -0.8};
static double Input_p1[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
// evalmod mode computes an EvalMod-style transform (sin(8*x) surrogate)
static double Expected_evalmod[] = {0.717356, -0.999574, 0.675463, 0.058374, -0.756802, 0.996165, -0.631267, -0.116549};
// inline/primitive/rtlib modes are message-preserving in this test.
static double Expected_identity[] = {0.1, -0.2, 0.3, -0.4, 0.5, -0.6, 0.7, -0.8};

static TENSOR* Generate_input_data(size_t n, size_t c, size_t h, size_t w,
                                  double* data) {
  return Alloc_tensor(n, c, h, w, data);
}

static bool Validate_output_data(double* result, double* expect, int len) {
  double error = 1e-2;
  for (int i = 0; i < len; i++) {
    if (fabs(result[i] - expect[i]) > error) {
      printf("index: %d, value: %f != %f\n", i, result[i], expect[i]);
      return false;
    }
  }
  return true;
}

static void Print_output_data(double* result, int len) {
  printf("RESULT:");
  for (int i = 0; i < len; i++) {
    printf(i == 0 ? "%.9f" : ",%.9f", result[i]);
  }
  printf("\n");
}

static bool Is_evalmod_mode(void) {
  const char* mode = getenv("ACE_BOOTSTRAP_IMPL");
  if (mode == NULL) {
    return false;
  }
  return strcmp(mode, "evalmod") == 0 || strcmp(mode, "cheb") == 0 || strcmp(mode, "legacy") == 0;
}

int main(int argc, char* argv[]) {
  Prepare_context();

  TENSOR* tensor_p0 = Generate_input_data(1, 1, 1, NUM_SLOTS, Input_p0);
  TENSOR* tensor_p1 = Generate_input_data(1, 1, 1, NUM_SLOTS, Input_p1);
  Prepare_input(tensor_p0, "p0");
  Prepare_input(tensor_p1, "p1");
  Free_tensor(tensor_p0);
  Free_tensor(tensor_p1);

  Run_main_graph();

  double* result = Handle_output("output");
  Print_output_data(result, NUM_SLOTS);
  Finalize_context();

  double* expected = Is_evalmod_mode() ? Expected_evalmod : Expected_identity;
  bool res = Validate_output_data(result, expected, NUM_SLOTS);
  free(result);
  if (res) {
    printf("SUCCESS\n");
    return 0;
  }
  printf("FAILED\n");
  return 1;
}

// The wrapper (Main_graph, Get_encode_scheme, etc.) is now appended to
// bootstrap_full.c by the demo, so no separate #include is needed here.
