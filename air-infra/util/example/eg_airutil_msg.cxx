//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include <fstream>
#include <iostream>

#include "air/util/debug.h"

// IMPORTANT: re-define Abort_location() ONLY for testing purpose
#define Abort_location()                       \
  do {                                         \
    std::cout << "Abort_location() called.\n"; \
  } while (0)

#include "air/util/io_misc.h"

using namespace air::util;

extern void Handle_signals(void);
extern void Catch_signal(int, int);

char* Src_file_name = (char*)"Simulated_Src_file_name.c";
int   Src_line_num  = 789;  // simulated src file line number;

void Test_signal() {
  // trigger a SIGALRM to break the infinite loop below.
  // Catch_signal() will exit with RC = 0, main() returns with RC = 4
  alarm(1);
  while (1)
    ;
}

#if 0
void Usr_msg(std::ostream& out, SEVL s, U_CODE c, ...)
{
  va_list  vp;

  out << Sv_to_string(s) <<": ";
  
  va_start(vp, c);
  
  int i = (int)c;
  const MSG_DESC msg = User_msg[i];
  //  for (int j = 0; j < MAX_PARMS; j++) {
    char buf[256];
    if (msg._err_code != c) {
      printf("ERROR: \n");
    }
    else {
      for (const char* fmt = msg._fmt; *fmt != '\0'; fmt++) {
	if (*fmt == '%') {
	  // TODO:: need to handle %llx etc
	  switch (*++fmt) {
	  case 'd':
	    out << va_arg(vp, int);
	    break;
	  case 's':
	    out << va_arg(vp, const char*);
	    break;
	  default:
	    out << "Internal error: unsupported";
	    break;
	  }
	}
	else {
	  out << *fmt;
	}
      }
      out << std::endl << std::flush;
    }
  va_end(vp);
}

#else
// extern void Usr_msg(std::ostream&, SEVL, U_CODE, ...);

#endif

// need to change line to srcpos at final implementation

int main() {
  float        f       = 0.1234;
  unsigned int hex_val = 0xdeadbeef;
  int          locali  = 0;
  int          localj  = -1;
  Templ_print(std::cout, "foo format: %s %d", "bar", 1234);
  Templ_print(std::cout, "foo format: %s %x", "bar", 1234);
  Templ_print(std::cout, "foo format: %s 0x%x, in float, %f ", "bar", 1234,
              0.1234);
  Templ_print(std::cout, "foo format: %s 0x%llx, in float, %f, char %c", "bar",
              hex_val, f, 'b');

  Handle_signals();
  IO_DEV test_file;
  test_file.Set_src_name(
      (char*)"testing.cxx");  // simulate input file name from main
  test_file.Set_trace_name();
  printf("Trace file name: %s\n", test_file.Trace_name());

  std::cout << "+++++++++++++" << std::endl;
  char* in_file_name =
      (char*)"filename_no_ext";  // simulate input file name error
  IO_DEV input_file;
  input_file.Set_src_name(in_file_name);
  input_file.Set_trace_name();

  std::cout << "Start of true API usage basic tests >>>\n" << std::endl;

  std::cout << "+++++++++++++" << std::endl;
  char* no_file_name =
      (char*)"";  // simulate input file name error (no src file name)
  input_file.Set_src_name(no_file_name);
  //  input_file.Set_trace_name();

  int line124 = 124;
  CMPLR_USR_MSG(U_CODE::Uninit, "variableX", 123, 45);
  CMPLR_USR_MSG(U_CODE::Uninit, "variableY", line124, 55);
  CMPLR_USR_MSG(U_CODE::GOT_Size_Conflict, 1234567);

  TFILE trace("usr_msg.t");

  CMPLR_ERR_MSG(
      trace,
      "This is cmplr testing ERROR payload %s 0x%llx, in float, %f, char %c",
      "bar", hex_val, f, 'b');

  CMPLR_WARN_MSG(
      trace, "This is cmplr testing payload %s 0x%llx, in float, %f, char %c",
      "bar", hex_val, f, 'b');

  AIR_TRACE(trace,
            "Tracing: for testing, we have set trace file to cout, the actual "
            "trace testing is %x %d %lx\n",
            1234, 1234, 1234);
  AIR_ASSERT_MSG(((localj == 0)), "%s should be %d", "assert part", 999);
  AIR_TRACE(trace, "Second Tracing\n");
  Test_signal();
  return (int)R_CODE::UNIMPLEMENTED;
}
