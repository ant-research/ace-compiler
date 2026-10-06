//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <stdio.h>
#include <unistd.h>

#include "air/util/binary/tar_reader.h"
#include "gtest/gtest.h"

namespace {

void Test_tar_reader(bool rd[4]) {
  // read file back with TAR_READER
  air::util::TAR_READER tr("ut_tar.tar");
  air::util::TAR_ENTRY  entry;
  bool                  bret = tr.Init();
  EXPECT_TRUE(bret);

  // verify first file content and length
  bret = tr.Next(entry, rd[0]);
  EXPECT_TRUE(bret);
  if (rd[0]) {
    EXPECT_EQ(memcmp(entry.Content(), "abcdefgh\n", 9), 0);
  }
  EXPECT_EQ(entry.Length(), 9);
  EXPECT_EQ(entry.Index(), 0);

  // verify second file content and length
  bret = tr.Next(entry, rd[1]);
  EXPECT_TRUE(bret);
  if (rd[1]) {
    EXPECT_EQ(memcmp(entry.Content(), "abcdefghijkl\n", 13), 0);
  }
  EXPECT_EQ(entry.Length(), 13);
  EXPECT_EQ(entry.Index(), 1);

  // verify third file content and length
  bret = tr.Next(entry, rd[2]);
  EXPECT_TRUE(bret);
  if (rd[2]) {
    EXPECT_EQ(memcmp(entry.Content(), "01234567\n", 9), 0);
  }
  EXPECT_EQ(entry.Length(), 9);
  EXPECT_EQ(entry.Index(), 2);

  // verify fourth file content and length
  bret = tr.Next(entry, rd[3]);
  EXPECT_TRUE(bret);
  if (rd[3]) {
    EXPECT_EQ(memcmp(entry.Content(), "012345678901\n", 13), 0);
  }
  EXPECT_EQ(entry.Length(), 13);
  EXPECT_EQ(entry.Index(), 3);

  // verify no more files available
  bret = tr.Next(entry, true);
  EXPECT_FALSE(bret);

  tr.Fini();
}

TEST(tar_reader, TAR_READER) {
  // save current directory and change to /tmp
  char  cwd[512];
  char* ptr = getcwd(cwd, sizeof(cwd));
  EXPECT_EQ(ptr, cwd);
  int iret = chdir("/tmp");
  EXPECT_EQ(iret, 0);

  // create 4 files and tar them
  iret = system("echo abcdefgh     > ut_tar_f0.txt");
  EXPECT_EQ(iret, 0);
  iret = system("echo abcdefghijkl > ut_tar_f1.txt");
  EXPECT_EQ(iret, 0);
  iret = system("echo 01234567     > ut_tar_f2.txt");
  EXPECT_EQ(iret, 0);
  iret = system("echo 012345678901 > ut_tar_f3.txt");
  EXPECT_EQ(iret, 0);
  iret = system(
      "tar cf ut_tar.tar ut_tar_f0.txt ut_tar_f1.txt ut_tar_f2.txt "
      "ut_tar_f3.txt");
  EXPECT_EQ(iret, 0);

  bool rd[4] = {true, true, true, true};
  Test_tar_reader(rd);

  rd[0] = false, rd[2] = false;
  Test_tar_reader(rd);

  rd[1] = false, rd[3] = false;
  Test_tar_reader(rd);

  rd[2] = true, rd[3] = true;
  Test_tar_reader(rd);

  rd[0] = true, rd[1] = true;
  Test_tar_reader(rd);

  // remove files and restore to cwd
  unlink("ut_tar_f0.txt");
  unlink("ut_tar_f1.txt");
  unlink("ut_tar_f2.txt");
  unlink("ut_tar_f3.txt");
  unlink("ut_tar.tar");
  int result = chdir(cwd);
  if (result == -1) {
    CMPLR_ASSERT(false, "chdir failed");
  }
}

}  // namespace
