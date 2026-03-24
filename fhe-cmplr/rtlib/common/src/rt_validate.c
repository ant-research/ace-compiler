//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "common/rt_validate.h"

#include <math.h>
#include <stdlib.h>

#define EXT_NUM 8

void Validate_impl(double* res, double* msg, uint32_t len, int32_t epsilon) {
  double   error       = pow(10, epsilon);
  bool     found_error = false;
  uint32_t i;
  for (i = 0; i < len; ++i) {
    if (fabs(res[i] - msg[i]) > error) {
      fprintf(stderr, "ERROR: validation failed at %d. %f != %f\n", i, res[i],
              msg[i]);
      found_error = true;
      break;
    }
  }
  if (found_error) {
    fprintf(stderr, "idx: ");
    int32_t start = (i > EXT_NUM) ? i - EXT_NUM : 0;
    int32_t end   = (start + 2 * EXT_NUM < len) ? start + 2 * EXT_NUM : len;
    for (int32_t j = start; j < end; j++) {
      fprintf(stderr, "%7d%c ", j, (j == i) ? '*' : ' ');
    }
    fprintf(stderr, "\nres: ");
    for (int32_t j = start; j < end; j++) {
      fprintf(stderr, "%8.4f ", res[j]);
    }
    fprintf(stderr, "\nstd: ");
    for (int32_t j = start; j < end; j++) {
      fprintf(stderr, "%8.4f ", msg[j]);
    }
    fprintf(stderr, "\n");
  }
  fprintf(stdout, "%s: internal validation %s.\n",
          found_error ? "ERROR" : "INFO", found_error ? "fail" : "pass");
  fflush(stdout);
}

void Validate_value_range(bool within_range) {
  if (!within_range) {
    fprintf(
        stdout,
        "Please set a higher level or increase the difference between q0 and "
        "sf.\n");
  }
  fprintf(stdout, "%s: internal value range validation %s.\n",
          !within_range ? "ERROR" : "INFO", !within_range ? "fail" : "pass");
  fflush(stdout);
}

double* Add_impl(double* msg0, double* msg1, uint32_t len) {
  double* ret = (double*)malloc(sizeof(double) * len);
  for (uint32_t i = 0; i < len; ++i) {
    ret[i] = msg0[i] + msg1[i];
  }
  return ret;
}

double* Mul_impl(double* msg0, double* msg1, uint32_t len) {
  double* ret = (double*)malloc(sizeof(double) * len);
  for (uint32_t i = 0; i < len; ++i) {
    ret[i] = msg0[i] * msg1[i];
  }
  return ret;
}

double* Rotate_impl(double* msg0, uint32_t len, int32_t rotation) {
  double* res = (double*)malloc(len * sizeof(double));
  if (rotation < 0) {
    for (int32_t i = 0; i < -rotation; ++i) {
      res[i] = msg0[i + len + rotation];
    }
    for (int32_t i = -rotation; i < len; ++i) {
      res[i] = msg0[i + rotation];
    }
  } else {
    for (int32_t i = 0; i < len - rotation; ++i) {
      res[i] = msg0[i + rotation];
    }
    for (int32_t i = len - rotation; i < len; ++i) {
      res[i] = msg0[i + rotation - len];
    }
  }
  return res;
}

double* Relu_impl(double* msg0, uint32_t len) {
  double  vmin = msg0[0], vmax = msg0[0];
  double* ret = (double*)malloc(sizeof(double) * len);
  for (uint32_t i = 0; i < len; ++i) {
    if (msg0[i] > vmax) {
      vmax = msg0[i];
    }
    if (msg0[i] < vmin) {
      vmin = msg0[i];
    }
    ret[i] = (msg0[i] < 0) ? 0 : msg0[i];
  }
  fprintf(stderr, "INFO: relu value range [%.4f, %.4f].\n", vmin, vmax);
  return ret;
}

static double* Pad_tensor(double* msg, int n, int c, int h, int w, int pn,
                          int pc, int ph, int pw) {
  int     nh  = h + 2 * ph;
  int     nw  = w + 2 * pw;
  double* res = (double*)malloc(sizeof(double) * n * c * nh * nw);
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < c; ++j) {
      for (int k = 0; k < nh; ++k) {
        for (int l = 0; l < nw; ++l) {
          double val;
          if (k < ph || l < pw || k >= h + ph || l >= w + pw) {
            val = 0;
          } else {
            val = msg[((i * c + j) * h + k - ph) * w + l - pw];
          }
          res[((i * c + j) * nh + k) * nw + l] = val;
        }
      }
    }
  }
  return res;
}

