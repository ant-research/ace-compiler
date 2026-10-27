//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @file need_bts.h
//! @brief Common, backend-agnostic declarator for the one supply-side symbol
//!        shared by every backend context.
//!
//! `Need_bts()` tells the FheContext<Backend> constructor whether bootstrap
//! must be set up. It is **codegen-emitted** into each compiled kernel .so as
//! a normal C++-linkage `bool Need_bts()` (mangled `_Z8Need_btsv`), so at
//! dlopen time the loaded kernel .so provides it. Backends `#include` this
//! when they call `Need_bts()` during context construction.
//!
//! Linkage note: this declaration is deliberately NOT inside `extern "C"`.
//! The codegen-emitted definition is C++ linkage (the kernel .so exports
//! `_Z8Need_btsv`, not an unmangled `Need_bts`). Wrapping the declaration in
//! `extern "C"` would make the backend .so reference the unmangled `Need_bts`
//! symbol, which the kernel .so does not export → `undefined symbol: Need_bts`
//! at dlopen time. Keeping it C++ matches the codegen emission. (This is the
//! form master's phantom c_api.h used; unifying the per-backend declarations
//! earlier accidentally pulled it into extern "C".)
//!
//! History: this was declared per-backend in `fhe/vendor/<backend>/c_api.h`,
//! but the declaration is identical across backends and not backend-specific,
//! so it now lives once here under the `fhe/` common layer. The per-backend
//! `c_api.h` files (which had shrunk to just this one declaration) are deleted.

#ifndef FHE_NEED_BTS_H
#define FHE_NEED_BTS_H

#ifdef __cplusplus
extern "C++"
{
#endif

    //! @brief Whether the current compiled kernel requires bootstrap setup.
    //!        Defined (C++ linkage) by the compiled kernel .so; defined weak by
    //!        the legacy rtlib baseline (`rt_api.h`) for compile-out-of-kernel use.
    bool Need_bts();

#ifdef __cplusplus
}  // extern "C++"
#endif

#endif  // FHE_NEED_BTS_H