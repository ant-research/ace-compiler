//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/util/debug.h"

// IMPORTANT: re-define Abort_location() ONLY for testing purpose
#define Abort_location()                       \
  do {                                         \
    std::cout << "Abort_location() called.\n"; \
  } while (0)

using namespace air::util;

/**
 * @brief test macro CMPLR_USR_MSG U_CODE::Uninit
 *
 */
static void Uninit(void) {
  //  CMPLR_USR_MSG(tfile, "This is CMPLR_USR_MSG testing .... \n");
  int line124 = 124;
  CMPLR_USR_MSG(U_CODE::Uninit, "variableX", 123, 45);
  CMPLR_USR_MSG(U_CODE::Uninit, "variableY", line124, 55);
}

/**
 * @brief test macro CMPLR_USR_MSG U_CODE::Src_File_Open_Err
 *
 */
static void Src_file_open_err(const std::string file) {
  CMPLR_USR_MSG(U_CODE::Src_File_Open_Err, file.c_str());
}

/**
 * @brief test macro CMPLR_USR_MSG U_CODE::Cmplr_Assert
 *
 */
static void Cmplr_assert(void) {
  CMPLR_USR_MSG(U_CODE::Cmplr_Assert, "Cmplr_assert", 31, 32);
}

/**
 * @brief test macro CMPLR_USR_MSG U_CODE::GOT_Size_Conflict
 *
 */
static void Got_size_conflict(void) {
  CMPLR_USR_MSG(U_CODE::GOT_Size_Conflict, 1234567);
}

/**
 * @brief test macro CMPLR_USR_MSG U_CODE::Elf_Ofst64_Error
 *
 */
static void Elf_ofst64_error(void) {
  CMPLR_USR_MSG(U_CODE::Elf_Ofst64_Error, "offset : % lx", 0xabcd);
}

/**
 * @brief test macro CMPLR_USR_MSG U_CODE::Srcfile_Not_Found
 *
 */
static void Srcfile_not_found(const std::string file) {
  CMPLR_USR_MSG(U_CODE::Srcfile_Not_Found, file.c_str());
}

int main() {
  // const char *file = "cmplr_usr_msg.t";
  std::string path = __FILE__;
  std::string file = path.substr(path.find_last_of("/\\") + 1);
  file             = file.substr(0, file.find_last_of(".")) + ".t";

  Src_file_open_err(file);

  Cmplr_assert();

  Uninit();

  Got_size_conflict();

  Elf_ofst64_error();

  Srcfile_not_found(file);

  return 0;
}
