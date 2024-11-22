//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_DEBUG_H
#define AIR_UTIL_DEBUG_H

#include "air/util/error.h"
#include "air/util/messg.h"

extern void Abort_location();
extern void Debug_location();

extern const MSG_DESC User_msg[];

#define AIR_STATIC_ASSERT(cond) static_assert((cond))

#if !defined(NDEBUG)

#define AIR_DEBUG_FLAG(x) bool dbg_flag = x;

//! @brief air debugging macro prints value on cout
#define AIR_DEBUG(...) Templ_print(std::cout, __VA_ARGS__);

//! @brief air traceing macro prints value to trace file or cout
#define AIR_TRACE(tf, ...)                \
  {                                       \
    const char* file = __FILE__;          \
    int         line = __LINE__;          \
    Templ_print(tf.Tfile(), __VA_ARGS__); \
  }

//! @brief air assert macro prints value on cout & cerr
#define AIR_ASSERT_MSG(Conditional_stmt, ...)    \
  {                                              \
    if (!(Conditional_stmt)) {                   \
      Assert_hdr(std::cerr, __FILE__, __LINE__); \
      Templ_print(std::cerr, __VA_ARGS__);       \
      Abort_location();                          \
    }                                            \
  }

//! @brief air assert macro prints value on cout & cerr
#define AIR_ASSERT(Conditional_stmt) \
  AIR_ASSERT_MSG(Conditional_stmt, #Conditional_stmt)

#else  // NDEBUG

#define AIR_DEBUG_FLAG(x)
#define AIR_DEBUG(...)
#define AIR_TRACE(tf, ...)

#define AIR_ASSERT_MSG(Conditional_stmt, ...)    \
  {                                              \
    if (!(Conditional_stmt)) {                   \
      Assert_hdr(std::cerr, __FILE__, __LINE__); \
      Templ_print(std::cerr, __VA_ARGS__);       \
      Abort_location();                          \
    }                                            \
  }
#define AIR_ASSERT(Conditional_stmt) \
  AIR_ASSERT_MSG(Conditional_stmt, #Conditional_stmt)

#endif  // NDEBUG

//! @brief cmplr warn message macro prints value
#define CMPLR_WARN_MSG(tf, ...)                          \
  {                                                      \
    Msg_hdr(tf.Tfile(), __FILE__, __LINE__, SEVL::WARN); \
    Templ_print(tf.Tfile(), __VA_ARGS__);                \
  }  // specific part of trace or message

//! @brief cmplr err message macro prints value
#define CMPLR_ERR_MSG(tf, ...) /* always ERROR to cerr and cout */    \
  {                                                                   \
    Msg_hdr(std::cerr, __FILE__, __LINE__, SEVL::ERR);                \
    Templ_print(std::cerr, __VA_ARGS__); /* specific part of error */ \
    Msg_hdr(tf.Tfile(), __FILE__, __LINE__, SEVL::ERR);               \
    Templ_print(tf.Tfile(), __VA_ARGS__);                             \
  }

//! @brief cmplr assert macro prints value
#define CMPLR_ASSERT(Conditional_stmt, ...)      \
  {                                              \
    if (!(Conditional_stmt)) {                   \
      Assert_hdr(std::cout, __FILE__, __LINE__); \
      Templ_print(std::cout, __VA_ARGS__);       \
      Assert_hdr(std::cerr, __FILE__, __LINE__); \
      Templ_print(std::cerr, __VA_ARGS__);       \
    }                                            \
  }

extern void Usr_msg(std::ostream&, SEVL, U_CODE, ...);

//! @brief cmplr user message macro prints value
//! @param SEVL::ERR The compiler should print the message and return to caller
//!         caller may wish to punt the phase at that point, or try to keep
//!         going.
//! @param SEVL::ERRFATAL The compiler should print the message and abort.
//! @param SEVL::WARN The compiler should print the message and return to
//! caller.
#define CMPLR_USR_MSG(c, ...)                        \
  {                                                  \
    const MSG_DESC msg = User_msg[(int)c];           \
    SEVL           s   = msg._severity;              \
    if ((s == SEVL::ERRFATAL) || (s == SEVL::ERR)) { \
      Usr_msg(std::cerr, s, c, __VA_ARGS__);         \
      if (s == SEVL::ERRFATAL) Abort_location();     \
    }                                                \
    Usr_msg(std::cout, s, c, __VA_ARGS__);           \
  }

#endif  // AIR_UTIL_DEBUG_H
