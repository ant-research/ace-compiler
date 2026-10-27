//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_IO_MISC_H
#define AIR_UTIL_IO_MISC_H

#include <stdio.h>
#include <string.h>

#define TRACE_EXT ".t"  // extension for trace file

// we assume FILENAME_MAX is big enough to add a ".xxx" at end and no overflow
class IO_DEV {
private:
  int   _line;  // need to change to srcpos later
  FILE* _fp_src;
  FILE* _fp_trace;
  char  _name_src[FILENAME_MAX];
  char  _name_trace[FILENAME_MAX];

public:
  FILE* In_file() { return _fp_src; }         // input src file ptr
  FILE* Tfile() { return _fp_trace; }         // input src file ptr
  char* Src_name() { return &_name_src[0]; }  // input src file name
  char* Trace_name(void) { return &_name_trace[0]; }
  void  Set_src_name(char* s) {
    if (*s == 0) {
      AIR_ASSERT_MSG(false, "Src file name missing");
      return;
    } else {
      strcpy(&_name_src[0], s);
      _fp_src = NULL;
    }
    return;
  }
  void Set_trace_name(void) {
    char* tfile = Make_aux_file(TRACE_EXT);
    strcpy(&_name_trace[0], tfile);
    return;
  }
  void Line(int l) { _line = l; }
  int  Line() { return _line; }  // always mean src file level

  char* Make_aux_file(const char* ext) {
    char* prefix = strtok(this->Src_name(), ".");
    if (prefix != NULL) {
      strcat(prefix, ext);
    } else {
      // *** CHANGE *** to internal error handling later
      // error, base file name has no extension, ignore for now
      CMPLR_USR_MSG(U_CODE::Src_File_Open_Err, this->Src_name());
    }
    return prefix;
  }
};

#endif  // AIR_UTIL_IO_MISC_H
