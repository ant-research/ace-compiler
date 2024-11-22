//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "common/trace.h"

#include <stdlib.h>
#include <string.h>

#include "common/rt_env.h"

#define DEFAULT_TRACE_FILE_NAME "fhe_trace.t"

static FILE* T_file_internal = NULL;
static bool  Trace_on        = false;
const char*  Log_file_intel  = NULL;

void Set_trace_file(char* filename) {
  if (T_file_internal != NULL) {
    fclose(T_file_internal);
  }
  if (filename != NULL) {
    T_file_internal = fopen(filename, "w");
  }
}

FILE* Get_trace_file(void) {
  if (T_file_internal == NULL) {
    const char* fname = getenv(ENV_RTLIB_TRACE_FILE);
    if (fname != NULL && strcmp(fname, "stdout") == 0) {
      T_file_internal = stdout;
    } else if (fname != NULL && strcmp(fname, "stderr") == 0) {
      T_file_internal = stderr;
    } else {
      if (fname == NULL) {
        fname = DEFAULT_TRACE_FILE_NAME;
      }
      T_file_internal = fopen(fname, "w");
      if (T_file_internal == NULL) {
        T_file_internal = stderr;
      }
    }
  }
  return T_file_internal;
}

void Close_trace_file(void) {
  if (T_file_internal != NULL && T_file_internal != stdout &&
      T_file_internal != stderr) {
    fclose(T_file_internal);
    T_file_internal = NULL;
  }
}

void Set_trace_on(bool v) { Trace_on = v; }

bool Is_trace_on(void) { return Trace_on; }
