//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "rt_phantom/rt_phantom.h"
#include "common/rt_api.h"
#include "common/rtlib_timing.h"
#include "common/io_api.h"
#include "context.h"

void Prepare_context() {
    Init_rtlib_timing();
    Io_init();

    // Trigger singleton creation with lazy initialization
    (void)PHANTOM_CONTEXT::Context();
}

void Finalize_context() {
    // Singleton auto-cleans up
    Io_fini();
}

void Prepare_input(TENSOR *input, const char *name) {
    PHANTOM_CONTEXT::Context().Prepare_input(input, name);
}

double *Handle_output(const char *name) {
    return PHANTOM_CONTEXT::Context().Handle_output(name, 0);
}