double* Conv_impl(double* msg0, int n, int c, int h, int w, float* weight,
                  int kn, int kc, int kh, int kw, float* bias, int bw, int sh,
                  int sw, int pn, int pc, int ph, int pw) {
  IS_TRUE(kc == c, "channel mismatch");
  IS_TRUE(kn == bw, "bias length mismatch");

  if (ph != 0 || pw != 0) {
    msg0 = Pad_tensor(msg0, n, c, h, w, pn, pc, ph, pw);
    h += ph * 2;
    w += pw * 2;
  }

  int     oh     = (h - (kh - 1) - 1) / sh + 1;
  int     ow     = (w - (kw - 1) - 1) / sw + 1;
  double* output = (double*)malloc(sizeof(double) * n * kn * oh * ow);

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < kn; ++j) {
      for (int k = 0; k < oh; ++k) {
        for (int l = 0; l < ow; ++l) {
          int    im_si = k * sh;
          int    im_sj = l * sw;
          double total = 0;

          // input's channel
          for (int m = 0; m < kc; ++m) {
            for (int n = 0; n < kh; ++n) {
              int x = im_si + n;
              if (x < 0 || x >= h) {
                continue;
              }
              for (int o = 0; o < kw; ++o) {
                int y = im_sj + o;
                if (y < 0 || y >= w) {
                  continue;
                }
                double a = msg0[((i * c + m) * h + x) * w + y];
                double b = weight[((j * kc + m) * kh + n) * kw + o];
                total += a * b;
              }
            }
          }
          total += bias[j];
          output[((i * kn + j) * oh + k) * ow + l] = total;
        }
      }
    }
  }

  if (ph != 0 || pw != 0) {
    free(msg0);
  }
  return output;
}

double* Gemm_impl(double* msg0, int h, int w, float* weight, int wh, int ww,
                  float* bias, int bw) {
  IS_TRUE(h == 1, "height not 1?");
  IS_TRUE(w == ww, "weight width mismatch");
  IS_TRUE(wh == bw, "bias width mismatch");

  double* output = (double*)malloc(sizeof(double) * h * wh);
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < wh; j++) {
      double tmp = 0;
      for (int k = 0; k < ww; k++) {
        tmp += msg0[i * ww + k] * weight[j * ww + k];
      }
      output[i * wh + j] = tmp;
    }
  }
  for (int i = 0; i < bw; ++i) {
    output[i] += bias[i];
  }
  return output;
}

double* Average_pool_impl(double* msg0, int n, int c, int h, int w, int kh,
                          int kw, int sh, int sw, int pn, int pc, int ph,
                          int pw) {
  if (ph != 0 || pw != 0) {
    msg0 = Pad_tensor(msg0, n, c, h, w, pn, pc, ph, pw);
    h += ph * 2;
    w += pw * 2;
  }

  int     oh     = (h - kh) / sh + 1;
  int     ow     = (w - kw) / sw + 1;
  double  scale  = 1.0 / (double)(kh * kw);
  double* output = malloc(sizeof(double) * n * c * oh * ow);

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < c; ++j) {
      for (int k = 0; k < ow; ++k) {
        for (int l = 0; l < oh; ++l) {
          double sum = 0.0;
          for (int m = 0; m < kh; ++m) {
            for (int n = 0; n < kw; ++n) {
              int iy = k * kh + m;
              int ix = l * kw + n;
              sum += msg0[((i * c + j) * h + iy) * w + ix];
            }
          }
          output[((i * c + j) * oh + k) * ow + l] = sum * scale;
        }
      }
    }
  }

  if (ph != 0 || pw != 0) {
    free(msg0);
  }
  return output;
}

double* Global_average_pool_impl(double* msg0, int n, int c, int h, int w) {
  double* output = (double*)malloc(sizeof(double) * n * c);
  double  scale  = 1.0 / (double)(h * w);
  for (int i = 0; i < n * c; ++i) {
    double tmp = 0;
    for (int j = 0; j < h * w; ++j) {
      tmp += msg0[i * h * w + j];
    }
    output[i] = tmp * scale;
  }
  return output;
}
