//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>

#include "air/util/debug.h"
#include "air/util/messg.h"

static const char* Cmplr_file = "xxx.yyy";
static int         Cmplr_line = 0;

extern FILE*       Input_file;
extern std::string Input_file_name;  // input file to the compiler

#include "err_msg.inc.c"

typedef void (*sighandler_t)(int);

void Usr_msg(std::ostream& out, SEVL s, U_CODE c, ...) {
  va_list vp;

  out << Sv_to_string(s) << ": ";

  va_start(vp, c);

  int            i   = (int)c;
  const MSG_DESC msg = User_msg[i];
  //  for (int j = 0; j < MAX_PARMS; j++) {
  char buf[256];
  if (msg._err_code != c) {
    printf("ERROR: \n");
  } else {
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
      } else {
        out << *fmt;
      }
    }
    out << std::endl << std::flush;
  }
  va_end(vp);
}

static void Cleanup_signal(int signal) {
  // TODO:: need to handle child/paraent processes,
  // close file descriptors (flush first) and resources
  // need to access all "file" related resources then unlink them
  // before exit(1);

  exit(1);
}

static void Catch_signal(int signum, int err_num) {
  signal(signum, SIG_DFL);

  switch (signum) {
    case SIGINT:
    case SIGTERM:
      //    CMPLR_ERR_MSG("Termination/interrupt signal received");  // should
      //    not exit
      Cleanup_signal(signum);  // close files, etc
      // fall through
    case SIGHUP:
      kill(getpid(), signum);
      exit((int)R_CODE::INTERNAL);

    case SIGSEGV:
      if (err_num == ENXIO || err_num == ENOSPC) {
        // I/O, file system or mmapped object error
        //      CMPLR_ERR_MSG("I/O error %s", strerror(err_num));
      } else {
        //      CMPLR_ERR_MSG("Segmentation fault %s", strerror(err_num));
      }
      exit((int)R_CODE::NORECOVER);

    case SIGALRM:
      exit((int)R_CODE::NORMAL);

    default:
        //    CMPLR_ERR_MSG("Unhandled signal caught %s", strerror(err_num));
        ;
  }
}

static void Setup_signal_handler(int s) {
  if (signal(s, SIG_IGN) != SIG_IGN)
    signal(s, reinterpret_cast<void (*)(int)>(Catch_signal));
}

void Handle_signals(void) {
  Setup_signal_handler(SIGINT);
  Setup_signal_handler(SIGILL);
  Setup_signal_handler(SIGABRT);
  Setup_signal_handler(SIGKILL);
  Setup_signal_handler(SIGSEGV);
  Setup_signal_handler(SIGTERM);
  Setup_signal_handler(SIGQUIT);
  Setup_signal_handler(SIGHUP);
  Setup_signal_handler(SIGALRM);
}

void Abort_location() { exit(1); }
void Debug_location() {}
