//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief rt_runtime_config.h
//! Key bundle path management and key mode for pre-generated FHE keys.
//!
//! When a key path is set via Set_key_path(), the FHE runtime can load
//! pre-generated keys from the specified directory instead of generating
//! new keys during Prepare_context(). This enables key pre-generation
//! workflows (KMS) and client-server deployment patterns.
//!
//! The key mode (Set_key_mode/Get_key_mode) controls which keys are
//! loaded/generated and whether encryption/decryption is available
//! at runtime.

#ifndef RTLIB_COMMON_RT_KEY_PATH_H
#define RTLIB_COMMON_RT_KEY_PATH_H

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Key Mode
//=============================================================================

//! @brief Key mode enum — controls key loading/generation behavior.
//!
//! - RT_KEY_MODE_ONLINE        (0): Generate all keys on GPU (default)
//! - RT_KEY_MODE_PRELOAD       (1): Load eval keys from disk at init (compute-only)
//! - RT_KEY_MODE_ON_DEMAND       (2): Load eval keys on-demand at runtime (reserved)
//! - RT_KEY_MODE_PRELOAD_FULL  (3): Load all keys from disk (can encrypt/decrypt)
//! - RT_KEY_MODE_ON_DEMAND_FULL  (4): Load all keys on-demand (reserved)
enum RtKeyMode {
    RT_KEY_MODE_ONLINE         = 0,
    RT_KEY_MODE_PRELOAD        = 1,
    RT_KEY_MODE_ON_DEMAND      = 2,
    RT_KEY_MODE_PRELOAD_FULL   = 3,
    RT_KEY_MODE_ON_DEMAND_FULL = 4,
};

//! @brief Set the key mode.
//!
//! Must be called before Prepare_context(). The mode controls which keys
//! are loaded/generated and whether Prepare_input/Handle_output are available.
//!
//! @param mode The key mode (RtKeyMode enum value).
void Set_key_mode(int mode);

//! @brief Get the current key mode.
//!
//! @return The current key mode (default: RT_KEY_MODE_ONLINE).
int Get_key_mode(void);

//=============================================================================
// Load Full Keys
//=============================================================================

//! @brief Set whether to load full keys (SK+PK+RLK+GK) when in PRELOAD mode.
//!
//! DEPRECATED: Use Set_key_mode(RT_KEY_MODE_PRELOAD_FULL) instead.
//! Kept for backward compatibility — Python code that calls this will
//! be migrated to use the new KeyMode values.
//!
//! When load_full_keys is 0 (default) and mode is PRELOAD, only eval keys
//! (RLK+GK) are loaded — compute-only mode, no encrypt/decrypt.
//! When load_full_keys is 1, all keys including SK+PK are loaded — full
//! pipeline mode with encrypt/decrypt capability.
//!
//! Ignored when mode is ONLINE (keys are always generated).
//!
//! @param value Non-zero to load full keys, zero for compute-only (default).
void Set_load_full_keys(int value);

//! @brief Get the current load_full_keys setting.
//!
//! DEPRECATED: Use Get_key_mode() and check for PRELOAD_FULL instead.
//!
//! @return Non-zero if full keys will be loaded, zero otherwise (default).
int Get_load_full_keys(void);

//=============================================================================
// Enc Policy — when weights are loaded / encoded
//=============================================================================

//! @brief Enc policy enum — controls weight file loading/encoding at init.
//!
//! - RT_ENC_POLICY_LOAD_RAW       (0): Read whole weight file into memory at
//!                                     init, no encoding (default). Whatever the
//!                                     file format (msg or enc), loaded as-is.
//! - RT_ENC_POLICY_LOAD_ENCODED   (1): Read file into memory AND, if raw (msg),
//!                                     encode every weight into an in-memory
//!                                     cache at init. No-op for already-encoded
//!                                     (enc) files.
//! - RT_ENC_POLICY_LOAD_ON_DEMAND (2): Do not read file at init; read per weight
//!                                     at inference (reserved — not implemented).
enum RtEncPolicy {
    RT_ENC_POLICY_LOAD_RAW       = 0,
    RT_ENC_POLICY_LOAD_ENCODED   = 1,
    RT_ENC_POLICY_LOAD_ON_DEMAND = 2,
};

//! @brief Set the enc policy.
//!
//! Must be called before Prepare_context(). Controls how the weight file is
//! loaded/encoded at context initialization.
//!
//! @param policy The enc policy (RtEncPolicy enum value).
void Set_enc_policy(int policy);

//! @brief Get the current enc policy.
//!
//! @return The current enc policy (default: RT_ENC_POLICY_LOAD_RAW).
int Get_enc_policy(void);

//=============================================================================
// Key Path
//=============================================================================

//! @brief Set the key bundle path for pre-generated keys.
//!
//! The path should point to a directory containing a metadata.json file
//! and key subdirectories (global/ for encryption keys, model/ for
//! evaluation keys). Must be called before Prepare_context().
//!
//! @param path Directory path to the key bundle. Pass NULL or empty
//!             string to clear the key path.
//! @return 0 on success, -1 on error (e.g., memory allocation failure)
int Set_key_path(const char* path);

//! @brief Get the stored key bundle path (internal helper).
//!
//! This returns the key path stored by Set_key_path() or initialized
//! from the ACE_KEY_PATH environment variable. It does NOT check
//! registered callbacks (use the Get_key_path() wrapper in rt_dynamic.c
//! for the full resolution chain).
//!
//! @return The current key bundle path, or NULL if not set.
//!         The returned pointer is valid until the next Set_key_path()
//!         or Clear_key_path() call.
const char* _Get_stored_key_path(void);

//! @brief Clear the key bundle path.
//!
//! After calling this, Get_key_path() will return NULL and
//! Prepare_context() will generate new keys as usual.
void Clear_key_path(void);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_COMMON_RT_KEY_PATH_H