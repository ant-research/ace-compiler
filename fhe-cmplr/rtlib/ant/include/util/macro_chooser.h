//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_INCLUDE_MACRO_CHOOSER_H
#define RTLIB_INCLUDE_MACRO_CHOOSER_H

#ifndef MACRO_CHOOSER_4
#define API_WITH_ARG4_3(arg1, arg2, arg3) API4_NAME(arg1, arg2, arg3, DEF_ARG4)
#define API_WITH_ARG4_4(arg1, arg2, arg3, arg4) \
  API4_NAME(arg1, arg2, arg3, arg4)
#define GET_4TH_ARG(arg1, arg2, arg3, arg4, API4_NAME, ...) API4_NAME
#define MACRO_CHOOSER_ARG4(...) \
  GET_4TH_ARG(__VA_ARGS__, API_WITH_ARG4_4, API_WITH_ARG4_3)
#endif

#ifndef MACRO_CHOOSER_5
#define API_WITH_ARG5_4(arg1, arg2, arg3, arg4) \
  API5_NAME(arg1, arg2, arg3, arg4, DEF_ARG5)
#define API_WITH_ARG5_5(arg1, arg2, arg3, arg4, arg5) \
  API5_NAME(arg1, arg2, arg3, arg4, arg5)
#define GET_5TH_ARG(arg1, arg2, arg3, arg4, arg5, API5_NAME, ...) API5_NAME
#define MACRO_CHOOSER_ARG5(...) \
  GET_5TH_ARG(__VA_ARGS__, API_WITH_ARG5_5, API_WITH_ARG5_4)
#endif

#endif  // RTLIB_INCLUDE_MACRO_CHOOSER_H