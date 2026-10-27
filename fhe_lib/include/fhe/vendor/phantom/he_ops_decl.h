//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief he_ops_decl.h
//! Phantom HE-operation dispatch hooks, separated out of vendor_traits.h.
//!
//! !!! READ BEFORE EDITING !!!
//! This file is `#include`d DIRECTLY INSIDE `struct phantom::VendorTraits { ... }`
//! (see vendor_traits.h). It is a preprocessor fragment, NOT a normal header:
//!   - NO include guard (#pragma once / include guard).
//!   - NO `namespace phantom { ... }`, NO `struct VendorTraits { ... }` wrapper.
//!   - The methods below remain MEMBERS of `phantom::VendorTraits` after
//!     preprocessing.
//!
//! Why a fragment and not a separate class? This runtime dlopens JIT-compiled
//! kernel .so alongside libFHErt_phantom.so; both instantiate the inline HE ops.
//! Keeping the ops as members of VendorTraits preserves their exact ODR /
//! Itanium mangling (e.g. `_ZN7phantom12VendorTraits...`). Moving them into a
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
// Each hook is a thin one-liner that delegates to the Phantom evaluator API.

/// Release ciphertext GPU memory.
static void free_cipher(Ciphertext& ct)
{
    if (ct.size() > 0)
        ct.release();
}

/// Release plaintext GPU memory.
static void free_plain(Plaintext& pt)
{
    pt.release();
}

/// Get the level identifier of a ciphertext.
static LevelType ciphertext_level(const Ciphertext& ct)
{
    return ct.coeff_modulus_size();
}

/// Equalize the levels of two ciphertexts (in-place mod-switch the higher one).
static void equal_level_ciph(Evaluator& eval, Ciphertext& a, Ciphertext& b);

/// Equalize the levels of a ciphertext and a plaintext.
static void equal_level_plain(Evaluator& eval, Ciphertext& ct, Plaintext& pt);

/// Encode a scalar double into a plaintext for Add_const / Mul_const.
static void encode_const(Evaluator& eval, Plaintext& pt, double value, const Ciphertext& ref);

/// Convert ciphertext to host representation (identity for Phantom).
static HostCiphertext to_host_ciphertext(Ciphertext& ct, Context& /*ctx*/)
{
    return ct;  // Phantom operates on GPU types directly
}

/// Convert plaintext to host representation for decoding (identity for Phantom).
/// Returns a mutable reference since Phantom's Encoder::decode takes non-const ref.
static HostPlaintext& to_host_plaintext_for_decode(Plaintext& pt, Context& /*ctx*/)
{
    return pt;  // Phantom operates on GPU types directly
}

/// Decode a host plaintext into a double vector.
static void decode_from_host(Evaluator& eval, HostPlaintext& pt, std::vector<double>& vec)
{
    eval.eval->encoder.decode(pt, vec);
}

// NOTE: CKKSEvaluator has its own .encoder member (distinct from the
// Evaluator template's .encoder unique_ptr).  The decode/encode operations
// in Phantom go through eval->encoder (the one inside CKKSEvaluator).

/// Add two ciphertexts: res = a + b.
static void Add(Evaluator& eval, Ciphertext& res, const Ciphertext& a, const Ciphertext& b)
{
    eval.eval->evaluator.add(a, b, res);
}

/// Add a plaintext to a ciphertext: res = ct + pt.
/// Handles inplace when &res == &ct.
static void Add(Evaluator& eval, Ciphertext& res, const Ciphertext& ct, const Plaintext& pt)
{
    if (&res == &ct) {
        eval.eval->evaluator.add_plain_inplace(res, pt);
    }
    else {
        eval.eval->evaluator.add_plain(ct, pt, res);
    }
}

/// Multiply two ciphertexts: res = a * b.
static void Mul(Evaluator& eval, Ciphertext& res, const Ciphertext& a, const Ciphertext& b)
{
    eval.eval->evaluator.multiply(a, b, res);
}

/// Multiply a plaintext with a ciphertext: res = ct * pt.
/// Handles inplace when &res == &ct.
static void Mul(Evaluator& eval, Ciphertext& res, const Ciphertext& ct, const Plaintext& pt)
{
    if (&res == &ct) {
        eval.eval->evaluator.multiply_plain_inplace(res, pt);
    }
    else {
        eval.eval->evaluator.multiply_plain(ct, pt, res);
    }
}

/// Rotate ciphertext by step positions. GaloisKey passed by mutable ref
/// (Phantom evaluator takes non-const ref to galois keys).
static void Rotate(Evaluator& eval, GaloisKey& gk, Ciphertext& res, const Ciphertext& ct, int step)
{
    if (&res == &ct) {
        eval.eval->evaluator.rotate_vector_inplace(res, step, gk);
    }
    else {
        eval.eval->evaluator.rotate_vector(const_cast<Ciphertext&>(ct), step, gk, res);
    }
}

/// Relinearize ciphertext. RelinKey passed by mutable ref.
static void Relinearize(Evaluator& eval, RelinKey& rk, Ciphertext& res, const Ciphertext& ct)
{
    if (&res == &ct) {
        eval.eval->evaluator.relinearize_inplace(res, rk);
    }
    else {
        eval.eval->evaluator.relinearize(ct, rk, res);
    }
}

/// Rescale ciphertext to the next level.
static void Rescale_next(Evaluator& eval, Ciphertext& res, const Ciphertext& ct)
{
    if (&res == &ct) {
        eval.eval->evaluator.rescale_to_next_inplace(res);
    }
    else {
        eval.eval->evaluator.rescale_to_next(ct, res);
    }
}

/// Mod-switch ciphertext to the next level.
static void Mod_switch_next(Evaluator& eval, Ciphertext& res, const Ciphertext& ct)
{
    if (&res == &ct) {
        eval.eval->evaluator.mod_switch_to_next_inplace(res);
    }
    else {
        // Phantom: copy then switch one level down
        res = ct;
        eval.eval->evaluator.mod_switch_to_next_inplace(res);
    }
}
