//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief vendor_traits_concept.h
//! Compile-time interface checks for VendorTraits types.
//!
//! Each FHE backend must provide a VendorTraits struct that satisfies
//! this interface. The checks verify that all required type aliases
//! are present, giving immediate compile-time errors when adding a
//! new backend.
//!
//! Note: Static method checks are NOT included here because many
//! methods are overloaded (e.g., is_key_generated) or have
//! backend-specific signatures (e.g., generate_keys, upload_keys_to_evaluator),
//! making SFINAE detection impractical. Type alias checks alone catch
//! the vast majority of integration errors.

#ifndef FHERT_VENDOR_TRAITS_CONCEPT_H
#define FHERT_VENDOR_TRAITS_CONCEPT_H

#include <type_traits>

namespace fhert
{
namespace traits_check
{

//=============================================================================
// Type alias checks via SFINAE
//=============================================================================

#define FHERT_CHECK_TYPE_ALIAS(NAME)                                                                                   \
    template <typename T, typename = void> struct has_##NAME : std::false_type                                         \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename T> struct has_##NAME<T, std::void_t<typename T::NAME>> : std::true_type                         \
    {                                                                                                                  \
    }

FHERT_CHECK_TYPE_ALIAS(SecretKey);
FHERT_CHECK_TYPE_ALIAS(PublicKey);
FHERT_CHECK_TYPE_ALIAS(RelinKey);
FHERT_CHECK_TYPE_ALIAS(GaloisKey);
FHERT_CHECK_TYPE_ALIAS(Context);
FHERT_CHECK_TYPE_ALIAS(Plaintext);
FHERT_CHECK_TYPE_ALIAS(Ciphertext);
FHERT_CHECK_TYPE_ALIAS(ScaleType);
FHERT_CHECK_TYPE_ALIAS(LevelType);
FHERT_CHECK_TYPE_ALIAS(Crypto);
FHERT_CHECK_TYPE_ALIAS(Evaluator);
FHERT_CHECK_TYPE_ALIAS(BtsMgr);
FHERT_CHECK_TYPE_ALIAS(SecretKeyAccess);
FHERT_CHECK_TYPE_ALIAS(PublicKeyAccess);
FHERT_CHECK_TYPE_ALIAS(RelinKeyAccess);
FHERT_CHECK_TYPE_ALIAS(GaloisKeyAccess);
FHERT_CHECK_TYPE_ALIAS(SecretKeyPopulate);
FHERT_CHECK_TYPE_ALIAS(PublicKeyPopulate);
FHERT_CHECK_TYPE_ALIAS(RelinKeyPopulate);
FHERT_CHECK_TYPE_ALIAS(GaloisKeyPopulate);
FHERT_CHECK_TYPE_ALIAS(PtAccess);
FHERT_CHECK_TYPE_ALIAS(PtPopulate);
FHERT_CHECK_TYPE_ALIAS(HostCiphertext);
FHERT_CHECK_TYPE_ALIAS(HostPlaintext);

#undef FHERT_CHECK_TYPE_ALIAS

//=============================================================================
// Composite check — type aliases only
//=============================================================================

template <typename T>
constexpr bool is_valid_vendor_traits_v =
    has_SecretKey<T>::value && has_PublicKey<T>::value && has_RelinKey<T>::value && has_GaloisKey<T>::value &&
    has_Context<T>::value && has_Plaintext<T>::value && has_Ciphertext<T>::value && has_ScaleType<T>::value &&
    has_LevelType<T>::value && has_Crypto<T>::value && has_Evaluator<T>::value && has_BtsMgr<T>::value &&
    has_SecretKeyAccess<T>::value && has_PublicKeyAccess<T>::value && has_RelinKeyAccess<T>::value &&
    has_GaloisKeyAccess<T>::value && has_SecretKeyPopulate<T>::value && has_PublicKeyPopulate<T>::value &&
    has_RelinKeyPopulate<T>::value && has_GaloisKeyPopulate<T>::value && has_PtAccess<T>::value &&
    has_PtPopulate<T>::value && has_HostCiphertext<T>::value && has_HostPlaintext<T>::value;

}  // namespace traits_check
}  // namespace fhert

#endif  // FHERT_VENDOR_TRAITS_CONCEPT_H
