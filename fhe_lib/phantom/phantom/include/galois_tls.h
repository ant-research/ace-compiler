// -*-c++-*-
//=============================================================================
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Mode B-P per-thread PhantomGaloisTool override — declaration only.
// Cross-DSO-safe TLS accessors for the per-session galois tool copy,
// mirroring the tl_current_stream discipline (see util/globals.h): the
// thread_local is read ONLY inside phantom .so (context.cu) via these
// function calls, never inlined into the compiled kernel .so.
//
// Included only where PhantomGaloisTool is already needed (evaluate.cuh,
// context.cu, fhe_lib context.cu) — galois.cuh is included here for the
// complete type. Definition in src/context.cu.
//=============================================================================
#pragma once

#include "galois.cuh"

namespace phantom::util::global_variables {
    // Mode B-P: per-thread override of the galois tool. nullptr = use the
    // shared PhantomContext::key_galois_tool_ (built on default_stream during
    // primary init). Non-null = this session's own tool copy, built on the
    // session stream (~18 MiB). Lets apply_galois/rotate/conjugate read a
    // stream-ordered tool WITHOUT regenerating the 22 GiB galois key set (the
    // regen workaround) and WITHOUT resetting the shared tool (use-after-free).
    extern thread_local ::PhantomGaloisTool *tl_current_galois_tool;

    // Return the calling thread's galois-tool override, or nullptr (use shared).
    ::PhantomGaloisTool *get_thread_galois_tool();

    // Set the calling thread's galois-tool override (Mode B-P). Called by fhe_lib
    // Activate_session_for_this_thread; pass nullptr to revert to the shared tool.
    void set_thread_galois_tool(::PhantomGaloisTool *t);
}
