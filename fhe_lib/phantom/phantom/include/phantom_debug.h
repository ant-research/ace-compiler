#pragma once

//============================================================================
// Phantom debug logging macros
//
// PHANTOM_DEBUG:  Master switch. 1 in Debug builds; 0 in Release (NDEBUG).
//
// PHANTOM_LOG(msg):     Prints msg + newline.  No-op in Release.
// PHANTOM_LOG_RAW(msg): Prints msg without newline. No-op in Release.
//
// Usage:
//   PHANTOM_LOG("Modulus Raising...");
//   PHANTOM_LOG("Coefftoslot..." << 5);
//   PHANTOM_LOG_RAW("Modulus Raising... ");
//   PHANTOM_LOG_RAW(timer.duration<milliseconds>() << "ms");
//
//============================================================================

#include <iostream>

#ifdef NDEBUG
  #define PHANTOM_DEBUG 0
#else
  #define PHANTOM_DEBUG 1
#endif

#if PHANTOM_DEBUG
  #define PHANTOM_LOG(x)     std::cout << x << std::endl
  #define PHANTOM_LOG_RAW(x) std::cout << x
#else
  #define PHANTOM_LOG(x)     ((void)0)
  #define PHANTOM_LOG_RAW(x) ((void)0)
#endif