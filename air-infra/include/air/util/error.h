//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_ERROR_H
#define AIR_UTIL_ERROR_H

#include <iostream>

// form payload suitable for output to stderr, stdout, trace file, error file...
// does not handle where to output. Only format the message as payload
// A payload may be generated w.r.t.
// 1. src file name if user msg,
//    otherwise, it is internal message from compiler/phase
//    which includes "packaged" error text with line, phase file name
// 2. user input (such as syntax error, src file open, ...)
// 3. a phase in compiler (driver, ONNX2AIR, global optimizer, code gen,...
// 4. user initiated task progress report (
//    e.g. progress report during compilation, header of some messg
//    for internal msg, system support message (such as stack back trace)
// To avoid intermediate buffering, we simply output directly to
// cerr, cout or trace/log files depending on nature of message type

// lambda is not used to avoid unnecessary code bloat
enum class SEVL {
  IGNORE    = 0,
  NOTE      = 1,
  WARN      = 2,
  ERR       = 3,
  ERRSAADV1 = 4,  // static analysis advisary
  ERRSAADV2 = 5,
  ERRSAADV3 = 6,
  ERRSAADV4 = 7,
  ERRPHASE  = 9,  // error but finish phase before exit
  ERRFATAL  = 10,
};

const static char* Sv_to_string(SEVL s) {
  switch (s) {
    case SEVL::IGNORE:
      return "Ignored";
    case SEVL::NOTE:
      return "Note";
    case SEVL::WARN:
      return "Warning";
    case SEVL::ERR:
      return "Error";
    case SEVL::ERRSAADV1:
      return "Vul";
    case SEVL::ERRSAADV2:
      return "Vul";
    case SEVL::ERRSAADV3:
      return "Vul";
    case SEVL::ERRSAADV4:
      return "Pfm";
    case SEVL::ERRPHASE:
      return "Error";
    case SEVL::ERRFATAL:
      return "Fatal error";
    default:
      return "Unknown severity level: Internal error";
  }
};

// Return code
enum class R_CODE {
  NORMAL        = 0,
  INTERNAL      = 1,
  USER          = 2,
  NORECOVER     = 3,
  UNIMPLEMENTED = 4,
};

#endif  // AIR_UTIL_ERROR_H
