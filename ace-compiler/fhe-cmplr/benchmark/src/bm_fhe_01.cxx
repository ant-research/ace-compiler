//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#define PROFILE

#include "benchmark/benchmark.h"

// TODO: replace later
void Bm_string_copy(benchmark::State& state) {
  std::string x = "hello";
  for (auto _ : state) std::string copy(x);
}

BENCHMARK(Bm_string_copy);

BENCHMARK_MAIN();