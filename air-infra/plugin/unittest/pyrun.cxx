//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/plugin/pyrun.h"

#include <limits.h>
#include <unistd.h>

#include <filesystem>
#include <stdexcept>
#include <string>

#include "gtest/gtest.h"

using namespace air::plugin;

namespace {

std::string Get_path(std::string file_name) {
  std::filesystem::path path = std::filesystem::current_path();

  char    buffer[PATH_MAX];
  ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
  if (len != -1) {
    buffer[len] = '\0';
    std::string exe_path(buffer);
    path = exe_path.substr(0, exe_path.find_last_of("/"));
  } else {
    throw std::runtime_error("Error occurred!");
  }

  return path / file_name;
}

// Executing Python script
TEST(RunPython, StrBase) {
  CONST_BYTE_PTR pystr = "print(f'Hello from Python code')\n";

  PYRUN* exec = new PYRUN();
  exec->Run(pystr);
  delete exec;
}

// Executing Python file
TEST(RunPython, FileBase) {
  std::string path;

  try {
    path = Get_path("pybase.py");
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
  std::cout << "Path: " << path << std::endl;

  PYRUN* exec = new PYRUN();
  exec->Run(path);
  delete exec;
}

}  // namespace
