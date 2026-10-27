//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief rt_key_path.c
//! Key bundle path management and key mode implementation for FHE keys.

#include "common/rt_runtime_config.h"

#include <stdlib.h>
#include <string.h>

//=============================================================================
// Key Mode
//=============================================================================

//! @brief Static storage for the current key mode.
//!
//! Default: RT_KEY_MODE_ONLINE (all keys generated on GPU).
static int g_key_mode = RT_KEY_MODE_ONLINE;

void Set_key_mode(int mode) {
  g_key_mode = mode;
}

int Get_key_mode(void) {
  return g_key_mode;
}

//=============================================================================
// Load Full Keys
//=============================================================================

//! @brief Static storage for the load_full_keys flag.
//!
//! Default: 0 (compute-only in PRELOAD mode — only load eval keys).
static int g_load_full_keys = 0;

void Set_load_full_keys(int value) {
  g_load_full_keys = value;
}

int Get_load_full_keys(void) {
  return g_load_full_keys;
}

//=============================================================================
// Enc Policy
//=============================================================================

//! @brief Static storage for the current enc policy.
//!
//! Default: RT_ENC_POLICY_LOAD_RAW (read weight file into memory, no encoding).
static int g_enc_policy = RT_ENC_POLICY_LOAD_RAW;

void Set_enc_policy(int policy) {
  g_enc_policy = policy;
}

int Get_enc_policy(void) {
  return g_enc_policy;
}

//=============================================================================
// Key Path
//=============================================================================

//! @brief Initialize key path from environment variable.
//!
//! Checks the ACE_KEY_PATH environment variable and sets the key path
//! if it is defined. This allows key path configuration without code
//! changes, useful for testing and deployment.
static void Init_key_path(void) {
  static int initialized = 0;
  if (!initialized) {
    const char* env_path = getenv("ACE_KEY_PATH");
    if (env_path != NULL && env_path[0] != '\0') {
      Set_key_path(env_path);
    }
    initialized = 1;
  }
}

//! @brief Static storage for the current key bundle path.
static char* g_key_path = NULL;

int Set_key_path(const char* path) {
  // Free existing path if any
  if (g_key_path != NULL) {
    free(g_key_path);
    g_key_path = NULL;
  }

  // Set new path if non-NULL and non-empty
  if (path != NULL && path[0] != '\0') {
    g_key_path = strdup(path);
    if (g_key_path == NULL) {
      return -1;  // Memory allocation failure
    }
  }

  return 0;
}

const char* _Get_stored_key_path(void) {
  // Lazy initialization from environment variable
  Init_key_path();
  return g_key_path;
}

void Clear_key_path(void) {
  if (g_key_path != NULL) {
    free(g_key_path);
    g_key_path = NULL;
  }
}

//! @brief Public Get_key_path() for static (non-shared) builds.
//!
//! When RTLIB_SUPPORT_DYNAMIC is not defined, rt_dynamic.c is not compiled,
//! so we provide Get_key_path() here as the sole definition. It simply
//! delegates to the stored key path (no callback chain in static builds).
//!
//! When RTLIB_SUPPORT_DYNAMIC IS defined (shared library build), rt_dynamic.c
//! provides Get_key_path() with callback resolution, and this definition is
//! excluded to avoid a multiple-definition conflict.
#ifndef RTLIB_SUPPORT_DYNAMIC
const char* Get_key_path(void) {
  return _Get_stored_key_path();
}
#endif