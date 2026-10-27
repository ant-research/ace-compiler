//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================
#include "bm_rtlib_poly.h"

#include "benchmark/benchmark.h"
#include "common/rt_config.h"

using namespace benchmark;

#define REGISTER_BENCHMARK(name) \
  RegisterBenchmark(#name, name)->Apply({Arguments})

static void Arguments(internal::Benchmark* b) {
  int degree = static_cast<int>(BM_GLOB_ENV::Get_env(TEST_DEGREE));
  b->Arg(degree)->Unit(::kMicrosecond);
}

static void Ntt(State& state) {
  size_t      degree = state.range(0);
  size_t      num_q  = Get_q_cnt();
  POLYNOMIAL* input =
      Alloc_rnd_poly(degree, num_q, false /*extend_p*/, false /*is_ntt*/);
  POLYNOMIAL* res = Alloc_zero_poly(degree, num_q, 0);

  for (auto _ : state) {
    Conv_poly2ntt(res, input);
  }
  Free_poly(res);
  Free_poly(input);
}

static void Intt(State& state) {
  size_t      degree = state.range(0);
  size_t      num_q  = Get_q_cnt();
  POLYNOMIAL* input  = Alloc_rnd_poly(degree, num_q);
  POLYNOMIAL* res    = Alloc_zero_poly(degree, num_q, 0);

  for (auto _ : state) {
    Conv_ntt2poly(res, input);
  }
  Free_poly(input);
  Free_poly(res);
}

static void Ntt_inplace(State& state) {
  size_t      degree = state.range(0);
  POLYNOMIAL* input =
      Alloc_rnd_poly(degree, Get_q_cnt(), false /*extend_p*/, false /*is_ntt*/);

  for (auto _ : state) {
    state.PauseTiming();
    Set_is_ntt(input, false);
    state.ResumeTiming();
    Conv_poly2ntt_inplace(input);
  }
  Free_poly(input);
}

static void Intt_inplace(State& state) {
  size_t      degree = state.range(0);
  POLYNOMIAL* input  = Alloc_rnd_poly(degree, Get_q_cnt());

  for (auto _ : state) {
    state.PauseTiming();
    Set_is_ntt(input, true);
    state.ResumeTiming();
    Conv_ntt2poly_inplace(input);
  }
  Free_poly(input);
}

static void Precomp_nofusion(State& state) {
  size_t      degree = state.range(0);
  POLYNOMIAL* poly   = Alloc_rnd_poly(degree, Get_q_cnt());

  // do not fusion decomp & mod_up
  Set_rtlib_config(CONF_OP_FUSION_DECOMP_MODUP, 0);

  for (auto _ : state) {
    VALUE_LIST* precomputed = Alloc_precomp(poly);
    state.PauseTiming();
    Free_precomp(precomputed);
    state.ResumeTiming();
  }
  Free_poly(poly);
}

static void Precomp_fusion(State& state) {
  size_t      degree = state.range(0);
  POLYNOMIAL* poly   = Alloc_rnd_poly(degree, Get_q_cnt());

  // fusion decomp & mod_up
  Set_rtlib_config(CONF_OP_FUSION_DECOMP_MODUP, 1);

  for (auto _ : state) {
    VALUE_LIST* precomputed = Alloc_precomp(poly);
    state.PauseTiming();
    Free_precomp(precomputed);
    state.ResumeTiming();
  }
  Free_poly(poly);
}

static void Decomp_modup_nofusion(State& state) {
  size_t degree     = state.range(0);
  size_t num_q      = Get_q_cnt();
  size_t q_part_idx = 0;

  POLYNOMIAL* input  = Alloc_rnd_poly(degree, num_q);
  POLYNOMIAL* decomp = Alloc_zero_poly(degree, Decomp_len(q_part_idx), 0);
  POLYNOMIAL* raise  = Alloc_zero_poly(degree, num_q, Get_p_cnt());

  // mod up decomp poly at q_part_idx
  for (auto _ : state) {
    Decomp(decomp, input, q_part_idx);
    Mod_up(raise, decomp, q_part_idx);
  }

  Free_poly(input);
  Free_poly(decomp);
  Free_poly(raise);
}

static void Decomp_modup_fusion(State& state) {
  size_t degree     = state.range(0);
  size_t num_q      = Get_q_cnt();
  size_t q_part_idx = 0;

  POLYNOMIAL* input = Alloc_rnd_poly(degree, num_q);
  POLYNOMIAL* raise = Alloc_zero_poly(degree, num_q, Get_p_cnt());

  // decompose and mod up with parts
  for (auto _ : state) {
    Decomp_modup(raise, input, q_part_idx);
  }

  Free_poly(input);
  Free_poly(raise);
}

static void Mod_up_with_parts(State& state) {
  size_t degree = state.range(0);
  // allocate input poly which is decomp poly of q_part_idx
  size_t      q_part_idx = 0;
  POLYNOMIAL* decomp =
      Alloc_rnd_poly(degree, Decomp_len(q_part_idx), true /*extend_p*/);
  POLYNOMIAL* raise = Alloc_zero_poly(degree, Get_q_cnt(), Get_p_cnt());

  // mod up decomp poly at q_part_idx
  for (auto _ : state) {
    Mod_up(raise, decomp, q_part_idx);
  }

  Free_poly(decomp);
  Free_poly(raise);
}

static void Mod_down(State& state) {
  size_t degree = state.range(0);
  size_t num_q  = Get_q_cnt();

  POLYNOMIAL* input  = Alloc_rnd_poly(degree, num_q, true /*extend_p*/);
  POLYNOMIAL* reduce = Alloc_zero_poly(degree, num_q, 0);

  for (auto _ : state) {
    Mod_down(reduce, input);
  }
  Free_poly(input);
  Free_poly(reduce);
}

static void Base_conv_with_parts(State& state) {
  size_t      degree     = state.range(0);
  size_t      q_part_idx = 0;
  POLYNOMIAL* input =
      Alloc_rnd_poly(degree, Decomp_len(q_part_idx), true /*extend_p*/);
  CRT_CONTEXT* crt   = Get_crt_context();
  size_t       level = Poly_level(input) - 1;
  VL_CRTPRIME* primes =
      Get_qpart_compl_at(Get_qpart_compl(crt), level, q_part_idx);
  POLYNOMIAL* res = Alloc_zero_poly(degree, LIST_LEN(primes), 0);

  for (auto _ : state) {
    Fast_base_conv_with_parts(res, input, Get_qpart(crt), primes, q_part_idx,
                              level);
  }
  Free_poly(input);
  Free_poly(res);
}

static void Base_conv(State& state) {
  size_t       degree = state.range(0);
  CRT_CONTEXT* crt    = Get_crt_context();
  POLYNOMIAL*  input  = Alloc_rnd_poly(degree, Get_p_cnt(), true /*extend_p*/);
  POLYNOMIAL*  res    = Alloc_zero_poly(degree, Get_q_cnt(), 0);

  // base convert from p to q
  for (auto _ : state) {
    Fast_base_conv(res, input, Get_q(crt), Get_p(crt));
  }
  Free_poly(input);
  Free_poly(res);
}

ENV_INFO BM_GLOB_ENV::Env_info[ENV_LAST];

int main(int argc, char** argv) {
  BM_GLOB_ENV::Initialize();
  BM_GLOB_ENV::Print_env();

  Initialize(&argc, argv);

  // alloc crt
  CRT_CONTEXT* crt = Alloc_crt();

  // register all benchmark
  REGISTER_BENCHMARK(Ntt);
  REGISTER_BENCHMARK(Intt);
  REGISTER_BENCHMARK(Ntt_inplace);
  REGISTER_BENCHMARK(Intt_inplace);
  REGISTER_BENCHMARK(Precomp_nofusion);
  REGISTER_BENCHMARK(Precomp_fusion);
  REGISTER_BENCHMARK(Decomp_modup_nofusion);
  REGISTER_BENCHMARK(Decomp_modup_fusion);
  REGISTER_BENCHMARK(Mod_up_with_parts);
  REGISTER_BENCHMARK(Mod_down);
  REGISTER_BENCHMARK(Base_conv_with_parts);
  REGISTER_BENCHMARK(Base_conv);

  RunSpecifiedBenchmarks();

  // free crt after all benchmarks have run
  Free_crt();

  return 0;
}