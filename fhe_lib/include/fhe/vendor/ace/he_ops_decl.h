//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief he_ops_decl.h
//! ACE HE-operation dispatch hooks, separated out of vendor_traits.h.
//!
//! !!! READ BEFORE EDITING !!!
//! This file is `#include`d DIRECTLY INSIDE `struct ace::VendorTraits { ... }`
//! (see vendor_traits.h). It is a preprocessor fragment, NOT a normal header:
//!   - NO include guard (#pragma once / include guard).
//!   - NO `namespace ace { ... }`, NO `struct VendorTraits { ... }` wrapper.
//!   - The methods below remain MEMBERS of `ace::VendorTraits` after
//!     preprocessing.
//!
//! Why a fragment and not a separate class? This runtime dlopens JIT-compiled
//! kernel .so alongside libFHErt_ace.so; both instantiate the inline HE ops.
//! Keeping the ops as members of VendorTraits preserves their exact ODR /
//! Itanium mangling (e.g. `_ZN3ace12VendorTraits...`). Moving them into a
//! separate class/template changes the defining entity and has, in this JIT
//! cross-DSO setting, been observed to SIGSEGV on a second backend context.
//! Do NOT wrap these in a different type — translate the bodies only.
//!
//! `get_active_eval` was previously defined here but had zero callers
//! repo-wide; dropped as dead code.

//=========================================================================
// HE operation dispatch hooks
//=========================================================================
// These static methods abstract the vendor-specific evaluator API so that
// FheContext<Backend> can implement HE operations generically in the header.
// Each hook is a thin delegation to the ACE evaluator API.

/// Release ciphertext GPU memory (no-op for ACE — RAII managed).
static void free_cipher(Ciphertext& ct)
{
    (void)ct;
}

/// Release plaintext GPU memory (no-op for ACE — RAII managed).
static void free_plain(Plaintext& pt)
{
    (void)pt;
}

/// Get the level identifier of a ciphertext.
static LevelType ciphertext_level(const Ciphertext& ct)
{
    return ct.parms_id();
}

/// Equalize the levels of two ciphertexts (in-place mod-switch the higher one).
/// Handles ACE_LIBRARY_32 scale adjustment under ADJUST_LEVEL_REDUCE_ERROR.
static void equal_level_ciph(Evaluator& eval, Ciphertext& a, Ciphertext& b);

/// Equalize the levels of a ciphertext and a plaintext.
static void equal_level_plain(Evaluator& eval, Ciphertext& ct, Plaintext& pt);

/// Encode a scalar double into a device plaintext for Add_const / Mul_const.
static void encode_const(Evaluator& eval, Plaintext& pt, double value, const Ciphertext& ref);

/// Convert ciphertext to host representation for decryption.
static HostCiphertext to_host_ciphertext(Ciphertext& ct, Context& ctx)
{
    return ct.to(ctx);
}

/// Convert plaintext to host representation for decoding.
static HostPlaintext to_host_plaintext_for_decode(Plaintext& pt, Context& ctx)
{
    return pt.to(ctx);
}

/// Decode a host plaintext into a double vector.
static void decode_from_host(Evaluator& eval, HostPlaintext& pt, std::vector<double>& vec)
{
    eval.encoder->decode(pt, vec);
}

/// Add two ciphertexts: res = a + b.
static void Add(Evaluator& eval, Ciphertext& res, const Ciphertext& a, const Ciphertext& b)
{
    eval.active_eval()->add(a, b, res);
}

/// Add a plaintext to a ciphertext: res = ct + pt.
/// Handles inplace when &res == &ct.
static void Add(Evaluator& eval, Ciphertext& res, const Ciphertext& ct, const Plaintext& pt)
{
    if (&res == &ct) {
        eval.active_eval()->add_plain_inplace(res, pt);
    }
    else {
        eval.active_eval()->add_plain(ct, pt, res);
    }
}

/// Multiply two ciphertexts: res = a * b.
static void Mul(Evaluator& eval, Ciphertext& res, const Ciphertext& a, const Ciphertext& b)
{
    eval.active_eval()->multiply(a, b, res);
}

/// Multiply a plaintext with a ciphertext: res = ct * pt.
/// Handles inplace when &res == &ct.
static void Mul(Evaluator& eval, Ciphertext& res, const Ciphertext& ct, const Plaintext& pt)
{
    if (&res == &ct) {
        eval.active_eval()->multiply_plain_inplace(res, pt);
    }
    else {
        eval.active_eval()->multiply_plain(ct, pt, res);
    }
}

/// Rotate ciphertext by step positions. Keys are internal to evaluator (gk ignored).
static void Rotate(Evaluator& eval, GaloisKey& /*gk*/, Ciphertext& res, const Ciphertext& ct, int step)
{
    if (&res == &ct) {
        eval.active_eval()->rotate_vector_inplace(res, step);
    }
    else {
        eval.active_eval()->rotate_vector(const_cast<Ciphertext&>(ct), step, res);
    }
}

/// Relinearize ciphertext. Keys are internal to evaluator (rk ignored).
static void Relinearize(Evaluator& eval, RelinKey& /*rk*/, Ciphertext& res, const Ciphertext& ct)
{
    if (&res == &ct) {
        eval.active_eval()->relinearize_inplace(res);
    }
    else {
        eval.active_eval()->relinearize(ct, res);
    }
}

/// Rescale ciphertext to the next level.
static void Rescale_next(Evaluator& eval, Ciphertext& res, const Ciphertext& ct)
{
    if (&res == &ct) {
        eval.active_eval()->rescale_to_next_inplace(res);
    }
    else {
        eval.active_eval()->rescale_to_next(ct, res);
    }
}

/// Mod-switch ciphertext to the next level.
static void Mod_switch_next(Evaluator& eval, Ciphertext& res, const Ciphertext& ct)
{
    if (&res == &ct) {
        eval.active_eval()->mod_switch_to_next_inplace(res);
    }
    else {
        eval.active_eval()->mod_switch_to_next(ct, res);
    }
}
