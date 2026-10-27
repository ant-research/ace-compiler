//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief fhe_config.h
//! FHE instance configuration — KeyMode, EncPolicy, FheParams.
//!
//! All three are defined here so that session.h, key_mgr.h, and context.h
//! can reference them without circular dependencies.  KeyMode was previously
//! in key_mgr.h but moved here to allow session.h to use it without
//! depending on key_mgr.h (and its transitive spdlog include chain).

#ifndef FHERT_FHE_CONFIG_H
#define FHERT_FHE_CONFIG_H

#include <cstdint>

namespace fhert
{

//=============================================================================
// KeyMode — how keys are obtained at runtime
//=============================================================================

//! @brief Key mode — controls how keys are obtained at runtime.
//!
//! - ONLINE:        Generate all keys (SK+PK+RLK+GK) on GPU (default)
//! - PRELOAD:       Load eval keys (RLK+GK) from disk at init (production
//!                  default, compute-only — no SK/PK for encrypt/decrypt).
//! - PRELOAD_FULL:  Load all keys (SK+PK+RLK+GK) from disk at init
//!                  (debug/verification — can encrypt and decrypt).
//! - ON_DEMAND:       Load eval keys on-demand at runtime (reserved).
//! - ON_DEMAND_FULL:  Load all keys on-demand at runtime (reserved).
enum class KeyMode
{
    ONLINE = 0,          //!< Generate all keys on GPU
    PRELOAD = 1,         //!< Load eval keys from disk (compute-only)
    ON_DEMAND = 2,       //!< Load eval keys on-demand (reserved)
    PRELOAD_FULL = 3,    //!< Load all keys from disk (can verify)
    ON_DEMAND_FULL = 4,  //!< Load all keys on-demand (reserved)
};

//=============================================================================
// EncPolicy — when plaintext weights are loaded / encoded
//=============================================================================

//! @brief Encoding policy — controls what happens to the weight file at
//! context initialization. Inference-time handling is fixed: an already
//! encoded weight (DE_PLAINTEXT) is used directly; a raw float weight
//! (DE_MSG_F32) is encoded on first use. EncPolicy only chooses *when*
//! the file is read and whether raw weights are pre-encoded at init.
//!
//! The weight file on disk may be either raw float data (DE_MSG_F32) or
//! pre-encoded plaintext (DE_PLAINTEXT, e.g. produced by an offline
//! encoding tool that converts msg→enc and updates the model JSON).
//! EncPolicy applies to either format.
//!
//! - LOAD_RAW:       Read the whole weight file into process memory at
//!                   init, no encoding. Whatever the file format is
//!                   (msg or enc), it is loaded as-is; inference then
//!                   uses encoded data directly or encodes raw data on
//!                   first use. (default)
//! - LOAD_ENCODED:   Read the file into memory at init AND, if the file
//!                   is raw (DE_MSG_F32), encode every weight into an
//!                   in-memory cache now. Inference then uses the cached
//!                   encoded weights with zero encoding cost — at the
//!                   cost of init-time encoding work and higher memory.
//!                   If the file is already DE_PLAINTEXT, this is
//!                   equivalent to LOAD_RAW.
//! - LOAD_ON_DEMAND: Do NOT read the file at init (only keep its fd).
//!                   Each weight is read from the file on demand during
//!                   inference, then handled per its format. Lowest
//!                   init/memory footprint, at the cost of per-inference
//!                   file IO. (reserved — not yet implemented)
enum class EncPolicy
{
    LOAD_RAW = 0,
    LOAD_ENCODED = 1,
    LOAD_ON_DEMAND = 2,
};

//=============================================================================
// FheParams — FHE scheme parameters
//=============================================================================

//! @brief FHE scheme parameters — populated during context initialization
//! from the compiler's CKKS_PARAMS.
//!
//! Grouped into a struct so that FheContext exposes a single `params()`
//! accessor instead of individual scalar getters, and so new parameters
//! can be added without changing the FheContext interface.
struct FheParams
{
    uint64_t scaling_mod_size = 0;  ///< log2 of scaling factor per level
    uint64_t slot_count = 0;        ///< N/2 (number of plaintext slots)
    uint64_t num_prime_parts = 0;   ///< number of prime modulus parts
};

}  // namespace fhert

#endif  // FHERT_FHE_CONFIG_H
