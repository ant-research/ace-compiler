//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief vendor_types.h
//! Common type definitions shared by all FHE backend vendor traits.
//!
//! This header provides:
//!   - PtAccessBase / PtPopulateBase: common plaintext serialization fields
//!   - Evaluator<EncoderT, EvalT>: common evaluator template
//!   - Crypto<EncryptorT, DecryptorT>: per-session encrypt/decrypt
//!
//! Each backend's VendorTraits extends these bases with vendor-specific
//! fields (e.g., ACE's parms_id/is_ntt_form, Phantom's chain_index,
//! ACE's eval_ptr/ctx for BTS dispatch).

#ifndef FHERT_VENDOR_TYPES_H
#define FHERT_VENDOR_TYPES_H

#include <cstddef>
#include <cstdint>
#include <memory>

namespace fhert
{

//=============================================================================
// PtAccessBase — common plaintext accessor fields
//=============================================================================

/// Common base for plaintext access (serialization: GPU → host).
///
/// Backend-specific subclasses add level identification and layout flags:
///   - ACE:     parms_id (parms_id_type) + is_ntt_form (bool)
///   - Phantom: chain_index (uint64_t)
struct PtAccessBase
{
    const void* gpu_data = nullptr;    ///< Device polynomial data pointer
    uint64_t poly_modulus_degree = 0;  ///< N (ring dimension)
    uint64_t coeff_modulus_size = 0;   ///< L (number of RNS moduli)
    double scale = 0.0;                ///< Encoding scale
    size_t data_size = 0;              ///< N * L * sizeof(uint64_t)
};

//=============================================================================
// PtPopulateBase — common plaintext populate fields
//=============================================================================

/// Common base for plaintext populate (deserialization: host → GPU).
///
/// Backend-specific subclasses add the same level/layout extensions as
/// PtAccessBase (parms_id+is_ntt_form for ACE, chain_index for Phantom).
struct PtPopulateBase
{
    const void* host_data = nullptr;   ///< Host buffer to copy to GPU
    size_t data_size = 0;              ///< Size of host_data in bytes
    uint64_t poly_modulus_degree = 0;  ///< N (ring dimension)
    uint64_t coeff_modulus_size = 0;   ///< L (number of RNS moduli)
    double scale = 0.0;                ///< Encoding scale
};

//=============================================================================
// Evaluator<EncoderT, EvalT> — common evaluator template
//=============================================================================

/// Common evaluator template — owns an encoder and an evaluator.
///
/// Each backend wraps its vendor-specific encoder and evaluator types.
/// Backend-specific extensions (e.g., ACE's BTS dispatch with eval_ptr/ctx)
/// are added by inheriting from this template and adding fields/methods.
///
/// Usage:
///   - Phantom: using Evaluator = fhert::Evaluator<PhantomCKKSEncoder, CKKSEvaluator>;
///   - ACE:     struct Evaluator : fhert::Evaluator<CKKSEncoder, acelib::Evaluator> { ... };
template <typename EncoderT, typename EvalT> struct Evaluator
{
    std::unique_ptr<EncoderT> encoder;  ///< Owning encoder
    std::unique_ptr<EvalT> eval;        ///< Owning evaluator

    Evaluator() = default;
    ~Evaluator() = default;

    // Non-copyable, movable
    Evaluator(const Evaluator&) = delete;
    Evaluator& operator=(const Evaluator&) = delete;
    Evaluator(Evaluator&&) = default;
    Evaluator& operator=(Evaluator&&) = default;
};

//=============================================================================
// Crypto<EncryptorT, DecryptorT> — per-session encrypt/decrypt
//=============================================================================

/// Per-session cryptographic operations — encrypt and decrypt.
///
/// This is the sole FHE capability that requires user-private keys (SK/PK).
/// All other HE operations (Add, Mul, Rotate, Relin, Bootstrap) are
/// key-free homomorphic computations on ciphertexts and live in the
/// shared Evaluator.
///
/// Ownership model:
///   - unique_ptr<EncryptorT> / unique_ptr<DecryptorT> — owns the objects.
///   - On switch_session, old encryptor/decryptor are destroyed and
///     new ones are created for the target session.
///   - In compute-only mode (PRELOAD), both are nullptr.
///
/// Vendor adaptation:
///   - Phantom: using Crypto = fhert::Crypto<phantom::Encryptor,
///                                           phantom::Decryptor>;
///   - ACE:     using Crypto = fhert::Crypto<acelib::host::Encryptor,
///                                           acelib::host::Decryptor>;
template <typename EncryptorT, typename DecryptorT> struct Crypto
{
    std::unique_ptr<EncryptorT> encryptor;
    std::unique_ptr<DecryptorT> decryptor;

    Crypto() = default;
    ~Crypto() = default;

    // Non-copyable, movable
    Crypto(const Crypto&) = delete;
    Crypto& operator=(const Crypto&) = delete;
    Crypto(Crypto&&) = default;
    Crypto& operator=(Crypto&&) = default;

    bool can_encrypt() const { return encryptor != nullptr; }
    bool can_decrypt() const { return decryptor != nullptr; }
};

}  // namespace fhert

#endif  // FHERT_VENDOR_TYPES_H
